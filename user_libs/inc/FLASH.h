#ifndef __FLASH_H_
#define __FLASH_H_

#include "stm32f4xx_conf.h"
#include "LED.h"

typedef struct
{
	float reglin_a;
	float reglin_b;
	float attenuation;
	uint8_t is_clibrated;
	uint8_t is_serialnum_set;
	float ADC_temperature;
	float ADC_ref_voltage;
	float ADC_detector_temp;
	float VCC;

}Cali_reglin;

extern Cali_reglin Cali_reglin_inst;
/* Base address of the Flash sectors */
#define FLASH_ADDR_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define FLASH_ADDR_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define FLASH_ADDR_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define FLASH_ADDR_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define FLASH_ADDR_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define FLASH_ADDR_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define FLASH_ADDR_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define FLASH_ADDR_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define FLASH_ADDR_SERIALNUM	((uint32_t)0x08070000) /* Base @ of Sector 7, 128 Kbytes */


inline uint32_t FLASH_read(uint32_t Address);

uint8_t FLASH_write(uint32_t FLASH_Sector, uint32_t Address, uint32_t* Data, uint32_t number_of_words_to_write);

void FLASH_read_reglin(Cali_reglin* Cali_reglin_struct);
uint8_t FLASH_write_reglin(Cali_reglin* Cali_reglin_struct);
inline uint32_t FLASH_read_serialnum(void);
uint8_t FLASH_write_serialnum(uint32_t serialnum);

#endif
