/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  NVG Decoder source file
 *
*/

#include "NVGIconData.h"

//const TUint KEncodedDataLength        = 1024;
const TUint KEncodedDataGranularity   = 64;

CNVGIconData::CNVGIconData()
    : iNVGData(0),
    iNVGDataPtr(0, 0),
    iTotalRead(0),
    iReadStream(0)
    {
    }

CNVGIconData::~CNVGIconData()
    {
    delete iNVGData;
    }

CNVGIconData * CNVGIconData::NewL(TInt aLength)
    {
    CNVGIconData* self    = CNVGIconData::NewLC(aLength);
    CleanupStack::Pop(self);
    return self;
    }
    
CNVGIconData * CNVGIconData::NewLC(TInt aLength)
    {
    CNVGIconData* self    = new (ELeave) CNVGIconData;
    CleanupStack::PushL(self);

    self->ConstructL(aLength);
    return self;
    }

CNVGIconData * CNVGIconData::NewL(const TDesC8& aBuf)
    {
    CNVGIconData* self    = CNVGIconData::NewLC(aBuf);
    CleanupStack::Pop(self);
    return self;
    }

CNVGIconData * CNVGIconData::NewLC(const TDesC8& aBuf)
    {
    CNVGIconData* self    = new (ELeave) CNVGIconData;
    CleanupStack::PushL(self);

    self->ConstructL(aBuf);
    
    return self;
    }

CNVGIconData * CNVGIconData::NewL(TUint8 * aBuf, TInt aLength)
    {
    return CNVGIconData::NewL(TPtr8(aBuf, aLength, aLength));
    }

CNVGIconData * CNVGIconData::NewLC(TUint8 * aBuf, TInt aLength)
    {
    return CNVGIconData::NewLC(TPtr8(aBuf, aLength, aLength));
    }

void CNVGIconData::ConstructL(const TDesC8& aBuf)
    {
    iNVGDataPtr.Set((TUint8 *)aBuf.Ptr(), aBuf.Length(), aBuf.Length());
    
    //set the reading pointers
    BeginRead();
    }

void CNVGIconData::ConstructL(TUint aLength)
    {    
    iNVGData = HBufC8::NewL(aLength);
    iNVGDataPtr = iNVGData->Des();
    }

void CNVGIconData::EncodeDataL(const TAny *aData, TUint aLength)
    {
    TInt result = KErrNone;
    
    TInt encodedDataLength      = iNVGDataPtr.Length() + aLength;
    TInt encodedDataMaxLength   = iNVGDataPtr.MaxLength();
    
    if (encodedDataLength >= encodedDataMaxLength)
        {
        result = ExpandEncodedData(encodedDataLength);
        if (result != KErrNone)
            {
            User::Leave(result);
            }
        
        iNVGDataPtr.Append((TUint8*)(aData), aLength);
        }
    else
        {
        iNVGDataPtr.Append((TUint8*)(aData), aLength);
        }    
    }

TInt CNVGIconData::EncodeData(const TAny *aData, TUint aLength)
    {
    TInt result = KErrNone;
    
    TInt encodedDataLength      = iNVGDataPtr.Length() + aLength;
    TInt encodedDataMaxLength   = iNVGDataPtr.MaxLength();
    
    if (encodedDataLength >= encodedDataMaxLength)
        {
        result = ExpandEncodedData(encodedDataLength);
        if (result == KErrNone)
            {
            iNVGDataPtr.Append((TUint8*)(aData), aLength);
            }
        }
    else
        {
        iNVGDataPtr.Append((TUint8*)(aData), aLength);
        }
    
    return result;
    }

TInt CNVGIconData::ExpandEncodedData(TUint aNewLength)
    {
    TInt result = KErrNone;
    
    TInt encodedDataMaxLength   = iNVGDataPtr.MaxLength();
    TUint granularities = ((aNewLength - encodedDataMaxLength) / KEncodedDataGranularity) + 1;
    
    HBufC8 * tmpBuf = HBufC8::New(encodedDataMaxLength + granularities * KEncodedDataGranularity);
    if (tmpBuf == 0)
        {
        result = KErrNoMemory;
        }
    else
        {
        TPtr8 tmpBufPtr (tmpBuf->Des());
        tmpBufPtr.Copy(*iNVGData);
        
        delete iNVGData;
        iNVGData = tmpBuf;
        iNVGDataPtr.Set(iNVGData->Des());
        }
    
    return result;
    }

void CNVGIconData::BeginRead()
    {
    iDataLength = iNVGDataPtr.Length();
    iTotalRead = 0;
    iReadStream = (TUint8 *)iNVGDataPtr.Ptr();
    }

void CNVGIconData::EndRead()
    {
    }

#define STR_TO_OTHER_DIR(TOTYPE) do {\
                                TOTYPE data = *(TOTYPE *)&iReadStream[iTotalRead];\
                                iTotalRead += sizeof(TOTYPE);\
                                return data;\
                           } while (0)


#define STR_TO_OTHER_IDIR(TOTYPE) do {\
                                TOTYPE data;\
                                TUint8 * dataPtr = (TUint8 *)&data;\
                                for (TInt i = 0; i < sizeof(TOTYPE); ++i)\
                                    {\
                                    dataPtr[i] = iReadStream[iTotalRead+i];\
                                    }\
                                iTotalRead += sizeof(TOTYPE);\
                                return data;\
                            } while (0)

#define STR_TO_OTHER(TOTYPE) do {\
                                CheckOutOfBoundL(sizeof(TOTYPE));\
                                if (reinterpret_cast<int>(&iReadStream[iTotalRead]) & (sizeof(TOTYPE) - 1))\
                                    {\
                                    STR_TO_OTHER_IDIR(TOTYPE);\
                                    }\
                                else\
                                    {\
                                    STR_TO_OTHER_DIR(TOTYPE);\
                                    }\
                            } while (0)
    

TInt16 CNVGIconData::ReadInt16L()
    {
    STR_TO_OTHER(TInt16);
    }

TInt32 CNVGIconData::ReadInt32L()
    {
    STR_TO_OTHER(TInt32);
    }

TInt8 CNVGIconData::ReadInt8L()
    {
    CheckOutOfBoundL(sizeof(TInt8));
    STR_TO_OTHER_DIR(TInt8);
    }

TReal32 CNVGIconData::ReadReal32L()
    {
    STR_TO_OTHER(TReal32);
    }

TReal64 CNVGIconData::ReadReal64L()
    {
    CheckOutOfBoundL(sizeof(TReal64));
    STR_TO_OTHER_IDIR(TReal64);
    }

void CNVGIconData::ReadL(TDes8 &aDes, TInt aLength)
    {
    CheckOutOfBoundL(aLength);
    aDes.Copy(&iReadStream[iTotalRead], aLength);
    iTotalRead += aLength;
    }

void CNVGIconData::ReadL(TUint8 *aPtr, TInt aLength)
    {
    CheckOutOfBoundL(aLength);
    memcpy(aPtr, &iReadStream[iTotalRead], aLength);
    iTotalRead += aLength;
    }

void CNVGIconData::SkipL(TInt aLength)
    {
    CheckOutOfBoundL(aLength);
    iTotalRead += aLength;
    }
