#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv/usbdrv.h"

#include <util/delay.h>

typedef enum CommandEnum{
    USB_LED_OFF = 0,
    USB_LED_ON = 1,
    USB_DATA_OUT = 2
} CommandEnum;

static uchar message[16] = "What's up!";

USB_PUBLIC uchar usbFunctionSetup(uchar data[8])
{
    usbRequest_t *rq = (usbRequest_t*)data; // cast data to correct type

    switch(rq->bRequest)   // custom command is in the bRequest field
    {
    case USB_LED_ON:
        PORTB |= 1; // turn LED on
        return 0;
    case USB_LED_OFF:
        PORTB &= ~1; // turn LED off
        return 0;
    case USB_DATA_OUT:
        usbMsgPtr = (uint16_t)message;
        return sizeof(message);
    }

    return 0; // should not get here
}

int main()
{
    message[0] = 'M';
    uchar i;
    DDRB = 1; // PB0 as output
    wdt_enable(WDTO_1S); // enable 1s watchdog timer

    usbInit();

    usbDeviceDisconnect(); // enforce re-enumeration
    for(i = 0; i<250; i++)   // wait 500 ms
    {
        wdt_reset(); // keep the watchdog happy
        _delay_ms(2);
        PORTB ^= 1;
    }
    usbDeviceConnect();

    sei(); // Enable interrupts after re-enumeration

    while(1)
    {
        wdt_reset(); // keep the watchdog happy
        usbPoll();
    }

    return 0;
}
