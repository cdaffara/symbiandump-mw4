/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Engine header file
 *
*/


#ifndef __INC_SVGEVENT_H__
#define __INC_SVGEVENT_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "GfxFloatFixPt.h"




//*******************************************************
// constants
const TUint8    KSvgEventMaskNone           = 0x0;
const TUint8    KSvgEventMaskExternalUI     = 0x1;
const TUint8    KSvgEventMaskTimer          = 0x2;
const TUint8    KSvgEventMaskInternal       = 0x4;
const TUint8    KSvgEventMaskExternalSystem = 0x8;

enum TSvgEngineEventType
    {
    ESvgEngineEventTimerPrep,
    ESvgEngineEventTimer,
    ESvgEngineInternalEvent,

    ESvgEngineEventMouseDown,
    ESvgEngineEventMouseUp,
    ESvgEngineEventMouseMove,
    ESvgEngineEventKeyPress,
    ESvgEngineEventScreenUpdate
    };

enum TSvgEvent
    {
    // 26 events defined in SVG spec
    ESvgEventFocusin                            = 0,
    ESvgEventFocusout                           = 1,
    ESvgEventActivate                           = 2,
    ESvgEventClick                              = 3,
    ESvgEventMousedown                          = 4,
    ESvgEventMouseup                            = 5,
    ESvgEventMouseover                          = 6,
    ESvgEventMousemove                          = 7,
    ESvgEventMouseout                           = 8,
    ESvgEventDOMSubtreeModified                 = 9,
    ESvgEventDOMNodeInserted                    = 10,
    ESvgEventDOMNodeRemoved                     = 11,
    ESvgEventDOMNodeRemovedFromDocument         = 12,
    ESvgEventDOMNodeInsertedIntoDocument        = 13,
    ESvgEventDOMAttrModified                    = 14,
    ESvgEventDOMCharacterDataModified           = 15,
    ESvgEventSVGLoad                            = 16,
    ESvgEventSVGUnload                          = 17,
    ESvgEventSVGAbort                           = 18,
    ESvgEventSVGError                           = 19,
    ESvgEventSVGResize                          = 20,
    ESvgEventSVGScroll                          = 21,
    ESvgEventSVGZoom                            = 22,
    ESvgEventBeginEvent                         = 23,
    ESvgEventEndEvent                           = 24,
    ESvgEventRepeatEvent                        = 25,

        // Extension for implementation
    ESvgEventKey                                = 100,
    ESvgEventWallClock                          = 101,
    ESvgEventNone                               = -1
    };

class   CSvgElementImpl;

class MSvgEvent
    {
    public:
         /**
         * Accessor that returns the event mask
         *
         * @since 1.0
         * @param None
         * @return An integer that is the event mask
         */
        virtual TUint8              EventMask() = 0;

         /**
         * Accessor for the event type
         *
         * @since 1.0
         * @param None
         * @return A TSvgEngineEventType that is the event type
         */
        virtual TSvgEngineEventType EventType() = 0;
    };


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgTimerEvent : public MSvgEvent
    {
    public:

        /**
         * Accessor that returns the time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the time of the event
         */
        virtual TUint32  Time() = 0;

        /**
         * Accessor that returns the begin time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the begin time of the event
         */
        virtual TInt32          BeginTime() = 0;

        /**
         * Accessor that sets time of the event
         *
         * @since 1.0
         * @param TInt32 An integer that is the begin time of the event
         * @return none
         */
        virtual void SetTime(TInt32 aTime) = 0;

        /**
         * Accessor that sets the begin time of the event
         *
         * @since 1.0
         * @param aTime - An integer that is the begin time
         * @return None
         */
        virtual void            SetBeginTime( TInt32 aTime ) = 0;

    };

// An internal event specialization
class MSvgInternalEvent : public MSvgEvent
    {
    public:

virtual TBool       UserSeek() = 0;
        /**
         * Return a pointer the element that is the target for the event
         *
         * @since 1.0
         * @param None
         * @return A pointer to the target element
         */
        virtual CSvgElementImpl* ObjectAddress() = 0;

        /**
         * Return the event description
         *
         * @since 1.0
         * @param None
         * @return A TSvgEvent type
         */
        virtual TSvgEvent       SvgEvent() = 0;

        /**
         * Accessor that returns the time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the time of the event
         */
        virtual TInt32          Time() = 0;

        /**
         * Accessor that sets the time of the event
         *
         * @since 1.0
         * @param aTime - An integer that is the time of the event
         * @return
         */
        virtual void            SetTime( TInt32 aTime ) = 0;

         /**
         * Accessor that returns the begin time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the begin time of the event
         */
        virtual TInt32          BeginTime() = 0;

         /**
         * Accessor that sets the begin time of the event
         *
         * @since 1.0
         * @param aTime - An integer that is the begin time
         * @return None
         */
        virtual void            SetBeginTime( TInt32 aTime ) = 0;

    };

// A UI Mouse event specialization
class MSvgUiMouseEvent : public MSvgEvent
    {
    public:

        /**
         * Accessor for the x coordinate of the mouse event
         *
         * @since 1.0
         * @param None
         * @return A fixed point that is the x coordinate of the event
         */
        virtual TFloatFixPt  X() = 0;

        /**
         * Accessor for the y coordinate of the mouse event
         *
         * @since 1.0
         * @param None
         * @return A fixed point that is the y coordinate of the event
         */
        virtual TFloatFixPt  Y() = 0;
    };

// A UI Key event specialization
class MSvgUiKeyEvent : public MSvgEvent
    {
    public:

        /**
         * Accessor that returns the key code of the event
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TUint32 KeyCode() = 0;

        /**
         * Accessor that returns the time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the time of the event
         */
        virtual TInt32  Time() = 0;

        /**
         * Accessor that sets the time of the event
         *
         * @since 1.0
         * @param aTime - An integer that is the time of the event
         * @return
         */
        virtual void    SetTime( TInt32 aTime ) = 0;

         /**
         * Accessor that returns the begin time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the begin time of the event
         */
        virtual TInt32 BeginTime() = 0;

         /**
         * Accessor that sets the begin time of the event
         *
         * @since 1.0
         * @param aTime - An integer that is the begin time
         * @return None
         */
        virtual void SetBeginTime( TInt32 aTime ) = 0;

    };


//*******************************************************

class TSvgUiMouseEvent : public MSvgUiMouseEvent
    {
    public:

        /**
         * Constructor
         *
         * @since 1.0
         * @param aEtype - A TSvgEngineEventType representing the event type
         * @param aX - x coordinate of the event
         * @param aY - y coordinate of the event
         * @return
         */
        TSvgUiMouseEvent( TSvgEngineEventType aEtype, TFloatFixPt aX, TFloatFixPt aY );

        /**
         * Accessor that returns the event mask
         *
         * @since 1.0
         * @param None
         * @return An integer that is the event mask
         */
        TUint8              EventMask();

        /**
         * Accessor for the event type
         *
         * @since 1.0
         * @param None
         * @return A TSvgEngineEventType that is the event type
         */
        TSvgEngineEventType EventType();

        /**
         * Accessor for the x coordinate of the mouse event
         *
         * @since 1.0
         * @param None
         * @return A fixed point that is the x coordinate of he event
         */
        TFloatFixPt              X();

        /**
         * Accessor for the y coordinate of the mouse event
         *
         * @since 1.0
         * @param None
         * @return A fixed point that is the y coordinate of the event
         */
        TFloatFixPt              Y();

    public:
        TSvgEngineEventType iEtype;
        TFloatFixPt              iX;
        TFloatFixPt              iY;
    };

class TSvgUiKeyEvent : public MSvgUiKeyEvent
    {
    public:

        /**
         * Accessor that returns the key code of the event
         *
         * @since 1.0
         * @param
         * @return
         */
        TSvgUiKeyEvent( TUint32 aKeyCode );

        /**
         * Accessor that returns the event mask
         *
         * @since 1.0
         * @param None
         * @return An integer that is the event mask
         */
        TUint8 EventMask();

        /**
         * Accessor for the event type
         *
         * @since 1.0
         * @param None
         * @return A TSvgEngineEventType that is the event type
         */
        TSvgEngineEventType EventType();

        /**
         * Accessor that returns the key code of the event
         *
         * @since 1.0
         * @param
         * @return
         */
        TUint32 KeyCode();

        /**
         * Accessor that returns the time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the time of the event
         */
        TInt32 Time();

        /**
         * Accessor that sets the time of the event
         *
         * @since 1.0
         * @param aTime - An integer that is the time of the event
         * @return None
         */
        void SetTime( TInt32 aTime );

        /**
         * Accessor that returns the begin time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the begin time of the event
         */
        TInt32 BeginTime();

        /**
         * Accessor that sets the begin time of the event
         *
         * @since 1.0
         * @param aTime - An integer that is the begin time
         * @return None
         */
        void SetBeginTime( TInt32 aTime );

    private:
        TUint32 iKeyCode;
        TInt32  iTime;
        TInt32  iBeginTime;
    };


//
class TSvgTimerEventPrep : public MSvgTimerEvent
    {
    public:

        /**
         * Constructor
         *
         * @since 1.0
         * @param aTime - An integer that is the event time
         * @return None
         */
        TSvgTimerEventPrep( TUint32 aTime );

        /**
         * Accessor that returns the event mask
         *
         * @since 1.0
         * @param None
         * @return An integer that is the event mask
         */
        TUint8 EventMask();

        /**
         * Accessor for the event type
         *
         * @since 1.0
         * @param None
         * @return A TSvgEngineEventType that is the event type
         */
        TSvgEngineEventType EventType();

        /**
         * Accessor that returns the time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the time of the event
         */
        TUint32 Time();

    private:
        TInt32  iTime;
    };

class TSvgTimerEvent : public MSvgTimerEvent
    {
    public:

        /**
         * Constructor
         *
         * @since 1.0
         * @param aTime - An integer that is the time of the event
         * @return None
         */
        TSvgTimerEvent( TUint32 aTime );

        /**
         * Accessor that returns the event mask
         *
         * @since 1.0
         * @param None
         * @return An integer that is the event mask
         */
        TUint8 EventMask();

        /**
         * Accessor for the event type
         *
         * @since 1.0
         * @param None
         * @return A TSvgEngineEventType that is the event type
         */
        TSvgEngineEventType EventType();

        /**
         * Accessor that returns the time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the time of the event
         */
        TUint32 Time();

        /**
         * Accessor that sets time of the event
         *
         * @since 1.0
         * @param TInt32 An integer that is the begin time of the event
         * @return none
         */
        void SetTime(TInt32 aTime);


        /**
         * Accessor that returns the begin time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the begin time of the event
         */
        TInt32 BeginTime();

        /**
         * Accessor that sets the begin time of the event
         *
         * @since 1.0
         * @param aTime - An integer that is the begin time
         * @return None
         */
        void SetBeginTime( TInt32 aTime );

    private:
        TInt32  iTime;
        TInt32  iBeginTime;
    };

class TSvgInternalEvent : public MSvgInternalEvent
    {
    public:

        /**
         * Constructor
         *
         * @since 1.0
         * @param aEvent - A TSvgEvent type
         * @param aElement - A pointer to the target element
         * @return None
         */
        TSvgInternalEvent( TSvgEvent aEvent, CSvgElementImpl* aElement );

		TSvgInternalEvent( TSvgEvent aEvent, CSvgElementImpl* aElement, TInt32 aTime );

		TBool UserSeek();
        /**
         * Accessor that returns the event mask
         *
         * @since 1.0
         * @param None
         * @return An integer that is the event mask
         */
        TUint8 EventMask();

        /**
         * Accessor for the event type
         *
         * @since 1.0
         * @param None
         * @return A TSvgEngineEventType that is the event type
         */
        TSvgEngineEventType EventType();

        /**
         * Return a pointer the element that is the target for the event
         *
         * @since 1.0
         * @param None
         * @return A pointer to the target element
         */
        CSvgElementImpl* ObjectAddress();

        /**
         * Return the event description
         *
         * @since 1.0
         * @param None
         * @return A TSvgEvent type
         */
        TSvgEvent SvgEvent();

        /**
         * Accessor that returns the time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the time of the event
         */
        TInt32 Time();

        /**
         * Accessor that sets the time of the event
         *
         * @since 1.0
         * @param aTime - An integer that is the time of the event
         * @return
         */
        void SetTime( TInt32 aTime );

        /**
         * Accessor that returns the begin time of the event
         *
         * @since 1.0
         * @param None
         * @return An integer that is the begin time of the event
         */
        TInt32 BeginTime();

        /**
         * Accessor that sets the begin time of the event
         *
         * @since 1.0
         * @param aTime - An integer that is the begin time
         * @return None
         */
        void SetBeginTime( TInt32 aTime );

    private:
        TSvgEvent       iEvent;
        TBool			 iRepeatOnBegin;
        CSvgElementImpl*iElement;
        TInt32          iTime;
        TInt32          iBeginTime;
		TBool           IsUserSeek;
    };

class TSvgScreenUpdateEvent : public MSvgEvent
    {
    public:
        /**
         * Accessor that returns the event mask
         *
         * @since 1.0
         * @param None
         * @return An integer that is the event mask
         */
        TUint8 EventMask();

        /**
         * Accessor for the event type
         *
         * @since 1.0
         * @param None
         * @return A TSvgEngineEventType that is the event type
         */
        TSvgEngineEventType EventType();
    };


#endif /* __INC_SvgEvent_H__ */
