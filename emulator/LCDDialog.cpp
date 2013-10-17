#include "LCDDialog.h"

#include <iostream>
#include <wx/intl.h>
#include <wx/string.h>

BEGIN_EVENT_TABLE(LCDDialog,wxDialog)

END_EVENT_TABLE()

LCDDialog::LCDDialog(wxWindow* parent,wxWindowID id)
{
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(320, 80));
	lcd = new LCDView((wxFrame*)this);
}

LCDDialog::~LCDDialog()
{
    delete lcd;
}

const uint8_t clk_bit = 0x80;
const uint8_t rs_bit = 0x40;
void LCDDialog::write(unsigned int address, unsigned int data)
{
    static uint8_t lastdata = 0;
    static bool need4bits = false;
    static bool in4bitmode = false;
    static uint8_t last4bits = 0x0;
    static uint8_t cursorpos = 0;
    if (address != 0xc000)
        return;
    if ((lastdata & clk_bit) || !(data & clk_bit)) // not a rising clock edge
    {
        lastdata = data;
        return;
    }
    if (data & rs_bit) // data write
    {
        if (need4bits)
        {
            if (cursorpos < LCD_N_CHARS)
                lcd->buffer[cursorpos] = (data & 0x0f) | (last4bits << 4);
            cursorpos++;
            Refresh();
        }
        else
        {
            last4bits = data & 0x0f;
        }
        need4bits = !need4bits;
    }
    else // command
    {
        uint8_t cmd = data & 0x0f;
        if (need4bits)
        {
            uint8_t fullcmd = (last4bits << 4) | cmd;
            if (fullcmd == 0x01)
            {
                for (int i = 0; i < LCD_N_CHARS; i++)
                    lcd->buffer[i] = ' ';
            }
            else if (fullcmd & 0x80)
            {
                uint8_t cursaddress = fullcmd & 0x7f;
                if (cursaddress < 20)
                    cursorpos = cursaddress;
                else if (cursaddress >= 64 and cursaddress < 84)
                    cursorpos = cursaddress - 64 + 20;
                else if (cursaddress >= 20 and cursaddress < 64)
                    cursorpos = cursorpos - 20 + 20;
                else if (cursaddress >= 84)
                    cursorpos = cursaddress - 84 + 60;
            }
            need4bits = false;
        }
        else
        {
            if (cmd == 0x03)
                in4bitmode = false;
            else if (cmd == 0x02)
                if (in4bitmode)
                    need4bits = true;
                else
                    in4bitmode = true;
            else if (in4bitmode)
            {
                last4bits = cmd;
                need4bits = true;
            }
        }
    }

    lastdata = data;
}

unsigned int LCDDialog::read(unsigned int address)
{
    return 0;
}
