#ifndef LINKVAL_H_INCLUDED
#define LINKVAL_H_INCLUDED

#include <stdint.h>
#include <string>

// linkvals are our "Assembly language" - they let us pass symbols and expressions for machine code
// instead of just the literal addresses, e.g. with labels where we don't know the location til we reach it.
// They get evaluated in the final "assemble" step.

typedef enum
{
    lv_literal = 0,
    lv_symbol,
    lv_expression
} lv_type;

struct linkval
{
    enum op_type {op_add, op_sub, op_gethigh, op_getlow};
    lv_type type;
    uint16_t literal;
    std::string sym;
    linkval *argA;
    linkval *argB;
    op_type operation;
    linkval() {argA = 0; argB = 0;}
    linkval(uint16_t lit):linkval() {type = lv_literal; literal = lit;}
    linkval(std::string s):linkval() {type = lv_symbol; sym = s;}
    linkval operator+(linkval rhs) const;
    linkval operator-(linkval rhs) const;
    bool operator==(linkval &rhs) const;
    linkval gethighbyte() const;
    linkval getlowbyte() const;
};

#endif // LINKVAL_H_INCLUDED
