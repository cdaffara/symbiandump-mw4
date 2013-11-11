/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef HBAUTOTEST_H
#define HBAUTOTEST_H

#include <hbnamespace.h>
#include <hbmainwindow.h>
#include <QtTest/QtTest>

class HbMainWindow;
class HbWidget;
class HbAutoTestMainWindow;
class HbAutoTest;
class HbAutoTestMouseEvent; 
/*
INSTRUCTIONS:
The class HbAutoTest is meant to be used with Orbit applications auto testing instead of GUI testing APIs of QTestLib.

The functions of this class is to used similarily to the related QTestLib functions.

Use HbAutoTestMainWindow (defined below) instead of HbMainWindow to enble filtering.
Filterin filters out UI events that are not sent by function defined in HbAutoTest class.

*/

class HbAutoTestMouseEvent : public QMouseEvent
{
public:
    HbAutoTestMouseEvent(Type type, const QPoint & pos, const QPoint & globalPos, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers ) 
        : QMouseEvent(type,pos,globalPos,button,buttons,modifiers){}
};

class HbAutoTestKeyEvent : public QKeyEvent
{
public:
        HbAutoTestKeyEvent(Type type, int key, Qt::KeyboardModifiers modifiers, const QString& text = QString(),
              bool autorep = false, ushort count = 1 ) 
        : QKeyEvent(type, key, modifiers, text, autorep, count){}
};

class HbAutoTest 
{
public:
    
    static void mouseMove (HbAutoTestMainWindow *window, HbWidget *widget, QPointF pos = QPointF(), int delay = -1 );
    static void mousePress (HbAutoTestMainWindow *window, HbWidget *widget, QPointF pos = QPointF(), int delay = -1);
    static void mouseRelease (HbAutoTestMainWindow *window, HbWidget *widget, QPointF pos = QPointF(), int delay = -1);
    static void mouseClick (HbAutoTestMainWindow *window, const HbWidget *widget, QPointF pos = QPointF(), int delay = -1);
   
private:
    static void drag(HbAutoTestMainWindow *window, QPointF targetPoint);
    static QPointF middlePointOfWidget( const HbWidget* widget);

    static bool pointerPressed;
    static QPointF pressPoint;

//Key event Part: copy-pasted from QTestLib and modified to support HbAutoTestKeyEvent to enable filtering.
//see HbAutoTestMainWindow below.
public:
    
    static Qt::Key asciiToKey(char ascii);
    static char keyToAscii(Qt::Key key);

    static void simulateEvent(QWidget *widget, bool press, int code,
                              Qt::KeyboardModifiers modifier, QString text, bool repeat, int delay=-1);

    static void sendKeyEvent(QTest::KeyAction action, QWidget *widget, Qt::Key code,
                             QString text, Qt::KeyboardModifiers modifier, int delay=-1);    

    static void sendKeyEvent(QTest::KeyAction action, QWidget *widget, Qt::Key code,
                             char ascii, Qt::KeyboardModifiers modifier, int delay=-1);

    static void keyEvent(QTest::KeyAction action, QWidget *widget, char ascii,
                                Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1);
 
    static void keyEvent(QTest::KeyAction action, QWidget *widget, Qt::Key key,
                                Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1);

    static void keyClicks(QWidget *widget, const QString &sequence,
                                 Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1);

    static void keyPress(QWidget *widget, char key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1);
    
    static void keyRelease(QWidget *widget, char key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1);
    
    static void keyClick(QWidget *widget, char key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1);
    
    static void keyPress(QWidget *widget, Qt::Key key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1);
   
    static void keyRelease(QWidget *widget, Qt::Key key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1);

    static void keyClick(QWidget *widget, Qt::Key key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1);
};

class HbTestEvent
{
public:
    virtual void simulate(QWidget *w) = 0;
    virtual HbTestEvent *clone() const = 0;

    virtual ~HbTestEvent() {}
};

class HbTestKeyEvent: public HbTestEvent
{
public:
    inline HbTestKeyEvent(QTest::KeyAction action, Qt::Key key, Qt::KeyboardModifiers modifiers, int delay)
        : _action(action), _delay(delay), _modifiers(modifiers), _ascii(0), _key(key) {}
    inline HbTestKeyEvent(QTest::KeyAction action, char ascii, Qt::KeyboardModifiers modifiers, int delay)
        : _action(action), _delay(delay), _modifiers(modifiers),
          _ascii(ascii), _key(Qt::Key_unknown) {}
    inline HbTestEvent *clone() const { return new HbTestKeyEvent(*this); }

    inline void simulate(QWidget *w)
    {
        if (_ascii == 0)
            HbAutoTest::keyEvent(_action, w, _key, _modifiers, _delay);
        else
            HbAutoTest::keyEvent(_action, w, _ascii, _modifiers, _delay);
    } 

protected:
    QTest::KeyAction _action;
    int _delay;
    Qt::KeyboardModifiers _modifiers;
    char _ascii;
    Qt::Key _key;
};

class HbTestKeyClicksEvent: public HbTestEvent
{
public:
    inline HbTestKeyClicksEvent(const QString &keys, Qt::KeyboardModifiers modifiers, int delay)
        : _keys(keys), _modifiers(modifiers), _delay(delay) {}
    inline HbTestEvent *clone() const { return new HbTestKeyClicksEvent(*this); }

    inline void simulate(QWidget *w)
    {
        HbAutoTest::keyClicks(w, _keys, _modifiers, _delay);
    }

private:
    QString _keys;
    Qt::KeyboardModifiers _modifiers;
    int _delay;
};

class HbTestDelayEvent: public HbTestEvent
{
public:
    inline HbTestDelayEvent(int msecs): _delay(msecs) {}
    inline HbTestEvent *clone() const { return new HbTestDelayEvent(*this); }

    inline void simulate(QWidget * /*w*/) { QTest::qWait(_delay); }

private:
    int _delay;
};

class HbTestEventList: public QList<HbTestEvent *>
{
public:
    inline HbTestEventList() {}
    inline HbTestEventList(const HbTestEventList &other): QList<HbTestEvent *>()
    { for (int i = 0; i < other.count(); ++i) append(other.at(i)->clone()); }
    inline ~HbTestEventList()
    { clear(); }
    inline void clear()
    { qDeleteAll(*this); QList<HbTestEvent *>::clear(); }

    inline void addKeyClick(Qt::Key qtKey, Qt::KeyboardModifiers modifiers = Qt::NoModifier, int msecs = -1)
    { addKeyEvent(QTest::Click, qtKey, modifiers, msecs); }
    inline void addKeyPress(Qt::Key qtKey, Qt::KeyboardModifiers modifiers = Qt::NoModifier, int msecs = -1)
    { addKeyEvent(QTest::Press, qtKey, modifiers, msecs); }
    inline void addKeyRelease(Qt::Key qtKey, Qt::KeyboardModifiers modifiers = Qt::NoModifier, int msecs = -1)
    { addKeyEvent(QTest::Release, qtKey, modifiers, msecs); }
    inline void addKeyEvent(QTest::KeyAction action, Qt::Key qtKey,
                            Qt::KeyboardModifiers modifiers = Qt::NoModifier, int msecs = -1)
    { append(new HbTestKeyEvent(action, qtKey, modifiers, msecs)); }

    inline void addKeyClick(char ascii, Qt::KeyboardModifiers modifiers = Qt::NoModifier, int msecs = -1)
    { addKeyEvent(QTest::Click, ascii, modifiers, msecs); }
    inline void addKeyPress(char ascii, Qt::KeyboardModifiers modifiers = Qt::NoModifier, int msecs = -1)
    { addKeyEvent(QTest::Press, ascii, modifiers, msecs); }
    inline void addKeyRelease(char ascii, Qt::KeyboardModifiers modifiers = Qt::NoModifier, int msecs = -1)
    { addKeyEvent(QTest::Release, ascii, modifiers, msecs); }
    inline void addKeyClicks(const QString &keys, Qt::KeyboardModifiers modifiers = Qt::NoModifier, int msecs = -1)
    { append(new HbTestKeyClicksEvent(keys, modifiers, msecs)); }
    inline void addKeyEvent(QTest::KeyAction action, char ascii, Qt::KeyboardModifiers modifiers = Qt::NoModifier, int msecs = -1)
    { append(new HbTestKeyEvent(action, ascii, modifiers, msecs)); }

    inline void addDelay(int msecs)
    { append(new HbTestDelayEvent(msecs)); }

    inline void simulate(QWidget *w)
    {
        for (int i = 0; i < count(); ++i)
            at(i)->simulate(w);
    }
};

class HbAutoTestMainWindow : public HbMainWindow
{
public:
    HbAutoTestMainWindow() : HbMainWindow() {}
    
    void mousePressEvent(QMouseEvent *event)
    {
        qDebug(
            "HbAutoTestMainWindow::mousePressEvent: x(%d) y(%d)",
            event->x(),
            event->y());
         if ( dynamic_cast<HbAutoTestMouseEvent *>(event) ) {
                HbMainWindow::mousePressEvent(event);
            } else {
                ;//Do nothing 
         }
    }

    void mouseMoveEvent(QMouseEvent *event)
    {
        if ( dynamic_cast<HbAutoTestMouseEvent *>(event) ) {
                HbMainWindow::mouseMoveEvent(event);
            } else {
                ;//Do nothing
         }
    }

    void mouseReleaseEvent(QMouseEvent *event)
    {
         if ( dynamic_cast<HbAutoTestMouseEvent *>(event) ) {
                HbMainWindow::mouseReleaseEvent(event);
            } else {
                ;//Do nothing  
         }
    }

    void keyPressEvent(QKeyEvent *event)
    {
        if ( dynamic_cast<HbAutoTestKeyEvent *>(event) ) {
                HbMainWindow::keyPressEvent(event);
            } else {
                ;//Do nothing  
         }
    }
    void keyReleaseEvent(QKeyEvent *event)
    {
        if ( dynamic_cast<HbAutoTestKeyEvent *>(event) ) {
                HbMainWindow::keyReleaseEvent(event);
            } else {
                ;//Do nothing  
         }
    }

    void mouseDoubleClickEvent(QMouseEvent *event)
    {
        Q_UNUSED(event);
        //Just ignore, not supported in Orbit
    }
};


Q_DECLARE_METATYPE(HbTestEventList)
#endif //HBAUTOTEST_H
