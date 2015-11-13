#ifndef IAP_H_
#define IAP_H_



/* P U B L I C   F U N C T I O N   P R O T O T Y P E S */
unsigned int 	iap_BootLoaderVersion(void);

unsigned int iap_Erase_sector(unsigned long sec_start,unsigned long sec_stop);

unsigned int 	iap_Erase(unsigned long tmp_adr);

void iap_Read(unsigned long address, char *buffer, int bufferLen);

int iap_Write(unsigned long addr);

#endif /*IAP_H_*/
