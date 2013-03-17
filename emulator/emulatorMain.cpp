/***************************************************************
 * Name:      emulatorMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-21
 * Copyright: Luke Wren (http://githhub.com/Wren6991)
 * License:
 **************************************************************/

#include "emulatorMain.h"
#include <fstream>
#include <string>
#include <wx/msgdlg.h>

//(*InternalHeaders(emulatorFrame)
#include <wx/bitmap.h>
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

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

//(*IdInit(emulatorFrame)
const long emulatorFrame::ID_LISTCTRL1 = wxNewId();
const long emulatorFrame::ID_STATICTEXT1 = wxNewId();
const long emulatorFrame::ID_LED7 = wxNewId();
const long emulatorFrame::ID_LED6 = wxNewId();
const long emulatorFrame::ID_LED5 = wxNewId();
const long emulatorFrame::ID_LED4 = wxNewId();
const long emulatorFrame::ID_LED3 = wxNewId();
const long emulatorFrame::ID_LED2 = wxNewId();
const long emulatorFrame::ID_LED1 = wxNewId();
const long emulatorFrame::ID_LED0 = wxNewId();
const long emulatorFrame::ID_STATICTEXT2 = wxNewId();
const long emulatorFrame::ID_SLIDER8 = wxNewId();
const long emulatorFrame::ID_SLIDER7 = wxNewId();
const long emulatorFrame::ID_SLIDER6 = wxNewId();
const long emulatorFrame::ID_SLIDER5 = wxNewId();
const long emulatorFrame::ID_SLIDER4 = wxNewId();
const long emulatorFrame::ID_SLIDER3 = wxNewId();
const long emulatorFrame::ID_SLIDER1 = wxNewId();
const long emulatorFrame::ID_SLIDER2 = wxNewId();
const long emulatorFrame::ID_STATUSBAR1 = wxNewId();
const long emulatorFrame::TOOL_NEW = wxNewId();
const long emulatorFrame::TOOL_OPEN = wxNewId();
const long emulatorFrame::TOOL_SAVE = wxNewId();
const long emulatorFrame::TOOL_ABOUT = wxNewId();
const long emulatorFrame::ID_TOOLBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(emulatorFrame,wxFrame)
    //(*EventTable(emulatorFrame)
    //*)
END_EVENT_TABLE()

emulatorFrame::emulatorFrame(wxWindow* parent,wxWindowID id)
{
    while (buffer.size() < 0x10000)
        buffer.push_back(0);


    //(*Initialize(emulatorFrame)
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer5;
    wxBoxSizer* frontpanelsizer;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer3;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(800,600));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    frontpanelsizer = new wxBoxSizer(wxVERTICAL);
    lstWatches = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL1"));
    lstWatches->SetMaxSize(wxSize(-1,-1));
    frontpanelsizer->Add(lstWatches, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer5 = new wxBoxSizer(wxVERTICAL);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Debug out:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    BoxSizer5->Add(StaticText1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Led7 = new wxLed(this,ID_LED7,wxColour(0,0,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led7->Disable();
    BoxSizer2->Add(Led7, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led6 = new wxLed(this,ID_LED6,wxColour(0,0,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led6->Disable();
    BoxSizer2->Add(Led6, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led5 = new wxLed(this,ID_LED5,wxColour(0,0,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led5->Disable();
    BoxSizer2->Add(Led5, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led4 = new wxLed(this,ID_LED4,wxColour(0,0,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led4->Disable();
    BoxSizer2->Add(Led4, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led3 = new wxLed(this,ID_LED3,wxColour(0,0,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led3->Disable();
    BoxSizer2->Add(Led3, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led2 = new wxLed(this,ID_LED2,wxColour(0,0,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led2->Disable();
    BoxSizer2->Add(Led2, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led1 = new wxLed(this,ID_LED1,wxColour(0,0,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led1->Disable();
    BoxSizer2->Add(Led1, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led0 = new wxLed(this,ID_LED0,wxColour(0,0,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led0->Disable();
    BoxSizer2->Add(Led0, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer5->Add(BoxSizer2, 1, wxLEFT|wxRIGHT|wxEXPAND|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4->Add(BoxSizer5, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer6 = new wxBoxSizer(wxVERTICAL);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Debug in:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    BoxSizer6->Add(StaticText2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    Slider8 = new wxSlider(this, ID_SLIDER8, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER8"));
    Slider8->SetTickFreq(1);
    Slider8->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider8, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider7 = new wxSlider(this, ID_SLIDER7, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER7"));
    Slider7->SetTickFreq(1);
    Slider7->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider7, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider6 = new wxSlider(this, ID_SLIDER6, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER6"));
    Slider6->SetTickFreq(1);
    Slider6->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider6, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider5 = new wxSlider(this, ID_SLIDER5, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER5"));
    Slider5->SetTickFreq(1);
    Slider5->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider5, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider4 = new wxSlider(this, ID_SLIDER4, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER4"));
    Slider4->SetTickFreq(1);
    Slider4->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider4, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider3 = new wxSlider(this, ID_SLIDER3, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER3"));
    Slider3->SetTickFreq(1);
    Slider3->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider3, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider1 = new wxSlider(this, ID_SLIDER1, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER1"));
    Slider1->SetTickFreq(1);
    Slider1->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider1, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider2 = new wxSlider(this, ID_SLIDER2, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER2"));
    Slider2->SetTickFreq(1);
    Slider2->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider2, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer6->Add(BoxSizer3, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4->Add(BoxSizer6, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    frontpanelsizer->Add(BoxSizer4, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(frontpanelsizer, 1, wxEXPAND|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    ToolBar1 = new wxToolBar(this, ID_TOOLBAR1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_NODIVIDER|wxNO_BORDER, _T("ID_TOOLBAR1"));
    ToolBarItem1 = ToolBar1->AddTool(TOOL_NEW, _("New"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\page_add.png"))), wxNullBitmap, wxITEM_NORMAL, _("New file"), wxEmptyString);
    ToolBarItem2 = ToolBar1->AddTool(TOOL_OPEN, _("Open"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\folder_page.png"))), wxNullBitmap, wxITEM_NORMAL, _("Open file"), wxEmptyString);
    ToolBarItem3 = ToolBar1->AddTool(TOOL_SAVE, _("Save"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\disk.png"))), wxNullBitmap, wxITEM_NORMAL, _("Right click for save as"), wxEmptyString);
    ToolBar1->AddSeparator();
    ToolBarItem4 = ToolBar1->AddTool(TOOL_ABOUT, _("About"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\norforkconditionally\\ide\\icons\\help.png"))), wxNullBitmap, wxITEM_NORMAL, _("About this software"), wxEmptyString);
    ToolBar1->Realize();
    SetToolBar(ToolBar1);
    dlgOpen = new wxFileDialog(this, _("Open"), wxEmptyString, wxEmptyString, _("Binary files (*.bin)|*.bin|All files (*.*)|*.*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    dlgSaveAs = new wxFileDialog(this, _("-1"), wxEmptyString, wxEmptyString, _("Binary files (*.bin)|*.bin|All files (*.*)|*.*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    SetSizer(BoxSizer1);
    Layout();

    Connect(TOOL_NEW,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnNewClicked);
    Connect(TOOL_OPEN,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnOpenClicked);
    Connect(TOOL_SAVE,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnSaveClicked);
    Connect(TOOL_SAVE,wxEVT_COMMAND_TOOL_RCLICKED,(wxObjectEventFunction)&emulatorFrame::OnSaveAsClicked);
    Connect(TOOL_ABOUT,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnAbout);
    //*)

    lstWatches->InsertColumn(0, "Name");
    lstWatches->InsertColumn(1, "Address");
    lstWatches->InsertColumn(2, "Value");

    memview = new HexView(this, &buffer);
    BoxSizer1->Prepend(memview, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Fit(this);

    this->SetTitle("Untitled - OISC Emulator");

    filename = "Untitled";
    filehaschanged = false;
}

emulatorFrame::~emulatorFrame()
{
    //(*Destroy(emulatorFrame)
    //*)
}

void emulatorFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void emulatorFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void emulatorFrame::OnOpenClicked(wxCommandEvent& event)
{
    if (dlgOpen->ShowModal() == wxID_OK)
    {
        SetFileName(dlgOpen->GetPath().ToStdString(), false);
        std::fstream file(filepath.c_str(), std::ios::in | std::ios::binary);
        file.seekg(0, std::ios::end);
        int length = file.tellg();
        file.seekg(0, std::ios::beg);
        uint8_t *inputbuf = new uint8_t[length];
        file.read((char*)inputbuf, length);
        file.close();

        buffer.clear();
        for (int i = 0; i < length; i++)
            buffer.push_back(inputbuf[i]);
        delete[] inputbuf;
        while (buffer.size() < 0x10000)
            buffer.push_back(0);
        memview->Refresh();
    }
}

void emulatorFrame::SetFileName(std::string path, bool hasbeenmodified)
{
    filepath = path;
    int index = path.rfind("/");
    if (index < 0)
        index = path.rfind("\\");
    filename = path.substr(index + 1, path.size() - index - 1);
    this->SetTitle((hasbeenmodified? "*" : "") + filename + " - OISC Emulator");
    filehaschanged = hasbeenmodified;
}

void emulatorFrame::OnSaveAsClicked(wxCommandEvent& event)
{
    if (dlgSaveAs->ShowModal() == wxID_OK)
    {
        /*Text->SaveFile(dlgSaveAs->GetPath());
        SetFileName(dlgSaveAs->GetPath().ToStdString());*/
    }
}

void emulatorFrame::OnSaveClicked(wxCommandEvent& event)
{
    /*if (filename == "Untitled")
    {
        wxCommandEvent evt;
        OnSaveAsClicked(evt);
    }
    else if (filehaschanged)
    {
        SetFileName(filepath);
        Text->SaveFile(filepath);
    }*/
}

void emulatorFrame::OnNewClicked(wxCommandEvent& event)
{
    //AddPage();
}
