#ifndef LCDVIEW_H
#define LCDVIEW_H

#include <vector>
#include <wx/panel.h>
#include <wx/wx.h>

#define LCD_N_CHARS 80

class LCDView: public wxPanel
{
    wxFont font;
    wxFont italicfont;
    wxBitmap *renderbuffer;

public:

    char buffer[LCD_N_CHARS];
    int padding;
    int n_columns;
    int n_rows;
    int pixels_per_column;
    int pixels_per_row;
    int address_columns;

    LCDView(wxFrame *parent, long id = wxID_ANY,
            long style = 0, wxString name = wxPanelNameStr);


    void onSize(wxSizeEvent &event);

    void onErase(wxEraseEvent &event);
    void paintEvent(wxPaintEvent &event);
    void paintNow();

    void render(wxDC &targetdc);

    wxPoint getSelection();

    ~LCDView();

    DECLARE_EVENT_TABLE()
};



#endif // LCDVIEW_H
