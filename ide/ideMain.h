/***************************************************************
 * Name:      ideMain.h
 * Purpose:   Defines Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-16
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#ifndef IDEMAIN_H
#define IDEMAIN_H

//(*Headers(ideFrame)
#include <wx/sizer.h>
#include <wx/toolbar.h>
#include <wx/frame.h>
//*)
#include <wx/stc/stc.h>

class ideFrame: public wxFrame
{
    public:

        ideFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~ideFrame();

    private:

        //(*Handlers(ideFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        //*)

        //(*Identifiers(ideFrame)
        static const long TOOL_NEW;
        static const long TOOL_OPEN;
        static const long TOOL_SAVE;
        static const long TOOL_RUN;
        static const long TOOL_CONNECT;
        static const long TOOL_DOWNLOAD;
        static const long TOOL_ABOUT;
        static const long ID_TOOLBAR1;
        //*)
        static const long ID_EDITCTRL;

        //(*Declarations(ideFrame)
        wxToolBarToolBase* ToolBarItem4;
        wxToolBar* ToolBar1;
        wxToolBarToolBase* ToolBarItem3;
        wxToolBarToolBase* ToolBarItem6;
        wxToolBarToolBase* ToolBarItem1;
        wxToolBarToolBase* ToolBarItem5;
        wxToolBarToolBase* ToolBarItem2;
        wxToolBarToolBase* ToolBarItem7;
        //*)
        wxStyledTextCtrl *Text;

        DECLARE_EVENT_TABLE()
};

#endif // IDEMAIN_H
