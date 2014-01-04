/***************************************************************
 * Name:      emulatorMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-21
 * Copyright: Luke Wren (http://githhub.com/Wren6991)
 * License:
 **************************************************************/

/*
 * Todo:
 * - Show PC cursor on hexview
 * - variable speed
 * - location variables for watches
 * - code mapping? (Link source lines->machine addresses, integrate with IDE)
 */
#include "emulatorMain.h"
#include <fstream>
#include <iomanip>
#include <sstream>
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
enum wxbuildinfoformat
{
    short_f, long_f
};

//(*IdInit(emulatorFrame)
const long emulatorFrame::ID_LSTWATCHES = wxNewId();
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
const long emulatorFrame::ID_SLIDER7 = wxNewId();
const long emulatorFrame::ID_SLIDER6 = wxNewId();
const long emulatorFrame::ID_SLIDER5 = wxNewId();
const long emulatorFrame::ID_SLIDER4 = wxNewId();
const long emulatorFrame::ID_SLIDER3 = wxNewId();
const long emulatorFrame::ID_SLIDER2 = wxNewId();
const long emulatorFrame::ID_SLIDER1 = wxNewId();
const long emulatorFrame::ID_SLIDER0 = wxNewId();
const long emulatorFrame::ID_BUTTON1 = wxNewId();
const long emulatorFrame::ID_BUTTON2 = wxNewId();
const long emulatorFrame::ID_STATUSBAR1 = wxNewId();
const long emulatorFrame::TOOL_NEW = wxNewId();
const long emulatorFrame::TOOL_OPEN = wxNewId();
const long emulatorFrame::TOOL_SAVE = wxNewId();
const long emulatorFrame::ID_TOOLBARITEM1 = wxNewId();
const long emulatorFrame::ID_TOOLBARITEM2 = wxNewId();
const long emulatorFrame::TOOL_LCD = wxNewId();
const long emulatorFrame::TOOL_FLASH = wxNewId();
const long emulatorFrame::TOOL_ABOUT = wxNewId();
const long emulatorFrame::ID_TOOLBAR1 = wxNewId();
const long emulatorFrame::ID_TIMER1 = wxNewId();
const long emulatorFrame::ID_MENUADDWATCH = wxNewId();
const long emulatorFrame::ID_MENUCHANGEADDRESS = wxNewId();
const long emulatorFrame::ID_MENUDELETEWATCH = wxNewId();
//*)

BEGIN_EVENT_TABLE(emulatorFrame,wxFrame)
    //(*EventTable(emulatorFrame)
    //*)
END_EVENT_TABLE()

emulatorFrame::emulatorFrame(wxWindow* parent,wxWindowID id)
{
    while (buffer.size() < (unsigned)VM_MEM_SIZE)
        buffer.push_back(0);


    //(*Initialize(emulatorFrame)
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer5;
    wxBoxSizer* BoxSizer7;
    wxBoxSizer* frontpanelsizer;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer3;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(800,600));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    frontpanelsizer = new wxBoxSizer(wxVERTICAL);
    lstWatches = new wxListCtrl(this, ID_LSTWATCHES, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_EDIT_LABELS|wxLC_SINGLE_SEL|wxLC_SORT_DESCENDING, wxDefaultValidator, _T("ID_LSTWATCHES"));
    lstWatches->SetMaxSize(wxSize(-1,-1));
    frontpanelsizer->Add(lstWatches, 8, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer5 = new wxBoxSizer(wxVERTICAL);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Debug out:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    BoxSizer5->Add(StaticText1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Led7 = new wxLed(this,ID_LED7,wxColour(0,64,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led7->Disable();
    BoxSizer2->Add(Led7, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led6 = new wxLed(this,ID_LED6,wxColour(0,64,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led6->Disable();
    BoxSizer2->Add(Led6, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led5 = new wxLed(this,ID_LED5,wxColour(0,64,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led5->Disable();
    BoxSizer2->Add(Led5, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led4 = new wxLed(this,ID_LED4,wxColour(0,64,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led4->Disable();
    BoxSizer2->Add(Led4, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led3 = new wxLed(this,ID_LED3,wxColour(0,64,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led3->Disable();
    BoxSizer2->Add(Led3, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led2 = new wxLed(this,ID_LED2,wxColour(0,64,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led2->Disable();
    BoxSizer2->Add(Led2, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led1 = new wxLed(this,ID_LED1,wxColour(0,64,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led1->Disable();
    BoxSizer2->Add(Led1, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Led0 = new wxLed(this,ID_LED0,wxColour(0,64,0),wxColour(0,255,0),wxDefaultPosition,wxDefaultSize);
    Led0->Disable();
    BoxSizer2->Add(Led0, 0, wxALL|wxSHAPED|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer5->Add(BoxSizer2, 1, wxLEFT|wxRIGHT|wxEXPAND|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4->Add(BoxSizer5, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer6 = new wxBoxSizer(wxVERTICAL);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Debug in:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    BoxSizer6->Add(StaticText2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    Slider7 = new wxSlider(this, ID_SLIDER7, 1, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER7"));
    Slider7->SetTickFreq(1);
    Slider7->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider7, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider6 = new wxSlider(this, ID_SLIDER6, 1, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER6"));
    Slider6->SetTickFreq(1);
    Slider6->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider6, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider5 = new wxSlider(this, ID_SLIDER5, 1, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER5"));
    Slider5->SetTickFreq(1);
    Slider5->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider5, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider4 = new wxSlider(this, ID_SLIDER4, 1, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER4"));
    Slider4->SetTickFreq(1);
    Slider4->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider4, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider3 = new wxSlider(this, ID_SLIDER3, 1, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER3"));
    Slider3->SetTickFreq(1);
    Slider3->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider3, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider2 = new wxSlider(this, ID_SLIDER2, 1, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER2"));
    Slider2->SetTickFreq(1);
    Slider2->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider2, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider1 = new wxSlider(this, ID_SLIDER1, 1, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER1"));
    Slider1->SetTickFreq(1);
    Slider1->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider1, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider0 = new wxSlider(this, ID_SLIDER0, 1, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER0"));
    Slider0->SetTickFreq(1);
    Slider0->SetMinSize(wxSize(20,40));
    BoxSizer3->Add(Slider0, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer6->Add(BoxSizer3, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4->Add(BoxSizer6, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    frontpanelsizer->Add(BoxSizer4, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    btnStartStop = new wxButton(this, ID_BUTTON1, _("Start"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer7->Add(btnStartStop, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    btnReset = new wxButton(this, ID_BUTTON2, _("Reset"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer7->Add(btnReset, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    frontpanelsizer->Add(BoxSizer7, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(frontpanelsizer, 1, wxEXPAND|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    ToolBar1 = new wxToolBar(this, ID_TOOLBAR1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_NODIVIDER|wxNO_BORDER, _T("ID_TOOLBAR1"));
    ToolBarItem1 = ToolBar1->AddTool(TOOL_NEW, _("New"), wxBitmap(wxImage(_T("icons/page_add.png"))), wxNullBitmap, wxITEM_NORMAL, _("New file"), wxEmptyString);
    ToolBarItem2 = ToolBar1->AddTool(TOOL_OPEN, _("Open"), wxBitmap(wxImage(_T("icons\\folder_page.png"))), wxNullBitmap, wxITEM_NORMAL, _("Open file"), wxEmptyString);
    ToolBarItem3 = ToolBar1->AddTool(TOOL_SAVE, _("Save"), wxBitmap(wxImage(_T("icons\\disk.png"))), wxNullBitmap, wxITEM_NORMAL, _("Right click for save as"), wxEmptyString);
    ToolBar1->AddSeparator();
    ToolBarItem4 = ToolBar1->AddTool(ID_TOOLBARITEM1, _("Step"), wxBitmap(wxImage(_T("icons\\resultset_next.png"))), wxNullBitmap, wxITEM_NORMAL, _("Step simulation"), wxEmptyString);
    ToolBarItem5 = ToolBar1->AddTool(ID_TOOLBARITEM2, _("Breakpoint"), wxBitmap(wxImage(_T("C:\\Users\\Owner\\Documents\\CodeBlocks\\NorForkConditionally\\emulator\\icons\\stop.png"))), wxNullBitmap, wxITEM_NORMAL, _("Set/remove breakpoint"), wxEmptyString);
    ToolBarItem6 = ToolBar1->AddTool(TOOL_LCD, _("LCD"), wxBitmap(wxImage(_T("icons/monitor.png"))), wxNullBitmap, wxITEM_NORMAL, _("Show LCD"), wxEmptyString);
    ToolBarItem7 = ToolBar1->AddTool(TOOL_FLASH, _("Flash"), wxBitmap(wxImage(_T("icons/drive_magnify.png"))), wxNullBitmap, wxITEM_NORMAL, _("Show flash contents"), wxEmptyString);
    ToolBar1->AddSeparator();
    ToolBarItem8 = ToolBar1->AddTool(TOOL_ABOUT, _("About"), wxBitmap(wxImage(_T("icons\\help.png"))), wxNullBitmap, wxITEM_NORMAL, _("About this software"), wxEmptyString);
    ToolBar1->Realize();
    SetToolBar(ToolBar1);
    dlgOpen = new wxFileDialog(this, _("Open"), wxEmptyString, wxEmptyString, _("Binary files (*.bin)|*.bin|All files (*.*)|*.*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    dlgSaveAs = new wxFileDialog(this, _("-1"), wxEmptyString, wxEmptyString, _("Binary files (*.bin)|*.bin|All files (*.*)|*.*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    TimerTick.SetOwner(this, ID_TIMER1);
    TimerTick.Start(50, false);
    MenuItem1 = new wxMenuItem((&mnuWatch), ID_MENUADDWATCH, _("Add Watch"), wxEmptyString, wxITEM_NORMAL);
    mnuWatch.Append(MenuItem1);
    MenuItem2 = new wxMenuItem((&mnuWatch), ID_MENUCHANGEADDRESS, _("Change Address"), wxEmptyString, wxITEM_NORMAL);
    mnuWatch.Append(MenuItem2);
    MenuItem3 = new wxMenuItem((&mnuWatch), ID_MENUDELETEWATCH, _("Delete Watch"), wxEmptyString, wxITEM_NORMAL);
    mnuWatch.Append(MenuItem3);
    SetSizer(BoxSizer1);
    Layout();

    Connect(ID_LSTWATCHES,wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&emulatorFrame::OnlstWatchesItemRClick);
    Connect(ID_LSTWATCHES,wxEVT_COMMAND_LIST_COL_RIGHT_CLICK,(wxObjectEventFunction)&emulatorFrame::OnlstWatchesItemRClick);
    Connect(ID_SLIDER7,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER7,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER6,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER6,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER5,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER5,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER4,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER4,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER3,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER3,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER2,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER2,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER1,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER1,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER0,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_SLIDER0,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&emulatorFrame::OnDebuginChange);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnbtnStartStopClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnbtnResetClick);
    Connect(TOOL_NEW,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnNewClicked);
    Connect(TOOL_OPEN,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnOpenClicked);
    Connect(TOOL_SAVE,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnSaveClicked);
    Connect(TOOL_SAVE,wxEVT_COMMAND_TOOL_RCLICKED,(wxObjectEventFunction)&emulatorFrame::OnSaveAsClicked);
    Connect(ID_TOOLBARITEM1,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnStepClicked);
    Connect(TOOL_LCD,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnLCDClicked);
    Connect(TOOL_FLASH,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnFlashClicked);
    Connect(TOOL_ABOUT,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&emulatorFrame::OnAbout);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&emulatorFrame::OnTimerTickTrigger);
    Connect(ID_MENUADDWATCH,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&emulatorFrame::OnMenuAddWatchSelected);
    Connect(ID_MENUDELETEWATCH,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&emulatorFrame::OnMenuItemDeleteSelected);
    //*)

    Connect(ID_LSTWATCHES, wxEVT_COMMAND_RIGHT_CLICK, (wxObjectEventFunction)&emulatorFrame::OnlstWatchesItemRClick);

    TimerTick.Stop();

    lstWatches->InsertColumn(0, "Name");
    lstWatches->InsertColumn(1, "Address");
    lstWatches->InsertColumn(2, "Value");

    memview = new HexView(this, &buffer);
    memview->cursors.push_back(HexView::cursor(wxColor(224, 255, 255), 0, 8));
    BoxSizer1->Prepend(memview, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Fit(this);

    machine.memory = &buffer;

    this->SetTitle("Untitled - OISC Emulator");

    lcd = new LCDDialog(this);
    machine.peripherals.push_back(lcd);
    flash = new FlashDialog(this);
    machine.peripherals.push_back(flash);

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
    wxMessageBox(_("NFC Emulator\n"
                   "(c) Luke Wren 2013\n"
                   "Thanks to:\n"
                   "Thomas Monjalon - LED Components"
                   ), _("About This Software"));
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
        while (buffer.size() < (unsigned)VM_MEM_SIZE)
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
        //Text->SaveFile(dlgSaveAs->GetPath());
        SetFileName(dlgSaveAs->GetPath().ToStdString());
    }
}

void emulatorFrame::OnSaveClicked(wxCommandEvent& event)
{
    if (filename == "Untitled")
    {
        wxCommandEvent evt;
        OnSaveAsClicked(evt);
    }
    else if (filehaschanged)
    {
        SetFileName(filepath);
        wxBell();
        //Text->SaveFile(filepath);
    }
}

void emulatorFrame::OnNewClicked(wxCommandEvent& event)
{
    //AddPage();
}

void emulatorFrame::OnDebuginChange(wxScrollEvent& event)
{
    uint8_t data = 0;
    if (Slider7->GetValue())
        data |= 0x80;
    if (Slider6->GetValue())
        data |= 0x40;
    if (Slider5->GetValue())
        data |= 0x20;
    if (Slider4->GetValue())
        data |= 0x10;
    if (Slider3->GetValue())
        data |= 0x08;
    if (Slider2->GetValue())
        data |= 0x04;
    if (Slider1->GetValue())
        data |= 0x02;
    if (Slider0->GetValue())
        data |= 0x01;
    data = ~data;
    buffer[DEBUG_IN_POS] = data;
    std::cout << (int)data << "\n";
}

void writevalue(wxLed* led, bool value)
{
    if (value)
        led->Enable();
    else
        led->Disable();
}

bool emulatorFrame::TakeStep()
{
    bool result = false;
    machine.step();
    if (machine.debug_written)
    {
        result = true;
        machine.debug_written = false;
        uint8_t data = buffer[DEBUG_OUT_POS];
        //std::cout << "debug output: " << std::hex << (int)data << "\n";
    }
    memview->cursors[0].index = machine.PC;
    return result;
}

void emulatorFrame::UpdateDisplay()
{
    uint8_t data = buffer[DEBUG_OUT_POS];
    writevalue(Led7, data & 0x80);
    writevalue(Led6, data & 0x40);
    writevalue(Led5, data & 0x20);
    writevalue(Led4, data & 0x10);
    writevalue(Led3, data & 0x08);
    writevalue(Led2, data & 0x04);
    writevalue(Led1, data & 0x02);
    writevalue(Led0, data & 0x01);
}

void emulatorFrame::OnStepClicked(wxCommandEvent& event)
{
    if (TakeStep())
        UpdateDisplay();
    memview->Refresh();
    wxListItem item;
    for (int i = 0; i < lstWatches->GetItemCount(); i++)
    {
        item.m_itemId = i;
        item.m_col = 1;
        item.m_mask = wxLIST_MASK_TEXT;
        lstWatches->GetItem(item);
        std::stringstream addressstream(item.m_text.ToStdString());
        addressstream.seekg(0, std::ios::beg);
        addressstream.ignore(2);
        addressstream << std::hex;
        int address;
        addressstream >> address;
        std::stringstream formatter;
        int data = 0;
        if (address >= 0 && address < (signed)machine.memory->size())
            data = (*machine.memory)[address];
        formatter << std::hex << std::setw(2) << std::setfill('0') << data;
        lstWatches->SetItem(i, 2, formatter.str());
    }
    std::stringstream ss;
    ss << "PC: " << std::hex << std::setw(4) << std::setfill('0') << machine.PC;
    StatusBar1->SetStatusText(ss.str());
}


void emulatorFrame::OnTimerTickTrigger(wxTimerEvent& event)
{
    bool debug_updated = false;
    for (int i = 0; i < 8192; i++)
        if (TakeStep())
            debug_updated = true;
    if (debug_updated)
        UpdateDisplay();
    wxCommandEvent evt;
    OnStepClicked(evt);
}

void emulatorFrame::OnbtnResetClick(wxCommandEvent& event)
{
    machine.PC = 0;
    if (TimerTick.IsRunning())
        OnbtnStartStopClick(event);
    memview->Refresh();
    std::stringstream ss;
    ss << "PC: " << std::hex << std::setw(4) << std::setfill('0') << machine.PC;
    StatusBar1->SetStatusText(ss.str());

}

void emulatorFrame::OnbtnStartStopClick(wxCommandEvent& event)
{
    if (TimerTick.IsRunning())
    {
        TimerTick.Stop();
        btnStartStop->SetLabel("Start");
    }
    else
    {
        TimerTick.Start();
        btnStartStop->SetLabel("Stop");
    }
}

void emulatorFrame::OnlstWatchesItemRClick(wxListEvent& event)
{
    lstWatches->PopupMenu(&mnuWatch);
}

int get_hex_address()
{
    std::string input = wxGetTextFromUser("Enter memory address:", "Add Watch").ToStdString();
    std::stringstream ss;
    ss << std::hex << input;
    int address = 0;
    ss >> address;
    return address;
}

void emulatorFrame::OnMenuAddWatchSelected(wxCommandEvent& event)
{
    long itemindex = lstWatches->InsertItem(lstWatches->GetItemCount(), "New Watch");
    std::stringstream formatter;
    formatter << "0x" << std::hex << std::setw(4) << std::setfill('0') << get_hex_address();
    lstWatches->SetItem(itemindex, 1, formatter.str());
}

void emulatorFrame::OnMenuItemDeleteSelected(wxCommandEvent& event)
{
    int selected = lstWatches->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selected == -1)
        return;
    lstWatches->DeleteItem(selected);
}

void emulatorFrame::OnLCDClicked(wxCommandEvent& event)
{
    lcd->Iconize(false);
    lcd->SetFocus();
    lcd->Raise();
    lcd->Show(true);
}

void emulatorFrame::OnFlashClicked(wxCommandEvent& event)
{
    flash->Iconize(false);
    flash->SetFocus();
    flash->Raise();
    flash->Show(true);
}
