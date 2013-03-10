#include <avr/wdt.h>

const uint8_t pin_e = 0x04;  //e: WE_HADDR
const uint8_t pin_f = 0x08;  //f: WE_LADDR
const uint8_t pin_g = 0x10;  //g: OE_ROM
const uint8_t pin_h = 0x20;  //h: WE_ROM

inline void write(char value)
{
  PORTD = (PORTD & 0x03) | (value & 0xfc);
  PORTC = (PORTC & 0xfc) | (value & 0x03);
}

inline char read()
{
  return (PIND & 0xfc) | (PINC & 0x03);
}

inline void attach()
{
  DDRD |= 0xfc;
  DDRC |= 0x03;
}

inline void detach()
{
  DDRD &= ~0xfc;
  DDRC &= ~0x03;
  write(0);
}

inline bool odd_parity_pass(uint8_t data, uint8_t bit)
{
  data = (data & 0xf) ^ (data >> 4);
  data = (data & 0x3) ^ (data >> 2);
  data = (data & 0x1) ^ (data >> 1);
  return (data ^ bit);
}

bool byte_is_command;
bool ready_for_command;

static uint8_t pagemem[72];
uint16_t currentaddress;

void setup()
{
  wdt_enable(WDTO_60MS);
  Serial.begin(9600);
  PORTC = pin_g | pin_h;
  DDRC = 0x3c;    // bit outputs only
  detach();
  byte_is_command = false;
  ready_for_command = true;
}


void loop()
{
  wdt_reset();
  if (Serial.available() <= 0)
    return;
  char c = Serial.read();
  if (byte_is_command)
  {
    if (c == 'A')
    {
      while (Serial.available() < 2);
      currentaddress = Serial.read() << 8;
      currentaddress |= Serial.read();
      Serial.print("O");
    }
    else if (c == 'P')
    {
      bool success = true;
      uint8_t count = 0;
      while (count < 72)
      {
        if (Serial.available() > 0)
        {
          pagemem[count++] = Serial.read();
          wdt_reset();
        }
      }
      for (uint8_t i = 0; i < 64; i++)
      {
        if (!odd_parity_pass(pagemem[i], pagemem[64 + (i >> 3)] >> (7 - (i & 0x7))))
        {
          Serial.print("Parity mismatch: byte ");
          Serial.print(pagemem[i], HEX);
          success = false;
          break;
        }
      }
      if (success)      
        Serial.print("O");
      else
        Serial.print("E");
    }
    else if (c == 'W')
    {
      Serial.print("O");
    }
    else
    {
      Serial.print("E");
      ready_for_command = false;
    }
    byte_is_command = false;
  }
  else if (c == 'S')
  {
    byte_is_command = true;
  }
  else if (c == ' ')
  {
    ready_for_command = true;
  }
  else if (ready_for_command)
  {
    Serial.print("E");
    ready_for_command = false;
  }
  while (Serial.available() <= 0)
    Serial.read();      // eat up all the newlines etc.  
}

