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


#ifndef CNVGICONDATA_H_
#define CNVGICONDATA_H_

#include <e32base.h>
#include <s32mem.h>


/**
 * @class CNVGIconData
 *        This class is useful when you want to read nd write data from and to memory
 *        This class check for buffer overflow
 *        This is much faster than stream classes provided by symbian
 *        During the ETM traces we found that those classes are 3 FPS less than these utility classes
 *        This class will be checking for data alignment
 */
class CNVGIconData : public CBase
    {
private:
    CNVGIconData();
    void ConstructL(TUint aLength);
    void ConstructL(const TDesC8& aBuf);
    
public:
    
    /**
     * Here the user should only read. Will Behave undefinitly if try to write.
     */
    static CNVGIconData * NewL(const TDesC8& aBuf);

    static CNVGIconData * NewL(TInt aLength = 0);
    
    static CNVGIconData * NewLC(const TDesC8& aBuf);

    static CNVGIconData * NewLC(TInt aLength = 0);

    static CNVGIconData * NewL(TUint8 * aBuf, TInt aLength);

    static CNVGIconData * NewLC(TUint8 * aBuf, TInt aLength);

    virtual ~CNVGIconData();
    
    void Set(const TDesC8& aBuf);
    
    void Set(TUint8 * aBuf, TInt aLength);

    TInt EncodeInt8(TUint8 aVal);

    TInt EncodeInt16(TUint16 aVal);

    TInt EncodeInt32(TUint32 aVal);

    TInt EncodeReal32(TReal32 aVal);

    TInt EncodeReal64(TReal64 aVal);
    
    TInt EncodeData(const TAny *aData, TUint aLength);

    void EncodeInt8L(TUint8 aVal);

    void EncodeInt16L(TUint16 aVal);

    void EncodeInt32L(TUint32 aVal);

    void EncodeReal32L(TReal32 aVal);

    void EncodeReal64L(TReal64 aVal);
    
    void EncodeDataL(const TAny *aData, TUint aLength);
    
    TInt ExpandEncodedData(TUint aNewLength);

    const HBufC8 * GetNVGData() const;
    
    const HBufC8 * GetNVGData();
    
    void BeginRead();
    void EndRead();
    
    TInt16 ReadInt16L();
    
    TInt32 ReadInt32L();
    
    TInt8 ReadInt8L();
    
    void ReadL(TDes8 &aDes, TInt aLength);
    
    void ReadL(TUint8 *aPtr, TInt aLength);
    
    TReal32 ReadReal32L();
    
    TReal64 ReadReal64L();

    void SkipL(TInt aLength);
    
    TInt ReadPos();
    
    TBool EOF();
    
    TInt DataLength();
            
private:
    
    void CheckOutOfBoundL(TInt aLength);
    
    HBufC8 *            iNVGData;
    TPtr8               iNVGDataPtr;
    TInt                iTotalRead;
    TInt                iDataLength;
    TUint8 *            iReadStream;
    };

/**
 * @class TDereferencer
 *        This class is useful when you want to dereference character data
 *        This class check for buffer overflow
 *        This is much faster than stream classes provided by symbian
 *        During the ETM traces we found that those classes are 3 FPS less than these utility classes
 *        This class won't be checking for data alignment.
 */
class TDereferencer
    {
public:
    
    TDereferencer(TUint8 * aBuf, TInt aLength);
    
    TDereferencer(const TDesC8& aBuf);
    
    /**
     *  @fn         SkipL
     *              Advances the read pointer with the given length
     */void SkipL(TInt aLength);

    /**
     * @fn          DerefInt16L
     *              Converts current position to TInt16 *. The caller should take care of alignment
     *              The advantage of this method is that you don't have to copy the data
     *              Bound check will happen
     * @param       aAt dereference from this position
     * @exception   Leaves if data size is not sufficient       
     */
    TInt16 DerefInt16L(TInt aAt = 0);
    operator TInt16();
    
    /**
     * @fn          DerefInt32L
     *              Converts current position to TInt32 *. The caller should take care of alignment
     *              The advantage of this method is that you don't have to copy the data
     *              Bound check will happen
     * @param       aAt dereference from this position
     * @exception   Leaves if data size is not sufficient       
     */
    TInt32 DerefInt32L(TInt aAt = 0);
    operator TInt32();
    
    /**
     * @fn          DerefInt8L
     *              Converts current position to TInt8 *. The caller should take care of alignment
     *              The advantage of this method is that you don't have to copy the data
     *              Bound check will happen
     * @param       aAt dereference from this position
     * @exception   Leaves if data size is not sufficient       
     */
    TInt8 DerefInt8L(TInt aAt = 0);
    operator TInt8();

    /**
     * @fn          DerefInt8ArrayL
     *              Converts current position to TUint8 *. The caller should take care of alignment
     *              The advantage of this method is that you don't have to copy the data
     *              Bound check will happen
     * @param       aLength making safe to dereference for this size
     * @param       aAt dereference from this position
     * @exception   Leaves if data size is not sufficient       
     */
    TUint8 * DerefInt8ArrayL(TInt aLength, TInt aAt = 0);
       
    /**
     * @fn          ReadReal32L
     *              Converts current position to TReal32 *. The caller should take care of alignment
     *              The advantage of this method is that you don't have to copy the data
     *              Bound check will happen
     * @param       aAt dereference from this position
     * @exception   Leaves if data size is not sufficient       
     */
    TReal32 DerefReal32L(TInt aAt = 0);
    operator TReal32();
    
    /**
     * @fn          ReadReal64L
     *              Converts current position to TReal64 *. The caller should take care of alignment
     *              The advantage of this method is that you don't have to copy the data
     *              Bound check will happen
     * @param       aAt dereference from this position
     * @exception   Leaves if data size is not sufficient
     */
    TReal64 DerefReal64L(TInt aAt = 0);
    operator TReal64();

    /**
     * @fn          IsSafeL
     *              Checks whether dereferencing is safe for the given length from the given offset
     * @param       aLength length for dereferencing
     * @param       aAt offset from where the dereferencing should start
     */
    void IsSafeL(TInt aLength, TInt aAt = 0);

    /**
     * @fn          GetPtr
     *              Retrieves the ptr to the data stream
     */
    TPtr8 GetPtr();

    /**
     * Retrieves the silze of the buffer
     */
    TInt GetLength();
    
    /**
     * Returns current reading position
     */
    TInt GetReadingPos();

private:
    void CheckOutOfBoundL(TInt aLength);

    TInt                iTotalRead;
    TInt                iDataLength;
    TUint8 *            iReadStream;
    };

#include "NVGIconData.inl"

#endif
