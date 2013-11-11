/**
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
* Description: CUpnpAppFilterInfo class implementation
*
*/


// INCLUDE FILES
#include "upnpappfilterinfo.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnpappfilterinfoTraces.h"
#endif

//Literals
_LIT8(KAppName,             "name");
_LIT8(KAppDesc,             "description");
_LIT8(KMimeType,            "mimetype");
_LIT8(KWidth,               "width");
_LIT8(KHeight,              "height");
_LIT8(KDepth,               "depth");
_LIT8(KAllowedProfileIds,   "allowedProfileIDs");
_LIT8(KRscStatus,           "resourceStatus");
_LIT8(KSignature,           "signature");


// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpAppFilterInfo::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpAppFilterInfo* CUpnpAppFilterInfo::NewL( )
    {
    OstTraceFunctionEntry0( CUPNPAPPFILTERINFO_NEWL_ENTRY );
    return ( new (ELeave) CUpnpAppFilterInfo() );   
    }

// ---------------------------------------------------------------------------------
// CUpnpAppFilterInfo::CUpnpAppFilterInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpAppFilterInfo::CUpnpAppFilterInfo( )
    {
    iAppName = NULL;
    iAppDesc = NULL;
    iMimeType = NULL;
    iWidth = NULL;
    iHeight = NULL;
    iDepth = NULL;
    iAllowedIdList = NULL;
    iProtocolId = NULL;
    iFormat = NULL;
    iDirection = NULL;
    iAppCategory = NULL; 
    iAppTrustLevel = NULL;
    iContCategory = NULL;
    iContentRules = NULL;
    iContTrustLevel = NULL;
    iAudioType = NULL;
    iRscStatus = NULL;
    iSignature = NULL;
    }

// ---------------------------------------------------------------------------------
// CUpnpAppFilterInfo::~CUpnpAppFilterInfo
// C++ default destructor 
// ---------------------------------------------------------------------------------
//
CUpnpAppFilterInfo::~CUpnpAppFilterInfo()
    {
    OstTraceFunctionEntry0( CUPNPAPPFILTERINFO_CUPNPAPPFILTERINFO_ENTRY );
    delete  iAppName;
    delete  iAppDesc;
    delete  iMimeType;
    delete  iWidth;
    delete  iHeight;
    delete  iDepth;
    delete  iAllowedIdList;
    delete  iProtocolId;
    delete  iFormat;
    delete  iDirection;
    delete  iAppCategory;
    delete  iAppTrustLevel;
    delete  iContCategory;
    delete  iContentRules;
    delete  iContTrustLevel;
    delete  iAudioType;
    delete  iRscStatus;
    delete  iSignature;
    OstTraceFunctionExit0( CUPNPAPPFILTERINFO_CUPNPAPPFILTERINFO_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpAppFilterInfo::SetFilterInfoL
// It compares the key with the individual elements and if it matches with none then it
// returns invalid argument as an error.
// @param aKey Key name
// @param aValue Value of the key
// @param aDisplayInfoFlag True if info type is displayInfo, false otherwise.
//         Default is false. It is used to distinguish "trustLevel" element 
//         of appInfo and displayInfo.
// @param aErr Terminal Mode error code
// ---------------------------------------------------------------------------------
//
void CUpnpAppFilterInfo::SetFilterInfoL( const TDesC8& aKey, const TDesC8& aValue,
                                        TTerminalModeErrorCode& aErr, TBool aDisplayInfoFlag )
    {
    OstTraceFunctionEntry0( CUPNPAPPFILTERINFO_SETFILTERINFOL_ENTRY );
    aErr = ETerminalModeInvalidArgument;
    if ( aKey.Compare(KAppName) == KErrNone )
        {
        delete iAppName;
        iAppName = NULL;
        iAppName = aValue.AllocL();
        aErr = ETerminalModeSuccess;  
        }
    else if ( aKey.Compare(KAppDesc) == KErrNone )
        {
        delete iAppDesc;
        iAppDesc = NULL;
        iAppDesc = aValue.AllocL();
        aErr = ETerminalModeSuccess; 
        }
    else if ( aKey.Compare(KMimeType) == KErrNone )
        {
        delete iMimeType;
        iMimeType = NULL;
        iMimeType = aValue.AllocL();
        aErr = ETerminalModeSuccess;  
        }
    else if ( aKey.Compare(KWidth) == KErrNone )
        {
        delete iWidth;
        iWidth = NULL;
        iWidth = aValue.AllocL();
        aErr = ETerminalModeSuccess;  
        }
    else if ( aKey.Compare(KHeight) == KErrNone )
        {
        delete iHeight;
        iHeight = NULL;
        iHeight = aValue.AllocL();
        aErr = ETerminalModeSuccess;   
        }
    else if ( aKey.Compare(KDepth) == KErrNone )
        {
        delete iDepth;
        iDepth = NULL;
        iDepth = aValue.AllocL();
        aErr = ETerminalModeSuccess;   
        }
    else if ( aKey.Compare(KAllowedProfileIds) == KErrNone )                                     
        {
        delete iAllowedIdList;
        iAllowedIdList = NULL;
        iAllowedIdList = aValue.AllocL();
        aErr = ETerminalModeSuccess;  
        }
    else if ( aKey.Compare(KProtocolId) == KErrNone )                                     
        {
        delete iProtocolId;
        iProtocolId = NULL;
        iProtocolId = aValue.AllocL();
        aErr = ETerminalModeSuccess;  
        }
    else if ( aKey.Compare(KFormat) == KErrNone )                                     
        {
        delete iFormat;
        iFormat = NULL;
        iFormat = aValue.AllocL();
        aErr = ETerminalModeSuccess;  
        }
    else if ( aKey.Compare(KRemotingDirection) == KErrNone )                                     
        {
        delete iDirection;
        iDirection = NULL;
        iDirection = aValue.AllocL();
        aErr = ETerminalModeSuccess;  
        }
    else if ( aKey.Compare(KAppCategory) == KErrNone )
        {
        delete iAppCategory;
        iAppCategory = NULL;
        iAppCategory = aValue.AllocL();
        aErr = ETerminalModeSuccess;  
        }
    else if (( aKey.Compare(KTrustLevel) == KErrNone ) && ( !aDisplayInfoFlag ))                                        
        {
        delete iAppTrustLevel;
        iAppTrustLevel = NULL;
        iAppTrustLevel = aValue.AllocL();
        aErr = ETerminalModeSuccess;  
        }
    else if ( aKey.Compare(KContCategory) == KErrNone )
        {
        delete iContCategory;
        iContCategory = NULL;
        iContCategory = aValue.AllocL();
        aErr = ETerminalModeSuccess;   
        }
    else if ( aKey.Compare(KContentRules) == KErrNone )
        {
        delete iContentRules;
        iContentRules = NULL;
        iContentRules = aValue.AllocL();
        aErr = ETerminalModeSuccess;   
        }
    else if (( aKey.Compare(KTrustLevel) == KErrNone ) && ( aDisplayInfoFlag ))
        {
        delete iContTrustLevel;
        iContTrustLevel = NULL;
        iContTrustLevel = aValue.AllocL();
        aErr = ETerminalModeSuccess;   
        }
    else if ( aKey.Compare(KAudioType) == KErrNone )
        {
        delete iAudioType;
        iAudioType = NULL;
        iAudioType = aValue.AllocL();
        aErr = ETerminalModeSuccess;   
        }
    else if ( aKey.Compare(KRscStatus) == KErrNone )
        {
        delete iRscStatus;
        iRscStatus = NULL;
        iRscStatus = aValue.AllocL();
        aErr = ETerminalModeSuccess;   
        }
    else if ( aKey.Compare(KSignature) == KErrNone )                                        
        {
        delete iSignature;
        iSignature = NULL;
        iSignature = aValue.AllocL();
        aErr = ETerminalModeSuccess;  
        }
    OstTraceFunctionExit0( CUPNPAPPFILTERINFO_SETFILTERINFOL_EXIT );
    }
    
// End of File
