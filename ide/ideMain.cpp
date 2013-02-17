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
const long ideFrame::DUMMY_SAVEAS_BUTTON = wxNewId();
const long ideFrame::TOOL_NEW = wxNewId();
const long ideFrame::TOOL_OPEN = wxNewId();
const long ideFrame::TOOL_SAVE = wxNewId();
const long ideFrame::TOOL_RUN = wxNewId();
const long ideFrame::TOOL_CONNECT = wxNewId();
const long ideFrame::TOOL_DOWNLOAD = wxNewId();
const long ideFrame::TOOL_ABOUT = wxNewId();
const long ideFrame::ID_TOOLBAR1 = wxNewId();
//*)
const long ideFrame::ID_TEXT = wxNewId();

BEGIN_EVENT_TABLE(ideFrame,wxFrame)
    //(*EventTable(ideFrame)
    //*)
END_EVENT_TABLE()

ideFrame::ideFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(ideFrame)
    wxBoxSizer* BoxSizer1;

    Create(parent, wxID_ANY, _("SpoonIDE - Untitled"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(800,600));
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    Button1 = new wxButton(this, DUMMY_SAVEAS_BUTTON, _("Label"), wxDefaultPosition, wxSize(0,0), 0, wxDefaultValidator, _T("DUMMY_SAVEAS_BUTTON"));
    BoxSizer1->Add(Button1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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
    dlgOpen = new wxFileDialog(this, _("Open"), wxEmptyString, wxEmptyString, _("Spoon files (*.spn;*.spoon)|*.spn;*.spoon|All files (*.*)|*.*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    dlgSaveAs = new wxFileDialog(this, _("Save As"), wxEmptyString, wxEmptyString, _("Spoon files (*.spn;*.spoon)|*.spn;*.spoon|All files (*.*)|*.*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    SetSizer(BoxSizer1);
    Layout();

    Connect(DUMMY_SAVEAS_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ideFrame::OnSaveAsClicked);
    Connect(TOOL_NEW,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnNewClicked);
    Connect(TOOL_OPEN,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnOpenClicked);
    Connect(TOOL_SAVE,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnSaveClicked);
    Connect(TOOL_SAVE,wxEVT_COMMAND_TOOL_RCLICKED,(wxObjectEventFunction)&ideFrame::OnSaveAsClicked);
    Connect(TOOL_RUN,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnRunClicked);
    Connect(TOOL_ABOUT,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnAbout);
    //*)
    wxAcceleratorEntry entries[4];
    entries[0].Set(wxACCEL_CTRL, (int)'N', TOOL_NEW);
    entries[1].Set(wxACCEL_CTRL, (int)'O', TOOL_OPEN);
    entries[2].Set(wxACCEL_CTRL, (int)'S', TOOL_SAVE);
    entries[3].Set(wxACCEL_NORMAL, WXK_F12, DUMMY_SAVEAS_BUTTON);
    wxAcceleratorTable accel(4, entries);
    this->SetAcceleratorTable(accel);

    Text = new wxStyledTextCtrl(this, ID_TEXT);
    Text->SetMarginWidth(MARGIN_LINE_NUMBERS, 40);
    Text->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(75, 75, 75));
    Text->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour(220, 220, 220));
    Text->SetMarginType(MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);
    Text->SetTabWidth(4);
    Text->SetUseTabs(false);

    Connect(ID_TEXT, wxEVT_STC_CHANGE, (wxObjectEventFunction)&ideFrame::OnTextChange);

    wxFont font(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas");

    Text->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    Text->StyleClearAll();
    Text->SetLexer(wxSTC_LEX_CPP);
    Text->StyleSetForeground(wxSTC_C_STRING,            wxColour(204, 0, 0));
    Text->StyleSetForeground(wxSTC_C_PREPROCESSOR,      wxColour(165, 105, 0));
    Text->StyleSetForeground(wxSTC_C_IDENTIFIER,        wxColour(40, 0, 60));
    Text->StyleSetForeground(wxSTC_C_NUMBER,            wxColour(128, 80, 144));
    Text->StyleSetForeground(wxSTC_C_CHARACTER,         wxColour(128, 0, 0));
    Text->StyleSetForeground(wxSTC_C_WORD,              wxColour(32, 96, 160));
    Text->StyleSetForeground(wxSTC_C_WORD2,             wxColour(160, 96, 64));
    Text->StyleSetForeground(wxSTC_C_COMMENT,           wxColour(160, 160, 160));
    Text->StyleSetForeground(wxSTC_C_COMMENTLINE,       wxColour(176, 176, 224));
    Text->StyleSetForeground(wxSTC_C_COMMENTDOC,        wxColour(150, 150, 150));
    Text->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORD, wxColour(0, 0, 200));
    Text->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORDERROR, wxColour(0, 0, 200));
    Text->StyleSetBold(wxSTC_C_WORD, true);
    Text->StyleSetBold(wxSTC_C_WORD2, true);
    Text->StyleSetBold(wxSTC_C_COMMENTDOCKEYWORD, true);

    Text->SetKeyWords(0, wxT("break const continue do else false for function goto if macro nfc return static true var while"));
    Text->SetKeyWords(1, wxT("int pointer void"));

    BoxSizer1->Add(Text, 1, wxEXPAND);

    filename = "Untitled";
    filehaschanged = false;
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

void ideFrame::OnOpenClicked(wxCommandEvent& event)
{
    if (dlgOpen->ShowModal() == wxID_OK)
    {
        Text->ClearAll();
        Text->LoadFile(dlgOpen->GetPath());
        Text->GotoLine(0);
        SetFileName(dlgOpen->GetPath().ToStdString());
    }
}

void ideFrame::SetFileName(std::string path)
{
    filepath = path;
    int index = path.rfind("/");
    if (index < 0)
        index = path.rfind("\\");
    filename = path.substr(index + 1, path.size() - index - 1);
    this->SetTitle("SpoonIDE - " + filename);
    filehaschanged = false;
}

void ideFrame::OnSaveAsClicked(wxCommandEvent& event)
{
    if (dlgSaveAs->ShowModal() == wxID_OK)
    {
        Text->SaveFile(dlgSaveAs->GetPath());
        SetFileName(dlgSaveAs->GetPath().ToStdString());
    }
}

void ideFrame::OnTextChange(wxStyledTextEvent &event)
{
    if (!filehaschanged)
    {
        filehaschanged = true;
        this->SetTitle("SpoonIDE - " + filename + "*");
    }
}

void ideFrame::OnSaveClicked(wxCommandEvent& event)
{
    if (filename == "Untitled")
    {
        wxCommandEvent evt;
        OnSaveAsClicked(evt);
    }
    else if (filehaschanged)
    {
        Text->SaveFile(filepath);
        filehaschanged = false;
        this->SetTitle("SpoonIDE - " + filename);
    }
}

void ideFrame::OnNewClicked(wxCommandEvent& event)
{
    if (filehaschanged)
    {
        wxMessageDialog dlg(this, "Save changes to " + filename + "?", "New File", wxYES_NO | wxCANCEL | wxICON_EXCLAMATION);
        int result = dlg.ShowModal();
        if (result == wxID_YES)
        {
            wxCommandEvent evt;
            OnSaveClicked(evt);
            if (!filehaschanged)    // check that the changes have actually been saved (user did not cancel saveas dialog)
            {
                NewBuffer();
            }
        }
        else if (result == wxID_NO)
        {
            NewBuffer();
        }
    }
    else
    {
        NewBuffer();
    }
}

void ideFrame::NewBuffer()
{
    Text->ClearAll();
    SetFileName("Untitled");
}

void ideFrame::OnRunClicked(wxCommandEvent& event)
{

}
