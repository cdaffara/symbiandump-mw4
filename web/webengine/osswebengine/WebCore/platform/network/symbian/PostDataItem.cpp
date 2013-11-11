/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of PostDataItem & FileDataItem
*
*/


// INCLUDE FILES
#include "PostDataItem.h"
#include "FormData.h"
#include "StaticObjectsContainer.h"

using namespace WebCore;

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// class members initialization
// -----------------------------------------------------------------------------
//
void PostDataItem::initL(const FormDataElement* formDataElement)
{
    m_dataOffset = 0;
    TPtrC8 ptr((TText8*)formDataElement->m_data.data(), formDataElement->m_data.size());
    m_data = ptr.AllocL();
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
//
PostDataItem::~PostDataItem()
{
    delete m_data;
}

// -----------------------------------------------------------------------------
// Retuns data contained by the post data itemaSizeOfDataToSupply , size of the data
// to be supplied if the aSizeOfDataToSupply is not supplied , then the content till
// the end of the buffer is returned
// -----------------------------------------------------------------------------
//
HBufC8* PostDataItem::dataL(int sizeToSupply )
{
    HBufC8* dataToSupply = NULL;
    int sizeOfDataToSupply = sizeToSupply;
    // if sizeOfDataToSupply exceeds the size of data supplied then set the sizeOfDataToSupply
    // to the content size
    if(sizeOfDataToSupply == -1 || sizeOfDataToSupply >= (m_data->Length() - m_dataOffset)) {
        sizeOfDataToSupply = m_data->Length()- m_dataOffset;
    }
    TPtrC8 dataSupplyDes(m_data->Ptr() + m_dataOffset, sizeOfDataToSupply);
    dataToSupply = dataSupplyDes.AllocL();
    // add supplied size count to the data offset
    m_dataOffset += sizeOfDataToSupply;
    return  dataToSupply;
}

// -----------------------------------------------------------------------------
// resets the state of the object
// -----------------------------------------------------------------------------
//
void PostDataItem::reset()
{
    m_dataOffset = 0;
}

// -----------------------------------------------------------------------------
// returns the pending size of the content to be posted
// -----------------------------------------------------------------------------
//
int PostDataItem::pendingContentSize() const
{
    return m_data->Length() - m_dataOffset;
}


// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
//
FileDataItem::~FileDataItem()
{
    if(m_fileLocked && m_fileSize > 0 ) {
        m_file.UnLock(0, m_fileSize);
    }
    m_file.Close();
}


// -----------------------------------------------------------------------------
// class members initialization
// -----------------------------------------------------------------------------
//
void FileDataItem::initL(const FormDataElement* formDataElement)
{
    m_dataOffset = 0;
    m_fileLocked = false;
    m_data = NULL;
    RFs rfs = StaticObjectsContainer::instance()->fsSession();
    HBufC* fileName = formDataElement->m_filename.des().AllocLC();
    // size of the file
    User::LeaveIfError(m_file.Open(rfs, fileName->Des(), EFileRead | EFileShareReadersOnly));
    User::LeaveIfError(m_file.Size(m_fileSize));
    if (m_fileSize > 0)
        {
    User::LeaveIfError(m_file.Lock(0, m_fileSize));
    m_fileLocked = ETrue;
        }
    CleanupStack::PopAndDestroy();// fileName
}


// -----------------------------------------------------------------------------
// Retuns data contained by the post data itemaSizeOfDataToSupply , size of the data
// to be supplied if the sizeToSupply is not supplied , then the content till
// the end of the buffer is returned
// -----------------------------------------------------------------------------
//
HBufC8* FileDataItem::dataL(int sizeToSupply )
{
    HBufC8* dataToSupply = NULL;
    int sizeOfDataToSupply = sizeToSupply;
    
    // if the sizeOfDataToSupply is not set then read from the offset to the end of the file
    // if the sizeOfDataToSupply  is greater than the size of the pending file content
    // then return content of size sizeOfDataToSupply
    if(sizeOfDataToSupply == -1 || sizeOfDataToSupply >= (m_fileSize - m_dataOffset)) {
        sizeOfDataToSupply = m_fileSize - m_dataOffset;
    }
    dataToSupply = HBufC8::NewLC(sizeOfDataToSupply);
    TPtr8 dataToSupplyDes = dataToSupply->Des();
    // read from the offset to the size of the data to be supplied
    User::LeaveIfError(m_file.Read(m_dataOffset, dataToSupplyDes, sizeOfDataToSupply));
    // add the read byte size count to the data offset
    m_dataOffset += sizeOfDataToSupply;
    CleanupStack::Pop();//dataToSupply
    return  dataToSupply;
}


// -----------------------------------------------------------------------------
// returns the pending size of the content to be posted
// -----------------------------------------------------------------------------
//
int FileDataItem::pendingContentSize() const
{
    return m_fileSize - m_dataOffset;
}

