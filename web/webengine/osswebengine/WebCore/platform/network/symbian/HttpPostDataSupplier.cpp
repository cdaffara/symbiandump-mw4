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
* Description:  Implementation of HttpPostDataSupplier
*
*/


// INCLUDE FILES
#include "config.h"

#include <e32std.h>
#include <e32base.h>
#include "HttpPostDataSupplier.h"
#include "PostDataItem.h"
#include "FormData.h"
#include "BrCtl.h"

using namespace WebCore;

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
// The post is transmitted in parts. A buffer is allocated of max size to
// contain the next part to be transmitted. 
const TInt KPartMaxSize = 10240;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================


void HttpPostDataSupplier::initL(FormData* formData)
{
    m_firstPart = true;
    m_lastPart = false;
    m_dataPart = NULL;
    m_totalSize = 0;
    m_postItemIndex = 0;
    m_refCount = 0;

    PostDataItem* postDataItem = NULL;
    //iterate thru the post items and create CPostDataItem instance for each data
    size_t n = formData->elements().size();
    for (size_t index = 0; index < n; ++index) {
        const FormDataElement& formDataElement = formData->elements()[index];
        if (formDataElement.m_type == FormDataElement::data) {
            postDataItem = new (ELeave) PostDataItem;
            // No CleanupStack support for a class that is not based on CBase.
            m_postData.append(postDataItem);
            postDataItem->initL(&formDataElement);
        }
        else {
            ASSERT(formDataElement.m_type == FormDataElement::encodedFile);
            // if a file read failed, just ignore the form element and instead
            // post rest of the content
            postDataItem = new(ELeave) FileDataItem;
            m_postData.append(postDataItem);
            TRAPD(error, postDataItem->initL(&formDataElement));
            if (error != KErrNone) {
                m_postData.removeLast();
                delete postDataItem;
                if(error == KErrNoMemory) {
                    User::Leave(error);
                }
                continue;
            }
        }
        // get the size of the data and add to the total size 
        m_totalSize += postDataItem->pendingContentSize();
    }
}


// -----------------------------------------------------------------------------
// HttpPostDataSupplier::HttpPostDataSupplier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
HttpPostDataSupplier::HttpPostDataSupplier(RHTTPTransaction* transaction, CBrCtl* brctl) 
:m_transaction(transaction), m_brctl(brctl)
{
}


HttpPostDataSupplier::~HttpPostDataSupplier()
{
    deleteAllValues(m_postData);
}

// -----------------------------------------------------------------------------
// HttpPostDataSupplier::ReleaseData
// 
// -----------------------------------------------------------------------------
//
void HttpPostDataSupplier::ReleaseData()
{
    delete m_dataPart;
    m_dataPart = NULL;
    if(!m_lastPart) {
        TRAP_IGNORE(m_transaction->NotifyNewRequestBodyPartL());
    }
}

// -----------------------------------------------------------------------------
// HttpPostDataSupplier::OverallDataSize
// 
// -----------------------------------------------------------------------------
//
TInt HttpPostDataSupplier::OverallDataSize() 
{
    return m_totalSize; 
}

// -----------------------------------------------------------------------------
// HttpPostDataSupplier::OverallDataSize
// 
// -----------------------------------------------------------------------------
//
TInt HttpPostDataSupplier::Reset() 
{
    m_firstPart = true;
    m_postItemIndex = 0;
    m_lastPart = false;
    //iterate thru the post items and create CPostDataItem instance for each data
    size_t n = m_postData.size();
    for (int index  = 0 ; index < m_postData.size(); index++) {
        PostDataItem* postDataItem = m_postData[index];     
        postDataItem->reset();
    }
    delete m_dataPart;
    m_dataPart = NULL;
    return KErrNone; 
}

// -----------------------------------------------------------------------------
// HttpPostDataSupplier::getNextDataPartL
// Accessor function to the request body. Return the body in one chunk.
// -----------------------------------------------------------------------------
//
void HttpPostDataSupplier::getNextDataPartL(TPtrC8& aDataPart)
{
    if(m_totalSize > KPartMaxSize && m_firstPart /*&& iBrCtlLoadEventObserver*/) {
        m_brctl->HandleBrowserLoadEventL( 
            TBrCtlDefs::EEventUploadStart, m_totalSize / 1000, 0) ;            
        m_firstPart = false;
    }
    // release previous part if it is still valid
    delete m_dataPart;
    m_dataPart = NULL;
    int partSize = 0;
    int partPostItems = m_postItemIndex;
    // find out the amount of memory that needs to be allocated for this part
    while (partPostItems < m_postData.size()) {
        PostDataItem* postDataItem = m_postData[partPostItems];
        if((partSize += postDataItem->pendingContentSize()) > KPartMaxSize ) {
            partSize = KPartMaxSize;
            break;
        }
        partPostItems++;
    }    
    // read the content of the items 
    m_dataPart = HBufC8::NewL(partSize);
    for (int index = m_postItemIndex; index < m_postData.size() && index <= partPostItems; index++) {
        PostDataItem* postDataItem = m_postData[index];
        HBufC8* itemContent = postDataItem->dataL(partSize - m_dataPart->Length());
        m_dataPart->Des().Append( *itemContent );
        delete itemContent;
    } 
    // if all the items have been read 
    if(partPostItems == m_postData.size()) {
        m_lastPart = true;
    }
    else {
    // if the last part has more data to be posted , set m_postItemIndex to that index else advance the index
        if(m_postData[partPostItems]->pendingContentSize()) {
            m_postItemIndex = partPostItems;
        }
        // advance to the next item
        else {
            m_postItemIndex = partPostItems + 1;    
        }
    }    
    aDataPart.Set(m_dataPart->Des());
    if(m_totalSize > KPartMaxSize) {
        if(!m_lastPart) {
            m_brctl->HandleBrowserLoadEventL( 
                TBrCtlDefs::EEventUploadIncrement, aDataPart.Length()/1000, 0) ;            
        }
        else {
            m_brctl->HandleBrowserLoadEventL( 
                TBrCtlDefs::EEventUploadFinished, aDataPart.Length()/1000, 0) ;            
        }
    }
}

// -----------------------------------------------------------------------------
// HttpPostDataSupplier::GetNextDataPart
// Accessor function to the request body. Return the body in one chunk.
// -----------------------------------------------------------------------------
//
TBool HttpPostDataSupplier::GetNextDataPart(TPtrC8& aDataPart)
{
    if(!m_lastPart) {
        TRAP_IGNORE( getNextDataPartL(aDataPart));
        // TODO:how should the error be handled ????, should the transaction be cancelled
    }
    else {
        aDataPart.Set(KNullDesC8);
    }
    return m_lastPart;
}

//  End of File
