#ifndef _ERROR_H_INCLUDED_
#define _ERROR_H_INCLUDED_

#include <string>

#define errexit(err) throw(error(err))

class error
{
    public:
    std::string errstring;
    error(std::string);
};

#endif // _ERROR_H_INCLUDED_
