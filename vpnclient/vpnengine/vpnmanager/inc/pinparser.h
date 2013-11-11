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



#ifndef __PINPARSER_H__
#define __PINPARSER_H__

#include <e32std.h>
#include "vpnapidefs.h"

_LIT(KNameSection,          "[POLICYNAME]");
_LIT(KVersionSection,       "[POLICYVERSION]");
_LIT(KDescriptionSection,   "[POLICYDESCRIPTION]");
_LIT(KIssuerNameSection,    "[ISSUERNAME]");
_LIT(KContactSection,       "[CONTACTINFO]");

const TChar KSectionBeginChar = '[';

const TInt KInitialDetailsTextLength = 1024;
                                      
_LIT(KNewLine, "\n");

class TFileUtil;

/**
 * Parses policy information file contents.
 */
class TPinParser
    {
public:
    TPinParser(TFileUtil& aFileUtil);
    void ParsePolicyInfoL(const TFileName& aPinFile, TVpnPolicyInfo& aPolicyInfo);
    void ParsePolicyDetailsL(const TDesC& aPinFile, TVpnPolicyDetails& aPolicyDetails);
    HBufC* PolicyDetailsAsTextL(const TVpnPolicyDetails& aPolicyDetails);

private:
    void ParseSectionInfo(const TDesC& aSectionTag, TVpnPolicyInfo& aPolicyInfo);
    void ParseSectionDetails(const TDesC& aSectionTag, TVpnPolicyDetails& aPolicyDetails);
    TPtrC GetSectionContent();
    void SmartAppendL(HBufC*& aBuf, const TDesC& aText);
            
private:
    TFileUtil& iFileUtil;
    TLex iLexer;
    };

#endif // __PINPARSER_H_
