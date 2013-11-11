/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Policy importer
*
*/



#include "policypatcher.h"
#include "vpnapidefs.h"


TAny* TPatchKeyArrayPtr::At(TInt aIndex) const
    {
    if (aIndex==KIndexPtr)
        {
        TAny* base = *(TUint8**)iPtr;
        CPolicyPatchInfo* info = (CPolicyPatchInfo*)base;
        return (TAny*)&(info->iPatchOffset);
        }
    else
        {
        TAny* base = *(TUint8**)iBase->Ptr(aIndex*sizeof(TUint8**)).Ptr();
        CPolicyPatchInfo* info = (CPolicyPatchInfo*)base;
        return (TAny*)&(info->iPatchOffset);
        }
    }


CPolicyPatcher* CPolicyPatcher::NewL()
    {
    CPolicyPatcher* self = new (ELeave) CPolicyPatcher();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CPolicyPatcher::CPolicyPatcher()
    {
    }

CPolicyPatcher::~CPolicyPatcher()
    {
    }

void CPolicyPatcher::ConstructL()
    {
    }

/*
 * The policy file is patched as follows:
 * - CA certificate references are changed from filenames
 *   into CA subject names (correspondingly, the reference
 *   type is changed from BIN to NAME)
 * - User private key and certificate references are not
 *   removed although they are no longer in use
 */
HBufC8* CPolicyPatcher::PatchPolicyL(const TDesC8& aPolicyData,
                                     CPolicyPatchInfoList* aPatchInfoList)
    {
    iInput.Set(aPolicyData);
    iPatchInfoList = aPatchInfoList;

    FindOffsetsL();

    // Sort the resulting array to ascending order, using key
    // patchInfo->iPatchOffset
    TPatchKeyArrayPtr sortedCerts(_FOFF(CPatchData, iPatchOffset), ECmpTInt32);
    iPatchInfoList->Sort(sortedCerts);

    return CreateOutputL();
    }

void CPolicyPatcher::FindOffsetsL()
    {
    TInt formatValueOffset = KErrGeneral;

    for (TInt i = 0; i < iPatchInfoList->Count(); i++)
        {
        CPolicyPatchInfo* patchInfo = iPatchInfoList->At(i);

        TInt fileNameOffset = iInput.FindF(patchInfo->iCertFileName);

        if (fileNameOffset != KErrNotFound)
            {
            TPtrC8 precedingInput = iInput.Left(fileNameOffset);

            if (0 == patchInfo->UserCertKeyLen())
                {
                // CA cert
                formatValueOffset = precedingInput.LocateReverseF(KBinChar);
                }
            else
                {
                // User cert
                formatValueOffset = precedingInput.LocateReverseF(KFormatChar);
                }

            if (formatValueOffset != KErrNone)
                {
                patchInfo->iPatchOffset = formatValueOffset;
                }
            else
                {
                User::Leave(KVpnErrInvalidPolicyFile);
                }

            patchInfo->iReminderOffset = fileNameOffset + patchInfo->iCertFileName.Length();
            }
        }
    }

HBufC8* CPolicyPatcher::CreateOutputL()
    {
    TInt patchInfoCount = iPatchInfoList->Count();

    CPatchDataList* patchDataList = new (ELeave) CPatchDataList(2);
    CleanupStack::PushL(patchDataList);

    for (TInt i = 0; i < patchInfoCount; i++)
        {
        CPolicyPatchInfo* patchInfo = iPatchInfoList->At(i);

        if (patchInfo->iPatchOffset != KUndefinedOffset)
            {
            CPatchData* patchData = new (ELeave) CPatchData;
            CleanupStack::PushL(patchData);

            patchData->iPatchOffset = patchInfo->iPatchOffset;
            patchData->iReminderOffset = patchInfo->iReminderOffset;
            patchData->iData = CreatePatchDataL(patchInfo);

            patchDataList->AppendL(patchData);

            CleanupStack::Pop(); // patchData (now owned by the list)
            }
        }

    TInt outputLength = iInput.Length();
    TInt patchDataCount = patchDataList->Count();

    for (TInt j = 0; j < patchDataCount; j++)
        {
        outputLength += patchDataList->At(j)->iData->Length();
        }

    HBufC8* output = HBufC8::NewL(outputLength);
    TPtr8 ptrOutput = output->Des();

    if (patchDataCount == 0)
        {
        ptrOutput.Append(iInput);
        }
    else
        {
        // Append all patches and the input data before,
        // between and after them to the outout
        for (TInt k = 0; k < patchDataCount; k++)
            {
            TInt startOffset;

            // If this is the first patch...
            if (k == 0)
                {
                // ...there is no previous patch whose
                // reminder offset we could use
                startOffset = 0;
                }
            else
                {
                // There is a previous patch whose reminder offset we can use
                startOffset = patchDataList->At(k - 1)->iReminderOffset;
                }

            CPatchData* currPatchData = patchDataList->At(k);

            TInt endOffset = currPatchData->iPatchOffset;

            // Append to the output the input data between the previous
            // patch (or the beginning of input if this is the first patch)
            // and the current patch
            ptrOutput.Append(iInput.Mid(startOffset, endOffset - startOffset));

            // Append the current patch data to the output
            ptrOutput.Append(*(currPatchData->iData));

            // If this is the last patch...
            if (k == (patchDataCount -1))
                {
                if(EFalse != iUserCertPatched)
                    {
                    // The data after user cert patch is not needed.
                    // Append new line instead.
                    // In practise this removes the PRIVATE_KEY_FORMAT
                    // and PRIVATE_KEY_DATA lines.
                    ptrOutput.Append(KNewLine);
                    }
                else
                    {
                    // ...append to the output the input data between the
                    // last patch and the end of the input
                    ptrOutput.Append(iInput.Mid(currPatchData->iReminderOffset));
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(); // patchDataList

    return output;
    }

HBufC8* CPolicyPatcher::CreatePatchDataL(const CPolicyPatchInfo* aPatchInfo)
    {
    TInt patchDataLength = 0;

    TBuf8<20> keyBuf;

    const TDesC8& certSubjectName = aPatchInfo->CertSubjectName();

    if (0 == aPatchInfo->UserCertKeyLen())
        {
        // CA cert patch
        patchDataLength = KName().Length() + KNewLine().Length() +
                          KDataField().Length() + KSpace().Length() +
                          certSubjectName.Length();
        }
    else
        {
        // user cert patch
        keyBuf.Num(aPatchInfo->UserCertKeyLen());
        patchDataLength = KDNField().Length() + certSubjectName.Length() +
                          KNewLine().Length() + KKeyLenField().Length() +
                          keyBuf.Length();
        }

    HBufC8* patchData = HBufC8::NewL(patchDataLength);
    CleanupStack::PushL(patchData);

    TPtr8 ptrPatchData(patchData->Des());

    if (0 == aPatchInfo->UserCertKeyLen())
        {
        // CA cert patch
        ptrPatchData.Append(KName);
        ptrPatchData.Append(KNewLine);
        ptrPatchData.Append(KDataField);
        ptrPatchData.Append(KSpace);
        ptrPatchData.Append(certSubjectName);
        }
    else
        {
        // User cert patch
        ptrPatchData.Append(KKeyLenField);
        ptrPatchData.Append(keyBuf);
        ptrPatchData.Append(KNewLine);
        ptrPatchData.Append(KDNField);
        ptrPatchData.Append(certSubjectName);
        iUserCertPatched = ETrue;
        }

    CleanupStack::Pop(); // patchData

    return patchData;
    }


// CPolicyPatchInfo

CPolicyPatchInfo::CPolicyPatchInfo()
    {
    iCertFileName.SetLength(0);
    iPatchOffset = KUndefinedOffset;
    iReminderOffset = KUndefinedOffset;
    iUserCertKeyLen = 0;
    }

CPolicyPatchInfo::~CPolicyPatchInfo()
    {
    delete iCertSubjectName;
    }

const TDesC8& CPolicyPatchInfo::CertSubjectName() const
    {
    return *iCertSubjectName;
    }

void CPolicyPatchInfo::SetCertSubjectNameL(const TDesC8& aCertSubjectName)
    {
    delete iCertSubjectName;
    iCertSubjectName = NULL;
    HBufC8* certSubjectBuf = CheckSubjectNameSyntaxL(aCertSubjectName);  
    iCertSubjectName = certSubjectBuf;
    }

void CPolicyPatchInfo::SetUserCertKeyLen(TInt aKeyLen)
    {
    iUserCertKeyLen = aKeyLen;
    }

TInt CPolicyPatchInfo::UserCertKeyLen() const
    {
    return iUserCertKeyLen;
    }
    
HBufC8* CPolicyPatchInfo::CheckSubjectNameSyntaxL(const TDesC8& aSubj)
    {
    const TInt KMaxSubjectItems = 20;
    
    _LIT8(KEqualSign, "=");
    _LIT8(KCommaSign, ",");
    _LIT8(KReplacementChar, "\"");
    
    //counts positions for equal sign characters
    CArrayFixFlat<TInt>* equalSignArr=new (ELeave) CArrayFixFlat<TInt> (KMaxSubjectItems); 
    
    //counts positions for comma characters
    CArrayFixFlat<TInt>* commaSignArr=new (ELeave) CArrayFixFlat<TInt> (KMaxSubjectItems); 
    
    //counts positions for double quatiation characters
    CArrayFixFlat<TInt>* updateArr=new (ELeave) CArrayFixFlat<TInt> (KMaxSubjectItems);
    
    TInt subjLth=aSubj.Length();
    TInt equalArrItemCount=0;
    TInt commaArrItemCount=0;
    TInt updateArrCount=0;
    
    CleanupStack::PushL(equalSignArr);
    CleanupStack::PushL(commaSignArr);
    
    //register '=' and ',' character positions 
    for (TInt i=0; i<subjLth; i++)
        {
        if ( aSubj.Mid(i, 1)== KEqualSign )
            {
            equalSignArr->AppendL(i);
            equalArrItemCount++;
            }
        
        if ( aSubj.Mid(i, 1)== KCommaSign )
            {
            commaSignArr->AppendL(i);
            commaArrItemCount++;
            }
        }
    
    CleanupStack::PushL(updateArr);
    
    TInt i=0;
    TInt j=0;
    
    // At least one comma required. Otherwise there is no chance for extra commas
    if ( commaArrItemCount )
        {
        while ( i< equalArrItemCount )
            {
            
            //advance starting position for every iteration
            TInt eqSignStartIndex=i;
            
            TInt cmCount=0;
        
            //check wether there is more than one comma between two adjacent fields.
            while ( commaSignArr->At(j)< equalSignArr->At(eqSignStartIndex) )
                {
                cmCount++;
                
                j++;
                
                if ( j==commaArrItemCount )
                    break;
                }
            
            //at least one extra comma character found. Mark positions for mofifications.
            if (cmCount>1)
                {
                TInt equalPos=equalSignArr->At(eqSignStartIndex-1) + 1;
                updateArr->AppendL(equalPos);
                
                TInt commaPos=commaSignArr->At(j-1);
                updateArr->AppendL(commaPos);
                
                updateArrCount++;
                updateArrCount++;
                }
            i++;    
            }
        }
        
        //Checking also last attribute of Subject Name string
        if ( j<commaArrItemCount )
            {
            updateArr->AppendL(equalSignArr->At(equalArrItemCount-1) + 1);
            updateArr->AppendL(subjLth);
            
            updateArrCount++;
            updateArrCount++;
            }
       
       TBuf8<256> resultBuf;
       resultBuf.Copy(aSubj);
       
       i=0;       
       
       //update subjectname acoording to updateArr array.
       if ( updateArr->Count()>0 )
           {
           while (i<updateArrCount)
               {
               if ( resultBuf.Length()<256 )
                   resultBuf.Insert(updateArr->At(i) + i, KReplacementChar);
               else
                   User::Leave(KErrNotSupported);
               i++;
               }
           }
       
       CleanupStack::Pop(updateArr);
       CleanupStack::Pop(commaSignArr);
       CleanupStack::Pop(equalSignArr);
       
       delete equalSignArr;
       equalSignArr=NULL;
       
       delete commaSignArr;
       commaSignArr=NULL;
       
       delete updateArr;
       updateArr=NULL;
       
       HBufC8* resultHeap = resultBuf.AllocL();
       
       return resultHeap;
    }


// CPolicyPatchInfoList

CPolicyPatchInfoList::CPolicyPatchInfoList(TInt aGranularity)
    : CArrayPtrFlat<CPolicyPatchInfo>(aGranularity)
    {
    }

CPolicyPatchInfoList::~CPolicyPatchInfoList()
    {
    ResetAndDestroy();
    }


// CPatchData

CPatchData::~CPatchData()
    {
    delete iData;
    }


// CPatchDataList

CPatchDataList::CPatchDataList(TInt aGranularity)
    : CArrayPtrFlat<CPatchData>(aGranularity)
    {
    }

CPatchDataList::~CPatchDataList()
    {
    ResetAndDestroy();
    }


/***/
