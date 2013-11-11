/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handle communications to Widget Registry.
*
*/


// INCLUDE FILES

#include "wrtharvesterregistryaccess.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
WrtHarvesterRegistryAccess::WrtHarvesterRegistryAccess()
	{
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
WrtHarvesterRegistryAccess::~WrtHarvesterRegistryAccess()
    {
    }

// ---------------------------------------------------------------------------
// Collect bundle names of widgets supporting miniviews.
// ---------------------------------------------------------------------------
//
void WrtHarvesterRegistryAccess::WidgetInfosL( 
    RWrtArray< CWrtInfo >& aWidgetInfoArray )
    {
    RWrtArray< CWidgetInfo > widgetInfoArr;
    widgetInfoArr.PushL();
    
    RWidgetRegistryClientSession session;
    CleanupClosePushL( session );
    User::LeaveIfError( session.Connect() );

    // Reset previously appended widget infos
    aWidgetInfoArray.ResetAll();

    TInt err = session.InstalledWidgetsL( widgetInfoArr );
    
    for( TInt i( widgetInfoArr.Count() - 1 ); i >= 0; --i )
        {
        CWidgetInfo* widgetInfo( widgetInfoArr[i] );
        
        if ( SupportsMiniviewL( session, widgetInfo->iUid ) )
            {
            CWrtInfo* info = new CWrtInfo();
            info->iUid = widgetInfo->iUid;
            info->iBundleId = WidgetPropertyL( session, widgetInfo->iUid, EBundleIdentifier );
            info->iDisplayName = WidgetPropertyL( session, widgetInfo->iUid, EBundleDisplayName );
            aWidgetInfoArray.AppendL( info );
            }
        }
    CleanupStack::PopAndDestroy( &session );
    CleanupStack::PopAndDestroy( &widgetInfoArr );
    }

// ---------------------------------------------------------------------------
// Check if widget supports miniview.
// ---------------------------------------------------------------------------
//
TBool WrtHarvesterRegistryAccess::SupportsMiniviewL( 
    RWidgetRegistryClientSession& aSession,
    const TUid& aUid )
    {
    TBool res( EFalse );
    
    CWidgetPropertyValue* value( NULL );
    value = aSession.GetWidgetPropertyValueL( aUid, EMiniViewEnable );
    
    res = value && *value;
    
    delete value;
    return res;
    }
    
// ---------------------------------------------------------------------------
// Get the widget property as string.
// ---------------------------------------------------------------------------
//
HBufC* WrtHarvesterRegistryAccess::WidgetPropertyL( 
    RWidgetRegistryClientSession& aSession,
    const TUid& aUid, TWidgetPropertyId aPropertyId )
    {   
    CWidgetPropertyValue* value( NULL );
    value = aSession.GetWidgetPropertyValueL( aUid, aPropertyId );
    CleanupStack::PushL( value );
    
    const TDesC& str = *value;
    HBufC* strBuf = str.AllocL();
    CleanupStack::PopAndDestroy( value );
    return strBuf;
    }
    
 //  End of File

