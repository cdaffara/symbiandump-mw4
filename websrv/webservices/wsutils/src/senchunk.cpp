/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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












#include "senchunk.h"
#include "sendebug.h" // internal Utils\inc - filelogging MACROs
#include "senlogger.h"

const TInt KSenChunkFourByteAlignment = 4;

EXPORT_C CSenChunk* CSenChunk::NewL(const TDesC& aChunkName)
    {
    CSenChunk* pOperation = NewLC(aChunkName);
    CleanupStack::Pop();
    return pOperation;
    }

EXPORT_C CSenChunk* CSenChunk::NewLC(const TDesC& aChunkName)
    {
    CSenChunk* pOperation = new (ELeave) CSenChunk;
    CleanupStack::PushL(pOperation);
    pOperation->ConstructL(aChunkName);
    return pOperation;
    }

void CSenChunk::ConstructL(const TDesC& aChunkName)
    {
    if (aChunkName.Length() > 0)
        {
        ipChunkName = aChunkName.AllocL();
        }
    else
        {
        ipChunkName = NULL;
        }
    }

EXPORT_C CSenChunk::CSenChunk() :
    ipChunkName(NULL)
    {
    }

EXPORT_C CSenChunk::~CSenChunk()
    {
    CloseChunk();
    delete ipChunkName;
    }

EXPORT_C RChunk& CSenChunk::Chunk()
    {
    return iChunk;
    }

EXPORT_C TPtrC CSenChunk::ChunkName() const
    {
    if(ipChunkName)
        {
        return *ipChunkName;
        }
    else
        {
        return KNullDesC();
        }
    }

EXPORT_C TBool CSenChunk::operator==(const CSenChunk& a)
    {
    return (this==&a);
    }

EXPORT_C TInt CSenChunk::CreateChunk()
    {
    TInt retVal(KErrNone);
#ifdef EKA2
    // Create protected global chunk in EKA2
    // (Global chunk will be protected chunk when chunk name length is zero)
    retVal = iChunk.CreateGlobal(KNullDesC, KSenMinRChunkSize, KSenMaxRChunkSize);
#else // EKA1
    // Create named global chunk in EKA1
    retVal = iChunk.CreateGlobal(*ipChunkName, KSenMinRChunkSize, KSenMaxRChunkSize);
#ifdef _SENDEBUG
        TLSLOG_FORMAT((KSenUtilsLogChannel, KNormalLogLevel, _L8("- RChunk.CreateGlobal('%S', %d, %d) returned: %d"),
                ipChunkName,
                KSenMinRChunkSize,
                KSenMinRChunkSize,
                retVal));
#endif // _SENDEBUG
#endif // EKA2/EKA1
    if (retVal == KErrNone)
        {
        TUint8* heapPtr = iChunk.Base();
        // Create header in the beginning of the chunk
        TLSLOG_FORMAT((KSenUtilsLogChannel, KNormalLogLevel, _L8("CSenChunk::CreateChunk KHeaderOffset size: (%d)"), KHeaderOffset));        
        new (heapPtr+KHeaderOffset) TChunkHeader;
        TChunkHeader& header = ChunkHeader();
        header.SetDescriptorCount(0);
        header.SetMessageType(MSenMessage::EMessageBase);
        header.SetMessageDirection(MSenMessage::EOutbound);
        header.SetPropertiesType(MSenProperties::ENotInUse);
        header.SetDoInitBeforeSending(EFalse);
        header.SetTransactionId(0);
        header.SetPendingTransactionId(0);
        }
    return retVal;
    }
    
EXPORT_C TInt CSenChunk::OpenChunk()
    {
    TInt retVal(KErrNone);
#ifdef EKA2
    retVal = KErrNotSupported;
#else // EKA1
    // Open named global chunk in EKA1
    retVal = iChunk.OpenGlobal(*ipChunkName, EFalse); 
#ifdef _SENDEBUG
        TLSLOG_FORMAT((KSenUtilsLogChannel, KNormalLogLevel, _L8("- RChunk.OpenGlobal() returned %d"), retVal));
#endif // _SENDEBUG
#endif // EKA2/EKA1
    return retVal;
    }
    
EXPORT_C TInt CSenChunk::OpenChunkFromRMsgL(const RMessage2& aMessage, TInt aIndex)
    {
    TInt retVal(KErrNone);
#ifdef EKA2 // // EKA2
    retVal = iChunk.Open(aMessage, aIndex, EOwnerProcess);
#else // EKA1
    HBufC* pChunkName = HBufC::NewLC(KMaxName);
    TPtr chunkname = pChunkName->Des();
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, retVal = aMessage.Read(aIndex, chunkname); )

    if(retVal==KErrNone && leaveCode != KErrNone)
        {
#ifdef _SENDEBUG
            TLSLOG_FORMAT((KSenUtilsLogChannel, KMinLogLevel, _L8("- aMessage.Read(aIndex, chunkname) leaved: %d"), leaveCode ));
#endif // _SENDEBUG
        retVal = leaveCode;
        }
        
    ipChunkName = chunkname.AllocL();
    CleanupStack::PopAndDestroy(pChunkName);

#ifdef _SENDEBUG
    if (ipChunkName->Length() > 0 )
        {
        TLSLOG_FORMAT((KSenUtilsLogChannel, KNormalLogLevel, _L8("- RChunk name from RMessage2: '%S'"), ipChunkName ));
        }
#endif // _SENDEBUG

    if(retVal==KErrNone)
        {
        retVal = iChunk.OpenGlobal(*ipChunkName, EFalse); 
#ifdef _SENDEBUG
             TLSLOG_FORMAT((KSenUtilsLogChannel, KNormalLogLevel, _L8("- RChunk.OpenGlobal() returned %d"), retVal));
#endif // _SENDEBUG
        }    
#endif // EKA2/EKA1
    return retVal;
    }

EXPORT_C TInt CSenChunk::OpenChunkFromHandleNumberL(TInt aHandleOrError)
    {
    TInt retVal(KErrNone);
#ifdef EKA2 // // EKA2
    retVal = iChunk.SetReturnedHandle(aHandleOrError);
#else // EKA1
    iChunk.SetHandle(aHandleOrError);
#endif // EKA2/EKA1
    return retVal;
    }
    
EXPORT_C void CSenChunk::CloseChunk()
    {
#ifdef _SENDEBUG
        TLSLOG_FORMAT((KSenUtilsLogChannel, KNormalLogLevel, _L8("CSenChunk::CloseChunk - handle: %d"), iChunk.Handle()));
#endif // _SENDEBUG

    THandleInfo chunkInfo;
    iChunk.HandleInfo(&chunkInfo);
#ifdef _SENDEBUG
        TLSLOG_FORMAT((KSenUtilsLogChannel, KNormalLogLevel, _L8("- Before Close(): TChunkInfo: numOpenInThread: %d, numOpenInProcess: %d, numProcesses: %d"),
            chunkInfo.iNumOpenInThread,
            chunkInfo.iNumOpenInProcess,
            chunkInfo.iNumProcesses));
#endif // _SENDEBUG

    iChunk.Close();
    }
    
EXPORT_C TInt CSenChunk::AdjustChunk(TInt aNewSize)
    {
    // Ensure that at least 10064 bytes are allocated:
    if(aNewSize<KSenMinRChunkSize)
        {
        aNewSize = KSenMinRChunkSize;
        }
    TInt retVal = iChunk.Adjust(aNewSize);
    TLSLOG_FORMAT((KSenUtilsLogChannel, KNormalLogLevel, _L8("CSenChunk::AdjustChunk returns: (%d)"), retVal));
    return retVal;
    }

EXPORT_C TChunkHeader& CSenChunk::ChunkHeader()
    {
    return *reinterpret_cast<TChunkHeader*>(iChunk.Base()+KHeaderOffset);
    }

EXPORT_C TInt CSenChunk::DescToChunk(const TDesC8& aDescriptor)
    {
    TInt retVal(KErrNone);

    TInt size(aDescriptor.Length()+sizeof(TPtr8)+KSenChunkFourByteAlignment+KDataOffset);

    retVal = AdjustChunk(size);

    if(retVal == KErrNone)
        {
        TUint8* heapPtr = iChunk.Base();
        TChunkHeader& header = ChunkHeader();
        header.SetDescriptorCount(1);
        heapPtr += KDataOffset;
        heapPtr = RoundToNextDividableByFourAddress(heapPtr);
        // Place a new descriptor in the chunk to initialize it
        new (heapPtr) TPtr8(heapPtr + sizeof(TPtr8), 0, aDescriptor.Length());
        // Set the descriptor
        TPtr8* pDes = reinterpret_cast<TPtr8*>(heapPtr);
        pDes->Zero();
        pDes->Append(aDescriptor);
        }
    return retVal;
    }
EXPORT_C TInt CSenChunk::DescToChunk(const TDesC8& aDescriptor, TInt aIndex)
    {
#ifdef _SENDEBUG
        TLSLOG_L(KSenUtilsLogChannel,KMinLogLevel ,"CSenChunk::DescFromChunk");
#endif // _SENDEBUG
    TInt retVal(KErrNone);
    
    if ( iChunk.Handle() > KErrNone )
        {
        TInt maxPtr = iChunk.Size();

        TInt size(aDescriptor.Length()+sizeof(TPtr8)+(aIndex+1)*KSenChunkFourByteAlignment);
        retVal = AdjustChunk(maxPtr+size);
        
        TChunkHeader& header = *reinterpret_cast<TChunkHeader*>(iChunk.Base() + KHeaderOffset);
        
        if ( aIndex <= header.DescriptorCount() )
            {
            header.SetDescriptorCount(aIndex+1);
            
            TUint8* heapPtr = iChunk.Base();
            heapPtr += KDataOffset;
            heapPtr = RoundToNextDividableByFourAddress(heapPtr);
            TPtr8* pDesc;
            pDesc = reinterpret_cast<TPtr8*>(heapPtr);
            for (TInt i=1; i < aIndex; i++)
                {
                heapPtr += (sizeof(TPtr8) + pDesc->MaxLength());
                heapPtr = RoundToNextDividableByFourAddress(heapPtr);
                pDesc = reinterpret_cast<TPtr8*>(heapPtr);
                }
            heapPtr += (sizeof(TPtr8) + pDesc->MaxLength());
            heapPtr = RoundToNextDividableByFourAddress(heapPtr);
            
            // Place a new descriptor in the chunk to initialize it
            new (heapPtr) TPtr8(heapPtr + sizeof(TPtr8), 0, aDescriptor.Length());
            // Set the descriptor
            pDesc = reinterpret_cast<TPtr8*>(heapPtr);
            pDesc->Zero();
            pDesc->Append(aDescriptor);
            }
        else
            {
            retVal = KErrNotFound;
            }
        }
    else
        {
        retVal = KErrBadHandle;
#ifdef _SENDEBUG
            TLSLOG_FORMAT((KSenUtilsLogChannel, KMinLogLevel, _L8("CSenChunk::DescFromChunk(chunk), RChunk.Handle(): %d is an error!"),
                iChunk.Handle()));
#endif // _SENDEBUG
        }
        
    return retVal;
    }
    
EXPORT_C TInt CSenChunk::AllocDescToChunk(TInt size, TPtr8*& apAllocated)
    {
    TInt retVal(KErrNone);
    
    retVal = AdjustChunk(size+KSenChunkFourByteAlignment);
    
    if(retVal == KErrNone)
        {
        TUint8* heapPtr = iChunk.Base();
        TChunkHeader& header = ChunkHeader();
        header.SetDescriptorCount(1);
        heapPtr += KDataOffset;
        heapPtr = RoundToNextDividableByFourAddress(heapPtr);
        // Place a new descriptor in the chunk to initialize it
        new (heapPtr) TPtr8(heapPtr + sizeof(TPtr8), 0, size);
        // Set the descriptor
        apAllocated = reinterpret_cast<TPtr8*>(heapPtr);
        apAllocated->Zero();
        }
    return retVal;    
    }

EXPORT_C TInt CSenChunk::DescsToChunk(const TDesC8& aDescriptor1,
                                   const TDesC8& aDescriptor2)
    {
TInt retVal(KErrNone);

    TInt size(KDataOffset+aDescriptor1.Length()+aDescriptor2.Length()+\
              2*sizeof(TPtr8)+2*KSenChunkFourByteAlignment); 

    retVal = AdjustChunk(size);

    if(retVal == KErrNone)
        {
        TUint8* heapPtr = iChunk.Base();
        TChunkHeader& header = ChunkHeader();
        header.SetDescriptorCount(2);
        heapPtr += KDataOffset;
        heapPtr = RoundToNextDividableByFourAddress(heapPtr);
        // Place a new descriptor in the chunk to initialize it
        new (heapPtr) TPtr8(heapPtr + sizeof(TPtr8), 0, aDescriptor1.Length());
        // Set the descriptor
        TPtr8* pDes = reinterpret_cast<TPtr8*>(heapPtr);
        pDes->Zero();
        pDes->Append(aDescriptor1);

        heapPtr = heapPtr + sizeof(TPtr8) + aDescriptor1.Length();
        heapPtr = RoundToNextDividableByFourAddress(heapPtr);
        // Place a new descriptor in the chunk to initialize it
        new (heapPtr) TPtr8(heapPtr + sizeof(TPtr8), 0, aDescriptor2.Length());
        // Set the descriptor
        pDes = reinterpret_cast<TPtr8*>(heapPtr);
        pDes->Zero();
        pDes->Append(aDescriptor2);
        }
    return retVal;

    }

EXPORT_C TInt CSenChunk::DescFromChunk(TPtrC8& aDesc, TInt aIndex)
    {
#ifdef _SENDEBUG
        TLSLOG_L(KSenUtilsLogChannel,KMinLogLevel ,"CSenChunk::DescFromChunk");
#endif // _SENDEBUG
    TInt retVal(KErrNone);
    
    if(iChunk.Handle()>KErrNone)
        {
        TChunkHeader& header = *reinterpret_cast<TChunkHeader*>(iChunk.Base() + KHeaderOffset);
        
        if (aIndex < header.DescriptorCount() && aIndex >= 0)
            {
            TUint8* heapPtr = iChunk.Base();
            heapPtr += KDataOffset;
            heapPtr = RoundToNextDividableByFourAddress(heapPtr);
            TPtr8* pDesc;
            pDesc = reinterpret_cast<TPtr8*>(heapPtr);
            for (TInt i=1; i <= aIndex; i++)
                {
                heapPtr += (sizeof(TPtr8) + pDesc->MaxLength());
                heapPtr = RoundToNextDividableByFourAddress(heapPtr);
                pDesc = reinterpret_cast<TPtr8*>(heapPtr);
                }
            const TInt length = pDesc->Length();
            const TInt maxLength = pDesc->MaxLength();
            pDesc->Set(heapPtr + sizeof(TPtr8), length, maxLength);
            aDesc.Set(*pDesc);
            }
        else
            {
            retVal = KErrNotFound;
            }
        }
    else
        {
        retVal = KErrBadHandle;
#ifdef _SENDEBUG
            TLSLOG_FORMAT((KSenUtilsLogChannel, KMinLogLevel, _L8("CSenChunk::DescFromChunk(chunk), RChunk.Handle(): %d is an error!"),
                iChunk.Handle()));
#endif // _SENDEBUG
        }

    return retVal;
    }
    
EXPORT_C TInt CSenChunk::DescsFromChunk(TPtrC8& aDesc1, TPtrC8& aDesc2)
    {
    TInt retVal;
    retVal = DescFromChunk(aDesc1,0);
    if (retVal == KErrNone)
        {
        retVal = DescFromChunk(aDesc2,1);
        }
    return retVal;
    }

EXPORT_C void CSenChunk::ChunkToArgs(TIpcArgs& aArgs, TInt aIndex)
    {
#ifdef EKA2
    aArgs.Set(aIndex, iChunk); // EKA2
#else
    aArgs.Set(aIndex, ipChunkName); // EKA1
#endif
    }

EXPORT_C void CSenChunk::SetLogger(RFileLogger* aLog)
    {
    iLog = aLog;
    }

EXPORT_C RFileLogger* CSenChunk::Log() const
    {
    return iLog;
    }
    
TUint8* CSenChunk::RoundToNextDividableByFourAddress(TUint8* aValue)
    {
    TInt modulo = ((TUint)aValue)%KSenChunkFourByteAlignment;
    if ( modulo == 0 ) 
        {
        return aValue;
        }
    else
        {
        return aValue+(KSenChunkFourByteAlignment-modulo);        
        }
    }
    
EXPORT_C RFile& CSenChunk::RequestFileHandle()
	{
	return iFile;
	}
	
EXPORT_C void CSenChunk::RequestFileHandle(RFile& aFile)
	{
	iFile = aFile;
	}
    

    
// End of File
