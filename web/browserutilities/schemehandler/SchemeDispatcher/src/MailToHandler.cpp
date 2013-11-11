/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Implementation of Scheme handler interface implementation for mailto:// scheme
*      
*
*/


// INCLUDE FILES

#include "MailToHandler.h"
#include "SchemeDispLogger.h"
#include <ecom/ecom.h>		// For REComSession
#include <eikenv.h>
#include <DocumentHandler.h>
#include <apgcli.h>
#include <apparc.h>
#include <eikdoc.h>
#include <eikproc.h>
#include <f32file.h>


// ================= CONSTANTS =======================

_LIT( KMailto,"mailto:");
_LIT( KSubject, "subject=" );
_LIT( KBody, "body=" );
_LIT( KCc, "cc=" );
_LIT( KTo, "to=" );
_LIT( KBcc, "bcc=" );
_LIT( KComma, "," );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMailToHandler::NewL()
// ---------------------------------------------------------
//
CMailToHandler* CMailToHandler::NewL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CMailToHandler::NewL()" );

	CMailToHandler* self=new(ELeave) CMailToHandler();
	CleanupStack::PushL(self);
	self->ConstructL( aUrl );
	CleanupStack::Pop(self);

	CLOG_LEAVEFN( "CMailToHandler::NewL()" );

	return self;
	}

// ---------------------------------------------------------
// CMailToHandler::~CMailToHandler()
// ---------------------------------------------------------
//
CMailToHandler::~CMailToHandler()
	{
	CLOG_ENTERFN( "CMailToHandler::~CMailToHandler()" );

    if( iTelService != NULL )
        {
	    iTelService->RemoveObserver(this);
	    delete iTelService;
        iTelService = NULL;
        }

	CLOG_LEAVEFN( "CMailToHandler::~CMailToHandler()" );
	}

// ---------------------------------------------------------
// CMailToHandler::CMailToHandler()
// ---------------------------------------------------------
//
CMailToHandler::CMailToHandler() : CBaseHandler()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation completion.
	}

// ---------------------------------------------------------
// CMailToHandler::ConstructL()
// ---------------------------------------------------------
//
void CMailToHandler::ConstructL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CMailToHandler::ConstructL()" );

	BaseConstructL( aUrl );

	CLOG_LEAVEFN( "CMailToHandler::ConstructL()" );
	}

// ---------------------------------------------------------
// CMailToHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CMailToHandler::HandleUrlEmbeddedL()
	{
	CLOG_ENTERFN( "CMailToHandler::HandleUrlEmbeddedL()" );

    //TPtrC path = iParsedUrl->Des();

	iTelService = CBrowserTelService::NewL();
	iTelService->AddObserver( this );

	TPtrC recipient = GetField( KMailto );
	TPtrC subject = GetField( KSubject );
	TPtrC msgBody = GetField( KBody );
	TPtrC cC = GetField( KCc );
    TPtrC tO = GetField( KTo );
    TPtrC bCC = GetField( KBcc );

    HBufC* rec = ChangeSeparationLC( recipient );
    HBufC* ccrec = ChangeSeparationLC( cC );
    HBufC* torec = ChangeSeparationLC( tO );
    HBufC* bccrec = ChangeSeparationLC( bCC );

    HBufC* allrec = HBufC::NewLC( ccrec->Length() +
                                  torec->Length() +
                                  bccrec->Length() + 3*KComma().Length() );
    if( ccrec->Length() != 0 )
        {
        if( allrec->Length() != 0 )
            {
            allrec->Des().Append( KComma() );
            }
        allrec->Des().Append( ccrec->Des() );
        }
    if( torec->Length() != 0 )
        {
        if( allrec->Length() != 0 )
            {
            allrec->Des().Append( KComma() );
            }
        allrec->Des().Append( torec->Des() );
        }
    if( bccrec->Length() != 0 )
        {
        if( allrec->Length() != 0 )
            {
            allrec->Des().Append( KComma() );
            }
        allrec->Des().Append( bccrec->Des() );
        }

    if( rec->Length() > 0 )
        {
        TChar ch1('?');
        TChar ch2('&');
        TChar recchar((*rec)[ rec->Length() - 1]);
        if( recchar == ch1 )
            {
            rec->Des().SetLength(rec->Length() - 1);
            }
        TChar recchar2((*rec)[ rec->Length() - 1]);
        if( recchar2 == ch2 )
            {
            rec->Des().SetLength(rec->Length() - 1);
            }
        }

    if( allrec->Length() > 0 )
        {
        TChar ch1('?');
        TChar ch2('&');
        TChar allrecchar1( (*allrec)[ allrec->Length() - 1] );
        if( allrecchar1 == ch1 )
            {
            allrec->Des().SetLength(allrec->Length() - 1);
            }
        TChar allrecchar2( (*allrec)[ allrec->Length() - 1] );
        if( allrecchar2 == ch2 )
            {
            allrec->Des().SetLength(allrec->Length() - 1);
            }
        }

 	TRAPD( err, iTelService->SendEmailMessageL( rec->Des(), allrec->Des(), subject, msgBody, ETrue) );

    CleanupStack::PopAndDestroy( 5 ); // rec, ccrec, torec, bccrec, allrec

    NotifyClient();

    ErrorHandlerL( err );

	CLOG_LEAVEFN( "CMailToHandler::HandleUrlEmbeddedL()" );
	}

// ---------------------------------------------------------
// CMailToHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CMailToHandler::HandleUrlStandaloneL()
	{
	CLOG_ENTERFN( "CMailToHandler::HandleUrlStandaloneL()" );

    LaunchSchemeAppWithCommandLineL();

	CLOG_LEAVEFN( "CMailToHandler::HandleUrlStandaloneL()" );
	}

// ---------------------------------------------------------
// CMailToHandler::GetField()
// ---------------------------------------------------------
//
TPtrC CMailToHandler::GetField(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CMailToHandler::GetField()" );

	TPtrC path = iParsedUrl->Des();

	TInt start = 0;
	TInt end = 0;

	CMailToHandler::TSchemeMailToFields posHeader = GetHeaderPos( aHeader );
	// field start from; and end at 
	switch( posHeader )
		{
		case ESchemeMailTo: // MailTo
			{
			start = FieldStart( KMailto );
			end = FieldEnd( KMailto );
			break;
			}
		case ESchemeSubject: // Subject
			{
			start = FieldStart( KSubject );
			end = FieldEnd( KSubject );
			break;
			}
		case ESchemeMsgBody: // Msg Body
			{
			start = FieldStart( KBody );
			end = FieldEnd( KBody );
			break;
			}
		case ESchemeCc: // Cc
			{
			start = FieldStart( KCc );
			end = FieldEnd( KCc );
			break;
			}
		case ESchemeTo: // To
			{
			start = FieldStart( KTo );
			end = FieldEnd( KTo );
			break;
			}
		case ESchemeBcc: // Bcc
			{
			start = FieldStart( KBcc );
			end = FieldEnd( KBcc );
			break;
			}
		case ESchemeNoMore:
			break;
		}

    // check that everything is OK
    if( start > end )
        {
        end = path.Length();
        }
	CLOG_LEAVEFN( "CMailToHandler::GetField()" );

	return path.Mid( start, end-start );
	}

// ---------------------------------------------------------
// CMailToHandler::IsHeader()
// ---------------------------------------------------------
//
TBool CMailToHandler::IsHeader(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CMailToHandler::IsHeader()" );

	TBool retVal = EFalse;

	TPtrC path = iParsedUrl->Des();

	/* is the field in the mailto sheme */
	if( KErrNotFound != path.FindF( aHeader ) )
		{
		retVal = ETrue;
		}

	CLOG_LEAVEFN( "CMailToHandler::IsHeader()" );

	return retVal;
	}

// ---------------------------------------------------------
// CMailToHandler::FieldStart()
// ---------------------------------------------------------
//
TInt CMailToHandler::FieldStart(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CMailToHandler::FieldStart()" );

	TPtrC path = iParsedUrl->Des();
	TInt retVal = path.Length();

	/* find the starting position of the specific filed */
	if( IsHeader( aHeader ) )
		{
		retVal = path.FindF( aHeader ) + aHeader.Length();
		}

	CLOG_LEAVEFN( "CMailToHandler::FieldStart()" );

	return retVal;
	}

// ---------------------------------------------------------
// CMailToHandler::FieldEnd()
// ---------------------------------------------------------
//
TInt CMailToHandler::FieldEnd(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CMailToHandler::FieldEnd()" );

	TPtrC path = iParsedUrl->Des();
	TInt length = path.Length(); // length of the scheme
    TInt retVal = length;

    TInt startPos = FieldStart( aHeader );

	if( IsHeader( aHeader ) )
		{
        TInt temp = GetNextField( startPos );
        /* we need to subtract 1 if the result is 
           not equal to length because of the & or ? */
        retVal = ( temp == length ) ? length : ( temp - 1);
		}

	CLOG_LEAVEFN( "CMailToHandler::FieldEnd()" );

	return retVal;
	}

// ---------------------------------------------------------
// CMailToHandler::GetHeaderPos()
// ---------------------------------------------------------
//
CMailToHandler::TSchemeMailToFields CMailToHandler::GetHeaderPos(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CMailToHandler::GetHeaderPos()" );

	TSchemeMailToFields retVal = ESchemeMailTo;

	if( 0 == aHeader.Compare( KMailto ) )
		{
		retVal = ESchemeMailTo;
		}
	else if ( 0 == aHeader.Compare( KSubject ) )
		{
		retVal = ESchemeSubject;
		}
	else if ( 0 == aHeader.Compare( KBody ) )
		{
		retVal = ESchemeMsgBody;
		}
	else if ( 0 == aHeader.Compare( KCc ) )
		{
		retVal = ESchemeCc;
		}
	else if ( 0 == aHeader.Compare( KTo ) )
		{
		retVal = ESchemeTo;
		}
	else if ( 0 == aHeader.Compare( KBcc ) )
		{
		retVal = ESchemeBcc;
		}

	CLOG_LEAVEFN( "CMailToHandler::GetHeaderPos()" );

	return retVal;
	}

// ---------------------------------------------------------
// CMailToHandler::GetNextField()
// ---------------------------------------------------------
//
TInt CMailToHandler::GetNextField( TInt aStart )
    {
	TPtrC path = iParsedUrl->Des();
	TInt retVal = path.Length();
    TPtrC scheme;

    //KSubject KBody KCc
    if( aStart < retVal )
        {
        scheme.Set( path.Right( retVal - aStart ) );
        }
    else
        {
        return retVal;
        }

    TInt subjPos = scheme.FindF( KSubject );
    subjPos = ( subjPos == KErrNotFound ) ? retVal : subjPos;

    TInt bodyPos = scheme.FindF( KBody );
    bodyPos = ( bodyPos == KErrNotFound ) ? retVal : bodyPos;

    TInt toPos = scheme.FindF( KTo );
    toPos = ( toPos == KErrNotFound ) ? retVal : toPos;

    TInt ccPos = scheme.FindF( KCc );
    ccPos = ( ccPos == KErrNotFound ) ? retVal : ccPos;

    TInt bccPos = scheme.FindF( KBcc );
    bccPos = ( bccPos == KErrNotFound ) ? retVal : bccPos;

    TInt temp = Minimum( subjPos, bodyPos, toPos, ccPos, bccPos );
    retVal = ( temp < retVal) ? temp + aStart : retVal;

    return retVal;
    }


// ---------------------------------------------------------
// CMailToHandler::Minimum()
// ---------------------------------------------------------
//
TInt CMailToHandler::Minimum( TInt aPos1, TInt aPos2, TInt aPos3, TInt aPos4, TInt aPos5 )
    {
    TInt min = Min( aPos1, aPos2 );
    min = Min( min, aPos3 );
    min = Min( min, aPos4 );
    min = Min( min, aPos5 );

    return min;
    }

// ---------------------------------------------------------
// CMailToHandler::BrowserTelServiceEvent()
// ---------------------------------------------------------
//
void CMailToHandler::BrowserTelServiceEvent( TBrowserTelServiceState /*aEvent*/)
	{
	}

// ---------------------------------------------------------
// CMailToHandler::BrowserTelServiceError()
// ---------------------------------------------------------
//
void CMailToHandler::BrowserTelServiceError( TBrowserTelServiceError /*aError*/)
	{
	}

// ---------------------------------------------------------
// CMailToHandler::NotifyClient()
// ---------------------------------------------------------
//
void CMailToHandler::NotifyClient()
	{
	if( NULL !=iSchemeDoc )
		{
        iSchemeDoc->HandleServerAppExit( KErrNone );
		}
	}

// ---------------------------------------------------------
// CMailToHandler::ChangeSeparationLC()
// ---------------------------------------------------------
//
HBufC* CMailToHandler::ChangeSeparationLC( const TDesC& aRecipients )
    {
    HBufC* newBuffer = aRecipients.AllocLC();

    TInt pos;
    while( KErrNotFound != ( pos = newBuffer->FindF( KTo ) ) )
        {
        TPtr16 ptr = newBuffer->Des();
        ptr.Replace( pos, KTo().Length(), KComma );
        }

    return newBuffer;
    }
