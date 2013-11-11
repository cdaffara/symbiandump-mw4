/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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






 

#ifndef WSSTAR_POLICY_MERGER_H
#define WSSTAR_POLICY_MERGER_H

#include <SenDomFragmentBase.h>
#include <RSenDocument.h>
#include <XmlEngDDocument.h>


class CSenElement;
class CWsSecurityPolicy;

/**
 * Class:   CPolicyMerger
 * Description: This class can merge two polcicies provided as fileNames or TElement
 * The Merging take care of domian specific knowledge of the assertion and can resolve it. 
 * currently only Ws-SecurityPolicy domain is supported
 */

class CPolicyMerger : public CBase	//CodeScannerWarnings

{
  
  public:

  static CPolicyMerger* NewL();
  static CPolicyMerger* NewLC();
    
  virtual ~CPolicyMerger();

  TXmlEngElement MergeL(TDesC& aFirstPolicy, TDesC& aSecondPolicy); //filenames with absolute path
  TXmlEngElement MergeL(TXmlEngElement aFirstPolicy, TXmlEngElement aSecondPolicy); //root elements
private:

  void ConstructL();

  CPolicyMerger();
  
  void MergeAllANDAssertionsL(TXmlEngElement aFirstElement, 
                    TXmlEngElement aSecondElement, RArray<TXmlEngElement>& aResult);
  TXmlEngElement MergeCompareSecondAssertionsL(TXmlEngElement aFirstElement, 
                    TXmlEngElement aSecondElement, TXmlEngElement aIntersectedAND);
  
  TInt VerifyNamespace(TXmlEngElement aFirstElement, TXmlEngElement aSecondElement);
  
private:    
   CWsSecurityPolicy* iSecurityPolicyDomain;
   
   TXmlEngElement  irootAND;
   TXmlEngElement  iMergedPolicyElement;
      
   RXmlEngDocument iPolicy1; 
   RXmlEngDocument iPolicy2;

//   TString XORString;
//   TString ANDString;
//   TString policyNsUri;
};

#endif //WSSTAR_POLICY_MERGER_H