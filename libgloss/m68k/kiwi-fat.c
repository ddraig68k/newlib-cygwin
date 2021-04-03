/* idp-outbyte.c
 * Copyright (c) 1995 Cygnus Support
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include <_ansi.h>
#include <v9990.h>
#include "fat_filelib.h"

#define	IDE_BASE	0x003dfd00
#define	IDE_DATA	0x00
#define	IDE_SEC_COUNT	0x04
#define	IDE_LBA_LOW	0x06
#define	IDE_LBA_MID	0x08
#define	IDE_LBA_HIGH	0x0a
#define	IDE_DRV_HEAD	0x0c
#define	IDE_COMMAND	0x0e
#define IDE_CONTROL     0x10c
#define READ_IDE(x)	(*((volatile char *) IDE_BASE + (x) ))
#define WRITE_IDE(x, y)	(*((char *) IDE_BASE + (x) ) = (y) ); (*((char *) IDE_BASE + (x) + 1 ) = 0x20 )
#define WRITE_IDE_DATA(x, y)	(*((char *) IDE_BASE + (x) ) = (y) )
/* 0x20 because 0x00 gets optimized to clrb which reads before clearing. Any value that results in a simple write should be sufficient. */

#define PIT2_BASE	0x003dfb00
#define	PGCR	0	// Port General Control Register
#define PBDDR	6	// Port B DDR
#define	PACR	12	// Port B Control Register
#define	PBCR	14	// Port B Control Register
#define	PBDR	18	// Port B Data Register
#define READ_PIT2(x)	(*((volatile char *) PIT2_BASE + (x) ))
#define WRITE_PIT2(x, y)	(*((char *) PIT2_BASE + (x) ) = (y) )

#define	FDC_BASE	0x003df700
#define READ_FDC(x)	(*((volatile char *) FDC_BASE + (x) ))

unsigned char slave=0; // 0=master, 1=slave

inline static void wait_retrace() {
  while ((READ_VDP(0x0a) & 64) != 0x00);
  while ((READ_VDP(0x0a) & 64) == 0x00);
}

init_pit() {
	WRITE_PIT2(PGCR,50);	// Mode0, H34 & H12 Enable, H1-4 Sense for printer
	WRITE_PIT2(PACR,112);	// Submode 1, H2 interlocked input handshake protocol
	WRITE_PIT2(PBCR,32);	// Submode 0, H4 negated (Printer !reset)
	WRITE_PIT2(PBDDR,124);	// Bit 7,0,1=input, Bit 6-2=output
}

int media_init()
{
	init_pit();
	WRITE_PIT2(PBDR,8);	// Deselect Drive
	WRITE_IDE(IDE_CONTROL, 0x0A); /* Keine IDE Interrupts */
	return 1;
}

uint8_t seek_track (unsigned char track) {
int i=0,j=0;
	//printf("Seek to Track %u\n", track);
	do {
		asm volatile ("move.b   %0, 0x3df706"::"m" (track));	// Datenregister mit Track fuellen
		asm volatile ("move.b   #16, 0x3df700");	// Seek Befehl
		//asm volatile ("move.b   #20, 0x3df700");	// Seek Befehl
		wait_retrace();
		wait_retrace();
		wait_retrace();
		wait_retrace();
		//for(i=0;i<1500;i++) {
		//	printf("SEEK!\n");
		//	while((READ_FDC(0) & 0x01) == 0x01);
		//	}
		//if((READ_FDC(0) & 0x01) == 0x01)
		//  return 0;
		if((READ_FDC(0) & 0x16) == 0x16) {
			//printf("ERROR: Track or sector not found.\n");
			return 0;
			break;
		}
	} while ((READ_FDC(2) != track) && j++<40);	
	if((READ_FDC(2)) != track) {
	  //printf("Track not found. :-(\n");
	  return 0;
	} else
	return 1;
}

uint8_t read_sector(unsigned char track, unsigned char sector, unsigned char side, char *buffer) {
int i=0,j=0;

	if (side == 0)
		WRITE_PIT2(PBDR,112);	// Select Drive A, Motor on, Side 0
	else 
		WRITE_PIT2(PBDR,48);	// Select Drive A, Motor on, Side 1
	if(!seek_track(track))
	  return 0;

	asm volatile ("move.b   %0, 0x3df704"::"m" (sector));	// Set Sector to read
	asm volatile ("move.b   #134, 0x3df700");	// Read Sector Befehl
	//asm volatile ("move.b   #150, 0x3df700");	// Read multiple Sector Befehl
		//printf("Status=%u\n",READ_FDC(0));
		//printf("Status=%u\n",READ_FDC(0));
	do {
	  wait_retrace();
	  wait_retrace();
	  //printf("Wait %d\n",(uint8_t)READ_FDC(0));
	} while((READ_FDC(0) & 0x01) == 0x00);
	if( ((READ_FDC(0) & 0x16) == 0x16)) {
	  //printf("Sektor nicht gefunden!");
	  return 0;
	  }
	  j=0;
	while(((READ_FDC(0) & 0x01) == 0x01) && (j<50000)) {
		if ((READ_FDC(0) & 0x02) == 0x02) {
			*buffer++ = READ_FDC(6);
		j=0;
		} else j++;
	}
	//printf("j=%d\n",j);
	if(j>49999) {
	asm volatile ("move.b   #208, 0x3df700");	// Force Interrupt Befehl
	return 0;
	}
	return 1;
	//printf("Status=%d\n",READ_FDC(0));
}


void write_sector(unsigned char track, unsigned char sector, unsigned char side, char *buffer) {
int i=0,j=0;

	if (side == 0)
		WRITE_PIT2(PBDR,112);	// Select Drive A, Motor on, Side 0
	else 
		WRITE_PIT2(PBDR,48);	// Select Drive A, Motor on, Side 1
	seek_track(track);
	asm volatile ("move.b   %0, 0x3df704"::"m" (sector));	// Set Sector to read
	asm volatile ("move.b   #166, 0x3df700");	// Write Sector Befehl
	//asm volatile ("move.b   #150, 0x3df700");	// Read multiple Sector Befehl
		//printf("Status=%u\n",READ_FDC(0));
		//printf("Status=%u\n",READ_FDC(0));
	while((READ_FDC(0) & 0x01) == 0x00);
	while((READ_FDC(0) & 0x01) == 0x01) {
		if ((READ_FDC(0) & 0x02) == 0x02) {
			//*buffer++ = READ_FDC(6);
			asm volatile ("move.b   %0, 0x3df706"::"m" (*buffer++));	// Puffer schreiben
		}
	}
	//printf("Status=%d\n",READ_FDC(0));
}

/* setting LBA address of given sector into IDE registers for MASTER */
void set_LBA_address( int sector ) {
	unsigned char LBA_addr[4];
	unsigned char mode;
	/* set LBA mode and master/slave */
	mode = 0b11100000 | (slave << 4);
	LBA_addr[0] = (unsigned char)sector;
	LBA_addr[1] = (unsigned char)(sector >> 8);
	LBA_addr[2] = (unsigned char)(sector >> 16);
	LBA_addr[3] = (unsigned char)(sector >> 24);
	WRITE_IDE(IDE_LBA_LOW, LBA_addr[0]);
	WRITE_IDE(IDE_LBA_MID, LBA_addr[1]);
	WRITE_IDE(IDE_LBA_HIGH, LBA_addr[2]);
	WRITE_IDE(IDE_DRV_HEAD, (LBA_addr[3] & 0b00001111) | mode);
}


/* wait for IDE become ready to transfer data. CHANGE THIS => potential dead lock */
int wait_busy() {
//	while ((READ_IDE(IDE_COMMAND) ) != 88);
	int tries=45000;
	while ((READ_IDE(IDE_COMMAND) & 0x80) && tries--);
	//while ((READ_IDE(IDE_COMMAND) & 0x80));
	if(tries) return 1;
	  else return 0;
}

int wait_ready() {
//	while ((READ_IDE(IDE_COMMAND) ) != 88);
	int tries=45000;
	while (((READ_IDE(IDE_COMMAND) & 0x20) == 0) && tries--);
	//while ((READ_IDE(IDE_COMMAND) & 0x80));
	if(tries) return 1;
	  else return 0;
}

int media_read_fdc(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
int i=0;
unsigned char track, sec, side;
	for(i=0;i<sector_count;i++) {
		track=(unsigned char)(sector/18);	//2*9(=18)Sektoren/Track
		side=(unsigned char)((sector/9)%2);	//9Sektoren/Seite
		sec=(unsigned char)((sector%9)+1);
		//printf("t=%u,sec=%u,side=%u\n",track,sec,side);
		if(!read_sector(track,sec,side,buffer)) {
		  WRITE_PIT2(PBDR,8);	// Deselect Drive
		  return 0;
		}
		buffer+=512;
		sector++;
	}
	WRITE_PIT2(PBDR,8);	// Deselect Drive
	return 1;
}

int media_write_fdc(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
int i=0;
unsigned char track, sec, side;
	for(i=0;i<sector_count;i++) {
		track=(unsigned char)(sector/18);	//2*9(=18)Sektoren/Track
		side=(unsigned char)((sector/9)%2);	//9Sektoren/Seite
		sec=(unsigned char)((sector%9)+1);
		//printf("t=%u,sec=%u,side=%u\n",track,sec,side);
		write_sector(track,sec,side,buffer);
		buffer+=512;
		sector++;
	}
	WRITE_PIT2(PBDR,8);	// Deselect Drive
	return 1;
}
int media_read(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
	//int count=1;
	int i;
	unsigned char state;
	wait_busy();
	//wait_ready();
	set_LBA_address(sector);
	WRITE_IDE(IDE_SEC_COUNT,sector_count); /* read 1 sector */
	WRITE_IDE(IDE_COMMAND, 0x20); /* read sector */
	for(i=0;i<4;i++)
	  wait_retrace();
	wait_busy();
	state=READ_IDE(IDE_COMMAND);
	if (state & 0x01) return 0;
	if (state & 0x20) return 0;
	if ((state & 0x08) == 0) return 0;
	//if(wait_busy()==0) return 0;
	//if ((READ_IDE(IDE_COMMAND) & 0x01) == 0x01) { return 0; }
	while ((READ_IDE(IDE_COMMAND) & 0x08) == 0x08) {
		*buffer++=READ_IDE(IDE_DATA);
		*buffer++=READ_IDE(IDE_DATA+1);
	wait_busy();
	}
	return 1;
}


int media_write(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
	//int count=1;
	int i;
	wait_busy();
	set_LBA_address(sector);
	WRITE_IDE(IDE_SEC_COUNT,sector_count); /* write 1 sector */
	WRITE_IDE(IDE_COMMAND, 0x30); /* write sector */
	for(i=0;i<4;i++)
	  wait_retrace();
	wait_busy();
	while ((READ_IDE(IDE_COMMAND) & 0x08) == 0x08) {
		WRITE_IDE_DATA(IDE_DATA,*buffer++);
		WRITE_IDE_DATA(IDE_DATA+1,*buffer++);
		wait_busy();
	}
	return 1;
}

void media_start () {

	wait_ready();
	media_init();

	wait_ready();

	// Initialise File IO Library
	fl_init();
	slave = 0;

	// Attach media access functions to library
	if (fl_attach_media(media_read, media_write) != FAT_INIT_OK)
	{
		iprintf("ERROR: Media attach failed\n");
		return; 
	}
}

