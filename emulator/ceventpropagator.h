// Inspired from code by 'rostfrei'

#include <wx/wx.h>

// ------------------------------------------------------------------------------------
// The CEventPropagator cutom event handler : it forces key events to propagate up

class CEventPropagator : public wxEvtHandler
{
public:
    CEventPropagator();
    static void registerFor(wxWindow* win);

private:
    void onKeyDown(wxKeyEvent& aEvent);
    void onKeyUp(wxKeyEvent& aEvent);
};


// ------------------------------------------------------------------------------------

