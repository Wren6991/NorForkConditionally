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
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/toolbar.h>
#include <wx/filedlg.h>
#include <wx/button.h>
#include <wx/frame.h>
//*)
#include <vector>
#include <wx/statbmp.h>
#include <wx/stc/stc.h>

#include "LogDialog.h"
#include "OptionsDialog.h"

struct page
{
    wxStyledTextCtrl *Text;
    std::string filename;
    std::string filepath;
    bool filehaschanged;
    long id;
    page(wxWindow *parent);
};

class MyStatusBar: public wxStatusBar
{
public:
    wxStaticBitmap *Indicator;
    MyStatusBar(wxWindow *parent, long id = wxID_ANY, long style = wxSTB_DEFAULT_STYLE, wxString name = wxStatusBarNameStr);
    virtual ~MyStatusBar();
    void OnSize(wxSizeEvent &event);

    DECLARE_EVENT_TABLE()
};

class ideFrame: public wxFrame
{
    public:

        ideFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~ideFrame();

    private:

        //(*Handlers(ideFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnOpenClicked(wxCommandEvent& event);
        void OnSaveAsClicked(wxCommandEvent& event);
        void OnSaveClicked(wxCommandEvent& event);
        void OnNewClicked(wxCommandEvent& event);
        void OnRunClicked(wxCommandEvent& event);
        void OnPagesPageChanged(wxNotebookEvent& event);
        void OnCloseClicked(wxCommandEvent& event);
        void OnPagesPageChanged1(wxNotebookEvent& event);
        void OnToolBarMessageLogClicked(wxCommandEvent& event);
        void OnToolBarOptionsClicked(wxCommandEvent& event);
        void OnBuildClicked(wxCommandEvent& event);
        //*)
        void OnTextChange(wxStyledTextEvent &event);

        //(*Identifiers(ideFrame)
        static const long ID_NOTEBOOK1;
        static const long DUMMY_SAVEAS_BUTTON;
        static const long TOOL_NEW;
        static const long TOOL_OPEN;
        static const long TOOL_SAVE;
        static const long TOOL_CLOSE;
        static const long TOOL_BUILD;
        static const long TOOL_OPTIONS;
        static const long TOOL_LOG;
        static const long TOOL_RUN;
        static const long TOOL_ABOUT;
        static const long ID_TOOLBAR1;
        //*)
        static const long ID_TEXT;
        static const long ID_STATUSBAR;

        //(*Declarations(ideFrame)
        wxToolBarToolBase* ToolBarItem4;
        wxToolBar* ToolBar1;
        wxToolBarToolBase* ToolBarItem9;
        wxToolBarToolBase* ToolBarItem3;
        wxButton* Button1;
        wxFileDialog* dlgOpen;
        wxToolBarToolBase* ToolBarItem6;
        wxToolBarToolBase* ToolBarItem1;
        wxToolBarToolBase* ToolBarItem5;
        wxToolBarToolBase* ToolBarItem8;
        wxFileDialog* dlgSaveAs;
        wxToolBarToolBase* ToolBarItem2;
        wxNotebook* Pages;
        wxToolBarToolBase* ToolBarItem7;
        //*)
        wxStyledTextCtrl *Text;
        MyStatusBar *StatusBar;
        LogDialog *Log;
        OptionsDialog *Options;

        bool filehaschanged;
        std::string filepath;
        std::string filename;
        std::vector <page> pagelist;
        int currentpage;

        void LogMessage(std::string message, bool nostatus = false);
        void SetFileName(std::string path, bool hasbeenmodified = false);
        void AddPage();
        void CloseCurrentPage();
        void OnStatusBarResize(wxSizeEvent &event);
        bool BuildFile(std::string filename, bool strip, bool exportdefs, bool compiletoram);

        DECLARE_EVENT_TABLE()
};

#endif // IDEMAIN_H
