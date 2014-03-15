/***************************************************************
 * Name:      ideMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-16
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#ifdef _WIN32
    #define FOLDER_SEP "\\"
#else
    #define FOLDER_SEP "/";
#endif // _WIN32

#include "ideMain.h"
#include <sstream>
#include <wx/msgdlg.h>
//#include <wx/util.h>
#include <stdio.h>

/*#include "icons/accept.xpm"
#include "icons/error.xpm"*/

//(*InternalHeaders(ideFrame)
#include <wx/bitmap.h>
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)
#include <wx/listctrl.h>

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
const long ideFrame::ID_NOTEBOOK1 = wxNewId();
const long ideFrame::DUMMY_SAVEAS_BUTTON = wxNewId();
const long ideFrame::TOOL_NEW = wxNewId();
const long ideFrame::TOOL_OPEN = wxNewId();
const long ideFrame::TOOL_SAVE = wxNewId();
const long ideFrame::TOOL_CLOSE = wxNewId();
const long ideFrame::TOOL_BUILD = wxNewId();
const long ideFrame::TOOL_OPTIONS = wxNewId();
const long ideFrame::TOOL_LOG = wxNewId();
const long ideFrame::TOOL_RUN = wxNewId();
const long ideFrame::TOOL_ABOUT = wxNewId();
const long ideFrame::ID_TOOLBAR1 = wxNewId();
//*)
const long ideFrame::ID_TEXT = wxNewId();
const long ideFrame::ID_STATUSBAR = wxNewId();

BEGIN_EVENT_TABLE(ideFrame,wxFrame)
    //(*EventTable(ideFrame)
    //*)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyStatusBar, wxStatusBar)
    EVT_SIZE(MyStatusBar::OnSize)
END_EVENT_TABLE()

ideFrame::ideFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(ideFrame)
    wxBoxSizer* BoxSizer1;

    Create(parent, wxID_ANY, _("SpoonIDE - Untitled"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(800,600));
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    Pages = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
    BoxSizer1->Add(Pages, 5, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button1 = new wxButton(this, DUMMY_SAVEAS_BUTTON, _("Label"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("DUMMY_SAVEAS_BUTTON"));
    Button1->Hide();
    BoxSizer1->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    ToolBar1 = new wxToolBar(this, ID_TOOLBAR1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_NODIVIDER|wxNO_BORDER, _T("ID_TOOLBAR1"));
    ToolBarItem1 = ToolBar1->AddTool(TOOL_NEW, _("Message Log"), wxBitmap(wxImage(_T("icons\\page_add.png"))), wxNullBitmap, wxITEM_NORMAL, _("Message log"), wxEmptyString);
    ToolBarItem2 = ToolBar1->AddTool(TOOL_OPEN, _("Open"), wxBitmap(wxImage(_T("icons\\folder_page.png"))), wxNullBitmap, wxITEM_NORMAL, _("Open file"), wxEmptyString);
    ToolBarItem3 = ToolBar1->AddTool(TOOL_SAVE, _("Save"), wxBitmap(wxImage(_T("icons\\disk.png"))), wxNullBitmap, wxITEM_NORMAL, _("Right click for save as"), wxEmptyString);
    ToolBarItem4 = ToolBar1->AddTool(TOOL_CLOSE, _("Close"), wxBitmap(wxImage(_T("icons\\page_delete.png"))), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
    ToolBar1->AddSeparator();
    ToolBarItem6 = ToolBar1->AddTool(TOOL_BUILD, _("Build"), wxBitmap(wxImage(_T("icons\\cog_add.png"))), wxNullBitmap, wxITEM_NORMAL, _("Build files"), wxEmptyString);
    ToolBarItem7 = ToolBar1->AddTool(TOOL_OPTIONS, _("Options"), wxBitmap(wxImage(_T("icons/wrench.png"))), wxNullBitmap, wxITEM_NORMAL, _("Build options"), wxEmptyString);
    ToolBarItem9 = ToolBar1->AddTool(TOOL_LOG, _("Message Log"), wxBitmap(wxImage(_T("icons/comments.png"))), wxNullBitmap, wxITEM_NORMAL, _("Message log"), wxEmptyString);
    ToolBarItem5 = ToolBar1->AddTool(TOOL_RUN, _("Run"), wxBitmap(wxImage(_T("icons\\resultset_next.png"))), wxNullBitmap, wxITEM_NORMAL, _("Run in simulator"), wxEmptyString);
    ToolBar1->AddSeparator();
    ToolBarItem8 = ToolBar1->AddTool(TOOL_ABOUT, _("About"), wxBitmap(wxImage(_T("icons\\help.png"))), wxNullBitmap, wxITEM_NORMAL, _("About this software"), wxEmptyString);
    ToolBar1->Realize();
    SetToolBar(ToolBar1);
    dlgOpen = new wxFileDialog(this, _("Open"), wxEmptyString, wxEmptyString, _("Spoon files (*.spn;*.spoon)|*.spn;*.spoon|All files (*.*)|*.*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    dlgSaveAs = new wxFileDialog(this, _("Save As"), wxEmptyString, wxEmptyString, _("Spoon files (*.spn;*.spoon)|*.spn;*.spoon|All files (*.*)|*.*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    SetSizer(BoxSizer1);
    Layout();

    Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&ideFrame::OnPagesPageChanged);
    Connect(DUMMY_SAVEAS_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ideFrame::OnSaveAsClicked);
    Connect(TOOL_NEW,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnNewClicked);
    Connect(TOOL_OPEN,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnOpenClicked);
    Connect(TOOL_SAVE,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnSaveClicked);
    Connect(TOOL_SAVE,wxEVT_COMMAND_TOOL_RCLICKED,(wxObjectEventFunction)&ideFrame::OnSaveAsClicked);
    Connect(TOOL_CLOSE,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnCloseClicked);
    Connect(TOOL_BUILD,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnBuildClicked);
    Connect(TOOL_OPTIONS,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnToolBarOptionsClicked);
    Connect(TOOL_LOG,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&ideFrame::OnToolBarMessageLogClicked);
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

    pagelist.push_back(page(Pages));

    Connect(pagelist[0].id, wxEVT_STC_CHANGE, (wxObjectEventFunction)&ideFrame::OnTextChange);
    Text = pagelist[0].Text;
    currentpage = 0;

    Pages->AddPage(Text, "Untitled", true);

    filename = "Untitled";
    filehaschanged = false;

    StatusBar = new MyStatusBar(this, ID_STATUSBAR, wxST_SIZEGRIP, _T("ID_STATUSBAR"));
    SetStatusBar(StatusBar);

    Options = new OptionsDialog(this);
    Log = new LogDialog(this);

    LogMessage("Ready.");
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
        long index = Options->BuildOrder->GetListCtrl()->GetItemCount();
        Options->BuildOrder->GetListCtrl()->InsertItem(index, pagelist[currentpage].filename);
        wxListEvent evt;
        evt.m_itemIndex = index;
        Options->OnBuildOrderSelect(evt);
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
        Options->BuildOrder->GetListCtrl()->InsertItem(0, pagelist[currentpage].filename);
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
    wxListCtrl *buildlist = Options->BuildOrder->GetListCtrl();
    long index = buildlist->FindItem(0, pagelist[currentpage].filename);
    Options->SetFocus();
    if (index != -1)
        buildlist->DeleteItem(index);
    pagelist.erase(pagelist.begin() + currentpage);
    Pages->RemovePage(currentpage);
}

void ideFrame::LogMessage(std::string message, bool nostatus)
{
    if (!nostatus)
        StatusBar->SetStatusText(message);
    *Log->LogText << wxString(message) << "\n";
    Log->LogText->ShowPosition(Log->LogText->GetLastPosition());
    if (message.substr(0, 5) == "Error")
    {
        wxBell();
        //StatusBar->Indicator->SetIcon(wxIcon(error_xpm));
    }
    else
    {
        wxBell();
        //StatusBar->Indicator->SetIcon(wxIcon(accept_xpm));
    }
}


bool ideFrame::BuildFile(std::string filename, bool strip, bool exportdefs, bool compiletoram)
{
    bool success = true; // Optimistic! :-)
    std::string command = progfolder + FOLDER_SEP + "spoon ";
    if (strip)
        command += "-s ";
    if (exportdefs)
        command += "-e ";
    if (compiletoram)
        command += "-r ";
    command += "\"" + filename + "\" \"";
    command += filename + ".bin\"";
    wxArrayString output;
    LogMessage("Executing: " + command);
    wxExecute(command, output);
    for (size_t i = 0; i < output.GetCount(); i++)
    {
        if (output[i].ToStdString().substr(0, 5) == "Error") // assuming each array item is exactly one line!
            success = false;
        LogMessage(output[i].ToStdString());
    }
    return success;
}

void ideFrame::OnRunClicked(wxCommandEvent& event)
{
    OnSaveClicked(event);
    FILE *pProcess;
    if ((pProcess = popen((progfolder + "/spoon \"" + filepath + "\" \"" + filepath + ".bin\"").c_str(), "r")))
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
            //StatusBar->Indicator->SetIcon(wxIcon(accept_xpm));
        }
        else
        {
            StatusBar->SetStatusText(message);
            wxBell();
            //StatusBar->Indicator->SetIcon(wxIcon(error_xpm));
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
    Text->SetKeyWords(1, wxT("int int16 pointer void char"));

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
    if (pagelist.size() <= 1)
        return;
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

void MyStatusBar::OnSize(wxSizeEvent &event)
{
    wxRect rect;
    GetFieldRect(1, rect);
    wxSize size = Indicator->GetSize();
    Indicator->Move(rect.GetX() + (rect.GetWidth() - size.GetX()) / 2, rect.GetY() + (rect.GetHeight() - size.GetY()) / 2);
}

MyStatusBar::MyStatusBar(wxWindow *parent, long id, long style, wxString name): wxStatusBar(parent, id, style, name)
{
    Indicator = new wxStaticBitmap(this, wxID_ANY, wxIcon(/*accept_xpm*/));
    int __wxStatusBarWidths_1[2] = { -1, 16 };
    int __wxStatusBarStyles_1[2] = { wxSB_NORMAL, wxSB_NORMAL };
    SetFieldsCount(2,__wxStatusBarWidths_1);
    SetStatusStyles(2,__wxStatusBarStyles_1);
}

MyStatusBar::~MyStatusBar()
{
}

void ideFrame::OnToolBarMessageLogClicked(wxCommandEvent& event)
{
    Log->Iconize(false);
    Log->SetFocus();
    Log->Raise();
    Log->Show(true);
}

void ideFrame::OnToolBarOptionsClicked(wxCommandEvent& event)
{
    Options->Iconize(false);
    Options->SetFocus();
    Options->Raise();
    Options->Show(true);
}

void ideFrame::OnBuildClicked(wxCommandEvent& event)
{
    int filecount = 0;
    for (int i = 0; i < Options->BuildOrder->GetListCtrl()->GetItemCount(); i++)
    {
        std::string name = Options->BuildOrder->GetListCtrl()->GetItemText(i).ToStdString();
        OptionsDialog::BuildInfoStruct &info = Options->BuildInfo[name];
        for (size_t i = 0; i < pagelist.size(); i++)
        {
            if (pagelist[i].filename == name)
            {
                if (!BuildFile(pagelist[i].filepath, info.strip, info.exportdefs, info.compiletoram))
                {
                    LogMessage("Build failed.", true);
                    return;
                }
                filecount++;
                break;
            }
        }
    }
    std::stringstream ss;
    ss << "Build Complete. " << filecount << " file(s) successfuly built.";
    LogMessage(ss.str());
}
