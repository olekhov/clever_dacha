
#include <wirish/wirish.h>
#include "tm1638.h"

#include "am2302.h"

void setup() {
    pinMode(14, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(16, OUTPUT);
    pinMode(17, OUTPUT);
    pinMode(18, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(20, OUTPUT);
    
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
}

static int strncmp(const char *a, const char *b, int n)
{
  while((n>0) && (*a == *b) )
  {
    n--;
    a++; b++;
  }
  if(n==0) return 0;
  return *a-*b;
}

void loop() 
{
  AM2302 am(8);

  char s[16];
  int i, charsread=0;
  char req[6]={0};

  while(true)
  {

    charsread=0;
    while(charsread<5)
    {
      if(SerialUSB.available()==0)
      {
        delay(100);
        continue;
      }

      req[charsread]=SerialUSB.read();
      charsread++;
      continue;
    }

    if( strncmp(req,"read\r",5) != 0)
    {
      SerialUSB.print("Unsupported cmd [");
      for(i=0;i<5;i++) SerialUSB.print(req[i], HEX);
      SerialUSB.println("]");

      continue;
    }

    am.UpdateData();
    uint16_t t,r;

    if(am.Status() != am.OK)
    {
      SerialUSB.println("ERROR READING SENSOR");
    } else
    {
      am.valuetostring(am.Temperature(),s);
      SerialUSB.print(s);
      SerialUSB.print(' ');
      am.valuetostring(am.RH(),s);
      SerialUSB.println(s);
    }
  }
}

__attribute__((constructor)) void premain() {
    init();
}


int main(void) 
{
    setup();

    loop();

    return 0;
}

