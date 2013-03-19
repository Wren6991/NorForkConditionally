/***************************************************************
 * Name:      emulatorApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-21
 * Copyright: Luke Wren (http://githhub.com/Wren6991)
 * License:
 **************************************************************/

#include "emulatorApp.h"

//(*AppHeaders
#include "emulatorMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(emulatorApp);

bool emulatorApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        emulatorFrame* Frame = new emulatorFrame(0);
        Frame->Show();
        SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
