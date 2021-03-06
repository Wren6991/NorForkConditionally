#include "HexView.h"

#include <iostream>
#include <sstream>
#include <wx/defs.h>
#include <wx/font.h>
#include <wx/validate.h>
#include <wx/clipbrd.h>

#include "ceventpropagator.h"

const int scrollbar_width = 17;

BEGIN_EVENT_TABLE(HexView, wxPanel)

    EVT_LEFT_DOWN(HexView::mouseDown)
    EVT_MOTION(HexView::mouseMove)
    EVT_LEFT_UP(HexView::mouseUp)
    EVT_KEY_DOWN(HexView::keyPressed)
    EVT_KEY_UP(HexView::keyReleased)
    EVT_SIZE(HexView::onSize)

    EVT_ERASE_BACKGROUND(HexView::onErase)
    EVT_PAINT(HexView::paintEvent)

END_EVENT_TABLE()

HexView::HexView(wxFrame *parent, std::vector<uint8_t> *data, long id, long style, wxString name)
    : wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, style | wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS, name)
{
    padding = 8;
    n_columns = 16;
    pixels_per_column = 24;
    pixels_per_row = 20;
    address_columns = 3;

    SetMinSize(wxSize((n_columns + address_columns) * pixels_per_column + 2 * padding + scrollbar_width, 100));
    buffer = data;
    font = wxFont(11, wxTELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Courier New");
    italicfont = wxFont(11, wxTELETYPE, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, "Courier New");
    SetBackgroundColour(wxColor(255, 255, 255));
    renderbuffer = new wxBitmap(0, 0);
    mousedown = false;

    scrollbar_id = wxNewId();
    scrollbar = new wxScrollBar(this, scrollbar_id, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);

    Connect(scrollbar_id, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&HexView::onScroll);
    Connect(scrollbar_id, wxEVT_SCROLL_CHANGED,    (wxObjectEventFunction)&HexView::onScroll);

    CEventPropagator::registerFor(this);
    this->SetCanFocus(true);
}

HexView::~HexView()
{
    scrollbar->PopEventHandler();
    delete scrollbar;
    delete renderbuffer;
}

void HexView::onErase(wxEraseEvent &event)
{

}

void HexView::paintEvent(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    render(dc);
}

void HexView::paintNow()
{
    wxClientDC dc(this);
    dc.Clear();
    render(dc);
}

std::string byte2str(uint8_t x)     // absolutely no need to do this! :)
{
    char str[3];
    if (x >= 0xa0)
        str[0] = 'a' + ((x - 0xa0) >> 4);
    else
        str[0] = '0' + (x >> 4);
    if (((x & 0xf) >= 0xa))
        str[1] = 'a' + ((x - 0xa) & 0xf);
    else
        str[1] = '0' + (x & 0xf);
    str[2] = 0;
    return std::string(str);
}

void HexView::render(wxDC &targetdc)
{
    wxMemoryDC dc(*renderbuffer);
    dc.Clear();

    int n_rows = (dc.GetSize().GetY() - padding + pixels_per_row - 1) / pixels_per_row;
    int scroll = scrollbar->GetThumbPosition();
    wxPoint pos;

    dc.SetBrush(wxBrush(wxColour(224, 224, 224)));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(wxPoint(0, 0), wxSize(address_columns * pixels_per_column - 8, dc.GetSize().GetHeight()));

    for (unsigned int i = 0; i < cursors.size(); i++)
    {
        cursor &cur = cursors[i];
        dc.SetBrush(wxBrush(cur.colour));
        int index = cur.index;
        for (int count = 0; count < cur.size; count++)
        {
            if (ongrid(index2grid(index)))
                dc.DrawRectangle(grid2screen(index2grid(index)), wxSize(pixels_per_column, pixels_per_row - 4));
            index++;
        }
    }
    if (ongrid(selection))
    {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawRectangle(grid2screen(selection), wxSize(pixels_per_column - 4, pixels_per_row - 4));
    }
    dc.SetFont(italicfont);
    for (int row = 0; row < n_rows; row++)
    {
        int address = (row + scroll) * n_columns;
        dc.DrawText(byte2str(address >> 8), padding, padding + row * pixels_per_row);
        dc.DrawText(byte2str(address), padding + pixels_per_column, padding + row * pixels_per_row);
    }
    dc.SetFont(font);
    for (int i = scroll * n_columns; i < (scroll  + n_rows) * n_columns; i++)
    {
        if ((unsigned)i > buffer->size())
            break;
        pos = grid2screen(wxPoint(i % n_columns, i / n_columns));
        dc.DrawText(byte2str((*buffer)[i]), pos.x, pos.y);
    }
    targetdc.Blit(0, 0, dc.GetSize().GetX(), dc.GetSize().GetY(), &dc, 0, 0);
}


wxPoint HexView::screen2grid(wxPoint in)
{
    return wxPoint((in.x - padding) / pixels_per_column - address_columns,
                   (in.y - padding) / pixels_per_row + scrollbar->GetThumbPosition());
}

wxPoint HexView::grid2screen(wxPoint in)
{
    return wxPoint((in.x + address_columns) * pixels_per_column + padding,
                   (in.y - scrollbar->GetThumbPosition()) * pixels_per_row + padding);
}

bool HexView::ongrid(wxPoint in)
{
    in.y -= scrollbar->GetThumbPosition();
    return in.x >= 0 && in.x < n_columns &&
           in.y >= 0 && in.y < this->GetSize().GetHeight() / pixels_per_row + 1;
}

wxPoint HexView::index2grid(int index)
{
    return wxPoint(index % n_columns, index / n_columns);
}

int HexView::grid2index(wxPoint grid)
{
    return grid.y * n_columns + grid.x;
}

void HexView::mouseDown(wxMouseEvent &event)
{
    mousedown = true;
    mouseMove(event);
    this->SetFocus();
}

void HexView::mouseMove(wxMouseEvent &event)
{
    if (mousedown)
    {
        setSelection(screen2grid(event.GetPosition()));
        Refresh();
    }

}

void HexView::mouseUp(wxMouseEvent &event)
{
    mousedown = false;
}

void HexView::sendCharEvent(char c)
{
    wxKeyEvent evt;
    evt.m_keyCode = ' ';
    keyPressed(evt);
}

void HexView::sendInt(int n)
{
    (*buffer)[grid2index(selection)] = n;
}

void HexView::keyPressed(wxKeyEvent &event)
{
    int c = event.GetKeyCode();
    int index = grid2index(selection);
    std::vector<uint8_t> &buf = *buffer;

    bool needtorefresh = true;

    if (c >= '0' && c <= '9')
        buf[index] = (buf[index] << 4) | (c - '0');
    else if (c >= 'a' && c <= 'f')
        buf[index] = (buf[index] << 4) | (c - 'a' + 0xa);
    else if (c >= 'A' && c <= 'F')
        buf[index] = (buf[index] << 4) | (c - 'A' + 0xa);
    else if (c == ' ' || c  == WXK_RETURN || c == WXK_RIGHT)
        setSelection(index2grid(index + 1));
    else if (c == WXK_LEFT || c == WXK_BACK)
        setSelection(index2grid(index - 1));
    else if (c == WXK_UP)
        setSelection(selection + wxPoint(0, -1));
    else if (c == WXK_DOWN)
        setSelection(selection + wxPoint(0, 1));
    else if ((c == 'G' || c == 'g') && event.AltDown())
    {
        std::string addresstext = wxGetTextFromUser("Enter address (hex):", "Go to Address", "", this).ToStdString();
        int address;
        std::stringstream ss;
        ss << std::hex << addresstext;
        ss >> address;
        setSelection(index2grid(address));
    }
    else if ((c == 'V' || c == 'v') && event.ControlDown())
    {
        if (wxTheClipboard->Open())
        {
            if (wxTheClipboard->IsSupported(wxDF_TEXT))
            {
                wxTextDataObject data;
                wxTheClipboard->GetData(data);
                wxMessageBox(data.GetText());
                std::string text = data.GetText().ToStdString();
                bool in_whitespace = false;
                bool int_read = false;
                int result = 0;
                for (unsigned int i = 0; i < text.size(); i++)
                {
                    char c = text[i];
                    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',')
                    {
                        if (!in_whitespace)
                        {
                            in_whitespace = true;
                            if (int_read)
                            {
                                int_read = false;
                                sendInt(result);
                                result = 0;
                            }
                            sendCharEvent(' ');
                        }
                    }
                    else
                    {
                        in_whitespace = false;
                        bool digit_read = true;
                        if (c >= '0' && c <= '9')
                            result = (result << 4) + c - '0';
                        else if (c >= 'a' && c <= 'f')
                            result = (result << 4) + c - 'a' + 10;
                        else if (c >= 'A' && c <= 'F')
                            result = (result << 4) + c - 'A' + 10;
                        else
                            digit_read = false;
                        int_read = int_read || digit_read;
                    }
                }
                if (int_read)
                    sendInt(result);
            }
            wxTheClipboard->Close();
        }
    }
    else
        needtorefresh = false;

    if (needtorefresh)
    {
        int n_rows = (this->GetSize().GetY() - padding + pixels_per_row - 1) / pixels_per_row;
        int pos = selection.y - scrollbar->GetThumbPosition();
        if (pos < 0)
            scrollbar->SetThumbPosition(selection.y);
        else if (pos > n_rows - 1)
            scrollbar->SetThumbPosition(selection.y - (n_rows - 1));
        Refresh();
    }
}

void HexView::keyReleased(wxKeyEvent &event)
{
    /*int c = event.GetKeyCode();
    int index = grid2index(selection);*/

    bool needtorefresh = true;

    /*if (c == WXK_LEFT)
        selection = index2grid(index - 1);
    else if (c == WXK_RIGHT)
        selection = index2grid(index + 1);
    else if (c == WXK_UP)
        selection = selection + wxPoint(0, -1);
    else if (c == WXK_DOWN)
        selection = selection + wxPoint(0, 1);
    else*/
    needtorefresh = false;

    if (needtorefresh)
        Refresh();
}


void HexView::setSelection(wxPoint sel)
{
    if (sel.x >= n_columns)
        sel.x = n_columns - 1;
    else if (sel.x < 0)
        sel.x = 0;

    if (grid2index(sel) < 0)
        sel = index2grid(0);
    else if ((unsigned)grid2index(sel) >= buffer->size())
        sel = index2grid(buffer->size() - 1);

    selection = sel;
}

wxPoint HexView::getSelection()
{
    return selection;
}

void HexView::onSize(wxSizeEvent &event)
{
    int selectionpos = grid2index(selection);
    int newcolumncount = event.GetSize().GetX() / pixels_per_column - address_columns;
    n_columns = newcolumncount - (newcolumncount % 8);
    if (!n_columns)
        n_columns = 8;
    selection = index2grid(selectionpos);
    scrollbar->SetSize(event.GetSize().GetWidth() - scrollbar_width, 0,
                       scrollbar_width, event.GetSize().GetHeight());     // x, y, width, height
    scrollbar->SetScrollbar(scrollbar->GetThumbPosition(), 1,
                            (buffer->size() + n_columns - 1) / n_columns - this->GetSize().GetHeight() / pixels_per_row + 1,
                            1, true);
    delete renderbuffer;
    renderbuffer = new wxBitmap(event.GetSize().GetWidth(), event.GetSize().GetHeight());
}

void HexView::onScroll(wxScrollEvent &event)
{
    Refresh();
}
