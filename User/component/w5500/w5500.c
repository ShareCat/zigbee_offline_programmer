/**
******************************************************************************
* @file         W5500.c
* @author       WIZnet Software Team
* @version      V1.0
* @date         2015-02-14
* @brief        ¶ÁÐ´W5500¼Ä´æÆ÷µ×²ãº¯Êý
******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include "w5500.h"
#include "w5500_conf.h"
#include "w5500_socket.h"

#include "./../../sys/sys_command_line.h"


#define DEBUG_W5500 FALSE           /* µ÷ÊÔ¿ª¹Ø */


uint16_t SSIZE[MAX_SOCK_NUM] = {0, 0, 0, 0, 0, 0, 0, 0}; // Max Tx buffer
uint16_t RSIZE[MAX_SOCK_NUM] = {0, 0, 0, 0, 0, 0, 0, 0}; // Max Rx buffer
uint8_t txsize[MAX_SOCK_NUM] = {2, 2, 2, 2, 2, 2, 2, 2}; // tx buffer set K bits
uint8_t rxsize[MAX_SOCK_NUM] = {2, 2, 2, 2, 2, 2, 2, 2}; // rx buffet set K bits

/**
*@brief     This function is to get the Max size to receive.
*@param     s: socket number
*@return    This function return the RX buffer size has been setted
*/
void iinchip_init(void)
{
    setMR(MR_RST);
    //printf("MR value is %02x \r\n", IINCHIP_READ_COMMON(MR));
}

/**
*@brief     This function is to get the Max size to receive.
*@param     s: socket number
*@return    This function return the RX buffer size has been setted
*/
uint16_t getIINCHIP_RxMAX(SOCKET s)
{
    return RSIZE[s];
}

/**
*@brief     This function is to get the Max size to receive.
*@param     s: socket number
*@return    This function return the RX buffer size has been setted
*/
uint16_t getIINCHIP_TxMAX(SOCKET s)
{
    return SSIZE[s];
}

/**
*@brief     This function is to set up gateway IP address.
*@param     addr: a pointer to a 4 -byte array responsible to set the Gateway IP address
*@return    None
*/
void setGAR(uint8_t * addr)
{
    wiz_write_buf(GAR0, addr, 4);
}

/**
*@brief     This function is to get gateway IP address.
*@param     addr: a pointer to a 4 -byte array responsible to get the Gateway IP address
*@return    None
*/
void getGAR(uint8_t * addr)
{
    wiz_read_buf(GAR0, addr, 4);
}

/**
*@brief     This function is to set up SubnetMask address
*@param     addr: a pointer to a 4 -byte array responsible to set the subway IP address.
*@return    None
*/
void setSUBR(uint8_t * addr)
{
    wiz_write_buf(SUBR0, addr, 4);
}
/**
*@brief     This function is to set up MAC address.
*@param     addr: a pointer to a 6 -byte array responsible to set the MAC address.
*@return    None
*/
void setSHAR(uint8_t * addr)
{
    wiz_write_buf(SHAR0, addr, 6);
}

/**
*@brief     This function is to set up Source IP address.
*@param     addr:a pointer to a 4 -byte array responsible to set the Source IP addres.
*@return    None
*/
void setSIPR(uint8_t * addr)
{
    wiz_write_buf(SIPR0, addr, 4);
}

/**
*@brief     This function is to get Subnet mask.
*@param     addr:a pointer to a 4 -byte array responsible to set the Subnet mask.
*@return    None
*/
void getSUBR(uint8_t * addr)
{
    wiz_read_buf(SUBR0, addr, 4);
}

/**
*@brief     This function is to get up Source MAC .
*@param     addr: a pointer to a 6 -byte array responsible to get the MAC
*@return    None
*/
void getSHAR(uint8_t * addr)
{
    wiz_read_buf(SHAR0, addr, 6);
}

/**
*@brief     This function is to get up Source IP .
*@param     addr: a pointer to a 4 -byte array responsible to get the Source IP
*@return    None
*/
void getSIPR(uint8_t * addr)
{
    wiz_read_buf(SIPR0, addr, 4);
}
/**
*@brief     This function is to set the MR register.
*@param     val: the value to set to MR
*@return    None
*/
void setMR(uint8_t val)
{
    IINCHIP_WRITE(MR, val);
}

/**
*@brief     This function is to get Interrupt register in common register.
*@param     None
*@return    The value read from the IR register
*/
uint8_t getIR(void)
{
    return IINCHIP_READ(IR);
}

/**
@brief      This function is to set up Retransmission time.
                    If there is no response from the peer or delay in response then retransmission
                    will be there as per RTR (Retry Time-value Register)setting
*@param     timeout: The value write to  the RTR0 register
*@return    None
*/
void setRTR(uint16_t timeout)
{
    IINCHIP_WRITE(RTR0, (uint8_t)((timeout & 0xff00) >> 8));
    IINCHIP_WRITE(RTR1, (uint8_t)(timeout & 0x00ff));
}

/**
@brief      This function is to set the number of Retransmission.
                    If there is no response from the peer or delay in response then recorded time
                    as per RTR & RCR register seeting then time out will occur.
*@param     retry: Times to  retry
*@return    None
*/
void setRCR(uint8_t retry)
{
    IINCHIP_WRITE(WIZ_RCR, retry);
}

/**
*@brief     This function is to the interrupt mask Enable/Disable appropriate Interrupt. ('1' : interrupt enable)
                    If any bit in IMR is set as '0' then there is not interrupt signal though the bit is
                    set in IR register.
*@param     mask: the bits to clear
*@return    None
*/
void clearIR(uint8_t mask)
{
    IINCHIP_WRITE(IR, ~mask | getIR());
}

/**
*@brief     This function is to set the maximum segment size of TCP in Active Mode), while in Passive Mode this is set by peer
*@param     s: socket number
*@param     Sn_MSSR: the maximum segment size
*@return    None
*/
void setSn_MSS(SOCKET s, uint16_t Sn_MSSR)
{
    IINCHIP_WRITE(Sn_MSSR0(s), (uint8_t)((Sn_MSSR & 0xff00) >> 8));
    IINCHIP_WRITE(Sn_MSSR1(s), (uint8_t)(Sn_MSSR & 0x00ff));
}

/**
*@brief     This function is to set the IP Time to live(TTL) Register
*@param     s: socket number
*@param     Sn_MSSR: the IP Time to live
*@return    None
*/
void setSn_TTL(SOCKET s, uint8_t ttl)
{
    IINCHIP_WRITE(Sn_TTL(s), ttl);
}

/**
*@brief     This function is to read the Interrupt & Soket Status registe
*@param     s: socket number
*@return    socket interrupt status
*/
uint8_t getSn_IR(SOCKET s)
{
    return IINCHIP_READ(Sn_IR(s));
}

/**
*@brief     This function is to write the Interrupt & Soket Status register to clear the interrupt
*@param     s: socket number
*@return  socket interrupt status
*/
void setSn_IR(uint8_t s, uint8_t val)
{
    IINCHIP_WRITE(Sn_IR(s), val);
}

/**
*@brief     This function is to get socket status
*@param     s: socket number
*@return  socket status
*/
uint8_t getSn_SR(SOCKET s)
{
    return IINCHIP_READ(Sn_SR(s));
}

/**
*@brief     This fuction is to get socket TX free buf size
                    This gives free buffer size of transmit buffer. This is the data size that user can transmit.
                    User shuold check this value first and control the size of transmitting data
*@param     s: socket number
*@return  socket TX free buf size
*/
uint16_t getSn_TX_FSR(SOCKET s)
{
    uint16_t val = 0, val1 = 0;

    do {
        val1 = IINCHIP_READ(Sn_TX_FSR0(s));
        val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR1(s));

        if(val1 != 0) {
            val = IINCHIP_READ(Sn_TX_FSR0(s));
            val = (val << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
        }
    } while(val != val1);

    return val;
}

/**
*@brief     This fuction is to give size of received data in receive buffer.
*@param     s: socket number
*@return  socket TX free buf size
*/
uint16_t getSn_RX_RSR(SOCKET s)
{
    uint16_t val = 0, val1 = 0;

    do {
        val1 = IINCHIP_READ(Sn_RX_RSR0(s));
        val1 = (val1 << 8) + IINCHIP_READ(Sn_RX_RSR1(s));

        if(val1 != 0) {
            val = IINCHIP_READ(Sn_RX_RSR0(s));
            val = (val << 8) + IINCHIP_READ(Sn_RX_RSR1(s));
        }
    } while(val != val1);

    return val;
}

/**
*@brief   This function is being called by send() and sendto() function also.

                    This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
                    register. User should read upper byte first and lower byte later to get proper value.
*@param     s: socket number
*@param     data: data buffer to send
*@param     len: data length
*@return  socket TX free buf size
*/
void send_data_processing(SOCKET s, uint8_t *data, uint16_t len)
{
    uint16_t ptr = 0;
    uint32_t addrbsb = 0;

    if(len == 0) {
#if DEBUG_W5500
        LOGA("CH: %d Unexpected1 length 0\r\n", s);
#endif
        return;
    }

    ptr = IINCHIP_READ(Sn_TX_WR0(s));
    ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR1(s));

    addrbsb = (uint32_t)(ptr << 8) + (s << 5) + 0x10;
    wiz_write_buf(addrbsb, data, len);

    ptr += len;
    IINCHIP_WRITE(Sn_TX_WR0(s), (uint8_t)((ptr & 0xff00) >> 8));
    IINCHIP_WRITE(Sn_TX_WR1(s), (uint8_t)(ptr & 0x00ff));
}

/**
*@brief     This function is being called by recv() also.
                    This function read the Rx read pointer register
                    and after copy the data from receive buffer update the Rx write pointer register.
                    User should read upper byte first and lower byte later to get proper value.
*@param     s: socket number
*@param     data: data buffer to receive
*@param     len: data length
*@return  None
*/
void recv_data_processing(SOCKET s, uint8_t *data, uint16_t len)
{
    uint16_t ptr = 0;
    uint32_t addrbsb = 0;

    if(len == 0) {
#if DEBUG_W5500
        LOGA("CH: %d Unexpected2 length 0\r\n", s);
#endif
        return;
    }

    ptr = IINCHIP_READ(Sn_RX_RD0(s));
    ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD1(s));

    addrbsb = (uint32_t)(ptr << 8) + (s << 5) + 0x18;
    wiz_read_buf(addrbsb, data, len);
    ptr += len;

    IINCHIP_WRITE(Sn_RX_RD0(s), (uint8_t)((ptr & 0xff00) >> 8));
    IINCHIP_WRITE(Sn_RX_RD1(s), (uint8_t)(ptr & 0x00ff));
}

/**
*@brief     This function set the transmit & receive buffer size as per the channels is used
*@Note:     TMSR and RMSR bits are as follows\n
                    Maximum memory size for Tx, Rx in the W5500 is 16K Bytes,\n
                    In the range of 16KBytes, the memory size could be allocated dynamically by each channel.\n
                    Be attentive to sum of memory size shouldn't exceed 8Kbytes\n
                    and to data transmission and receiption from non-allocated channel may cause some problems.\n
                    If the 16KBytes memory is already  assigned to centain channel, \n
                    other 3 channels couldn't be used, for there's no available memory.\n
                    If two 4KBytes memory are assigned to two each channels, \n
                    other 2 channels couldn't be used, for there's no available memory.\n
*@param     tx_size: tx buffer size to set=tx_size[s]*(1024)
*@param     rx_size: rx buffer size to set=rx_size[s]*(1024)
*@return    None
*/
void socket_buf_init(uint8_t * tx_size, uint8_t * rx_size)
{
    int16_t i;
    int16_t ssum = 0, rsum = 0;

    for(i = 0 ; i < MAX_SOCK_NUM; i++) {      // Set the size, masking and base address of Tx & Rx memory by each channel
        IINCHIP_WRITE((Sn_TXMEM_SIZE(i)), tx_size[i]);
        IINCHIP_WRITE((Sn_RXMEM_SIZE(i)), rx_size[i]);

#if DEBUG_W5500
        LOGA("tx_size[%d]: %d, Sn_TXMEM_SIZE = %d\r\n", i, tx_size[i], IINCHIP_READ(Sn_TXMEM_SIZE(i)));
        LOGA("rx_size[%d]: %d, Sn_RXMEM_SIZE = %d\r\n", i, rx_size[i], IINCHIP_READ(Sn_RXMEM_SIZE(i)));
#endif

        SSIZE[i] = (int16_t)(0);
        RSIZE[i] = (int16_t)(0);

        if(ssum <= 16384) {
            SSIZE[i] = (int16_t)tx_size[i] * (1024);
        }

        if(rsum <= 16384) {
            RSIZE[i] = (int16_t)rx_size[i] * (1024);
        }

        ssum += SSIZE[i];
        rsum += RSIZE[i];

    }
}


