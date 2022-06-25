/*

	sample code for the I2C displays from WIDE.HK
     this library works for the 2 line  16 char / 20 char displays
     as well as the 4 line 20 char display.  I have them all, and tested
     the code on them all.  also i have built several projects using
     these displays, i LOVE THEM.   to see some of my projects i publish
     seriously rough and unedited videos on YOUTUBE - usually partially
     finished projects - but you can see the displays in action.  my
     YOUTUBE name is nucloid99  so if you search for nucloid99 videos, you'll
     find a few.  they are all bad, not edited, meant for some friends 
     to see what i am doing - not really for a wide audience (pun).

	rick pemble  W4RP  (that is my HAM RADIO CALLSIGN, not some stupid
       punk version of warp)  and nucloid99

     21-March-2015

     any updates, comments, complaints, etc --   rick@nucloid.com
*/

#include <Wire.h>       
#define OLED_Address      0x3c
#define OLED_Command_Mode 0x80
#define OLED_Data_Mode    0x40

int x,y,z;


void setup()
{
  Wire.begin();
  init_oled();
   
  setCursor(0,0);  sendString("Line 1");   //1 　　line
  setCursor(5,1);  sendString("Line 2");   //2 　　line
  
  // the next two lines are only if you have a 4 line display
//  setCursor(10,2); sendString("Line 3");   //3 　　line
//  setCursor(0,3);  sendString("Line 4");   //4 　　line
  
  sendFloat(3.145926,5,6,10,0);
   
}

void loop()
{
 
}


//////////////////////////////////////////////////////////////////////////////
//                                                                         //
//              OLED INIT                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void init_oled()  { 
// there are 3 command sets, Fundamental (RE and SD =0), Extended (RE=1), and OLED (RE=1 and SD=1)
// we switch back and forth between the command tables  and i try to put extra lines beteen the switches

 sendCommand(0x2A);  // function set (extended command set)     RE = 1  (Extended ON)
 sendCommand(0x71);  // function selection A, 
   sendData(0x5C);   //  enable internal Vdd regulator (if  you use 4.5-5.5  volts)
 sendCommand(0x28);  // function set (fundamental command set)  RE = 0  (Extended OFF)
 
 sendCommand(0x08);  // display off, cursor off, blink off
 
 sendCommand(0x2A);  // function set (extended command set) RE = 1   (Extended ON)
 sendCommand(0x79);  // OLED command set enabled            SD=1     (OLED ON)
 
 sendCommand(0xD5);  // set display clock divide ratio/oscillator frequency
 sendCommand(0x70);  // Power on Reset (POR) Value
 
 sendCommand(0x78);  // OLED command set disabled              SD=0    (OLED OFF)  (RE still on)

 sendCommand(0x09);  // extended function set (4-lines)  (0x08 for 2 lines)
 sendCommand(0x06);  // COM SEG direction
 sendCommand(0x72);  // function selection B, CROM, CGRAM selection -  3 CROMs   0= A  4 =B  8 = C
 sendData(0x00);     // 0Rd with CGRAM Selection lower 2 bits  00: 240/8, 01:248/8, 02:250/6, 03: 256/0
 // ROM A            // so the values for ROMs A,B,C could be: 00,04,08 - 01,05,09 - 02,06,0A  or  03,07,0B
                       
 sendCommand(0x2A);  // function set (extended command set)     RE = 1   (Extended ON)
 sendCommand(0x79);  // OLED command set enabled                SD = 1   (OLED ON)
 
 sendCommand(0xDA);  // set SEG pins hardware configuration
 sendCommand(0x10);  // to POR (power on Reset) values
 
 sendCommand(0xDC);  // function selection C
 sendCommand(0x00);  // SET VSL & GPIO  internal VSL (POR), and  HI-Z GPIO
 
 sendCommand(0x81);  // set contrast control
 sendCommand(0xEF);  // 00-ff   EF seems good enough
 
 sendCommand(0xD9);  // set phase length
 sendCommand(0x78);  // POR values
 
 sendCommand(0xDB);  // set VCOMH deselect level
 sendCommand(0x30);  // ~0.83 x Vcc
 
 sendCommand(0x78);  // OLED command set disabled               SD = 0   (OLED OFF)
 sendCommand(0x28);  // function set (fundamental command set)  RE = 0   (Extended OFF)
 
 sendCommand(0x01);  // clear display
 sendCommand(0x80);  // set DDRAM address to 0x00
 sendCommand(0x0C);  // display ON
}

///////////////////////////////////////////////////////////////////////////////
//
//      END   init_oled()
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  sendData(unsigned char data
//
////////////////////////////////////////////////////////////////////////////////
void sendData(unsigned char data)  {
  Wire.beginTransmission(OLED_Address);  	
  Wire.write(OLED_Data_Mode);     		
  Wire.write(data);
  Wire.endTransmission();                     
}
/////////////////////////////////////////////////////////////////////////////////
//
//    END  send Data(unsigned char data
//
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////
//
//    sendCommand(unsigned char command)
//
/////////////////////////////////////////////////////////////////////////////////////////
void sendCommand(unsigned char command) {
  Wire.beginTransmission(OLED_Address); 	 
  Wire.write(OLED_Command_Mode);     		 
  Wire.write(command);
  Wire.endTransmission();                 	 
  delay(10);
}
/////////////////////////////////////////////////////////////////////////////////////////
//
//    END  sendCommand(unsigned char command)
//
/////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//
//       sendString(const char *String)
//
///////////////////////////////////////////////////////////////////////////////////////
void sendString(const char *String) {
  unsigned char i=0;
  while(String[i])sendData(String[i++]);      // *** Send String to OLED
}
////////////////////////////////////////////////////////////////////////////////////////
//
//       END  sendString(const char *String)
//
///////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////
//
//   setCursor(uint8_t col, uint8_t row)
//
/////////////////////////////////////////////////////////////////////////////////////////////
void setCursor(uint8_t col, uint8_t row) {
  int row_offsets[] = {0x00, 0x20, 0x40, 0x60   };  // these offsetts are from the ROM char table map of the SSD1311 spec
  sendCommand(0x80 | (col + row_offsets[row]));   // and will work for all displays as long as you don't try to write to line 3 of a 2 line display, etc.
}
/////////////////////////////////////////////////////////////////////////////////////////////
//
//   END  setCursor(uint8_t col, uint8_t row)
//
/////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//
//      sendFloat(float digit, uint8_t dec, uint8_t nad, uint8_t col, uint8_t row)
//
////////////////////////////////////////////////////////////////////////////////////////
void sendFloat(float digit, uint8_t width, uint8_t precision, uint8_t col, uint8_t row)
{
  char line[10];
  dtostrf(digit,width,precision,line);        //Convert the float value to a string
  setCursor( col, row); 
  sendString(line);
}
////////////////////////////////////////////////////////////////////////////////////////
//
//      END  sendFloat(float digit, uint8_t dec, uint8_t nad, uint8_t col, uint8_t row)
//
////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
//          clearOLED()
//
//////////////////////////////////////////////////////////////////////////////////////
void clearOLED()
{
  sendCommand(0x01);   // clear dispaly (fill with blanks)
  sendCommand(0x02);   // set addr counter to 0
}
///////////////////////////////////////////////////////////////////////////////////////
//
//         END clearOLED()
//
//////////////////////////////////////////////////////////////////////////////////////






