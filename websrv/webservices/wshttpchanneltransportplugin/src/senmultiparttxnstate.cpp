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

#include "senmultiparttxnstate.h"
#include "senhttpchannel.h"
#include "sendebug.h"
#include "senlogger.h"


// =========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSenMultiPartTxnState::CSenMultiPartTxnState
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CSenMultiPartTxnState::CSenMultiPartTxnState(MSenResponseObserver& aObserver)
    : CSenTxnState(aObserver),
   	  iRootCid(NULL),
	  iMimeBoundaryOut(NULL),
	  iMimeBoundaryIn(NULL),
	  iMimeBoundaryEndIn(NULL),
  	  iPartDataOut(1),
 	  iLastPartOut(EFalse),
   	  iPartDataIn(1),
 	  iLastPartIn(EFalse),
 	  iCurPart(EFalse),
 	  iBinaryDataEnd(ETrue),
 	  iChecked(EFalse),
 	  iBinaryDataPart(NULL),
 	  iBinaryDataParts(NULL),
 	  iBinaryDataRest(NULL),
 	  iFirst(ETrue)
    {
    }
    

    
CSenMultiPartTxnState::~CSenMultiPartTxnState()
	{
	iMessage = NULL;
	iBinaryDataList.Close();
	iRootCid.Close();
	iMimeBoundaryOut.Close();
	iMimeBoundaryIn.Close();
	iMimeBoundaryEndIn.Close();
    iCurrentPart.Close();
    iSoapAction.Close();
    iXopEnvelopeResponse.Close();
    iBlobHeader.Close();
    
    for(TUint i(0); i < iFileNames.Count(); ++i) // iFileNames.Count() equals iCids.Count()
        {
        iFileNames[i].Close();
        iCids[i].Close();
        }
    iFileNames.Close();
    iCids.Close();
	}



// ---------------------------------------------------------------------
// calculates number of parts in BLOB (every part = 10KB) 
// and how many bytes there are in the last part
//----------------------------------------------------------------------
//
void CSenMultiPartTxnState::SizeBinaryData(TUint aIndex)
    {
    TUint size = iBinaryDataList[aIndex].Size();
    iBinaryDataParts = size/KTenKBytes;
    iBinaryDataRest = size%KTenKBytes;
    if (iBinaryDataParts > 0)
        {
        iBinaryDataEnd = EFalse;
        }
    }

// ---------------------------------------------------------------------
// creates header for Blob in Request
// ---------------------------------------------------------------------
//    
void CSenMultiPartTxnState::BlobHeaderL()
	{
	// iCurrentPart == header, binary part
	RBuf8 boundaryStart;
	boundaryStart.CleanupClosePushL();
	SenMultiPartUtils::BoundaryLineStartL(iMimeBoundaryOut, boundaryStart);	

	TUint index = (iPartDataOut - 3)/2;
    			
	RBuf8 headerBinaryData;
	headerBinaryData.CleanupClosePushL();
   	SenMultiPartUtils::HeaderBinaryDataL(index, iBinaryDataList, headerBinaryData);

	iCurrentPart.ReAllocL(2*KNewLine().Length()+boundaryStart.Length()+headerBinaryData.Length());
	iCurrentPart.Append(KNewLine);
	iCurrentPart.Append(KNewLine);
	iCurrentPart.Append(boundaryStart);
	iCurrentPart.Append(headerBinaryData);
	CleanupStack::PopAndDestroy(&headerBinaryData); 
	CleanupStack::PopAndDestroy(&boundaryStart);  
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenMultiPartTxnState::GetNextDataPart - aDataPart (header, binary part):")));
	iCurPart = ETrue;
	}

// ---------------------------------------------------------------------
// for case if Blob is kept in a file 
// this function passes Blob from a file part by part (every part = 10KB)
// in Request
// ---------------------------------------------------------------------
//    
void CSenMultiPartTxnState::FileContainerL(TUint aIndex)
    {
    if (!iChecked)
        {
    	SizeBinaryData(aIndex);
    	iChecked = ETrue;
    	}
    if (iBinaryDataPart < iBinaryDataParts) 
        {
        SenMultiPartUtils::FileDataPartL(iBinaryDataPart, aIndex, iBinaryDataList, iCurrentPart);
        }
    else if (iBinaryDataRest != 0)
        {
        SenMultiPartUtils::FileDataRestL(iBinaryDataParts, iBinaryDataRest, aIndex, iBinaryDataList, iCurrentPart);
        iBinaryDataEnd = ETrue;
        }
    iCurPart = ETrue;    
    }

// ---------------------------------------------------------------------
// creates the last part (MimeBoundaryEnd) in Request
// ---------------------------------------------------------------------
//    
void CSenMultiPartTxnState::XopEndL()
	{
	// iCurrentPart == XOP end
	SenMultiPartUtils::BoundaryLineEndL(iMimeBoundaryOut, iCurrentPart);	
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenMultiPartTxnState::GetNextDataPart - aDataPart (boundary end):")));
	iCurPart = ETrue;
	iLastPartOut = ETrue;
	}

// ----------------------------------------------------------------------------
// CSenMultiPartTxnState::GetNextDataPart
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
TBool CSenMultiPartTxnState::GetNextDataPart(TPtrC8& aDataPart)
    {
    TBool lastPart(EFalse);
    TRAP_IGNORE(lastPart = GetNextDataPartL(aDataPart));
    return lastPart;
    }

// ----------------------------------------------------------------------------
// CSenMultiPartTxnState::ReleaseData
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
void CSenMultiPartTxnState::ReleaseData()
	{
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenMultiPartTxnState::ReleaseData")));
    iCurPart = EFalse;
    iCurrentPart.Close();
	if (!iLastPartOut)
		{
		// Notify HTTP of more data available immediately
		TRAP_IGNORE(Transaction().NotifyNewRequestBodyPartL());
		if (iBinaryDataEnd)
		    {
    		iPartDataOut++;
		    iChecked = EFalse;
		    iBinaryDataPart = 0;
		    }
		else 
		    {
		    ++iBinaryDataPart;
		    }    
		}
	}

// ----------------------------------------------------------------------------
// CSenMultiPartTxnState::OverallDataSize
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
TInt CSenMultiPartTxnState::OverallDataSize()
    {
	return KErrNotFound;
    }


// ----------------------------------------------------------------------------
// CSenMultiPartTxnState::Reset
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
TInt CSenMultiPartTxnState::Reset()
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenMultiPartTxnState::Reset")));
	return KErrNone;
    }

// ---------------------------------------------------------------------------
// parses Response from server (MultiPart message) and extracts MimeBoundary part
// and MimeBoundaryEnd part
// ---------------------------------------------------------------------------
//
void CSenMultiPartTxnState::MimeBoundaryInL(TPtrC8& aDataPartPtr, TInt& aOffset, TInt& aOffset1)
    {
    _LIT8(KDDash,"--");
    aOffset = aDataPartPtr.Find(KDDash); 	//CodeScannerWarnings
    if (aOffset == KErrNotFound)
        {
        User::Panic(KMultiPartResponseBodyInvalidPanicText, SenMultiPartUtils::EMultiPartResponseBodyInvalid);
        }
    aDataPartPtr.Set(aDataPartPtr.Right(aDataPartPtr.Length()-aOffset));
    aOffset = aDataPartPtr.Find(_L8("\r\n")); 

    iMimeBoundaryIn.ReAllocL(aOffset);
    iMimeBoundaryIn.Copy(aDataPartPtr.Left(aOffset));

    iMimeBoundaryEndIn.ReAllocL(aOffset+2);
    iMimeBoundaryEndIn.Append(iMimeBoundaryIn);
    iMimeBoundaryEndIn.Append(KDDash);
        
    aDataPartPtr.Set(aDataPartPtr.Right(aDataPartPtr.Length()-aOffset-2));

    aOffset = aDataPartPtr.Find(_L8("\r\n\r\n"));
    aOffset1 = aDataPartPtr.Find(iMimeBoundaryEndIn);
    if (aOffset1 != KErrNotFound)
        {
        iLastPartIn = ETrue;
        }
    if (aOffset != KErrNotFound) 
        {
        aDataPartPtr.Set(aDataPartPtr.Right(aDataPartPtr.Length()-aOffset-4));
        iPartDataIn++;
        }
    }

// ---------------------------------------------------------------------------
// parses Response from server (MultiPart message) and extracts XopEnvelope part
// ---------------------------------------------------------------------------
//
void CSenMultiPartTxnState::XopEnvelopeResponseL(TPtrC8& aDataPartPtr, TInt& aOffset, TInt& aOffset1)    
    {
    aOffset = aDataPartPtr.Find(iMimeBoundaryIn);
    aOffset1 = aDataPartPtr.Find(iMimeBoundaryEndIn);
    if (aOffset1 != KErrNotFound)
        {
        iLastPartIn = ETrue; 
        }
    if (aOffset == KErrNotFound) 
        {
        iXopEnvelopeResponse.ReAllocL(iXopEnvelopeResponse.Length()+aDataPartPtr.Length());
        iXopEnvelopeResponse.Append(aDataPartPtr);
        }
    else
        {
        iXopEnvelopeResponse.ReAllocL(iXopEnvelopeResponse.Length()+aOffset-4);
        iXopEnvelopeResponse.Append(aDataPartPtr.Left(aOffset-4));
        aDataPartPtr.Set(aDataPartPtr.Right(aDataPartPtr.Length()-aOffset));
        iPartDataIn++;
        iObserver->FileProgress(iId, ETrue, ETrue,
                    iXopEnvelopeResponse, iXopEnvelopeResponse.Length());    
        }
    }

// ---------------------------------------------------------------------------
// parses Response from server (MultiPart message) and extracts header of Blob part
// ---------------------------------------------------------------------------
//
void CSenMultiPartTxnState::BlobHeaderResponseL(TPtrC8& aDataPartPtr, TInt& aOffset, TInt& aOffset1) 
    {
    aOffset = aDataPartPtr.Find(_L8("\r\n\r\n"));
    aOffset1 = aDataPartPtr.Find(iMimeBoundaryEndIn);
    
    if (aOffset1 != KErrNotFound)
        {
        iLastPartIn = ETrue;
        }
    if (aOffset == KErrNotFound)
        {
        iBlobHeader.ReAllocL(iBlobHeader.Length()+aDataPartPtr.Length());
        iBlobHeader.Append(aDataPartPtr);
        }
    else
        {
        iBlobHeader.ReAllocL(iBlobHeader.Length()+aOffset);
        iBlobHeader.Append(aDataPartPtr.Left(aOffset));

        // extract cids
        SenMultiPartUtils::CidL(iBlobHeader, iCids);

        iBlobHeader.Close();
        
        aDataPartPtr.Set(aDataPartPtr.Right(aDataPartPtr.Length()-aOffset-4));
        iPartDataIn++;
        }
    }  
    
// ---------------------------------------------------------------------------
// parses Response from server (MultiPart message) and extracts Blob part
// puts it in the file
// ---------------------------------------------------------------------------
//
void CSenMultiPartTxnState::BlobResponseL(TPtrC8& aDataPartPtr, TInt& aOffset, TInt& aOffset1)
    {
    TInt size(0);
    aOffset = aDataPartPtr.Find(iMimeBoundaryIn);
    aOffset1 = aDataPartPtr.Find(iMimeBoundaryEndIn);
    if (aOffset1 != KErrNotFound)
        {
        iLastPartIn = ETrue;
        }
    TUint index = (iPartDataIn-4)/2;

    if (aOffset == KErrNotFound)
        {
        size = SenMultiPartUtils::SetFileL(index, iFirst, aDataPartPtr, iFileNames);
        iFirst = EFalse;
        }
    else
        {
        TPtrC8 blobPartPtr;
        blobPartPtr.Set(aDataPartPtr.Left(aOffset-4));
        size = SenMultiPartUtils::SetFileL(index, iFirst, blobPartPtr, iFileNames);
        aDataPartPtr.Set(aDataPartPtr.Right(aDataPartPtr.Length()-aOffset));
        iFirst = ETrue;
        iPartDataIn++;
        }
    iObserver->FileProgress(iId, ETrue, EFalse, iCids[index], size); 
    }      

// ---------------------------------------------------------------------------
// parses Response from server (MultiPart message) 
// ---------------------------------------------------------------------------
//
void CSenMultiPartTxnState::ParseMultiPartResponseL(TDesC8& aDataPart)
    {
    TPtrC8 dataPartPtr;
	dataPartPtr.Set(aDataPart);

    TInt offset(0);
    TInt offset1(-1);
    if (!iLastPartIn)
        {
        while ((dataPartPtr.Length()) && (offset != offset1))
            {
            if (iPartDataIn == 1)
	            {
	            MimeBoundaryInL(dataPartPtr, offset, offset1);
	            }
	        else if (iPartDataIn == 2)
   		        {
   		        XopEnvelopeResponseL(dataPartPtr, offset, offset1);
	            }
	        else if ((iPartDataIn > 2) && (iPartDataIn%2 != 0))
	            {
   		        BlobHeaderResponseL(dataPartPtr, offset, offset1);
  		        }
   	        else if ((iPartDataIn > 2) && (iPartDataIn%2 == 0))
   		        {
   		        BlobResponseL(dataPartPtr, offset, offset1);
   		        }
            }
        }
     }

// -----------------------------------------------------------------------------
// added HttpTransportProperties about Cids of Blobs and File namess, 
// where Blobs are kept
// and passes transport properties and XopEnvelope to an observer
//------------------------------------------------------------------------------
//
void CSenMultiPartTxnState::ResponseReceivedL(const SenMultiPartUtils::TMultiPartContentType& aMultiPartContentType)
    {
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenMultiPartTxnState(%d)::ResponseReceivedL"), iId));
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMaxLogLevel,"iXopEnvelopeResponse:");
    TLSLOG_ALL(KSenHttpChannelLogChannelBase , KMaxLogLevel,(iXopEnvelopeResponse));


    if(iXopEnvelopeResponse.Length())
        {
		CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();
															
		pHttpProperties->SetDownloadFolderL(KDownloadFolder);
		for (TUint i=0; i < iCids.Count(); ++i)
			{
			pHttpProperties->SetFileAttachmentL(iCids[i], iFileNames[i]);
			}
        HBufC8* pResponse = iXopEnvelopeResponse.AllocL();
        // delete allocated body
        iXopEnvelopeResponse.Close();  
        iObserver->ResponseReceivedL(iId, &aMultiPartContentType, pResponse, pHttpProperties);
       	CleanupStack::PopAndDestroy(pHttpProperties);
        }
    else
        {
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"Fatal: NULL response received in MultiPartTxnState!");
        // delete allocated body
        iXopEnvelopeResponse.Close();  
        }

    }
// ---------------------------------------------------------------------
// creates header for SoapEnvelope in Request
// ---------------------------------------------------------------------
//    
void CSenMultiPartTxnState::MessageHeaderL()
    {
    RBuf8 boundaryStart;
    boundaryStart.CleanupClosePushL();

    // Note: BoundaryLineStart re-allocates (and first de-allocates) boundaryStart
    SenMultiPartUtils::BoundaryLineStartL(iMimeBoundaryOut, boundaryStart);	

    RBuf8 headerRoot;
    headerRoot.CleanupClosePushL();

    // Note: HeaderRootL re-allocates (and first de-allocates) iRootCid
    SenMultiPartUtils::HeaderRootL(iMessage, iRootCid, headerRoot);
    		
    // iCurrentPart == header root:
    iCurrentPart.ReAllocL(boundaryStart.Length()+headerRoot.Length());
    iCurrentPart.Append(boundaryStart);
    iCurrentPart.Append(headerRoot);
    CleanupStack::PopAndDestroy(&headerRoot); 
    CleanupStack::PopAndDestroy(&boundaryStart); 
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenMultiPartTxnState::MessageHeaderL")));
    iCurPart = ETrue; 
    }

// ---------------------------------------------------------------------
// creates SoapEnvelope in Request
// ---------------------------------------------------------------------
//    
void CSenMultiPartTxnState::MessageL()
    {
    }


// ----------------------------------------------------------------------------
// creates Request for MultiPart message in MultiPart format 
// should be used to send MultiPart message through HTTP Channel
// ----------------------------------------------------------------------------
//
TBool CSenMultiPartTxnState::GetNextDataPartL(TPtrC8& aDataPart)
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenMtomTxnState::GetNextDataPart")));

	if(!HasRequestBody())
		{
		iLastPartOut = ETrue;
		iPartDataOut = 0;
		}	
	else 
	    {
	    if(!iCurPart) // Note: this is zero only when new part is requested
      		{
    		if ((iPartDataOut == 1) && (iPartDataOut < iCountDataParts))
    		    {
    		    MessageHeaderL(); // First part of MultiPart message = header of SoapEnvelope
    		    }
    		else if ((iPartDataOut == 2) && (iPartDataOut < iCountDataParts))
    		    {
    		    MessageL(); // Second part of MultiPart message = SoapEnvelope
    		    }
    		else if ((iPartDataOut > 2) && (iPartDataOut%2 != 0) && (iPartDataOut < iCountDataParts))
    		    {
    		    BlobHeaderL(); // Third and then all odd parts = headers of Blobs 
    		    }
    		else if ((iPartDataOut > 2) && (iPartDataOut%2 == 0) && (iPartDataOut < iCountDataParts))
    			{
    			TUint index = (iPartDataOut - 4)/2;
    			
    			switch (iBinaryDataList[index].NodeType()) // Fourth and then all even parts = Blobs
		            {
     		        case TXmlEngNode::EChunkContainer: // blob is kept in Chunk
    			        {
    		            TInt offset = iBinaryDataList[index].AsChunkContainer().ChunkOffset();
   			            aDataPart.Set(iBinaryDataList[index].AsChunkContainer().Chunk().Base()+offset,iBinaryDataList[index].Size());
     		            TLSLOG_ALL(KSenHttpChannelLogChannelBase , KMaxLogLevel,aDataPart); 
    		            iBinaryDataEnd = ETrue;
     		            return iLastPartOut;
    			        }
 	
 		            case TXmlEngNode::EFileContainer: // blob is kept in File
   			            {
                        FileContainerL(index);
    			        break;
    			        }
      	
 	                case TXmlEngNode::EBinaryContainer: // blob is kept as binary data
    			        {
	     		        aDataPart.Set(iBinaryDataList[index].AsBinaryContainer().Contents()); 
	     		        return iLastPartOut;
    			        }
    	
 	            	default:
 			            {
                    	User::Panic(KMultiPartBlobContainerTypeInvalidPanicText, 
                    	            SenMultiPartUtils::EMultiPartBlobContainerTypeInvalid); 			            }
 		                }

     			TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenMtomTxnState::GetNextDataPart - aDataPart (binary data)"))); 
                if (iBinaryDataPart)
                    {
                    iObserver->FileProgress(iId, EFalse, EFalse,
                                iBinaryDataList[index].Cid(), iBinaryDataPart*KTenKBytes);
                    }
    			}
    		else if (iPartDataOut == iCountDataParts)
    		    {
    		    TUint index = (iPartDataOut - 4)/2;
    		    XopEndL(); // The last part of MultiPart message = MimeBoundaryEnd
    		    if ((iPartDataOut > 2) && (iPartDataOut%2 == 0))
    		        {
    		        iObserver->FileProgress(iId, EFalse, EFalse,
                                iBinaryDataList[index].Cid(), iBinaryDataList[index].Size());
    		        }
        		    
    		    }
    		} // on first call, allocate the current part
	    aDataPart.Set(iCurrentPart); // .. otherwise, just re-use it
        TLSLOG_ALL(KSenHttpChannelLogChannelBase , KMaxLogLevel,aDataPart); 
        }
	return iLastPartOut;	
    }

//  End of File
