#ifndef SCREENSIZE_H
#define SCREENSIZE_H

#include <qobject.h>
#include <qrect.h>
#include <qapplication.h>
#include <qdesktopwidget.h>

class ScreenSize : public QObject
{
public:
    inline static QRect GetScreenRect()
    {
        return   QApplication::desktop()->screenGeometry();
    }
};

#endif // SCREENSIZE_H
