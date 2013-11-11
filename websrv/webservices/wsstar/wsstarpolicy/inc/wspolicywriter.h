/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/








#ifndef C_WS_POLICY_WRITER_H
#define C_WS_POLICY_WRITER_H


#include <e32std.h>
#include <f32file.h>
#include <s32mem.h>

#include "SenDomFragment.h"
#include "policyassertion.h"
#include "primitiveassertion.h"

class CPolicyAssertion;
class CAndCompositeAssertion;
class CXorCompositeAssertion;
class CAssertion;
class CPrimitiveAssertion;
class CPolicyReferenceAssertion;
namespace PolicyWriter
{
    // XML String constants
_LIT8(KSenColon,                 ":");
_LIT8(KStartElement,              "<");
_LIT8(KSenGreaterThan,           ">");
_LIT8(KSenSpace,                 " ");
_LIT8(KSenDblQuot,               "\"");
_LIT8(KSenEquals,                "=");
_LIT8(KSenEqualsDblQuot,         "=\"");
_LIT8(KEndElement,               "/>");
_LIT8(KSenLessThanSlash,         "</");
_LIT8(KSenSpaceXmlns,            " xmlns");
_LIT8(KSenXmlns,                 "xmlns");
_LIT8(KSenXmlNsAttNamePlusColon, " xmlns:"); 

// The predeclared, basic XML entities:
// &apos;    (')
// &quot;    (")
// &gt;      (>)
// &lt;      (<)
// &amp;     (&)
_LIT8(KSenQuotedApos,    "&apos;");
_LIT8(KSenQuotedDblQuot, "&quot;");
_LIT8(KSenQuotedGt,      "&gt;");
_LIT8(KSenQuotedLt,      "&lt;");
_LIT8(KSenQuotedAmp,     "&amp;");
_LIT8(KWspPolicyPrefix,     "wsp:Policy");
_LIT8(KWspXorPrefix,     "wsp:ExactlyOne");
_LIT8(KWspAndPrefix,     "wsp:All");
_LIT8(KWspPolicyReferencePrefix,     "wsp:PolicyReference");
_LIT8(KWspPolicyReferenceUri,     "URI ");

_LIT8(KSenXmlNsAttNamePlusColonWsp, " xmlns:wsp");
}

typedef RSenPointerMap<TDesC8, TDesC8> RNamespaceMap;

class CWSPolicyWriter : public CBase
{
public:
    static  CWSPolicyWriter* NewL();
    static  CWSPolicyWriter* NewLC();
    ~CWSPolicyWriter();
    
//#ifdef _SENDEBUG    
    void WriteToFileAllL(CSenElement* aXml);
    void WriteToFileAllL(TDesC8& aXml);
//#endif
    
    HBufC8* WriteAllAsXMlL(CPolicyAssertion* aPolicy);
    HBufC8* WriteAllAsXmlL(CSenElement* aXml);
    TPtrC8 WriteToBufL(CBufBase& aBuf, CPolicyAssertion* aEle);
private:
    
     TInt WritePolicyL(CPolicyAssertion* aEle, RWriteStream& aWriteStream);
     TInt WriteAndAssertionL(CAndCompositeAssertion* aEle, RWriteStream& aWriteStream);
     TInt WriteXorAssertionL(CXorCompositeAssertion* aEle, RWriteStream& aWriteStream);
     TInt WriteAssertionL(MAssertion* aEle, RWriteStream& aWriteStream);
     TInt WritePolicyReferenceL(CPolicyReferenceAssertion* aAssertion, RWriteStream& aWriteStream);
     TInt WritePrimitiveAssertionL(CPrimitiveAssertion* aEle, RWriteStream& aWriteStream);
     TInt WriteTermsL(RPointerArray<MAssertion>& aTerms, RWriteStream& aWriteStream);
     void WriteAttributesL(CPrimitiveAssertion* aAsserion, RWriteStream& aWriteStream);
     
     TPtrC8 GetAttrib(TPtrC8 aReference, CSenElement* aEle);
     
     TInt AddNameSpaceL(TPtrC8 aName, TPtrC8 aUri);	//codescannerwarnings
     TBool IsNamespaceAdded(TPtrC8 aName);
     
     CWSPolicyWriter();
     void ConstructL();
private:
    RNamespaceMap iNamespcaes;
    TInt count;
};
#endif //C_WS_POLICY_WRITER_H