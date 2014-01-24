#include "FlashDialog.h"

#include <fstream>
#include <iomanip>
#include <iostream>

//(*InternalHeaders(FlashDialog)
#include <wx/bitmap.h>
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//(*IdInit(FlashDialog)
const long FlashDialog::TOOL_FLASH_LOAD = wxNewId();
const long FlashDialog::TOOL_FLASH_SAVE = wxNewId();
const long FlashDialog::ID_TOOLBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(FlashDialog,wxFrame)
	//(*EventTable(FlashDialog)
	//*)
END_EVENT_TABLE()

FlashDialog::FlashDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    readlocations.insert(flash_out_address);
    for (int i = 0; i < flash_size; i++)
        storage.push_back(0xff);
	//(*Initialize(FlashDialog)
	Create(parent, id, _("Flash"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(BoxSizer1);
	ToolBar1 = new wxToolBar(this, ID_TOOLBAR1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER, _T("ID_TOOLBAR1"));
	ToolBarItem1 = ToolBar1->AddTool(TOOL_FLASH_LOAD, _("Load"), wxBitmap(wxImage(_T("icons/folder_page.png"))), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
	ToolBarItem2 = ToolBar1->AddTool(TOOL_FLASH_SAVE, _("Save"), wxBitmap(wxImage(_T("icons/disk.png"))), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
	ToolBar1->Realize();
	SetToolBar(ToolBar1);
	OpenFileDialog = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(TOOL_FLASH_LOAD,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&FlashDialog::OnToolBarOpenClicked);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&FlashDialog::OnClose);
	//*)
    storageview = new HexView(this, &storage);
    BoxSizer1->Prepend(storageview, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Fit(this);
}

FlashDialog::~FlashDialog()
{
    delete storageview;
	//(*Destroy(FlashDialog)
	//*)
}

void FlashDialog::write(unsigned int address, unsigned int data)
{
    if (address == flash_mid_address)
        regs.mid = data;
    else if (address == flash_lo_address)
        regs.lo = data;
    else if (address == flash_in_address)
        regs.in = data;
    else if (address == flash_sig_address)
    {
        uint8_t last_sig = regs.sig;
        regs.sig = data;
        if ((last_sig & flash_sig_we) && !(regs.sig & flash_sig_we))
        {
            int cmd = ((regs.lo | (regs.mid << 8) | ((regs.sig & 0x7) << 16)) << 8) | regs.in;
            command_history.push_front(cmd);
            processcommand();
        }
    }
}

void FlashDialog::processcommand()
{
    if (command_history.size() > 6)
        command_history.pop_back();
    if (command_history.size() < 4)
        return;
    std::list<int>::iterator iter = command_history.begin();
    for (int i = 0; i < 4; i++, ++iter);
    if (*(--iter) == 0x5555aa &&
        *(--iter) == 0x2aaa55 &&
        *(--iter) == 0x5555a0)
    {
        int cmd = *command_history.begin();
        storage[cmd >> 8] &= cmd & 0xff;    //not an assignment: flash writes can only clear bits
        storageview->Refresh();
        command_history.clear();
    }
    if (command_history.size() < 6)
        return;
    iter = command_history.begin();
    for (int i = 0; i < 6; i++, ++iter);
    if (*(--iter) == 0x5555aa &&
        *(--iter) == 0x2aaa55 &&
        *(--iter) == 0x555580 &&
        *(--iter) == 0x5555aa &&
        *(--iter) == 0x2aaa55)
    {
        int cmd = *command_history.begin();
        if ((cmd & 0xff) == 0x30)
        {
            int sax = (cmd >> 8) & ~0xfff;
            for (int i = sax; i < sax + 0x1000; i++)
                storage[i] = 0xff;
        }
        storageview->Refresh();
        command_history.clear();
    }
}

unsigned int FlashDialog::read(unsigned int address)
{
    if (address != flash_out_address)
        return 0;
    if (regs.sig & flash_sig_oe)
        return regs.in;
    else
        return storage[regs.lo | (regs.mid << 8) | ((regs.sig & 0x7) << 16)];
}

void FlashDialog::OnClose(wxCloseEvent& event)
{
    event.Veto();
    this->Hide();
}

void FlashDialog::OnToolBarOpenClicked(wxCommandEvent& event)
{
    if (OpenFileDialog->ShowModal() == wxID_OK)
    {
        SetFileName(OpenFileDialog->GetPath().ToStdString(), false);
        std::fstream file(filepath.c_str(), std::ios::in | std::ios::binary);
        file.seekg(0, std::ios::end);
        int length = file.tellg();
        file.seekg(0, std::ios::beg);
        uint8_t *inputbuf = new uint8_t[length];
        file.read((char*)inputbuf, length);
        file.close();

        storage.clear();
        storage.reserve(length);
        for (int i = 0; i < length; i++)
            storage.push_back(inputbuf[i]);
        delete[] inputbuf;
        while (storage.size() < (1 << 19))
            storage.push_back(0xff);
        storageview->Refresh();
    }
}

void FlashDialog::SetFileName(std::string path, bool hasbeenmodified)
{
    filepath = path;
    int index = path.rfind("/");
    if (index < 0)
        index = path.rfind("\\");
    filename = path.substr(index + 1, path.size() - index - 1);
    this->SetTitle((hasbeenmodified? "*" : "") + filename + " - Flash View");
    filehaschanged = hasbeenmodified;
}
