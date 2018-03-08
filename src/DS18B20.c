#include "DS18B20.h"
#include "delay.h"


static void InitDS18B20(void){
	//Nothing for now
    //maybe reconfigure sensor resolution?
}

static unsigned int ResetDevice(void){

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
    DS1820_LO();         						// Drive bus low
    DELAY_US (480);                             // hold for 480us
    DS1820_DIR &= ~DS1820_DATA_IN_PIN;			//release bus. set port in input mode
    if(DS1820_IN & DS1820_DATA_IN_PIN)
	{
		device_present=0;
	}
    DELAY_US (480);								//wait for 480us
  	return device_present;

}

static void onewire_low(void){
    DS1820_DIR |= DS1820_DATA_IN_PIN; //set port as output
	DS1820_OUT |= DS1820_DATA_IN_PIN;	//set port high
}

static void onewire_high(void){

    DS1820_DIR|=DS1820_DATA_IN_PIN; //set port as output
	DS1820_OUT|=DS1820_DATA_IN_PIN;	//set port high
}

static inline void onewire_writezero(void){
    /*Steps for master to transmit logical zero to slave device on bus
	 * pull bus low
	 * hold for 60us
	 * release bus
	 * wait for 1us for recovery 
	 */ 
	
	DS1820_LO();         						// Drive bus low
	DELAY_US (60);								//sample time slot for the slave
	DS1820_DIR &= ~DS1820_DATA_IN_PIN;			//release bus. set port in input mode
    DELAY_US (1);								//recovery time slot
	
}

static inline void onewire_writeone(void){
    /*Steps for master to transmit logical one to slave device on bus
	 * pull bus low
	 * hold for 5us
	 * release bus
	 * wait for 1us for recovery 
	 */ 
	DS1820_LO();         						// Drive bus low
	DELAY_US (5);  
	DS1820_DIR &= ~DS1820_DATA_IN_PIN;			//release bus. set port in input mode
    DELAY_US (55);								//sample time slot for the slave
    DELAY_US (1);								//recovery time slot

}

static void WriteByte(char byte){
    unsigned char i;
	for(i=8;i>0;i--)
    {
        if(byte){
            WriteOne();
        }else{
        	WriteZero();
        }
          	
		byte >>=1;
	
    }
}

static uint16_t ReadData(void){
    unsigned char i;
 	unsigned int data=0;
	DS1820_DIR &= ~DS1820_DATA_IN_PIN;			//release bus. set port in input mode
 	
 	 for(i=16;i>0;i--)
 	{
		data>>=1;
		if(ReadBit())
		{
			data |=0x8000;
		}
	}
	return(data);
}

float GetData(void)
{
    unsigned int temp;
  	ResetDS1820();
    WriteDS1820(DS1820_SKIP_ROM,0);
	WriteDS1820(DS1820_CONVERT_T,1);
    DELAY_MS(750);
    ResetDS1820();
    WriteDS1820(DS1820_SKIP_ROM,0);
    WriteDS1820(DS1820_READ_SCRATCHPAD,0);
    temp = ReadDS1820();
    if(temp<0x8000)     
    {
    	
        return(temp*0.0625);
    }
    else                     
    {
        temp=(~temp)+1;
        return(temp*0.0625);
    }    
	
}