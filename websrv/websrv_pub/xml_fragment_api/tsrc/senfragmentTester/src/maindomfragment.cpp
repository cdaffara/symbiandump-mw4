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
* Description:      
*
*/






#include "maindomfragment.h"
#include "SenFragmentBase.h"

namespace
    {
	const TInt KStateParsingDelegate    = 100;
    _LIT8(KMainFragmentXmlns,   "urn:main:fragment");
    _LIT8(KMainFragmentName,    "MainFragment");
    _LIT8(KMainFragmentQName,   "mn:MainFragment");	
    _LIT8(KDelegateName,        "DelegateFragment");
    }

CMainDomFragment* CMainDomFragment::NewL()
    {
    CMainDomFragment* pNew = NewLC();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CMainDomFragment* CMainDomFragment::NewLC()
    {
    CMainDomFragment* pNew = new (ELeave) CMainDomFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }


CMainDomFragment* CMainDomFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CMainDomFragment* pNew = NewLC( aNsUri, aLocalName, aQName );
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CMainDomFragment* CMainDomFragment::NewLC(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CMainDomFragment* pNew = new (ELeave) CMainDomFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName);
    return pNew;
    }


CMainDomFragment::CMainDomFragment()
: ipDelegateFragment(NULL)
    {
    }

void CMainDomFragment::BaseConstructL()
    {
    CSenDomFragmentBase::BaseConstructL(
        KMainFragmentXmlns,
        KMainFragmentName,
        KMainFragmentQName
        );
    }

void CMainDomFragment::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CSenDomFragmentBase::BaseConstructL(aNsUri, aLocalName, aQName);
    }

CMainDomFragment::~CMainDomFragment()
    {
    delete ipDelegateFragment;
    }

void CMainDomFragment::OnStartElementL(const RTagInfo& aElement,
                                    const RAttributeArray& aAttributes,
                                    TInt aErrorCode)
    {
    switch (iState)
        {
        case KSenStateSave:
            {
            const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
            
            if (saxLocalName == KDelegateName)
                {
                const TPtrC8 saxNsUri       = aElement.Uri().DesC();
                const TPtrC8 saxPrefix      = aElement.Prefix().DesC();
                
                TXmlEngElement element = AsElementL();
                RSenDocument& document = AsDocumentL();

				ipDelegateFragment = CDelegateDomFragment::NewL(
					                         saxNsUri, saxLocalName,
					                         saxPrefix, aAttributes,
					                         element, document
					                         );
                
                iState = KStateParsingDelegate;
                
                OnDelegateParsingL(*ipDelegateFragment);
                }
            else
                {
                CSenDomFragmentBase::OnStartElementL(aElement, aAttributes,
                                                  aErrorCode);
                }
            break;
            }
        default:
            {
            CSenDomFragmentBase::OnStartElementL(aElement, aAttributes,
                                            aErrorCode);
            break;
            }
        }
    }

void CMainDomFragment::OnEndElementL(const RTagInfo& aElement,
        						          TInt aErrorCode)

    {
    switch(iState)
        {
        case KStateParsingDelegate:
            {
            iState = KSenStateSave;
            break;
            }

        default:
            {
            CSenDomFragmentBase::OnEndElementL(aElement, aErrorCode);
            break;
            }
        }
    }
    
void CMainDomFragment::OnResumeParsingFromL(const RTagInfo& aElement,
                                                TInt aErrorCode)
    {
    SetContentHandler(*this);

    switch (iState)
        {
        // no other states may be resumed(!)
        case KStateParsingDelegate:
            {
            OnEndElementL(aElement, aErrorCode);
            }
            break;
        }    
    }
    
CDelegateDomFragment& CMainDomFragment::DelegateFragment()
    {
    return *ipDelegateFragment;
    }

// END OF FILE

