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
* Description:  Holds information about a feed encloures.
*
*/


#include "FeedAttributes.h"
#include "FeedsServerFeed.h"
#include "Logger.h"

// -----------------------------------------------------------------------------
// CEnclosure::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEnclosure* CEnclosure::NewL(CFeedsEntity* aFeedsEntity)
    {
    CEnclosure* self = new (ELeave) CEnclosure(aFeedsEntity);

    CleanupStack::PushL(self);

    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CEnclosure::CEnclosure
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEnclosure::CEnclosure(CFeedsEntity* aFeedsEntity):iLeakTracker(CLeakTracker::EEnclosure),
        iFeedsEntity(aFeedsEntity), iContentType(KNullDesC), 
        iSize(KNullDesC), iUrl(KNullDesC)
    {
    }

// -----------------------------------------------------------------------------
// CEnclosure::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnclosure::ConstructL()
    {
    TPtrC aContentType;
    TPtrC aSize;
    TPtrC aUrl;

    iFeedsEntity->GetStringValue( EEnclosureAttributeContentType, aContentType);
    SetAttribute( EEnclosureAttributeContentType, aContentType);

    iFeedsEntity->GetStringValue( EEnclosureAttributeSize, aSize);  
    SetAttribute( EEnclosureAttributeSize, aSize);

    iFeedsEntity->GetStringValue( EEnclosureAttributeLink, aUrl);  
    SetAttribute( EEnclosureAttributeLink, aUrl);    

    }

// -----------------------------------------------------------------------------
// CEnclosure::~CEnclosure
// Deconstructor.
// -----------------------------------------------------------------------------
//
CEnclosure::~CEnclosure()
    {
    }


// -----------------------------------------------------------------------------
// CEnclosure::ContentType
//
// Returns the ContentType.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CEnclosure::ContentType() const
    {  
    return iContentType;
    }


// -----------------------------------------------------------------------------
// CEnclosure::Size
//
// Returns the Size.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CEnclosure::Size() const
    {
    return iSize;
    }

// -----------------------------------------------------------------------------
// CEnclosure::Url
//
// Returns the url.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CEnclosure::Url() const
    {           
    return iUrl;
    }


// -----------------------------------------------------------------------------
// CEnclosure::SetAttribute
//
// Sets an attribute.
// -----------------------------------------------------------------------------
//
void CEnclosure::SetAttribute(TUint aAttribute, const TPtrC& aAttributeValue)
    {
    switch (aAttribute)
        {
        case EEnclosureAttributeContentType:
            iContentType.Set( aAttributeValue);
            break;

        case EEnclosureAttributeSize:
            iSize.Set( aAttributeValue);
            break;

        case EEnclosureAttributeLink:
            iUrl.Set( aAttributeValue);
            break;

        default:
            break;
        }
    }


//#ifdef _DEBUG
// -----------------------------------------------------------------------------
// CEnclosure::Debug_Print
//
// Prints the item to the log file.
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnclosure::Debug_Print(void) const
    {
    FEED_LOG(_L("Feeds"), _L("Feeds_Dump.log"), 
        EFileLoggingModeAppend, _L("\tItem:"));

    if (iContentType.Length() > 0)
        {
        FEED_LOG1(_L("Feeds"), _L("Feeds_Dump.log"), 
            EFileLoggingModeAppend, _L("\t\tTitle: %S"), &iContentType);
        }

    if (iSize.Length() > 0)
        {
        FEED_LOG1(_L("Feeds"), _L("Feeds_Dump.log"), 
            EFileLoggingModeAppend, _L("\t\tSize: %S"), &iSize);
        }

    if (iUrl.Length() > 0)
        {
        FEED_LOG1(_L("Feeds"), _L("Feeds_Dump.log"), 
            EFileLoggingModeAppend, _L("\t\tLink: %S"), &iUrl);
        }
    }
