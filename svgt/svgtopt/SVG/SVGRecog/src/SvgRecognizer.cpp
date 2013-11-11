/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
 *
*/


#include <apmrec.h>
#include <apmstd.h>
#include <f32file.h>

#include <implementationproxy.h>

#include "SvgRecognizer.h"

// --------------------------------------------------------------------------
// CSvgRecognizer::CSvgRecognizer() : CApaDataRecognizerType( KUidMimeSvgRecognizer,
// ---------------------------------------------------------------------------
CSvgRecognizer::CSvgRecognizer() : CApaDataRecognizerType( KUidMimeSvgRecognizer,
                                                           CApaDataRecognizerType::ELow )
    {
    iCountDataTypes = 1;
    }

// --------------------------------------------------------------------------
// TUint CSvgRecognizer::PreferredBufSize()
// ---------------------------------------------------------------------------
TUint CSvgRecognizer::PreferredBufSize()
    {
    return KMaxBufferLength;
    }

// --------------------------------------------------------------------------
// TDataType CSvgRecognizer::SupportedDataTypeL( TInt /*aIndex */) const
// ---------------------------------------------------------------------------
TDataType CSvgRecognizer::SupportedDataTypeL( TInt /*aIndex */) const
    {
    return TDataType( KDataTypeImageSvg );
    }

// --------------------------------------------------------------------------
// void CSvgRecognizer::DoRecognizeL( const TDesC& aName,
// ---------------------------------------------------------------------------
void CSvgRecognizer::DoRecognizeL( const TDesC& aName,
                                   const TDesC8& /*aBuffer */)
    {
    iConfidence = ENotRecognized;

    const TInt positionOfLastDot = aName.LocateReverse( '.' );

    if ( positionOfLastDot >= 0 )
        { //check the extension
        const TPtrC ext = aName.Mid( positionOfLastDot );
        /* SVGEngine suppported file extensions */
        _LIT(KSvgExt1, ".svg");
        _LIT(KSvgExt2, ".svgz");
        _LIT(KSvgExt3, ".svgb");

        if ( ext.CompareF( KSvgExt1 ) == 0 ||
             ext.CompareF( KSvgExt2 ) == 0 ||
             ext.CompareF( KSvgExt3 ) == 0 )
            {
            iConfidence = ECertain;
            iDataType = TDataType( KDataTypeImageSvg );
            }
        }
    }

// --------------------------------------------------------------------------
// CApaDataRecognizerType* CSvgRecognizer::CreateRecognizerL()
// ---------------------------------------------------------------------------
CApaDataRecognizerType* CSvgRecognizer::CreateRecognizerL()
    {
    CApaDataRecognizerType* lSvgDataRecogType = NULL;
    lSvgDataRecogType = new (ELeave) CSvgRecognizer();
    return lSvgDataRecogType; // NULL if new failed
    }

// --------------------------------------------------------------------------
// const TImplementationProxy SvgRecogImplTable[] =
// ---------------------------------------------------------------------------
const TImplementationProxy SvgRecogImplTable[] =
  {
    IMPLEMENTATION_PROXY_ENTRY(KImplementationValue, CSvgRecognizer::CreateRecognizerL)
  };

// --------------------------------------------------------------------------
// EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aImplTableCount)
// ---------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aImplTableCount)
    {
    aImplTableCount = sizeof(SvgRecogImplTable) / sizeof(TImplementationProxy);
    return SvgRecogImplTable;
    }
