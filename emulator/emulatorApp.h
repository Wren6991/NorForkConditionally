/***************************************************************
 * Name:      emulatorApp.h
 * Purpose:   Defines Application Class
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-21
 * Cop
 ight: Luke Wren (http://githhub.com/Wren6991)
 * License:
 **************************************************************/

#ifndef EMULATORAPP_H
#define EMULATORAPP_H

#include <wx/app.h>

#include "emulatorMain.h"

class emulatorApp : public wxApp
{
    emulatorFrame *Frame;
public:
    virtual bool OnInit();
    virtual int FilterEvent(wxEvent &event);
};

#endif // EMULATORAPP_H
