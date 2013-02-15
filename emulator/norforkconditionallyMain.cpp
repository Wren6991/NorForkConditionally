/***************************************************************
 * Name:      norforkconditionallyMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2012-10-16
 * Copyright: Luke Wren ()
 * License:
 **************************************************************/

#include "norforkconditionallyMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(norforkconditionallyFrame)
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <sstream>
#include <iomanip>

#include "vm.h"

vm machine;

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

//(*IdInit(norforkconditionallyFrame)
const long norforkconditionallyFrame::ID_TEXTCTRL1 = wxNewId();
const long norforkconditionallyFrame::ID_BUTTON2 = wxNewId();
const long norforkconditionallyFrame::ID_LISTCTRL1 = wxNewId();
const long norforkconditionallyFrame::ID_TEXTCTRL3 = wxNewId();
const long norforkconditionallyFrame::ID_BUTTON4 = wxNewId();
const long norforkconditionallyFrame::ID_STATICTEXT1 = wxNewId();
const long norforkconditionallyFrame::ID_SLIDER9 = wxNewId();
const long norforkconditionallyFrame::ID_SLIDER8 = wxNewId();
const long norforkconditionallyFrame::ID_SLIDER7 = wxNewId();
const long norforkconditionallyFrame::ID_SLIDER6 = wxNewId();
const long norforkconditionallyFrame::ID_SLIDER5 = wxNewId();
const long norforkconditionallyFrame::ID_SLIDER4 = wxNewId();
const long norforkconditionallyFrame::ID_SLIDER3 = wxNewId();
const long norforkconditionallyFrame::ID_SLIDER2 = wxNewId();
const long norforkconditionallyFrame::ID_BUTTON1 = wxNewId();
const long norforkconditionallyFrame::ID_BUTTON3 = wxNewId();
const long norforkconditionallyFrame::ID_MENUITEM0 = wxNewId();
const long norforkconditionallyFrame::ID_MENUITEM1 = wxNewId();
const long norforkconditionallyFrame::ID_MENUITEM2 = wxNewId();
const long norforkconditionallyFrame::idMenuQuit = wxNewId();
const long norforkconditionallyFrame::ID_MEMWINDOW = wxNewId();
const long norforkconditionallyFrame::idMenuAbout = wxNewId();
const long norforkconditionallyFrame::ID_TIMER1 = wxNewId();
const long norforkconditionallyFrame::ID_TIMER2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(norforkconditionallyFrame,wxFrame)
    //(*EventTable(norforkconditionallyFrame)
    //*)
END_EVENT_TABLE()

norforkconditionallyFrame::norforkconditionallyFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(norforkconditionallyFrame)
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer5;
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxBoxSizer* BoxSizer2;
    wxMenu* Menu1;
    wxMenuItem* mnuNew;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxBoxSizer* BoxSizer3;
    wxMenu* Menu2;

    Create(parent, id, _("Nor and Fork Conditionally"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer4 = new wxBoxSizer(wxVERTICAL);
    txtProgram = new wxTextCtrl(this, ID_TEXTCTRL1, _("bfff 0070 0008 0008\nbfff c001 0010 0010\nbfff 0071 0018 0018\nbffe 0070 0020 0020\nbffe bfff 0028 0028\nbffe bffe 0030 0050\nbfff 0070 0038 0038\nbfff 0072 0040 0040\nc000 bfff 0048 0048\nbfff bfff 0000 0000\nbfff 0070 0058 0058\nbfff 0073 0060 0060\nc000 bfff 0068 0068\nbfff bfff 0000 0000\nff0f 0001 0000 0000"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    txtProgram->SetMinSize(wxSize(200,250));
    wxFont txtProgramFont(10,wxSWISS,wxFONTSTYLE_NORMAL,wxNORMAL,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
    txtProgram->SetFont(txtProgramFont);
    BoxSizer4->Add(txtProgram, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    btnLoad = new wxButton(this, ID_BUTTON2, _("Load"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer4->Add(btnLoad, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    lstWatches = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_EDIT_LABELS|wxLC_HRULES|wxLC_VRULES|wxLC_NO_SORT_HEADER|wxNO_FULL_REPAINT_ON_RESIZE, wxDefaultValidator, _T("ID_LISTCTRL1"));
    BoxSizer2->Add(lstWatches, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    txtWatchAddress = new wxTextCtrl(this, ID_TEXTCTRL3, _("0000"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    BoxSizer6->Add(txtWatchAddress, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    cmdWatchAddress = new wxButton(this, ID_BUTTON4, _("Watch Address"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    BoxSizer6->Add(cmdWatchAddress, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer6, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Debug Input:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    BoxSizer5->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    dbg7 = new wxSlider(this, ID_SLIDER9, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER9"));
    dbg7->SetTickFreq(1);
    dbg7->SetMinSize(wxSize(-1,40));
    BoxSizer5->Add(dbg7, 0, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    dbg6 = new wxSlider(this, ID_SLIDER8, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER8"));
    dbg6->SetTickFreq(1);
    dbg6->SetMinSize(wxSize(-1,40));
    BoxSizer5->Add(dbg6, 0, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    dbg5 = new wxSlider(this, ID_SLIDER7, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER7"));
    dbg5->SetTickFreq(1);
    dbg5->SetMinSize(wxSize(-1,40));
    BoxSizer5->Add(dbg5, 0, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    dbg4 = new wxSlider(this, ID_SLIDER6, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER6"));
    dbg4->SetTickFreq(1);
    dbg4->SetMinSize(wxSize(-1,40));
    BoxSizer5->Add(dbg4, 0, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    dbg3 = new wxSlider(this, ID_SLIDER5, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER5"));
    dbg3->SetTickFreq(1);
    dbg3->SetMinSize(wxSize(-1,40));
    BoxSizer5->Add(dbg3, 0, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    dbg2 = new wxSlider(this, ID_SLIDER4, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER4"));
    dbg2->SetTickFreq(1);
    dbg2->SetMinSize(wxSize(-1,40));
    BoxSizer5->Add(dbg2, 0, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    dbg1 = new wxSlider(this, ID_SLIDER3, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER3"));
    dbg1->SetTickFreq(1);
    dbg1->SetMinSize(wxSize(-1,40));
    BoxSizer5->Add(dbg1, 0, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    dbg0 = new wxSlider(this, ID_SLIDER2, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER2"));
    dbg0->SetTickFreq(1);
    dbg0->SetMinSize(wxSize(-1,40));
    BoxSizer5->Add(dbg0, 0, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer5, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    btnRunStop = new wxButton(this, ID_BUTTON1, _("Run"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer3->Add(btnRunStop, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    btnStep = new wxButton(this, ID_BUTTON3, _("Step"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    BoxSizer3->Add(btnStep, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer1->Add(BoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SetSizer(BoxSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    mnuNew = new wxMenuItem(Menu1, ID_MENUITEM0, _("&New\tCtrl-N"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(mnuNew);
    mnuOpenTextFile = new wxMenuItem(Menu1, ID_MENUITEM1, _("Open Text File\tCtrl-N"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(mnuOpenTextFile);
    MenuItem4 = new wxMenuItem(Menu1, ID_MENUITEM2, _("Open Binary File"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu3, ID_MEMWINDOW, _("View &Memory\tCtrl-M"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem3);
    MenuBar1->Append(Menu3, _("Tools"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    tmrStep.SetOwner(this, ID_TIMER1);
    tmrStep.Start(1, false);
    tmrStep.Stop();
    tmrUpdate.SetOwner(this, ID_TIMER2);
    tmrUpdate.Start(53, false);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&norforkconditionallyFrame::OnbtnLoadClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&norforkconditionallyFrame::OncmdWatchAddressClick);
    Connect(ID_SLIDER9,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&norforkconditionallyFrame::OndbgCmdScrollThumbTrack);
    Connect(ID_SLIDER8,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&norforkconditionallyFrame::OndbgCmdScrollThumbTrack);
    Connect(ID_SLIDER7,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&norforkconditionallyFrame::OndbgCmdScrollThumbTrack);
    Connect(ID_SLIDER6,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&norforkconditionallyFrame::OndbgCmdScrollThumbTrack);
    Connect(ID_SLIDER5,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&norforkconditionallyFrame::OndbgCmdScrollThumbTrack);
    Connect(ID_SLIDER4,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&norforkconditionallyFrame::OndbgCmdScrollThumbTrack);
    Connect(ID_SLIDER3,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&norforkconditionallyFrame::OndbgCmdScrollThumbTrack);
    Connect(ID_SLIDER2,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&norforkconditionallyFrame::OndbgCmdScrollThumbTrack);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&norforkconditionallyFrame::OnbtnRunStopClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&norforkconditionallyFrame::OnbtnStepClick);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&norforkconditionallyFrame::OnmnuOpenTextFileSelected);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&norforkconditionallyFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&norforkconditionallyFrame::OnAbout);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&norforkconditionallyFrame::OntmrStepTrigger);
    Connect(ID_TIMER2,wxEVT_TIMER,(wxObjectEventFunction)&norforkconditionallyFrame::OntmrUpdateTrigger);
    //*)
    tmrStep.Stop();
    tmrUpdate.Stop();

    lstWatches->InsertColumn(0, "Name");
    lstWatches->InsertColumn(1, "Address");
    lstWatches->InsertColumn(2, "Value");

    long itemIndex = lstWatches->InsertItem(0, "");
    lstWatches->SetItem(itemIndex, 0, "PC");
    lstWatches->SetItem(itemIndex, 1, "-");

    itemIndex = lstWatches->InsertItem(itemIndex + 1, "");
    lstWatches->SetItem(itemIndex, 0, "debug in");
    lstWatches->SetItem(itemIndex, 1, "c001");

    itemIndex = lstWatches->InsertItem(itemIndex + 1, "");
    lstWatches->SetItem(itemIndex, 0, "debug out");
    lstWatches->SetItem(itemIndex, 1, "c000");
}

norforkconditionallyFrame::~norforkconditionallyFrame()
{
    //(*Destroy(norforkconditionallyFrame)
    //*)
}

void norforkconditionallyFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void norforkconditionallyFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void norforkconditionallyFrame::step()
{
    machine.step();
    if (machine.debugWritten)
    {
        machine.debugWritten = false;
    }
}

void norforkconditionallyFrame::OnbtnStepClick(wxCommandEvent& event)
{
    step();
    wxTimerEvent evt;
    OntmrUpdateTrigger(evt);
}

void norforkconditionallyFrame::OnbtnRunStopClick(wxCommandEvent& event)
{
    if (tmrStep.IsRunning())
    {
        btnRunStop->SetLabel("Run");
        tmrStep.Stop();
        tmrUpdate.Stop();
    }
    else
    {
        btnRunStop->SetLabel("Stop");
        tmrStep.Start(1, false);
        tmrUpdate.Start(1, false);
    }
}

void norforkconditionallyFrame::OnbtnLoadClick(wxCommandEvent& event)
{
    machine = vm();
    int memoffset = 0;
    const char *str = txtProgram->GetValue().ToStdString().c_str();
    char c, nibble;
    while ((c = *str++))
    {
        std::cout << c;
        if ((c < '0' || c > '9') && (c < 'a' || c > 'f') && (c < 'A' || c > 'F'))
            continue;
        if (c >= '0' && c <= '9')
            nibble = c - '0';
        else if (c >= 'a' && c <= 'f')
            nibble = c - 'a' + 0xa;
        else
            nibble = c - 'A' + 0xA;
        machine.memory[memoffset >> 1] |= nibble << ((1 - (memoffset & 0x1)) << 2);     // write to the high or low nibble of the byte at half the nibble count.
        memoffset++;
    }
    /*for (int i = 0; i < 0x40; i++)
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)machine.memory[i];*/
    wxTimerEvent evt;
    OntmrUpdateTrigger(evt);        // update the watch list to show new values of program counter etc.
}


void norforkconditionallyFrame::OndbgCmdScrollThumbTrack(wxScrollEvent& event)
{
    int value = dbg7->GetValue() << 7 | dbg6->GetValue() << 6 | dbg5->GetValue() << 5 | dbg4->GetValue() << 4 | dbg3->GetValue() << 3 | dbg2->GetValue() << 2 | dbg1->GetValue() << 1 | dbg0->GetValue() << 0;
    std::cout << value;
    machine.memory[DEBUG_IN_POS] = value;
}

void norforkconditionallyFrame::OntmrStepTrigger(wxTimerEvent& event)
{
    for (int i = 0; i < 1; i++)
        step();
}


void norforkconditionallyFrame::OntmrUpdateTrigger(wxTimerEvent& event)
{
    int count = lstWatches->GetItemCount();
    wxListItem item;
    std::stringstream outstr;
    // Display PC in the first row before we do the others:
    outstr << std::hex << std::setfill('0') << std::setw(4) << machine.PC;
    lstWatches->SetItem(0, 2, outstr.str());
    // Loop through each list item, read and fetch the address, put the result in the last column.
    for (int i = 1; i < count; i++)
    {
        item.m_itemId = i;
        item.m_col = 1;
        item.m_mask = wxLIST_MASK_TEXT;
        lstWatches->GetItem(item);
        std::stringstream addressstream(item.m_text.ToStdString());
        addressstream.seekg(0, std::ios::beg);
        addressstream << std::hex;
        uint16_t address;
        addressstream >> address;
        std::stringstream outstr;
        outstr << std::hex << std::setfill('0') << std::setw(2) << (int)machine.memory[address];
        std::cout << outstr.str() << "\n";
        lstWatches->SetItem(i, 2, outstr.str());

    }
}

void norforkconditionallyFrame::OncmdWatchAddressClick(wxCommandEvent& event)
{
    long itemIndex = lstWatches->InsertItem(lstWatches->GetItemCount(), "");
    lstWatches->SetItem(itemIndex, 1, txtWatchAddress->GetValue());
}

void norforkconditionallyFrame::OnClose(wxCloseEvent& event)
{
}

void norforkconditionallyFrame::OnmnuOpenTextFileSelected(wxCommandEvent& event)
{
    txtProgram->SetValue("");
}
