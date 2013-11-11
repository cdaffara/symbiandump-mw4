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
* Description:  Implementation of CFileReader
*
*/


// INCLUDE FILES
#include "ResourceHandle.h"
#include "FileConnection.h"
#include "FileReader.h"

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
// CFileReader::CFileReader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFileReader::CFileReader(RFile& aFile, TInt aChunkSize, FileConnection* aFileConnection) 
    : CActive( CActive::EPriorityHigh ), 
      m_file( aFile ), m_chunkSize( aChunkSize ), m_fileConnection( aFileConnection ),
      m_chunkBufPtr( NULL, 0 ), m_stopReading( EFalse )
{
    CActiveScheduler::Add( this );
}

// -----------------------------------------------------------------------------
// CFileReader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFileReader::ConstructL()
{
    m_chunkBuffer = HBufC8::NewL( m_chunkSize );
    //
    m_chunkBufPtr.Set( (TUint8*)m_chunkBuffer->Des().Ptr(), m_chunkSize, m_chunkSize );
}

// -----------------------------------------------------------------------------
// CFileReader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFileReader* CFileReader::NewL(RFile& aFile, TInt aChunkSize, FileConnection* aFileConnection) 
{
    CFileReader* self = new( ELeave ) CFileReader( aFile, aChunkSize, aFileConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

    
// Destructor
CFileReader::~CFileReader()
{
    Cancel();
    //
    delete m_chunkBuffer;
}

// -----------------------------------------------------------------------------
// CFileReader::StartReading
// 
// 
// -----------------------------------------------------------------------------
//
void CFileReader::StartReading()
{
    //
    m_filePos = 0;
    // start reading
    ReadFile();
}

// -----------------------------------------------------------------------------
// CFileReader::StopReading
// 
// 
// -----------------------------------------------------------------------------
//
void CFileReader::StopReading()
{
    m_stopReading = ETrue;
}

// -----------------------------------------------------------------------------
// CFileReader::GetChunkBuffer
// 
// 
// -----------------------------------------------------------------------------
//
void CFileReader::GetChunkBuffer( TPtrC8& aChunkBuffer )
{
    aChunkBuffer.Set( m_chunkBufPtr );
}


// -----------------------------------------------------------------------------
// CFileReader::DoCancel
// 
// 
// -----------------------------------------------------------------------------
//
void CFileReader::DoCancel()
{
    if (IsActive()) {
        m_file.ReadCancel(iStatus);
    }    
}

// -----------------------------------------------------------------------------
// CFileReader::RunL
// 
// 
// -----------------------------------------------------------------------------
//
void CFileReader::RunL()
{
    // do not send the chunk back to the transaction if the trans has been cancelled
    if( !m_stopReading ) {
        switch( iStatus.Int() )
        {
            case KErrNone:
            {
                TInt bytesRead = m_chunkBufPtr.Length();
                // 0 means we read beyond the file 
                if( bytesRead ) {
                    // let the observer know. 
                    m_fileConnection->response();
                    // If it is not Browser content, reading the file
                    // is canceled in FileConnection::download().
                    if( !m_stopReading ) {
                        // fix file pos
                        m_filePos+=bytesRead;
                        // keep on reading
                        ReadFile();
                    }
                }
                else {
                    // let the observer know
                    m_fileConnection->complete(KErrNone);
                }
                break;
            }
            default:
            {
                //
                m_fileConnection->complete( iStatus.Int() );
                break;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// CFileReader::ReadFile
// 
// 
// -----------------------------------------------------------------------------
//
void CFileReader::ReadFile() 
{
    if( !m_stopReading ) {
        SetActive();
        m_file.Read( m_filePos, m_chunkBufPtr, m_chunkSize, iStatus );
    }
}


//  End of File   
