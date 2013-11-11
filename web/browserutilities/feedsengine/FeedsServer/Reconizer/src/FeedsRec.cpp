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
* Description:  Recognizer for the bowser supported MIME types.
*
*/


#include "FeedsRec.h"

#include <ImplementationProxy.h>


// Constants
_LIT8(KMimeTypeRss,   "application/rss+xml");
//_LIT8(KMimeTypeAtom,  "application/atom+xml");
_LIT8(KMimeTypeXml1,  "application/xml");
_LIT8(KMimeTypeXml2,  "text/xml");


// -----------------------------------------------------------------------------
// CFeedsRec::CFeedsRec
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsRec::CFeedsRec()
        :CApaDataRecognizerType(KUidMimeFeedsRec, CApaDataRecognizerType::EHigh)
	{
	iCountDataTypes = KSupportedMimetypes;
	}


// -----------------------------------------------------------------------------
// CFeedsRec::PreferredBufSize
//
// Returns the preferred buffer size.
// -----------------------------------------------------------------------------
//
TUint CFeedsRec::PreferredBufSize()
	{
	return 0x100;
	}


// -----------------------------------------------------------------------------
// CFeedsRec::SupportedDataTypeL
//
// Returns the index'ed supported mime-type.
// -----------------------------------------------------------------------------
//
TDataType CFeedsRec::SupportedDataTypeL(TInt aIndex) const
	{
	__ASSERT_DEBUG((aIndex >= 0) && (aIndex < KSupportedMimetypes), User::Invariant());

	switch (aIndex)
		{
        case 0:
            return TDataType(KMimeTypeRss);

        default:
            return TDataType();
        }
	}


// -----------------------------------------------------------------------------
// CFeedsRec::DoRecognizeL
//
// Given the buffer it examines the buffer and sets the inherited
// member variable, iConfidence.
// -----------------------------------------------------------------------------
//
void CFeedsRec::DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
    {
    TInt  confidence;

    iConfidence = ENotRecognized;

    // Examine the buffer for RSS.
    if ((confidence = RecognizeRss(aName, aBuffer)) != ENotRecognized)
        {
        iDataType = TDataType(KMimeTypeRss);
        iConfidence = confidence;
        }

    // Examine the buffer for Atom.
    // TODO: in 3.1

    // Examine the buffer for Opml 1.0.
    // TODO: in 3.1
    }


// -----------------------------------------------------------------------------
// CFeedsRec::DoRecognizeRss
//
// Given the buffer it examines the buffer and returns the confidence level that
// this document is rss.
// -----------------------------------------------------------------------------
//
TInt CFeedsRec::RecognizeRss(const TDesC& /*aName*/, const TDesC8& aBuffer)
    {
    _LIT8(KRss, "<rss");
    _LIT8(KRdf, "<rdf:RDF");
    _LIT8(KChannel, "<channel");
    _LIT8(KRss1, "xmlns=\"http://purl.org/rss/1.0/\"");
    _LIT8(KXml, "<?xml");
    _LIT8(KDtd, "//DTD RSS");
    _LIT8(KOPML,  "<opml");

    TInt  confidence = ENotRecognized;
    TInt  count = 0;

    // TODO: Determine if there are common dot-extensions used for 
    //       this mime-type.  If so, add code to check for them in aName.
    //       If found set iConfidence to ECertain.

    if (aBuffer.FindF(KRss) != KErrNotFound)
        {
        count++;
        }
    if (aBuffer.FindF(KRdf) != KErrNotFound)
        {
        count++;
        }
    if (aBuffer.FindF(KChannel) != KErrNotFound)
        {
        count++;
        }
    if (aBuffer.FindF(KRss1) != KErrNotFound)
        {
        count++;
        }
    if (aBuffer.FindF(KXml) != KErrNotFound)
        {
        if (aBuffer.FindF(KOPML) == KErrNotFound)
        	{        	
	        count++;
        	}
        }
    if (aBuffer.FindF(KDtd) != KErrNotFound)
        {
        count++;
        }
    
    if (count == 0)
        {
        confidence = ENotRecognized;
        }
    else if (count <= 2)
        {
        confidence = EProbable;
        }
    else
        {
        confidence = ECertain;
        }

    return confidence;
    }


// Constants
const TImplementationProxy ImplementationTable[] = 
	{
    IMPLEMENTATION_PROXY_ENTRY(KFeedsRecImplUIDValue, CFeedsRec::CreateRecognizerL)
	};


// -----------------------------------------------------------------------------
// ImplementationGroupProxy
//
// Returns the proxy.
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable; 
    }


// -----------------------------------------------------------------------------
// CFeedsRec::CreateRecognizerL
//
// Creates an recognizer instance.
// -----------------------------------------------------------------------------
//
CApaDataRecognizerType* CFeedsRec::CreateRecognizerL()
	{
	return new (ELeave) CFeedsRec();
	}

