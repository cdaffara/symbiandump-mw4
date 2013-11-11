// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

CTestPnPParamSetBase* CTestPnPParamSetBase::NewL ( RParameterFamily& aFamily, TTestPnPParamSetFamily aFamilyType )
	{
	STypeId typeId = STypeId::CreateSTypeId(KTestPnPParamsFactoryUid,aFamilyType);
	CTestPnPParamSetBase* self = static_cast < CTestPnPParamSetBase* > ( CSubConParameterSet::NewL( typeId ) );
	TBool containerValue = aFamily.IsNull();
	if(!containerValue)
		{
		CleanupStack::PushL ( self );
		aFamily.AddParameterSetL ( self, RParameterFamily::ERequested );			
		CleanupStack::Pop ( self );
		}
	return self;	
	}
	
CTestPnPParamSetBase::CTestPnPParamSetBase ()
	{
	}

// ------------------------------------------
CTestPnPRequestParamSetBase::CTestPnPRequestParamSetBase ()
	{
	}

CTestPnPRequestParamSetBase::~CTestPnPRequestParamSetBase ()
	{
	}

// -------------------------------------------------------------------------
CTestPnPResponseParamSetBase::CTestPnPResponseParamSetBase ()
	{
	}

void CTestPnPResponseParamSetBase::SetTestPnPEvent ( const TTestPnPEvent& aEvent )
	{
	iTestPnPEvent = aEvent;
	}

const TTestPnPEvent& CTestPnPResponseParamSetBase::TestPnPEvent () const
	{
	return iTestPnPEvent;		
	}


// -------------------------------------------------------------------------
CTestPnPDiscoverRequestParamSet* CTestPnPDiscoverRequestParamSet::NewL ( RParameterFamily& aFamily )
 	{	 
 	return static_cast < CTestPnPDiscoverRequestParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPDiscoverRequestParamSet ) );
 	}


CTestPnPSubscribeRequestParamSet* CTestPnPSubscribeRequestParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CTestPnPSubscribeRequestParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPSubscribeRequestParamSet ) );
	}


// ----------------------------------------------------------------------------

CTestPnPRegisterAnnouncementParamSet* CTestPnPRegisterAnnouncementParamSet::NewL ( RParameterFamily& aFamily )
	{
	return static_cast < CTestPnPRegisterAnnouncementParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPRegisterAnnouncementParamSet ) );
	}

void CTestPnPRegisterAnnouncementParamSet::SetRegisterVariable(TBool aValue)
	{
	iRegister=aValue;
	}

TBool CTestPnPRegisterAnnouncementParamSet::RegisterVariable()
	{
	return iRegister;
	}
// -----------------------------------------------------------------------------

CTestPnPDescribeRequestParamSet* CTestPnPDescribeRequestParamSet::NewL ( RParameterFamily& aFamily )
	{
	return static_cast < CTestPnPDescribeRequestParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPDescribeRequestParamSet ) );
	}



// -----------------------------------------------------------------------------

CTestPnPCancelDiscoverParamSet* CTestPnPCancelDiscoverParamSet::NewL ( RParameterFamily& aFamily)
	{
	return static_cast < CTestPnPCancelDiscoverParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPCancelDiscoverParamSet ) );
	}
// -----------------------------------------------------------------------------

CTestPnPCancelDescribeParamSet* CTestPnPCancelDescribeParamSet::NewL ( RParameterFamily& aFamily)
	{
	return static_cast < CTestPnPCancelDescribeParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPCancelDescribeParamSet ) );
	}

// -----------------------------------------------------------------------------
CTestPnPCancelSubscribeParamSet* CTestPnPCancelSubscribeParamSet::NewL ( RParameterFamily& aFamily)
	{
	return static_cast < CTestPnPCancelSubscribeParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPCancelSubscribeParamSet ) );
	}

// -----------------------------------------------------------------------------
CTestPnPCancelRegisterNotifyParamSet* CTestPnPCancelRegisterNotifyParamSet::NewL ( RParameterFamily& aFamily)
	{
	return static_cast < CTestPnPCancelRegisterNotifyParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPCancelRegisterNotifyParamSet ) );
	}
// -----------------------------------------------------------------------------
CTestPnPDiscoverResponseParamSet* CTestPnPDiscoverResponseParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CTestPnPDiscoverResponseParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPDiscoverResponseParamSet ) );
	}

// D'tor
CTestPnPDiscoverResponseParamSet::~CTestPnPDiscoverResponseParamSet ()
	{
	}


CTestPnPDescribeResponseParamSet* CTestPnPDescribeResponseParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CTestPnPDescribeResponseParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPDescribeResponseParamSet ) );
	}
	
CTestPnPDescribeResponseParamSet::~CTestPnPDescribeResponseParamSet ()
	{
	}


// -----------------------------------------------------------------------------
CTestPnPSubscribeResponseParamSet* CTestPnPSubscribeResponseParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CTestPnPSubscribeResponseParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPSubscribeResponseParamSet ) );
	}


CTestPnPNotifyEventParamSet* CTestPnPNotifyEventParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CTestPnPNotifyEventParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPNotifyEventParamSet ) );
	}


CTestPnPNotifyEventParamSet::~CTestPnPNotifyEventParamSet ()
	{
	}


// -------------------------------------------------------------------------------------
CTestPnPAbsenceAnnouncementParamSet* CTestPnPAbsenceAnnouncementParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CTestPnPAbsenceAnnouncementParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPAbsenceAnnouncementParamSet ) );
	}


CTestPnPAbsenceAnnouncementParamSet::~CTestPnPAbsenceAnnouncementParamSet ()
	{
	}
	

CTestPnPPresenceAnnouncementParamSet* CTestPnPPresenceAnnouncementParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CTestPnPPresenceAnnouncementParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPPresenceAnnouncementParamSet ) );
	}
	
		
CTestPnPPresenceAnnouncementParamSet::~CTestPnPPresenceAnnouncementParamSet ()
	{
	}


CTestPnPServiceRegisterParamSet::~CTestPnPServiceRegisterParamSet ()
	{
	}

CTestPnPServiceRegisterParamSet* CTestPnPServiceRegisterParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CTestPnPServiceRegisterParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPServiceRegisterParamSet ) );
	}


// -------------------------------------------------------------------------------------

CTestPnPPublishResponseParamSet* CTestPnPPublishResponseParamSet::NewL(RParameterFamily& aFamily)
	{
	return static_cast < CTestPnPPublishResponseParamSet* > ( CTestPnPParamSetBase::NewL( aFamily, ETestPnPPublishResponseParamSet ) );
	}
