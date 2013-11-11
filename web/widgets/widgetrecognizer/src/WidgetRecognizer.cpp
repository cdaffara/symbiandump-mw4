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
* Description:  Recognizer for the widget supported MIME types.
*
*/

#include "WidgetRecognizer.h"
#include "implementationproxy.h"

// CONSTANTS
_LIT(KWidgetExtension, ".wgz");
_LIT8(KWidgetMimeType,"application/x-nokia-widget");

//
// CWidgetsRecognizer
//

// ====================== MEMBER FUNCTIONS =============================

// ============================================================================
// CWidgetRecognizer::CWidgetRecognizer()
// Default constructor
//
// @since 3.1
// ============================================================================
CWidgetRecognizer::CWidgetRecognizer()
:CApaDataRecognizerType( KUidMimeWidgetRecognizer, CApaDataRecognizerType::EHigh )
    {
    iCountDataTypes = 1;
    }

// ============================================================================
// From CApaDataRecognizerType
// Gets the size of buffer preferred for the purpose of recognizing the data type.
// @return The preferred data size.
// @since 3.1
// ============================================================================
TUint CWidgetRecognizer::PreferredBufSize()
    {
    return 0x80;
    }

// ============================================================================
// From CApaDataRecognizerType
// Gets one of the data (MIME) types that the recognizer can recognize.
// @param aIndex - An index that identifies the data type. Typically,
//        the minimum value is zero and the maximum value is the value of MimeTypesCount() - 1.
//
// @return The data (MIME) type.
// @since 3.1
// ============================================================================
TDataType CWidgetRecognizer::SupportedDataTypeL( TInt /*aIndex*/ ) const
    {
    // only support one widget mime type
    return TDataType( KWidgetMimeType() );
    }

// ============================================================================
// From CApaDataRecognizerType
// This implements recognition behaviour -- called by RecognizeL()
// @param aName - The name of the data; typically this is a file name
//                containing the data to be recognized.
// @param aBuffer - A buffer containing data to be recognized; typically,
//                  this is read from the start of the file containing the data.
//
// @return void.
// @since 3.1
// ============================================================================
void CWidgetRecognizer::DoRecognizeL( const TDesC& aName, const TDesC8& /*aBuffer*/ )
    {
    iConfidence = ENotRecognized;

    if ( aName.Length() >= 4 )
        {
        TInt dotPos = aName.LocateReverse( '.' );
        if ( dotPos != KErrNotFound )
            {
            TInt extLength = aName.Length() - dotPos;
            HBufC* ext = aName.Right( extLength ).AllocL();
            CleanupStack::PushL( ext );

            // recognize .wgz extensions.
            if (ext->CompareF( KWidgetExtension ) == 0)
                {
                iDataType = TDataType( KWidgetMimeType );
                iConfidence = ECertain;
                }
            CleanupStack::PopAndDestroy();  // ext
            }
        }
    }

// ============================================================================
// CWidgetRecognizer::CreateRecognizerL
// Create CWidgetRecognizer instance
// ============================================================================
//
CApaDataRecognizerType* CWidgetRecognizer::CreateRecognizerL()
    {
    return new (ELeave) CWidgetRecognizer();
    }

// Implementation table contains only one entry for widget recognizer
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KWidgetRecognizerImplUIDValue, CWidgetRecognizer::CreateRecognizerL)
    };

// ========================== OTHER EXPORTED FUNCTIONS =========================

// ============================================================================
// ImplementationGroupProxy
// Returns the filters implemented in this DLL
// Returns: The filters implemented in this DLL
// ============================================================================
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
