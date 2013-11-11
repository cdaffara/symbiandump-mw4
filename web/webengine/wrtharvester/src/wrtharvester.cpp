/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


// INCLUDE FILES
#include "wrtharvester.h"
#include "wrtharvesterregistryaccess.h"
#include "wrtharvesterpsnotifier.h"
#include "wrtharvesterpublisherobserver.h"

#include "wrtharvesterconst.h"
#include <WidgetRegistryConstants.h>
#include <wrtharvester.rsg>

#include <implementationproxy.h>

#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include <e32property.h>    //Publish & Subscribe
#include <AknTaskList.h>
#include <apaid.h>
#include <apacmdln.h>
#include <s32mem.h>
#include <apgtask.h>
#include <coemain.h>
#include <bautils.h>
#include <f32file.h>
#include <e32std.h>
#include <AknNotify.h>
#include <AknGlobalConfirmationQuery.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>

// CONSTANTS
_LIT( KResourceFileName, "\\resource\\wrtharvester.rsc" );
_LIT( KResourceDir, "Z:wrtharvester.rsc" );

/**
* Utility class to show the prompt for platform security access.
*
* The class exists only to provide platform security access prompt
* for the widgets which are launched in minview 
*/
class CGlobalQueryHandlerAO : public CActive
    {
public:
    /**
    * Startup.
    *
    * @param aManager Window Manager.
    * @param aWindow Window.
    * @param aMessage Message to be prompted.
    * @param aSoftkeys for prompt.
    * @param aWrtHarvester for callback
    */
    static CGlobalQueryHandlerAO* StartLD (
                            TUid& aUid,
                            const TDesC& aMessage, 
                            TInt aSoftkeys,
                            CWrtHarvester* aWrtHarvester);
    /**
    * ShowGlobalQueryDialogL.
    *
    * @param aMessage Message to be prompted.
    * @param aSoftkeys for prompt.
    */
    void ShowGlobalQueryDialogL ( 
                            const TDesC& aMessage, 
                            TInt aSoftkeys );
protected: // From CActive
    /**
    * Execute asynchronous operation.
    */
    void RunL();
    
    /**
    * Provide cancellation methods.
    */
    void DoCancel();
    
private:

    /**
    * Constructor.
    *
    * @param aManager Manager.
    * @param aWindow Window.
    * @param aMessage Message for prompt.
    * @param aSoftkeys for prompt.
    * @param aWrtHarvester for callback    
    */
    CGlobalQueryHandlerAO (
            TUid& aUid,
            const TDesC& aMessage, 
            TInt aSoftkeys,
            CWrtHarvester* aWrtHarvester);
        
    /**
    * Destructor. 
    *
    * Private on purpose.
    */
    ~CGlobalQueryHandlerAO();
    
private:

    TUid iWidgetUid;
    CAknGlobalConfirmationQuery* iGlobalConfirmationQuery ;
    HBufC* iConfirmationText;
    CWrtHarvester* iWrtHarvester;
    };

// ============================= LOCAL FUNCTIONS ===============================
    
// ----------------------------------------------------------------------------
// Returns the app full name
// ----------------------------------------------------------------------------
//
static HBufC* GetAppNameLC( RApaLsSession& aSession, const TUid& aUid )
    {
    TApaAppInfo info;
    User::LeaveIfError( aSession.GetAppInfo( info, aUid ) );
    
    return info.iFullName.AllocLC();
    }
    
    
// ----------------------------------------------------------------------------
// Sends the command to Widget launcher
// ----------------------------------------------------------------------------
//
static void HandleWidgetCommandL( 
    RApaLsSession& aSession, 
    const TDesC& aWidget,
    const TUid& aUid,
    TUint32 aOperation )
    {
    const TInt size( 2* aWidget.Length() + 3*sizeof( TUint32 ) );
    
    // Message format is <filenameLength><unicode_filename><someintegervalue>
    CApaCommandLine* cmd( CApaCommandLine::NewLC() );
    HBufC8* opaque( HBufC8::NewLC( size ) );
    
    RDesWriteStream stream;
    TPtr8 des( opaque->Des() );
    
    stream.Open( des );
    CleanupClosePushL( stream );
    
    // Generate the command.
    stream.WriteUint32L( aUid.iUid );
    stream.WriteUint32L( aWidget.Length() );
    stream.WriteL( reinterpret_cast< const TUint8* >( aWidget.Ptr() ),
                   aWidget.Size() );
    
    stream.WriteInt32L( aOperation );
    
    CleanupStack::PopAndDestroy( &stream );
    
    // Generate command.
    cmd->SetCommandL( EApaCommandBackgroundAndWithoutViews );
    cmd->SetOpaqueDataL( *opaque );    

    CleanupStack::PopAndDestroy( opaque );
    
    cmd->SetExecutableNameL( KLauncherApp );
    
    User::LeaveIfError( aSession.StartApp( *cmd ) );
    CleanupStack::PopAndDestroy( cmd );
    }

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0xA12345FE, CWrtHarvester::NewL )
    };

// ----------------------------------------------------------------------------
// Exported proxy for instantiation method resolution
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
                                                TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CWrtHarvester::CWrtHarvester( MLiwInterface* aCPSInterface ):
    iCPSInterface( aCPSInterface ),
    iHSCount(0)
	{
	}

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CWrtHarvester::ConstructL()
    {
    User::LeaveIfError( iApaSession.Connect() );
    iWidgetUIListener = CWrtHarvesterPSNotifier::NewL( this, EWidgetUIState );
    iWidgetRegListener = CWrtHarvesterPSNotifier::NewL( this, EWidgetRegAltered );
    
    User::LeaveIfError( iFs.Connect() );
    iWidgetUsbListener = CWrtUsbHandler::NewL( this, iFs );
    
    iWidgetUsbListener->Start();
    SetMSMode(0);
    
    TFileName resourceFileName;  
    TParse parse;    
    Dll::FileName (resourceFileName);           
    parse.Set(KResourceFileName, &resourceFileName, NULL); 
    resourceFileName = parse.FullName(); 
          
    CCoeEnv* coeEnv = CCoeEnv::Static(); 
    BaflUtils::NearestLanguageFile(coeEnv->FsSession(), resourceFileName); 
       
    if ( !BaflUtils::FileExists( coeEnv->FsSession(), resourceFileName ) )
        { 
        // Use resource file on the Z drive instead  
        parse.Set( KResourceDir, &KDC_RESOURCE_FILES_DIR, NULL ); 
        resourceFileName = parse.FullName();  
        BaflUtils::NearestLanguageFile( coeEnv->FsSession(),resourceFileName );            
        } 
    iResourceFileOffset = coeEnv->AddResourceFileL(resourceFileName);
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CWrtHarvester* CWrtHarvester::NewL(  MLiwInterface* aCPSInterface )
    {
    CWrtHarvester* self = new ( ELeave ) CWrtHarvester( aCPSInterface );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWrtHarvester::~CWrtHarvester()
    {
    if ( iResourceFileOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
        }
    iObservers.ResetAll();
    iWidgetInfo.ResetAll();
    iWidgetStateArray.ResetAll();
    delete iWidgetUIListener;
    delete iWidgetRegListener;
    delete iWidgetMMCListener;
    delete iWidgetUsbListener;
    iWidgetOperations.Close();
    iHSWidgets.ResetAndDestroy();
    iApaSession.Close();
    }
    
// ----------------------------------------------------------------------------
// Is called by Content Harvester server
// ----------------------------------------------------------------------------
//	
void CWrtHarvester::UpdateL() 
	{
	UpdatePublishersL();
	} 

// ----------------------------------------------------------------------------
// Is called when Homescreen sends events to publisher
// ----------------------------------------------------------------------------
//  
void CWrtHarvester::HandlePublisherNotificationL( const TDesC& aContentId, const TDesC8& aTrigger )
    {
    
    //Do not send the Operations to the Widgets when in Mass storage mode.. . .  
    if( IsInMSMode() == 1 )
        {
         if(aTrigger == KDeActive)
         RemovePublisherAndObserverL(aContentId);
         return;              
        }
   
    TUid uid( WidgetUid( aContentId ) );
    TWidgetOperations operation( Uninitialized );
    if( aTrigger == KActive )
        {
        HBufC* temp = aContentId.Alloc();
        iHSWidgets.Append( temp );
        iHSCount++;	
        // queue the activated state event only for network accessing widgets
        if ( CheckNetworkAccessL( uid) )
            {
            TWrtState* widgetState = NULL;
            widgetState = new TWrtState( uid, EActivatedState );
            if ( widgetState )
                {
                iWidgetStateArray.AppendL( widgetState );
                }
            }
        
        operation = LaunchMiniview;
        }
    else if ( aTrigger == KDeActive )
        {
        iHSCount--;
        operation = Deactivate;
        HBufC *temp = aContentId.Alloc();
        TPtr ptr (temp->Des());
        for( TInt i=0; i<iHSWidgets.Count(); i++ )
          {
          if( ! ptr.Compare(iHSWidgets[i]->Des()))
            {
            iHSWidgets.Remove(i);
            break;
            }
          }
        delete temp;        
        }
    else if( aTrigger == KSuspend )
        {
        operation = WidgetSuspend;
        }
    else if( aTrigger == KResume )
        {
        TInt idx = FindWidget( uid );
        
        // if unable to find the widget, queue the resume as usual
        if ( idx == -1 )
            {
            QueueResumeL( uid );
            }
        else
            {
            iWidgetStateArray[idx]->iState = EResumeState;
            }
        return;
        }
    else if( aTrigger == KSelected )
        {
        operation = WidgetSelect;
        }
    else if( aTrigger == KHSOnline )
        {
        ProcessNetworkModeL( uid, WidgetOnline );
        return;
        }
    else if( aTrigger == KHSOffline )
        {
        ProcessNetworkModeL( uid, WidgetOffline );
        return;
        }
    QueueOperationL( operation, uid );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::UpdatePublishersL() 
    {
    iRegistryAccess.WidgetInfosL( iWidgetInfo );
    RemoveObsoletePublishersL();
    
    for( TInt i = iWidgetInfo.Count() - 1; i >= 0; --i )
        {
        // Register the observer first as notification may be
        // received immediately after registration
        RequestForNotificationL( *iWidgetInfo[i] );
        TInt id = RegisterPublisherL( *iWidgetInfo[i] );
        if( id == KErrNotFound )
            {
            // remove the observer as the publisher registration failed
            RemoveObserver( *iWidgetInfo[i]->iBundleId);
            }
        }
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RemoveObsoletePublishersL() 
    {
    RWrtArray<HBufC> publishers;
    publishers.PushL();
    RegisteredPublishersL( publishers );
    
    // Remove observer and CPS registry entry of uninstalled widget
    TInt count = publishers.Count();
    for( TInt i = count - 1; i >= 0; --i )
        {
        TBool found( EFalse );
        for( TInt j = 0; j < iWidgetInfo.Count(); j++ )
            {
            if( *publishers[i] == iWidgetInfo[j]->iBundleId )
                {
                found = ETrue;
                break;
                }
            }
        if( found == EFalse )
            {
            RemovePublisherAndObserverL( *publishers[i] );
            }
        }
    CleanupStack::PopAndDestroy( &publishers );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RegisteredPublishersL( RPointerArray<HBufC>& publishers )
    {
    publishers.ResetAndDestroy();

    CLiwGenericParamList* publisherList( CLiwGenericParamList::NewLC() );
    CLiwGenericParamList* inParamList( CLiwGenericParamList::NewLC() );
    
    TLiwGenericParam type( KType, TLiwVariant( KPubData ));
    inParamList->AppendL( type );
    
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    
    filter->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ) );
    filter->InsertL( KContentType, TLiwVariant( KTemplatedWidget ) );
     
    //append filter to input param
    TLiwGenericParam item( KFilter, TLiwVariant( filter ));
    inParamList->AppendL( item );
       
    // Get all publishers from CPS
    iCPSInterface->ExecuteCmdL( KGetList, *inParamList, *publisherList ); 
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( inParamList );
    
    TInt pos = 0;
    publisherList->FindFirst( pos, KResults );
    if( pos != KErrNotFound )
        {
        //extract iterator on results list
        TLiwVariant variant = (*publisherList)[pos].Value();
        CLiwIterable* iterable = variant.AsIterable();
        iterable->Reset();
        
        //get all the templated widget publishers
        while( iterable->NextL( variant ) )
            {
            //extract content map
            CLiwDefaultMap* publisherMap = CLiwDefaultMap::NewLC();         
            variant.Get( *publisherMap );
            variant.Reset();
             
            if( publisherMap && publisherMap->FindL( KContentId , variant ))
                {
                 HBufC* bundleId = variant.AsDes().AllocLC();
                 publishers.AppendL( bundleId );
                 CleanupStack::Pop( bundleId );
                 }
             CleanupStack::PopAndDestroy( publisherMap );     
             }
            
        variant.Reset();
        }
    CleanupStack::PopAndDestroy( publisherList );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TInt CWrtHarvester::RegisterPublisherL( CWrtInfo& wrtInfo )
    {
    TBool networkAccess = CheckNetworkAccessL( wrtInfo.iUid );
    
    __UHEAP_MARK;
    TInt id( KErrNotFound );
    if( iCPSInterface )
        {   
        CLiwGenericParamList* inparam( CLiwGenericParamList::NewLC() );
        CLiwGenericParamList* outparam( CLiwGenericParamList::NewLC() );

        TLiwGenericParam type( KType, TLiwVariant( KPubData ));
        inparam->AppendL( type );
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* actionmap( NULL );

        // Create the data map for publisher registry
        cpdatamap->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ));
        cpdatamap->InsertL( KContentType, TLiwVariant( KTemplatedWidget ));
        cpdatamap->InsertL( KContentId, TLiwVariant( wrtInfo.iBundleId ));
        // Widget info map
    	CLiwDefaultMap* widgetInfo = CLiwDefaultMap::NewLC();
		widgetInfo->InsertL( KTemplateType, TLiwVariant( KTemplateName ));
		widgetInfo->InsertL( KWidgetName, TLiwVariant( wrtInfo.iDisplayName ));
		datamap->InsertL( KWidgetInfo , TLiwVariant( widgetInfo ));
		CleanupStack::PopAndDestroy( widgetInfo );
        
		// Take dynamic menu items into use
		if (networkAccess)
		    {
		    CLiwDefaultMap* mapMenu = CLiwDefaultMap::NewLC();
		    mapMenu->InsertL( KItemOnlineOffline, TLiwVariant( KMyActionName ));
		    datamap->InsertL( KMenuItems, TLiwVariant( mapMenu ));
		    CleanupStack::PopAndDestroy(mapMenu);
		    }

        cpdatamap->InsertL( KDataMap, TLiwVariant(datamap) );
        
        // Create the action map for publisher registry
        actionmap = CLiwDefaultMap::NewLC();
        actionmap->InsertL( KActive, TLiwVariant( KMyActionName ));
        actionmap->InsertL( KDeActive, TLiwVariant( KMyActionName ));
        actionmap->InsertL( KSuspend, TLiwVariant( KMyActionName ));
        actionmap->InsertL( KResume, TLiwVariant( KMyActionName ));
        actionmap->InsertL( KSelected, TLiwVariant( KMyActionName ));
        if (networkAccess)
            {
            actionmap->InsertL( KHSOnline, TLiwVariant( KMyActionName ));
            actionmap->InsertL( KHSOffline, TLiwVariant( KMyActionName ));
            }
 
        cpdatamap->InsertL( KMyActionMap, TLiwVariant(actionmap));
        CleanupStack::PopAndDestroy( actionmap );
        
        TLiwGenericParam item( KMyItem, TLiwVariant( cpdatamap ));        
        inparam->AppendL( item );
        
        iCPSInterface->ExecuteCmdL( KMyAdd , *inparam, *outparam);
        id = ExtractItemId(*outparam);
        
        CleanupStack::PopAndDestroy( datamap );
        CleanupStack::PopAndDestroy( cpdatamap );
        item.Reset();
        type.Reset();   
        CleanupStack::PopAndDestroy(outparam);
        CleanupStack::PopAndDestroy(inparam);
        }
    __UHEAP_MARKEND;
    return id;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RemoveObserver(const TDesC& aBundleId)
	{
	TInt count = iObservers.Count();
	for( TInt i = 0; i < count; i++ )
		{
		if( iObservers[i]->BundleId() == aBundleId )
			{
			delete iObservers[i];
			iObservers.Remove( i );
			break;
			}
		}
	}
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RemovePublisherAndObserverL( const TDesC& aBundleId )
    {
    
    if( IsInMSMode() == 1 )
      {
      HBufC *temp = aBundleId.Alloc();
      TPtr ptr (temp->Des());
      for( TInt i=0; i<iHSWidgets.Count(); i++ )
        {

        if( ptr.Compare(iHSWidgets[i]->Des()) == 0)
          {
           return;
          }
        }

      }   
              
    RemoveObserver( aBundleId );
    
    __UHEAP_MARK;
    if( iCPSInterface )
        {
        CLiwGenericParamList* inparam = CLiwGenericParamList::NewLC();
        CLiwGenericParamList* outparam = CLiwGenericParamList::NewLC();
        TLiwGenericParam type( KType, TLiwVariant( KPubData ));
        inparam->AppendL( type );
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();

        // Create data map
        cpdatamap->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ));
        cpdatamap->InsertL( KContentType, TLiwVariant( KTemplatedWidget ));
        cpdatamap->InsertL( KContentId, TLiwVariant( aBundleId ));
        
        cpdatamap->InsertL( KDataMap, TLiwVariant(datamap) );
        
        TLiwGenericParam item( KMyItem, TLiwVariant( cpdatamap ));        
        inparam->AppendL( item );
        iCPSInterface->ExecuteCmdL( KMyDelete , *inparam, *outparam);
        
        CleanupStack::PopAndDestroy( datamap );
        CleanupStack::PopAndDestroy( cpdatamap );
        item.Reset();
        type.Reset();
        CleanupStack::PopAndDestroy(outparam);
        CleanupStack::PopAndDestroy(inparam);
        }
    __UHEAP_MARKEND;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RequestForNotificationL( CWrtInfo& wrtInfo )
    {
    // Preventing duplicate entries.
    for( TInt i = 0; i < iObservers.Count(); i++ )
        {
        if( iObservers[i]->BundleId() == wrtInfo.iBundleId )
            {
            return;
            }
        }
    
    CWrtHarvesterPublisherObserver* observer( NULL );
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    
    filter->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ) );
    filter->InsertL( KContentType, TLiwVariant( KTemplatedWidget ) );
    filter->InsertL( KContentId, TLiwVariant( wrtInfo.iBundleId ) );
    filter->InsertL( KOperation, TLiwVariant( KExecute ));
    
    observer = CWrtHarvesterPublisherObserver::NewLC( *wrtInfo.iBundleId, this );
    observer->RegisterL(filter);
    
    iObservers.AppendL( observer );
    
    CleanupStack::Pop( observer );
    
    CleanupStack::PopAndDestroy(filter);
    }
    
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::QueueOperationL( TWidgetOperations aOperation, TUid aUid )
    {
    //Hack to find out if WidgetUi exist as Queue keeps filling up
    if((iHSCount*3 <= iWidgetOperations.Count() && !CheckTaskExistsL()) || (aOperation == WidgetSelect))
        {
        ClearAllOperations();
        iWidgetUIListener->SetValue(1);
        }        
    SWidgetOperation op = { aOperation, aUid };
    iWidgetOperations.Append( op );
    TryLaunchNextOperationL();
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TBool CWrtHarvester::CheckTaskExistsL()
    {
    TUid widgetAppUid( TUid::Uid( KWidgetAppUid ) );
    RWsSession wsSession;    
        
    // Create Window server session
    User::LeaveIfError( wsSession.Connect() );
    
    TApaTaskList taskList( wsSession );
    TApaTask task = taskList.FindApp( widgetAppUid );
    return task.Exists()?ETrue:EFalse;
    }
    

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::TryLaunchNextOperationL()
    {
    TInt value = KErrNone;
    TInt error = iWidgetUIListener->GetValue(value);
    if( error == KErrNone && value == 1 && iWidgetOperations.Count() != 0 )
        {
        // Set value to 0 so that next widget is not launched before Widget App sets value to 1.
        iWidgetUIListener->SetValue( 0 );
        //Always launch the first in operation
        LaunchWidgetOperationL( iWidgetOperations[0] );
        iWidgetOperations.Remove( 0 );
        }
    }

void CWrtHarvester::ClearAllOperations()
    {
    TInt value = KErrNone;
    TInt error = iWidgetUIListener->GetValue(value);
    if( error == KErrNone )
        {
        iWidgetOperations.Reset();
        }
    }
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TUid CWrtHarvester::WidgetUid( TPtrC aBundleId )
    {
    TInt count( iWidgetInfo.Count());
    TUid uid = {0};
    for( TInt i = 0; i < count; i++ )
        {
        if( iWidgetInfo[i]->iBundleId == aBundleId )
            {
            uid = iWidgetInfo[i]->iUid;
            break;
            }
        }
    return uid;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TInt CWrtHarvester::ExtractItemId( const CLiwGenericParamList& aInParamList )
    {
    TInt result ( KErrNotFound );
    TInt pos( 0 );
    aInParamList.FindFirst( pos, KMyItemId );
    if( pos != KErrNotFound )
        {
        // item id present - extract and return
        TUint uResult = 0;
        if ( aInParamList[pos].Value().Get( uResult ) )
            {
            result = uResult;
            }
        }
    return result;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::LaunchWidgetOperationL( SWidgetOperation aOperation )
    {
    HBufC* widgetName( GetAppNameLC( iApaSession, aOperation.iUid) );
    HandleWidgetCommandL( iApaSession, *widgetName, aOperation.iUid, aOperation.iOperation );
    CleanupStack::PopAndDestroy( widgetName );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::QueueResumeL( TUid& aUid )
    {
    RWidgetRegistryClientSession session;
    CleanupClosePushL( session );
    User::LeaveIfError( session.Connect() );
    if ( session.IsBlanketPermGranted ( aUid ) == EBlanketUnknown && !iDialogShown )
        {
        iDialogShown = ETrue;            
        AllowPlatformAccessL( aUid );
        }
    else if(!iDialogShown)
        {
        QueueOperationL( WidgetResume, aUid );
        }        
    CleanupStack::PopAndDestroy( &session );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::ProcessNetworkModeL( TUid& aUid, TWidgetOperations aOperation )
    {
    // first queue the online/offline event and then the resume event
    QueueOperationL( aOperation, aUid );
    
    // check if there is a resume event to queue
    TInt idx = FindWidget(aUid, EResumeState);
    if ( idx != -1 )
        {
        QueueResumeL( aUid );          
        }
    idx = (idx == -1) ? FindWidget(aUid): idx;
    if(idx != -1 )
        {
        // remove it from the array, no longer needed
        delete iWidgetStateArray[idx];
        iWidgetStateArray.Remove(idx);
        }
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TInt CWrtHarvester::FindWidget( TUid& aUid, TWidgetState aState )
    {
    TInt idx = -1;
    for( TInt i( iWidgetStateArray.Count() - 1 ); i >= 0; --i )
        {
        if ( ( iWidgetStateArray[i]->iUid == aUid ) && ( iWidgetStateArray[i]->iState == aState ) )
            {
            idx = i;
            }
        }
    return idx;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TInt CWrtHarvester::FindWidget( TUid& aUid )
    {
    TInt idx = -1;
    for( TInt i( iWidgetStateArray.Count() - 1 ); i >= 0; --i )
        {
        if ( iWidgetStateArray[i]->iUid == aUid )
            {
            idx = i;
            }
        }
    return idx;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TBool CWrtHarvester::CheckNetworkAccessL( TUid& aUid )
    {
    RWidgetRegistryClientSession session;
    CleanupClosePushL( session );
    User::LeaveIfError( session.Connect() );
    TBool networkAccess = *(session.GetWidgetPropertyValueL( aUid, EAllowNetworkAccess ) );
    CleanupStack::PopAndDestroy( &session );
    
    return networkAccess;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::AllowPlatformAccessL( TUid& aUid )
    {
    HBufC* confirmationText = StringLoader::LoadLC(R_WRTHRV_PLATFORM_ACCESS);
    TInt softKey = R_AVKON_SOFTKEYS_OK_CANCEL;
    CGlobalQueryHandlerAO* tmp = CGlobalQueryHandlerAO::StartLD( aUid, confirmationText->Des(), softKey, this );
    CleanupStack::PopAndDestroy(confirmationText);
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::StartLD
//
// Initialize AO.
// ------------------------------------------------------------------------
CGlobalQueryHandlerAO* CGlobalQueryHandlerAO::StartLD(
    TUid& aUid,
    const TDesC& aMessage, 
    TInt aSoftkeys,
    CWrtHarvester* aWrtHarvester)
    {
    CGlobalQueryHandlerAO* self( new( ELeave ) CGlobalQueryHandlerAO( aUid, aMessage, aSoftkeys, aWrtHarvester) );
    TRAPD(error, self->ShowGlobalQueryDialogL ( aMessage, aSoftkeys ));
    if ( error )
        {
        delete self;
        User::Leave(error);
        }
    self->SetActive();
    return self;
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::CGlobalQueryHandlerAO
//
// Constructor.
// ------------------------------------------------------------------------
CGlobalQueryHandlerAO::CGlobalQueryHandlerAO(
    TUid& aUid,
    const TDesC& aMessage, 
    TInt /*aSoftkeys*/,
    CWrtHarvester* aWrtHarvester):CActive( EPriorityHigh ),
    iWidgetUid(aUid),
    iConfirmationText(aMessage.AllocL()),
    iWrtHarvester(aWrtHarvester)
    {
    CActiveScheduler::Add( this );
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::CGlobalQueryHandlerAO
//
// ISet network and platform access permission based on user response.
// ------------------------------------------------------------------------
void CGlobalQueryHandlerAO::RunL()
    {    
    RWidgetRegistryClientSession session;
    CleanupClosePushL( session );
    User::LeaveIfError( session.Connect() );  
    if (iStatus == EAknSoftkeyOk)
        {
        session.SetBlanketPermissionL( iWidgetUid, EBlanketTrue );
        }
    else if ( iStatus == EAknSoftkeyCancel)
        {
        session.SetBlanketPermissionL( iWidgetUid, EBlanketFalse );
        }
    iWrtHarvester->QueueOperationL( WidgetResume, iWidgetUid );
    iWrtHarvester->DialogShown();
    CleanupStack::PopAndDestroy( &session );
    
    delete this;
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::DoCancel
//
// Do nothing.
// ------------------------------------------------------------------------
void CGlobalQueryHandlerAO::DoCancel()
    {
    if ( iGlobalConfirmationQuery )
        {
        iGlobalConfirmationQuery->CancelConfirmationQuery();
        }
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::~CGlobalQueryHandlerAO
//
// Destructor.
// ------------------------------------------------------------------------
CGlobalQueryHandlerAO::~CGlobalQueryHandlerAO()
    {
    Cancel();
    delete iGlobalConfirmationQuery;
    delete iConfirmationText;
    }

// ---------------------------------------------------------
// CGlobalQueryHandlerAO::ShowGlobalQueryDialogL()
// ---------------------------------------------------------
// 
void CGlobalQueryHandlerAO::ShowGlobalQueryDialogL(const TDesC& aMessage, TInt aSoftkeys)
    {
    iGlobalConfirmationQuery = CAknGlobalConfirmationQuery::NewL();
    iGlobalConfirmationQuery->ShowConfirmationQueryL
                                (iStatus,
                                aMessage,
                                aSoftkeys);
    }

 //  End of File
