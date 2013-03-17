/***************************************************************
 * Name:      emulatorApp.h
 * Purpose:   Defines Application Class
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-21
 * Copyright: Luke Wren (http://githhub.com/Wren6991)
 * License:
 **************************************************************/

#ifndef EMULATORAPP_H
#define EMULATORAPP_H

#include <wx/app.h>

class emulatorApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // EMULATORAPP_H
