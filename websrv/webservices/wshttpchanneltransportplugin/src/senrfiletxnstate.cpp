/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include <http.h>
#include "senrfiletxnstate.h"
#include "senhttpchannel.h"
#include "sendebug.h"
#include "senlogger.h"
#include "senasynctimeout.h"


// MACROS
const TInt KDataPartLength = 1024;
// =========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSenRfileTxnState::CSenRfileTxnState
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//

CSenRfileTxnState::CSenRfileTxnState(MSenResponseObserver& aObserver)//,
                           //RFileLogger* aLog)
    : CSenTxnState (aObserver/*, iLog(aLog)*/)
    {
    }

// ----------------------------------------------------------------------------
// CSenRfileTxnState::NewLC
// ----------------------------------------------------------------------------
//

CSenRfileTxnState* CSenRfileTxnState::NewLC(MSenResponseObserver& aObserver,
                                  //RFileLogger* aLog,
                                  const TDesC8* aUri,
                                  CSenHttpChannelImpl* aHttpClient,
                                  const TDesC8& aContentType,
                                  const RFile& aContent)
    {
    CSenRfileTxnState* pNew = new (ELeave) CSenRfileTxnState(aObserver);//, aLog);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aUri, aHttpClient,aContentType, aContent);
    return pNew;
    }

// ----------------------------------------------------------------------------
// CSenRfileTxnState::NewL
// ----------------------------------------------------------------------------
//

CSenRfileTxnState* CSenRfileTxnState::NewL(MSenResponseObserver& aObserver,
                                 //RFileLogger* aLog,
                                 const TDesC8* aUri,
                                 CSenHttpChannelImpl* aHttpClient,
                                 const TDesC8& aContentType,
                                 const RFile& aContent)
    {
    CSenRfileTxnState* pNew = NewLC( aObserver, /*aLog,*/ aUri,
                                aHttpClient,aContentType, aContent );
    CleanupStack::Pop(pNew);
    return pNew;
    }

// ----------------------------------------------------------------------------
// CSenRfileTxnState::ConstructL
// ----------------------------------------------------------------------------
//

void CSenRfileTxnState::ConstructL(const TDesC8* aUri,
                              CSenHttpChannelImpl* aHttpClient,
                              const TDesC8& aContentType,
                              const RFile& aContent)
    {
    __ASSERT_ALWAYS(aUri != NULL,
                    User::Panic(KRequestUriNullPanicText,
                                CSenHttpChannel::ERequestUriNull));
    iRequestUri = aUri->AllocL();
    iFile = aContent;
    iHttpClient = aHttpClient ;
    iSize = 0 ;
    TInt err = iFile.Size(iSize) ;
    if(iSize >0)
        {
            iHasRequestBody = ETrue;
            iPostContentType = aContentType.AllocL();
        }    
    }

// ----------------------------------------------------------------------------
// CSenRfileTxnState::~CSenRfileTxnState
// ----------------------------------------------------------------------------
//
CSenRfileTxnState::~CSenRfileTxnState()
    {
    iFile.Close();
    }


// ----------------------------------------------------------------------------
// CSenRfileTxnState::GetNextDataPart
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
TBool CSenRfileTxnState::GetNextDataPart(TPtrC8& aDataPart)
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenRfileTxnState::GetNextDataPart")));
    TBool noMoreData = ETrue;   // No more data
    TInt retVal(KErrNone);
    if ( iFile.SubSessionHandle() )    // Send in a chunked mode from file
        {        
        if ( !iReadData )
            {
            TRAP(retVal,
		            iReadData = HBufC8::NewL( KDataPartLength );  
		            TPtr8 ptr = iReadData->Des();
		            User::LeaveIfError( iFile.Read( ptr, KDataPartLength ) );
            		);
            
            // More content remains if buffer is full
            // If buffer is not full (!EOF) => no more data
            if (iReadData->Length() == KDataPartLength)
	            {
	            iMoreContent = ETrue ;	
	            }
	            else
	            {
	            iMoreContent = EFalse ;
	            }
            }
        
        // Check if more content to be send (false = more data)  
        noMoreData = !MoreBody();        
        }
    
    aDataPart.Set( *iReadData );
    return noMoreData;
    }

// ----------------------------------------------------------------------------
// CSenRfileTxnState::ReleaseData
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
void CSenRfileTxnState::ReleaseData()
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenRfileTxnState::ReleaseData")));
    if ( MoreBody() )    // More data
        {
        TRAP_IGNORE( iHttpClient->NotifyMoreBodyL() );
        }
    
    delete iReadData;
    iReadData = NULL;   
    }

// ----------------------------------------------------------------------------
// CSenRfileTxnState::OverallDataSize
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
TInt CSenRfileTxnState::OverallDataSize()
    {
	// File is uploaded in chunked mode.
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KNormalLogLevel, _L8("CSenRfileTxnState::OverallDataSize() [%d]"), iSize));	
	    if(iFile.SubSessionHandle())
		    {
		    return iSize;	
		    }
	    else
			{
			return KErrNotFound;	
			}
    }

// ----------------------------------------------------------------------------
// CSenRfileTxnState::Reset
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
TInt CSenRfileTxnState::Reset()
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenRfileTxnState::Reset")));
    if ( iFile.SubSessionHandle() )
        {
        TInt pos;
        return iFile.Seek( ESeekStart, pos );   // Go to
        }
    
    return KErrNone;     // KErrNone if reset succeeded
    }

// ---------------------------------------------------------------------------
// Return flag if more body exist to be send in chunked mode.
// ---------------------------------------------------------------------------
//
TBool CSenRfileTxnState::MoreBody() const
    {
    // File == chunked mode
    return ( iFile.SubSessionHandle() && iMoreContent );
    }
//  End of File
