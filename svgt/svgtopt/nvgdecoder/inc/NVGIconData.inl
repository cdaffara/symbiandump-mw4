/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  NVG Decoder header file
*
*/

#ifndef NVGICONDATA_INL_
#define NVGICONDATA_INL_

inline void CNVGIconData::CheckOutOfBoundL(TInt aLength)
    {
    if (iTotalRead + aLength > iDataLength ||
        iTotalRead + aLength < 0)
        {
        User::Leave(KErrEof);
        }
    }

inline TInt CNVGIconData::ReadPos()
    {
    return iTotalRead;
    }

inline TInt CNVGIconData::DataLength()
    {
    return iDataLength;
    }


inline const HBufC8 * CNVGIconData::GetNVGData()
    {
    return iNVGData;
    }

inline TBool CNVGIconData::EOF()
    {
    return (iTotalRead >= iDataLength);
    }

inline TInt CNVGIconData::EncodeInt8(TUint8 aVal)
    {
    return EncodeData(&aVal, sizeof(aVal));
    }

inline TInt CNVGIconData::EncodeInt16(TUint16 aVal)
    {
    return EncodeData(&aVal, sizeof(aVal));
    }

inline TInt CNVGIconData::EncodeInt32(TUint32 aVal)
    {
    return EncodeData(&aVal, sizeof(aVal));
    }

inline TInt CNVGIconData::EncodeReal32(TReal32 aVal)
    {
    return EncodeData(&aVal, sizeof(aVal));
    }

inline TInt CNVGIconData::EncodeReal64(TReal64 aVal)
    {
    return EncodeData(&aVal, sizeof(aVal));
    }

inline void CNVGIconData::EncodeInt8L(TUint8 aVal)
    {
    return EncodeDataL(&aVal, sizeof(aVal));
    }

inline void CNVGIconData::EncodeInt16L(TUint16 aVal)
    {
    return EncodeDataL(&aVal, sizeof(aVal));
    }

inline void CNVGIconData::EncodeInt32L(TUint32 aVal)
    {
    return EncodeDataL(&aVal, sizeof(aVal));
    }

inline void CNVGIconData::EncodeReal32L(TReal32 aVal)
    {
    return EncodeDataL(&aVal, sizeof(aVal));
    }

inline void CNVGIconData::EncodeReal64L(TReal64 aVal)
    {
    return EncodeDataL(&aVal, sizeof(aVal));
    }

inline const HBufC8 * CNVGIconData::GetNVGData() const
    {
    return iNVGData;
    }

inline void CNVGIconData::Set(const TDesC8& aBuf)
    {
    iNVGDataPtr.Set((TUint8 *)aBuf.Ptr(), aBuf.Length(), aBuf.Length());
    }

inline void CNVGIconData::Set(TUint8 * aBuf, TInt aLength)
    {
    iNVGDataPtr.Set(aBuf, aLength, aLength);
    }

inline TDereferencer::TDereferencer(TUint8 * aBuf, TInt aLength)
    : iTotalRead(0),
      iDataLength(aLength),
      iReadStream(aBuf)
    {
    
    }

inline TDereferencer::TDereferencer(const TDesC8& aBuf)
    : iTotalRead(0),
      iDataLength(aBuf.Length()),
      iReadStream((unsigned char *)aBuf.Ptr())
    {        
    }

inline void TDereferencer::CheckOutOfBoundL(TInt aLength)
    {
    if (iTotalRead + aLength > iDataLength ||
        iTotalRead + aLength < 0)
        {
        User::Leave(KErrEof);
        }
    }

inline void TDereferencer::SkipL(TInt aLength)
    {
    CheckOutOfBoundL(aLength);
    iTotalRead += aLength;
    }

#define DEREF_PTR(TOTYPE, Offset, Size) do {\
                                CheckOutOfBoundL(Offset + Size); \
                                return * (TOTYPE *)&iReadStream[iTotalRead + Offset];\
                           } while (0)

inline TReal64 TDereferencer::DerefReal64L(TInt aAt)
    {
    DEREF_PTR(TReal64, aAt, sizeof(TReal64));
    }

inline TReal32 TDereferencer::DerefReal32L(TInt aAt)
    {
    DEREF_PTR(TReal32, aAt, sizeof(TReal32));
    }

inline TUint8 * TDereferencer::DerefInt8ArrayL(TInt aLength, TInt aAt)
    {
    CheckOutOfBoundL(aAt + aLength);
    return (TUint8 *)&iReadStream[iTotalRead + aAt];
    }

inline void TDereferencer::IsSafeL(TInt aLength, TInt aAt)
    {
    CheckOutOfBoundL(aAt + aLength);
    }

inline TInt8 TDereferencer::DerefInt8L(TInt aAt)
    {
    DEREF_PTR(TInt8, aAt, sizeof(TInt8));
    }

inline TInt32 TDereferencer::DerefInt32L(TInt aAt)
    {
    DEREF_PTR(TInt32, aAt, sizeof(TInt32));
    }

inline TInt16 TDereferencer::DerefInt16L(TInt aAt)
    {
    DEREF_PTR(TInt16, aAt, sizeof(TInt16));
    }

inline TDereferencer::operator TReal64()
    {
    return DerefReal64L();
    }

inline TDereferencer::operator TReal32()
    {
    return DerefReal32L();    
    }

inline TDereferencer::operator TInt8()
    {
    return DerefInt8L();    
    }

inline TDereferencer::operator TInt32()
    {
    return DerefInt32L();    
    }

inline TDereferencer::operator TInt16()
    {
    return DerefInt16L();    
    }

inline TPtr8 TDereferencer::GetPtr()
    {
    return TPtr8(iReadStream, iDataLength, iDataLength);
    }

inline TInt TDereferencer::GetLength()
    {
    return iDataLength;
    }

inline TInt TDereferencer::GetReadingPos()
    {
    return iTotalRead;
    }

#endif
