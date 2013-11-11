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
* Description:   Handles the viewing of a single frame. If the page is not frame
*                enabled, this class is used as the single view.  If frame
*                enabled, there is one instance of this class for each frame.
*
*/


#ifndef __WEBTOOLBARINTERFACE
#define __WEBTOOLBARINTERFACE

//  INCLUDES
#include <e32base.h>
#include <bitstd.h>
#include <w32std.h>
#include <coedef.h>
#include <AknsConstants.h>
#include <AknsItemID.h>
#include <AknsConstants.h>
#include "BrCtlDefs.h"
#include "ToolBar.h"
/*
#include "Favicon.h"
#include "WebCoreIconDatabaseBridge.h"
*/
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CGulIcon;
class TBrCtlSubscribeTo;
class WebView;

class WebToolBarInterface: public MToolBarCallback
{

public: 
    WebToolBarInterface(WebView* webView);
    virtual ~WebToolBarInterface();

    TPoint CursorPosition();
    void SetAndUpdateCursorPosition(const TPoint& aPoint);
    const RPointerArray<TBrCtlSubscribeTo>& SubscribeToMenuItemsL();
    CCoeControl& View();
    CCoeControl& CCoeControlParent();
    void CloseToolBarL();    
    CGulIcon* GetFaviconL(const TDesC& aUrl);
    void SendCommandsToClient(TBrCtlDefs::TBrCtlClientCommands aCommand,
                                      const CArrayFix<TPtrC>& aAttributesNames,
                                      const CArrayFix<TPtrC>& aAttributeValues);    
    void LoadUrlL( const TDesC& aUrl, TBrCtlDefs::TBrCtlCacheMode aCacheMode );    
    HBufC* ResolveUrlL( const TDesC& aUrl);
    void OpenPageViewL();
    TUint GetBrowserSettingL(TUint aSetting);
    void HandleHistoryCommandL(TBrCtlDefs::TBrCtlCommands aCommand);
    void ViewImagesL();
    TBool wmlMode();
    
private:
    WebView* m_webView;
};

#endif
