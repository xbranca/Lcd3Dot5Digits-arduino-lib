#include "Lcd3Dot5Digits.h"

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//////////////////////////Alias defintions ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

//Defined HT1621's command see datasheet "command summary" table page 18 
#define  Sys_en     0x02  //Turn on system oscillator "SYS EN" 1000 0000 0010 
#define  RCosc      0x30  //on-chip RC oscillator "RC 256K" (Power-on default) 1000 0011 0000 
#define  ComMode    0x52  //4COM,1/3bias "BIAS 1/3" 1000 0101 0010  
#define  LCD_on     0x06  //Turn on LCD bias
#define  LCD_off    0x04  //Turn off LCD bias
#define  Sys_dis    0x00  //Turn off both system oscillator and LCd bias 1000   0000 0010 

//Alias for communication with HT1621
#define CS1    digitalWrite(m_CS, HIGH) 
#define CS0    digitalWrite(m_CS, LOW)
#define WR1    digitalWrite(m_WR, HIGH) 
#define WR0    digitalWrite(m_WR, LOW)
#define DATA1  digitalWrite(m_DATA, HIGH) 
#define DATA0  digitalWrite(m_DATA, LOW)

//Mode setup sent before data or command
#define Command_mode   0x80  //Write control cmd 
#define Data_write_mode   0xa0  //Write data cmd   

//Register map starting adress
#define	LCD_reg_map_start_adress	0x00

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////////////////// Function description /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
Lcd3Dot5Digits::Lcd3Dot5Digits(uint CS, uint WR, uint DATA, uint BLA)
{
	m_CS = CS;
	m_WR = WR;
	m_BLA = BLA;
	m_DATA = DATA;
	m_TubeTab[0] = {0x5f}; //0
	m_TubeTab[1] = {0x06}; //1
	m_TubeTab[2] = {0x6b}; //2
	m_TubeTab[3] = {0x2f}; //3
	m_TubeTab[4] = {0x36}; //4
	m_TubeTab[5] = {0x3d}; //5
	m_TubeTab[6] = {0x7d}; //6
	m_TubeTab[7] = {0x07}; //7
	m_TubeTab[8] = {0x7f}; //8
	m_TubeTab[9] = {0x3f}; //9
	m_TubeTab[10] = {0x77}; //A
	m_TubeTab[11] = {0x7c}; //B
	m_TubeTab[12] = {0x59}; //C
	m_TubeTab[13] = {0x6e}; //D
	m_TubeTab[14] = {0x79}; //E
	m_TubeTab[15] = {0x71}; //F
	m_TubeTab[16] = {0x00}; // 
	m_TubeTab[17] = {0x76}; //H



//0x5f,0x06,0x6b,0x2f,
//					  0x36,0x3d,0x7d,0x07,
//					  0x7f,0x3f,0x77,0x7c,
//					  0x59,0x6e,0x79,0x71,
//					  0x00, 0x76
//					  };//0~9,A,b,C,d,E,F," ",H

}
////////////////////////////////////////////////////////////////////////////////
void Lcd3Dot5Digits::begin(void) 
{
	//Initialize arduino IOs
	//Setting of all 3 pins required to write in HT1621 Memory
	pinMode(m_CS, OUTPUT); 
	pinMode(m_WR, OUTPUT); 
	pinMode(m_DATA, OUTPUT);
	pinMode(m_BLA, OUTPUT); 	//BLA pin supplies LCD baclight
	digitalWrite(m_BLA,LOW);	//set it high to enable LCD backlight or LOW to disable it
	delay(50);
	Init_1621();		//launch HT1621 setup
}

///////////////////////////////////////////////////////////////////////////////
// Initialize arduino's IOs to communicate with HT1621
// 
void  Lcd3Dot5Digits::Init_1621(void) 
{
	//Send command to setup HT1621
	SendCmd_1621(Sys_en);	//turn on the HT1621
       	SendCmd_1621(RCosc);	//Select internal osc
       	SendCmd_1621(ComMode);  //Set 4Com,1/3bias
       	SendCmd_1621(LCD_on);	//turn On LCD Bias
} 
///////////////////////////////////////////////////////////////////////////////
void  Lcd3Dot5Digits::SendCmd_1621(uchar command) 
{ 
	CS0; 
	SendBit_1621(Command_mode,4);    
	SendBit_1621(command,8);  
	CS1;                    
}
///////////////////////////////////////////////////////////////////////////////
void  Lcd3Dot5Digits::SendBit_1621(uchar sdata,uchar cnt) //High bit first
{ 
	uchar i; 
       	for(i=0;i<cnt;i++) 
             { 
              	WR0;
		delayMicroseconds(20); 
              	if(sdata&0x80) DATA1; 
              	else DATA0; 
		delayMicroseconds(20);
              	WR1;
		delayMicroseconds(20);
              	sdata<<=1; 
              } 
}

////////////////////////////////////////////////////////////////////////////////
void  Lcd3Dot5Digits::HT1621_all_on(void) 
{ 
	uchar i; 
	uchar addr=LCD_reg_map_start_adress; 
	for(i=0;i<6;i++) 
	{ 
		send4bits(addr,0x0F); 
		addr+=1; 
	} 
}

/////////////////////////////////////////////////////////////////////////////////
void  Lcd3Dot5Digits::HT1621_all_off(void) 
{ 
	uchar i; 
        uchar addr=LCD_reg_map_start_adress; 
        for(i=0;i<6;i++) 
       	{ 
       		send4bits(addr,0x00); 
                addr+=1; 
        } 
} 

////////////////////////////////////////////////////////////////////////////////
void Lcd3Dot5Digits::send4bits(uchar addr,uchar sdata) 
{ 
	//Shift adress 2 bits left because two first bits are used by command and msb is pushed first
	addr  <<= 2;
	//Shift adress 4 bits left because each mem adress as 4 bits capacity only 
	sdata <<= 4;
	//Start feeding datas on the line
	CS0; 					//pull CS to 0	
	SendBit_1621(Data_write_mode,3);	//sends 101 to start WRITE MODE
	SendBit_1621(addr,6);     		//Write addr 6 bits with MSB first
	SendBit_1621(sdata,4);    		//Write data 4 bits
	CS1; 
}


////////////////////////////////////////////////////////////////////////////////
void Lcd3Dot5Digits::send8bits(uchar addr,uchar sdata) 
{ 
	//Shift adress 2 bits left because two first bits are used by command and msb is pushed first
	addr  <<= 2; 
	//start feeding datas on the line
	CS0; 					//pull CS to 0	
	SendBit_1621(Data_write_mode,3);	//sends 101 to start WRITE MODE
	SendBit_1621(addr,6);     		//Write addr 6 bits with MSB first
	SendBit_1621(sdata,8);    		//Write data 4 bits
	CS1; 
}

////////////////////////////////////////////////////////////////////////////////
void Lcd3Dot5Digits:: display_2_first_digits(uchar num)
{
	uchar data;
	uchar second_digit;
	//check that the data value is displayable
	if(num > 19 || num < 0)return;
	// set the data to display the second digit correctly on the 7 segment LCD
	second_digit = num%10;
	data = m_TubeTab[second_digit];
	//turn the first digit to 1 in case of by setting first bit of data to 1
	if(num >= 10)data+=0x80;
	
	//write the data to the LCD
	send8bits(0,data);
}

////////////////////////////////////////////////////////////////////////////////
void Lcd3Dot5Digits::display_third_digit(uchar num)
{
	uchar data;
	//check that the data value is displayable
	if(num > 9 || num < 0)return;
	// set the data to display the correct digit on 7 segment LCD
	data = m_TubeTab[num];
	//write the data to the LCD
	send8bits(2,data);
}

////////////////////////////////////////////////////////////////////////////////
void Lcd3Dot5Digits::display_last_digit(uchar num)
{
	uchar data;
	//check that the data value is displayable
	if(num > 9 || num < 0)return;
	// set the data to display the correct digit on 7 segment LCD
	data = m_TubeTab[num];
	//set first bit of Data to one to display the dot
	data+=0x80;
	//write the data to the LCD
	send8bits(4,data);
}

////////////////////////////////////////////////////////////////////////////////
void Lcd3Dot5Digits::display_H_as_last_digit(void)
{
	uchar data;
	// set the data to display the correct digit on 7 segment LCD
	data = 0x76;
	//write the data to the LCD
	send8bits(4,data);
}

