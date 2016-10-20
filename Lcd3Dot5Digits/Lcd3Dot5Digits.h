#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


//type def alias
#define uchar   unsigned char 
#define uint   unsigned int 

class Lcd3Dot5Digits
{
	public:
	Lcd3Dot5Digits(uint CS=12, uint WR=11, uint DATA=4, uint BLA=13);
	void begin(void);
	void Init_1621(void); 
	void SendCmd_1621(uchar command);
	void SendBit_1621(uchar sdata,uchar cnt); //High bit first
	void display_all_on(void); 
	void display_all_off(void); 
	void send4bits(uchar addr,uchar sdata);
	void send8bits(uchar addr,uchar sdata);
	void display_2_first_digits(uchar num);
	void display_third_digit(uchar num);
	void display_H_as_last_digit(void);
	void display_C_as_last_digit(void);
	void display_last_digit(uchar num);
	void display_float(float num);
	
	private:
	uint m_CS;
	uint m_WR;
	uint m_BLA;
	uint m_DATA;
	uint m_TubeTab[17];

};
