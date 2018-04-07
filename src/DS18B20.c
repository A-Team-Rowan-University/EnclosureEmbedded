#include "DS18B20.h"
#include "delay.h"
#include <msp430g2553.h>

static void InitDS18B20(void);
static unsigned int ResetDS18B20(void);
static void WriteZero(void);
static void WriteOne(void);
static unsigned int ReadBit (void);
static void WriteByte(char byte);
static uint16_t ReadDS18B20(void);

void InitDS18B20(void){
    //Nothing for now
    //maybe reconfigure sensor resolution?
}

unsigned int ResetDS18B20(void){
    /* Steps to reset one wire bus
     * Pull bus low
     * hold condition for 480us
     * release bus
     * wait for 60us
     * read bus
     * if bus low then device present set / return var accordingly
     * wait for balance period (480-60)
     */
    int device_present=0;
    DS18B20_LO();                                // Drive bus low
    DELAY_US (480);                             // hold for 480us
    DS18B20_DIR &= ~DS18B20_DATA_IN_PIN;        //release bus. set port in input mode
    DELAY_US (240);                             //wait for 480us
    if(DS18B20_IN & DS18B20_DATA_IN_PIN)
    {
        return device_present=1;
    }
    else
    {
        return device_present= 0;
    }

}

void DS18B20_LO(void){
    DS18B20_DIR |= DS18B20_DATA_IN_PIN; //set port as output
    DS18B20_OUT &= ~DS18B20_DATA_IN_PIN;   //set port low
}

void DS18B20_HI(void){

    DS18B20_DIR |= DS18B20_DATA_IN_PIN; //set port as output
    DS18B20_OUT |= DS18B20_DATA_IN_PIN; //set port high
}

inline void WriteZero(void){
    /*Steps for master to transmit logical zero to slave device on bus
     * pull bus low
     * hold for 60us
     * release bus
     * wait for 1us for recovery
     */

    DS18B20_LO();                                // Drive bus low
    DELAY_US (60);                              //sample time slot for the slave
    DS18B20_DIR &= ~DS18B20_DATA_IN_PIN;          //release bus. set port in input mode
    DELAY_US (1);                               //recovery time slot
}

inline void WriteOne(void){
    /*Steps for master to transmit logical one to slave device on bus
     * pull bus low
     * hold for 5us
     * release bus
     * wait for 1us for recovery
     */
    DS18B20_LO();                                // Drive bus high
    DELAY_US (5);
    DS18B20_DIR &= ~DS18B20_DATA_IN_PIN;          //release bus. set port in input mode
    DELAY_US (55);                              //sample time slot for the slave
    DELAY_US (1);                               //recovery time slot

}

unsigned int ReadBit (void)
{

    /*Steps for master to issue a read request to slave device on bus aka milk slave device
     * pull bus low
     * hold for 5us
     * release bus
     * wait for 45us for recovery
     */
    int bit=0;
    DS18B20_LO();                                // Drive bus low
    DELAY_US (5);                               //hold for 5us
    DS18B20_DIR &= ~DS18B20_DATA_IN_PIN;          //release bus. set port in input mode, P2.4
    DELAY_US (10);                              //wait for slave to drive port either high or low
    if(DS18B20_IN & DS18B20_DATA_IN_PIN)          //read bus P2.4
    {
        bit=1;                                  //if read high set bit high
    }
    DELAY_US (45);                              //recovery time slot
    return bit;

}

void WriteByte(char byte){
    unsigned char i;
    for(i=8;i>0;i--)
    {
        if(byte & 0x01){
            WriteOne();
        }else{
            WriteZero();
        }
        byte >>=1;
    }
}

uint16_t ReadDS18B20(void){
    unsigned char i;
    unsigned int data=0;
    DS18B20_DIR &= ~DS18B20_DATA_IN_PIN;          //release bus. set port in input mode

     for(i=16;i>0;i--)
    {
        data>>=1;                                 //replaced 1 with i1
        if(ReadBit())
        {
            data |= 0x8000;
        }
    }
    return(data);
}


float GetData(void){
    unsigned int temp = 0;
    ResetDS18B20();
    // 4.5 ms delay
    WriteByte(DS18B20_SKIP_ROM);
    WriteByte(DS18B20_CONVERT_T);
    DELAY_MS(750);
    ResetDS18B20();
    WriteByte(DS18B20_SKIP_ROM);
    WriteByte(DS18B20_READ_SCRATCHPAD);
    temp = ReadDS18B20();
    ResetDS18B20();
    if(temp<0x8000)     
    {
        temp = temp*0.0625;
        return(temp);
    }
    else                     
    {
        temp=(~temp)+1;
        temp = temp*0.0625;
        return(temp);
    }    
}
