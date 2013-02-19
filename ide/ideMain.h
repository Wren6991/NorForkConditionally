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
#include <wx/statusbr.h>
//*)
#include <vector>
#include <wx/statbmp.h>
#include <wx/stc/stc.h>

struct page
{
    wxStyledTextCtrl *Text;
    std::string filename;
    std::string filepath;
    bool filehaschanged;
    long id;
    page(wxWindow *parent);
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
        //*)
        void OnTextChange(wxStyledTextEvent &event);

        //(*Identifiers(ideFrame)
        static const long DUMMY_SAVEAS_BUTTON;
        static const long ID_NOTEBOOK1;
        static const long TOOL_NEW;
        static const long TOOL_OPEN;
        static const long TOOL_SAVE;
        static const long TOOL_CLOSE;
        static const long TOOL_RUN;
        static const long TOOL_CONNECT;
        static const long TOOL_DOWNLOAD;
        static const long TOOL_ABOUT;
        static const long ID_TOOLBAR1;
        static const long ID_STATUSBAR1;
        //*)
        static const long ID_TEXT;

        //(*Declarations(ideFrame)
        wxToolBarToolBase* ToolBarItem4;
        wxToolBar* ToolBar1;
        wxToolBarToolBase* ToolBarItem3;
        wxButton* Button1;
        wxFileDialog* dlgOpen;
        wxToolBarToolBase* ToolBarItem6;
        wxToolBarToolBase* ToolBarItem1;
        wxToolBarToolBase* ToolBarItem5;
        wxToolBarToolBase* ToolBarItem8;
        wxFileDialog* dlgSaveAs;
        wxStatusBar* StatusBar;
        wxToolBarToolBase* ToolBarItem2;
        wxNotebook* Pages;
        wxToolBarToolBase* ToolBarItem7;
        //*)
        wxStyledTextCtrl *Text;
        wxStaticBitmap *Indicator;

        bool filehaschanged;
        std::string filepath;
        std::string filename;
        std::vector <page> pagelist;
        int currentpage;

        void SetFileName(std::string path, bool hasbeenmodified = false);
        void AddPage();
        void CloseCurrentPage();
        void OnStatusBarResize(wxSizeEvent &event);

        DECLARE_EVENT_TABLE()
};

#endif // IDEMAIN_H
