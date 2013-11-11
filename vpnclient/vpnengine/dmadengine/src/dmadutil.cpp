/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of TDmAdUtil.
*
*/



#include <utf.h>
#include "dmadutil.h"
#include "vpnlogger.h"

//-----------------------------------------------------------------------   

DMAD_EXPORT_C TInt TDmAdUtil::DesToInt(const TDesC& aDes)
    {
    if (aDes.Length() == 0)
        {
        return 0;
        }
    TLex16 lex(aDes);
    TInt value = 0;
    if (lex.Val(value) != KErrNone)
        {
        ;
        }
    return value;
    }

DMAD_EXPORT_C TInt TDmAdUtil::DesToInt(const TDesC8& aDes)
    {
    if (aDes.Length() == 0)
        {
        return 0;
        }
    TLex8 lex(aDes);
    TInt value = 0;
    if (lex.Val(value) != KErrNone)
        {
        ;
        }
    return value;
    }

DMAD_EXPORT_C HBufC* TDmAdUtil::IntToDesLC(const TInt aInt)
    {
    HBufC* buf = HBufC::NewLC(10); //10 = max length of 32bit integer
    TPtr ptrBuf = buf->Des();
    ptrBuf.Num(aInt);
    return buf;
    }

DMAD_EXPORT_C HBufC8* TDmAdUtil::IntToDes8LC(const TInt aInt)
    {
    HBufC8* buf = HBufC8::NewLC(10); //10 = max length of 32bit integer
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Num(aInt);
    return buf;
    }

DMAD_EXPORT_C HBufC* TDmAdUtil::IntToDesLC(const TInt32 aInt32)
    {
    return IntToDesLC((TInt)aInt32);
    }
    
DMAD_EXPORT_C HBufC8* TDmAdUtil::IntToDes8LC(const TInt32 aInt32)
    {
    return IntToDes8LC((TInt)aInt32);
    }
    
//-----------------------------------------------------------------------
    
DMAD_EXPORT_C TUint TDmAdUtil::DesToUint(const TDesC& aDes)
    {
    if (aDes.Length() == 0)
        {
        return 0;
        }
    TLex16 lex(aDes);
    TUint value = 0;
    if (lex.Val(value) != KErrNone)
        {
        ;
        }
    return value;
    }

DMAD_EXPORT_C TUint TDmAdUtil::DesToUint(const TDesC8& aDes)
    {
    if (aDes.Length() == 0)
        {
        return 0;
        }
    TLex8 lex(aDes);
    TUint value = 0;
    if (lex.Val(value) != KErrNone)
        {
        ;
        }
    return value;
    }

DMAD_EXPORT_C HBufC* TDmAdUtil::IntToDesLC(const TUint aUint)
    {
    HBufC* buf = HBufC::NewLC(10); //10 = max length of 32bit integer
    TPtr ptrBuf = buf->Des();
    ptrBuf.Num(aUint);
    return buf;
    }

DMAD_EXPORT_C HBufC8* TDmAdUtil::IntToDes8LC(const TUint aUint)
    {
    HBufC8* buf = HBufC8::NewLC(10); //10 = max length of 32bit integer
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Num(aUint);
    return buf;
    }
    
DMAD_EXPORT_C HBufC* TDmAdUtil::IntToDesLC(const TUint32 aUint32)
    {
    return IntToDesLC((TUint)aUint32);
    }

DMAD_EXPORT_C HBufC8* TDmAdUtil::IntToDes8LC(const TUint32 aUint32)
    {
    return IntToDes8LC((TUint)aUint32);
    }
    
//-----------------------------------------------------------------------   
    
DMAD_EXPORT_C TPtrC8 TDmAdUtil::LastUriSeg(const TDesC8& aUri)
    {
    TInt i;
    for (i=aUri.Length()-1; i>=0; i--)
        {
        if (aUri[i] == '/')
            {
            break;
            }
        }
    if (i==0)
        {
        return aUri;
        }
    else
        {
        return aUri.Mid(i+1);
        }
    }

DMAD_EXPORT_C TPtrC8 TDmAdUtil::FirstUriSeg(const TDesC8& aUri)
    {
    TInt i;
    TBool found = EFalse;
    for (i=0; i<aUri.Length(); i++)
        {
        if (aUri[i] == '/')
            {
            found = ETrue;
            break;
            }
        }
    if (found)
        {
        return aUri.Left(i);
        }
    else
        {
        return aUri;
        }
    }

DMAD_EXPORT_C TPtrC8 TDmAdUtil::RemoveLastUriSeg(const TDesC8& aUri)
    {
    TInt i;
    for (i=aUri.Length()-1; i>=0; i--)
        {
        if (aUri[i] == '/')
            {
            break;
            }
        }
    return aUri.Left(i);
    }

DMAD_EXPORT_C TInt TDmAdUtil::NumOfURISegs(const TDesC8& aUri)
    {
    TPtrC8 uri(TDmAdUtil::RemoveDotSlash(aUri));
    TInt numOfURISegs = 1;
    for (TInt i=0; i<uri.Length(); i++)
        {
        if (uri[i] == '/')
            {
            numOfURISegs++;
            }
        }
    return numOfURISegs;
    }

DMAD_EXPORT_C TPtrC8 TDmAdUtil::RemoveDotSlash(const TDesC8& aUri)
    {
    if (aUri.Find(KDmAdUriDotSlash) == 0)
        {
        return aUri.Right(aUri.Length() - KDmAdUriDotSlash().Length());
        }
    else
        {
        return aUri;
        }
    }

DMAD_EXPORT_C TInt TDmAdUtil::CompareUris(const TDesC8& aUri1, const TDesC8& aUri2)
    {
    TPtrC8 uri1(TDmAdUtil::RemoveDotSlash(aUri1));
    TPtrC8 uri2(TDmAdUtil::RemoveDotSlash(aUri2));
    return uri1.Compare(uri2);
    }

DMAD_EXPORT_C HBufC8* TDmAdUtil::BuildUriLC(const TDesC8& aUriPath, const TDesC8& aUriSeg)
    {
    HBufC8* uri = BuildUriL(aUriPath, aUriSeg);
    CleanupStack::PushL(uri);
    return uri;
    }
    
DMAD_EXPORT_C HBufC8* TDmAdUtil::BuildUriL(const TDesC8& aUriPath, const TDesC8& aUriSeg)
    {
    HBufC8* uri = HBufC8::NewL(aUriPath.Length() + 1 + aUriSeg.Length());
    TPtr8 uriDesc = uri->Des();
    uriDesc.Copy(aUriPath);
    uriDesc.Append(KDmAdSeparator);
    uriDesc.Append(aUriSeg);
    return uri;
    }

DMAD_EXPORT_C MSmlDmAdapter::TError TDmAdUtil::MapStatusCode(TInt aStatus)
    {
    TRACE("TDmAdUtil::MapStatusCode");
    
    
    MSmlDmAdapter::TError dmStatus;
    if (aStatus == KErrNone)
        {
        dmStatus = MSmlDmAdapter::EOk;
        }
    else if (aStatus == KErrNotFound)
        {
        dmStatus = MSmlDmAdapter::ENotFound;
        }
    else if (aStatus == KErrCorrupt)
        {
        dmStatus = MSmlDmAdapter::EInvalidObject;
        }
    else if (aStatus == KErrAlreadyExists)
        {
        dmStatus = MSmlDmAdapter::EAlreadyExists;
        }
    else if (aStatus == KErrTooBig)
        {
        dmStatus = MSmlDmAdapter::ETooLargeObject;
        }
    else if (aStatus == KErrDiskFull)
        {
        dmStatus = MSmlDmAdapter::EDiskFull;
        }
    /*
                ERollbackFailed,
                EObjectInUse,
                ENoMemory,
                ECommitOK,
                ERollbackOK,
                ECommitFailed
    */
    else
        {
        dmStatus = MSmlDmAdapter::EError;
        }
    return dmStatus;
    }

DMAD_EXPORT_C void TDmAdUtil::ParseUriLC(const TDesC8& aUri, CArrayFix<TPtrC8>*& aUriSegList)
    {
    TPtrC8 uri(TDmAdUtil::RemoveDotSlash(aUri));
    
    CArrayFix<TPtrC8>* uriSegList;
    uriSegList = new (ELeave) CArrayFixFlat<TPtrC8>(8);
    CleanupStack::PushL(uriSegList);

    TPtrC8 seg;
    TPtrC8 curr(uri);

    while (curr.Length() > 0)
        {
        TInt offset = curr.Locate('/');
        if (offset == KErrNotFound)
            {
            seg.Set(curr);
            curr.Set(KNullDesC8);
            }
        else
            {
            seg.Set(curr.Left(offset));
            TInt rightLth = curr.Length() - offset - 1;
            if (rightLth <= 0)           
                {
                DEBUG_LOG(_L("TDmAdUtil::ParseUriLC: corrupted uri"));
                DEBUG_LOG1(_L8("URI: %S"), &aUri);
                
                User::Leave(KErrGeneral);
                }
            curr.Set(curr.Right(rightLth));
            }
        uriSegList->AppendL(seg);
        }
    
    aUriSegList = uriSegList;
    }

DMAD_EXPORT_C HBufC8* TDmAdUtil::LuidTo8L(const TDesC& aLuid16)
    {
    HBufC8* luid8 = HBufC8::NewL(aLuid16.Length());
    luid8->Des().Copy(aLuid16);
    return luid8;
    }
    
DMAD_EXPORT_C HBufC* TDmAdUtil::LuidTo16L(const TDesC8& aLuid8)
    {
    HBufC* luid16 = HBufC::NewL(aLuid8.Length());
    luid16->Des().Copy(aLuid8);
    return luid16;
    }

//-----------------------------------------------------------------------   
    
DMAD_EXPORT_C void TDmAdUtil::FillNodeInfoL(MSmlDmDDFObject&               aNode,
                                            TSmlDmAccessTypes              aAccTypes,
                                            MSmlDmDDFObject::TOccurence    aOccurrence,
                                            MSmlDmDDFObject::TScope        aScope,
                                            MSmlDmDDFObject::TDFFormat     aFormat,
                                            const TDesC8&                  aDescription,
                                            TBool                          aObjectGroup,
                                            const TDesC8&                  aMimeType)
{
    aNode.SetAccessTypesL(aAccTypes);
    aNode.SetOccurenceL(aOccurrence);
    aNode.SetScopeL(aScope);
    aNode.SetDFFormatL(aFormat);

    if (aMimeType.Length() > 0)
        {
        aNode.AddDFTypeMimeTypeL(aMimeType);
        }

    aNode.SetDescriptionL(aDescription);

    (void)aObjectGroup;
    }

DMAD_EXPORT_C MSmlDmDDFObject& TDmAdUtil::AddChildObjectL(MSmlDmDDFObject& aNode, const TDesC8& aNodeName)
    {
    if (aNodeName.Length() == 0)
        {
        MSmlDmDDFObject& object = aNode.AddChildObjectGroupL();
        return object;
        }
    else
        {
        MSmlDmDDFObject& object = aNode.AddChildObjectL(aNodeName);
        return object;
        }
    }

DMAD_EXPORT_C HBufC8* TDmAdUtil::BuildLocallyCreatedRtNodeUriSegLC(TInt& aLargest)
    {
    ++aLargest;
    HBufC8* number = TDmAdUtil::IntToDes8LC(aLargest);
    HBufC8* uri = HBufC8::NewL(KDmAdLocallyCreatedRtNodeUriSegPrefix().Length() + number->Length());
    uri->Des().Copy(KDmAdLocallyCreatedRtNodeUriSegPrefix);
    uri->Des().Append(*number);
    CleanupStack::PopAndDestroy(); // number
    CleanupStack::PushL(uri);
    return uri;
    }

TInt TDmAdUtil::FindLargestLocallyCreated(const CArrayFix<TSmlDmMappingInfo>& aPreviousUriSegmentList)
    {
    TRACE("TDmAdUtil::FindLargestLocallyCreated");
    TInt largest = 0;
    for (TInt i=0; i < aPreviousUriSegmentList.Count(); i++)
        {
        const TSmlDmMappingInfo& mappingInfo = aPreviousUriSegmentList.At(i);        
        if (mappingInfo.iURISeg.Find(KDmAdLocallyCreatedRtNodeUriSegPrefix) == 0)
            {            
            TPtrC8 numberPart(mappingInfo.iURISeg.Mid(KDmAdLocallyCreatedRtNodeUriSegPrefix().Length()));
            TInt number = TDmAdUtil::DesToInt(numberPart);
            if (number > largest)
                {
                largest = number;
                }
            }
        }
    return largest;
    }
    
    
void PointerArrayCleanup(TAny* item)
    {
    RPointerArray<HBufC8>* a = static_cast<RPointerArray<HBufC8>*>(item);
    a->ResetAndDestroy();
    a->Close();
    }


#define DMAD_DUMP_PREVIOUS_URI_SEGMENT_LIST
    
DMAD_EXPORT_C void TDmAdUtil::BuildRtNodeChildUriListL(MDmAdCallBack*                          aDmAdCallBack,
                                                  MDmAdStoreApi*                          aStoreApi,
                                                  const TDesC8&                           aUri,
                                                  const TDesC8&                           aParentLuid,
                                                  const CArrayFix<TSmlDmMappingInfo>&     aPreviousUriSegmentList,
                                                  CBufBase&                               aCurrentList)
    {
#ifdef DMAD_DUMP_PREVIOUS_URI_SEGMENT_LIST
        DEBUG_LOG(_L("BuildRtNodeChildUriListL:"));
        {
        for (TInt i=0; i < aPreviousUriSegmentList.Count(); i++)
            {
            const TSmlDmMappingInfo& mappingInfo = aPreviousUriSegmentList.At(i);
                        
            DEBUG_LOG1(_L("entry %d:"), i);            
            DEBUG_LOG1(_L8("Uri: %S"), &(mappingInfo.iURISeg));
            DEBUG_LOG_HEX(mappingInfo.iURISegLUID);
            
            }
        }
#endif

    RPointerArray<HBufC8> luidList;
    CleanupStack::PushL(TCleanupItem(PointerArrayCleanup, &luidList));
    
    aStoreApi->LuidListL(aUri, aParentLuid, luidList);


    // Finds largest number used in cli<x> named nodes.
    TInt largest = FindLargestLocallyCreated(aPreviousUriSegmentList);
    DEBUG_LOG1(_L("largest is cli%d"), largest);
    
    TInt countLuidList = luidList.Count();
    for (TInt j=0; j < countLuidList; j++)
        {
        const HBufC8* luidElem = luidList[j];

        HBufC8* uriSeg = 0;
        
        //Tries to find the luid from the aPreviousUriSegmentList
        for (TInt i=0; i < aPreviousUriSegmentList.Count(); i++)
            {
            const TSmlDmMappingInfo& mappingInfo = aPreviousUriSegmentList.At(i);
                        
            if (mappingInfo.iURISegLUID.Compare(*luidElem) == 0)
                {            
                uriSeg = mappingInfo.iURISeg.AllocLC();
                break;
                }
            }
        
        if (uriSeg == 0)
            {
            //Uri was not found from the aPreviousUriSegmentList
            uriSeg = BuildLocallyCreatedRtNodeUriSegLC(largest);
            
            DEBUG_LOG2(_L8("uriSeg %S, largest %d"), uriSeg, largest);
            
            HBufC8* wholeUri = TDmAdUtil::BuildUriLC(aUri, *uriSeg);            
            aDmAdCallBack->SetMappingL(*wholeUri, *luidElem);
            CleanupStack::PopAndDestroy(); //wholeUri
            }

        //If this is not the first element, inserts slash at the beginning
        //of the result list.
        if (j > 0)
            {            
            aCurrentList.InsertL(aCurrentList.Size(), KDmAdSeparator);
            }
        aCurrentList.InsertL(aCurrentList.Size(), *uriSeg);
        
        CleanupStack::PopAndDestroy(); // uriSeg
        }
    
    CleanupStack::PopAndDestroy(); //luidList
    }
