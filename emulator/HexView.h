#ifndef HEXVIEW_H
#define HEXVIEW_H

#include <vector>
#include <wx/panel.h>
#include <wx/wx.h>

class HexView: public wxPanel
{
    std::vector <uint8_t> *buffer;
    wxFont font;
    wxFont italicfont;
    wxBitmap *renderbuffer;

    wxScrollBar *scrollbar;
    long scrollbar_id;

    bool mousedown;
    wxPoint selection; // int coords of column, row

    void sendCharEvent(char c);
    void sendInt(int n);

public:

    struct cursor
    {
        wxColor colour;
        int index;
        int size;
        cursor(wxColor _colour, int _index, int _size) {colour = _colour; index = _index; size = _size;}
    };


    int padding;
    int n_columns;
    int pixels_per_column;
    int pixels_per_row;
    int address_columns;
    std::vector <cursor> cursors;


    HexView(wxFrame *parent, std::vector<uint8_t> *data, long id = wxID_ANY,
            long style = 0, wxString name = wxPanelNameStr);

    void mouseDown(wxMouseEvent &event);
    void mouseMove(wxMouseEvent &event);
    void mouseUp(wxMouseEvent &event);
    void keyPressed(wxKeyEvent &event);
    void keyReleased(wxKeyEvent &event);
    void onSize(wxSizeEvent &event);
    void onScroll(wxScrollEvent &event);

    void onErase(wxEraseEvent &event);
    void paintEvent(wxPaintEvent &event);
    void paintNow();

    void render(wxDC &targetdc);

    wxPoint getSelection();
    void setSelection(wxPoint); // performs bounding

    wxPoint screen2grid(wxPoint);
    wxPoint grid2screen(wxPoint);   // rather just use one matrix but...
    bool ongrid(wxPoint);

    wxPoint index2grid(int index);
    int grid2index(wxPoint grid);

    ~HexView();

    DECLARE_EVENT_TABLE()
};



#endif // HEXVIEW_H
