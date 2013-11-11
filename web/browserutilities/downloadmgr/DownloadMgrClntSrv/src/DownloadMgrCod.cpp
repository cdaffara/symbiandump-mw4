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
*     This file contains the declaration of transaction observer of Download Mgr Server.
*
*/


// INCLUDE FILES
#include "DownloadMgrClient.h"
#include "DownloadMgrCod.h"
#include "DownloadMgrLogger.h"
#include <apgtask.h>
#include <eikenv.h>

// CONSTANTS
const TInt KBrowserAppUid = 0x10008D39;
// Enough data retrieved for recogntion of internal type in case of DRM
const TInt KRespSizeForRecognition = 1024;  //for THttpProgressState EHttpContTypeRecognitionAvail

const TInt KDefaultCodDownloadLength = -1;

// GLOBAL FUNCTIONS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodObserver::NewL
// ---------------------------------------------------------
//
CCodObserver* CCodObserver::NewL( RHttpDownloadMgr* aDownloadMgr )
    {
    CLOG_ENTERFN_NULL("CCodObserver::NewL");
    
    CCodObserver* self = new( ELeave ) CCodObserver( aDownloadMgr );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CCodObserver::CCodObserver
// ---------------------------------------------------------
//
CCodObserver::CCodObserver( RHttpDownloadMgr* aDownloadMgr )
    {
    CLOG_CREATE;
    CLOG_ENTERFN("CCodObserver::CCodObserver");
    
    iDownloadMgr = aDownloadMgr;
    
    iCodLength = KDefaultCodDownloadLength;
    iReadyForPD = EFalse;
    }

// ---------------------------------------------------------
// CCodObserver::ConstructL
// ---------------------------------------------------------
//
void CCodObserver::ConstructL()
    {
    CLOG_ENTERFN("CCodObserver::ConstructL");
    iFilename  = HBufC::NewMaxL( KMaxPath );
    }

// ---------------------------------------------------------
// CCodObserver::CCodObserver
// ---------------------------------------------------------
//
CCodObserver::~CCodObserver()
    {
    CLOG_ENTERFN("CCodObserver::~CCodObserver");
    
    delete iFilename;
    
    CLOG_CLOSE;
    }

// ---------------------------------------------------------
// CCodObserver::CodEventL
// ---------------------------------------------------------
//
void CCodObserver::CodEventL( CCodDownload& aDownload, TEvent aEvent )
    {
    CLOG_ENTERFN("CCodObserver::CodEventL");

    TInt handle =
        ( (SDMgrCodUserData*)( aDownload.UserData() ) )->iHandle;
    RHttpDownload* download = NULL;
    // This leaves when there is no download assotiated with handle   
    TRAPD( err, download = &iDownloadMgr->DownloadFromHandleL( handle ) );
    if( err )
        {
        return;
        }
    
    const TCodDownloadProgress& progress = aDownload.Progress();
    SDMgrCodUserData* ptr =(SDMgrCodUserData*)( aDownload.UserData() );
    ptr->iPrevCodEvent = aEvent;

    switch( aEvent )
    	{
    	case EStart:
    		{
    		CLOG_WRITE( "TEvent EStart");
            download->SetCodInProgress( RHttpDownload::EInProgress );
			}
    		break;
    		
    	case ENameChanged:
    		{
    		CLOG_WRITE( "TEvent ENameChanged");
    		
    		TPtrC ptr = aDownload.Name();
    		download->SetStringAttribute( EDlAttrName, ptr );
    		}
    		break;
    		
    	case ETypeChanged:
    		{
    		CLOG_WRITE( "TEvent ETypeChanged");
    		
    		TPtrC8 ptr = aDownload.Type();
    		download->SetStringAttribute( EDlAttrContentType, ptr );
    		}
    		break;
    		
    	case EProgressUpdate:
    		{
            CLOG_WRITE( "TEvent EProgressUpdate");

    	    
            TInt32 bytesCur = progress.iBytesCur;
            TInt32 bytesMax = progress.iBytesMax;
 
            /*
            *   Update the length and send only if it is changed
            */
            if(iCodLength != bytesMax)
    		    {
                iCodLength = bytesMax;
                download->SetIntAttribute( EDlAttrLength, bytesMax );
                }
 
    		
            download->SetIntAttribute( EDlAttrDownloadedSize, bytesCur );
            
            TPtrC ptr = aDownload.GetPath();
            
            /*
            *   Store the destination file name and temprory file name so that next time send it only if it is changed
            */
            if( 0 != ptr.Compare(iFilename->Des()))
                {
                iFilename->Des().Copy(ptr); ;
                download->SetStringAttribute( EDlAttrDestFilename, ptr );            
                download->SetStringAttribute( EDlAttrLocalFileName, ptr );
                }
            

            if ( iReadyForPD && iDlAttrCodPdAvailableSet && ( bytesCur >= KRespSizeForRecognition ) )
                {
                TInt32 numMediaObjects;
                download->GetIntAttribute( EDlAttrNumMediaObjects,numMediaObjects );
                if( numMediaObjects>1 )
                	{
                	//No progressive play for album.So don't set EDlAttrCodPdAvailable to ETrue.
                	download->SetBoolAttribute( EDlAttrCodPdAvailable, EFalse );
                	}
                else
                	{
                	download->SetBoolAttribute( EDlAttrCodPdAvailable, ETrue );
                	}

                iDlAttrCodPdAvailableSet = EFalse;
                }
    		}
    		break;
    		
    	case EAccept:
    	    {
    	    CLOG_WRITE( "TEvent EAccept");
    	    download->SetBoolAttribute( EDlAttrCodDescriptorAccepted, ETrue );
    	    TBool removableMedia = aDownload.RemovableMedia();
    	    download->SetBoolAttribute( EDlAttrDestRemovable, removableMedia );
    	    download->iASReady = ETrue;
    	    }
    	    break;
    	    
    	case EEndLoad:
    	    {
    	    CLOG_WRITE( "TEvent EEndLoad");
    	    download->SetBoolAttribute( EDlAttrCodLoadEnd, ETrue );
			download->StopWaitingAS();
    	    }
    	    break;
    	
    	case EHandlePostResponse:
    		{
    		CLOG_WRITE( "TEvent EHandlePostResponse");
    		HBufC8* url8 = NULL;
			TRAP( err, url8 = aDownload.GetPostResponseUrlL() );
			if( ( KErrNone == err ) && ( NULL != url8 )  )
                {
                // Using NextUriOBserver() for initiating the PostResponseURL download	                   
             	TRAP_IGNORE( iDownloadMgr->NextUriObserver()->NextUriL( *download, *url8 ) );
                }
    		}
    		break;
    		
    	case EDone:
    		{
    		CLOG_WRITE( "TEvent EDone");

            download->SetCodInProgress( RHttpDownload::ENotActive );
    		if ( download->IsDeleteCodWhenDone() )
    		    {
                CLOG_WRITE( "COD done, delete DL");
//    		    download->Delete();
    		    download->StopWaitingAS();
    		    }
                TBool doNextUrl( EFalse );
        		switch( progress.iState )
        		    {
        		    case TCodDownloadProgress::ESucceeded:
						{
                        CLOG_WRITE( "TCodDownloadProgress::ESucceeded");
                        
                        for(TInt32 i=1; ; i++)
                            {
                            TPtrC ptr = aDownload.GetDestFilePath(i);
                            if( ptr.Length() == 0)
                                {
                                break;
                                }
                            download->SetStringAttribute( EDlAttrDestFilename, i, ptr );
                            }
                        
                        doNextUrl = ETrue;
                        TBool isProgressive = EFalse;
                        download->GetBoolAttribute( EDlAttrProgressive, isProgressive );
                        // The browser client should not move the file in case of progressive download.
                        // It should be done by the MP/VP after the playback we be done.
                        if (!isProgressive)
						    {
                            download->SetIntAttribute( EDlAttrSucceeded, 0 );
						    }
                        else
                            {
                            download->SetIntAttribute( EDlAttrSucceeded, 1 );
                            }						    
						}
        		        break;       		    

        		    case TCodDownloadProgress::EFailedTemporary:
        		    case TCodDownloadProgress::EFailedPermanent:
        		        {
                        CLOG_WRITE_FORMAT( "TCodDownloadProgress::EFailedXXX %d", progress.iError );

                        // bring the error message to forground
                        if( CEikonEnv::Static() )
                        {
	                        TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
	                        TApaTask task = taskList.FindApp( TUid::Uid( KBrowserAppUid ) );
	                        if ( task.Exists() )
	                        {
	                            task.BringToForeground();
	                        }
                        	
                        }

                        if ( progress.iError <= -1000 )
                        	{
                        	download->SetOnError( KErrGeneral, (THttpDownloadMgrError)progress.iError );
                        	}
                        else
                        	{
                        	download->SetOnError( progress.iError );
                        	}
                        	
                        doNextUrl = ETrue;
        		        }
        		        break;
        		        
        		    case TCodDownloadProgress::EPaused:
        		        {
                        CLOG_WRITE_FORMAT( "TCodDownloadProgress::EPaused %d", progress.iError );
						download->SetCodInProgress( RHttpDownload::EPaused );
						TInt32 dlState;
						download->GetIntAttribute( EDlAttrState, dlState );
						if( dlState != EHttpDlPaused)
						    {
                            download->Pause();

                            if ( progress.iError <= -1000 )
                            	{
                            	download->SetOnError( KErrGeneral, (THttpDownloadMgrError)progress.iError );
                            	}
                            else
                            	{
                            	download->SetOnError( progress.iError );
                            	}
						    }
                        doNextUrl = EFalse;
        		        }
        		        break;
    				}
                if( doNextUrl )
                    {
                    // follow NextURL
                    if( NULL != iDownloadMgr->NextUriObserver() )
                        {
                        HBufC8* url8 = NULL;
                        TRAP( err, url8 = aDownload.NextUrlL() );
                        if( ( KErrNone == err ) && ( NULL != url8 )  )
                            {                   
                            TRAP_IGNORE( 
                                iDownloadMgr->NextUriObserver()->NextUriL( *download, *url8 ) );
                            if( url8 )
                                {
                                delete url8;
                                }                            
                            }
                        }
                    }
    		}
    		break;

        case EPdPlayAvailable:
			{
              CLOG_WRITE( "TEvent EPdPlayAvailable");
              iDlAttrCodPdAvailableSet = ETrue;
			}
            break;

        case EUpdatedDDURI:
			{
            CLOG_WRITE( "TEvent EUpdatedDDURI");
            HBufC8* updatedDDUri8 = NULL;
            TRAP( err, updatedDDUri8 = aDownload.UpdatedDDUriL() );              
            download->SetBoolAttribute( EDlAttrDownloadUpdatedDDUri, ETrue );
            download->SetStringAttribute( EDlAttrUpdatedDDUri, *updatedDDUri8 );
            download->SetBoolAttribute ( EDlAttrCodDownload , EFalse);
            download->Start();
			}
            break;
            
        case EResumed:
    		{
    		iReadyForPD = ETrue;
    		iDlAttrCodPdAvailableSet = ETrue;
    		}
    		break;
    		
        case EDownloadPausable:
    		{
            download->SetBoolAttribute ( EDlAttrCodPausable , ETrue);
    		}
    		break;    		

    	case EDownloadNonPausable:
    		{
            download->SetBoolAttribute ( EDlAttrCodPausable , EFalse);
    		}
    		break;    		
    		
    	case EUpdatedMediaInfo:
    	    {
    	    HBufC8* mediaInfo8 = NULL;
    	    TRAP( err, mediaInfo8 = aDownload.UpdatedDownloadDataL() );
    	    if (mediaInfo8)
    	        {
    	        download->SetDownloadDataAttribute(*mediaInfo8);
    	        delete mediaInfo8;
    	        }
    	    }
    	    break;
    	    
	    case ESetActiveDownload:
	        {
            download->SetIntAttribute ( EDlAttrActiveDownload , aDownload.ActiveDownload());	        
	        }
	    break;
	    
    	case EUpdatedTrackInfo:
    		{
    		HBufC8* trackInfo8 = NULL;
    		TInt trackIndex = 0;
    	    TRAP( err, trackInfo8 = aDownload.UpdatedTrackDataL(trackIndex) );
    	    if (trackInfo8)
    	        {
    	        download->SetTrackDataAttribute(trackIndex, *trackInfo8);
    	        delete trackInfo8;
    	        }
    	    }
    		break;
        default:
         	{
         	__ASSERT_DEBUG( EFalse , User::Panic( _L( "DMgrServer" ), KErrNotFound ) );
         	}
    	}    
    }
