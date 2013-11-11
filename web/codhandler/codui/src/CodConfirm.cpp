/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CCodConfirm.   
*
*/


// INCLUDE FILES

#include <AknUtils.h>
#include <AknDef.h>
#include <StringLoader.h>
#include <CodData.h>
#include <CodDefs.h>
#include <avkon.rsg>
#include <CodUi.rsg>
#include "CodConfirm.h"
#include "CodLogger.h"
#include <Oma2Agent.h>
#include <aknlayout.cdl.h>


#include <eikmobs.h>
#include <aknPopup.h>
#include <aknlists.h>
#include    <StringLoader.h>

_LIT( KListBoxSeparator, "\t" );

// FORWARD DECLARATIONS


// ================= TYPES =======================

/**
* Mime type to resource id mapping entry.
*/
struct TMimeAndResId
    {
    const TText8* iName;    ///< MIME type.
    TInt iResId;            ///< Resource id.
    };

// ================= CONSTANTS =======================

/// Number of bytes in a kilobyte.
LOCAL_D const TInt KCodKbyte = 1024;

/**
* Null-terminated MIME-to-resId mapping table.
* Order of entries is important (from most specific to most generic), as
* entries are evaluated linearly.
*/
LOCAL_D const TMimeAndResId KCodMimeAndResIds[] =
    {
        { _S8( "application/vnd.nokia.ringing-tone" ),  R_QTN_CD_TYPE_RTONE },
        { _S8( "application/x-nokiaGameData" ),         R_QTN_CD_TYPE_GAME_LEVEL },
        { _S8( "application/vnd.oma.drm" ),             R_QTN_CD_TYPE_NOT_CLASSIFIED },
        { _S8( "application/" ),                        R_QTN_CD_TYPE_APPLICATION },
        { _S8( "image/vnd.nokia.ota-bitmap" ),          R_QTN_CD_TYPE_OPERATOR_LOGO },
        { _S8( "image/vnd.nok-oplogo" ),                R_QTN_CD_TYPE_OPERATOR_LOGO },
        { _S8( "image/vnd.nok-picturemessage" ),        R_QTN_CD_TYPE_PICTURE_MESSAGE },
        { _S8( "image/" ),                              R_QTN_CD_TYPE_IMAGE },
        { _S8( "audio/" ),                              R_QTN_CD_TYPE_SOUND },
        { _S8( "text/X-vCard" ),                        R_QTN_CD_TYPE_VCARD },
        { _S8( "text/" ),                               R_QTN_CD_TYPE_DOCUMENT },
        { _S8( "" ),                                    R_QTN_CD_TYPE_NOT_CLASSIFIED },
        { NULL,                                         0 }                                     
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodConfirm:CCodConfirm()
// ---------------------------------------------------------
//
CCodConfirm::CCodConfirm()
    {
    }

    

// ---------------------------------------------------------
// CCodConfirm::~CCodConfirm()
// ---------------------------------------------------------
//
CCodConfirm::~CCodConfirm()
    {
    }


// ---------------------------------------------------------------------------
// CCodView::AllocTypeNameL
// ---------------------------------------------------------------------------
//
 HBufC* CCodConfirm::AllocTypeNameL( const TDesC8& aMimeType )
    {
    const TMimeAndResId* mapping;
    TInt resId = R_QTN_CD_VALUE_UNDEFINED;
    for( mapping = &KCodMimeAndResIds[0]; mapping->iName; mapping++ )
        {
        TPtrC8 name( mapping->iName );
        // Looking for partial match (leftmost part of aMimeType is compared
        // to mapping->iName).
        if( name.Length() <= aMimeType.Length() )
            {
            if( aMimeType.Left( name.Length() ).CompareF( name ) == 0 )
                {
                resId = mapping->iResId;
                break;
                }
            }
        }
    return StringLoader::LoadL( resId, iCoeEnv );
    }


// ---------------------------------------------------------------------------
// CCodView::DisplayType
// ---------------------------------------------------------------------------
//
 TPtrC8 CCodConfirm::DisplayType( const CCodData& aData )
    {
    for ( TInt i = 0; i < (*aData[aData.ActiveDownload()]).Types().MdcaCount(); i++ )
        {
        TPtrC8 type( (*aData[aData.ActiveDownload()]).Types().MdcaPoint( i ) );
        if (
            type.Compare( KOma1DrmMessageContentType ) &&
            type.Compare( KOma1XmlRoContentType ) &&
            type.Compare( KOma1WbxmlRoContentType ) &&
            type.Compare( KOma2RoContentType ) &&
            type.Compare( KOma2TriggerContentType ) &&
            type.Compare( KOma1DcfContentType ) &&
            type.Compare( KOma2DcfContentType ) &&
            type.Compare( KOmaImportContentType )
           )
            {
            return type;    // Non-DRM, return it.
            }
        }
    return (*aData[aData.ActiveDownload()]).Type();    // By default returns the first, whatever it is.
    }

    
// ---------------------------------------------------------------------------
// CCodConfirm::ShowDownloadConfirmationL
// ---------------------------------------------------------------------------
//
TBool CCodConfirm::ShowDownloadConfirmationL(TInt aCbaResource, 
                                          const CCodData& aData, 
                                          TBool aCod )
{
    HBufC* itemText = NULL; // a buffer holding the text for each list item

    CEikFormattedCellListBox* listBox = new ( ELeave ) CAknDoublePopupMenuStyleListBox;
    CleanupStack::PushL( listBox );

    // If supported show OK & Cancel buttons, otherwise just CANCEL
    
    CAknPopupList* popupList = CAknPopupList::NewL
            ( listBox, aCbaResource, AknPopupLayouts::EMenuDoubleWindow );
    CleanupStack::PushL( popupList );

    TInt flags = CEikListBox::EDisableHighlight;
    listBox->ConstructL( popupList, flags );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL
        ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    MDesCArray* itemList = listBox->Model()->ItemTextArray();
    CDesCArray* itemArray = (CDesCArray*)itemList;
    
    //-------------------------------------------
    // Header
    HBufC* header = StringLoader::LoadLC( R_QTN_CD_TITLE );
    popupList->SetTitleL( *header );
    CleanupStack::PopAndDestroy( header );  // header
    header = NULL;
    

    // make description of buf
    HBufC* buf;

    //-------------------------------------------
    // Name 
    //
    HBufC* popHeader = StringLoader::LoadLC( R_QTN_CD_DETAIL_NAME );
    // If media count more than one, show album name if present
    itemText = FormatListBoxItemLC( *popHeader, aData.Count() > 1 && aData.Name().Length()? aData.Name() : (*aData[aData.ActiveDownload()]).Name() );
    itemArray->AppendL( *itemText );
    CleanupStack::PopAndDestroy( 2, popHeader ); // itemText,popHeader
    itemText = NULL;
    popHeader = NULL;

    //-------------------------------------------
    // Content Size
    //
    HBufC* sizeHeader = StringLoader::LoadLC( R_QTN_CD_DETAIL_SIZE );
    TInt size = aData.Size();
    if( size >= KCodKbyte )
        {
        // Round up.
        buf = StringLoader::LoadLC
            ( R_QTN_SIZE_KB, (size - 1) / KCodKbyte + 1, iCoeEnv );
        }
    else
        {
        buf = StringLoader::LoadLC( R_QTN_SIZE_B, size, iCoeEnv );
        }
    buf->Des().Trim();   
    itemText = FormatListBoxItemLC( *sizeHeader, *buf );
    itemArray->AppendL( *itemText );
    CleanupStack::PopAndDestroy( 3, sizeHeader ); // itemText, sizeHeader
    itemText = NULL;
    sizeHeader = NULL;

    //-------------------------------------------
    // Type 
    //
    popHeader = StringLoader::LoadLC( R_QTN_CD_DETAIL_TYPE );
    buf = AllocTypeNameL( DisplayType(aData));
    CleanupStack::PushL(buf);
    itemText = FormatListBoxItemLC( *popHeader, *buf );
    itemArray->AppendL( *itemText );
    CleanupStack::PopAndDestroy( 3, popHeader ); // itemText, nameHeader
    itemText = NULL;
    popHeader = NULL;
    
    
    //-------------------------------------------
    // Price: only if it's aCod
    //
    if ( aCod ) 
    {
        popHeader = StringLoader::LoadLC( R_QTN_CD_DETAIL_PRICE );
        itemText = FormatListBoxItemLC( *popHeader, aData.Price() );
        itemArray->AppendL( *itemText );
        CleanupStack::PopAndDestroy( 2, popHeader ); // itemText, nameHeader
        itemText = NULL;
        popHeader = NULL;    	
    }

    //-------------------------------------------
    // Detail Description
    //
    popHeader = StringLoader::LoadLC( R_QTN_CD_DETAIL_DESCRIPT );   
    HBufC* popValue = StringLoader::LoadLC(R_QTN_CD_VALUE_UNDEFINED ); 
    if (aData.Description().Length())
    {
       itemText = FormatListBoxItemLC( *popHeader, aData.Description() );	
    }
    else
    {
    	itemText = FormatListBoxItemLC( *popHeader, *popValue );
    } 
    itemArray->AppendL( *itemText );
    CleanupStack::PopAndDestroy( 3, popHeader ); // itemText, popValue
    itemText = NULL;
    popHeader = NULL;    	
    

    //-------------------------------------------
    // Vendor Information 
    //
    popHeader = StringLoader::LoadLC( R_QTN_CD_DETAIL_VENDOR );
    
	popValue = StringLoader::LoadLC(R_QTN_CD_VALUE_UNDEFINED ); 
    if (aData.Vendor().Length())
       itemText = FormatListBoxItemLC( *popHeader, aData.Vendor() );
    else 
	   itemText = FormatListBoxItemLC( *popHeader, *popValue ); 
    itemArray->AppendL( *itemText );
    CleanupStack::PopAndDestroy( 3, popHeader ); // itemText, popValue
    itemText = NULL;
    popHeader = NULL;    	

    TInt popupOk = popupList->ExecuteLD();



    // cleanup
    CleanupStack::Pop( popupList );
    CleanupStack::PopAndDestroy( listBox ); // listBox
    
	return popupOk; 
}
// -----------------------------------------------------------------------------
// CCodConfirm::FormatListBoxItemLC
// -----------------------------------------------------------------------------
//
HBufC* CCodConfirm::FormatListBoxItemLC
    ( const TDesC& aFirst, const TDesC& aSecond )
    {
    HBufC* res = HBufC::NewLC( aFirst.Length() + 
                               aSecond.Length() + 
                               KListBoxSeparator().Length() );
    res->Des().Copy( aFirst );
    res->Des().Append( KListBoxSeparator );
    res->Des().Append( aSecond );
    return res;
    }
