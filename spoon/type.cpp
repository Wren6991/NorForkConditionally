#include "type.h"

#include <sstream>

const std::string friendly_type_names[] = {
    "void",
    "int",
    "pointer",
    "label",
    "array",
    "number"
};

const int type_sizes[] = {
    0,
    1,
    2,
    0,
    -1,
    1
};

bool type_t::operator==(const type_t &rhs) const
{
    if (type == rhs.type)
        return true;
    else if (type == type_array && rhs.type == type_array)
        return second == rhs.second && (count == 0 || count == rhs.count);
    else
        return false;
}

int type_t::getsize() const
{
    if (type == type_array)
        return count * type_sizes[second];
    else
        return type_sizes[type];
}

std::string type_t::getname() const
{
    if (type == type_array)
    {
        std::stringstream ss;
        ss << friendly_type_names[second] << "[";
        if (count)
            ss << count;
        ss << "]";
        return ss.str();
    }
    else
        return friendly_type_names[type];
}

