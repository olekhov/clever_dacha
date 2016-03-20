/*  
 *  Класс для общения с датчиком температуры/влажности DHT22 / AM2302
 *  */
#ifndef __AM2302_H__
#define __AM2302_H__

#include <wirish/wirish.h>


class AM2302
{
  private:
    uint8_t _datapin;
    uint32_t _lastread;
    int16_t _temp, _rh;
    uint8_t _status;

  public:

  static const uint8_t
    NOT_READ = 1,
    CRC_ERROR = 2,
    NO_DEVICE = 3,
    OK = 0;

    AM2302(uint8_t datapin) : _datapin(datapin)
  {
    pinMode(datapin, OUTPUT);
    digitalWrite(datapin, HIGH);
    _lastread=0;
    _temp=-10000;
    _rh=-10000;
    _status=NOT_READ;
  }

    uint8_t Status()
    {
      return _status;
    }

    int16_t Temperature()
    {
      return _temp;
    }
    int16_t RH()
    {
      return _rh;
    }

    void valuetostring(int16_t v, char *s)
    {
      int i,p;
      char buf[10];
      bool haslead=false;

      i=abs(v);
      buf[0]=(i%10)+'0'; i/=10;
      buf[1]='.'; 
      p=2;

      while(i>0)
      {
        buf[p]=i%10+'0'; 
        i/=10;
        p++;
        haslead=true;
      }

      p--;

      if(v<0) *s++='-';
      if(!haslead) *s++='0';

      while(p>=0)
      {
        *s=buf[p];
        p--;
        *s++;
      }
      *s=0;
    }


  private:
    
    uint32_t waitFor(uint8_t val, uint32_t timeout_clocks)
    {
      uint32 ctr; 
      ctr=0;
      while(digitalRead(_datapin) != val){
        ctr++;
        if(ctr>timeout_clocks) return 0xFFFFFFFF;
      }
      return ctr;
    }

  public:
    void UpdateData()
    {
      uint32_t t=millis(), c1, c2;
      if(t<_lastread) _lastread=0;

      if( (_temp != -10000) && ( t-_lastread < 2000 ))
      {
        return;
      }
      _lastread=t;


      digitalWrite(_datapin,LOW);
      delay(1);
      digitalWrite(_datapin,HIGH);
      delayMicroseconds(70);
      pinMode(_datapin,INPUT);

      c1=waitFor(HIGH,80000); // sensor pulled line up
      c2=waitFor(LOW, 80000); // wait for sensor pull low

      if(c1==0xFFFFFFFF || c2==0xFFFFFFFF)
      {
        _status = NO_DEVICE;
        pinMode(_datapin, OUTPUT);
        digitalWrite(_datapin,HIGH);
        return;
      }

      if(c1!=0xFFFFFFFF && c2!=0xFFFFFFFF && (c2-c1<80))
      {
        unsigned char data[4] = {0,0,0,0}, crc=0;
        int i,j, check;
        for(i=0;i<4;i++)
        {
          for(j=7;j>=0;j--)
          {
            c1=waitFor(HIGH,8000);
            c2=waitFor(LOW, 8000);
            if(c2>c1) data[i] |= 1<<j;
          }
        }
        for(j=7;j>=0;j--)
        {
          c1=waitFor(HIGH, 8000);
          c2=waitFor(LOW,  8000);
          if(c2>c1) crc |= 1<<j;
        }

        check=(data[0]+data[1]+data[2]+data[3]) & 0xFF;
        if(check != crc) 
        {
          _status = CRC_ERROR;
          pinMode(_datapin, OUTPUT);
          digitalWrite(_datapin,HIGH);
          return;
        }

        _temp=(data[2]&0x7F)*256+data[3];

        if(data[2]&0x80) 
          _temp=-_temp;

        _rh=data[0]*256+data[1];

        _status=OK;


      } else
      {
        _status=NO_DEVICE;
      }
      pinMode(_datapin, OUTPUT);
      digitalWrite(_datapin,HIGH);


//    interrupts();
  }
};



#endif
