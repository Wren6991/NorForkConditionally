#ifndef _PERIPHERAL_H_
#define _PERIPHERAL_H_

#include <set>
#include <stdint.h>

class peripheral
{
public:
    virtual void write(unsigned int address, unsigned int value) = 0;
    virtual unsigned int read(unsigned int address) = 0;
    std::set<uint16_t> readlocations;
};

#endif // _PERIPHERAL_H_
