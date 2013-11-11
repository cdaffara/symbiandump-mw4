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
* Description: Pin parser main module.
*
*/



#include <e32base.h>

#include "pinparser.h"
#include "fileutil.h"

TPinParser::TPinParser(TFileUtil& aFileUtil) : iFileUtil(aFileUtil)
    {
    }

void TPinParser::ParsePolicyInfoL(const TFileName& aPinFile, TVpnPolicyInfo& aPolicyInfo)
    {
    HBufC* fileData = iFileUtil.LoadFileDataUL(aPinFile);

    iLexer.Assign(*fileData);

    TPtrC token(NULL, 0);

    while (!iLexer.Eos())
        {
        token.Set(iLexer.NextToken());
        
        ParseSectionInfo(token, aPolicyInfo);
        }

    delete fileData;
    }

void TPinParser::ParsePolicyDetailsL(const TDesC& aPinFile, TVpnPolicyDetails& aPolicyDetails)
    {
    HBufC* fileData = iFileUtil.LoadFileDataUL(aPinFile);

    iLexer.Assign(*fileData);

    TPtrC token(NULL, 0);

    while (!iLexer.Eos())
        {
        token.Set(iLexer.NextToken());
        
        ParseSectionDetails(token, aPolicyDetails);
        }

    delete fileData;
    }
    
void TPinParser::ParseSectionInfo(const TDesC& aSectionTag, TVpnPolicyInfo& aPolicyInfo)
    {
    TPtrC content = GetSectionContent();
    
    if (aSectionTag.CompareF(KNameSection) == 0)
        {
        aPolicyInfo.iName.Copy(content.Left(aPolicyInfo.iName.MaxLength()));
        aPolicyInfo.iName.Trim();
        }
    }

void TPinParser::ParseSectionDetails(const TDesC& aSectionTag, TVpnPolicyDetails& aPolicyDetails)
    {
    TPtrC content = GetSectionContent();
    
    if (aSectionTag.CompareF(KNameSection) == 0)
        {
        aPolicyDetails.iName.Copy(content.Left(aPolicyDetails.iName.MaxLength()));
        aPolicyDetails.iName.Trim();
        }
    if (aSectionTag.CompareF(KDescriptionSection) == 0)
        {
        aPolicyDetails.iDescription.Copy(content.Left(aPolicyDetails.iDescription.MaxLength()));
        aPolicyDetails.iDescription.Trim();
        }
    if (aSectionTag.CompareF(KVersionSection) == 0)
        {
        aPolicyDetails.iVersion.Copy(content.Left(aPolicyDetails.iVersion.MaxLength()));
        aPolicyDetails.iVersion.Trim();
        }
    if (aSectionTag.CompareF(KIssuerNameSection) == 0)
        {
        aPolicyDetails.iIssuerName.Copy(content.Left(aPolicyDetails.iIssuerName.MaxLength()));
        aPolicyDetails.iIssuerName.Trim();
        }
    if (aSectionTag.CompareF(KContactSection) == 0)
        {
        aPolicyDetails.iContactInfo.Copy(content.Left(aPolicyDetails.iContactInfo.MaxLength()));
        aPolicyDetails.iContactInfo.Trim();
        }
    }
    
TPtrC TPinParser::GetSectionContent()
    {
    iLexer.Mark();
    
    while (!iLexer.Eos())
        {
        iLexer.Inc();

        if (iLexer.Eos() || (iLexer.Peek() == KSectionBeginChar))
            {
            break;
            }
        }

    return iLexer.MarkedToken();
    }

HBufC* TPinParser::PolicyDetailsAsTextL(const TVpnPolicyDetails& aPolicyDetails)
    {
    HBufC* text = HBufC::NewL(KInitialDetailsTextLength);
    CleanupStack::PushL(text);

    SmartAppendL(text, KNameSection());
    SmartAppendL(text, KNewLine());
    SmartAppendL(text, aPolicyDetails.iName);
    SmartAppendL(text, KNewLine());
    
    SmartAppendL(text, KDescriptionSection());
    SmartAppendL(text, KNewLine());
    SmartAppendL(text, aPolicyDetails.iDescription);
    SmartAppendL(text, KNewLine());
    
    SmartAppendL(text, KVersionSection());
    SmartAppendL(text, KNewLine());
    SmartAppendL(text, aPolicyDetails.iVersion);
    SmartAppendL(text, KNewLine());    

    SmartAppendL(text, KIssuerNameSection());
    SmartAppendL(text, KNewLine());    
    SmartAppendL(text, aPolicyDetails.iIssuerName);
    SmartAppendL(text, KNewLine());    

    SmartAppendL(text, KContactSection());
    SmartAppendL(text, KNewLine());    
    SmartAppendL(text, aPolicyDetails.iContactInfo);
    SmartAppendL(text, KNewLine());

    CleanupStack::Pop(); // text

    return text;
    }

void TPinParser::SmartAppendL(HBufC*& aBuf, const TDesC& aText)
    {
    // Make sure that we have enough space for the new text
    
    TInt spaceLeft = aBuf->Des().MaxLength() - aBuf->Des().Length();

    if (aText.Length() > spaceLeft)
        {
        // Allocate enough space for the new text + some additional 
        // free space so that allocations are not too frequent

        TInt newMaxLength = aBuf->Des().MaxLength() + aText.Length() + KInitialDetailsTextLength;

        aBuf = aBuf->ReAllocL(newMaxLength);
        }
    
    aBuf->Des().Append(aText);
    }
