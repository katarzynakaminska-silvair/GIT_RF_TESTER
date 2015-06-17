#include "flasher.h"

void programmer_SPI_Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	/* Peripheral Clock Enable -------------------------------------------------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);

	GPIO_PinAFConfig(GPIO_N, DD , GPIO_AF_SPI4);
	GPIO_PinAFConfig(GPIO_N, DC , GPIO_AF_SPI4);

	GPIO_InitStructure.GPIO_Pin =  DC  | DD  ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed =   GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_Init( GPIOE, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//
//	GPIO_Init( GPIOA, &GPIO_InitStructure);


	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Rx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //NOT SURE
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI4, &SPI_InitStructure);
//	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	SPI_Cmd(SPI4, ENABLE);
/**************************************************************************//**
* @brief    Initializes the programmer by switching  32 MHz clk and
*           configuring I/O.
******************************************************************************/
void programmer_init(void)
{
	//switch on clock
//init structure for RESET_N - output high
//DD - output low
//DC - output low

	GPIO_ResetBits(GPIO_N, RESET_N);

}

/**************************************************************************//**
* @brief    Writes a byte on the debug interface. Requires DD to be
*           output when function is called.
******************************************************************************/
void programmer_write_debug_byte(unsigned char data)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        // Set clock high and put data on DD line
    	GPIO_SetBits(RESET_N,GPIO_N);
        (data & 0x80) == 0x80 ? GPIO_SetBits(GPIO_N,DC|DD) : GPIO_SetBits(GPIO_N,DC);
        data <<= 1;
        GPIO_ResetBits(GPIO_N, DC);; // set clock low (DUP capture flank)
    }
}
/**************************************************************************//**
* @brief    Reads a byte from the debug interface. Requires DD to be
*           input when function is called.
******************************************************************************/
unsigned char programmer_read_debug_byte(void)
{
    unsigned char i;
    unsigned char data;
    for (i = 0; i < 8; i++)
    {
    	GPIO_SetBits(GPIO_N,DC);
        data <<= 1;
        data |= GPIO_ReadInputDataBit(GPIO_N,DD);  // Read DD line
        GPIO_ResetBits(GPIO_N,DC);     // DC low
    }
    return data;
}
/**************************************************************************//**
* @brief    Function waits for DUP to indicate that it is ready. The DUP will
*           pulls DD line low when it is ready. Requires DD to be input when
*           function is called.
* @return   Returns 0 if function timed out waiting for DD line to go low
* @return   Returns 1 when DUP has indicated it is ready.
******************************************************************************/
unsigned char programmer_wait_dup_ready(void)
{
    // DUP pulls DD low when ready
    unsigned char count = 0;
    while (GPIO_ReadInputDataBit(GPIO_N,DD) && count < 16)
    {
        // Clock out 8 bits before checking if DD is low again
    	programmer_read_debug_byte();
        count++;
    }
    return (count == 16) ? 0 : 1;
}
/**************************************************************************//**
* @brief    Issues a command on the debug interface. Only commands that return
*           one output byte are supported.
* @param    cmd             Command byte
* @param    cmd_bytes       Pointer to the array of data bytes following the
*                           command byte [0-3]
* @param    num_cmd_bytes   The number of data bytes (input to DUP) [0-3]
* @return   Data returned by command
******************************************************************************/
unsigned char programmer_debug_command(unsigned char cmd, unsigned char *cmd_bytes,unsigned short num_cmd_bytes)
{
    unsigned short i;
    unsigned char output = 0;

    // Make sure DD is output
    SET_DD_OUTPUT();

    // Send command
    programmer_write_debug_byte(cmd);

    // Send bytes
    for (i = 0; i < num_cmd_bytes; i++)
    {
    	programmer_write_debug_byte(cmd_bytes[i]);
    }

    // Set DD as input
    SET_DD_INPUT();

    // Wait for data to be ready
    programmer_wait_dup_ready();

    // Read returned byte
    output = programmer_read_debug_byte();

    // Set DD as output
    SET_DD_OUTPUT();

    return output;
}
/**************************************************************************//**
* @brief    Resets the DUP into debug mode. Function assumes that
*           the programmer I/O has already been configured using e.g.
*           programmer_init().
******************************************************************************/
void programmer_debug_init(void)
{
    volatile unsigned char i;

	// Send two flanks on DC while keeping RESET_N low
	GPIO_ResetBits(GPIO_N, RESET_N | DD | DC);	// All low (incl. RESET_N)
	for (i = 0; i < 25; i++);   				// Wait
	GPIO_SetBits(GPIO_N, DC);					// DC high
	GPIO_ResetBits(GPIO_N, DC);					// DC low
	GPIO_SetBits(GPIO_N, DC);					// DC high
	GPIO_ResetBits(GPIO_N, DC);					// DC low
	for (i = 0; i < 25; i++);   				// Wait
	GPIO_ResetBits(GPIO_N, RESET_N);			// Release RESET_N



}
/**************************************************************************//**
* @brief    Reads the chip ID over the debug interface using the
*           GET_CHIP_ID command.
* @return   Returns the chip id returned by the DUP
******************************************************************************/
unsigned char programmer_read_chip_id(void)
{
    unsigned char id = 0;

    // Make sure DD is output
    SET_DD_OUTPUT();

    // Send command
    programmer_write_debug_byte(CMD_GET_CHIP_ID);

    // Set DD as input
    SET_DD_INPUT();

    // Wait for data to be ready
    programmer_wait_dup_ready();

    // Read ID and revision
    id = programmer_read_debug_byte(); // ID
    programmer_read_debug_byte();      // Revision (discard)

    // Set DD as output
    SET_DD_OUTPUT();

    return id;
}
/**************************************************************************//**
* @brief    Sends a block of data over the debug interface using the
*           BURST_WRITE command.
* @param    src         Pointer to the array of input bytes
* @param    num_bytes   The number of input bytes

******************************************************************************/
void programmer_burst_write_block(unsigned char *src, unsigned short num_bytes)
{

}
/**************************************************************************//**
* @brief    Issues a CHIP_ERASE command on the debug interface and waits for it
*           to complete.
******************************************************************************/
void programmer_chip_erase(void)
{

}
/**************************************************************************//**
* @brief    Writes a block of data to the DUP's XDATA space.
*
* @param    address     XDATA start address
* @param    values      Pointer to the array of bytes to write
* @param    num_bytes   Number of bytes to write
******************************************************************************/
void programmer_write_xdata_memory_block(unsigned short address, const unsigned char *values, unsigned short num_bytes)
{


}
/**************************************************************************//**
* @brief    Writes a byte to a specific address in the DUP's XDATA space.
* @param    address     XDATA address
* @param    value       Value to write
******************************************************************************/
void programmer_write_xdata_memory(unsigned short address, unsigned char value)
{

}
/**************************************************************************//**
* @brief    Read a byte from a specific address in the DUP's XDATA space.
*
* @param    address     XDATA address
*
* @return   Value read from XDATA
******************************************************************************/
unsigned char programmer_read_xdata_memory(unsigned short address)
{

}

/**************************************************************************//**
* @brief    Reads 1-32767 bytes from DUP's flash to a given buffer on the
*           programmer.
* @param    bank        Flash bank to read from [0-7]
* @param    address     Flash memory start address [0x0000 - 0x7FFF]
* @param    values      Pointer to destination buffer.
******************************************************************************/
void read_flash_memory_block(unsigned char bank,unsigned short flash_addr, unsigned short num_bytes, unsigned char *values)
{

}
/**************************************************************************//**
* @brief    Writes 4-2048 bytes to DUP's flash memory. Parameter \c num_bytes
*           must be a multiple of 4.
* @param    src         Pointer to programmer's source buffer (in XDATA space)
* @param    start_addr  FLASH memory start address [0x0000 - 0x7FFF]
* @param    num_bytes   Number of bytes to transfer [4-1024]
******************************************************************************/
void write_flash_memory_block(unsigned char *src, unsigned long start_addr,
                              unsigned short num_bytes)
{
    // 1. Write the 2 DMA descriptors to RAM
    write_xdata_memory_block(ADDR_DMA_DESC_0, dma_desc_0, 8);
    write_xdata_memory_block(ADDR_DMA_DESC_1, dma_desc_1, 8);

    // 2. Update LEN value in DUP's DMA descriptors
    unsigned char len[2] = {HIBYTE(num_bytes), LOBYTE(num_bytes)};
    write_xdata_memory_block((ADDR_DMA_DESC_0+4), len, 2);  // LEN, DBG => ram
    write_xdata_memory_block((ADDR_DMA_DESC_1+4), len, 2);  // LEN, ram => flash

    // 3. Set DMA controller pointer to the DMA descriptors
    write_xdata_memory(DUP_DMA0CFGH, HIBYTE(ADDR_DMA_DESC_0));
    write_xdata_memory(DUP_DMA0CFGL, LOBYTE(ADDR_DMA_DESC_0));
    write_xdata_memory(DUP_DMA1CFGH, HIBYTE(ADDR_DMA_DESC_1));
    write_xdata_memory(DUP_DMA1CFGL, LOBYTE(ADDR_DMA_DESC_1));

    // 4. Set Flash controller start address (wants 16MSb of 18 bit address)
    write_xdata_memory(DUP_FADDRH, HIBYTE( (start_addr>>2) ));
    write_xdata_memory(DUP_FADDRL, LOBYTE( (start_addr>>2) ));

    // 5. Arm DBG=>buffer DMA channel and start burst write
    write_xdata_memory(DUP_DMAARM, CH_DBG_TO_BUF0);
    burst_write_block(src, num_bytes);

    // 6. Start programming: buffer to flash
    write_xdata_memory(DUP_DMAARM, CH_BUF0_TO_FLASH);
    write_xdata_memory(DUP_FCTL, 0x06);

    // 7. Wait until flash controller is done
    while (read_xdata_memory(XREG_TO_INT(FCTL)) & 0x80);
}
