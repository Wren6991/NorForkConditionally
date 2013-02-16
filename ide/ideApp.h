/***************************************************************
 * Name:      ideApp.h
 * Purpose:   Defines Application Class
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-02-16
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#ifndef IDEAPP_H
#define IDEAPP_H

#include <wx/app.h>

class ideApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // IDEAPP_H
