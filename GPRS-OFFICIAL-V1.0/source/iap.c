/* iap driver
 *
 * based on iap driver for LPC213x Controller made by CUONG DOLA, 2015
 * see:
 *
 */


//IAP-Commands
#define	PREPARE_SECTOR_FOR_WRITE_OPERATION	50
#define	COPY_RAM_TO_FLASH					51
#define	ERASE_SECTOR						52
#define	BLANK_CHECK_SECTOR					53
#define READ_PART_ID						54
#define	READ_BOOT_CODE_VERSION				55
#define COMPARE								56

// IAP status codes
#define CMD_SUCCESS             0
#define INVALID_COMMAND         1
#define SRC_ADDR_ERROR          2
#define DST_ADDR_ERROR          3
#define SRC_ADDR_NOT_MAPPED     4
#define DST_ADDR_NOT_MAPPED     5
#define COUNT_ERROR             6
#define INVALID_SECTOR          7
#define SECTOR_NOT_BLANK        8
#define SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION     9
#define COMPARE_ERROR           10
#define BUSY                    11


//other iap defines
#define IAP_LOCATION  0x7FFFFFF1	// IAP start location on flash
#define _XTAL	55296				// CCLK in kHz

// PLL
#define PLLCON_PLLE		0x01		// PLL Enable
#define PLLCON_PLLD		0x00		// PLL Disable
#define PLLCON_PLLC		0x03		// PLL Connect
#define PLLSTAT_PLOCK	0x0400		// PLL Lock Status

// config defines
//#define DEBUG
/* use only one of the following defines! */
//#define	USING_LPC2119	1
#define	USING_LPC2132	1



#include "iap.h"
#include <stdlib.h>
#include <LPC213x.H>                       /* LPC213x definitions  */
#include "main.h"
#ifdef DEBUG
	#include <stdio.h>
#endif


extern __attribute ((aligned(32))) char my_bl_data[256];
static unsigned int iap_command[5];			// contains parameters for IAP command
static unsigned int iap_result[2];			// contains results
typedef void (*IAP) (unsigned int[],unsigned int[]);	// typedefinition for IAP entry function
IAP IAP_Entry=(IAP) 0x7FFFFFF1;




/* P R I V A T E   F U N C T I O N   P R O T O T Y P E S */
long Blank_Check_Sector(long tmp_sect1, long tmp_sect2);
long Read_Boot_Code_Version(void);
long Prepare_Sectors(long tmp_sect1, long tmp_sect2);
long Erase_Sectors(long tmp_sect1, long tmp_sect2);
long Copy_RAM_To_Flash(long tmp_adr_dst, long tmp_adr_src, long tmp_size);
long Compare(long tmp_adr_dst, long tmp_adr_src, long tmp_size);
unsigned int Get_Sec_Num(unsigned long addr);
static long iap(long code, long p1, long p2, long p3, long p4);
static void pll_feed(void);
void pll_on(void);
void pll_off(void);


/******************************************************************************
 * P U B L I C   F U N C T I O N S
 *****************************************************************************/

/******************************************************************************
 * Function:	iap_BootLoaderVersion
 *
 * Description:	This functions gets the version of the internal boot loader and
 * 				prints it on the serial terminal if DEBUG is defined.
 *
 * Parameters:	-
 *
 * Return: 		Code CMD_SUCCESS,
 * 				Result0: 2 bytes of boot code version number. It is to be
 * 				interpreted as <byte1 (major)>.<byte0 (minor)>
 *****************************************************************************/
unsigned int iap_BootLoaderVersion(void)
{
	if(Read_Boot_Code_Version())
	{
		#ifdef DEBUG
		printf("\n-- ERROR - Reading boot code version --");
		#endif
		return 0;
	}
	#ifdef DEBUG
	printf("\nInternal boot code version: ");
//	printf(itoa((unsigned char) (iap_result[1] >> 8)));
//	printf(".");
//	printf(itoa((unsigned char) iap_result[1]));
	printf("\n");
	#endif

	return (unsigned int) (iap_result[1] & 0xFFFF);
}

/******************************************************************************
 * Function:	iap_Read
 *
 * Description:	This functions read the data of
 * 				__attribute ((aligned(32))) char my_bl_data[512];  
 * 				from a Flash address to buffer
 *
 * Parameters:	unsigned long addr:		Flash adress
 * Return: No
 *****************************************************************************/
void iap_Read(unsigned long address, char *buffer, int bufferLen){
	uint32_t i;
	unsigned char *ptr;
	ptr = (unsigned char*) address;
	for(i=0;i<bufferLen;i++){
		*(buffer+i)=*(ptr + i);
	}
}
/******************************************************************************
 * Function:	iap_Write
 *
 * Description:	This functions write the data of
 * 				__attribute ((aligned(32))) char my_bl_data[512];  : see iap.h
 * 				to a Flash address
 *
 * Parameters:	unsigned long addr:		Flash adress
 *
 * Return: 		0	= OK
 * 				!0	= error
 *****************************************************************************/
int iap_Write(unsigned long addr)
{
	char err;
	long buffer_vic;
	unsigned int sec;

	buffer_vic  = VICIntEnable;		// save interrupt enable
	VICIntEnClr = 0xFFFFFFFF;		// clear vic
	
	
	sec = Get_Sec_Num(addr);

//----- check sector
	// if not blank, dump sector data
	if(Blank_Check_Sector( sec, sec ) == SECTOR_NOT_BLANK)
	{
		#ifdef DEBUG
		printf("\n-- Warning: Sector ");		// print a warning
//		printf(itoa(sec));
		printf(" not blank!\n");
		#endif
	}

//---- prepare sector
	if(Prepare_Sectors( Get_Sec_Num(addr), Get_Sec_Num(addr)))
	{
		#ifdef DEBUG
		printf("\n-- ERROR: PREPARE_SECTOR_FOR_WRITE_OPERATION: ");
		#endif
		VICIntEnable = buffer_vic;		// set back ints
		return 2;
	}
	else
	{
//----- write flash
		err = Copy_RAM_To_Flash( addr, (unsigned int) my_bl_data, sizeof(my_bl_data));
		if(err)
		{
			#ifdef DEBUG
			printf("\n-- ERROR: COPY_RAM_TO_FLASH: ");
			//printf(itoa(err));
			//printf(" --");
			#endif
			VICIntEnable = buffer_vic;		// set back ints
			return 3;
		}
		else
		{
			// write operation ok?
			if(Compare(addr, (long)my_bl_data, sizeof(my_bl_data)))
			{
				#ifdef DEBUG
				printf("\n-- ERROR: COMPARE ");
				#endif
				VICIntEnable = buffer_vic;		// set back ints
				return 4;
			}
			else
			{
				#ifdef DEBUG
				printf("\nData successfully written! \n\r ");
				#endif
			  VICIntEnable = buffer_vic;		// set back ints
				return 0;
			}
		}
	}

//VICIntEnable = buffer_vic;		// set back ints
}

/******************************************************************************
 * Function:	iap_Erase_sector
 *
 * Description:	This function (prepares and) erases a sector.
 *
 * Parameters:	unsigned long addr:		Flash adress
 *
 * Return: 		0	= OK
 * 				!0	= error
 *****************************************************************************/
unsigned int iap_Erase_sector(unsigned long sec_start,unsigned long sec_stop)
{
	long buffer_vic;
	buffer_vic  = VICIntEnable;		// save interrupt enable
	VICIntEnClr = 0xFFFFFFFF;		// clear vic
	
//----- check sector
	// if blank, skip erase
	if(!Blank_Check_Sector( sec_start,sec_stop ))
	{
		#ifdef DEBUG
		printf("\n\rSector already blank!");
		#endif
		VICIntEnable = buffer_vic;		// set back ints
		return 1;
	}
//----- Prepare	sector
	if(Prepare_Sectors(sec_start,sec_stop ))
	{
		#ifdef DEBUG
		printf("\n\r-- ERROR: PREPARE_SECTOR_FOR_WRITE_OPERATION --");
		#endif
		VICIntEnable = buffer_vic;		// set back ints
		return 2;
	}
//----- erase sector
	if(Erase_Sectors( sec_start,sec_stop ))
	{
		#ifdef DEBUG
		printf("\n\r-- ERROR: ERASE SECTOR --");
		#endif
		VICIntEnable = buffer_vic;		// set back ints
		return 3;
	}
//----- check sector
	if(Blank_Check_Sector( sec_start,sec_stop ))
	{
		#ifdef DEBUG
		printf("\n\r-- ERROR: BLANK_CHECK_SECTOR --");
		#endif
		VICIntEnable = buffer_vic;		// set back ints
		return 4;
	}
	printf("check ok\r");
	#ifdef DEBUG
	printf("\n\rSector successfully erased.\n\r");
	#endif
	VICIntEnable = buffer_vic;		// set back ints
	return 0;
}
/******************************************************************************
 * Function:	iap_Erase
 *
 * Description:	This function (prepares and) erases a sector.
 *
 * Parameters:	unsigned long addr:		Flash adress
 *
 * Return: 		0	= OK
 * 				!0	= error
 *****************************************************************************/
unsigned int iap_Erase(unsigned long tmp_adr)
{
	long buffer_vic;
	buffer_vic  = VICIntEnable;		// save interrupt enable
	VICIntEnClr = 0xFFFFFFFF;		// clear vic
	
//----- check sector
	// if blank, skip erase
	if(!Blank_Check_Sector( Get_Sec_Num(tmp_adr), Get_Sec_Num(tmp_adr) ))
	{
		#ifdef DEBUG
		printf("\n\rSector already blank!");
		#endif
		VICIntEnable = buffer_vic;		// set back ints
		return 1;
	}
//----- Prepare	sector
	if(Prepare_Sectors( Get_Sec_Num(tmp_adr), Get_Sec_Num(tmp_adr) ))
	{
		#ifdef DEBUG
		printf("\n\r-- ERROR: PREPARE_SECTOR_FOR_WRITE_OPERATION --");
		#endif
		VICIntEnable = buffer_vic;		// set back ints
		return 2;
	}
//----- erase sector
	if(Erase_Sectors( Get_Sec_Num(tmp_adr), Get_Sec_Num(tmp_adr) ))
	{
		#ifdef DEBUG
		printf("\n\r-- ERROR: ERASE SECTOR --");
		#endif
		VICIntEnable = buffer_vic;		// set back ints
		return 3;
	}
//----- check sector
	if(Blank_Check_Sector( Get_Sec_Num(tmp_adr), Get_Sec_Num(tmp_adr) ))
	{
		#ifdef DEBUG
		printf("\n\r-- ERROR: BLANK_CHECK_SECTOR --");
		#endif
		VICIntEnable = buffer_vic;		// set back ints
		return 4;
	}
	printf("check ok\r");
	#ifdef DEBUG
	printf("\n\rSector successfully erased.\n\r");
	#endif
	VICIntEnable = buffer_vic;		// set back ints
	return 0;
}


/******************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

static long iap(long code, long p1, long p2, long p3, long p4)
{
	iap_command[0] = code;		// set command code
	iap_command[1] = p1;		// set 1st param
	iap_command[2] = p2;		// set 2nd param
	iap_command[3] = p3;		// set 3rd param
	iap_command[4] = p4;		// set 4th param

	//pll_off();				// disable PLL
	//IAP_Entry(iap_command, iap_result);		
	((void (*)())0x7ffffff1)(iap_command, iap_result);		// IAP entry point
	//pll_on();					// re-connect PLL
	return *iap_result;			// return status
}


/******************************************************************************
 * Function:	Read_Boot_Code_Version
 *
 * Description:	This command is used to read the boot code version number.
 * 				Command: 55, no parameters.
 *
 * Parameters:	-
 *
 * Return: 		Code CMD_SUCCESS,
 * 				Result0: 2 bytes of boot code version number. It is to be
 * 				interpreted as <byte1 (major)>.<byte0 (minor)>
 *****************************************************************************/
long Read_Boot_Code_Version(void)
{
	return iap(READ_BOOT_CODE_VERSION, 0, 0, 0, 0);
}


/******************************************************************************
 * Function:	Blank_Check_Sector
 *
 * Description:	This command is used to blank check a sector or multiple sectors
 * 				of on-chip Flash memory. To blank check a single sector use the
 * 				same "Start" and "End" sector numbers.
 * 				Command: 53
 * 				Param0:	Start Sector Number
 * 				Param1: End Sector Number (should be greater than equal to the start
 * 						sector number)
 *
 * Parameters:	long tmp_sect1:		Param0
 * 				long tmp_sect2:		Param1
 *
 * Return: 		Code 	CMD_SUCCESS |
 * 						BUSY |
 * 						SECTOR_NOT_BLANK |
 * 						INVALID_SECTOR
 * 				Result0: Offset of the first non blank word location if the status code is SECTOR_NOT_BLANK.
 * 				Result1: Contents of non blank wird location.
 *****************************************************************************/
long Blank_Check_Sector(long tmp_sect1, long tmp_sect2)
{
	return iap(BLANK_CHECK_SECTOR, tmp_sect1, tmp_sect2, 0 , 0);
}


/******************************************************************************
 * Function:	Copy_RAM_To_Flash
 *
 * Description:	This command is used to programm the flash memory. the affected should be
 * 				prepared first by calling "Prepare Sector for Write Operation" command. the
 * 				affected sectors are automatically protected again once the copy command is
 * 				successfully executed. the boot sector cannot be written by this command.
 * 				Command: 51
 * 				Param0:	(DST) Destination Flash adress where data bytes are to be written.
 * 						This address should be a 512 byte boundary.
 * 				Param1: (SRC) Source RAM adress from which data byre are to be read.
 * 				Param2:	Number of bytes to be written. Should be 512 | 1024 | 4096 | 8192.
 * 				Param3: System Clock Frequency (CCLK) in KHz.
 *
 * Parameters:	long tmp_adr_dst:	Param0
 *  			long tmp_adr_src: 	Param1
 * 				long tmp_size:		Param2
 *
 * Return: 		Code 	CMD_SUCCESS |
 * 						SRC_ADDR_ERROR (Address not on word boundary) |
 *						DST_ADDR_ERROR (Address not on correct boundary) |
 *						SRC_ADDR_NOT_MAPPED |
 *						DST_ADDR_NOT_MAPPED |
 *						COUNT_ERROR (Byte count is not 512 | 1024 | 4096 | 8192) |
 * 						SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION |
 *						BUSY
 *****************************************************************************/
long Copy_RAM_To_Flash(long tmp_adr_dst, long tmp_adr_src, long tmp_size)
{
	return iap(COPY_RAM_TO_FLASH, tmp_adr_dst, tmp_adr_src, tmp_size, _XTAL);
}


/******************************************************************************
 * Function:	Prepare_Sector
 *
 * Description:	This command must be executed before executing "Copy RAM to Flash" or "Erase Sector(s)"
 *				command. Successful execution of the "Copy RAM to Flash" or "Erase Sector(s)" command causes
 *				relevant sectors to be protected again. The boot sector can not be prepared by this command. To
 *				prepare a single sector use the same "Start" and "End" sector numbers..
 * 				Command code: 50
 *				Param0: Start Sector Number
 *				Param1: End Sector Number: Should be greater than or equal to start sector number.
 *
 * Parameters:	long tmp_sect1: 	Param0
 * 				long tmp_sect2:		Param1
 *
 * Return: 		Code 	CMD_SUCCESS |
 *						BUSY |
 *						INVALID_SECTOR
 *****************************************************************************/
long Prepare_Sectors(long tmp_sect1, long tmp_sect2)
{
	return iap(PREPARE_SECTOR_FOR_WRITE_OPERATION, tmp_sect1, tmp_sect2, 0 , 0);
}


/******************************************************************************
 * Function:	Erase_Sectors
 *
 * Description:	This command is used to erase a sector or multiple sectors of on-chip Flash memory. The boot
 *				sector can not be erased by this command. To erase a single sector use the same "Start" and "End"
 *				sector numbers.
 * 				Command code: 52
 *				Param0: Start Sector Number
 *				Param1: End Sector Number: Should be greater than or equal to start sector number.
 *				Param2: System Clock Frequency (CCLK) in KHz.
 *
 * Parameters:	long tmp_sect1: 	Param0
 * 				long tmp_sect2:		Param1
 *
 * Return: 		Code 	CMD_SUCCESS |
 *						BUSY |
 *						SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION |
 *						INVALID_SECTOR
 *****************************************************************************/
long Erase_Sectors(long tmp_sect1, long tmp_sect2)
{
	return iap(ERASE_SECTOR, tmp_sect1, tmp_sect2, _XTAL, 0);
}


/******************************************************************************
 * Function:	Compare
 *
 * Description:	This command is used to compare the memory contents at two locations. Compare result may not
 *				be correct when source or destination address contains any of the first 64 bytes starting
 *				from address zero. First 64 bytes can be re-mapped to RAM.
 * 				Command Code: 56
 *				Param0(DST): Starting Flash or RAM address from where data bytes are to be
 *								address should be a word boundary.
 *				Param1(SRC): Starting Flash or RAM address from where data bytes are to be
 *								address should be a word boundary.
 *				Param2: Number of bytes to be compared. Count should be in multiple of 4.
 *
 * Parameters:	long tmp_adr_dst
 * 				long tmp_adr_src
 * 				long tmp_size
 *
 * Return: 		Code 	CMD_SUCCESS |
 *						COMPARE_ERROR |
 * 						COUNT_ERROR (Byte count is not multiple of 4) |
 *						ADDR_ERROR |
 *						ADDR_NOT_MAPPED
 * 				Result0: Offset of the first mismatch if the Status Code is COMPARE_ERROR.
 *****************************************************************************/
long Compare(long tmp_adr_dst, long tmp_adr_src, long tmp_size)
{
	return iap(COMPARE, tmp_adr_dst, tmp_adr_src, tmp_size, 0);
}

/******************************************************************************
 * Function:	Get_Sec_Num
 *
 * Description:	Converts 'addr' to sector number
 *
 * Parameters:	addr:	Flash addr
 *
 * Return: 		sector number (unsigned int)
 *
 * Note:
 *
 * Sector table (Users Manual P. 268)
 *
 *****************************************************************************/
unsigned int Get_Sec_Num(unsigned long addr)
{
//----- sectors numbers identical for LPC213x
	if((addr >=0x00000000) && (addr <= 0x00000FFF))
	{	return 0; 	}
	if((addr >=0x00001000) && (addr <= 0x00001FFF))
	{	return 1; 	}
	if((addr >=0x00002000) && (addr <= 0x00002FFF))
	{	return 2; 	}
	if((addr >=0x00003000) && (addr <= 0x00003FFF))
	{	return 3; 	}
	if((addr >=0x00004000) && (addr <= 0x00004FFF))
	{	return 4; 	}
	if((addr >=0x00005000) && (addr <= 0x00005FFF))
	{	return 5; 	}
	if((addr >=0x00006000) && (addr <= 0x00006FFF))
	{	return 6; 	}
	if((addr >=0x00007000) && (addr <= 0x00007FFF))
	{	return 7; 	}

#ifdef USING_LPC2132
//----- sector numbers for LPC2132
	if((addr >=0x00008000) && (addr <= 0x0000FFFF))
	{	return 8; 	}
#endif
	return 0xFF;	// error
}





void pll_off(void)
{
	PLLCON = PLLCON_PLLD;	// PLL disable
	pll_feed();				// Update PLL with feed sequence
}

void pll_on(void)
{
	PLLCON = PLLCON_PLLE;					// PLL enable
	pll_feed();								// Update PLL with feed sequence
	while((PLLSTAT & PLLSTAT_PLOCK) == 0);	// Test Lock bit
	PLLCON = PLLCON_PLLC;					// Connect the PLL
	pll_feed();								// Update PLL with feed sequence
}

static void pll_feed(void)
{
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
}

