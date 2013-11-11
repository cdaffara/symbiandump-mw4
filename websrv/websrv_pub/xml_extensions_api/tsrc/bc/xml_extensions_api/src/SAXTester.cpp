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

#include "SAXTester.h"
#include "WSTester.h"

#define STATUS_NEW 0
#define STATUS_READY 1
#define STATUS_EXPIRED 2

namespace
	{
	}

enum TErrorState
	{
	ENoError = 0,
	ESoapError,
	ENestedError
	};

CSAXTester::CSAXTester( ):
	iDelegate(NULL)
    {
	iStartElem = 0;
	iEndElem = 0;
	iContent = 0;
	iRunCase = 0;
	iAsXml	 = NULL;
    }
CSAXTester::~CSAXTester()
	{
	delete iXmlReader;
	delete iAsXml;
	if(iDelegate)
		{
//		iDelegate->ExtractElement(); // if delegate's element is onwned by this class
		delete iDelegate;
		}
	}

CSAXTester* CSAXTester::NewL(const TDesC8& aLocalName)
	{
	CSAXTester* saxTester = new (ELeave) CSAXTester();
	CleanupStack::PushL(saxTester);
	saxTester->ConstructL( aLocalName );
	CleanupStack::Pop();
	return saxTester;
	}

CSAXTester* CSAXTester::NewL(const TDesC8& aNsUri, const TDesC8& aLocalName )
	{
	CSAXTester* saxTester = new (ELeave) CSAXTester();
	CleanupStack::PushL(saxTester);
	saxTester->ConstructL( aNsUri, aLocalName );
	CleanupStack::Pop();
	return saxTester;
	}
CSAXTester* CSAXTester::NewL(const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName  )
	{
	CSAXTester* saxTester = new (ELeave) CSAXTester();
	CleanupStack::PushL(saxTester);
	saxTester->ConstructL( aNsUri, aLocalName, aQName );
	CleanupStack::Pop();
	return saxTester;
	}
CSAXTester* CSAXTester::NewL(const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName, const RAttributeArray& apAttrs  )
	{
	CSAXTester* saxTester = new (ELeave) CSAXTester();
	CleanupStack::PushL(saxTester);
	saxTester->ConstructL( aNsUri, aLocalName, aQName, apAttrs );
	CleanupStack::Pop();
	return saxTester;
	}
CSAXTester* CSAXTester::NewL(const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName, const RAttributeArray& apAttrs, CSenElement& aParent  )
	{
	CSAXTester* saxTester = new (ELeave) CSAXTester();
	CleanupStack::PushL(saxTester);
	saxTester->ConstructL( aNsUri, aLocalName, aQName, apAttrs, aParent );
	CleanupStack::Pop();
	return saxTester;
	}

CSAXTester* CSAXTester::NewL(CSenElement& aParent  )
	{
	CSAXTester* saxTester = new (ELeave) CSAXTester();
	CleanupStack::PushL(saxTester);
	saxTester->ConstructL( aParent );
	CleanupStack::Pop();
	return saxTester;
	}

void CSAXTester::ConstructL()
	{
//	CSenBaseFragment::BaseConstructL(KPpContract, KQueryResponseLocalName);
	//iReader.CreateL();
	iXmlReader = CSenXmlReader::NewL();
	}
void CSAXTester::ConstructL( const TDesC8& aLocalName )
	{
	CSenBaseFragment::BaseConstructL( aLocalName );
	//iReader.CreateL();
	iXmlReader = CSenXmlReader::NewL();
	}

void CSAXTester::ConstructL( const TDesC8& aNsUri, const TDesC8& aLocalName  )
	{
	CSenBaseFragment::BaseConstructL( aNsUri, aLocalName );
	//iReader.CreateL();
	iXmlReader = CSenXmlReader::NewL();
	}

void CSAXTester::ConstructL( const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName )
	{
	CSenBaseFragment::BaseConstructL( aNsUri, aLocalName, aQName );
	//iReader.CreateL();
	iXmlReader = CSenXmlReader::NewL();
	}
void CSAXTester::ConstructL( const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName, const RAttributeArray& apAttrs )
	{
	CSenBaseFragment::BaseConstructL( aNsUri, aLocalName, aQName, apAttrs );
	//iReader.CreateL();
	iXmlReader = CSenXmlReader::NewL();
	}
void CSAXTester::ConstructL( const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName, const RAttributeArray& apAttrs, CSenElement& aParent )
	{
	CSenBaseFragment::BaseConstructL( aNsUri, aLocalName, aQName, apAttrs, aParent );
	//iReader.CreateL();
	iXmlReader = CSenXmlReader::NewL();
	}
void CSAXTester::ConstructL( CSenElement& aParent )
	{
	CSenBaseFragment::BaseConstructL( aParent );
	//iReader.CreateL();
	iXmlReader = CSenXmlReader::NewL();
	}

void CSAXTester::StartElementL(	const TDesC8& aNsUri,
										const TDesC8& aLocalName,
										const TDesC8& aQName,
										const RAttributeArray& aAttrs)
	{
	
	iStartElem++;

	if (iRunCase == KDelegateParsing && aLocalName == KL2Name8)
		{
		iState = KStateIgnore;
		iDelegate = CSenBaseFragment::NewL(KL2Name8);
		DelegateParsingL( *iDelegate );
		}
	else if (iRunCase == KDelegateParsing2 && iStartElem == 2)
		{
		iState = KStateIgnore;
		DelegateParsingL( aNsUri, aLocalName, aQName, aAttrs );
		
		}
   	else if (iRunCase == KDetach && aLocalName == KL2Name8)
		{
		iState = KStateIgnore;
		iDelegate = CSenBaseFragment::NewL( KDefaultNS(), KL2Name8()  );
		DelegateParsingL( *iDelegate );

		}
	else 
		{
		CSenBaseFragment::StartElementL( aNsUri, aLocalName, aQName , aAttrs );
		}

	
//	LOG_L("CAddressBookEngine::StartElementL() ***");
//	LOG_FORMAT((_L("- namespace URI  (%S)"), aNsUri));
//	LOG_FORMAT((_L("- localname      (%S)"), aLocalName));
//	LOG_FORMAT((_L("- qualified name (%S)"), aQName));
//
//	if (aLocalName == KStatus)
//		{
//		
//		if (SenXmlUtils::LocalName(SenXmlUtils::AttrValue(aAttrs, KCode)) != KOk)
//			{
//			iErrorState = ENestedError;
//			delete iFaultDelegate;
//			iFaultDelegate = NULL;
//			iFaultDelegate = CSenDomFragment::NewL(aNsUri,
//							aLocalName, aQName, aAttrs);
//			DelegateParsingL(*iFaultDelegate);
//			return;
//			}
//		}
//	else if (aLocalName == KCard)
//		{
//		iErrorState = ENoError;
//
//		CContact* delegate = CContact::NewL();
//		iContacts.Append(delegate);
//		DelegateParsingL(*delegate);
//		return;
//		}
	}
void CSAXTester::EndElementL(	const TDesC8& aNsUri,
										const TDesC8& aLocalName,
										const TDesC8& aQName)
	{
	iEndElem++;
	if(iRunCase == KDelegateParsing || iRunCase == KDelegateParsing2 || 
		iRunCase == KDetach )
		{
		if(aLocalName == KL2Name8 )
			{
			// got a call from delegate created by this class
			iState = KStateSave;

			// we could delete delegate here to efficiently free memory
			// asap
			}
		else
			{
			CSenBaseFragment::EndElementL( aNsUri, aLocalName, aQName );
			}
		}
	else
		{
		CSenBaseFragment::EndElementL( aNsUri, aLocalName, aQName );
		}
//	if(aLocalName == KStatus)
//		{
//		if (iErrorState == ENestedError)
//			{
//			iErrorState = ENoError;
//			CSenElement* statusElement = NULL;
//			if (iFaultDelegate)
//				{
//					statusElement =
//						iFaultDelegate->AsElement().Element(KStatus);
//				}
//
//			if(statusElement)
//				{
//				const TDesC8* errorCode = statusElement->AttrValue(KCode);
//				iContacts.ResetAndDestroy();
//				if (errorCode)
//					{
//					if (SenXmlUtils::LocalName(*errorCode) == KTooManyMatches)
//						{
//						iObserver.ErrorL(ETooManyMatches);
//						}
//					else
//						{
//						iObserver.ErrorL(ESearchFailed); // Default error
//						}
//					}
//				}
//			else
//				{
//				// could not find status element:
//				iObserver.ErrorL(ESearchFailed); // Default error
//				}
//			}
//		else if (iErrorState == ESoapError)
//			{
//			iErrorState = ENoError;
//			delete iFaultDelegate;
//			iFaultDelegate = NULL;
//			iContacts.ResetAndDestroy();
//			iObserver.ErrorL(ESearchFailed); // Default error
//			}
//
//		}
//	else
//		{
//		CSenBaseFragment::EndElementL(aNsUri, aLocalName, aQName);
//		}
	}

void CSAXTester::CharactersL(	const TDesC8& aChars,
								TInt aStart,
								TInt aLength
							)
	{
	CSenBaseFragment::CharactersL( aChars, aStart, aLength );

	iContent++;
	}

TBool CSAXTester::CheckParsingStatus(TInt aStart, TInt aEnd, TInt aContent )
	{
		if (aStart != iStartElem || aEnd != iEndElem || aContent != iContent )
			{
			return EFalse;
			}
		else
			{
			return ETrue;
			}
	}
void CSAXTester::SetRunCase(TInt aCase)
	{
	iRunCase = aCase;
	}
CSenBaseFragment* CSAXTester::GetDetachedDelegate()
	{
	iDelegate->DetachL();
	return iDelegate;
	}
CSenBaseFragment* CSAXTester::GetDelegate()
	{
	return iDelegate;
	}
