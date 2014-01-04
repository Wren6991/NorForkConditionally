/***************************************************************
 * Name:      emulatorMain.h
 * Purpose:   Defines Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-21
 * Copyright: Luke Wren (http://githhub.com/Wren6991)
 * License:
 **************************************************************/

#ifndef EMULATORMAIN_H
#define EMULATORMAIN_H

//( *Headers(emulatorFrame)
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/slider.h>
#include <wx/filedlg.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/statusbr.h>
//*)
#include <vector>

#include "HexView.h"
#include "LCDDialog.h"
#include "FlashDialog.h"
#include "led.h"
#include "vm.h"

class emulatorFrame: public wxFrame
{
public:

    emulatorFrame(wxWindow* parent,wxWindowID id = -1);
    virtual ~emulatorFrame();

private:

    std::string filename;
    std::string filepath;
    bool filehaschanged;

    HexView *memview;
    LCDDialog *lcd;
    FlashDialog *flash;
    std::vector <uint8_t> buffer;
    vm machine;

    //(*Handlers(emulatorFrame)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNewClicked(wxCommandEvent& event);
    void OnOpenClicked(wxCommandEvent& event);
    void OnSaveClicked(wxCommandEvent& event);
    void OnSaveAsClicked(wxCommandEvent& event);
    void OnDebuginChange(wxScrollEvent& event);
    void OnSlider7CmdScroll(wxScrollEvent& event);
    void OnStepClicked(wxCommandEvent& event);
    void OnTimer1Trigger(wxTimerEvent& event);
    void OnTimerTickTrigger(wxTimerEvent& event);
    void OnbtnResetClick(wxCommandEvent& event);
    void OnbtnStartStopClick(wxCommandEvent& event);
    void OnlstWatchesItemRClick(wxListEvent& event);
    void OnMenuAddWatchSelected(wxCommandEvent& event);
    void OnMenuItemDeleteSelected(wxCommandEvent& event);
    void OnLCDClicked(wxCommandEvent& event);
    void OnFlashClicked(wxCommandEvent& event);
    //*)

    //(*Identifiers(emulatorFrame)
    static const long ID_LSTWATCHES;
    static const long ID_STATICTEXT1;
    static const long ID_LED7;
    static const long ID_LED6;
    static const long ID_LED5;
    static const long ID_LED4;
    static const long ID_LED3;
    static const long ID_LED2;
    static const long ID_LED1;
    static const long ID_LED0;
    static const long ID_STATICTEXT2;
    static const long ID_SLIDER7;
    static const long ID_SLIDER6;
    static const long ID_SLIDER5;
    static const long ID_SLIDER4;
    static const long ID_SLIDER3;
    static const long ID_SLIDER2;
    static const long ID_SLIDER1;
    static const long ID_SLIDER0;
    static const long ID_BUTTON1;
    static const long ID_BUTTON2;
    static const long ID_STATUSBAR1;
    static const long TOOL_NEW;
    static const long TOOL_OPEN;
    static const long TOOL_SAVE;
    static const long ID_TOOLBARITEM1;
    static const long ID_TOOLBARITEM2;
    static const long TOOL_LCD;
    static const long TOOL_FLASH;
    static const long TOOL_ABOUT;
    static const long ID_TOOLBAR1;
    static const long ID_TIMER1;
    static const long ID_MENUADDWATCH;
    static const long ID_MENUCHANGEADDRESS;
    static const long ID_MENUDELETEWATCH;
    //*)

    //(*Declarations(emulatorFrame)
    wxSlider* Slider1;
    wxToolBarToolBase* ToolBarItem4;
    wxTimer TimerTick;
    wxToolBar* ToolBar1;
    wxSlider* Slider2;
    wxLed* Led4;
    wxToolBarToolBase* ToolBarItem3;
    wxStaticText* StaticText2;
    wxMenuItem* MenuItem2;
    wxSlider* Slider5;
    wxLed* Led1;
    wxLed* Led7;
    wxMenuItem* MenuItem1;
    wxLed* Led5;
    wxSlider* Slider3;
    wxMenu mnuWatch;
    wxSlider* Slider0;
    wxFileDialog* dlgOpen;
    wxLed* Led6;
    wxSlider* Slider7;
    wxStaticText* StaticText1;
    wxLed* Led0;
    wxLed* Led3;
    wxToolBarToolBase* ToolBarItem6;
    wxToolBarToolBase* ToolBarItem1;
    wxMenuItem* MenuItem3;
    wxListCtrl* lstWatches;
    wxButton* btnReset;
    wxStatusBar* StatusBar1;
    wxSlider* Slider4;
    wxButton* btnStartStop;
    wxToolBarToolBase* ToolBarItem5;
    wxToolBarToolBase* ToolBarItem8;
    wxSlider* Slider6;
    wxLed* Led2;
    wxFileDialog* dlgSaveAs;
    wxToolBarToolBase* ToolBarItem2;
    wxToolBarToolBase* ToolBarItem7;
    //*)

    void SetFileName(std::string path, bool hasbeenmodified = false);
    bool TakeStep();
    void UpdateDisplay();

    DECLARE_EVENT_TABLE()
};

#endif // EMULATORMAIN_H
