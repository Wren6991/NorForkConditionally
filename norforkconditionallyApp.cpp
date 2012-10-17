/***************************************************************
 * Name:      norforkconditionallyApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2012-10-16
 * Copyright: Luke Wren ()
 * License:
 **************************************************************/

#include "norforkconditionallyApp.h"

//(*AppHeaders
#include "norforkconditionallyMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(norforkconditionallyApp);

bool norforkconditionallyApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	norforkconditionallyFrame* Frame = new norforkconditionallyFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
