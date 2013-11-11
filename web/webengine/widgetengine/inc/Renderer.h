/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This class represents the Widget Renderer object
*
*/

#ifndef __WIDGETENGINERENDERER
#define __WIDGETENGINERENDERER

//  INCLUDES
#include <e32base.h>
#include "WidgetEngineCallbacks.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATION

// CLASS DECLARATION

/**
*  WidgetRenderer
*
*  @lib widgetengine.dll
*  @since 3.1
*/
class WidgetRenderer
{

public:
    WidgetRenderer(MWidgetEngineCallbacks& aWidgetEngineCallback);
    ~WidgetRenderer();

public:
    void prepareForTransitionL(const TDesC& aTransition);
    void performTransitionL();

public:
    void transitionCb();
    TBool transitionInProgress() { return m_transitionbitmap!=0; }
    void preparetodrawTransition();    
    void drawTransition(CWindowGc& gc, CFbsBitmap* aCurrentBitmap);
    
protected:
    void createTransitionBitmapL();
    void destroyTransitionBitmap();
    void createFadeMaskL();
    void destroyFadeMask();
    void drawFadeMaskL();

private:      
    CFbsBitmap*     m_transitionbitmap;
    CFbsBitmap*     m_fademask;
    CPeriodic*      m_transitiontimer; 
    TUint8          m_transitioncount;
    TUint8          m_maxtransitions;
    MWidgetEngineCallbacks* m_widgetenginecallback;

};

#endif
