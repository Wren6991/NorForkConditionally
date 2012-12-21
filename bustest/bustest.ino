const uint8_t pin_e = 0x04;  //e: WE_HADDR
const uint8_t pin_f = 0x08;  //f: WE_LADDR
const uint8_t pin_g = 0x10;  //g: OE_ROM
const uint8_t pin_h = 0x20;  //h: WE_ROM


#define ROM_SIZE 116

const char rombuffer[] = {
0xbf, 0xff, 0x00, 0x70, 0x00, 0x08, 0x00, 0x08,
0xbf, 0xff, 0xc0, 0x01, 0x00, 0x10, 0x00, 0x10,
0xbf, 0xff, 0x00, 0x71, 0x00, 0x18, 0x00, 0x18,
0xbf, 0xfe, 0x00, 0x70, 0x00, 0x20, 0x00, 0x20,
0xbf, 0xfe, 0xbf, 0xff, 0x00, 0x28, 0x00, 0x28,
0xbf, 0xfe, 0xbf, 0xfe, 0x00, 0x30, 0x00, 0x50,
0xbf, 0xff, 0x00, 0x70, 0x00, 0x38, 0x00, 0x38,
0xbf, 0xff, 0x00, 0x72, 0x00, 0x40, 0x00, 0x40,
0xc0, 0x00, 0xbf, 0xff, 0x00, 0x48, 0x00, 0x48,
0xbf, 0xff, 0xbf, 0xff, 0x00, 0x00, 0x00, 0x00,
0xbf, 0xff, 0x00, 0x70, 0x00, 0x58, 0x00, 0x58,
0xbf, 0xff, 0x00, 0x73, 0x00, 0x60, 0x00, 0x60,
0xc0, 0x00, 0xbf, 0xff, 0x00, 0x68, 0x00, 0x68,
0xbf, 0xff, 0xbf, 0xff, 0x00, 0x00, 0x00, 0x00,
0xff, 0x0f, 0x00, 0x01
};

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

void setup()
{
  Serial.begin(9600);
  PORTC = 0x3c;
  DDRC = 0x3c;    // bit outputs only
  detach();
}

char val;

void loop()
{
  if (Serial.available() <= 0)
    return;
  char c = Serial.read();
  if (c == 'p')
  {
    Serial.print("data: 0x");
    Serial.println((uint8_t)read(), HEX);
  }
  else if (c == 'w')
  {
    uint8_t data = Serial.parseInt();
    Serial.print("Writing data: ");
    Serial.println(data, HEX);
    write(data);
  }
  else if (c == 'a')
  {
    attach();
    Serial.println("Attached.");
  }
  else if (c == 'd')
  {
    detach();
    Serial.println("Detached.");
  }
  else if (c == 's')
  {
    delay(100);
    char pin = Serial.read();
    if (pin >= 'e' && pin <= 'h')
    {
      PORTC |= 0x04 << (pin - 'e');
      Serial.print("Set bit ");
      Serial.println(pin);
    }
    else
    {
      Serial.print("Unrecognised pin: ");
      Serial.println(pin);
    }
  }
  else if (c == 'c')
  {
    delay(100);
    char pin = Serial.read();
    if (pin >= 'e' && pin <= 'h')
    {
      PORTC &= ~(0x04 << (pin - 'e'));
      Serial.print("Cleared bit ");
      Serial.println(pin);
    }
    else
    {
      Serial.print("Unrecognised pin: ");
      Serial.println(pin);
    }
  }
  else if (c == 'r')
  {
    Serial.println("Reading EEPROM...");
    detach();
    PORTC = pin_g | pin_h;
    for (int i = 0; i < ROM_SIZE; i++)
    {
      attach();
      write((i >> 8) & 0xff);
      PORTC |= pin_e;
      PORTC &= ~pin_e;
      write(i & 0xff);
      PORTC |= pin_f;
      PORTC &= ~pin_f;
      detach();
      PORTC &= ~pin_g;
      Serial.print("0x");
      Serial.println((uint8_t)read(), HEX);
      PORTC |= pin_g;
    }
  }
  else if (c == 'R')
  {
    Serial.println("Programming EEPROM...");
    bool failed = false;
    detach();
    PORTC = pin_g | pin_h;
    for (int i = 0; i < ROM_SIZE; i++)
    {
      attach();
      write((i >> 8) & 0xff);
      PORTC |= pin_e;
      PORTC &= ~pin_e;
      write(i & 0xff);
      PORTC |= pin_f;
      PORTC &= ~pin_f;
      write(rombuffer[i]);
      PORTC &= ~pin_h;
      PORTC |= pin_h;
      detach();
      delay(4);
      PORTC &= ~pin_g;
      delay(2);
      if (read() != rombuffer[i])
      {
        Serial.print("Error at index 0x");
        Serial.print(i, HEX);
        Serial.print(": wrote 0x");
        Serial.print((uint8_t)rombuffer[i], HEX);
        Serial.print(", got 0x");
        Serial.println((uint8_t)read(), HEX);
        failed = true;
        break;
      }
      PORTC |= pin_g;
    }
    if (!failed)
    {
      Serial.println("Write successful.");
    }
  }
  else if (c == 'P')
  {
    detach();
    while (true)
    {
      Serial.println((uint8_t)read(), HEX);
      delay(10);
    }
  }
  else if (c != '\n' && c != ' ')
  {
    Serial.println("Usage:");
    Serial.println("p: probe");
    Serial.println("w: write data");    
    Serial.println("a/d: attach/detach");
    Serial.println("(s/c)(efgh): set/clear bit");
  }
  while (Serial.available() <= 0)
    Serial.read();      // eat up all the newlines etc.  
}

