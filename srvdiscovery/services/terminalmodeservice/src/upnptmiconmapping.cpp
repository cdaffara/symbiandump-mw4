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
* Description: CUpnpTmIconMapping class implementation.
*
*/

//Include Files  

#include "upnptmiconmapping.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptmiconmappingTraces.h"
#endif
   

_LIT8(KSvgMimeType,     "image/svg+xml");  

// ================= MEMBER FUNCTIONS ==============================================

// ---------------------------------------------------------------------------------
// CUpnpTmIconMapping::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpTmIconMapping* CUpnpTmIconMapping::NewL( )
    {
    OstTraceFunctionEntry0( CUPNPTMICONMAPPING_NEWL_ENTRY );
    CUpnpTmIconMapping* self = new (ELeave) CUpnpTmIconMapping(); 
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CUPNPTMICONMAPPING_NEWL_EXIT );
    return self; 
    }


// ---------------------------------------------------------------------------------
// CUpnpTmIconMapping::CUpnpTmIconMapping
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpTmIconMapping::CUpnpTmIconMapping( )
     {

     }

// ---------------------------------------------------------------------------------
// CUpnpTmIconMapping::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------------
//
void CUpnpTmIconMapping::ConstructL()
    {
    OstTraceFunctionEntry0( CUPNPTMICONMAPPING_CONSTRUCTL_ENTRY );
    iFileUrlArray = new ( ELeave ) CDesC8ArrayFlat( KExpectedMaxIconParms  );
    iFilePathArray = new ( ELeave ) CDesCArrayFlat( KExpectedMaxIconParms );
    OstTraceFunctionExit0( CUPNPTMICONMAPPING_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmIconMapping::~CUpnpTmIconMapping
// Destructor
// ---------------------------------------------------------------------------------
//
CUpnpTmIconMapping::~CUpnpTmIconMapping()
    {
    OstTraceFunctionEntry0( CUPNPTMICONMAPPING_CUPNPTMICONMAPPING_ENTRY );
    if ( iFileUrlArray )
        {
        iFileUrlArray->Reset();
        delete iFileUrlArray;
        }
    if ( iFilePathArray )
        {
        iFilePathArray->Reset();
        delete iFilePathArray;
        }   
    iWidthArray.Close();
    iHeightArray.Close();
    OstTraceFunctionExit0( CUPNPTMICONMAPPING_CUPNPTMICONMAPPING_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmIconMapping::AddIconFileInfoL
// Method is used to store information about each application icon which can be 
// used to fetch iconPath for the corresponding iconUrl and to determine whether  
// conversion is needed or not.
// @param aUrl     Iccon URL.
// @param aPath    Icon Path.
// @param aWidth   Width of the icon.
// @param aHeight  Height of the icon.
// -------------------------------------------------------------------------------------
//
void CUpnpTmIconMapping::AddIconFileInfoL( const TDesC8& aUrl, const TDesC& aPath, 
                                                        TInt aWidth, TInt aHeight )
    {
    OstTraceFunctionEntry0( CUPNPTMICONMAPPING_ADDICONFILEINFOL_ENTRY );
    iFileUrlArray->AppendL( aUrl );
    iFilePathArray->AppendL( aPath );
    iWidthArray.AppendL( aWidth );
    iHeightArray.AppendL( aHeight );
    OstTraceFunctionExit0( CUPNPTMICONMAPPING_ADDICONFILEINFOL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmIconMapping::GetSvgIconPath
// Method is used to fetch the equivalent icon path for the requested icon url
// @param aUrl  iconUrl
// @param aSvgFilePath[out]  iconPath
// @return   Returns the position of the filepath in the array if found else 
//           returns KErrNotFound.
// ---------------------------------------------------------------------------------
//
TInt CUpnpTmIconMapping::GetSvgIconPath( const TDesC8& aUrl,RBuf& aSvgFilePath )
    {
    OstTraceFunctionEntry0( CUPNPTMICONMAPPING_GETSVGICONPATH_ENTRY );
    TInt pos;
    if ( iFileUrlArray->Find(aUrl,pos) == KErrNone )
        {
        aSvgFilePath.Close();
        aSvgFilePath.Create((*iFilePathArray)[pos]);
        }
    else
        {
        // The requested file is not available
        pos = KErrNotFound;
        }
    OstTraceFunctionExit0( CUPNPTMICONMAPPING_GETSVGICONPATH_EXIT );
    return pos;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmIconMapping::IsConversionNeeded
// Method is used to fetch the dimensions of the icon which needs to be served and 
// also return the information whether conversion is needed for a particular icon.
// @param aIndex   Index of the icon filepath
// @param aClientIconPref   Reference to Client Icon Preference object.
// @param[out] aIconWidth   Width of the icon which needs to be sent.
// @param[out] aIconHeight  Height of the icon which needs to be sent.
// @return  Returns the boolean value for conversion is needed(true) or not(false).
// ---------------------------------------------------------------------------------
//
TBool CUpnpTmIconMapping::IsConversionNeeded( TInt aIndex, const CUpnpTmClientIconPref& aClientIconPref,
                                                                   TInt& aIconWidth, TInt& aIconHeight )
    {
    OstTraceFunctionEntry0( CUPNPTMICONMAPPING_ISCONVERSIONNEEDED_ENTRY );
    ASSERT( ( aIndex >= 0 ) && ( aIndex < iWidthArray.Count()) );
    TBool isConversionNeeded(ETrue);
    // Fetch the client's icon preferences
    const TDesC8& mimeType = aClientIconPref.MimeType();
    TInt iconWidth  = aClientIconPref.Width();
    TInt iconHeight = aClientIconPref.Height();
    OstTraceExt2( TRACE_NORMAL, DUP1_CUPNPTMICONMAPPING_ISCONVERSIONNEEDED, "CUpnpTmIconMapping::IsConversionNeeded;iconWidth=%d;iconHeight=%d", iconWidth, iconHeight );

    if ( mimeType.Compare(KSvgMimeType) == KErrNone )
        {
        // Client supports "image/svg+xml" mime type icons
        if ((( iconWidth == KErrNone ) || ( iconWidth == iWidthArray[aIndex] )) && 
            (( iconHeight == KErrNone ) || ( iconHeight == iHeightArray[aIndex] )))
            {
            /* Either the client has not provided its dimension preferences or 
               preferred width and height also matches. So no conversion needed */
            isConversionNeeded = EFalse;
            }
        }
    /* 
     * If the width and height fields are set properly then only use the client's 
     * specified icon dimensions else use the icon's original dimensions.
     */
    if ( isConversionNeeded && iconWidth && iconHeight )
        {
        // Use the dimensions specified by the client
        aIconWidth  = iconWidth;
        aIconHeight = iconHeight;  
        }
    else
        {
        // Use original dimensions
        aIconWidth  = iWidthArray[aIndex];
        aIconHeight = iHeightArray[aIndex]; 
        } 
    OstTraceExt2( TRACE_NORMAL, CUPNPTMICONMAPPING_ISCONVERSIONNEEDED, "CUpnpTmIconMapping::IsConversionNeeded;aIconWidth=%d;aIconHeight=%d", aIconWidth, aIconHeight );
    OstTraceFunctionExit0( CUPNPTMICONMAPPING_ISCONVERSIONNEEDED_EXIT );
    return isConversionNeeded;
    }
