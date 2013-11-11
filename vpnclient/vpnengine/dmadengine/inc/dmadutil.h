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
* Description: Class definition of TDmAdUtil.
*
*/



#ifndef __DMADUTIL_H__
#define __DMADUTIL_H__

#include <smldmadapter.h>
#include "dmadengineexportdef.h"
#include "dmadcallback.h"
#include "dmadstoreapi.h"

_LIT8(KDmAdUriDotSlash, "./");
_LIT8(KDmAdSeparator, "/");
_LIT8(KDmAdMimeTypeTextPlain, "text/plain");
_LIT8(KDmAdLocallyCreatedRtNodeUriSegPrefix, "cli");

//------------------------------------------------------------------------------------------------
// TDmAdUtil
//------------------------------------------------------------------------------------------------
class TDmAdUtil
    {
public:
    DMAD_IMPORT_C static TInt DesToInt(const TDesC& aDes);
    DMAD_IMPORT_C static TInt DesToInt(const TDesC8& aDes);
    DMAD_IMPORT_C static HBufC* IntToDesLC(const TInt aInt);
    DMAD_IMPORT_C static HBufC8* IntToDes8LC(const TInt aInt);
    DMAD_IMPORT_C static HBufC* IntToDesLC(const TInt32 aInt32);
    DMAD_IMPORT_C static HBufC8* IntToDes8LC(const TInt32 aInt32);
    DMAD_IMPORT_C static TUint DesToUint(const TDesC& aDes);
    DMAD_IMPORT_C static TUint DesToUint(const TDesC8& aDes);
    DMAD_IMPORT_C static HBufC* IntToDesLC(const TUint aUint);
    DMAD_IMPORT_C static HBufC8* IntToDes8LC(const TUint aUint);
    DMAD_IMPORT_C static HBufC* IntToDesLC(const TUint32 aUint32);
    DMAD_IMPORT_C static HBufC8* IntToDes8LC(const TUint32 aUint32);
    DMAD_IMPORT_C static TPtrC8 LastUriSeg(const TDesC8& aUri);
    DMAD_IMPORT_C static TPtrC8 FirstUriSeg(const TDesC8& aUri);
    DMAD_IMPORT_C static TPtrC8 RemoveLastUriSeg(const TDesC8& aUri);
    DMAD_IMPORT_C static TInt NumOfURISegs(const TDesC8& aUri);
    DMAD_IMPORT_C static TPtrC8 RemoveDotSlash(const TDesC8& aUri);
    DMAD_IMPORT_C static TInt CompareUris(const TDesC8& aUri1, const TDesC8& aUri2);
    DMAD_IMPORT_C static HBufC8* BuildUriLC(const TDesC8& aUriPath, const TDesC8& aUriSeg);
    DMAD_IMPORT_C static HBufC8* BuildUriL(const TDesC8& aUriPath, const TDesC8& aUriSeg);
    DMAD_IMPORT_C static MSmlDmAdapter::TError MapStatusCode(TInt aStatus);
    DMAD_IMPORT_C static void ParseUriLC(const TDesC8& aUri, CArrayFix<TPtrC8>*& aUriSegList);
    DMAD_IMPORT_C static HBufC8* LuidTo8L(const TDesC& aLuid16);
    DMAD_IMPORT_C static HBufC* LuidTo16L(const TDesC8& aLuid8);
    DMAD_IMPORT_C static void FillNodeInfoL(MSmlDmDDFObject&             aNode,
                                            TSmlDmAccessTypes            aAccTypes,
                                            MSmlDmDDFObject::TOccurence  aOccurrence,
                                            MSmlDmDDFObject::TScope      aScope,
                                            MSmlDmDDFObject::TDFFormat   aFormat,
                                            const TDesC8&                aDescription,
                                            TBool                        aObjectGroup,
                                            const TDesC8&                aMimeType=KDmAdMimeTypeTextPlain);
    DMAD_IMPORT_C static MSmlDmDDFObject& AddChildObjectL(MSmlDmDDFObject& aNode, const TDesC8& aNodeName);
    DMAD_IMPORT_C static HBufC8* BuildLocallyCreatedRtNodeUriSegLC(TInt& aLargest);    
    DMAD_IMPORT_C static void BuildRtNodeChildUriListL(MDmAdCallBack*                          aDmAdCallBack,
                                                       MDmAdStoreApi*                          aStoreApi,
                                                       const TDesC8&                           aUri,
                                                       const TDesC8&                           aParentLuid,
                                                       const CArrayFix<TSmlDmMappingInfo>&     aPreviousUriSegmentList,
                                                       CBufBase&                               aCurrentList);


private:
    static TInt FindLargestLocallyCreated(const CArrayFix<TSmlDmMappingInfo>& aPreviousUriSegmentList);
                                                       
    };

#endif
