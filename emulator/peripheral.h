#ifndef _PERIPHERAL_H_
#define _PERIPHERAL_H_

class peripheral
{
public:
    virtual void write(unsigned int address, unsigned int value) = 0;
    virtual unsigned int read() = 0;
};

#endif // _PERIPHERAL_H_
