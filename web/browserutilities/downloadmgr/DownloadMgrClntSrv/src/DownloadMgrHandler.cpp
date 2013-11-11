/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     This file contains the declaration of the  receive handler of Download Mgr Server.
*
*/


// INCLUDE FILES
#include "DownloadMgrLogger.h"
#include "DownloadMgrHandler.h"
#include "DownloadMgrClient.h"
#include "DownloadMgrDefAttrib.h"

// CONSTANTS


// GLOBAL FUNCTIONS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDownloadMgrHandler::CDownloadMgrHandler
// ---------------------------------------------------------
//
CDownloadMgrHandler::CDownloadMgrHandler( MHttpDownloadMgrObserver* aHandler,
                                          RHttpDownload* aSubSession )
:CActive( EPriorityStandard + 2 ) /* EPriorityStandard + 2 is used until EHttpProgResponseBodyReceived
                                    event is received, then it must be set to EPriorityLow 
                                    until an other event arrives. Then it is set back to 
                                    EPriorityStandard + 2. */
,iPtr(0,0)                             
    {
    CLOG_CREATE;
    
    CLOG_ENTERFN( "CDownloadMgrHandler::CDownloadMgrHandler" )
    iHandler = aHandler;
    iDownloadSubSession = aSubSession;
    iDownloadSession = NULL;
    CActiveScheduler::Add( this );
    iClosed = NULL;
    iHandlerType = EHttpDownload;
    iPrevDlEvent.iDownloadState = THttpDownloadState(0); // None
    iPrevDlEvent.iProgressState = EHttpProgNone;
    //iPrevDlEvent.iMoIndex = 0;
    }

// ---------------------------------------------------------
// CDownloadMgrHandler::CDownloadMgrHandler
// ---------------------------------------------------------
//
CDownloadMgrHandler::CDownloadMgrHandler( RHttpDownloadMgr* aSession )
:CActive( EPriorityStandard ) /* Session-level event handler: priority remains 
                                 EPriorityStandard! */
,iPtr(0,0)                                 
    {
    CLOG_CREATE;
    
    CLOG_ENTERFN( "CDownloadMgrHandler::CDownloadMgrHandler" )
    iHandler = NULL;
    iDownloadSubSession = NULL;
    iDownloadSession = aSession;
    CActiveScheduler::Add( this );
    iClosed = NULL;
    iHandlerType = EHttpDownloadMgr;
    iPrevDlEvent.iDownloadState = THttpDownloadState(0); // None
    iPrevDlEvent.iProgressState = EHttpProgNone;
    //iPrevDlEvent.iMoIndex = 0;
    }

// ---------------------------------------------------------
// CDownloadMgrHandler::CDownloadMgrHandler
// ---------------------------------------------------------
//
CDownloadMgrHandler::~CDownloadMgrHandler()
    {
    CLOG_WRITE( "CDownloadMgrHandler::~CDownloadMgrHandler" )
    Cancel();
    CLOG_WRITE_FORMAT( "this: 0x%x", this );
    

    CLOG_WRITE_FORMAT( "~iClosed: 0x%x", iClosed );
    if( iClosed )
        {
        CLOG_WRITE( "*iClosed = ETrue;" );
        *iClosed = ETrue;
        }

        
    if( iPackedEventAttribs )
        {
        delete iPackedEventAttribs;
        }
        
    CLOG_CLOSE;
    }

// ---------------------------------------------------------
// CDownloadMgrHandler::AdjustPriority
// ---------------------------------------------------------
//
void CDownloadMgrHandler::AdjustPriority( THttpDownloadEvent& aCurrentDlEvent )
    {
    if( !iDownloadSubSession->EventPriorityFlag() )
    //The flag is zero if there is no cod AND client side download
        {
        if ( aCurrentDlEvent.iProgressState == EHttpProgCodDescriptorDownloaded )
            {
            // Cod download to be started in client side
            // Set priority to the original (EPriorityStandard + 2) value.
            CLOG_WRITE( "Setting EPriorityStandard + 2" );            
            }        
        else if( aCurrentDlEvent.iProgressState == EHttpProgResponseBodyReceived && 
            iPrevDlEvent.iProgressState != EHttpProgResponseBodyReceived )
            { // Progress changes to EHttpProgResponseBodyReceived
            // Set priority to a low value.
            CLOG_WRITE( "Setting EPriorityStandard + 1" );
            SetPriority( EPriorityStandard + 1 );
            }
        else if ( aCurrentDlEvent.iProgressState != EHttpProgResponseBodyReceived && 
                  iPrevDlEvent.iProgressState == EHttpProgResponseBodyReceived )
            { // Progress changes from EHttpProgResponseBodyReceived
            // Set priority to the original (EPriorityStandard + 2) value.
            CLOG_WRITE( "Setting EPriorityStandard + 2" );
            SetPriority( EPriorityStandard + 2 );
            }

        // Otherwise no changes in priority!
        
        iPrevDlEvent = aCurrentDlEvent; // Remember for the event
        }
    }

// ---------------------------------------------------------
// CDownloadMgrHandler::RunL
// ---------------------------------------------------------
//
void CDownloadMgrHandler::RunL()
    {
    CLOG_WRITE_FORMAT( " CDownloadMgrHandler::RunL DH-iStat: %d", iStatus.Int() );
    if ( iStatus.Int() == KErrNone )
        {
        THttpDownloadEvent event;
        event.iDownloadState = (THttpDownloadState)iDownloadState();
        event.iProgressState = (THttpProgressState)iProgressState();
        CLOG_WRITE_FORMAT( "DS: %d", event.iDownloadState );
        CLOG_WRITE_FORMAT( "PS: %d", event.iProgressState );
        CLOG_WRITE_FORMAT( "Handler Type: %d", iHandlerType );
        CLOG_WRITE_FORMAT( "this: 0x%x", this );
        switch( iHandlerType )
            {
            case EHttpDownload:
                {
                TBool closed( EFalse );
                iClosed = &closed;
                CLOG_WRITE_FORMAT( "iClosed before: %d", *iClosed );
                
                // *** Do active object priority adjustments here ***
                AdjustPriority( event );
                TPtr8 ptr = iPackedEventAttribs->Des();
                iDownloadSubSession->UnpackEventAttribL( ptr );
                
                // Media object index
                //event.iMoIndex = iDownloadSubSession->ActiveMoIndex();
                
                if( EHttpDlDeleting == event.iDownloadState )
                    {
                    iDownloadSubSession->Delete();
					iClosed = NULL;
                    }
                else
                    {
                    iHandler->HandleDMgrEventL( *iDownloadSubSession, event );
                    }
                // Check if the download subsession has been closed by the client
                CLOG_WRITE_FORMAT( "closed: %d", closed );
                iClosed = NULL;
                if( !closed )
                    {
                    StartL();
                    }
                break;
                }
            case EHttpDownloadMgr:
                {
                // Unmask iHandle into 'handle' and 'media index'
                TInt handle = (iHandle() & 0x0000FFFF);
                //event.iMoIndex = (iHandle() >> 16);
                TBool closed( EFalse );
                iClosed = &closed;
                iDownloadSession->HandleSessionEventL( handle, event );
                CLOG_WRITE_FORMAT( "closed: %d", closed );
                iClosed = NULL;
                if( !closed )
                    {
                    StartL();
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else
        {
        //Should be got here!
        }
    }

// ---------------------------------------------------------
// CDownloadMgrHandler::DoCancel
// ---------------------------------------------------------
//
void CDownloadMgrHandler::DoCancel()
    {
    CLOG_ENTERFN( "CDownloadMgrHandler::DoCancel" )
    switch( iHandlerType )
        {
        case EHttpDownload:
            {
            iDownloadSubSession->CancelEventNotification();
            break;
            }
        case EHttpDownloadMgr:
            {
            iDownloadSession->CancelSessionEventNotification();
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CDownloadMgrHandler::StartL
// ---------------------------------------------------------
//
void CDownloadMgrHandler::StartL()
    {
    CLOG_ENTERFN( "CDownloadMgrHandler::StartL" )
    if( !IsActive() )
        {
        iStatus = KRequestPending;
        switch( iHandlerType )
            {
            case EHttpDownload:
                {
                //Use packer to alloc buffer
                HBufC8* temp_packer = TDMgrUtils::PackEventAttribL();
                if( iPackedEventAttribs )
                    {
                    delete iPackedEventAttribs;
                    }                
                iPackedEventAttribs = temp_packer;
                CLOG_WRITE( "EHttpDownload" );
                TInt length = iPackedEventAttribs->Des().Length();
                iPtr.Set( iPackedEventAttribs->Des() );
                iDownloadSubSession->StartEventNotification( iDownloadState,
                                                             iProgressState,
                                                             iPtr,
                                                             iStatus );
                break;
                }
            case EHttpDownloadMgr:
                {
                CLOG_WRITE( "EHttpDownloadMgr" );
                iDownloadSession->StartSessionEventNotification( iDownloadState,
                                                                 iProgressState,
                                                                 iHandle,
                                                                 iStatus );
                break;
                }
            default:
                {
                break;
                }
            }
        SetActive();
        }
    }
