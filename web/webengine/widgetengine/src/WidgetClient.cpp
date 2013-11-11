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

// INCLUDE FILES
#include <e32std.h>
#include <eikenv.h>
#include <apgcli.h>
#include <apgtask.h>
#include <BrowserLauncher.h>
#include <widgetmenu.rsg>
#include <avkon.hrh>

#include "WidgetClient.h"
#include "Widget.h"
#include "Renderer.h"
#include "Preferences.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

const TInt KWebBrowserUid = 0x10008D39;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES


// ----------------------------------------------------------------------------
// CWidgetClient::NewL
//
//
//
// ----------------------------------------------------------------------------
//
CWidgetClient* CWidgetClient::NewL(	MWidgetCallback& aWidgetCallback,
									MJSObjectProtector* protector, 
									MWidgetEngineCallbacks& aWidgetEngineCallback, 
									WidgetPreferences* preferences
								  )
{
    CWidgetClient* self = new ( ELeave ) CWidgetClient(aWidgetCallback, aWidgetEngineCallback, preferences);
    CleanupStack::PushL( self );
    self->ConstructL(aWidgetEngineCallback, protector);
    CleanupStack::Pop();
    return self;
}

// ----------------------------------------------------------------------------
// CWidgetClient::CWidgetClient
//
//
//
// ----------------------------------------------------------------------------
//
CWidgetClient::CWidgetClient(MWidgetCallback& aWidgetCallback, 
                             MWidgetEngineCallbacks& aWidgetEngineCallback, 
                             WidgetPreferences* preferences) : 
                                                            m_preferences (preferences), 
                                                            m_renderer(0), 
                                                            m_widgetcallback( &aWidgetCallback ), 
                                                            m_widgetenginecallback(&aWidgetEngineCallback),
                                                            m_jswidget(0)
{
}

// ----------------------------------------------------------------------------
// CWidgetClient::~CWidgetClient
//
//
//
// ----------------------------------------------------------------------------
//
CWidgetClient::~CWidgetClient()
{    
    delete m_renderer;
}


// ----------------------------------------------------------------------------
// CWidgetClient::ConstructL
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::ConstructL(MWidgetEngineCallbacks& aWidgetEngineCallback, MJSObjectProtector* protector)
{    
    m_jswidget = new KJS::JSWidget(this, protector);     
    m_renderer = new (ELeave) WidgetRenderer(aWidgetEngineCallback);    
}


// ----------------------------------------------------------------------------
// CWidgetClient::OnShowSelected
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::onShowSelected()
{
    if (m_jswidget) {
        m_jswidget->setVisibility(true);        
    }
}


// ----------------------------------------------------------------------------
// CWidgetClient::OnHideSelected
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::onHideSelected()
{
    if (m_jswidget) {
        m_jswidget->setVisibility(false);        
    }    
}

// ----------------------------------------------------------------------------
// CWidgetClient::OnExitCalled
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::onExitCalled()
{
    if (m_jswidget) {
        m_jswidget->handleExit();        
    }    
}

////////////////////////////////////////////////////////////////////////////////
// From MWidgetExtension
// -----------------------------------------------------------------------------
// CWidgetClient::HandleCommandL
//
//
//
// -----------------------------------------------------------------------------
//
TBool CWidgetClient::HandleCommandL( TInt aCommandId )
{
    TInt cmnd(aCommandId - (TInt)TBrCtlDefs::ECommandIdBase);
    
    if ( cmnd == TBrCtlDefs::ECommandAppForeground ) {
        onShowSelected();
        return ETrue;
    }
    if ( cmnd == TBrCtlDefs::ECommandAppBackground ) {
        onHideSelected();
        return ETrue;
    }
    if ((aCommandId == EEikCmdExit) || (aCommandId == EAknCmdExit) || (aCommandId == EAknSoftkeyExit)) {
        onExitCalled();
    }

    return EFalse;
}

// From MWidgetCallbacks
// ----------------------------------------------------------------------------
// CWidgetClient::launchApplicationL
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::launchApplicationL(const TUid& aUid, const TDesC& aParam)
{
    RApaLsSession apaLsSession;
    TApaTaskList taskList(CEikonEnv::Static()->WsSession());
    TApaTask task = taskList.FindApp(aUid);

    if ( task.Exists() ) {
    
        task.BringToForeground();
        if ( aParam.Length() > 0 ) {
            HBufC8* param8 = HBufC8::NewLC( aParam.Length() );
            param8->Des().Append( aParam );
            task.SendMessage( TUid::Uid( 0 ), *param8 );
            CleanupStack::PopAndDestroy( param8 );
        }
    }
    else {
    
        TInt eConnect = KErrNone;
        if ( !apaLsSession.Handle() ) {
            eConnect = apaLsSession.Connect();
        }

        if ( eConnect == KErrNone ) {
            TThreadId threadId;
            apaLsSession.StartDocument( aParam, aUid, threadId );
            apaLsSession.Close();
        }
    }

}

// ----------------------------------------------------------------------------
// CWidgetClient::openApplication
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::openApplication(const TUid& aAppUid, const TDesC& aParam)
{
    TRAP_IGNORE( launchApplicationL( aAppUid, aParam ) );
}

// ----------------------------------------------------------------------------
// CWidgetClient::openUrl
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::openUrl( const TDesC& aUrl )
{    
    HBufC* param = HBufC::NewLC( aUrl.Length() + 2 );
    param->Des().Append( _L("4 ") );
    param->Des().Append( aUrl );
    TRAP_IGNORE( launchApplicationL( TUid::Uid( KWebBrowserUid ), *param ) );
    CleanupStack::PopAndDestroy( param );
}

// ----------------------------------------------------------------------------
// CWidgetClient::getWidgetBundleId
//
//
//
// ----------------------------------------------------------------------------
//
TDesC& CWidgetClient::getWidgetBundleId()
{            
    return m_preferences->getWidgetBundleId();    
}


// ----------------------------------------------------------------------------
// CWidgetClient::setNavigationEnabled
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::setNavigationEnabled( TBool aEnable )
{
    m_widgetenginecallback->setTabbednavigation(!aEnable);
}

// ----------------------------------------------------------------------------
// CWidgetClient::setNavigationType
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::setNavigationType( const TDesC& aType )
{
    m_widgetenginecallback->setNavigationType(aType);
}

// ----------------------------------------------------------------------------
// CWidgetClient::prepareForTransition
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::prepareForTransition( const TDesC& aTransition )
{    
    TRAP_IGNORE( m_renderer->prepareForTransitionL( aTransition ) );
}

// ----------------------------------------------------------------------------
// CWidgetClient::performTransition
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::performTransition()
{    
    TRAP_IGNORE( m_renderer->performTransitionL() );
}

// ----------------------------------------------------------------------------
// CWidgetClient::preferenceForKey
//
//
//
// ----------------------------------------------------------------------------
//
TInt CWidgetClient::preferenceForKey( const TDesC& aKey, HBufC*& aValue )
{    
    TInt ret = KErrNotFound;
    TRAP_IGNORE( ret = m_preferences->preferenceL( aKey, aValue ) );
    return ret;
}

// ----------------------------------------------------------------------------
// CWidgetClient::setPreferenceForKey
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::setPreferenceForKey( const TDesC& aKey, const TDesC& aValue )
{    
    TRAP_IGNORE( m_preferences->setPreferenceL( aKey, aValue ) );
}

// ----------------------------------------------------------------------------
// CWidgetClient::removePreferenceForKey
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::removePreferenceForKey( const TDesC& aKey, const TDesC& aValue )
{    
    TRAP_IGNORE( m_preferences->removePreferenceL( aKey, aValue ) );
}

// ----------------------------------------------------------------------------
// CWidgetClient::setDisplayLandscape
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::setDisplayLandscape()
{    
    m_widgetcallback->SetDisplayMode(TBrCtlDefs::EOrientationLandscape);
}

// ----------------------------------------------------------------------------
// CWidgetClient::setDisplayPortrait
//
//
//
// ----------------------------------------------------------------------------
//
void CWidgetClient::setDisplayPortrait()
{   
    m_widgetcallback->SetDisplayMode(TBrCtlDefs::EOrientationPortrait);
}


//END OF FILE




