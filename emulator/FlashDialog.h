#ifndef FLASHDIALOG_H
#define FLASHDIALOG_H

#include <list>
#include <vector>

#include "HexView.h"
#include "peripheral.h"

//(*Headers(FlashDialog)
#include <wx/sizer.h>
#include <wx/toolbar.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
//*)

class FlashDialog: public wxFrame, public peripheral
{
    static const int flash_size = 0x80000;

    static const unsigned int flash_sig_address = 0xc002;
    static const unsigned int flash_mid_address = 0xc003;
    static const unsigned int flash_lo_address = 0xc004;
    static const unsigned int flash_in_address = 0xc005;
    static const unsigned int flash_out_address = 0xc006;

    static const int flash_sig_we = 0x80;
    static const int flash_sig_oe = 0x40;

    std::vector<uint8_t> storage;
    HexView *storageview;

    struct
    {
        uint8_t sig;
        uint8_t mid;
        uint8_t lo;
        uint8_t in;
    } regs;

    std::list<int> command_history;

public:

    FlashDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
    virtual ~FlashDialog();

    virtual void write(unsigned int address, unsigned int data);
    virtual unsigned int read(unsigned int address);
    void processcommand();

    //(*Declarations(FlashDialog)
    wxToolBar* ToolBar1;
    wxFileDialog* OpenFileDialog;
    wxToolBarToolBase* ToolBarItem1;
    wxBoxSizer* BoxSizer1;
    wxToolBarToolBase* ToolBarItem2;
    //*)

protected:

    //(*Identifiers(FlashDialog)
    static const long TOOL_FLASH_LOAD;
    static const long TOOL_FLASH_SAVE;
    static const long ID_TOOLBAR1;
    //*)

private:

    //(*Handlers(FlashDialog)
    void OnClose(wxCloseEvent& event);
    void OnToolBarOpenClicked(wxCommandEvent& event);
    //*)
    std::string filename;
    std::string filepath;
    bool filehaschanged;
    void SetFileName(std::string path, bool hasbeenmodified = false);

    DECLARE_EVENT_TABLE()
};

#endif
