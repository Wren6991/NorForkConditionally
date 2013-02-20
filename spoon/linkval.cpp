#include "linkval.h"

// If they're both literals, just add their values
// Otherwise, return an expression with pointers to the two arguments.
linkval linkval::operator+(linkval rhs) const
{
    linkval result(0);
    switch (type)
    {
    case lv_literal:
        if (rhs.type == lv_literal)
        {
            result.type = lv_literal;
            result.literal = literal + rhs.literal;
            break;
        }   // fall through if not:
    case lv_symbol:
    case lv_expression:
        result.type = lv_expression;
        result.operation = op_add;
        result.argA = new linkval(0);
        *result.argA = *this;
        result.argB = new linkval(0);
        *result.argB = rhs;
        break;
    }
    return result;
}

// If they're both literals, just subtract their values
// Otherwise, return an expression with pointers to the two arguments.
linkval linkval::operator-(linkval rhs) const
{
    linkval result(0);
    switch (type)
    {
    case lv_literal:
        if (rhs.type == lv_literal)
        {
            result.type = lv_literal;
            result.literal = literal - rhs.literal;
            break;
        }   // fall through if not:
    case lv_symbol:
    case lv_expression:
        result.type = lv_expression;
        result.operation = op_sub;
        result.argA = new linkval(0);
        *result.argA = *this;
        result.argB = new linkval(0);
        *result.argB = rhs;
        break;
    }
    return result;
}

// if they're different types, they're not equal.
// otherwise, check whether their values are equal.
// symbols don't get looked up, just directly compared.
bool linkval::operator==(linkval &rhs) const
{
    if (type != rhs.type)   // this is a conservative equality: those of different types may be equal, but we assume not.
        return false;
    switch (type)
    {
    case lv_literal:
        return literal == rhs.literal;
    case lv_symbol:
        return sym == rhs.sym;
    case lv_expression:
        return operation == rhs.operation &&
        (argA && rhs.argA ? *argA == *rhs.argA : argA == rhs.argA) &&
        (argB && rhs.argB ? *argB == *rhs.argB : argB == rhs.argB);     // compare by value if both non-null, else compare by reference.
    default:
        return false;
    }
}

// Shift it, append a "_HI", or return an expression.
linkval linkval::gethighbyte() const
{
    if (type == lv_literal)
        return literal >> 8;
    else if (type == lv_symbol)
        return sym + "_HI";
    else
    {
        linkval lv(0);
        lv.type = lv_expression;
        lv.operation = op_gethigh;
        lv.argA = new linkval(0);
        *lv.argA = *this;
        return lv;
    }
}

// Shift it, append a "_LO", or return an expression.
linkval linkval::getlowbyte() const
{
    if (type == lv_literal)
        return literal & 0xff;
    else if (type == lv_symbol)
        return sym + "_LO";
    else
    {
        linkval lv(0);
        lv.type = lv_expression;
        lv.operation = op_getlow;
        lv.argA = new linkval(0);
        *lv.argA = *this;
        return lv;
    }
}
