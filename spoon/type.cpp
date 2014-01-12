#include "type.h"

#include <sstream>

const std::string friendly_type_names[] = {
    "void",
    "int",
    "pointer",
    "label",
    "array",
    "number",
    "expression"
};

const int type_sizes[] = {
    0,
    1,
    2,
    0,
    2,
    1,
    0
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

// tells us how much space the thing takes up in memory - used primarily by vardict when allocating memory.
int type_t::getstoragesize() const
{
    if (type == type_array)
        return count * type_sizes[second];
    else
        return type_sizes[type];
}

// tells us the size of the type, as interacted with by the programmer: e.g. arrays have size of two, as the programmer uses them as const pointers.
int type_t::getsize() const
{
    return type_sizes[type];
}

std::string type_t::getname() const
{
    if (type == type_array)
    {
        std::stringstream ss;
        ss << ((second >= 0 && second < n_types) ? friendly_type_names[second] : std::string("UNDEFINEDTYPE")) << "[";
        if (count)
            ss << count;
        ss << "]";
        return ss.str();
    }
    else
        return (type >= 0 && type < n_types) ? friendly_type_names[type] : std::string("UNDEFINEDTYPE");
}

