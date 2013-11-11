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
* Description:  This class represents the Widget Extension object
*
*/

#ifndef __WIDGETCLIENT_
#define __WIDGETCLIENT_

//  INCLUDES
#include <e32base.h>
#include "brctlinterface.h"
#include "WidgetCallbacks.h"
#include "WidgetEngineCallbacks.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATION
class WidgetRenderer;
class WidgetPreferences;
class MJSObjectProtector;

namespace KJS {
    class JSWidget;
}

// CLASS DECLARATION
/**
*  CWidgetClient
*
*  @lib browserengine.dll
*  @since 3.2
*/
class CWidgetClient: public CBase, public MJSWidgetCallbacks
{

public:
    static CWidgetClient* NewL(MWidgetCallback& aWidgetCallback, MJSObjectProtector* aProtector, MWidgetEngineCallbacks& aWidgetEngineCallback, WidgetPreferences* preferences);
    virtual ~CWidgetClient();

public:
    KJS::JSWidget* jswidget() { return m_jswidget; }
    void onShowSelected();
    void onHideSelected();
    void onExitCalled();
    WidgetRenderer* renderer() { return m_renderer; }
        
public: // From MWidgetExtension
    TBool HandleCommandL( TInt aCommandId );        

public: // From MJSWidgetCallbacks
    void openApplication( const TUid& aAppUid, const TDesC& aParam );
    void openUrl( const TDesC& aUrl );
    TDesC& getWidgetBundleId();
    void setNavigationEnabled( TBool aEnable );
    void setNavigationType( const TDesC& aType );
    void prepareForTransition( const TDesC& aTransition );
    void performTransition();        
    TInt preferenceForKey( const TDesC& aKey, HBufC*& aValue );
    void setPreferenceForKey( const TDesC& aKey, const TDesC& aValue );
    void removePreferenceForKey( const TDesC& aKey, const TDesC& aValue );
    void setDisplayLandscape();
    void setDisplayPortrait();
    
private:
    void launchApplicationL( const TUid& aUid, const TDesC& aParam );

protected:
    void ConstructL(MWidgetEngineCallbacks& aWidgetEngineCallback, MJSObjectProtector* protector);
    CWidgetClient(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback, WidgetPreferences* preferences);

private:
    WidgetPreferences*     m_preferences; //not owned
    WidgetRenderer*        m_renderer;

    MWidgetCallback*       m_widgetcallback;
    MWidgetEngineCallbacks* m_widgetenginecallback;
    KJS::JSWidget*         m_jswidget;

};


#endif

