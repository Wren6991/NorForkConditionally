#include "LCDView.h"

#include <iostream>
#include <wx/defs.h>
#include <wx/font.h>
#include <wx/validate.h>


const int scrollbar_width = 17;

BEGIN_EVENT_TABLE(LCDView, wxPanel)

    EVT_SIZE(LCDView::onSize)

    EVT_ERASE_BACKGROUND(LCDView::onErase)
    EVT_PAINT(LCDView::paintEvent)

END_EVENT_TABLE()

LCDView::LCDView(wxFrame *parent, long id, long style, wxString name)
    : wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, style | wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS, name)
{
    padding = 8;
    n_columns = 20;
    n_rows = 4;
    pixels_per_column = 16;
    pixels_per_row = 20;
    address_columns = 3;

    SetMinSize(wxSize((n_columns + address_columns) * pixels_per_column + 2 * padding + scrollbar_width, 100));
    buffer[0] = 'H'; buffer[1] = 'e'; buffer[2] = 'l'; buffer[3] = 'l'; buffer[4] = 'o';
    for (int i = 0; i < 20; i++)
    {
        buffer[i + 20] = '0' + (i % 10);
        buffer[i + 40] = '0' + (i % 10);
        buffer[i + 60] = '0' + (i % 10);
    }
    font = wxFont(14, wxTELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Courier New");
    SetBackgroundColour(wxColor(160, 192, 48));
    renderbuffer = new wxBitmap(0, 0);
}

LCDView::~LCDView()
{
    delete renderbuffer;
}

void LCDView::onErase(wxEraseEvent &event)
{

}

void LCDView::paintEvent(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    render(dc);
}

void LCDView::paintNow()
{
    wxClientDC dc(this);
    dc.Clear();
    render(dc);
}

char* char2str(char c)
{
    static char str[2];
    str[0] = c;
    str[1] = 0;
    return str;
}

void LCDView::render(wxDC &targetdc)
{
    wxMemoryDC dc(*renderbuffer);
    dc.SetBackground(wxBrush(wxColor(160, 192, 48)));
    dc.Clear();
    dc.SetFont(font);

    for (int i = 0; i < LCD_N_CHARS; i++)
    {
        wxPoint pos = wxPoint((i % n_columns) * pixels_per_column + 2, i / n_columns * pixels_per_row);
        dc.DrawText(buffer[i], pos.x, pos.y);
    }
    dc.SetPen(wxPen(wxColour(144, 176, 40)));
    for (int i = 1; i < n_rows; i++)
        dc.DrawLine(0, i * pixels_per_row, n_columns * pixels_per_column, i * pixels_per_row);
    for (int i = 1; i < n_columns; i++)
        dc.DrawLine(i * pixels_per_column, 0, i * pixels_per_column, n_rows * pixels_per_row);
    targetdc.Blit(0, 0, dc.GetSize().GetX(), dc.GetSize().GetY(), &dc, 0, 0);
}




void LCDView::onSize(wxSizeEvent &event)
{
    delete renderbuffer;
    renderbuffer = new wxBitmap(event.GetSize().GetWidth(), event.GetSize().GetHeight());
}
