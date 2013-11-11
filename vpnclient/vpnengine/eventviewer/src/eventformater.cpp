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
* Description: Event formatting services.
*
*/


#include "eventviewer2.h"

#define KEventBufferSizeIncrement  1000
#define KConvTypeU    1
#define KConvTypeN    2
#define KConvTypeA    3


///////////////////////////////////////////////////////////////////
//  HBufC* aDestBuf = FormatEvent(TInt aDescrCount, TDesc8 aDescrList,
//  aEventString)
//
//  This function formats an event string loaded from the localisation
//  file by replacing the conversion specification elements with the data
//  description parameters available in a log event read from the 
//  log file.
//
//  -- aDestBuf = the result of formatting. The caller have to release
//     the buffer
//  -- aDescrCount = number of data description elements (lth/data
//     pairs) in aDescrList.
//  -- aDescrList  = LTH, LTH, ... , Data , Data, ...
//  -- aEventString = text containing 0 - 20 conversion specification
//     elements. The format of conversion specification elements is
//     as follows: %N %U or %iN %iU, where % starts a specification,
//     N is numeric data, U is Unicode data, i is an optional sequence
//     number of description used with this specification (1,2,3...).
//
///////////////////////////////////////////////////////////////////
HBufC* CEventViewer::FormatEvent(TInt   aDescrCount,
                                 const TDesC8& aDescrList,
                                 const TDesC&  aEventString)
    {
    TInt  err;
    TInt  beginOffset;
    TInt  convSpecOffset;
    TInt  continueToLoop;
    TInt  descrNumber;
    TInt  currDescrNumber;
    TUint32 convType;
    TUint32 seqNumber;
    HBufC*  descrBuf = 0;
    
    // Allocate a buffer for the result data. This buffer will
    // contain the modified event string. The buffer may be extended
    // during the operation.

    iResultBuf = HBufC::New( KEventBufferSizeIncrement);
    if (iResultBuf == 0)
        {
        return iResultBuf;
        }
    //
    // Prepare to process an event string 
    //
    beginOffset = 0;
    err         = KErrNone;
    continueToLoop = ETrue;
    currDescrNumber = 0;
    //
    // Loop here until all conversion specifications of an event string
    // have been processed
    //
    while (continueToLoop)
        {

    //  
    // Locate a conversion specification element (It begins with '%'
    //

        convSpecOffset = aEventString.Mid(beginOffset).Locate('%');

    //
    // If no prefix (%) found, copy the last or the only part
    // of text to buffer
    //

        if (convSpecOffset == KErrNotFound)
            {
            TInt lth = aEventString.Length();
            if (lth > beginOffset)
                {
                TPtrC tempPtr = aEventString.Mid(beginOffset, lth - beginOffset);
                err = BufferAppend(iResultBuf,
                                   tempPtr);
                }
            continueToLoop = EFalse;
            continue;
            }
        
    //  
    // Copy the part preceding the conv spec to the destination buffer
    //
        TPtrC tempPtr = aEventString.Mid(beginOffset, convSpecOffset);
        err = BufferAppend(iResultBuf,
                           tempPtr);
        if (err != KErrNone)
            {
            continueToLoop = EFalse;
            continue;
            }

    //  
    // Analyse the conversion specification element
    //
        beginOffset += convSpecOffset + 1;
        err = AnalyseConvSpec(
                                 aEventString,
                                 beginOffset,
                                 convType,
                                 seqNumber);
        if (err != KErrNone)
            {
            continueToLoop = EFalse;
            continue;
            }

    //
    // Define the seq number of the descriptor element
    //
    
        if (seqNumber == 0) {
            currDescrNumber++,                 // no number in conv spec element
            descrNumber = currDescrNumber;
            }
        else
            {
            descrNumber = seqNumber;   // number available in conv spec element
            }

    //
    // Find descritptor from the descriptor list and return the data
    //
        descrBuf = 0;
        descrBuf = GetDescriptorData (
                                   aDescrCount,
                                   aDescrList,
                                   convType,
                                   descrNumber);
        if (descrBuf == 0) 
            {
            continue;
            }
    //
    // Store the data got from the descriptor list to the
    // destination  buffer
    //

        err = BufferAppend(iResultBuf,
                           descrBuf->Des());
        if (err != KErrNone)
            {
            continueToLoop = EFalse;
            continue;
            }
        delete descrBuf;
        descrBuf = 0;
        beginOffset++;
        }
    //
    // While loop completed
    //
    delete descrBuf;
    return iResultBuf;
    }
///////////////////////////////////////////////////////////////////
//  GetDescriptorData - Searches the requested descriptor and
//  returns the data.
//
//  -- returns: converted data in heap buffer. Caller must delete the
//     buffer. Null if no descriptor returned.   
//
//  -- aDescrCount defines the number of LTH/DATA pairs 
//
//  --aDescrList has the following format:
//     LTH LTH ... DATA DATA ..., where
//     --  LTH is TUint32 length of corresponding DATA
//     --  DATA is either 32-bit integer or 8-bit byte strings.
//
//  --aConvType has value KConvTypeN (integer to desimal conversion
//    needed) or KConvTypeU (Unicode data)
//    or KConvTypeA (8-bits data)
//
//  -- aDescrNumber is the sequence number of LTH/DATA pair (1, 2..)
//
//
///////////////////////////////////////////////////////////////////
HBufC* CEventViewer::GetDescriptorData (
                                  TUint32 aDescrCount,
                                  const TDesC8&  aDescrList,
                                  TUint32 aConvType,
                                  TUint32 aDescrNumber)

    {
    HBufC* descrDataBuf = 0;
    TBuf8<4> lthBuf;
    TUint lth = 0;;

    //
    // Check that descriptor number is valid
    //
    if (aDescrNumber > aDescrCount ||  aDescrNumber == 0)
        {
        return descrDataBuf;      // return Null
        }

    TInt dataOffset = 0;
    TInt lthOffset  = 0;
    TUint32 currDescrNumber = 1;

    //
    // Search the requested LTH/DATA pair 
    //
    TInt fillerLth = 0;
    while (currDescrNumber <= aDescrNumber)
        {
        lthBuf.Copy(&aDescrList[lthOffset],4);   // Copy lth to 4 byte buffer
        lth = *(TInt*) (lthBuf.Ptr());
        dataOffset += 4;
        fillerLth = 0;
        if (currDescrNumber < aDescrNumber)
            {
            dataOffset += lth;
            if (lth % 4 != 0)
                {
                fillerLth = 4 - (lth % 4);
                }
            dataOffset += fillerLth;         // Remainder is filler count in data
            }
        lthOffset += 4;
        currDescrNumber++;
        }
    dataOffset += 4 * (aDescrCount - (currDescrNumber - 1)); // rest of lth fields
    
    // 
    // Convert integer to character string
    //

    if (aConvType == KConvTypeN)
        {
        TInt intValue;
        if (lth != 4)
            {
            return descrDataBuf;      // return 0, because wrong size for integer
            }
        descrDataBuf = HBufC::New(16);
        if (descrDataBuf !=0)
            {
            TPtr descrDataPtr(descrDataBuf->Des());
            TPtrC8 ptr8 = aDescrList.Mid(dataOffset, 4);
            intValue = *(TInt*) (ptr8.Ptr());
            descrDataPtr.Num(intValue);
            }
        return descrDataBuf;
        }
    else if (aConvType == KConvTypeA)

    //
    // ConvType == A, convert 8-bit string to unicode   
    //  
        {
        descrDataBuf = HBufC::New(lth);
        if (descrDataBuf == 0)
            {
            return descrDataBuf;
            }
        TPtr descrDataPtr(descrDataBuf->Des());
        TPtrC8 ptr8 = aDescrList.Mid(dataOffset, lth);
        descrDataPtr.Copy(ptr8);
        return descrDataBuf;
        }
    else

    //
    // ConvType == U, unicode string, only copy data
    //  
        {
        descrDataBuf = HBufC::New(lth/2);
        if (descrDataBuf == 0)
            {
            return descrDataBuf;
            }
        TPtr descrDataPtr(descrDataBuf->Des());
        TPtrC8 ptr8 = aDescrList.Mid(dataOffset, lth);
        TPtrC ptr16(reinterpret_cast<const TUint16*>(ptr8.Ptr()), ptr8.Length() / 2);;
        descrDataPtr.Copy(ptr16);
        return descrDataBuf;
        }
    }
        
///////////////////////////////////////////////////////////////////
//  AnalyseConvSpec - Analyse Conversion Specification string
//  The format of string is as follows:
//  %U or %N or %A or %sU or %sN or %sA where:
//  --  % is prefix
//  --  U means Unicode (16-bits) data
//  --  A means 8-bits data
//  --  N means numeric data
//  --  s means 1 or 2 character sequence number    
///////////////////////////////////////////////////////////////////
TInt CEventViewer::AnalyseConvSpec(
                               const TDesC&    aSourceBuf,
                               TInt&     aOffset,         // input/output
                               TUint32&  aConvType,       // U or N
                               TUint32&  aSeqNumber)
    {
    TInt currOffset = aOffset;
    TUint32 seqNumber = 0;
    TUint32 convType;
    TInt   sourceDataLeft = aSourceBuf.Length() -  aOffset;
    TBuf<2> seqNumberBuf;

    // One byte data should exist
    
    if (sourceDataLeft < 1)
        {
        return KErrNotFound;
        }

    // Check if 1 or 2 byte long sequence number follows. Value
    // should be 1 - 20.
    
    if (aSourceBuf[currOffset] >= '0' && 
          aSourceBuf[currOffset] <= '9')
        {
        seqNumberBuf.Copy(&aSourceBuf[currOffset], 1);
        currOffset++;
        sourceDataLeft--; 
        
        if (aSourceBuf[currOffset] >= '0' && 
            aSourceBuf[currOffset] <= '9')
            {
            seqNumberBuf.Append(&aSourceBuf[currOffset], 1);
            currOffset++;
            sourceDataLeft--;
            }
        TLex seqNumberLex(seqNumberBuf);
        seqNumberLex.Val(seqNumber, EDecimal);
        if (seqNumber == 0 || seqNumber > 20)
            {
            return KErrNotFound;
            }
        }

    // One byte data should exist

    if (sourceDataLeft < 1)
        {
        return KErrNotFound;
        }

    // Check if the conversion type is U


    if (aSourceBuf[currOffset] == 'U' || 
         aSourceBuf[currOffset] == 'u')
        {
        convType = KConvTypeU;
        } 

    // Check if the conversion type is N

    else if
        (aSourceBuf[currOffset] == 'N' || 
          aSourceBuf[currOffset] == 'n')
        {
        convType = KConvTypeN;
        }
    
    // Check if the conversion type is A

    else if
        (aSourceBuf[currOffset] == 'A' || 
          aSourceBuf[currOffset] == 'a')
        {
        convType = KConvTypeA;
        }
    else
        {
        return KErrNotFound;
        }
    
    // Return parameters to caller

    aConvType   = convType;
    aOffset     = currOffset;
    aSeqNumber  = seqNumber;
    return KErrNone;
    }



///////////////////////////////////////////////////////////////
// BufferAppend()
// This function appends a buffer by storing a new string.
// If the buffer does not have space enough, the function extends
// the buffer.
//
///////////////////////////////////////////////////////////////
TInt CEventViewer::BufferAppend(HBufC*& aDestBuf,
                                const TDesC& aString)
    {
    TInt err = KErrNone;
    // Make sure that we have enough space for the new text

    TInt spaceLeft = aDestBuf->Des().MaxLength() - aDestBuf->Des().Length();
    if (aString.Length() > spaceLeft)
        {
    // Allocate enough space for the new string + some additional
    // free space so that allocations are not too frequent

        TInt newMaxLength = aDestBuf->Des().MaxLength() + aString.Length() + KEventBufferSizeIncrement;
        HBufC* tempBuf = aDestBuf->ReAlloc(newMaxLength);
        if (tempBuf != NULL)
            {
            aDestBuf = tempBuf;
            }
        else
            {
            err = KErrNoMemory;
            }
        }
    //
    // Store current string to the buffer
    //
    aDestBuf->Des().Append(aString);
    return err;
    }



