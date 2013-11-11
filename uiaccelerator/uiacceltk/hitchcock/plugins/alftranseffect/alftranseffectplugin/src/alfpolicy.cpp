/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   a policy
*
*/



#include "alfpolicy.h"
#include <alflogger.h>
// ======== MEMBER FUNCTIONS ========

CFullscreenPolicy::~CFullscreenPolicy()
	{
	__ALFFXLOGSTRING("CFullscreenPolicy::~CFullscreenPolicy >>");
	delete iFilename;
	delete iResourceDir; 
	__ALFFXLOGSTRING("CFullscreenPolicy::~CFullscreenPolicy <<");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CFullscreenPolicy::CFullscreenPolicy(TUint aAction, const TUid &aUid, 
									 const TThreadId& aThreadId, TInt aCachePriority, 
					 				 TInt aWantedTime, TInt aMinTime) : 
					 				 iAction(aAction), iUid(aUid), iThreadId(aThreadId), 
					 				 iCachePriority(aCachePriority), iWantedTime(aWantedTime), 
					 				 iMinTime(aMinTime)
	{
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//

CFullscreenPolicy* CFullscreenPolicy::NewL(TUint aAction, const TUid& aUid, 
										   const TThreadId& aThreadId, const TDesC& aResourceDir, 
										   const TDesC& aFilename,  TInt aCachePriority, 
					 					   TInt aWantedTime, TInt aMinTime)
	{
	__ALFFXLOGSTRING("CFullscreenPolicy::NewL >>");
	CFullscreenPolicy* self = new (ELeave)CFullscreenPolicy(aAction, aUid, aThreadId, aCachePriority, 
															aWantedTime, aMinTime);
	CleanupStack::PushL(self);
	self->ConstructL(aResourceDir, aFilename);
	CleanupStack::Pop(self);
	__ALFFXLOGSTRING("CFullscreenPolicy::NewL <<");
	return self;
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CFullscreenPolicy::ConstructL(const TDesC& aResourceDir, const TDesC& aFilename)
	{
	__ALFFXLOGSTRING2("CFullscreenPolicy::ConstructL - Dir: %S Filename: %S>>", &aResourceDir, &aFilename );
	iResourceDir = HBufC::NewL( aResourceDir.Length() );
	iResourceDir->Des().Copy( aResourceDir );
	iFilename = HBufC::NewL( aFilename.Length() );
	iFilename->Des().Copy( aFilename );
	__ALFFXLOGSTRING("CFullscreenPolicy::ConstructL <<");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CActionPolicy::~CActionPolicy()
	{
	__ALFFXLOGSTRING("CActionPolicy::~CActionPolicy >>");
	delete iActionString;
	__ALFFXLOGSTRING("CActionPolicy::~CActionPolicy <<");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CActionPolicy::SetActionStringL(const TDesC& aActionString)
	{
	__ALFFXLOGSTRING1("CActionPolicy::SetActionStringL - aActionString: %S>>", &aActionString );
	if(iActionString)
		{
		delete iActionString;
		iActionString = NULL;
		}
		
	iActionString = HBufC::NewL( aActionString.Length() );
	iActionString->Des().Copy( aActionString );
	__ALFFXLOGSTRING("CActionPolicy::SetActionStringL <<");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CControlPolicy* CControlPolicy::NewL(const TUid& aUid, 
									 const TThreadId& aThreadId, 
									 const TDesC& aResourceDir, 
						  			 const TDesC& aFilename, 
						  			 TInt aCachePriority,
						  			 TInt aWantedTime,
						  			 TInt aMinTime)
	{
	__ALFFXLOGSTRING("CControlPolicy::NewL >>");
	CControlPolicy* self = new (ELeave) CControlPolicy(aUid, aThreadId, aCachePriority, aWantedTime, aMinTime);
	CleanupStack::PushL(self);
	self->ConstructL(aResourceDir, aFilename);
	CleanupStack::Pop(self);
	__ALFFXLOGSTRING("CControlPolicy::NewL <<");
	return self;
	}	
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CControlPolicy::~CControlPolicy()
	{
	__ALFFXLOGSTRING("CControlPolicy::~CControlPolicy >>");
	delete iResourceDir;
	delete iFilename;
	
	iControlPolicies.ResetAndDestroy();
	__ALFFXLOGSTRING("CControlPolicy::~CControlPolicy <<");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CControlPolicy::ConstructL(const TDesC& aResourceDir, const TDesC& aFilename)
	{
	__ALFFXLOGSTRING2("CControlPolicy::ConstructL - Dir: %S Filename: %S>>", &aResourceDir, &aFilename );
	    
	iResourceDir = HBufC::NewL( aResourceDir.Length() );
	iResourceDir->Des().Copy( aResourceDir );
	iFilename = HBufC::NewL( aFilename.Length() );
	iFilename->Des().Copy( aFilename );
	__ALFFXLOGSTRING("CControlPolicy::ConstructL <<");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CControlPolicy::CControlPolicy(const TUid aUid, const TThreadId aThreadId, TInt aCachePriority, TInt aWantedTime, TInt aMinTime) : 
	iUid(aUid), 
	iThreadId(aThreadId),
	iCachePriority(aCachePriority),
	iWantedTime(aWantedTime),
	iMinTime(aMinTime)
	{
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CListBoxPolicy* CListBoxPolicy::NewL(const TUid &aUid, 
									 const TThreadId& aThreadId,
                                     const CAlfTransitionServerClient::TListBoxType aListBoxType,
                                     const TDesC& aResourceDir, 
									 const TDesC& aBackgroundFilename, const TDesC& aItemFilename, 
									 TInt aCachePriority, TInt aWantedTime, TInt aMinTime)
	{
	__ALFFXLOGSTRING("CListBoxPolicy::NewL >>");
	CListBoxPolicy* self = new (ELeave)CListBoxPolicy(aUid, aThreadId, aListBoxType, aCachePriority, aWantedTime, aMinTime);
	CleanupStack::PushL(self);
	self->ConstructL(aResourceDir, aBackgroundFilename, aItemFilename);
	CleanupStack::Pop(self);
	__ALFFXLOGSTRING("CListBoxPolicy::NewL <<");
	return self;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CListBoxPolicy::ConstructL(const TDesC& aResourceDir, const TDesC& aBackgroundFilename, const TDesC& aItemFilename)
	{
    __ALFFXLOGSTRING3("CListBoxPolicy::ConstructL - Dir: %S, aBackgroundFilename %S, ItemFilename: %S>>", &aResourceDir, &aBackgroundFilename, &aItemFilename );

	iResourceDir = HBufC::NewL( aResourceDir.Length() );
	iResourceDir->Des().Copy( aResourceDir );
	iBackgroundFilename = HBufC::NewL( aBackgroundFilename.Length() );
	iBackgroundFilename->Des().Copy( aBackgroundFilename );
	iItemFilename = HBufC::NewL( aItemFilename.Length() );
	iItemFilename->Des().Copy( aItemFilename );
	__ALFFXLOGSTRING("CListBoxPolicy::ConstructL <<");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CListBoxPolicy::CListBoxPolicy(const TUid& aUid, const TThreadId& aThreadId,
                               const CAlfTransitionServerClient::TListBoxType aListBoxType,
                               TInt aCachePriority, TInt aWantedTime, TInt aMinTime) :
	iUid(aUid), iThreadId(aThreadId), iListBoxType(aListBoxType), iCachePriority(aCachePriority), 
	iWantedTime(aWantedTime), iMinTime(aMinTime)
	{
	TInt spec = iUid.iUid == 0 ? 0 : 2;
	spec += aListBoxType == CAlfTransitionServerClient::EListTypeAny ? 0 : 1;
	iSpecificity = spec;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CListBoxPolicy::~CListBoxPolicy()
	{
	__ALFFXLOGSTRING("CListBoxPolicy::~CListBoxPolicy >>");
	delete iResourceDir;
	delete iBackgroundFilename;
	delete iItemFilename;
	__ALFFXLOGSTRING("CListBoxPolicy::~CListBoxPolicy <<");
	}

// --- EOF ---


