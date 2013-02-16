/***************************************************************
 * Name:      ideMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-16
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#include "ideMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(ideFrame)
#include <wx/bitmap.h>
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

enum
{
    MARGIN_LINE_NUMBERS,
    MARGIN_FOLD
};

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(ideFrame)
const long ideFrame::TOOL_NEW = wxNewId();
const long ideFrame::TOOL_OPEN = wxNewId();
const long ideFrame::TOOL_SAVE = wxNewId();
const long ideFrame::TOOL_RUN = wxNewId();
const long ideFrame::TOOL_CONNECT = wxNewId();
const long ideFrame::TOOL_DOWNLOAD = wxNewId();
const long ideFrame::TOOL_ABOUT = wxNewId();
const long ideFrame::ID_TOOLBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ideFrame,wxFrame)
    //(*EventTable(ideFrame)
    //*)
END_EVENT_TABLE()

ideFrame::ideFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(ideFrame)
    wxBoxSizer* BoxSizer1;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(800,600));
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    SetSizer(BoxSizer1);
    ToolBar1 = new wxToolBar(this, ID_TOOLBAR1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_NODIVIDER|wxNO_BORDER, _T("ID_TOOLBAR1"));
    ToolBarItem1 = ToolBar1->AddTool(TOOL_NEW, _("New"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\page_add.png"))), wxNullBitmap, wxITEM_NORMAL, _("New file"), wxEmptyString);
    ToolBarItem2 = ToolBar1->AddTool(TOOL_OPEN, _("Open"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\folder_page.png"))), wxNullBitmap, wxITEM_NORMAL, _("Open file"), wxEmptyString);
    ToolBarItem3 = ToolBar1->AddTool(TOOL_SAVE, _("Save"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\disk.png"))), wxNullBitmap, wxITEM_NORMAL, _("Right click for save as"), wxEmptyString);
    ToolBar1->AddSeparator();
    ToolBarItem4 = ToolBar1->AddTool(TOOL_RUN, _("Run"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\resultset_next.png"))), wxNullBitmap, wxITEM_NORMAL, _("Run in simulator"), wxEmptyString);
    ToolBarItem5 = ToolBar1->AddTool(TOOL_CONNECT, _("Connect"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\disconnect.png"))), wxNullBitmap, wxITEM_NORMAL, _("Connect to device"), wxEmptyString);
    ToolBarItem6 = ToolBar1->AddTool(TOOL_DOWNLOAD, _("Download"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\arrow_down.png"))), wxNullBitmap, wxITEM_NORMAL, _("Download to device"), wxEmptyString);
    ToolBar1->AddSeparator();
    ToolBarItem7 = ToolBar1->AddTool(TOOL_ABOUT, _("About"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\help.png"))), wxNullBitmap, wxITEM_NORMAL, _("About this software"), wxEmptyString);
    ToolBar1->Realize();
    SetToolBar(ToolBar1);
    SetSizer(BoxSizer1);
    Layout();

    Connect(TOOL_ABOUT,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnAbout);
    //*)
    Text = new wxStyledTextCtrl(this);
    Text->SetMarginWidth(MARGIN_LINE_NUMBERS, 40);
    Text->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(75, 75, 75));
    Text->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour(220, 220, 220));
    Text->SetMarginType(MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);
    Text->SetTabWidth(4);
    Text->SetUseTabs(false);

    /*EditCtrl->SetMarginType(MARGIN_FOLD, wxSTC_MARGIN_SYMBOL);
    EditCtrl->SetMarginWidth(MARGIN_FOLD, 10);
    EditCtrl->SetMarginMask (MARGIN_FOLD, wxSTC_MASK_FOLDERS);
    EditCtrl->StyleSetBackground(MARGIN_FOLD, wxColour(200, 200, 200));
    EditCtrl->SetMarginSensitive(MARGIN_FOLD, true);*/

    wxFont font(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas");

    Text->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    Text->StyleClearAll();
    Text->SetLexer(wxSTC_LEX_CPP);
    Text->StyleSetForeground (wxSTC_C_STRING,            wxColour(204, 0, 0));
    Text->StyleSetForeground (wxSTC_C_PREPROCESSOR,      wxColour(165, 105, 0));
    Text->StyleSetForeground (wxSTC_C_IDENTIFIER,        wxColour(40, 0, 60));
    Text->StyleSetForeground (wxSTC_C_NUMBER,            wxColour(128, 80, 144));
    Text->StyleSetForeground (wxSTC_C_CHARACTER,         wxColour(128, 0, 0));
    Text->StyleSetForeground (wxSTC_C_WORD,              wxColour(32, 96, 160));
    Text->StyleSetForeground (wxSTC_C_WORD2,             wxColour(160, 96, 64));
    Text->StyleSetForeground (wxSTC_C_COMMENT,           wxColour(160, 160, 160));
    Text->StyleSetForeground (wxSTC_C_COMMENTLINE,       wxColour(176, 176, 224));
    Text->StyleSetForeground (wxSTC_C_COMMENTDOC,        wxColour(150, 150, 150));
    Text->StyleSetForeground (wxSTC_C_COMMENTDOCKEYWORD, wxColour(0, 0, 200));
    Text->StyleSetForeground (wxSTC_C_COMMENTDOCKEYWORDERROR, wxColour(0, 0, 200));
    Text->StyleSetBold(wxSTC_C_WORD, true);
    Text->StyleSetBold(wxSTC_C_WORD2, true);
    Text->StyleSetBold(wxSTC_C_COMMENTDOCKEYWORD, true);

    Text->SetKeyWords(0, wxT("break const continue do else false for function goto if macro nfc return static true var while"));
    Text->SetKeyWords(1, wxT("int pointer void"));

    BoxSizer1->Add(Text, 1, wxEXPAND);
}

ideFrame::~ideFrame()
{
    //(*Destroy(ideFrame)
    //*)
}

void ideFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void ideFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox(wxT("Spoon IDE version 0.0\nCopyright (c) Luke Wren 2013\nhttp://github.com/Wren6991\n\nLicensed under Creative Commons Attribution Sharealike"), _("About this Software"));
}
