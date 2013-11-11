/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include "DownloadMgrEventQueue.h"
#include "DownloadMgrLogger.h"
#include <HttpDownload.h>

#ifdef __DOWNLOADMGR_LOG__
//#define __DUMP_EVENT_QUEUE
#ifdef __DUMP_EVENT_QUEUE
#pragma message ("Dump event queue!!!")
#endif /* __DUMP_EVENT_QUEUE */
#endif /*__DOWNLOADMGR_LOG__*/
// ============================= LOCAL FUNCTIONS ===============================

// ---------------------------------------------------------
// CDownloadMgrEventQueue::CDownloadMgrEventQueue
// ---------------------------------------------------------
//
CDownloadMgrEventQueue::CDownloadMgrEventQueue()
	{
	__DECLARE_NAME( _S( "CDownloadMgrEventQueue" ) );
	CLOG_CREATE;
	}

// ---------------------------------------------------------
// CDownloadMgrEventQueue::~CDownloadMgrEventQueue
// ---------------------------------------------------------
//
CDownloadMgrEventQueue::~CDownloadMgrEventQueue()
	{
	iEventQueue.ResetAndDestroy();
	CLOG_CLOSE;
	}

// ---------------------------------------------------------
// CDownloadMgrEventQueue::In
// ---------------------------------------------------------
//
TBool CDownloadMgrEventQueue::In( CHttpDownload* aDownload, 
                                  THttpDownloadState aDlState,
                                  THttpProgressState aProgState,
                                  TInt32 aMoIndex )
	{
    CLOG_ENTERFN( "CDownloadMgrEventQueue::In" );

    if( aDlState == EHttpDlInprogress && 
        aProgState == EHttpProgResponseBodyReceived && 
        iEventQueue.Count() )
        // If EHttpDlInprogress-EHttpProgResponseBodyReceived is last
        // event in the queue, it doesn't need to be appended again.
        {
        CLOG_WRITE( "In-progress filter" );
        TInt count = iEventQueue.Count();
        TInt32 dlState;
        TInt32 progState;
        TInt32 moIndex;

        CEventRecord* record = iEventQueue[count-1];
        record->States( dlState, progState, moIndex );

        if( aDlState == dlState &&
            aProgState == progState &&
            aMoIndex == moIndex &&
            aDownload == record->Download())
            {
            CLOG_WRITE( "Event filtered" );
            return ETrue;
            }
        }

    CEventRecord* record = new CEventRecord( aDownload, aDlState, aProgState, aMoIndex );
    if( record == NULL )
        {
        return EFalse;
        }
        
    CLOG_WRITE_FORMAT( "CDownloadMgrEventQueue::In: 0x%x", record );
        
    TInt err = iEventQueue.Append( record );
    if( KErrNone != err )
        {
        delete record;
        return EFalse;
        }
#ifdef __DUMP_EVENT_QUEUE
    CLOG_WRITE( "=====================>>>>" );
    CLOG_WRITE_FORMAT( "CDownloadMgrEventQueue::In Download: 0x%x", aDownload )
    CLOG_WRITE_FORMAT( "CDownloadMgrEventQueue::In State: %d", aDlState )
    CLOG_WRITE_FORMAT( "CDownloadMgrEventQueue::In Progress: %d",aProgState )
    CLOG_WRITE_FORMAT( "CDownloadMgrEventQueue::In Media Index: %d",aMoIndex )
    CLOG_WRITE( "Dump event queue---------" );
    for( TInt i = 0; i < iEventQueue.Count(); i++ )
        {
        CLOG_WRITE_FORMAT( "i= %d", i )
        CEventRecord* r = iEventQueue[i];
        CLOG_WRITE_FORMAT( "Download: 0x%x", r->Download() )
        TInt32 state;
        TInt32 progress;
        TInt32 mediaIndex;
        r->States( state, progress, mediaIndex );
        CLOG_WRITE_FORMAT( "Download: 0x%x", r->Download() )
        CLOG_WRITE_FORMAT( "State: %d", state )
        CLOG_WRITE_FORMAT( "Progress: %d", progress )        
        CLOG_WRITE_FORMAT( "Media Index: %d", mediaIndex ) 
        }
    CLOG_WRITE( "=====================>>>>" );
#endif /* __DUMP_EVENT_QUEUE */

    return ETrue;
	}

// ---------------------------------------------------------
// CDownloadMgrEventQueue::Next
// ---------------------------------------------------------
//
CHttpDownload* CDownloadMgrEventQueue::Next( TInt32& aDownloadState,
                                             TInt32& aProgressState,
                                             TInt32& aMoIndex )
	{
    CLOG_ENTERFN( "CDownloadMgrEventQueue::Next" );
    CEventRecord* record = iEventQueue[ 0 ];
    record->States( aDownloadState, aProgressState, aMoIndex );
    CLOG_WRITE( "Next=====================" );
    CLOG_WRITE_FORMAT( "CDownloadMgrEventQueue::Next Download: 0x%x", record->Download() )
    CLOG_WRITE_FORMAT( "CDownloadMgrEventQueue::Next State: %d", aDownloadState )
    CLOG_WRITE_FORMAT( "CDownloadMgrEventQueue::Next Progress: %d",aProgressState ) 
    CLOG_WRITE_FORMAT( "CDownloadMgrEventQueue::Next Media Index: %d",aMoIndex ) 
#ifdef __DUMP_EVENT_QUEUE
   
    CLOG_WRITE( "Dump event queue---------" );
    for( TInt i = 0; i < iEventQueue.Count(); i++ )
        {
        CLOG_WRITE_FORMAT( "i= %d", i )
        CEventRecord* r = iEventQueue[i];
        TInt32 dlState;
        TInt32 progState;
        TInt32 mediaIndex;
        r->States( dlState, progState, mediaIndex );
        CLOG_WRITE_FORMAT( "Download: 0x%x", r->Download() )
        CLOG_WRITE_FORMAT( "State: %d", dlState )
        CLOG_WRITE_FORMAT( "Progress: %d", progState )
        CLOG_WRITE_FORMAT( "Media Index: %d", mediaIndex )
        }
    CLOG_WRITE( "Next=====================" );    
#endif /* __DUMP_EVENT_QUEUE */
    
	return record->Download();
	}

// ---------------------------------------------------------
// CDownloadMgrEventQueue::Out
// ---------------------------------------------------------
//
void CDownloadMgrEventQueue::Out()
	{
    CLOG_ENTERFN( "CDownloadMgrEventQueue::Out" );

    CEventRecord* record = iEventQueue[ 0 ];
    iEventQueue.Remove( 0 );
    CLOG_WRITE_FORMAT( "CDownloadMgrEventQueue::Out Download: 0x%x", record->Download() )
    delete record;
    iEventQueue.Compress();

#ifdef __DUMP_EVENT_QUEUE
    CLOG_WRITE( "<<<<=====================" );
    CLOG_WRITE( "Dump event queue---------" );
    for( TInt i = 0; i < iEventQueue.Count(); i++ )
        {
        CLOG_WRITE_FORMAT( "i= %d", i )
        CEventRecord* r = iEventQueue[i];
        TInt32 dlState;
        TInt32 progState;
        r->States( dlState, progState );        
        CLOG_WRITE_FORMAT( "Download: 0x%x", r->Download() )
        CLOG_WRITE_FORMAT( "State: %d", dlState )
        CLOG_WRITE_FORMAT( "Progress: %d", progState )
        }
    CLOG_WRITE( "<<<<=====================" );
#endif /* __DUMP_EVENT_QUEUE */
	}

// ---------------------------------------------------------
// CDownloadMgrEventQueue::IsEmpty
// ---------------------------------------------------------
//	
TBool CDownloadMgrEventQueue::IsEmpty() const
	{
    CLOG_WRITE( "CDownloadMgrEventQueue::IsEmpty" );
    return ( iEventQueue.Count() ? EFalse : ETrue );
	}

// ---------------------------------------------------------
// CDownloadMgrEventQueue::Remove
// ---------------------------------------------------------
//	
void CDownloadMgrEventQueue::Remove( CHttpDownload* aDownload )
	{
    CLOG_WRITE( "CDownloadMgrEventQueue::Remove" );
    for( TInt i = 0; i < iEventQueue.Count(); i++ )
        {
        if( iEventQueue[i]->IsDownload( aDownload ) )
            {
            CEventRecord* record = iEventQueue[i];
            CLOG_WRITE_FORMAT( "CDownloadMgrEventQueue::Remove Download: 0x%x", record->Download() )
            iEventQueue.Remove( i );
            delete record;
            record = NULL;
            iEventQueue.Compress();
            i--;
            }
        }
	}

// ---------------------------------------------------------
// CEventRecord::CEventRecord
// ---------------------------------------------------------
//
CEventRecord::CEventRecord( CHttpDownload* aDownload, 
                            TInt32 aDownloadState,
                            TInt32 aProgressState,
                            TInt32 aMoIndex )
:iDownloadState( aDownloadState )
,iProgressState( aProgressState )
,iMoIndex( aMoIndex )
    {
    iDownload = aDownload;
    }

// ---------------------------------------------------------
// CEventRecord::~CEventRecord
// ---------------------------------------------------------
//
CEventRecord::~CEventRecord()
    {
    }

// ---------------------------------------------------------
// CEventRecord::States
// ---------------------------------------------------------
//
void CEventRecord::States( TInt32& aDownloadState,
                           TInt32& aProgressState,
                           TInt32& aMoIndex )
    {
    aDownloadState = iDownloadState;
    aProgressState = iProgressState;
    aMoIndex = iMoIndex;
    }

// ---------------------------------------------------------
// CEventRecord::Download
// ---------------------------------------------------------
//
CHttpDownload* CEventRecord::Download()
    {
    return iDownload;
    }

// ---------------------------------------------------------
// CEventRecord::IsDownload
// ---------------------------------------------------------
//
TBool CEventRecord::IsDownload( CHttpDownload* aDownload )
    {
    return (iDownload == aDownload) ? ETrue : EFalse;
    }

//  End of File  
