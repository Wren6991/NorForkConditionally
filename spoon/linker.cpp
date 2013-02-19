#include "error.h"
#include "linker.h"

#include <set>
#include <sstream>

#include <iostream>

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
linkval vardict::addvar(std::string name, type_t type)
{
    variable *var = new variable;
    var->type = type;
    var->offset = getspace(type.getstoragesize());
    if (vars.find(name) != vars.end())
        var->next = vars[name];  // push the stack down one...
    vars[name] = var;
    var->address = var->offset + HEAP_BOTTOM;
#ifdef EBUG
    std::cout << "adding var " << name << " " << var->offset << "\n";
#endif
    return var->address;
}

void vardict::registervar(std::string name, type_t type, linkval address)
{
    variable *var = new variable;
    var->type = type;
    var->offset = -1;
    if (vars.find(name) != vars.end())
        var->next = vars[name];
    vars[name] = var;
    var->address = address;
}

void vardict::remove(std::string name)
{
    // sanity check:
    std::map<std::string, variable*>::iterator iter = vars.find(name);
    if (iter == vars.end())
        throw(error("Error: tried to free non-existent variable! (Link-time)"));
    variable *var = iter->second;
    // release the memory:
    if (var->offset >= 0)   // registervar() doesn't allocate memory, so it sets offset to -1.
    {
        for (int i = var->offset; i < var->offset + var->type.getstoragesize(); i++)
            memory_in_use[i] = 0;
        if (var->offset < first_available_space)
            first_available_space = var->offset;
    }
    // update the dictionary: pop or remove
    if (var->next)
    {
        variable *old = var;
        var = var->next;
        delete old;
    }
    else
    {
        delete var;
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

void vardict::push_temp_scope()
{
    tempscopes.push_back(std::vector<std::string>());
}

void vardict::pop_temp_scope()
{
    if (tempscopes.size() < 1)
        throw(error("Linker error: no tempscope to pop!"));
    std::vector<std::vector<std::string> >::iterator iter = tempscopes.end() - 1;
    std::vector<std::string> &names = *iter;
    for (unsigned int i = 0; i < names.size(); i++)
    {
#ifdef EBUG
        std::cout << "popping " << names[i] << "\n";
#endif
        remove(names[i]);
    }
    tempscopes.erase(iter);
}

void vardict::remove_on_pop(std::string name)
{
    if (tempscopes.size() < 1)
        remove(name);               // if we're not in a temp scope, assume we can just chuck the variable away.
    else
        tempscopes[tempscopes.size() - 1].push_back(name);
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
    defined_funcs["val"] = 0;
    defined_funcs["read"] = 0;
    defined_funcs["write"] = 0;
    defined_funcs["increment"] = 0;
    defined_funcs["decrement"] = 0;
    defined_funcs["shiftleft"] = 0;
    defined_funcs["shiftright"] = 0;

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
    linkval temploc = vars.addvar("__brztemp", type_int);
    emit_nfc2(temploc, getconstaddress(0xff));
    emit_nfc2(temploc, testloc);
    uint16_t next = index + 8;
    write16(temploc);
    write16(temploc);
    write16(next);
    write16(dest);
    vars.remove("__brztemp");
}

void linker::emit_branchalways(linkval dest, bool always_emit)
{
    padto8bytes();
    // check if the last instruction points at this one:
    if (!always_emit &&
        buffer[index - 4] == buffer[index  - 2] && buffer[index - 3] == buffer[index - 1] &&
        buffer[index - 4].type == lv_literal && buffer[index - 4].literal == index >> 8 &&
        buffer[index - 3].type == lv_literal && buffer[index - 3].literal == (index & 0xff))
    {
        buffer[index - 4] = dest.gethighbyte();
        buffer[index - 3] = dest.getlowbyte();
        buffer[index - 2] = dest.gethighbyte();
        buffer[index - 1] = dest.getlowbyte();
    }
    else
    {
        linkval temploc = vars.addvar("__bratemp", type_int);
        write16(temploc);
        write16(temploc);
        write16(dest);
        write16(dest);
        vars.remove("__bratemp");
    }

}

void linker::emit_copy(linkval src, linkval dest)
{
    emit_nfc2(dest, getconstaddress(0xff));
    emit_nfc2(dest, src);
    emit_nfc2(dest, dest);
}

void linker::emit_copy_inverted(linkval src, linkval dest)
{
    emit_nfc2(dest, getconstaddress(0xff));
    emit_nfc2(dest, src);
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
        if ((def->type != dt_funcdef || !((funcdef*)def)->defined) && (def->type != dt_macrodef && def->type != dt_vardec))
        {
            throw(error("Error: linker only accepts defined functions as symbols, wtf are you doing."));
        }
        if (def->type == dt_funcdef)
        {
            funcdef *fdef = (funcdef*)def;
            defined_funcs[fdef->name] = fdef;
        }
        else if (def->type == dt_macrodef)
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

    // Allocate static storage for function arguments/return vectors, and global variables:
    allocatefunctionstorage();
    for (unsigned int i = 0; i < definitions.size(); i++)
    {
        if (definitions[i]->type == dt_vardec)
        {
            vardeclaration &dec = *((vardeclaration*)definitions[i]);
            for (unsigned int i = 0; i < dec.vars.size(); i++)
            {
                vardeclaration::varpair &var = dec.vars[i];
                vars.addvar(var.name, var.type);
                if (var.type.type == type_array)
                    savelabel(var.name, vars.getvar(var.name)->address);
            }
        }
    }

    // set up pointer read/write instructions:
    // bfc0:    qqqq 'ff  bfc8 bfc8
    // bfc8:    qqqq bff0 bfe0 bfe0
    // bfd0:    bff0 'ff  bfd8 bfd8
    // bfd8:    bff0 pppp bfe0 bfe0
    // bfe0:    bff1 'ff  xxxx rrrr
    // To write:
    // we write ~val to bff0, jump to RAM.
    // in RAM we clear dest, write ~(~val) to dest, and then jump back to ROM using the same return instruction as for reads.
    // To read:
    // we write to bfda (rrrr) to set the pointer read location
    // when we jump to bfd0, it clears bff0 and then reads ~*ptr into it.
    // bff1 is cleared - as the result is always 0, the machine jumps to the return (rrrr) which was written beforehand.
    emit_writeconst_multiple(0x7d00, 0xbfc2, 2);
    emit_writeconst_multiple(0xbfc8bfc8, 0xbfc4, 4);
    emit_writeconst_multiple(0xbff0, 0xbfca, 2);
    emit_writeconst_multiple(0xbfe0bfe0, 0xbfcc, 4);
    emit_writeconst_multiple(0xbff07d00, 0xbfd0, 4);
    emit_writeconst_multiple(0xbfd8bfd8, 0xbfd4, 4);
    emit_writeconst_multiple(0xbff0, 0xbfd8, 2);
    emit_writeconst_multiple(0xbfe0bfe0, 0xbfdc, 4);
    emit_writeconst_multiple(0xbff17d00, 0xbfe0, 4);

    // Link in the main function body
#ifdef EBUG
    std::cout << "Linking main\n";
#endif
    vars.addvar(makeguid("__return", (int)defined_funcs["main"]), type_label);
    link(((funcdef*)defined_funcs["main"])->body);
    savelabel(makeguid("__return", (int)defined_funcs["main"]), index);

    // generate halt instruction:
    emit_branchalways(index, true);

    // Link in the rest of the functions afterwards.
    for(std::map<std::string, definition*>::iterator iter = defined_funcs.begin(); iter != defined_funcs.end(); iter++)
    {
        if (!iter->second)  // skip it if it's hardcoded! (largely 'cause we don't want null dereferencing.)
            continue;
        if (iter->second && iter->second->type == dt_funcdef && ((funcdef*)iter->second)->name != "main")
            link((funcdef*)iter->second);
    }
    return assemble();
}

void linker::link(funcdef* fdef)
{
#ifdef EBUG
    std::cout << "Linking function: " << fdef->name << ", @" << std::hex << index << "\n";
#endif
    vars.push_function_scope();
    std::string returnstat_target = makeguid("__return", (int)fdef);
    vars.addvar(returnstat_target, type_label);
    savelabel(fdef->name + ":__startvector", index);
    link(fdef->body);
    savelabel(returnstat_target, index);
    emit_copy_multiple(vars.getvar(fdef->name + ":__returnvector")->address,
                       JUMP_PVECTOR, type_t(type_pointer).getsize());
    emit_branchalways(JUMP_INSTRUCTION);
}

void linker::link(block *blk)
{
    for (std::vector<vardeclaration*>::iterator iter = blk->declarations.begin(); iter != blk->declarations.end(); iter++)
    {
        for (unsigned int i = 0; i < (*iter)->vars.size(); i++)
        {
            vardeclaration::varpair &var = (*iter)->vars[i];
            vars.addvar(var.name, var.type);
            if (var.type.type == type_array)
                savelabel(var.name, vars.getvar(var.name)->address);
        }
    }
    for (std::vector<statement*>::iterator iter = blk->statements.begin(); iter != blk->statements.end(); iter++)
    {
        link(*iter);
    }
    // TODO: make this a simple scope pop operation.
    for (std::vector<vardeclaration*>::iterator iter = blk->declarations.begin(); iter != blk->declarations.end(); iter++)
    {
        for (unsigned int i = 0; i < (*iter)->vars.size(); i++)
        {
            vars.remove((*iter)->vars[i].name);
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
        linkbuiltinfunction(call->args, call->name);
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
            linkfunctioncall(call->args, (funcdef*)def);
        }
    }
}

// passes in arguments, sets up return vector and jumps.
// returns: the location of the function returnval register.
linkval linker::linkfunctioncall(std::vector<expression*> &args, funcdef *fdef)
{
    for (unsigned int i = 0; i < args.size(); i++)
    {
        if (args[i]->type == exp_number)
            emit_writeconst_multiple(args[i]->number, vars.getvar(fdef->args[i].name)->address, fdef->args[i].type.getsize());
        else
            emit_copy_multiple(evaluate(args[i]), vars.getvar(fdef->args[i].name)->address, fdef->args[i].type.getsize());
    }
    // return location: number of instructions taken to write the pointer + 1 instruction for the function jump.
    std::string returnlabel = getlabel();
    emit_writelabel(returnlabel, vars.getvar(fdef->name + ":__returnvector")->address);
    emit_branchalways(linkval(fdef->name + ":__startvector"));
    savelabel(returnlabel, index);
    return vars.getvar(fdef->name + ":__returnval")->address;
}

linkval linker::linkbuiltinfunction(std::vector<expression*> &args, std::string name)
{
        if (name == "nfc")
        {
            emit_nfc2(evaluate_or_return_literal(args[0]), evaluate_or_return_literal(args[1]));
        }
        else if (name == "nfc4")
        {
            padto8bytes();
            write16(evaluate_or_return_literal(args[0]));
            write16(evaluate_or_return_literal(args[1]));
            write16(evaluate_or_return_literal(args[2]));
            write16(evaluate_or_return_literal(args[3]));
        }
        else if (name == "val")
        {
            return getconstaddress(evaluate_or_return_literal(args[0]).literal);
        }
        else if (name == "pair")
        {
            linkval temploc = vars.addvar("__pairtemp", type_pointer);
            emit_copy(evaluate(args[0]), temploc);
            emit_copy(evaluate(args[1]), temploc + 1);
            vars.remove_on_pop("__pairtemp");
            return temploc;
        }
        else if (name == "write")
        {
            std::string returnlabel = getlabel();
            emit_copy_inverted(evaluate(args[1]), POINTER_READ_RESULT);
            linkval pointerpos = evaluate(args[0]);
            emit_copy_multiple(pointerpos, POINTER_WRITE_CLEAR_INSTRUCTION, type_t(type_pointer).getsize());
            emit_copy_multiple(pointerpos, POINTER_WRITE_COPY_INSTRUCTION, type_t(type_pointer).getsize());
            emit_writelabel(returnlabel, JUMP_PVECTOR);
            emit_branchalways(POINTER_WRITE_CLEAR_INSTRUCTION);
            savelabel(returnlabel, index);                                          // URGH this is so inefficient :(
        }
        else if (name == "read" || name == "increment" || name == "decrement" ||
                 name == "shiftleft" || name == "shiftright")
        {
            std::string returnlabel = getlabel();
            if (name == "read")
            {
                if (args[0]->type == exp_number)
                {
                    emit_copy(args[0]->number, POINTER_READ_RESULT);
                    return POINTER_READ_RESULT;
                }
                emit_copy_multiple(evaluate(args[0]), POINTER_READ_PVECTOR, type_t(type_pointer).getsize());
            }
            else
            {
                if (name == "increment")
                    emit_writeconst(INCREMENT_START >> 8, POINTER_READ_PVECTOR);
                else if (name == "decrement")
                    emit_writeconst(DECREMENT_START >> 8, POINTER_READ_PVECTOR);
                else if (name == "shiftleft")
                    emit_writeconst(LEFTSHIFT_START >> 8, POINTER_READ_PVECTOR);
                else //if (name == "shiftright")
                    emit_writeconst(RIGHTSHIFT_START >> 8, POINTER_READ_PVECTOR);
                if (args[0]->type == exp_number)
                    emit_writeconst(args[0]->number, POINTER_READ_PVECTOR + 1);
                else
                    emit_copy(evaluate(args[0]), POINTER_READ_PVECTOR + 1);
            }
            emit_writelabel(returnlabel, JUMP_PVECTOR);
            emit_branchalways(POINTER_READ_INSTRUCTION);
            savelabel(returnlabel, index);
            emit_nfc2(POINTER_READ_RESULT, POINTER_READ_RESULT);    // NB multiple functions are returning here!
            return POINTER_READ_RESULT;                             // Careful of collisions.
        }
        else if (name == "andnot")
        {
            linkval returnloc = vars.addvar("__andnottemp", type_int);
            emit_nfc2(returnloc, getconstaddress(0xff));
            emit_nfc2(returnloc, evaluate(args[0]));
            emit_nfc2(returnloc, evaluate(args[1]));
            vars.remove_on_pop("__andnottemp");
            return returnloc;
        }
        else if (name == "and")
        {
            linkval returnloc = vars.addvar("__andreturnloc", type_int);
            linkval temploc = vars.addvar("__andtemploc", type_int);
            emit_nfc2(returnloc, getconstaddress(0xff));
            emit_nfc2(returnloc, evaluate(args[0]));
            emit_nfc2(temploc, getconstaddress(0xff));
            emit_nfc2(temploc, evaluate(args[1]));
            emit_nfc2(returnloc, temploc);
            vars.remove_on_pop("__andreturnloc");
            vars.remove_on_pop("__andtemploc");
            return returnloc;
        }
        else if (name == "not")
        {
            linkval returnloc = vars.addvar("__notreturnloc", type_int);
            emit_nfc2(returnloc, getconstaddress(0xff));
            emit_nfc2(returnloc, evaluate(args[0]));
            vars.remove_on_pop("__notreturnloc");
            return returnloc;
        }
        else if (name == "or")
        {
            linkval returnloc = vars.addvar("__orreturnloc", type_int);
            emit_copy(evaluate(args[0]), returnloc);
            emit_nfc2(returnloc, evaluate(args[1]));
            emit_nfc2(returnloc, returnloc);
            vars.remove_on_pop("__orreturnloc");
            return returnloc;
        }
        else if (name == "xor")
        {
            linkval returnloc = vars.addvar("__xorreturnloc", type_int);
            linkval temp1 = vars.addvar("__xortemp1", type_int);
            linkval temp2 = vars.addvar("__xortemp2", type_int);
            linkval argA = evaluate(args[0]);
            linkval argB = evaluate(args[1]);   // WATCH OUT: possibly clobber each other's memory, cause we're not passing by value
            emit_nfc2(temp1, getconstaddress(0xff));
            emit_nfc2(temp1, argA);
            emit_nfc2(temp1, argB);
            emit_nfc2(temp2, getconstaddress(0xff));
            emit_nfc2(temp2, argB);
            emit_nfc2(temp2, argA);
            emit_copy(temp1, returnloc);
            emit_nfc2(returnloc, temp2);
            emit_nfc2(returnloc, returnloc);
            vars.remove_on_pop("__xorreturnloc");
            vars.remove_on_pop("__xortemp1");
            vars.remove_on_pop("__xortemp2");
            return returnloc;
        }
        return 0;
}

void linker::link(goto_stat *sgoto)
{
    linkval temploc = vars.addvar("temp", type_int) + HEAP_BOTTOM;
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
    vars.push_temp_scope();
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
    vars.pop_temp_scope();
}

//L1:
//  <test>
//  BRZ L2
//  <body>
//  BRA L1
//L2:
void linker::link(while_stat *whiles)
{
    vars.push_temp_scope();
    std::string toplabel = makeguid("__top", (int)whiles->blk);
    std::string exitlabel = makeguid("__exit", (int)whiles->blk);
    vars.addvar(toplabel, type_label);
    vars.addvar(exitlabel, type_label);
    savelabel(toplabel, index);
    emit_branchifzero(evaluate(whiles->expr), exitlabel);
    link(whiles->blk);
    // jump unconditionally to top:
    emit_branchalways(linkval(toplabel), true);
    savelabel(exitlabel, index);
    vars.remove(toplabel);
    vars.remove(exitlabel);
    vars.pop_temp_scope();
}

void linker::link(assignment *assg)
{
    expression targetexp;
    targetexp.type = exp_name;
    targetexp.name = assg->name;
    linkval target = evaluate(&targetexp);
    if (assg->expr->type == exp_number)
    {
        emit_writeconst_multiple(assg->expr->number, target, assg->expr->val_type.getsize());
    }
    else
    {
        emit_copy_multiple(evaluate(assg->expr), target, assg->expr->val_type.getsize());
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
            return getconstaddress(expr->number);       // NOT the literal itself. If you want the actual literal (e.g. with NFC) then fetch it directly, as this is an oddball case. (SEE evaluate_or_return_literal())
        else
        {
            linkval constloc = vars.addvar("__consttemp", expr->val_type);
            emit_writeconst_multiple(expr->number, constloc, expr->val_type.getsize());
            vars.remove_on_pop("__consttemp");
            return constloc;
        }
    }
    else if (expr->type == exp_name)
    {
        if (vars.exists(expr->name))
        {
            variable *var = vars.getvar(expr->name);
            if (var->type == type_label)
            {
                // eager evaluation of labels: we get the most recently bound lable identity,
                // not the last one. This is what allows us to change argname meaning in different calls to a macro.
                if (valtable.find(expr->name) != valtable.end())
                    return valtable[expr->name];
                else
                    return expr->name;
            }
            else if (var->type.type == type_array)
            {
                linkval addressloc = vars.addvar("__arraytemp", type_pointer);
                emit_writelabel(expr->name, addressloc);
                vars.remove_on_pop("__arraytemp");
                return addressloc;
            }
            else
                return vars.getvar(expr->name)->address;
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
        if (!defined_funcs[expr->name])
        {
            return linkbuiltinfunction(expr->args, expr->name);
        }
        else
        {
            if (defined_funcs[expr->name]->type != dt_funcdef)
                throw(error("Error: only functions can return values (call to " + expr->name + ")"));
            return linkfunctioncall(expr->args, (funcdef*)defined_funcs[expr->name]);
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
    if (image.size() > INCREMENT_START)
        throw(error("Error: program is too big!"));
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


// For each function, allocate the following statically:
// - a return value address, of a size matching the return type. The retval is found here after execution.
// - a return vector address: the caller writes its next address here before calling, and the callee jumps to this address.
// - an appropriately-sized variable for each argument (pass-by-value).
void linker::allocatefunctionstorage()
{
    for (std::map<std::string, definition*>::iterator iter = defined_funcs.begin(); iter != defined_funcs.end(); iter++)
    {
#ifdef EBUG
        std::cout << "Allocating for " << iter->first << "\n";
#endif
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
            uint16_t second_operand = 0;
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








