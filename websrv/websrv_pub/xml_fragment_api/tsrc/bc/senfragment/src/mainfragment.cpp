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








#include "mainfragment.h"
#include "SenFragmentBase.h"

namespace
    {
	const TInt KStateParsingDelegate    = 100;
    _LIT8(KMainFragmentXmlns,   "urn:main:fragment");
    _LIT8(KMainFragmentName,    "MainFragment");
    _LIT8(KMainFragmentQName,   "mn:MainFragment");
    _LIT8(KDelegateName,        "DelegateFragment");
    }

CMainFragment* CMainFragment::NewL()
    {
    CMainFragment* pNew = NewLC();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CMainFragment* CMainFragment::NewLC()
    {
    CMainFragment* pNew = new (ELeave) CMainFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }


CMainFragment* CMainFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CMainFragment* pNew = NewLC( aNsUri, aLocalName, aQName );
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CMainFragment* CMainFragment::NewLC(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CMainFragment* pNew = new (ELeave) CMainFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName);
    return pNew;
    }


CMainFragment::CMainFragment()
: ipDelegateFragment(NULL)
    {
    }

void CMainFragment::BaseConstructL()
    {
    CSenFragmentBase::BaseConstructL(
        KMainFragmentXmlns,
        KMainFragmentName,
        KMainFragmentQName
        );
    }

void CMainFragment::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CSenFragmentBase::BaseConstructL(aNsUri, aLocalName, aQName);
    }

CMainFragment::~CMainFragment()
    {
    delete ipDelegateFragment;
    }

void CMainFragment::OnStartElementL(const RTagInfo& aElement,
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

				ipDelegateFragment = CDelegateFragment::NewL(
					                         saxNsUri, saxLocalName,
					                         saxPrefix, aAttributes
					                         );

                iState = KStateParsingDelegate;

                OnDelegateParsingL(*ipDelegateFragment);
                }
            else
                {
                CSenFragmentBase::OnStartElementL(aElement, aAttributes,
                                                  aErrorCode);
                }
            break;
            }
        default:
            {
            CSenFragmentBase::OnStartElementL(aElement, aAttributes,
                                            aErrorCode);
            break;
            }
        }
    }

void CMainFragment::OnEndElementL(const RTagInfo& aElement,
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
            CSenFragmentBase::OnEndElementL(aElement, aErrorCode);
            break;
            }
        }
    }
    
CDelegateFragment& CMainFragment::DelegateFragment()
    {
    return *ipDelegateFragment;
    }

// End of File

