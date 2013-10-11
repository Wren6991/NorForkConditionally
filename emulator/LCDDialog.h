#ifndef LCDDIALOG_H
#define LCDDIALOG_H

//(*Headers(LCDDialog)
#include <wx/dialog.h>
//*)

#include "LCDView.h"
#include "peripheral.h"

class LCDDialog: public wxDialog, public peripheral
{
public:

    LCDDialog(wxWindow* parent,wxWindowID id=wxID_ANY);
    virtual ~LCDDialog();

    virtual void write(unsigned int address, unsigned int data);
    virtual unsigned int read();

    //(*Declarations(LCDDialog)
    //*)

protected:

    //(*Identifiers(LCDDialog)
    //*)

private:

    //(*Handlers(LCDDialog)
    //*)

    LCDView *lcd;

    DECLARE_EVENT_TABLE()
};

#endif
