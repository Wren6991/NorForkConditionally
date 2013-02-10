#include "error.h"
#include "linker.h"

#include <set>
#include <sstream>

#include <iostream>

extern const int typesizes[n_types];
std::string makeguid(std::string name, int ptr);


std::string getlabel()
{
    static int lcount = 0;
    std::stringstream ss;
    ss << "__L" << std::hex << lcount++;
    return ss.str();
}

uint16_t getconstaddress(uint8_t val)
{
    return DECREMENT_START + ((val + 1) % 256);
}

//////////////////////// Vardict defs /////////////////////////////

void vardict::find_first_available_space(int searchstart)
{
    for (int i = searchstart; i < HEAP_SIZE; i++)
    {
        if (!memory_in_use[i])
        {
            first_available_space = i;
            return;
        }
    }
    // If it fell through:
    throw(error("Error: no more free space in heap!"));
}

// Search through memory for a block large enough
// If we find it, block it out and return the start.
int vardict::getspace(int size)
{
    int start = first_available_space;
    while (start < HEAP_SIZE)
    {
        bool enoughSpace = true;
        for (int i = start; i < start + size; i++)
        {
            if (memory_in_use[i])
            {
                enoughSpace = false;
                start = i + 1;          // start checking again at the next unchecked location
                break;
            }
        }
        if (enoughSpace)
        {
            for (int i = start; i < start + size; i++)
            {
                memory_in_use[i] = true;
                has_been_used[i] = true;
            }
            if (start == first_available_space)
                find_first_available_space(first_available_space);       // if we've covered the first known free space, find a new one.
            return start;
        }
    }
    throw(error("Error: no more free space in heap!"));
}

// Note: returns an offset from the heap start, you'll have to add
// the heap bottom to this value to get a machine address.
int vardict::addvar(std::string name, type_enum type)
{
    std::cout << "adding var " << name;
    variable *var = new variable;
    var->type = type;
    var->offset = getspace(typesizes[type]);
    if (vars.find(name) != vars.end())
        var->next = vars[name];  // push the stack down one...
    vars[name] = var;
    std::cout << " " << var->offset << "\n";
    return var->offset;
}

void vardict::remove(std::string name)
{
    // sanity check:
    std::map<std::string, variable*>::iterator iter = vars.find(name);
    if (iter == vars.end())
        throw(error("Error: tried to free non-existent variable! (Link-time)"));
    // release the memory:
    for (int i = iter->second->offset; i < iter->second->offset + typesizes[iter->second->type]; i++)
        memory_in_use[i] = 0;
    if (iter->second->offset < first_available_space)
        first_available_space = iter->second->offset;
    // update the dictionary: pop or remove
    if (iter->second->next)
    {
        variable *old = iter->second;
        iter->second = iter->second->next;
        delete old;
    }
    else
    {
        delete iter->second;
        vars.erase(iter);
    }
}

variable* vardict::getvar(std::string name)
{
    std::map<std::string, variable*>::iterator iter = vars.find(name);
    if (iter == vars.end())
        return 0;
    else
        return iter->second;
}

bool vardict::exists(std::string name)
{
    return vars.find(name) != vars.end();
}

// When we link a new function body, we mark all memory that is used by other functions as currently in use.
// This means memory is still packed as efficiently as possible within functions, but functions
// can't clobber memory used by others when they declare vars.
void vardict::push_function_scope()
{
    memory_in_use = has_been_used;
}

vardict::vardict()
{
    first_available_space = 0;
    for (int i = 0; i < (HEAP_TOP - HEAP_BOTTOM); i++)
    {
        memory_in_use.push_back(0);
        has_been_used.push_back(0);
    }
}


//////////////////////// Linker defs /////////////////////////////


linker::linker()
{
    defined_funcs["nfc"] = 0;
    defined_funcs["nfc4"] = 0;  // hardcoded funcs

    index = 0;
    buffer.reserve(ROM_SIZE);
}

void linker::savelabel(std::string name, linkval address)
{
    valtable[name + "_HI"] = address.gethighbyte();
    valtable[name + "_LO"] = address.getlowbyte();
}

// NB: index refers to the location _about_ to be written. (I.e. next location)
void linker::write8(linkval val)
{
    if (index >= ROM_SIZE)
        throw(error("Error: program too big!"));
    buffer.push_back(val);
    index++;
}

void linker::write16(linkval val)
{
    write8(val.gethighbyte());
    write8(val.getlowbyte());
}

void linker::padto8bytes()
{
    while ((buffer.size() % 8) != 0)
        write8(0);
}

void linker::emit_nfc2(linkval x, linkval y)
{
    padto8bytes();
    uint16_t next = index + 8;
    write16(x);
    write16(y);
    write16(next);
    write16(next);
}

// clear temp; set temp to not(x);  invert temp and branch on result.
void linker::emit_branchifzero(linkval testloc, linkval dest)
{
    uint16_t temploc = vars.addvar("__iftemp", type_int) + HEAP_BOTTOM;
    emit_nfc2(temploc, getconstaddress(0xff));
    emit_nfc2(temploc, testloc);
    uint16_t next = index + 8;
    write16(temploc);
    write16(temploc);
    write16(next);
    write16(dest);
    vars.remove("__iftemp");
}

void linker::emit_branchalways(linkval dest)
{
    uint16_t temploc = vars.addvar("__temp", type_int) + HEAP_BOTTOM;
    padto8bytes();
    write16(temploc);
    write16(temploc);
    write16(dest);
    write16(dest);
    vars.remove("__temp");
}

void linker::emit_copy(linkval src, linkval dest)
{
    emit_nfc2(dest, getconstaddress(0xff));
    emit_nfc2(dest, src);
    emit_nfc2(dest, dest);
}

void linker::emit_writeconst(uint8_t val, linkval dest)
{
    emit_nfc2(dest, getconstaddress(0xff));
    emit_nfc2(dest, getconstaddress(~val));
}

void linker::emit_copy_multiple(linkval src, linkval dest, int nbytes)
{
    for (int i = 0; i < nbytes; i++)
    {
        emit_copy(src + i, dest + i);
    }
}

void linker::emit_writeconst_multiple(int value, linkval dest, int nbytes)
{
    for (int i = 0; i < nbytes; i++)
    {
        emit_writeconst((value >> (nbytes - i - 1) * 8) & 0xff, dest + i);
    }
}

void linker::emit_writelabel(std::string label, linkval dest)
{
    emit_nfc2(dest, getconstaddress(0xff));
    emit_nfc2(dest, linkval(DECREMENT_START) + 256 - linkval(label + "_HI"));           // 256 - x  == ~x + 1
    emit_nfc2(dest + 1, getconstaddress(0xff));
    emit_nfc2(dest + 1, linkval(DECREMENT_START) + 256 - linkval(label + "_LO"));
}

void linker::add_object(object *obj)
{
    // check for collisions:
    for (std::set<std::string>::iterator iter = obj->defined_funcs.begin(); iter != obj->defined_funcs.end(); iter++)
    {
        if (defined_funcs.find(*iter) != defined_funcs.end())
        {
            std::stringstream ss;
            ss << "Error: multiple definitions of function \"" << *iter << "\"";
            throw(error(ss.str()));
        }
    }
    // add all the definitions to linker's symbol table:
    for (unsigned int i = 0; i < obj->tree->defs.size(); i++)
    {
        definition *def = obj->tree->defs[i];
        if ((def->type != dt_funcdef || !((funcdef*)def)->defined) && (def->type != dt_macrodef))
        {
            throw(error("Error: linker only accepts defined functions as symbols, wtf are you doing."));
        }
        if (def->type == dt_funcdef)
        {
            funcdef *fdef = (funcdef*)def;
            defined_funcs[fdef->name] = fdef;
        }
        else
        {
            macrodef *mdef = (macrodef*)def;
            defined_funcs[mdef->name] = mdef;
        }
        definitions.push_back(def);
    }
}

// keep a dict of static vars.
// start at main, pass through the statements. Link in all the hardcoded funcs.
// increment the current machine index as you go.
std::vector<char> linker::link()
{
    if (defined_funcs.find("main") == defined_funcs.end())
    {
        throw(error("Error: no definition of function main."));
    }
    valtable.clear();

    // Allocate static storage for function arguments/return vectors:
    allocatefunctionstorage();

    // set up pointer read instructions:
    // bfd0:    bff0 'ff  bfd8 bfd8
    // bfd8:    bff0 pppp bfe0 bfe0
    // bfe0:    bff1 'ff  xxxx rrrr
    // we write to bfda (rrrr) to set the pointer read location
    // when we jump to bfd0, it clears bff0 and then reads ~*ptr into it.
    // bff1 is cleared - as the result is always 0, the machine jumps to the return (rrrr) which was written beforehand.
    emit_writeconst_multiple(0xbff07d00, 0xbfd0, 4);
    emit_writeconst_multiple(0xbfd8bfd8, 0xbfd4, 4);
    emit_writeconst_multiple(0xbff0, 0xbfd8, 2);
    emit_writeconst_multiple(0xbfe0bfe0, 0xbfdc, 4);
    emit_writeconst_multiple(0xbff17d00, 0xbfe0, 4);

    // Link in the main function body
    std::cout << "Linking main\n";
    link(((funcdef*)defined_funcs["main"])->body);

    // generate halt instruction:
    emit_branchalways(index);

    // Link in the rest of the functions afterwards.
    for(std::map<std::string, definition*>::iterator iter = defined_funcs.begin(); iter != defined_funcs.end(); iter++)
    {
        if (iter->second && iter->second->type == dt_funcdef && ((funcdef*)iter->second)->name != "main")
            link((funcdef*)iter->second);
    }

    return assemble();
}

void linker::link(funcdef* fdef)
{
    std::cout << "Linking function: " << fdef->name << ", @" << std::hex << index << "\n";
    vars.push_function_scope();
    savelabel(fdef->name + ":__startvector", index);
    link(fdef->body);
    emit_copy_multiple(vars.getvar(fdef->name + ":__returnvector")->offset + HEAP_BOTTOM,
                       JUMP_PVECTOR, typesizes[type_pointer]);
    emit_branchalways(JUMP_INSTRUCTION);
}

void linker::link(block *blk)
{
    for (std::vector<vardeclaration*>::iterator iter = blk->declarations.begin(); iter != blk->declarations.end(); iter++)
    {
        for (unsigned int i = 0; i < (*iter)->names.size(); i++)
        {
            vars.addvar((*iter)->names[i], (*iter)->type);
        }
    }
    for (std::vector<statement*>::iterator iter = blk->statements.begin(); iter != blk->statements.end(); iter++)
    {
        link(*iter);
    }
    // TODO: make this a simple scope pop operation.
    for (std::vector<vardeclaration*>::iterator iter = blk->declarations.begin(); iter != blk->declarations.end(); iter++)
    {
        for (unsigned int i = 0; i < (*iter)->names.size(); i++)
        {
            vars.remove((*iter)->names[i]);
        }
    }
}

void linker::link(statement *stat)
{
    switch(stat->type)
    {
    case stat_call:
        link((funccall*)stat);
        break;
    case stat_goto:
        link((goto_stat*)stat);
        break;
    case stat_label:
        savelabel(((label*)stat)->name, index);
        break;
    case stat_if:
        link((if_stat*)stat);
        break;
    case stat_while:
        link((while_stat*)stat);
        break;
    case stat_assignment:
        link((assignment*)stat);
        break;
    default:
        throw(error("Error: linking unrecognized statement type"));
    }
}

// For the special case of NFC, where inputs are treated as hard addresses so we want the actual
// literal value instead of getconstaddress().
linkval linker::evaluate_or_return_literal(expression *expr)
{
    if (expr->type == exp_number)
        return expr->number;
    else
        return evaluate(expr);
}

// TODO: make this work properly for macros! (symbol problems)
void linker::link(funccall *call)
{
    if (defined_funcs.find(call->name) == defined_funcs.end())
    {
        std::stringstream ss;
        ss << "Error: no definition for function \"" << call->name << "\"";
        throw(error(ss.str()));
    }
    definition *def = defined_funcs[call->name];
    if (!def)   // it's a hardcoded function...
    {
        if (call->name == "nfc")
        {
            emit_nfc2(evaluate_or_return_literal(call->args[0]), evaluate_or_return_literal(call->args[1]));

        }
        else if (call->name == "nfc4")
        {
            padto8bytes();
            write16(evaluate_or_return_literal(call->args[0]));
            write16(evaluate_or_return_literal(call->args[1]));
            write16(evaluate_or_return_literal(call->args[2]));
            write16(evaluate_or_return_literal(call->args[3]));
        }
    }
    else
    {
        if (def->type == dt_macrodef)
        {
            macrodef *mdef = (macrodef*)def;
            // WHAT HAPPENS IF THE SAME MACRO IS CALLED IN DIFFERENT LOCATIONS?
            for (unsigned int i = 0; i < mdef->args.size(); i++)
            {
                std::string local_argname = getlabel();
                savelabel(local_argname, evaluate_or_return_literal(call->args[i]));
                valtable[mdef->args[i]] = local_argname;
                vars.addvar(mdef->args[i], type_label);
            }
            link(mdef->body);
            for (unsigned int i = 0; i < mdef->args.size(); i++)
                vars.remove(mdef->args[i]);
        }
        else if (def->type == dt_funcdef)
        {
            linkfunctioncall(call, (funcdef*)def);
        }
    }
}

// passes in arguments, sets up return vector and jumps.
// returns: the location of the function returnval register.
uint16_t linker::linkfunctioncall(funccall *fcall, funcdef *fdef)
{
    for (unsigned int i = 0; i < fcall->args.size(); i++)
    {
        if (fcall->args[i]->type == exp_number)
            emit_writeconst_multiple(fcall->args[i]->number, vars.getvar(fdef->args[i].name)->offset + HEAP_BOTTOM, typesizes[fdef->args[i].type]);
        else
            emit_copy_multiple(evaluate(fcall->args[i]), vars.getvar(fdef->args[i].name)->offset + HEAP_BOTTOM, typesizes[fdef->args[i].type]);
    }
    // return location: number of instructions taken to write the pointer + 1 instruction for the function jump.
    std::string returnlabel = getlabel();
    emit_writelabel(returnlabel, vars.getvar(fcall->name + ":__returnvector")->offset + HEAP_BOTTOM);
    emit_branchalways(linkval(fcall->name + ":__startvector"));
    savelabel(returnlabel, index);
    return vars.getvar(fcall->name + ":__returnval")->offset + HEAP_BOTTOM;
}

void linker::link(goto_stat *sgoto)
{
    uint16_t temploc = vars.addvar("temp", type_int) + HEAP_BOTTOM;
    write16(temploc);
    write16(temploc);
    linkval target = evaluate(sgoto->target);
    write16(target);
    write16(target);
    vars.remove("temp");
}


//  BRNZ L1
//  <if block>
//  BRA L2
//L1:
//  <else block
//L2:
void linker::link(if_stat* ifs)
{
    std::string elselabel = getlabel();
    std::string endlabel = getlabel();
    linkval testloc = linker::evaluate(ifs->expr);
    emit_branchifzero(testloc, linkval(elselabel));
    link(ifs->ifblock);
    if (ifs->elseblock)
    {
        // finished the if clause: nonconditional jump past else clause.
        emit_branchalways(linkval(endlabel));
        savelabel(elselabel, index);
        link(ifs->elseblock);
    }
    else
        savelabel(elselabel, index);
    savelabel(endlabel, index);
}

//L1:
//  <test>
//  BRZ L2
//  <body>
//  BRA L1
//L2:
void linker::link(while_stat *whiles)
{
    std::string toplabel = makeguid("__top", (int)whiles->blk);
    std::string exitlabel = makeguid("__exit", (int)whiles->blk);
    vars.addvar(toplabel, type_label);
    vars.addvar(exitlabel, type_label);
    savelabel(toplabel, index);
    emit_branchifzero(evaluate(whiles->expr), exitlabel);
    link(whiles->blk);
    // jump unconditionally to top:
    emit_branchalways(linkval(toplabel));
    savelabel(exitlabel, index);
    vars.remove(toplabel);
    vars.remove(exitlabel);
}

void linker::link(assignment *assg)
{
    expression targetexp;
    targetexp.type = exp_name;
    targetexp.name = assg->name;
    linkval target = evaluate(&targetexp);
    if (assg->expr->type == exp_number)
    {
        emit_writeconst_multiple(assg->expr->number, target, typesizes[assg->expr->val_type]);
    }
    else
    {
        emit_copy_multiple(evaluate(assg->expr), target, typesizes[assg->expr->val_type]);
    }

}


// Can return a literal value or a symbol: this is transparent to using functions.
// (think of a symbol as an "IOU" for an actual address: e.g. for labels we don't know the address til we reach them.)
// Always returns an address: if we need code to calculate the value (e.g. a function call) then this function emits that code
// and then returns the location where the value will be found.
linkval linker::evaluate(expression *expr)
{
    if (expr->type == exp_number)
    {
        if (expr->val_type == type_int)
            return getconstaddress(expr->number);       // NOT the literal itself. If you want the actual literal (e.g. with NFC) then fetch it directly, as this is an oddball case.
        else
        {
            uint16_t constloc = vars.addvar("__consttemp", expr->val_type) + HEAP_BOTTOM;
            emit_writeconst_multiple(expr->number, constloc, typesizes[expr->val_type]);
            vars.remove("__consttemp");
            return constloc;
        }
    }
    else if (expr->type == exp_name)
    {
        if (vars.exists(expr->name))
        {
            variable *var = vars.getvar(expr->name);
            // eager evaluation of labels: we get the most recently bound lable identity,
            // not the last one. This is what allows us to change argname meaning in different calls to a macro.
            if (var->type == type_label)
                if (valtable.find(expr->name) != valtable.end())
                    return valtable[expr->name];
                else
                    return expr->name;
            else
                return vars.getvar(expr->name)->offset + HEAP_BOTTOM;
        }
        else
        {
            std::stringstream ss;
            ss << "Error: unknown linker symbol \"" << expr->name << "\"";
            throw(error(ss.str()));
        }
    }
    else if (expr->type == exp_funccall)
    {
        if (expr->name == "val")
        {
            return getconstaddress(evaluate_or_return_literal(expr->args[0]).literal);
        }
        //TODO: make these return the value of this address.
        else if (expr->name == "first")
        {
            linkval ptr = evaluate(expr->args[0]);
            if (ptr.type == lv_literal)
                return ptr.literal >> 8;
            else
                return ptr.sym + "_HI";
        }
        else if (expr->name == "second")
        {
            linkval ptr = evaluate(expr->args[0]);
            if (ptr.type == lv_literal)
                return ptr.literal & 0xff;
            else
                return ptr.sym + "_LO";
        }
        else if (expr->name == "read" || expr->name == "increment" || expr->name == "decrement" ||
                 expr->name == "shiftleft" || expr->name == "shiftright")
        {
            std::string returnlabel = getlabel();
            if (expr->name == "read")
            {
                if (expr->args[0]->type == exp_number)
                    emit_writeconst_multiple(expr->args[0]->number, POINTER_READ_PVECTOR, typesizes[type_pointer]);
                else
                    emit_copy_multiple(evaluate(expr->args[0]), POINTER_READ_PVECTOR, typesizes[type_pointer]);
            }
            else
            {
                if (expr->name == "increment")
                    emit_writeconst(INCREMENT_START >> 8, POINTER_READ_PVECTOR);
                else if (expr->name == "decrement")
                    emit_writeconst(DECREMENT_START >> 8, POINTER_READ_PVECTOR);
                else if (expr->name == "shiftleft")
                    emit_writeconst(LEFTSHIFT_START >> 8, POINTER_READ_PVECTOR);
                else //if (expr->name == "shiftright")
                    emit_writeconst(RIGHTSHIFT_START >> 8, POINTER_READ_PVECTOR);
                if (expr->args[0]->type == exp_number)
                    emit_writeconst(expr->args[0]->number, POINTER_READ_PVECTOR + 1);
                else
                    emit_copy(evaluate(expr->args[0]), POINTER_READ_PVECTOR + 1);
            }
            emit_writelabel(returnlabel, JUMP_PVECTOR);
            emit_branchalways(POINTER_READ_INSTRUCTION);
            savelabel(returnlabel, index);
            emit_nfc2(POINTER_READ_RESULT, POINTER_READ_RESULT);    // NB multiple functions are returning here!
            return POINTER_READ_RESULT;                             // Careful of collisions.
        }
        else
        {
            if (defined_funcs[expr->name]->type != dt_funcdef)
                throw(error("Error: only functions can return values (call to " + expr->name + ")"));
            funccall fcall;
            fcall.args = expr->args;
            fcall.name = expr->name;
            return linkfunctioncall(&fcall, (funcdef*)defined_funcs[expr->name]);
        }
    }
    else
    {
        throw(error("Error: linking unknown expression type"));
    }
    return 0;
}


std::vector<char> linker::assemble()
{
    // assemble literals and symbols into ROM image:
    std::vector<char> image;
    for (std::vector<linkval>::iterator iter = buffer.begin(); iter != buffer.end(); iter++)
    {
        image.push_back(evaluate(*iter));
    }
    // put constant tables into last kilobyte.
    while (image.size() < (unsigned)INCREMENT_START)
        image.push_back(0);
    for (unsigned int i = 0; i <= 255; i++)
        image.push_back((i + 1) & 0xff);
    for (unsigned int i = 0; i <= 255; i++)
        image.push_back((i - 1) & 0xff);
    for (unsigned int i = 0; i <= 255; i++)
        image.push_back((i << 1) & 0xff);
    for (unsigned int i = 0; i <= 255; i++)
        image.push_back((i >> 1) & 0xff);
    return image;
}

linkval linkval::operator+(linkval rhs)
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

linkval linkval::operator-(linkval rhs)
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

linkval linkval::gethighbyte()
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

linkval linkval::getlowbyte()
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


// For each function, allocate the following statically:
// - a return value address, of a size matching the return type. The retval is found here after execution.
// - a return vector address: the caller writes its next address here before calling, and the callee jumps to this address.
// - an appropriately-sized variable for each argument (pass-by-value).
void linker::allocatefunctionstorage()
{
    for (std::map<std::string, definition*>::iterator iter = defined_funcs.begin(); iter != defined_funcs.end(); iter++)
    {
        std::cout << "Allocating for " << iter->first << "\n";
        if (iter->second && iter->second->type == dt_funcdef)
        {
            funcdef *fdef = (funcdef*)(iter->second);
            if (fdef->name == "main")
                continue;
            vars.addvar(fdef->name + ":__returnval", fdef->return_type);
            vars.addvar(fdef->name + ":__returnvector", type_pointer);
            for (unsigned int argnum = 0; argnum < fdef->args.size(); argnum++)
            {
                vars.addvar(fdef->args[argnum].name, fdef->args[argnum].type);  // name is already resolved to a global symbol by compiler.
            }
        }
    }
}

uint16_t linker::evaluate(linkval lv)
{
    switch (lv.type)
    {
    case lv_literal:
        return lv.literal;
    case lv_symbol:
        if (valtable.find(lv.sym) == valtable.end())
            throw(error("Linker error: no such symbol: " + lv.sym));
        else
            return evaluate(valtable[lv.sym]);
    case lv_expression:
        {
            linkval lv1 = *lv.argA;
            uint16_t first_operand = evaluate(lv1);
            uint16_t second_operand;
            if (lv.argB)
                second_operand = evaluate(*lv.argB);
            switch (lv.operation)
            {
            case linkval::op_add:
                return first_operand + second_operand;
            case linkval::op_sub:
                return first_operand - second_operand;
            case linkval::op_gethigh:
                return first_operand >> 8;
            case linkval::op_getlow:
                return first_operand & 0xff;
            }
        }
    }
    return 0;
}








