/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/



#ifndef __WIDGETCALLBACKS_H
#define __WIDGETCALLBACKS_H

//  INCLUDES
#include <e32base.h>

class MJSWidgetCallbacks
{
public:
    virtual void openApplication( const TUid& aAppUid, const TDesC& aParam ) = 0;
    virtual void openUrl( const TDesC& aUrl ) = 0;
    virtual TDesC& getWidgetBundleId() = 0;    
    virtual TInt preferenceForKey( const TDesC& aKey, HBufC*& aValue ) = 0;
    virtual void prepareForTransition( const TDesC& aTransition ) = 0;
    virtual void performTransition() = 0;
    virtual void setPreferenceForKey( const TDesC& aKey, const TDesC& aValue ) = 0;
    virtual void removePreferenceForKey( const TDesC& aKey, const TDesC& aValue ) = 0;
    virtual void setNavigationEnabled( TBool aEnable ) = 0;
    virtual void setNavigationType( const TDesC& aType ) = 0;
    virtual void setDisplayLandscape() = 0;
    virtual void setDisplayPortrait() = 0;
};

#endif
