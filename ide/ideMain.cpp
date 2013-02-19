/***************************************************************
 * Name:      ideMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-16
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#include "ideMain.h"
#include <sstream>
#include <wx/msgdlg.h>
//#include <wx/util.h>
#include <stdio.h>

#include "icons/accept.xpm"
#include "icons/error.xpm"

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

extern std::string progfolder;
wxFont defaultFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas");

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
const long ideFrame::ID_NOTEBOOK1 = wxNewId();
const long ideFrame::TOOL_NEW = wxNewId();
const long ideFrame::TOOL_OPEN = wxNewId();
const long ideFrame::TOOL_SAVE = wxNewId();
const long ideFrame::TOOL_CLOSE = wxNewId();
const long ideFrame::TOOL_RUN = wxNewId();
const long ideFrame::TOOL_CONNECT = wxNewId();
const long ideFrame::TOOL_DOWNLOAD = wxNewId();
const long ideFrame::TOOL_ABOUT = wxNewId();
const long ideFrame::ID_TOOLBAR1 = wxNewId();
const long ideFrame::ID_STATUSBAR1 = wxNewId();
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
    Button1->Hide();
    BoxSizer1->Add(Button1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Pages = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
    BoxSizer1->Add(Pages, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    ToolBar1 = new wxToolBar(this, ID_TOOLBAR1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_NODIVIDER|wxNO_BORDER, _T("ID_TOOLBAR1"));
    ToolBarItem1 = ToolBar1->AddTool(TOOL_NEW, _("New"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\page_add.png"))), wxNullBitmap, wxITEM_NORMAL, _("New file"), wxEmptyString);
    ToolBarItem2 = ToolBar1->AddTool(TOOL_OPEN, _("Open"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\folder_page.png"))), wxNullBitmap, wxITEM_NORMAL, _("Open file"), wxEmptyString);
    ToolBarItem3 = ToolBar1->AddTool(TOOL_SAVE, _("Save"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\disk.png"))), wxNullBitmap, wxITEM_NORMAL, _("Right click for save as"), wxEmptyString);
    ToolBarItem4 = ToolBar1->AddTool(TOOL_CLOSE, _("Close"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\page_delete.png"))), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
    ToolBar1->AddSeparator();
    ToolBarItem5 = ToolBar1->AddTool(TOOL_RUN, _("Run"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\resultset_next.png"))), wxNullBitmap, wxITEM_NORMAL, _("Run in simulator"), wxEmptyString);
    ToolBarItem6 = ToolBar1->AddTool(TOOL_CONNECT, _("Connect"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\disconnect.png"))), wxNullBitmap, wxITEM_NORMAL, _("Connect to device"), wxEmptyString);
    ToolBarItem7 = ToolBar1->AddTool(TOOL_DOWNLOAD, _("Download"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\arrow_down.png"))), wxNullBitmap, wxITEM_NORMAL, _("Download to device"), wxEmptyString);
    ToolBar1->AddSeparator();
    ToolBarItem8 = ToolBar1->AddTool(TOOL_ABOUT, _("About"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\help.png"))), wxNullBitmap, wxITEM_NORMAL, _("About this software"), wxEmptyString);
    ToolBar1->Realize();
    SetToolBar(ToolBar1);
    dlgOpen = new wxFileDialog(this, _("Open"), wxEmptyString, wxEmptyString, _("Spoon files (*.spn;*.spoon)|*.spn;*.spoon|All files (*.*)|*.*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    dlgSaveAs = new wxFileDialog(this, _("Save As"), wxEmptyString, wxEmptyString, _("Spoon files (*.spn;*.spoon)|*.spn;*.spoon|All files (*.*)|*.*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    StatusBar = new wxStatusBar(this, ID_STATUSBAR1, wxST_SIZEGRIP, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[2] = { -10, 16 };
    int __wxStatusBarStyles_1[2] = { wxSB_NORMAL, wxSB_NORMAL };
    StatusBar->SetFieldsCount(2,__wxStatusBarWidths_1);
    StatusBar->SetStatusStyles(2,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar);
    SetSizer(BoxSizer1);
    Layout();

    Connect(DUMMY_SAVEAS_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ideFrame::OnSaveAsClicked);
    Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&ideFrame::OnPagesPageChanged);
    Connect(TOOL_NEW,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnNewClicked);
    Connect(TOOL_OPEN,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnOpenClicked);
    Connect(TOOL_SAVE,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnSaveClicked);
    Connect(TOOL_SAVE,wxEVT_COMMAND_TOOL_RCLICKED,(wxObjectEventFunction)&ideFrame::OnSaveAsClicked);
    Connect(TOOL_CLOSE,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnCloseClicked);
    Connect(TOOL_RUN,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnRunClicked);
    Connect(TOOL_ABOUT,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnAbout);
    //*)

    Connect(ID_STATUSBAR1, wxEVT_SIZE, (wxObjectEventFunction)&ideFrame::OnStatusBarResize);

    wxAcceleratorEntry entries[4];
    entries[0].Set(wxACCEL_CTRL, (int)'N', TOOL_NEW);
    entries[1].Set(wxACCEL_CTRL, (int)'O', TOOL_OPEN);
    entries[2].Set(wxACCEL_CTRL, (int)'S', TOOL_SAVE);
    entries[3].Set(wxACCEL_NORMAL, WXK_F12, DUMMY_SAVEAS_BUTTON);
    wxAcceleratorTable accel(4, entries);
    this->SetAcceleratorTable(accel);

    pagelist.push_back(page(Pages));

    Connect(pagelist[0].id, wxEVT_STC_CHANGE, (wxObjectEventFunction)&ideFrame::OnTextChange);
    Text = pagelist[0].Text;
    currentpage = 0;

    Pages->AddPage(Text, "Untitled", true);

    filename = "Untitled";
    filehaschanged = false;

    StatusBar->SetStatusText("Ready.");

    Indicator = new wxStaticBitmap(StatusBar, wxID_ANY, wxIcon(error_xpm));
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
        if (filehaschanged || filename != "Untitled")
            AddPage();
        Text->ClearAll();
        Text->LoadFile(dlgOpen->GetPath());
        Text->GotoLine(0);
        SetFileName(dlgOpen->GetPath().ToStdString());
    }
}

void ideFrame::SetFileName(std::string path, bool hasbeenmodified)
{
    filepath = path;
    int index = path.rfind("/");
    if (index < 0)
        index = path.rfind("\\");
    filename = path.substr(index + 1, path.size() - index - 1);
    this->SetTitle((hasbeenmodified? "*" : "") + filename + " - SpoonIDE");
    Pages->SetPageText(currentpage, (hasbeenmodified? "*" : "") + filename);
    pagelist[currentpage].filename = filename;
    pagelist[currentpage].filepath = filepath;
    filehaschanged = hasbeenmodified;
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
        SetFileName(pagelist[currentpage].filepath, true);
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
        SetFileName(filepath);
        Text->SaveFile(filepath);
    }
}

void ideFrame::OnNewClicked(wxCommandEvent& event)
{
    AddPage();
}

void ideFrame::AddPage()
{
    pagelist.push_back(page(Pages));
    Pages->AddPage(pagelist[pagelist.size() - 1].Text, "", true);   // this invokes the page change event, which sets up the currentpage and stuff.
    Connect(pagelist[currentpage].id, wxEVT_STC_CHANGE, (wxObjectEventFunction)&ideFrame::OnTextChange);
}

void ideFrame::CloseCurrentPage()
{
    pagelist.erase(pagelist.begin() + currentpage);
    Pages->RemovePage(currentpage);
}

void ideFrame::OnRunClicked(wxCommandEvent& event)
{
    OnSaveClicked(event);
    FILE *pProcess;
    if (pProcess = popen((progfolder + "/spoon \"" + filepath + "\" \"" + filepath + ".bin\"").c_str(), "r"))
    {
        std::string message;
        char messagebuffer[256];
        while (!feof(pProcess))
        {
            if (fgets(messagebuffer, 256, pProcess))
                message += messagebuffer;
        }
        if (message == "")
        {
            StatusBar->SetStatusText("Done.");
        }
        else
        {
            StatusBar->SetStatusText(message);
            wxBell();
        }
        pclose(pProcess);
    }
}

page::page(wxWindow *parent)
{
    id = wxNewId();

    Text = new wxStyledTextCtrl(parent, id);

    Text->SetMarginWidth(MARGIN_LINE_NUMBERS, 40);
    Text->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(75, 75, 75));
    Text->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour(220, 220, 220));
    Text->SetMarginType(MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);
    Text->SetTabWidth(4);
    Text->SetUseTabs(false);

    Text->StyleSetFont(wxSTC_STYLE_DEFAULT, defaultFont);
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

    filename = "Untitled";
    filepath = "Untitled";
    filehaschanged = false;
}


void ideFrame::OnPagesPageChanged(wxNotebookEvent& event)
{
    currentpage = event.GetSelection();
    if (currentpage >= 0)
    {
        SetFileName(pagelist[currentpage].filepath, pagelist[currentpage].filehaschanged);
        Text = pagelist[currentpage].Text;
    }
}

void ideFrame::OnCloseClicked(wxCommandEvent& event)
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
                CloseCurrentPage();
            }
        }
        else if (result == wxID_NO)
        {
            CloseCurrentPage();
        }
    }
    else
    {
        CloseCurrentPage();
    }

}

void ideFrame::OnStatusBarResize(wxSizeEvent &event)
{
    wxMessageBox("Wassup!");
    wxRect rect;
    StatusBar->GetFieldRect(1, rect);
    wxSize size = Indicator->GetSize();
    Indicator->Move(rect.GetX() + (rect.GetWidth() - size.GetX()) / 2, rect.GetY() + (rect.GetHeight() - size.GetY()) / 2);
}


