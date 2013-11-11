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
*      Wrapper functions and classes for cXmlLibrary (NW_...) functionality.  
*      
*
*/


// INCLUDE FILES

#include "NwUtil.h"
#include "CodError.h"
#include "CodPanic.h"
#include <xml/cxml/nw_dom_document.h>

// ================= LOCAL FUNCTIONS =======================

/**
* Cleanup function of type TCleanupOperation. Calls NW_String_deleteStorage
* for aString.
* @param aString NW_String_t* string.
*/
LOCAL_C void NwStringDeleteStorage( TAny* aString )
    {
    NW_String_deleteStorage( STATIC_CAST( NW_String_t*, aString ) );
    }

// ================= GLOBAL FUNCTIONS =======================

// ---------------------------------------------------------
// NwStatus2Error()
// ---------------------------------------------------------
//
TInt NwStatus2Error( NW_Status_t aNwStatus )
    {
    TInt ret( KErrGeneral );

    switch ( aNwStatus )
        {
        case NW_STAT_SUCCESS:
            {
            ret = KErrNone;
            break;
            }

        case NW_STAT_OUT_OF_MEMORY:
            {
            ret = KErrNoMemory;
            break;
            }

        case NW_STAT_CANCELLED:
        case NW_STAT_CONN_CANCELLED:
            {
            ret = KErrCancel;
            break;
            }

        default:
            {
            // XML parser returns NW_STAT_FAILURE for practically any error
            // except OOM. Let's say it's an ill-formed XML.
            ret = KErrCodInvalidDescriptor;
            break;
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// CleanupNwUcs2PushL()
// ---------------------------------------------------------
//
void CleanupNwUcs2PushL( NW_Ucs2* aUcs2 )
    {
    // TODO: replace 'free' with NW_Mem_Free, when they export it!
    // That will remove our dependency on STLIB.
    __ASSERT_DEBUG( aUcs2, CodPanic( ECodInternal ) );
    CleanupStack::PushL( TCleanupItem( free, aUcs2 ) );
    }

// ---------------------------------------------------------
// CleanupNwStringStoragePushL()
// ---------------------------------------------------------
//
void CleanupNwStringStoragePushL( NW_String_t* aString )
    {
    __ASSERT_DEBUG( aString, CodPanic( ECodInternal ) );
    CleanupStack::PushL( TCleanupItem( NwStringDeleteStorage, aString ) );
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RNwWbxmlDictionary::Initialize()
// ---------------------------------------------------------
//
TInt RNwWbxmlDictionary::Initialize
( NW_Int32 aCount, NW_WBXML_Dictionary_t* aDictArray[] )
    {
    return NwStatus2Error
        ( NW_WBXML_Dictionary_initialize( aCount, aDictArray ) );
    }

// ---------------------------------------------------------
// RNwWbxmlDictionary::Close()
// ---------------------------------------------------------
//
void RNwWbxmlDictionary::Close()
    {
    NW_WBXML_Dictionary_destroy();
    }

// ---------------------------------------------------------
// CNwDomDocumentNode::~CNwDomDocumentNode()
// ---------------------------------------------------------
//
CNwDomDocumentNode::~CNwDomDocumentNode()
    {
    if ( iDocNode )
        {
        NW_DOM_DocumentNode_Delete( iDocNode );
        }
    }
