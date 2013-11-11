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
* Description:  A util class to cleanup libxml2 related instances.
*
*/



#include <e32base.h>
#include <libxml2_xmlmemory.h>
#include <libxml2_globals.h>

#include "CleanupLibXml2.h"


// -----------------------------------------------------------------------------
// CleanupLibXml2::PushL
//
// Pushes a Libxml2 doc-ptr on the cleanup stack.
// -----------------------------------------------------------------------------
//
void CleanupLibXml2::PushL(xmlDocPtr aDocPtr)
    {
    CleanupStack::PushL(TCleanupItem(&CleanupXmlDocPtr, aDocPtr));
    }
    

// -----------------------------------------------------------------------------
// CleanupLibXml2::PushL
//
// Pushes a Libxml2 string on the cleanup stack.
// -----------------------------------------------------------------------------
//
void CleanupLibXml2::PushL(xmlChar* aXmlString)
    {
    CleanupStack::PushL(TCleanupItem(&CleanupXmlString, aXmlString));
    }
    

// -----------------------------------------------------------------------------
// CleanupLibXml2::PushL
//
// Pushes a Libxml2 buffer-ptr on the cleanup stack.
// -----------------------------------------------------------------------------
//
void CleanupLibXml2::PushL(xmlBufferPtr aBufferPtr)
    {
    CleanupStack::PushL(TCleanupItem(&CleanupXmlBufferPtr, aBufferPtr));
    }
    

//#if 0
// -----------------------------------------------------------------------------
// CleanupLibXml2::PushL
//
// Pushes a Libxml2 text-writer-ptr on the cleanup stack.
// -----------------------------------------------------------------------------
//
void CleanupLibXml2::PushL(xmlTextWriterPtr aTextWriterPtr)
    {
    CleanupStack::PushL(TCleanupItem(&CleanupXmlTextWriterPtr, aTextWriterPtr));
    }
//#endif    

// -----------------------------------------------------------------------------
// CleanupLibXml2::CleanupXmlDocPtr
//
// Cleanup stack callback method to delete a Libxml2 doc-ptr.
// -----------------------------------------------------------------------------
//
void CleanupLibXml2::CleanupXmlDocPtr(TAny *aPtr)
    {
    xmlFreeDoc(static_cast<xmlDocPtr>(aPtr));
    }


// -----------------------------------------------------------------------------
// CleanupLibXml2::CleanupXmlDocPtr
//
// Cleanup stack callback method to delete a Libxml2 doc-ptr.
// -----------------------------------------------------------------------------
//
void CleanupLibXml2::CleanupXmlString(TAny *aPtr)
    {
    xmlFree(static_cast<void*>(aPtr));
    }


// -----------------------------------------------------------------------------
// CleanupLibXml2::CleanupXmlBufferPtr
//
// Cleanup stack callback method to delete a Libxml2 buffer-ptr.
// -----------------------------------------------------------------------------
//
void CleanupLibXml2::CleanupXmlBufferPtr(TAny *aPtr)
    {
    xmlBufferFree(static_cast<xmlBufferPtr>(aPtr));
    }


//#if 0
// -----------------------------------------------------------------------------
// CleanupLibXml2::CleanupXmlTextWriterPtr
//
// Cleanup stack callback method to delete a Libxml2 text-writer-ptr.
// -----------------------------------------------------------------------------
//
void CleanupLibXml2::CleanupXmlTextWriterPtr(TAny *aPtr)
    {
    xmlFreeTextWriter(static_cast<xmlTextWriterPtr>(aPtr));
    }
//#endif
