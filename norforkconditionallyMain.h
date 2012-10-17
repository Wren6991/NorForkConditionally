/***************************************************************
 * Name:      norforkconditionallyMain.h
 * Purpose:   Defines Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2012-10-16
 * Copyright: Luke Wren ()
 * License:
 **************************************************************/

#ifndef NORFORKCONDITIONALLYMAIN_H
#define NORFORKCONDITIONALLYMAIN_H

//(*Headers(norforkconditionallyFrame)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/slider.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/timer.h>
//*)

class norforkconditionallyFrame: public wxFrame
{
    public:

        norforkconditionallyFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~norforkconditionallyFrame();

    private:

        //(*Handlers(norforkconditionallyFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnbtnRunStopClick(wxCommandEvent& event);
        void OnButton2Click(wxCommandEvent& event);
        void OnbtnLoadClick(wxCommandEvent& event);
        void Ondbg7CmdScroll(wxScrollEvent& event);
        void OndbgCmdScrollThumbTrack(wxScrollEvent& event);
        void Ondbg5CmdScrollThumbRelease(wxScrollEvent& event);
        void OnButton1Click1(wxCommandEvent& event);
        void OnbtnStepClick(wxCommandEvent& event);
        void OnTimer1Trigger(wxTimerEvent& event);
        void OntmrStepTrigger(wxTimerEvent& event);
        //*)
        void step();

        //(*Identifiers(norforkconditionallyFrame)
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON2;
        static const long ID_TEXTCTRL2;
        static const long ID_STATICTEXT1;
        static const long ID_SLIDER9;
        static const long ID_SLIDER8;
        static const long ID_SLIDER7;
        static const long ID_SLIDER6;
        static const long ID_SLIDER5;
        static const long ID_SLIDER4;
        static const long ID_SLIDER3;
        static const long ID_SLIDER2;
        static const long ID_BUTTON1;
        static const long ID_BUTTON3;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM2;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_TIMER1;
        //*)

        //(*Declarations(norforkconditionallyFrame)
        wxSlider* dbg3;
        wxTimer tmrStep;
        wxSlider* dbg4;
        wxSlider* dbg5;
        wxTextCtrl* txtOutput;
        wxSlider* dbg7;
        wxButton* btnRunStop;
        wxButton* btnStep;
        wxSlider* dbg0;
        wxStaticText* StaticText1;
        wxTextCtrl* txtProgram;
        wxSlider* dbg2;
        wxButton* btnLoad;
        wxSlider* dbg6;
        wxSlider* dbg1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // NORFORKCONDITIONALLYMAIN_H
