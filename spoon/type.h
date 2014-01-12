#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#include <string>

typedef enum {
    type_none = 0,
    type_int,
    type_pointer,
    type_label,
    type_array,
    type_number,        // generic number type, for compiler use only.
    type_expression,
    n_types
} type_enum;

struct type_t
{
    type_enum type;
    type_enum second;       // secondary type: e.g. array of *int*.
    int count;
    type_t(type_enum _type = type_none, type_enum _second = type_none, int _count = 0) {type = _type; second = _second; count = _count;}
    bool operator== (const type_t &rhs) const;
    int getsize() const;
    int getstoragesize() const; // NB the difference between these two! for an array, getsize() is the pointer size, and storagesize() is the underlying array size.
    std::string getname() const;
};




#endif // TYPE_H_INCLUDED
