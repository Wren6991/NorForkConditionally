/***************************************************************
 * Name:      ideApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-16
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#include "ideApp.h"
#include <string>

//(*AppHeaders
#include "ideMain.h"
#include <wx/image.h>
//*)


IMPLEMENT_APP(ideApp);

std::string progfolder;

bool ideApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	ideFrame* Frame = new ideFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    std::string progpath = argv[0].ToStdString();
    int slashpos = progpath.rfind("/");
    if (slashpos < 0)
        slashpos = progpath.rfind("\\");
    progfolder = progpath.substr(0, slashpos);

    return wxsOK;
}
