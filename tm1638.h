/*
 * Library class for TM1638 LED 7-segment display driver
 * Класс для обслуживания светодиодного 7-сегментного дисплея на TM1638
 */

#ifndef __TM1638_H__
#define __TM1638_H__

#include <wirish/wirish.h>

/*
 *  ___0___
 * |       |
 * 5       1
 * |___6___|
 * |       |
 * 4       2
 * |___3___| (*) 7
 *
 *
 */

const static uint8_t font[]=
    { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, // 0..9
      0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71,    // A B C D E F
    };


class TM1638 : public Print
{

  private:
    uint8_t _strobe, _data, _clock, curaddr;
    const static uint8_t 
      DISPLAY_CONTROL = 0x80,
      ADDRESS_SET = 0xC0,

      DATA_WRITE = 0x40,
      DATA_READ = 0x42,
      DATA_ADDRFIX = 0x44;

  public:

    const static uint8_t RED_LED=0x01, GREEN_LED=0x02;


  /*  Задать пины: STB0, DIO, CLK */
  TM1638(uint8_t strobe, uint8_t data, uint8_t clock) :
    _strobe(strobe), _data(data), _clock(clock)
    {
      pinMode(_strobe, OUTPUT);
      pinMode(_clock, OUTPUT);
      pinMode(_data, OUTPUT);
      digitalWrite(_strobe, HIGH);
      curaddr=0;

      sendBatch(DATA_WRITE );
      sendBatch(DISPLAY_CONTROL|0x0F);
      clear();
   }

  void clear()
  {
      uint8_t clear[16]={0};
      sendBatch(ADDRESS_SET|0, clear, 16);
      sendBatch(ADDRESS_SET|0); 
  }

  void setLED(uint8_t pos, uint8_t color)
  {
    sendBatch(ADDRESS_SET|(pos*2+1), &color, 1);
  }

  void rawWrite(uint8_t pos, uint8_t data)
  {
    sendBatch(ADDRESS_SET|pos, &data, 1);
  }

  uint8_t FontChar(uint8_t ch)
  {
    if(ch>='0' && ch<='9') return font[ch-'0'];
    if(ch=='-') return 0x40;
    return 0; 
  }
  virtual void write(uint8_t ch)
  {
    uint8_t d;

    d=FontChar(ch);

    sendBatch(ADDRESS_SET|curaddr,&d,1);
    curaddr+=2;

  }

  virtual void write(const char *s)
  {
    int i=0,bp=0;
    uint8_t sendbuf[16]={0};

    while(s[i]!=0)
    {
      
      uint8_t ch=s[i];

      ch=FontChar(s[i]);
      if(s[i+1] =='.') {ch |= 0x80; i++; }
      sendbuf[bp]=ch; bp+=2;
      if(bp>=16) break;
      i++;
    }
    sendBatch(ADDRESS_SET|0, sendbuf, bp);
  }

  private:

  const static uint32_t delayscale=1;

  void sendByte(uint8_t b)
  {
    uint8_t i;
    pinMode(_data,OUTPUT);

    for(i=0;i<8;i++) 
    {
      digitalWrite(_clock,LOW);
      digitalWrite(_data,b&1);
      delayMicroseconds(delayscale);
      digitalWrite(_clock,HIGH);
      delayMicroseconds(delayscale);
      b>>=1;
    }
    
  }
  void sendBatch(uint8_t cmd)
  {
    sendBatch(cmd, NULL, 0);
  }

  void sendBatch(uint8_t cmd, const uint8_t *data, uint32_t data_count)
  {
    uint32_t i;
    digitalWrite(_strobe, LOW);

    sendByte(cmd);
    for(i=0;i<data_count;i++)
      sendByte(data[i]);

    digitalWrite(_strobe, HIGH);
  }


};
#endif
