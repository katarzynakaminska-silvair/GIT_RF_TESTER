#include "FLASH.h"

Cali_reglin Cali_reglin_inst;

uint8_t FLASH_write(uint32_t FLASH_Sector, uint32_t Address, uint32_t* Data, uint32_t number_of_words_to_write)
{
	int i;
	uint32_t UINT32Data;
	  /* Unlock the Flash to enable the flash control register access *************/
	  FLASH_Unlock();

	  FLASH_ClearFlag(FLASH_FLAG_EOP| FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
              FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	  //erase the entire page before you can write
	    if (FLASH_EraseSector(FLASH_Sector, VoltageRange_3) != FLASH_COMPLETE)
	    {
	      /* Error occurred while sector erase.*/
	    	return 0;
	    }

	    for(i=0; i<number_of_words_to_write; i++)
	    {
	    	UINT32Data = *(Data+i);
	      if (FLASH_ProgramWord(Address, UINT32Data) == FLASH_COMPLETE)
	      {
	        Address = Address + 4;
	      }
	      else
	      {
	        /* Error occurred while writing data in Flash memory.*/
	    	  return 0;
	      }
	    }

	    FLASH_Lock();
	    return 1;
}


inline uint32_t FLASH_read(uint32_t Address)
{
	uint32_t data32;
	data32 = *(__IO uint32_t*)Address;
	return data32;
}

void FLASH_read_reglin(Cali_reglin* Cali_reglin_struct)
{
	uint32_t temp;
	float ftemp;

	temp = FLASH_read(FLASH_ADDR_SECTOR_7);
	Cali_reglin_struct->reglin_a = *(float*)&temp;
	temp = FLASH_read(FLASH_ADDR_SECTOR_7+4);
	Cali_reglin_struct->reglin_b = *(float*)&temp;
	temp = FLASH_read(FLASH_ADDR_SECTOR_7+8);
	ftemp = *(float*)&temp;
	Cali_reglin_struct->attenuation = *(float*)&temp;
	temp = FLASH_read(FLASH_ADDR_SECTOR_7+12);
	ftemp = *(float*)&temp;
	temp = (uint32_t)ftemp;
	Cali_reglin_struct->is_clibrated = (uint8_t)temp;
	temp = FLASH_read(FLASH_ADDR_SECTOR_7+16);
	Cali_reglin_struct->VCC = *(float*)&temp;
	temp = FLASH_read(FLASH_ADDR_SECTOR_7+20);
	ftemp = *(float*)&temp;
	temp = (uint32_t)ftemp;
	Cali_reglin_struct->is_serialnum_set = (uint8_t)temp;
}

uint8_t FLASH_write_reglin(Cali_reglin* Cali_reglin_struct)
{
	float temptab[6] = {Cali_reglin_struct->reglin_a,Cali_reglin_struct->reglin_b, Cali_reglin_struct->attenuation, Cali_reglin_struct->is_clibrated, Cali_reglin_struct->VCC, Cali_reglin_struct->is_serialnum_set};
	if(FLASH_write(FLASH_Sector_7, FLASH_ADDR_SECTOR_7, temptab, 6)) return 1;
	else return 0;
}

inline uint32_t FLASH_read_serialnum(void)
{
	return FLASH_read(0x1FFF7800);//FLASH_read(FLASH_ADDR_SERIALNUM);
}
uint8_t FLASH_write_serialnum(uint32_t serialnum)
{
	LED_on(LED_BLUE);
	if(Cali_reglin_inst.is_serialnum_set != 1)
	{
		  FLASH_Unlock();

		  FLASH_ClearFlag(FLASH_FLAG_EOP| FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
				FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

		  // set serialnum
		FLASH_ProgramWord(0x1FFF7800, serialnum);
		//set block bit for OTP 0 to 0x00 in order to block OTP area 0 from rewrite
		FLASH_ProgramWord( 0x1FFF7A00, 0xFFFFFF00 );
		 FLASH_Lock();
		 Cali_reglin_inst.is_serialnum_set = 1;
		 FLASH_write_reglin(&Cali_reglin_inst);

		 LED_off(LED_BLUE);
		 return 1;
	 }
	 else
	{
		LED_off(LED_BLUE);
		return 0;
	}
}

