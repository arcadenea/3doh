/*
  www.freedo.org
The first and only working 3DO multiplayer emulator.

The FreeDO licensed under modified GNU LGPL, with following notes:

*   The owners and original authors of the FreeDO have full right to develop closed source derivative work.
*   Any non-commercial uses of the FreeDO sources or any knowledge obtained by studying or reverse engineering
    of the sources, or any other material published by FreeDO have to be accompanied with full credits.
*   Any commercial uses of FreeDO sources or any knowledge obtained by studying or reverse engineering of the sources,
    or any other material published by FreeDO is strictly forbidden without owners approval.

The above notes are taking precedence over GNU LGPL in conflicting situations.

Project authors:

Alexander Troosh
Maxim Grishin
Allen Wright
John Sammons
Felix Lazarev
*/

// Iso.cpp: implementation of the CIso class.
//
//////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "freedoconfig.h"
#ifndef DREAMCAST
#include <memory.h>
#else
#include <string.h>
#endif
#include "IsoXBUS.h"
#include "types.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define STATDELAY 100
#define REQSIZE	2048
typedef enum MEI_CDROM_Error_Codes {
  MEI_CDROM_no_error = 0x00,
  MEI_CDROM_recv_retry = 0x01,
  MEI_CDROM_recv_ecc = 0x02,
  MEI_CDROM_not_ready = 0x03,
  MEI_CDROM_toc_error = 0x04,
  MEI_CDROM_unrecv_error = 0x05,
  MEI_CDROM_seek_error = 0x06,
  MEI_CDROM_track_error = 0x07,
  MEI_CDROM_ram_error = 0x08,
  MEI_CDROM_diag_error = 0x09,
  MEI_CDROM_focus_error = 0x0A,
  MEI_CDROM_clv_error = 0x0B,
  MEI_CDROM_data_error = 0x0C,
  MEI_CDROM_address_error = 0x0D,
  MEI_CDROM_cdb_error = 0x0E,
  MEI_CDROM_end_address = 0x0F,
  MEI_CDROM_mode_error = 0x10,
  MEI_CDROM_media_changed = 0x11,
  MEI_CDROM_hard_reset = 0x12,
  MEI_CDROM_rom_error = 0x13,
  MEI_CDROM_cmd_error = 0x14,
  MEI_CDROM_disc_out = 0x15,
  MEI_CDROM_hardware_error = 0x16,
  MEI_CDROM_illegal_request = 0x17
}MEI_CDROM_Error_Codes;


#define POLSTMASK	0x01
#define POLDTMASK	0x02
#define POLMAMASK	0x04
#define POLREMASK	0x08
#define POLST		0x10
#define POLDT		0x20
#define POLMA		0x40
#define POLRE		0x80

#define CDST_TRAY  0x80
#define CDST_DISC  0x40
#define CDST_SPIN  0x20
#define CDST_ERRO  0x10
#define CDST_2X    0x02
#define CDST_RDY   0x01
#define CDST_TRDISC 0xC0
#define CDST_OK    CDST_RDY|CDST_TRAY|CDST_DISC|CDST_SPIN

//medium specific
#define CD_CTL_PREEMPHASIS      0x01
#define CD_CTL_COPY_PERMITTED   0x02
#define CD_CTL_DATA_TRACK       0x04
#define CD_CTL_FOUR_CHANNEL     0x08
#define CD_CTL_QMASK            0xF0
#define CD_CTL_Q_NONE           0x00
#define CD_CTL_Q_POSITION       0x10
#define CD_CTL_Q_MEDIACATALOG   0x20
#define CD_CTL_Q_ISRC           0x30

#define MEI_DISC_DA_OR_CDROM    0x00
#define MEI_DISC_CDI            0x10
#define MEI_DISC_CDROM_XA       0x20

#define CDROM_M1_D              2048
#define CDROM_DA                2352
#define CDROM_DA_PLUS_ERR       2353
#define CDROM_DA_PLUS_SUBCODE   2448
#define CDROM_DA_PLUS_BOTH      2449

//medium specific
//drive specific
#define MEI_CDROM_SINGLE_SPEED  0x00
#define MEI_CDROM_DOUBLE_SPEED  0x80

#define MEI_CDROM_DEFAULT_RECOVERY         0x00
#define MEI_CDROM_CIRC_RETRIES_ONLY        0x01
#define MEI_CDROM_BEST_ATTEMPT_RECOVERY    0x20

#define Address_Blocks    0
#define Address_Abs_MSF   1
#define Address_Track_MSF 2

#pragma pack(push,1)
//drive specific
//disc data
typedef struct TOCEntry{
unsigned char res0;
unsigned char CDCTL;
unsigned char TRKNUM;
unsigned char res1;
unsigned char mm;
unsigned char ss;
unsigned char ff;
unsigned char res2;
}TOCEntry;


//disc data
typedef struct DISCStc{
unsigned char curabsmsf[3]; //BIN form
unsigned char curtrack;
unsigned char nextmsf[3]; //BIN form
unsigned char tempmsf[3]; //BIN form
int  tempblk;
int  templba;
unsigned char currenterror;
unsigned char currentxbus;
unsigned int  currentoffset;
unsigned int  currentblocksize;
unsigned char currentspeed;
unsigned char  totalmsf[3];//BIN form
unsigned char  firsttrk;
unsigned char  lasttrk;
unsigned char  discid;
unsigned char  sesmsf[3]; //BIN form
TOCEntry DiscTOC[100];
}DISCStc;



typedef struct cdrom_Device
{
	unsigned char Poll;
	unsigned char XbusStatus;
	unsigned char StatusLen;
	int  DataLen;
	int  DataPtr;
	unsigned int olddataptr;
	unsigned char CmdPtr;
	unsigned char Status[256];
	unsigned char Data[REQSIZE];
	unsigned char Command[7];
	char STATCYC;
	int Requested;
	MEI_CDROM_Error_Codes MEIStatus;
	DISCStc DISC;
    unsigned int curr_sector;
}cdrom_Device;

void cdrom_DeviceInit(cdrom_Device *cdrom);

unsigned int cdrom_DeviceGetStatusFifo(cdrom_Device *cdrom);
void  cdrom_DeviceSendCommand(cdrom_Device *cdrom,unsigned char val);
unsigned int cdrom_DeviceGetPoll(cdrom_Device *cdrom);
int cdrom_DeviceTestFIQ(cdrom_Device *cdrom);
void cdrom_DeviceSetPoll(cdrom_Device *cdrom,unsigned int val);
unsigned int cdrom_DeviceGetDataFifo(cdrom_Device *cdrom);
void cdrom_DeviceDoCommand(cdrom_Device *cdrom);
unsigned char cdrom_DeviceBCD2BIN(cdrom_Device *cdrom,unsigned char in);
unsigned char cdrom_DeviceBIN2BCD(cdrom_Device *cdrom,unsigned char in);
void cdrom_DeviceMSF2BLK(cdrom_Device *cdrom);
void cdrom_DeviceBLK2MSF(cdrom_Device *cdrom);
void cdrom_DeviceLBA2MSF(cdrom_Device *cdrom);
void cdrom_DeviceMSF2LBA(cdrom_Device *cdrom);
unsigned char * cdrom_DeviceGetDataPtr(cdrom_Device *cdrom);
unsigned int cdrom_DeviceGetDataLen(cdrom_Device *cdrom);
void cdrom_DeviceClearDataPoll(cdrom_Device *cdrom,unsigned int len);
int cdrom_DeviceInitCD(cdrom_Device *cdrom);
unsigned char  *  cdrom_DeviceGetBytes(cdrom_Device *cdrom,unsigned int len);
unsigned int cdrom_DeviceGedWord(cdrom_Device *cdrom);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#pragma pack(pop)

extern unsigned int _3do_DiscSize();
extern void _3do_Read2048(void *buff);
extern void _3do_OnSector(unsigned int sector);


void cdrom_DeviceInit(cdrom_Device *cdrom)
{
	unsigned int filesize;

	filesize=150;
	cdrom->DataPtr=0;

		cdrom->XbusStatus=0;
		//XBPOLL=POLSTMASK|POLDTMASK|POLMAMASK|POLREMASK;
		cdrom->Poll=0xf;
		cdrom->XbusStatus|=CDST_TRAY; //Inject the disc
		cdrom->XbusStatus|=CDST_RDY;
		cdrom->XbusStatus|=CDST_DISC;
		cdrom->XbusStatus|=CDST_SPIN;
		cdrom->MEIStatus=MEI_CDROM_no_error;

		cdrom->DISC.firsttrk=1;
		cdrom->DISC.lasttrk=1;
		cdrom->DISC.curabsmsf[0]=0;
		cdrom->DISC.curabsmsf[1]=2;
		cdrom->DISC.curabsmsf[2]=0;

		cdrom->DISC.DiscTOC[1].CDCTL=CD_CTL_DATA_TRACK|CD_CTL_Q_NONE;//|CD_CTL_COPY_PERMITTED;
		cdrom->DISC.DiscTOC[1].TRKNUM=1;
		cdrom->DISC.DiscTOC[1].mm=0;
		cdrom->DISC.DiscTOC[1].ss=2;
		cdrom->DISC.DiscTOC[1].ff=0;

		cdrom->DISC.firsttrk=1;
		cdrom->DISC.lasttrk=1;
		cdrom->DISC.discid=MEI_DISC_DA_OR_CDROM;

		cdrom->DISC.templba=filesize;
		cdrom_DeviceLBA2MSF(cdrom);
		cdrom->DISC.totalmsf[0]=cdrom->DISC.tempmsf[0];
		cdrom->DISC.totalmsf[1]=cdrom->DISC.tempmsf[1];
		cdrom->DISC.totalmsf[2]=cdrom->DISC.tempmsf[2];

		cdrom->DISC.templba=filesize-150;
		cdrom_DeviceLBA2MSF(cdrom);
		cdrom->DISC.sesmsf[0]=cdrom->DISC.tempmsf[0];
		cdrom->DISC.sesmsf[1]=cdrom->DISC.tempmsf[1];
		cdrom->DISC.sesmsf[2]=cdrom->DISC.tempmsf[2];


		cdrom->STATCYC=STATDELAY;
}

unsigned int cdrom_DeviceGetStatusFifo(cdrom_Device *cdrom)
{
	unsigned int res;
	res=0;
	if(cdrom->StatusLen>0)
	{
		res=cdrom->Status[0];
		cdrom->StatusLen--;
		if(cdrom->StatusLen>0)
			memcpy(cdrom->Status,cdrom->Status+1,cdrom->StatusLen);
		else
		{
				cdrom->Poll&=~POLST;
		}
	}
	return res;

}

void  cdrom_DeviceSendCommand(cdrom_Device *cdrom, unsigned char val)
{
 	if (cdrom->CmdPtr<7)
	{
			cdrom->Command[cdrom->CmdPtr]=(unsigned char)val;
			cdrom->CmdPtr++;

	}
	if((cdrom->CmdPtr>=7) || (cdrom->Command[0]==0x8))
	{

			//Poll&=~0x80; ???
			cdrom_DeviceDoCommand(cdrom);
			cdrom->CmdPtr=0;
	}
}

unsigned int cdrom_DeviceGetPoll(cdrom_Device *cdrom)
{
	return cdrom->Poll;
}

int cdrom_DeviceTestFIQ(cdrom_Device *cdrom)
{
	if(((cdrom->Poll&POLST) && (cdrom->Poll&POLSTMASK)) || ((cdrom->Poll&POLDT) && (cdrom->Poll&POLDTMASK)))
	{
		return 1;
	}
	return 0;
}

void cdrom_DeviceSetPoll(cdrom_Device *cdrom, unsigned int val)
{
	cdrom->Poll&=0xF0;
	val&=0xf;
	cdrom->Poll|=val;
}

unsigned int cdrom_DeviceGetDataFifo(cdrom_Device *cdrom)
{
	unsigned int res;
	res=0;
        //int i;

	if(cdrom->DataLen>0)
	{
		res=(unsigned char)cdrom->Data[cdrom->DataPtr];
		cdrom->DataLen--;
		cdrom->DataPtr++;

		if(cdrom->DataLen==0)
		{
			cdrom->DataPtr=0;
			if(cdrom->Requested)
			{
                _3do_OnSector(cdrom->curr_sector++);
                _3do_Read2048(cdrom->Data);
				cdrom->Requested--;
				cdrom->DataLen=REQSIZE;
			}
			else
			{
				cdrom->Poll&=~POLDT;
				cdrom->Requested=0;
				cdrom->DataLen=0;
				cdrom->DataPtr=0;
			}

		}
	}

	return res;
}

void cdrom_DeviceDoCommand(cdrom_Device *cdrom)
{
	int i;

	/*for(i=0;i<=0x100000;i++)
		Data[i]=0;

	cdrom->DataLen=0;*/
	cdrom->StatusLen=0;


	cdrom->Poll&=~POLST;
	cdrom->Poll&=~POLDT;
	cdrom->XbusStatus&=~CDST_ERRO;
	cdrom->XbusStatus&=~CDST_RDY;
	switch(cdrom->Command[0])
	{
	case 0x1:
		//seek
		//not used in opera
		//01 00 ll-bb-aa 00 00.
		//01 02 mm-ss-ff 00 00.
		//status 4 bytes
		//xx xx xx XS  (xs=xbus status)
//		sprintf(str,"#CDROM 0x1 SEEK!!!\n");
//		CDebug::DPrint(str);
		break;
	case 0x2:
		//spin up
		//opera status request = 0
		//status 4 bytes
		//xx xx xx XS  (xs=xbus status)
		if((cdrom->XbusStatus&CDST_TRAY) && (cdrom->XbusStatus&CDST_DISC))
		{
			cdrom->XbusStatus|=CDST_SPIN;
			cdrom->XbusStatus|=CDST_RDY;
			cdrom->MEIStatus=MEI_CDROM_no_error;
		}
		else
		{
			cdrom->XbusStatus|=CDST_ERRO;
			cdrom->XbusStatus&=~CDST_RDY;
			cdrom->MEIStatus=MEI_CDROM_recv_ecc;

		}

		cdrom->Poll|=POLST; //status is valid

		cdrom->StatusLen=2;
		cdrom->Status[0]=0x2;
		//Status[1]=0x0;
		//Status[2]=0x0;
		cdrom->Status[1]=cdrom->XbusStatus;


		break;
	case 0x3:
		// spin down
		//opera status request = 0 // not used in opera
		//status 4 bytes
		//xx xx xx XS  (xs=xbus status)
		if((cdrom->XbusStatus&CDST_TRAY) && (cdrom->XbusStatus&CDST_DISC))
		{
			cdrom->XbusStatus&=~CDST_SPIN;
			cdrom->XbusStatus|=CDST_RDY;
			cdrom->MEIStatus=MEI_CDROM_no_error;

		}
		else
		{
			cdrom->XbusStatus|=CDST_ERRO;
			cdrom->XbusStatus|=CDST_RDY;
			cdrom->MEIStatus=MEI_CDROM_recv_ecc;

		}

		cdrom->Poll|=POLST; //status is valid

		cdrom->StatusLen=2;
		cdrom->Status[0]=0x3;
		//Status[1]=0x0;
		//Status[2]=0x0;
		cdrom->Status[1]=cdrom->XbusStatus;

		break;
	case 0x4:
		//diagnostics
		// not used in opera
         //04 00 ll-bb-aa 00 00.
         //04 02 mm-ss-ff 00 00.
		 //status 4 bytes
		 //xx S1 S2 XS
//		sprintf(str,"#CDROM 0x4 Diagnostic!!!\n");
//		CDebug::DPrint(str);

		break;
	case 0x6:
		// eject disc
		//opera status request = 0
		//status 4 bytes
		//xx xx xx XS
		// 1b command of scsi
		//emulation ---
		// Execute EJECT command;
		// Check Sense, update cdrom->PollRegister (if medium present)
		cdrom->XbusStatus&=~CDST_TRAY;
		cdrom->XbusStatus&=~CDST_DISC;
		cdrom->XbusStatus&=~CDST_SPIN;
		cdrom->XbusStatus&=~CDST_2X;
		cdrom->XbusStatus&=~CDST_ERRO;
		cdrom->XbusStatus|=CDST_RDY;
		cdrom->Poll|=POLST; //status is valid
		cdrom->Poll&=~POLMA;
		cdrom->MEIStatus=MEI_CDROM_no_error;

		cdrom->StatusLen=2;
		cdrom->Status[0]=0x6;
		//Status[1]=0x0;
		//Status[2]=0x0;
		cdrom->Status[1]=cdrom->XbusStatus;

	/*	ClearCDB();
		CDB[0]=0x1b;
		CDB[4]=0x2;
		CDBLen=12;
*/



		break;
	case 0x7:
		// inject disc
		//opera status request = 0
		//status 4 bytes
		//xx xx xx XS
		//1b command of scsi
//		sprintf(str,"#CDROM 0x7 INJECT!!!\n");
//		CDebug::DPrint(str);

		break;
	case 0x8:
		// abort !!!
		//opera status request = 31
		//status 4 bytes
		//xx xx xx XS
		//
		cdrom->StatusLen=33;
		cdrom->Status[0]=0x8;
		for(i=1;i<32;i++)
			cdrom->Status[i]=0;
		cdrom->Status[32]=cdrom->XbusStatus;

		cdrom->XbusStatus|=CDST_RDY;
		cdrom->MEIStatus=MEI_CDROM_no_error;


		break;
	case 0x9:
		// mode set
		//09 MM nn 00 00 00 00 // 2048 or 2340 transfer size
		// to be checked -- wasn't called even once
		// 2nd byte is type selector
		// MM = mode nn= value
		//opera status request = 0
		//status 4 bytes
		//xx xx xx XS
		// to check!!!

	//	if((cdrom->XbusStatus&CDST_TRAY) && (cdrom->XbusStatus&CDST_DISC))
	//	{
			cdrom->XbusStatus|=CDST_RDY;
			cdrom->MEIStatus=MEI_CDROM_no_error;

			//CDMode[Command[1]]=Command[2];
	//	}
	//	else
	//	{
	//		cdrom->XbusStatus|=CDST_ERRO;
	//		cdrom->XbusStatus&=~CDST_RDY;
	//	}filesize

		cdrom->Poll|=POLST; //status is valid

		cdrom->StatusLen=2;
		cdrom->Status[0]=0x9;
		cdrom->Status[1]=cdrom->XbusStatus;



		break;
	case 0x0a:
		// reset
		//not used in opera
		//status 4 bytes
		//xx xx xx XS
//		sprintf(str,"#CDROM 0xa RESET!!!\n");
//		CDebug::DPrint(str);
		break;
	case 0x0b:
		// flush
		//opera status request = 31
		//status 4 bytes
		//xx xx xx XS
		//returns data
		//flush all internal buffer
		//1+31+1
		cdrom->XbusStatus|=CDST_RDY;
		cdrom->StatusLen=33;
		cdrom->Status[0]=0xb;
		for(i=1;i<32;i++)
			cdrom->Status[i]=0;
		cdrom->Status[32]=cdrom->XbusStatus;

		//cdrom->XbusStatus|=CDST_RDY;
		cdrom->MEIStatus=MEI_CDROM_no_error;



		break;
	case 0x10:
		//Read Data !!!
		//10 01 00 00 00 00 01 // read 0x0 blocks from MSF=1.0.0
		//10 xx-xx-xx nn-nn fl.
		//00 01 02 03 04 05 06
		//reads nn blocks from xx
		//fl=0 xx="lba"
		//fl=1 xx="msf"
		//block= 2048 bytes
		//opera status request = 0
		//status 4 bytes
		//xx xx xx XS
		//returns data
		// here we go


		//olddataptr=cdrom->DataLen;
		if((cdrom->XbusStatus&CDST_TRAY)&&(cdrom->XbusStatus&CDST_DISC)&&(cdrom->XbusStatus&CDST_SPIN))
		{
			cdrom->XbusStatus|=CDST_RDY;
			//CDMode[Command[1]]=Command[2];
			cdrom->StatusLen=2;
			cdrom->Status[0]=0x10;
			//Status[1]=0x0;
			//Status[2]=0x0;
			cdrom->Status[1]=cdrom->XbusStatus;

			//if(Command[6]==Address_Abs_MSF)
			{
				cdrom->DISC.curabsmsf[0]=(cdrom->Command[1]);
				cdrom->DISC.curabsmsf[1]=(cdrom->Command[2]);
				cdrom->DISC.curabsmsf[2]=(cdrom->Command[3]);
				cdrom->DISC.tempmsf[0]=cdrom->DISC.curabsmsf[0];
				cdrom->DISC.tempmsf[1]=cdrom->DISC.curabsmsf[1];
				cdrom->DISC.tempmsf[2]=cdrom->DISC.curabsmsf[2];
				cdrom_DeviceMSF2LBA(cdrom);


				//if(fiso!=NULL)
				//	fseek(fiso,cdrom->DISC.templba*2048+iso_off_from_begin,SEEK_SET);
				{
                         cdrom->curr_sector=cdrom->DISC.templba;
						_3do_OnSector(cdrom->DISC.templba);
				}
					//fseek(fiso,cdrom->DISC.templba*2048,SEEK_SET);
					//fseek(fiso,cdrom->DISC.templba*2336,SEEK_SET);
			}





			cdrom->olddataptr=(cdrom->Command[5]<<8)+cdrom->Command[6];
			//olddataptr=olddataptr*2048; //!!!
			cdrom->Requested=cdrom->olddataptr;


				if(cdrom->Requested)
				{
                    _3do_OnSector(cdrom->curr_sector++);
					_3do_Read2048(cdrom->Data);
                    cdrom->DataLen=REQSIZE;
                    cdrom->Requested--;
				}
                                else cdrom->DataLen=0;

			cdrom->Poll|=POLDT;
			cdrom->Poll|=POLST;
			cdrom->MEIStatus=MEI_CDROM_no_error;


		}
		else
		{
			cdrom->XbusStatus|=CDST_ERRO;
			cdrom->XbusStatus&=~CDST_RDY;
			cdrom->Poll|=POLST; //status is valid
			cdrom->StatusLen=2;
			cdrom->Status[0]=0x10;
			//Status[1]=0x0;
			//Status[2]=0x0;
			cdrom->Status[1]=cdrom->XbusStatus;
			cdrom->MEIStatus=MEI_CDROM_recv_ecc;

		}


		break;
	case 0x80:
		// data path chech
		//opera status request = 2
		//MKE =2
		// status 4 bytes
		// 80 AA 55 XS
		cdrom->XbusStatus|=CDST_RDY;
		cdrom->StatusLen=4;
		cdrom->Status[0]=0x80;
		cdrom->Status[1]=0xaa;
		cdrom->Status[2]=0x55;
		cdrom->Status[3]=cdrom->XbusStatus;
		cdrom->Poll|=POLST;
		cdrom->MEIStatus=MEI_CDROM_no_error;


		break;
	case 0x82:
		//read error (get last status???)
		//opera status request = 8 ---- tests status req=9?????
		//MKE = 8!!!
		//00
		//11
		//22   Current Status //MKE / Opera???
		//33
		//44
		//55
		//66
		//77
		//88   Current Status //TEST
		cdrom->Status[0]=0x82;
		cdrom->Status[1]=cdrom->MEIStatus;
		cdrom->Status[2]=cdrom->MEIStatus;
		cdrom->Status[3]=cdrom->MEIStatus;
		cdrom->Status[4]=cdrom->MEIStatus;
		cdrom->Status[5]=cdrom->MEIStatus;
		cdrom->Status[6]=cdrom->MEIStatus;
		cdrom->Status[7]=cdrom->MEIStatus;
		cdrom->Status[8]=cdrom->MEIStatus;
		cdrom->XbusStatus|=CDST_RDY;
		cdrom->Status[9]=cdrom->XbusStatus;
		//cdrom->Status[9]=cdrom->XbusStatus; // 1 == disc present
		cdrom->StatusLen=10;
		cdrom->Poll|=POLST;
		//cdrom->Poll|=0x80; //MDACC



		break;
	case 0x83:
		//read id
		//opera status request = 10
		//status 12 bytes (3 words)
		//MEI text + XS
		//00 M E I 1 01 00 00 00 00 00 XS
		cdrom->XbusStatus|=CDST_RDY;
		cdrom->StatusLen=12;
		cdrom->Status[0]=0x83;
		cdrom->Status[1]=0x00;//manufacture id
		cdrom->Status[2]=0x10;//10
		cdrom->Status[3]=0x00;//MANUFACTURE NUM
		cdrom->Status[4]=0x01;//01
		cdrom->Status[5]=00;
		cdrom->Status[6]=00;
		cdrom->Status[7]=0;//REVISION NUMBER:
		cdrom->Status[8]=0;
		cdrom->Status[9]=0x00;//FLAG BYTES
		cdrom->Status[10]=0x00;
		cdrom->Status[11]=cdrom->XbusStatus;//DEV.DRIVER SIZE
		//cdrom->Status[11]=cdrom->XbusStatus;
		//cdrom->Status[12]=cdrom->XbusStatus;
		cdrom->Poll|=POLST;
		cdrom->MEIStatus=MEI_CDROM_no_error;

		break;
	case 0x84:
		//mode sense
		//not used in opera
		//84 mm 00 00 00 00 00.
		//status 4 bytes
		//xx S1 S2 XS
		//xx xx nn XS
		//
		cdrom->StatusLen=4;
		cdrom->Status[0]=0x0;
		cdrom->Status[1]=0x0;
		cdrom->Status[2]=0x0;

		if((cdrom->XbusStatus&CDST_TRAY) && (cdrom->XbusStatus&CDST_DISC))
		{
			cdrom->XbusStatus|=CDST_RDY;
			//CDMode[cdrom->Command[1]]=cdrom->Command[2];
			//cdrom->Status[2]=CDMode[cdrom->Command[1]];
		}
		else
		{
			cdrom->XbusStatus|=CDST_ERRO;
			cdrom->XbusStatus&=~CDST_RDY;
		}

		cdrom->Poll|=POLST; //status is valid

		cdrom->Status[3]=cdrom->XbusStatus;



		break;
	case 0x85:
		//read capacity
		//status 8 bytes
		//opera status request = 6
		//cc cc cc cc cc cc cc XS
		//data?
		//00 85
		//11 mm  total
		//22 ss  total
		//33 ff  total
		//44 ??
		//55 ??
		//66 ??
		if((cdrom->XbusStatus&CDST_TRAY)&&(cdrom->XbusStatus&CDST_DISC)&&(cdrom->XbusStatus&CDST_SPIN))
		{
			cdrom->StatusLen=8;//CMD+status+DRVSTAT
			cdrom->Status[0]=0x85;
			cdrom->Status[1]=0;
			cdrom->Status[2]=cdrom->DISC.totalmsf[0]; //min
			cdrom->Status[3]=cdrom->DISC.totalmsf[1]; //sec
			cdrom->Status[4]=cdrom->DISC.totalmsf[2]; //fra
			cdrom->Status[5]=0x00;
			cdrom->Status[6]=0x00;
			cdrom->XbusStatus|=CDST_RDY;
			cdrom->Status[7]=cdrom->XbusStatus;
			cdrom->Poll|=POLST;
			cdrom->MEIStatus=MEI_CDROM_no_error;


		}
		else
		{
			cdrom->XbusStatus|=CDST_ERRO;
			cdrom->XbusStatus&=~CDST_RDY;
			cdrom->StatusLen=2;//CMD+status+DRVSTAT
			cdrom->Status[0]=0x85;
			cdrom->Status[1]=cdrom->XbusStatus;
			cdrom->Poll|=POLST;
			cdrom->MEIStatus=MEI_CDROM_recv_ecc;

		}

		break;
	case 0x86:
		//read header
		// not used in opera
		// 86 00 ll-bb-aa 00 00.
		// 86 02 mm-ss-ff 00 00.
		// status 8 bytes
		// data?
//		sprintf(str,"#CDROM 0x86 READ HEADER!!!\n");
//		CDebug::DPrint(str);

		break;
	case 0x87:
		//read subq
		//opera status request = 10
		//87 fl 00 00 00 00 00
		//fl=0 "lba"
		//fl=1 "msf"
		//
		//11 00 (if !=00 then break)
		//22 Subq_ctl_adr=swapnibles(_11_)
		//33 Subq_trk = but2bcd(_22_)
		//44 Subq_pnt_idx=byt2bcd(_33_)
		//55 mm run tot
		//66 ss run tot
		//77 ff run tot
		//88 mm run trk
		//99 ss run trk
		//aa ff run trk

		if((cdrom->XbusStatus&CDST_TRAY)&&(cdrom->XbusStatus&CDST_DISC)&&(cdrom->XbusStatus&CDST_SPIN))
		{
			cdrom->StatusLen=12;//CMD+status+DRVSTAT
			cdrom->Status[0]=0x87;
			cdrom->Status[1]=0;//cdrom->DISC.totalmsf[0]; //min
			cdrom->Status[2]=0; //sec
			cdrom->Status[3]=0; //fra
			cdrom->Status[4]=0;
			cdrom->Status[5]=0;
			cdrom->XbusStatus|=CDST_RDY;
			cdrom->Status[6]=0x0;
			cdrom->Status[7]=0x0;
			cdrom->Status[8]=0x0;
			cdrom->Status[9]=0x0;
			cdrom->Status[10]=0x0;
			cdrom->Status[11]=cdrom->XbusStatus;
			cdrom->Poll|=POLST;
			cdrom->MEIStatus=MEI_CDROM_no_error;


		}
		else
		{
			cdrom->XbusStatus|=CDST_ERRO;
			cdrom->XbusStatus&=~CDST_RDY;
			cdrom->StatusLen=2;//CMD+status+DRVSTAT
			cdrom->Status[0]=0x85;
			cdrom->Status[1]=cdrom->XbusStatus;
			cdrom->Poll|=POLST;
			cdrom->MEIStatus=MEI_CDROM_recv_ecc;

		}




		break;
	case 0x88:
		//read upc
		// not used in opera
		//88 00 ll-bb-aa 00 00
		//88 02 mm-ss-ff 00 00
		//status 20(16) bytes
		//data?
//		sprintf(str,"#CDROM 0x88 READ UPC!!!\n");
//		CDebug::DPrint(str);

		break;
	case 0x89:
		//read isrc
		// not used in opera
		//89 00 ll-bb-aa 00 00
		//89 02 mm-ss-ff 00 00
		//status 16(15) bytes
		//data?

//		sprintf(str,"#CDROM 0x89 READ ISRC!!!\n");
//		CDebug::DPrint(str);

		break;
	case 0x8a:
		//read disc code
		//ignore it yet...
		////opera status request = 10
		// 8a 00 00 00 00 00 00
		//status 10 bytes
		//????? which code???
		if((cdrom->XbusStatus&CDST_TRAY)&&(cdrom->XbusStatus&CDST_DISC)&&(cdrom->XbusStatus&CDST_SPIN))
		{
			cdrom->StatusLen=12;//CMD+status+DRVSTAT
			cdrom->Status[0]=0x8a;
			cdrom->Status[1]=0;//cdrom->DISC.totalmsf[0]; //min
			cdrom->Status[2]=0; //sec
			cdrom->Status[3]=0; //fra
			cdrom->Status[4]=0;
			cdrom->Status[5]=0;
			cdrom->XbusStatus|=CDST_RDY;
			cdrom->Status[6]=0x0;
			cdrom->Status[7]=0x0;
			cdrom->Status[8]=0x0;
			cdrom->Status[9]=0x0;
			cdrom->Status[10]=0x0;
			cdrom->Status[11]=cdrom->XbusStatus;
			cdrom->Poll|=POLST;
			cdrom->MEIStatus=MEI_CDROM_no_error;


		}
		else
		{
			cdrom->XbusStatus|=CDST_ERRO;
			cdrom->XbusStatus&=~CDST_RDY;
			cdrom->StatusLen=2;//CMD+status+DRVSTAT
			cdrom->Status[0]=0x85;
			cdrom->Status[1]=cdrom->XbusStatus;
			cdrom->Poll|=POLST;
			cdrom->MEIStatus=MEI_CDROM_recv_ecc;

		}



		break;
	case 0x8b:
		//MKE !!!v the same
		//read disc information
		//opera status request = 6
		//8b 00 00 00 00 00 00
		//status 8(6) bytes
		//read the toc descritor
		//00 11 22 33 44 55 XS
		//00= 8b //command code
		//11= Disc ID /// XA_BYTE
		//22= 1st track#
		//33= last track#
		//44= minutes
		//55= seconds
		//66= frames


		cdrom->StatusLen=8;//6+1 + 1 for what?
		cdrom->Status[0]=0x8b;
		if(cdrom->XbusStatus&(CDST_TRAY|CDST_DISC|CDST_SPIN))
		{
			cdrom->Status[1]=cdrom->DISC.discid;
			cdrom->Status[2]=cdrom->DISC.firsttrk;
			cdrom->Status[3]=cdrom->DISC.lasttrk;
			cdrom->Status[4]=cdrom->DISC.totalmsf[0]; //minutes
			cdrom->Status[5]=cdrom->DISC.totalmsf[1]; //seconds
			cdrom->XbusStatus|=CDST_RDY;
			cdrom->Status[6]=cdrom->DISC.totalmsf[2]; //frames
			cdrom->MEIStatus=MEI_CDROM_no_error;
			cdrom->Status[7]=cdrom->XbusStatus;
		}
		else
		{
			cdrom->StatusLen=2;//6+1 + 1 for what?
			cdrom->XbusStatus|=CDST_ERRO;
			cdrom->MEIStatus=MEI_CDROM_recv_ecc;
			cdrom->Status[1]=cdrom->XbusStatus;
		}

		cdrom->Poll|=POLST; //status is valid

		break;
	case 0x8c:
		//read toc
		//MKE !!!v the same
		//opera status request = 8
		//8c fl nn 00 00 00 00 // reads nn entry
		//status 12(8) bytes
		//00 11 22 33 44 55 66 77 XS
		//00=8c
		//11=reserved0; // NIX BYTE
		//22=addressAndControl; //TOCENT_CTL_ADR=swapnibbles(11) ??? UPCCTLADR=_10_ | x02 (_11_ &F0 = _10_)
		//33=trackNumber;  //TOC_ENT NUMBER
		//44=reserved3;    //TOC_ENT FORMAT
		//55=minutes;     //TOCENT ADRESS == 0x00445566
		//66=seconds;
		//77=frames;
		//88=reserved7;
		cdrom->StatusLen=10;//CMD+status+DRVSTAT
		cdrom->Status[0]=0x8c;
		if(cdrom->XbusStatus&(CDST_TRAY|CDST_DISC|CDST_SPIN))
		{
			cdrom->Status[1]=cdrom->DISC.DiscTOC[cdrom->Command[2]].res0;
			cdrom->Status[2]=cdrom->DISC.DiscTOC[cdrom->Command[2]].CDCTL;
			cdrom->Status[3]=cdrom->DISC.DiscTOC[cdrom->Command[2]].TRKNUM;
			cdrom->Status[4]=cdrom->DISC.DiscTOC[cdrom->Command[2]].res1;
			cdrom->Status[5]=cdrom->DISC.DiscTOC[cdrom->Command[2]].mm; //min
			cdrom->XbusStatus|=CDST_RDY;
			cdrom->Status[6]=cdrom->DISC.DiscTOC[cdrom->Command[2]].ss; //sec
			cdrom->Status[7]=cdrom->DISC.DiscTOC[cdrom->Command[2]].ff; //frames
			cdrom->Status[8]=cdrom->DISC.DiscTOC[cdrom->Command[2]].res2;
			cdrom->MEIStatus=MEI_CDROM_no_error;
			cdrom->Status[9]=cdrom->XbusStatus;

		}
		else
		{
			cdrom->StatusLen=2;
			cdrom->XbusStatus|=CDST_ERRO;
			cdrom->MEIStatus=MEI_CDROM_recv_ecc;
			cdrom->Status[1]=cdrom->XbusStatus;
		}

		cdrom->Poll|=POLST;




		break;
	case 0x8d:
		//read session information
		//MKE !!!v the same
		//opera status request = 6
		//status 8(6)
		//00 11 22 33 44 55 XS ==
		//00=8d
		//11=valid;  // 0x80 = MULTISESS
		//22=minutes;
		//33=seconds;
		//44=frames;
		//55=rfu1; //ignore
		//66=rfu2  //ignore

		cdrom->StatusLen=8;//CMD+status+DRVSTAT
		cdrom->Status[0]=0x8d;
		if((cdrom->XbusStatus&CDST_TRAY) && (cdrom->XbusStatus&CDST_DISC))
		{
			cdrom->Status[1]=0x00;
			cdrom->Status[2]=0x0;//cdrom->DISC.sesmsf[0];//min
			cdrom->Status[3]=0x2;//cdrom->DISC.sesmsf[1];//sec
			cdrom->Status[4]=0x0;//cdrom->DISC.sesmsf[2];//fra
			cdrom->Status[5]=0x00;
			cdrom->XbusStatus|=CDST_RDY;
			cdrom->Status[6]=0x00;
			cdrom->Status[7]=cdrom->XbusStatus;
			cdrom->MEIStatus=MEI_CDROM_no_error;

		}
		else
		{
			cdrom->StatusLen=2;//CMD+status+DRVSTAT
			cdrom->XbusStatus|=CDST_ERRO;
			cdrom->Status[1]=cdrom->XbusStatus;
			cdrom->MEIStatus=MEI_CDROM_recv_ecc;

		}


		cdrom->Poll|=POLST;




		break;
	case 0x8e:
		//read device driver
		break;
	case 0x93:
		//?????
		cdrom->StatusLen=4;
		cdrom->Status[0]=0x0;
		cdrom->Status[1]=0x0;
		cdrom->Status[2]=0x0;

		if((cdrom->XbusStatus&CDST_TRAY) && (cdrom->XbusStatus&CDST_DISC))
		{
			cdrom->XbusStatus|=CDST_RDY;
			//CDMode[cdrom->Command[1]]=cdrom->Command[2];
//			cdrom->Status[2]=CDMode[cdrom->Command[1]];
		}
		else
		{
			cdrom->XbusStatus|=CDST_ERRO;
			cdrom->XbusStatus|=CDST_RDY;
		}

		cdrom->Poll|=POLST; //status is valid

		cdrom->Status[3]=cdrom->XbusStatus;

		break;
	default:
		// error!!!
		//sprintf(str,"#CDROM %x!!!\n",cdrom->Command[0]);
		//CDebug::DPrint(str);
		break;
	}

}

unsigned char cdrom_DeviceBCD2BIN(cdrom_Device *cdrom, unsigned char in)
{
	return ((in>>4)*10+(in&0x0F));
}

unsigned char cdrom_DeviceBIN2BCD(cdrom_Device *cdrom, unsigned char in)
{
	return((in/10)<<4)|(in%10);
}

void cdrom_DeviceMSF2BLK(cdrom_Device *cdrom)
{


	cdrom->DISC.tempblk=(cdrom->DISC.tempmsf[0] * 60 + cdrom->DISC.tempmsf[1]) * 75 + cdrom->DISC.tempmsf[2] - 150;
	if (cdrom->DISC.tempblk<0)
		cdrom->DISC.tempblk=0; //??

}

void cdrom_DeviceBLK2MSF(cdrom_Device *cdrom)
{
	unsigned int mm;
	cdrom->DISC.tempmsf[0]=(cdrom->DISC.tempblk+150) / (60*75);
	mm= (cdrom->DISC.tempblk+150)%(60*75);
	cdrom->DISC.tempmsf[1]=mm/75;
	cdrom->DISC.tempmsf[2]=mm%75;
}


void cdrom_DeviceLBA2MSF(cdrom_Device *cdrom)
{
 		cdrom->DISC.templba+=150;
		cdrom->DISC.tempmsf[0]=cdrom->DISC.templba/(60*75);
		cdrom->DISC.templba%=(60*75);
		cdrom->DISC.tempmsf[1]=cdrom->DISC.templba/75;
		cdrom->DISC.tempmsf[2]=cdrom->DISC.templba%75;
}

void cdrom_DeviceMSF2LBA(cdrom_Device *cdrom)
{
	cdrom->DISC.templba=(cdrom->DISC.tempmsf[0] * 60 + cdrom->DISC.tempmsf[1]) * 75 + cdrom->DISC.tempmsf[2] - 150;
	if(cdrom->DISC.templba<0)
		cdrom->DISC.templba=0;

}

unsigned char * cdrom_DeviceGetDataPtr(cdrom_Device *cdrom)
{
	return cdrom->Data;
}

unsigned int cdrom_DeviceGetDataLen(cdrom_Device *cdrom)
{
	return cdrom->DataLen;
}

void cdrom_DeviceClearDataPoll(cdrom_Device *cdrom, unsigned int len)
{
	if((int)len<=cdrom->DataLen)
	{
		if(cdrom->DataLen>0)
		{
			cdrom->DataLen-=len;
			if(cdrom->DataLen>0)
				memcpy(cdrom->Data,cdrom->Data+4,len);
			else
			{
				cdrom->Poll&=~POLDT;
			}
		}
	}
	else
	{
		cdrom->Poll&=~POLDT;

	}
}


int cdrom_DeviceInitCD(cdrom_Device *cdrom)
{
	unsigned int filesize=0;

    cdrom->curr_sector=0;
	_3do_OnSector(0);
	filesize=_3do_DiscSize()+150;
	cdrom->XbusStatus=0;
	cdrom->Poll=0xf;
	cdrom->XbusStatus|=CDST_TRAY; //Inject the disc
	cdrom->XbusStatus|=CDST_RDY;
	cdrom->XbusStatus|=CDST_DISC;
	cdrom->XbusStatus|=CDST_SPIN;

	cdrom->MEIStatus=MEI_CDROM_no_error;

	cdrom->DISC.firsttrk=1;
	cdrom->DISC.lasttrk=1;
	cdrom->DISC.curabsmsf[0]=0;
	cdrom->DISC.curabsmsf[1]=2;
	cdrom->DISC.curabsmsf[2]=0;

	cdrom->DISC.DiscTOC[1].CDCTL=CD_CTL_DATA_TRACK|CD_CTL_Q_NONE;//|CD_CTL_COPY_PERMITTED;
	cdrom->DISC.DiscTOC[1].TRKNUM=1;
	cdrom->DISC.DiscTOC[1].mm=0;
	cdrom->DISC.DiscTOC[1].ss=2;
	cdrom->DISC.DiscTOC[1].ff=0;

	cdrom->DISC.firsttrk=1;
	cdrom->DISC.lasttrk=1;
	cdrom->DISC.discid=MEI_DISC_DA_OR_CDROM;

	cdrom->DISC.templba=filesize;
	cdrom_DeviceLBA2MSF(cdrom);
	cdrom->DISC.totalmsf[0]=cdrom->DISC.tempmsf[0];
	cdrom->DISC.totalmsf[1]=cdrom->DISC.tempmsf[1];
	cdrom->DISC.totalmsf[2]=cdrom->DISC.tempmsf[2];

	//sprintf(str,"##ISO M=0x%x  S=0x%x  F=0x%x\n",cdrom->DISC.totalmsf[0],cdrom->DISC.totalmsf[1],cdrom->DISC.totalmsf[2]);
	//CDebug::DPrint(str);


	cdrom->DISC.templba=filesize-150;
	cdrom_DeviceLBA2MSF(cdrom);
	cdrom->DISC.sesmsf[0]=cdrom->DISC.tempmsf[0];
	cdrom->DISC.sesmsf[1]=cdrom->DISC.tempmsf[1];
	cdrom->DISC.sesmsf[2]=cdrom->DISC.tempmsf[2];
	return 0;
}

unsigned char  *cdrom_DeviceGetBytes(cdrom_Device *cdrom, unsigned int len)
{
	//unsigned char * retmem;

	//retmem=Data;
        (void)len;

	return cdrom->Data;
}

unsigned int cdrom_DeviceGedWord(cdrom_Device *cdrom)
{
	unsigned int res;
	res=0;


	if(cdrom->DataLen>0)
	{
		//res=(unsigned char)Data[0];
		//res
		res=(cdrom->Data[0]<<24)+(cdrom->Data[1]<<16)+(cdrom->Data[2]<<8)+cdrom->Data[3];
		if(cdrom->DataLen<3)
		{
			cdrom->DataLen--;
			if(cdrom->DataLen>0)
				memcpy(cdrom->Data,cdrom->Data+1,cdrom->DataLen);
			else
			{
				cdrom->Poll&=~POLDT;
				cdrom->DataLen=0;
			}
			cdrom->DataLen--;
			if(cdrom->DataLen>0)
				memcpy(cdrom->Data,cdrom->Data+1,cdrom->DataLen);
			else
			{
				cdrom->Poll&=~POLDT;
				cdrom->DataLen=0;
			}
			cdrom->DataLen--;
			if(cdrom->DataLen>0)
				memcpy(cdrom->Data,cdrom->Data+1,cdrom->DataLen);
			else
			{
				cdrom->Poll&=~POLDT;
				cdrom->DataLen=0;
			}
		}
		else
		{
			//DataLen-=4;
			{
				memcpy(cdrom->Data,cdrom->Data+4,cdrom->DataLen-4);
				cdrom->DataLen-=4;
			}

			if(cdrom->DataLen<=0)
			{
				cdrom->DataLen=0;
				cdrom->Poll&=~POLDT;
			}
		}

	}

	return res;
}








//plugins----------------------------------------------------------------------------------
cdrom_Device isodrive;

void* _xbplug_MainDevice(int proc, void* data)
{
 uint32 tmp;
 //void* xfisonew;
 switch(proc)
 {
	case XBP_INIT:
		cdrom_DeviceInit(&isodrive);
		return (void*)1;
	case XBP_RESET:
                cdrom_DeviceInit(&isodrive);
                if(_3do_DiscSize())
		        cdrom_DeviceInitCD(&isodrive);
		break;
	case XBP_SET_COMMAND:
		cdrom_DeviceSendCommand(&isodrive, (intptr_t)data);
		break;
	case XBP_FIQ:
		return (void*)cdrom_DeviceTestFIQ(&isodrive);
	case XBP_GET_DATA:
		return (void*)(intptr_t)cdrom_DeviceGetDataFifo(&isodrive);
	case XBP_GET_STATUS:
		return (void*)(intptr_t)cdrom_DeviceGetStatusFifo(&isodrive);
	case XBP_SET_POLL:
		cdrom_DeviceSetPoll(&isodrive,(intptr_t)data);
		break;
	case XBP_GET_POLL:
		return (void*)(intptr_t)cdrom_DeviceGetPoll(&isodrive);
	case XBP_DESTROY:
		break;
	case XBP_GET_SAVESIZE:
		tmp=sizeof(cdrom_Device);
		return (void*)(intptr_t)tmp;
	case XBP_GET_SAVEDATA:
		memcpy(data,&isodrive,sizeof(cdrom_Device));
		break;
	case XBP_SET_SAVEDATA:
		memcpy(&isodrive,data,sizeof(cdrom_Device));
		return (void*)1;
 };

 return NULL;
}




