// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements a AT extension plugin
// 
//

// INCLUDES
#include <utf.h>
#include "atcmdpam_debug.h"
#include "pamengine.h"
#include "pamplugin.h"
#include "pamplugindebug.h"


// CONSTANTS


// ============================= LOCAL FUNCTIONS  =============================

// ============================= MEMBER FUNCTIONS =============================

/**
 Create a new instance
 */
CPamEngine* CPamEngine::NewL( CPamPlugin* aOwner )
	{
	CPamEngine* self = CPamEngine::NewLC( aOwner );
	self->ConstructL();
	CleanupStack::Pop();

    ATDEBUG( DebugPrint( _L( "CPamEngine::NewL()" )))
	return self;
	}
	

/**
 Create a new instance and push in cleanup stack
 */
CPamEngine* CPamEngine::NewLC( CPamPlugin* aOwner )
	{
	CPamEngine* self = new ( ELeave ) CPamEngine( aOwner );
	CleanupStack::PushL( self );

    ATDEBUG( DebugPrint( _L( "CPamEngine::NewLC()" )))
	return self;
	}


/**
 Destructor
 */
CPamEngine::~CPamEngine()
	{
	ATDEBUG( DebugPrint( _L( "CPamEngine::~CPamEngine()" ) ) )
    delete iTelephony;
    }

/**
 2nd phase Constructor
 (may leave)
 */
void CPamEngine::ConstructL()
	{
    ATDEBUG( DebugPrint( _L( "CPamEngine::ConstructL()" ) ) )
    
    CActiveScheduler::Add( this );
    iTelephony = CTelephony::NewL();
    }


/**
 C++ Contructror
 */
CPamEngine::CPamEngine( CPamPlugin* aOwner ) 
    : CActive( EPriorityNormal ), iOwner( aOwner )
	{
	ATDEBUG( DebugPrint( _L( "CPamEngine::CPamEngine()" )))
	return;
    }


/**
 Get the service provider name
 */
void CPamEngine::GetServiceProviderName()
    {
    TRACE_FUNC_ENTRY
    if ( !IsActive() )
        {
        CTelephony::TNetworkNameV1Pckg namePckg( iNWName );
        iTelephony->GetCurrentNetworkName( iStatus, namePckg );
        SetActive();
        }
	TRACE_FUNC_EXIT
    }

/**
 From CActive
 */
void CPamEngine::RunL()
	{
	TRACE_FUNC_ENTRY
	ATDEBUG( DebugPrint( _L( "CPamEngine::RunL()" ) ) )
    if ( iStatus.Int() != KErrNone )
        {
        iOwner->NameReqCallback( KNullDesC8() );
        return;
        }
    TBuf8<20> out;
    CnvUtfConverter::ConvertFromUnicodeToUtf8( out, iNWName.iNetworkName );
	iOwner->NameReqCallback( out );
	TRACE_FUNC_EXIT
    return;
    }


/**
 From CActive
 */
void CPamEngine::DoCancel()
	{
	ATDEBUG( DebugPrint( _L( "CPamEngine::DoCancel()" ) ) )
	iTelephony->CancelAsync( CTelephony::EGetCurrentNetworkNameCancel );
    return;
    }


/**
 Get Access point table by the specified access point type
 */
void CPamEngine::ReadAccessPointTableL(TInt aAccessPointType,  CDesC8ArrayFlat*& apArray )
    {
    TRACE_FUNC_ENTRY

    switch ( aAccessPointType )
        {
        case EINTERNETACCESSPOINT:      
            {
            //Internet access point;
            ReadPacketServicetableL( CMManager::EPacketDataInternetOnly, apArray );
            }
            break;
        case EWAPACCESSPOINT:
            {
            //Wap access point;
            ReadPacketServicetableL( CMManager::EPacketDataWAPOnly, apArray );
            break;
            }
        case EBOTHACCESSPOINT:
            {
            //Internet and wap access point only;
            ReadPacketServicetableL( CMManager::EPacketDataBoth, apArray );
            }
            break;
        default:
            {
            User::Leave( KErrArgument );
            }
            break;
        }
	TRACE_FUNC_EXIT
    return;
    }


/**
 Get supported access point type
 */
void CPamEngine::SupportedAccessPoints(HBufC16*& aBuf)
    {
    TRACE_FUNC_ENTRY
    _LIT( KAccessTypeString, "(%d,%d,%d)" );
    aBuf->Des().AppendFormat( KAccessTypeString, CMManager::EPacketDataInternetOnly, 
                                                 CMManager::EPacketDataWAPOnly, 
                                                 CMManager::EPacketDataBoth );
    TRACE_FUNC_EXIT
    return;
    }


/**
 Get service names by the specified access point type
 */
void CPamEngine::ReadPacketServicetableL( CMManager::TPacketDataApType aAccessPointType,  CDesC8ArrayFlat*& apArray  )
    {
    TRACE_FUNC_ENTRY
	ATDEBUG( DebugPrint( _L( "CPamEngine::ReadPacketServicetable()" ) ) )

    _LIT( KIP, "IP" );
    _LIT( KIPV6, "IPV6" );
    _LIT( KComma, "," );
    _LIT( KNumber, "%d" );
    TBuf<1> apsecure_auth;
    TBuf<3> apnumber;
    const TInt KBufsize = 256;

    RCmManager cmManager;
    cmManager.OpenL();
    CleanupClosePushL(cmManager);
    RArray<TUint32> destinations;
    cmManager.AllDestinationsL(destinations);
    CleanupClosePushL(destinations);
    RCmDestination destination;

    HBufC16 *tmpName = NULL;
    HBufC16* buf = HBufC16::NewLC( KBufsize );
    TBuf<4> apPDP_Type;


    for(TInt i = 0; i < destinations.Count(); i++)
        {
        destination = cmManager.DestinationL(destinations[i]);
        CleanupClosePushL(destination);
        tmpName = destination.NameLC();
       	ATDEBUG( DebugPrint( _L( "CPamEngine::ReadPacketServicetable() destination.name: %S" ), tmpName ) )

        RCmConnectionMethod connectionMethod;
        for(TInt j = 0; j < destination.ConnectionMethodCount(); j++)
            {
            connectionMethod = destination.ConnectionMethodL(j);
            CleanupClosePushL(connectionMethod);

            TUint32 bearerType = connectionMethod.GetIntAttributeL( CMManager::ECmBearerType );
            if (bearerType == KUidPacketDataBearerType)
                {
                TUint32 connectionType = connectionMethod.GetIntAttributeL( CMManager::EPacketDataApType );
                if (connectionType == aAccessPointType)
                    {
                    // ap number
                    TUint32 id = connectionMethod.GetIntAttributeL( CMManager::ECmIapId );
                    apnumber.Format( KNumber, id );
                    buf->Des().Copy( apnumber );
                    buf->Des().Append( KComma );

                    // IAP name
                    tmpName = connectionMethod.GetStringAttributeL(CMManager::ECmName);
                    AddQuotationmarks( *tmpName, buf );
                    buf->Des().Append( KComma );

                    // gprs_accesspointname
                    tmpName = connectionMethod.GetStringAttributeL(CMManager::EPacketDataAPName);
                    AddQuotationmarks( *tmpName, buf );

                    // PDP_type
                    TInt isIPv6Supported = connectionMethod.GetBoolAttributeL(CMManager::EPacketDataPDPType);
                    if ( isIPv6Supported )
                        {
                        apPDP_Type = KIPV6;
                        }
                    else
                        {
                        apPDP_Type = KIP;
                        }
                    buf->Des().Append( KComma );
                    AddQuotationmarks( apPDP_Type, buf );

                    // username
                    tmpName = connectionMethod.GetStringAttributeL(CMManager::ECmIFAuthName);
                    buf->Des().Append( KComma );
                    AddQuotationmarks( *tmpName, buf );

                    // password
                    tmpName = connectionMethod.GetStringAttributeL(CMManager::ECmIFAuthPass);
                    buf->Des().Append( KComma );
                    AddQuotationmarks( *tmpName, buf );

                    // secure_auth
                    TBool tmp_apsecure_auth = connectionMethod.GetBoolAttributeL(CMManager::ECmDisablePlainTextAuth);
                    if ( tmp_apsecure_auth )
                        {
                        apsecure_auth.Format( KNumber, 1 );
                        }
                    else
                        {
                        apsecure_auth.Format( KNumber, 0 );
                        }
                    buf->Des().Append( KComma );
                    buf->Des().Append( apsecure_auth );

                	ATDEBUG( DebugPrint( _L( "CPamEngine::ReadPacketServicetable() ATCommand items fetched" ) ) )

                    TBuf8<KBufsize> text8;
                    if ( CnvUtfConverter::ConvertFromUnicodeToUtf8( text8, *buf ) )
                        {
                        ATDEBUG( DebugPrint( _L( "CnvUtfConverter::ConvertFromUnicodeToUtf8 failed" ) ) )
                        }
                    else
                        {
                        // Conversion succeeded
                        // Add data to sending table
                        apArray->AppendL(text8);
                        }
                    }
                }
            CleanupStack::PopAndDestroy(); // connectionMethod
            }
        tmpName = NULL;
        CleanupStack::PopAndDestroy(2); // destination
        }
    CleanupStack::PopAndDestroy(3); // cmManager
    TRACE_FUNC_EXIT
    return;
    }

/**
 Add quotation marks
 */
void CPamEngine::AddQuotationmarks( const TDesC& aString, HBufC16*& aBuf )
    {
    TRACE_FUNC_ENTRY
    _LIT( KQuotationmark, "\"" );

    if ( aString.Length() > 0 )
        {
        aBuf->Des().Append( KQuotationmark );
        aBuf->Des().Append( aString );
        aBuf->Des().Append( KQuotationmark );
        }
	TRACE_FUNC_EXIT
    return;
    }


// End of File

