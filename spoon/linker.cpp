#include "error.h"
#include "linker.h"


// REMOVE:
#include "printtree.h"

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
    current_address = 0;
    compile_to_ram = false;
    strip_unused_functions = false;
    buffer.reserve(ROM_SIZE);
}

void linker::savelabel(std::string name, linkval current_address)
{
    valtable[name] = current_address;
    valtable[name + "_HI"] = current_address.gethighbyte();
    valtable[name + "_LO"] = current_address.getlowbyte();
}

// NB: index refers to the location _about_ to be written. (I.e. next location)
void linker::write8(linkval val)
{
    if (index >= ROM_SIZE)
        throw(error("Error: program too big!"));
    buffer.push_back(val);
    index++;
    current_address++;
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
    uint16_t next = current_address + 8;
    write16(x);
    write16(y);
    write16(next);
    write16(next);
}

// clear temp; set temp to not(x);  invert temp and branch on result.
void linker::emit_branchifzero(linkval testloc, linkval dest, bool amend_previous, bool invert)
{
    if (amend_previous)
    {
        if (testloc.gethighbyte() != buffer[index - 8] || testloc.getlowbyte() != buffer[index - 7])
            throw(error("Linker error: option amend_previous used but previous instruction does not write to testloc"));
        if (!invert)
        {
            buffer[index - 2] = dest.gethighbyte(); // branch on zero
            buffer[index - 1] = dest.getlowbyte();
        }
        else
        {
            buffer[index - 4] = dest.gethighbyte(); // branch on non zero
            buffer[index - 3] = dest.getlowbyte();
        }
    }
    else
    {
        padto8bytes();
        linkval temploc = vars.addvar("__brztemp", type_int);
        emit_nfc2(temploc, getconstaddress(0xff));
        emit_nfc2(temploc, testloc);
        uint16_t next = index + 8;
        write16(temploc);
        write16(temploc);
        if (!invert)
        {
            write16(next);
            write16(dest);  // dest if 0
        }
        else
        {
            write16(dest);  // dest if non zero
            write16(next);
        }
    }


    vars.remove("__brztemp");
}

void linker::emit_branchifnonzero(linkval testloc, linkval dest, bool amend_previous)
{
    emit_branchifzero(testloc, dest, amend_previous, true);
}

void linker::emit_branchalways(linkval dest, bool always_emit)
{
    padto8bytes();
    // check if the last instruction points at this one:
    if (!always_emit && last_instruction_points_to_this_one())

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
    // Copy to self is destructive! (As destination is cleared before copy... x = x; would otherwise set x to 0)
    // Note however that linkval inequality will compare false in some cases such as labels vs. literals, so don't trust it.
    if (src == dest)
        return;
    // Note that this also protects against __brztemp being in the same place as a temporary function return loc.
    emit_nfc2(dest, getconstaddress(0xff));
    emit_nfc2(dest, src);
    emit_nfc2(dest, dest);

}

void linker::emit_copy_inverted(linkval src, linkval dest)
{
    if (src == dest)
    {
        emit_nfc2(dest, dest);  // just invert, no copy (so it still does what we expect)
    }
    else
    {
        emit_nfc2(dest, getconstaddress(0xff));
        emit_nfc2(dest, src);
    }
}

void linker::emit_writeconst(uint8_t val, linkval dest)
{
    emit_nfc2(dest, getconstaddress(0xff));
    if (val)    // no need to do this if 0, as already cleared.
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
    emit_nfc2(dest, linkval(DECREMENT_START) + (linkval(256) - linkval(label + "_HI")).getlowbyte());            // 256 - x  == ~x + 1
    emit_nfc2(dest + 1, getconstaddress(0xff));
    emit_nfc2(dest + 1, linkval(DECREMENT_START) + (linkval(256) - linkval(label + "_LO")).getlowbyte());        // getlowbyte is equivalent to & 0xff; makes 0 -> 0 instead of 256.
}

bool linker::last_instruction_points_to_this_one()
{
    // true if both skip fields match, and each pair equals the current index.
    return
        buffer[index - 4] == buffer[index  - 2] && buffer[index - 3] == buffer[index - 1] &&
        buffer[index - 4].type == lv_literal && buffer[index - 4].literal == index >> 8 &&
        buffer[index - 3].type == lv_literal && buffer[index - 3].literal == (index & 0xff);
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
            vardeclaration *dec = (vardeclaration*)definitions[i];
            for (unsigned int i = 0; i < dec->vars.size(); i++)
            {
                vardeclaration::varpair &var = dec->vars[i];
                if (var.exported)
                    vars.registervar(var.name, var.type, var.exportvector);
                else
                    vars.addvar(var.name, var.type);
                if (var.type.type == type_array)
                    savelabel(var.name, vars.getvar(var.name)->address);
                exportvardeclaration(dec);
            }
        }
    }

    // set up pointer read/write instructions:
    // bfc0:    qqqq 'ff  bfc8 bfc8     ; clear target location q (q field written by caller)
    // bfc8:    qqqq bff0 bfe0 bfe0     ; q = ~ bff0 (bff0 previously set to ~x; q must be set here again.) Skip to jump instruction.
    // bfd0:    bff0 'ff  bfd8 bfd8     ; clear bff0
    // bfd8:    bff0 pppp bfe0 bfe0     ; set bff0 to ~p (p field written by caller)
    // bfe0:    bff1 'ff  xxxx rrrr     ; jump to rrrr
    // To write:
    // we write ~val to bff0, jump to RAM.
    // in RAM we clear dest, write ~(~val) to dest, and then jump back to ROM using the same return instruction as for reads.
    // To read:
    // we write to bfda (rrrr) to set the pointer read location
    // when we jump to bfd0, it clears bff0 and then reads ~*ptr into it.
    // bff1 is cleared - as the result is always 0, the machine jumps to the return (rrrr) which was written beforehand.
    if (!compile_to_ram)
    {
        emit_writeconst_multiple(0x7d00, 0xbfc2, 2);
        emit_writeconst_multiple(0xbfc8bfc8, 0xbfc4, 4);
        emit_writeconst_multiple(0xbff0, 0xbfca, 2);
        emit_writeconst_multiple(0xbfe0bfe0, 0xbfcc, 4);
        emit_writeconst_multiple(0xbff07d00, 0xbfd0, 4);
        emit_writeconst_multiple(0xbfd8bfd8, 0xbfd4, 4);
        emit_writeconst_multiple(0xbff0, 0xbfd8, 2);
        emit_writeconst_multiple(0xbfe0bfe0, 0xbfdc, 4);
        emit_writeconst_multiple(0xbff17d00, 0xbfe0, 4);
    }


    // Link in the main function body
#ifdef EBUG
    std::cout << "Linking main\n";
#endif
    vars.addvar(makeguid("__return", (long)defined_funcs["main"]), type_label);
    link(((funcdef*)defined_funcs["main"])->body);
    savelabel(makeguid("__return", (long)defined_funcs["main"]), current_address);

    // generate halt instruction:
    emit_branchalways(current_address, true);

    // Link in the rest of the functions afterwards.
    if (strip_unused_functions)
        removeunusedfunctions();

    for(std::map<std::string, definition*>::iterator iter = defined_funcs.begin(); iter != defined_funcs.end(); iter++)
    {
        if (!iter->second)  // skip it if it's hardcoded! (largely 'cause we don't want null dereferencing.)
            continue;
        if (iter->second && iter->second->type == dt_funcdef && ((funcdef*)iter->second)->name != "main")
            link((funcdef*)iter->second);
    }

    // Finally, push all the strings onto the end.
    for (unsigned int stringnum = 0; stringnum < stringvalues.size(); stringnum++)
    {
        std::cout << "Saving string at "  << stringvalues[stringnum].first << "\n";
        savelabel(stringvalues[stringnum].first, current_address);
        std::string &str = stringvalues[stringnum].second;
        for (unsigned int i = 0; i <= str.size(); i++)      // <= instead of < because we want to include the terminating zero.
            write8(str[i]);
    }
    savelabel("__program_end", current_address);
    std::cout << "__program_end: " << evaluate(linkval("__program_end")) << "\n";
//#ifdef EBUG
    std::cout << "Executable size: " << std::dec << index << std::hex << " (0x" << index << ") bytes.\n";
//#endif // EBUG

    return assemble();
}

void linker::link(funcdef* fdef)
{

#ifdef EBUG
    std::cout << "Linking function: " << fdef->name << ", @" << std::hex << current_address << "\n";
#endif
    vars.push_function_scope();
    if (!fdef->exported)
    {
        std::string returnstat_target = makeguid("__return", (long)fdef);
        vars.addvar(returnstat_target, type_label);
        savelabel(fdef->name + ":__startvector", current_address);
        link(fdef->body);
        savelabel(returnstat_target, current_address);
        emit_copy_multiple(vars.getvar(fdef->name + ":__returnvector")->address,
                       JUMP_PVECTOR, type_t(type_pointer).getsize());
        emit_branchalways(JUMP_INSTRUCTION);
    }
    else
    {
        savelabel(fdef->name + ":__startvector", fdef->exportvectors[0]);
    }
    exportfuncdef(fdef);
}

void linker::exportfuncdef(funcdef *fdef)
{
    // No export if hardcoded:
    if (defined_funcs[fdef->name] == 0)
        return;
    defstring << "export function " << fdef->return_type.getname() << " " << fdef->name << "(";
    if (fdef->args.size() > 0)
    {
        std::string fullname = fdef->args[0].name;
        defstring << fdef->args[0].type.getname() << " " << fullname.substr(0, fullname.find("@"));
        for (unsigned int i = 1; i < fdef->args.size(); i++)
        {
            std::string fullname = fdef->args[i].name;
            defstring << ", " << fdef->args[i].type.getname() << " " << fullname.substr(0, fullname.find("@"));
        }
    }
    // Export function information in the following order: call address, return value location, return vector, argument locations
    defstring << "): 0x";
    defstring << std::hex << ((valtable[fdef->name + ":__startvector_HI"].literal << 8) + valtable[fdef->name + ":__startvector_LO"].literal);
    defstring << ", 0x" << vars.getvar(fdef->name + ":__returnval")->address.literal;
    defstring << ", 0x" << vars.getvar(fdef->name + ":__returnval")->address.literal;
    for (unsigned int i = 0; i < fdef->args.size(); i++)
        defstring << ", 0x" << vars.getvar(fdef->args[i].name)->address.literal;
    defstring << ";\r\n";
}

void linker::exportvardeclaration(vardeclaration *vardec)
{
    for (unsigned int i = 0; i < vardec->vars.size(); i++)
    {
        vardeclaration::varpair &var = vardec->vars[i];
        defstring << "export var ";
        std::string type_str = var.type.getname();
        std::string subscript = "";
        if (type_str.find('[') != std::string::npos)
        {
            int bracketpos = type_str.find('[');
            subscript = type_str.substr(bracketpos);
            type_str = type_str.substr(0, bracketpos);
        }
        std::string name = var.name;
        name = name.substr(0, name.find('@'));
        defstring << type_str << " " << name << subscript <<  ": ";
        defstring << "0x" << std::hex << vars.getvar(var.name)->address.literal << ";\r\n";
    }
}


void linker::link(block *blk)
{
    for (std::vector<vardeclaration*>::iterator iter = blk->declarations.begin(); iter != blk->declarations.end(); iter++)
    {
        for (unsigned int i = 0; i < (*iter)->vars.size(); i++)
        {
            vardeclaration::varpair &var = (*iter)->vars[i];
            if (var.exported)
                vars.registervar(var.name, var.type, var.exportvector);
            else
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
        savelabel(((label*)stat)->name, current_address);
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
            vars.push_temp_scope();
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
            vars.pop_temp_scope();
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
    // For each argument: if it is a constant then emit a constant copy, else evaluate the expression and copy the result to the argloc.
    for (unsigned int i = 0; i < args.size(); i++)
    {
        if (args[i]->type == exp_number)
            emit_writeconst_multiple(args[i]->number, vars.getvar(fdef->args[i].name)->address, fdef->args[i].type.getsize());
        else
        {
            linkval argloc = vars.getvar(fdef->args[i].name)->address;
            // Pass argloc as the preferred location, store the value in arg_val_loc: if argloc is successfully used then these linkvals are the same and emit_copy_multiple() is a no-op.
            linkval arg_val_loc = evaluate(args[i], true, argloc);
            emit_copy_multiple(arg_val_loc, argloc, fdef->args[i].type.getsize());
        }
    }
    // Create a label name which refers to the first instruction after the call: pass this to the call-writing code. Once the call code is written we then save the actual location that label refers to.
    // (Avoids trying to guess the number of instructions: unique label acts as an "IOU" for the location)
    std::string returnlabel = getlabel();
    // We write the location of the label to the function's return vector (even if we don't know it yet - this gets sorted out at the final assembly), then jump into the function.
    emit_writelabel(returnlabel, vars.getvar(fdef->name + ":__returnvector")->address);
    emit_branchalways(linkval(fdef->name + ":__startvector"));
    savelabel(returnlabel, current_address);
    return vars.getvar(fdef->name + ":__returnval")->address;
}

linkval linker::linkbuiltinfunction(std::vector<expression*> &args, std::string name, bool givenpreferred, linkval preferred)
{
        if (name == "nfc")
        {
            vars.push_temp_scope();
            emit_nfc2(evaluate_or_return_literal(args[0]), evaluate_or_return_literal(args[1]));
            vars.pop_temp_scope();
        }
        else if (name == "nfc4")
        {
            padto8bytes();
            vars.push_temp_scope();
            write16(evaluate_or_return_literal(args[0]));
            write16(evaluate_or_return_literal(args[1]));
            write16(evaluate_or_return_literal(args[2]));
            write16(evaluate_or_return_literal(args[3]));
            vars.pop_temp_scope();
        }
        else if (name == "val")
        {
            return getconstaddress(evaluate_or_return_literal(args[0]).literal);
        }
        else if (name == "first")
        {
            if (args[0]->type == exp_number)
            {
                return getconstaddress(args[0]->number >> 8);
            }
            else
            {
                linkval temploc = vars.addvar("__firsttemp", type_int);
                vars.push_temp_scope();             // The scope is pushed partly to clean up variables as soon as possible, but largely so that the evaluations take place in a valid scope to avoid internal clobbering.
                emit_copy(evaluate(args[0]), temploc);
                vars.pop_temp_scope();
                vars.remove_on_pop("__firsttemp");
                return temploc;
            }
        }
        else if (name == "second")
        {
            if (args[0]->type == exp_number)
            {
                return getconstaddress(args[0]->number & 0xff);
            }
            else
            {
                linkval temploc = vars.addvar("__secondtemp", type_int);
                vars.push_temp_scope();
                emit_copy(evaluate(args[0]) + 1, temploc);
                vars.pop_temp_scope();
                vars.remove_on_pop("__secondtemp");
                return temploc;
            }
        }
        else if (name == "pair")
        {
            linkval temploc = vars.addvar("__pairtemp", type_pointer);
            vars.push_temp_scope();
            emit_copy(evaluate(args[0], true, temploc), temploc);
            emit_copy(evaluate(args[1], true, temploc + 1), temploc + 1);
            vars.pop_temp_scope();
            vars.remove_on_pop("__pairtemp");
            return temploc;
        }
        else if (name == "write")
        {
            std::string returnlabel = getlabel();
            vars.push_temp_scope();
            emit_copy_inverted(evaluate(args[0], true, POINTER_WRITE_VALUE), POINTER_WRITE_VALUE);
            linkval pointerpos = evaluate(args[1], true, POINTER_WRITE_CLEAR_INSTRUCTION);
            vars.pop_temp_scope();
            emit_copy_multiple(pointerpos, POINTER_WRITE_CLEAR_INSTRUCTION, type_t(type_pointer).getsize());
            emit_copy_multiple(pointerpos, POINTER_WRITE_COPY_INSTRUCTION, type_t(type_pointer).getsize());
            emit_writelabel(returnlabel, JUMP_PVECTOR);
            emit_branchalways(POINTER_WRITE_CLEAR_INSTRUCTION);
            savelabel(returnlabel, current_address);                                          // URGH this is so inefficient :(
        }
        else if (name == "read" || name == "increment" || name == "decrement" ||
                 name == "shiftleft" || name == "shiftright")
        {
            std::string returnlabel = getlabel();
            if (name == "read")
            {
                if (args[0]->type == exp_number)
                    return args[0]->number; // this should ONLY get reached with a call such as read(debugin). In this case, no need to worry about timing or clobbering!
                vars.push_temp_scope();
                emit_copy_multiple(evaluate(args[0], true, POINTER_READ_PVECTOR), POINTER_READ_PVECTOR, type_t(type_pointer).getsize());
                vars.pop_temp_scope();
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
                    emit_copy(evaluate(args[0], true, POINTER_READ_PVECTOR + 1), POINTER_READ_PVECTOR + 1);
            }
            emit_writelabel(returnlabel, JUMP_PVECTOR);
            emit_branchalways(POINTER_READ_INSTRUCTION);
            savelabel(returnlabel, current_address);
            if (givenpreferred)
            {
                emit_copy_inverted(POINTER_READ_RESULT, preferred);
                return preferred;
            }
            else
            {
                emit_nfc2(POINTER_READ_RESULT, POINTER_READ_RESULT);    // NB multiple functions are returning here!
                return POINTER_READ_RESULT;                             // Careful of collisions.
            }
        }
        else if (name == "andnot")
        {
            linkval returnloc = givenpreferred ? preferred : vars.addvar("__andnottemp", type_int);
            vars.push_temp_scope(); // For the arguments
            emit_copy_inverted(evaluate(args[0], true, returnloc), returnloc);
            emit_nfc2(returnloc, evaluate(args[1]));
            vars.pop_temp_scope();
            if (!givenpreferred)
                vars.remove_on_pop("__andnottemp");
            return returnloc;
        }
        else if (name == "and")     // the problem occurs when the argument starts off in the finish location: it is destroyed by the invcopy to self (destructive).
        {
            linkval returnloc = givenpreferred ? preferred : vars.addvar("__andreturnloc", type_int);
            vars.push_temp_scope(); // For the arguments
            linkval temploc = vars.addvar("__andtemploc", type_int);
            emit_copy_inverted(evaluate(args[0], true, returnloc), returnloc);
            emit_copy_inverted(evaluate(args[1]), temploc);
            emit_nfc2(returnloc, temploc);
            vars.pop_temp_scope();
            if (!givenpreferred)
                vars.remove_on_pop("__andreturnloc");
            vars.remove_on_pop("__andtemploc");
            return returnloc;
        }
        else if (name == "not")
        {
            linkval returnloc = givenpreferred ? preferred : vars.addvar("__notreturnloc", type_int);
            vars.push_temp_scope(); // For the arguments
            emit_copy_inverted(evaluate(args[0], true, returnloc), returnloc);
            vars.pop_temp_scope();
            if (!givenpreferred)
                vars.remove_on_pop("__notreturnloc");
            return returnloc;
        }
        else if (name == "or")
        {
            linkval returnloc = givenpreferred ? preferred : vars.addvar("__orreturnloc", type_int);
            vars.push_temp_scope(); // For the arguments
            emit_copy(evaluate(args[0], true, returnloc), returnloc);
            emit_nfc2(returnloc, evaluate(args[1]));
            emit_nfc2(returnloc, returnloc);
            vars.pop_temp_scope();
            if (!givenpreferred)
                vars.remove_on_pop("__orreturnloc");
            return returnloc;
        }
        else if (name == "xor")
        {
            linkval returnloc = givenpreferred ? preferred : vars.addvar("__xorreturnloc", type_int);
            linkval temp1 = vars.addvar("__xortemp1", type_int);
            linkval temp2 = vars.addvar("__xortemp2", type_int);
            vars.push_temp_scope(); // For the arguments
            linkval argA = evaluate(args[0], true, temp1);
            linkval argB = evaluate(args[1], true, temp2);
            emit_copy_inverted(argA, temp1);
            emit_nfc2(temp1, argB);
            emit_copy_inverted(argB, temp2);
            emit_nfc2(temp2, argA);
            emit_copy(temp1, returnloc);   // we can't do away with temp1 because if the second argument is the same loc as the preferred return, writing ~the first argument to returnloc would have side effects on the second argument.
            emit_nfc2(returnloc, temp2);
            emit_nfc2(returnloc, returnloc);
            vars.pop_temp_scope();
            if (!givenpreferred)
                vars.remove_on_pop("__xorreturnloc");
            vars.remove_on_pop("__xortemp1");
            vars.remove_on_pop("__xortemp2");
            return returnloc;
        }
        else
        {
            throw(error("Linker Error: unknown builtin function " + name));
        }
        return 0;
}

void linker::link(goto_stat *sgoto)
{
    linkval temploc = vars.addvar("temp", type_int);
    write16(temploc);
    write16(temploc);
    linkval target = evaluate_or_return_literal(sgoto->target);
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
        emit_branchalways(linkval(endlabel), true);
        savelabel(elselabel, current_address);
        link(ifs->elseblock);
    }
    else
        savelabel(elselabel, current_address);
    savelabel(endlabel, current_address);
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
    std::string toplabel = makeguid("__top", (long)whiles->blk);
    std::string exitlabel = makeguid("__exit", (long)whiles->blk);
    vars.addvar(toplabel, type_label);
    vars.addvar(exitlabel, type_label);
    savelabel(toplabel, current_address);
    emit_branchifzero(evaluate(whiles->expr), exitlabel);
    link(whiles->blk);
    // jump unconditionally to top:
    emit_branchalways(linkval(toplabel), true);
    savelabel(exitlabel, current_address);
    vars.remove(toplabel);
    vars.remove(exitlabel);
    vars.pop_temp_scope();
}

void linker::link(assignment *assg)
{
    vars.push_temp_scope();
    expression targetexp;
    targetexp.type = exp_name;
    targetexp.name = assg->name;
    if (assg->indexed)
    {
        targetexp.indexed = true;
        targetexp.number = assg->index;
    }
    linkval target = evaluate(&targetexp);
    if (assg->expr->type == exp_number)
    {
        emit_writeconst_multiple(assg->expr->number, target, assg->expr->val_type.getsize());
    }
    else
    {
        type_t type;
        if (assg->expr->indexed)
            type = assg->expr->val_type.second;
        else
            type = assg->expr->val_type;
        // Pass in the target as the preferred location: if resultloc and target match, no actual copy will be emitted.
        emit_copy_multiple(evaluate(assg->expr, true, target), target, type.getsize());
    }
    vars.pop_temp_scope();
}


// Can return a literal value or a symbol: this is transparent to using functions.
// (think of a symbol as an "IOU" for an actual address: e.g. for labels we don't know the address til we reach them.)
// Always returns an address: if we need code to calculate the value (e.g. a function call) then this function emits that code
// and then returns the location where the value will be found.
// Caller can pass in a "preferred" destination, e.g. when performing assignments - if possible, result will end up there.
linkval linker::evaluate(expression *expr, bool givenpreferred, linkval preferred)
{
    if (expr->type == exp_number)
    {
        if (expr->val_type == type_int)
            return getconstaddress(expr->number);       // NOT the literal itself. If you want the actual literal (e.g. with NFC) then fetch it directly, as this is an oddball case. (SEE evaluate_or_return_literal())
        else
        {
            linkval constloc = givenpreferred? preferred : vars.addvar("__consttemp", expr->val_type);
            emit_writeconst_multiple(expr->number, constloc, expr->val_type.getsize());
            if (!givenpreferred)
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
                // eager evaluation of labels: we get the most recently bound label identity,
                // not the last one. This is what allows us to change argname meaning in different calls to a macro.
                if (valtable.find(expr->name) != valtable.end())
                    return valtable[expr->name];
                else
                    return expr->name;
            }
            else if (var->type.type == type_array)
            {
                if (expr->indexed)
                    return var->address + expr->number;
                linkval addressloc = givenpreferred? preferred : vars.addvar("__arraytemp", type_pointer);
                emit_writelabel(expr->name, addressloc);
                if (!givenpreferred)
                    vars.remove_on_pop("__arraytemp");
                return addressloc;
            }
            else
                return var->address;
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
            return linkbuiltinfunction(expr->args, expr->name, givenpreferred, preferred);
        }
        else
        {
            if (defined_funcs[expr->name]->type != dt_funcdef)
                throw(error("Error: only functions can return values (call to " + expr->name + ")"));
            return linkfunctioncall(expr->args, (funcdef*)defined_funcs[expr->name]);
        }
    }
    else if (expr->type == exp_string)
    {
        std::string stringlocation = getlabel();
        stringvalues.push_back(std::pair<std::string, std::string>(stringlocation, expr->name));    // the actual string will get linked in later, tacked onto the end of the program.
        if (givenpreferred)
        {
            emit_writelabel(stringlocation, preferred);
            return preferred;
        }
        else
        {
            linkval temploc = vars.addvar("__stringloctemp", type_pointer);
            emit_writelabel(stringlocation, temploc);
            vars.remove_on_pop("__stringloctemp");
            return temploc;
        }
    }
    else if (expr->type == exp_not)
    {
        linkval return_loc = givenpreferred ? preferred : vars.addvar("__lnottemp", type_int);
        std::string skiplabel = getlabel();
        emit_nfc2(return_loc, getconstaddress(0xff));
        emit_branchifnonzero(evaluate(expr->args[0]), skiplabel);
        emit_nfc2(return_loc, getconstaddress(~1));
        savelabel(skiplabel, current_address);
        if(!givenpreferred)
            vars.remove_on_pop("__lnottemp");
        return return_loc;
    }
    else if (expr->type == exp_and)
    {
        linkval return_loc = givenpreferred ? preferred : vars.addvar("__landtemp", type_int);
        std::string skiplabel = getlabel();
        emit_copy(evaluate(expr->args[0], true, return_loc), return_loc);
        emit_branchifzero(return_loc, skiplabel);
        emit_copy(evaluate(expr->args[1], true, return_loc), return_loc);
        savelabel(skiplabel, current_address);
        if (!givenpreferred)
            vars.remove_on_pop("__landtemp");
        return return_loc;
    }
    else if (expr->type == exp_or)
    {
        linkval return_loc = givenpreferred ? preferred : vars.addvar("__lortemp", type_int);
        std::string skiplabel = getlabel();
        emit_copy(evaluate(expr->args[0], true, return_loc), return_loc);
        emit_branchifnonzero(return_loc, skiplabel);
        emit_copy(evaluate(expr->args[1], true, return_loc), return_loc);
        savelabel(skiplabel, current_address);
        if (!givenpreferred)
            vars.remove_on_pop("__lortemp");
        return return_loc;
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
    if (image.size() > (unsigned)INCREMENT_START)
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
            if (!fdef->exported)
            {
                vars.addvar(fdef->name + ":__returnval", fdef->return_type);
                vars.addvar(fdef->name + ":__returnvector", type_pointer);
            }
            else
            {
                vars.registervar(fdef->name + ":__returnval", fdef->return_type, fdef->exportvectors[1]);
                vars.registervar(fdef->name + ":__returnvector", type_pointer, fdef->exportvectors[2]);
            }
            for (unsigned int argnum = 0; argnum < fdef->args.size(); argnum++)
            {
                if (!fdef->exported)
                    vars.addvar(fdef->args[argnum].name, fdef->args[argnum].type);  // name is already resolved to a global symbol by compiler.
                else
                    vars.registervar(fdef->args[argnum].name, fdef->args[argnum].type, fdef->exportvectors[argnum + 3]);
            }
        }
    }
}

void linker::removeunusedfunctions()
{
    markusedfunctions("main");
    std::map<std::string, definition*>::iterator fiter;
    bool noincrement = false;
    for (fiter = defined_funcs.begin(); fiter != defined_funcs.end(); fiter++)
    {
        if (noincrement)
        {
            fiter--;
            noincrement = false;
        }
        funcdef *def = (funcdef*)fiter->second;
        if (!def || def->type != dt_funcdef)
            continue;   // ignore the builtin functions.
        if (!def->is_used)
        {
#ifdef EBUG
            std::cout << "erasing function " << fiter->first << "\n";
#endif // EBUG
            defined_funcs.erase(fiter++);
            if (fiter == defined_funcs.begin())
                noincrement = true;
            else
                fiter--;
        }
        else
            std::cout << "function " << fiter->first << " is used\n";
    }
}

void linker::markusedfunctions(std::string rootfunc)
{
    std::cout << "Finding dependencies for " << rootfunc << "\n";
    funcdef *rootdef = (funcdef*)defined_funcs[rootfunc];
    rootdef->is_used = true;
    std::set<std::string> &dependencies = rootdef->dependson;
    std::cout << " - Depends on";
    for (std::set<std::string>::iterator i = dependencies.begin(); i != dependencies.end(); i++)
        std::cout << " " << *i;
    std::cout << ".\n";

    for (std::set<std::string>::iterator i = dependencies.begin(); i != dependencies.end(); i++)
    {
        funcdef *def = (funcdef*)defined_funcs[*i];
        if (!def || def->type != dt_funcdef)
            continue;
        if (!def->is_used)
        {
            markusedfunctions(def->name);
        }
    }
}

std::string linker::getdefstring()
{
    return defstring.str();
}

void linker::setcompiletoram(bool ram)
{
    compile_to_ram = ram;
    if (compile_to_ram)
        current_address = index + HEAP_BOTTOM;
    else
        current_address = index;
    vars.start_from_top = !compile_to_ram;
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








