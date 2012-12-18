class parser
{
    std::vector<token> tokens;
    int index;
    token t;
    token lastt;
    void gettoken();
    bool accept(token t);
    void expect(token t);
    public:
    parser(std::vector<token> tokens_);
    // parse methods follow
}

typedef enum val_type
{
    val_type_name = 0
} val_type;

struct value
{
    val_type type;
    std::string val;
};

typedef enum cmd_type
{
    cmd_type_invalid = 0;
    cmd_type_four,
    cmd_type_two,
    cmd_type_call,
    cmd_type_funcdef,
    cmd_type_valdef,
    cmd_type_vardec,
    cmd_type_dat,
    cmd_type_buffer,
    cmd_type_block;
}

struct command
{
    cmd_type type;
    virtual command() = 0;
    virtual ~command() = 0;
};

struct command_block;

struct command_four: public command
{
    value *a, *b, *c, *d;
};

struct command_two: public command
{
    value *a, *b;
}

struct command_call: public command
{
    std::string name;
    std::vector<value*> args;
}

struct command_funcdef: public command
{
    std::string name;
    std::vector<std::string> argnames;
    command_block *body;
}

struct command_valdef: public command
{
    std::string name;
    value *val;
}

struct command_vardec: public command
{
    std::vector<std::string> names;
}
