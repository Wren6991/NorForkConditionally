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

#define throw_error(error_stream) { std::stringstream __ss; (__ss << error_stream); throw(error(__ss.str()));}

#endif // _ERROR_H_INCLUDED_
