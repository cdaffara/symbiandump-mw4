/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Environment
*
*/



#include <coemain.h>
#include <AknsConstants.h>
#include <AknTaskList.h>
#include <alf/ftokenclient.h>
#include <aknappui.h>
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfdisplay.h"
#include "alf/alfroster.h"
#include "alf/alfcontrolgroup.h"
#include "alf/alfevent.h"
#include "alf/alftexturemanager.h"
#include "alf/alftextstylemanager.h"
#include "alfcommandscheduler.h"
#include "alf/alfstatic.h"
#include "alf/alflayoutmetrics.h"
#include "alf/alfconstants.h"
#include "alf/alfevent.h"
#include "alf/alfcontrol.h"
#include "alf/alfbatchbuffer.h"
#include "alf/alfenvobject.h"
#include "alfuids.h"
#include "alfpanic.h"

#include "uiacceltk/HuiUtil.h"
#include "alflogger.h"
// Literals and constants
// This string is used with multiple ALF clients -panic. The panic conserns 
// only ALF clients, NOT the server!
_LIT( KUIAcceltkClientPanic, "UIAcceltkClient" );

struct TSharedTextureManagerEntry 
    {
    public:
    	~TSharedTextureManagerEntry()
        {
        }
    	TSharedTextureManagerEntry()                      
                      : iRefCount(0), iTextureManager(NULL)
        {                            
        }
    	
    	TSharedTextureManagerEntry(CAlfTextureManager* aTextureManager)                      
                      : iRefCount(1), iTextureManager(aTextureManager)
        {
        }

    /** The texturemanager reference count */
    TInt iRefCount;

    /** The shared texture manager entry. */
    CAlfTextureManager* iTextureManager;
    };


// Used flags.
enum TAlfEnvFlags
    {
    EOwnClient =            0x01,
    ETokenClientConnected = 0x02 
    // ...add other flags...
    };


// Deault max framerate
const TInt KAlfDefaultMaxFrameRate = 33;
    
// Default flags
const TUint KAlfEnvDefaultFlags = 0x00;

// Size of buffer towards client side.
const TInt KAlfTextureInfoMonitorBufferSize = 128;

// ---------------------------------------------------------------------------
// Returns connected control with ID 
// ---------------------------------------------------------------------------
//
CAlfControl* FindConnectedControl(CAlfControl& aControl, TInt aId, TBool aUserId)
    {
    CAlfControl* result = NULL;
    for ( TInt connectionIndex = 0 ; connectionIndex < aControl.ConnectionCount() && !result ; connectionIndex++ )
        {
        CAlfControl& connection = aControl.Connection( connectionIndex );
        if( (aUserId?
            connection.Id():
            connection.Identifier()) 
                == aId)
            {
            result = &connection;
            }
        else
            {
            result = FindConnectedControl( connection, aId, aUserId );
            }
        }
    
    return result;
    }

/**
 * Gets the pointer evetns from the server
 */
NONSHARABLE_CLASS(CAlfPtrEventFetcher):public CActive
    {
    public:
    CAlfPtrEventFetcher(RAlfClient& aClient, CAlfEnv& aEnv)
        :CActive(CActive::EPriorityHigh),iClient(aClient),iEventAsDescriptor(iEvent),iEnv(aEnv)
        {
        CActiveScheduler::Add(this);
        }

    ~CAlfPtrEventFetcher()
        {
        Cancel();
        }

    void Start()
        {
        ASSERT(!IsActive());
        SetActive();
        iClient.RequestPointerEvents(iEventAsDescriptor, iStatus);
        }

    private:
    void DeliverPointerEventL();
    
    void RunL()
        {
        if(iStatus.Int() == KErrNone)    
        	{
        	DeliverPointerEventL();		
        	Start();
        	}
        else
        	{
        	__ALFLOGSTRING1( "~CAlfPtrEventFetcher::RunL error %d", iStatus.Int() )	
        	}
        
        }
        
    void DoCancel()
        {
        iClient.CancelPointerEvents();
        }

    TInt RunError(TInt /*aError*/)
        {
        //Nothing to worry, just restart
        Start();
        return KErrNone;
        }

    RAlfClient& iClient;
    TAlfTouchEvent iEvent;
    TPckg<TAlfTouchEvent> iEventAsDescriptor;
    CAlfEnv& iEnv;
    friend class CAlfEnv; // allow env to acces our event data
    };

void CAlfPtrEventFetcher::DeliverPointerEventL()
    {
    TBool eventSent(EFalse);
    
    // search clients based on handles from server
    CAlfControl* ctrl = iEnv.FindControl(iEvent.iControls[0], EFalse);
    TInt visualIdentifier = iEvent.iVisuals[0];
    
    for(TInt ii = 1; ctrl ; ii++)
        {
        // Find the visual index
        CAlfVisual* visual = NULL;
        for ( TInt i = ctrl->VisualCount() - 1 ; i >= 0 && visualIdentifier ; i-- )
            {
            if ( ctrl->Visual( i ).Identifier() == visualIdentifier )
                {
                visual = &ctrl->Visual( i );
                break;
                }
            }
        
        // Todo: Do we allow ctrl to leave and still pass the event to other cntrls ??
        TBool consumed = EFalse;
        TAlfEvent event(*ctrl->Display(), iEvent.iEvent); 
        event.SetVisual( visual );
        eventSent = ETrue;
        /*TRAP_IGNORE(*/consumed = ctrl->OfferEventL(event)/*)*/;
        if (consumed)
            {
            break;
            }
            
        ctrl = iEnv.FindControl(iEvent.iControls[ii], EFalse);
        visualIdentifier = iEvent.iVisuals[ii];
        }
    
    if (!eventSent)
        {
        // ptr event hit visual having groupless control as an owner
        // we can't resolve destination in toolkit side, just post infromation about the event data to 
        // action observers and let the app implementation decide whether event is used or nor.
        iEnv.ReportAction(TAlfActionCommand(KAlfOrpheanPtrEventReceived));
        }
    }


NONSHARABLE_CLASS(CAlfSystemEventFetcher):public CActive
    {
    public:
    CAlfSystemEventFetcher(RAlfClient& aClient, CAlfEnv& aEnv)
        :CActive(CActive::EPriorityHigh),iClient(aClient),iEventAsDescriptor(iEvent),iEnv(aEnv)
        {
        CActiveScheduler::Add(this);
        }

    ~CAlfSystemEventFetcher()
        {
        Cancel();
        }

    void Start()
        {
        ASSERT(!IsActive());
        SetActive();
        iClient.RequestSystemEvents(iEventAsDescriptor, iStatus);
        }

    private:
    void DeliverSystemEventL();
    
    void RunL()
        {
        if(iStatus.Int() != KErrNone)
            { // for now..
            USER_INVARIANT();
            }
            
        DeliverSystemEventL();
        Start();
        }
        
    void DoCancel()
        {
        iClient.CancelSystemEvents();
        }

    TInt RunError(TInt /*aError*/)
        {
        //Nothing to worry, just restart
        Start();
        return KErrNone;
        }

    RAlfClient& iClient;
    TInt iEvent;
    TPckg<TInt> iEventAsDescriptor;
    CAlfEnv& iEnv;
    };

void CAlfSystemEventFetcher::DeliverSystemEventL()
    {
    if (iEvent == KAknsMessageSkinChange)
        {
        iEnv.SetSkinChangePending(ETrue);
        iEnv.ReportWsEventAsActionCommand(KAlfActionIdSkinChanged); 
        // Some action observer may have already called by themselves
        // NotifySkinChangedL, so check before we do it.
        if (iEnv.SkinChangePending())
            {
            iEnv.NotifySkinChangedL();            
            }        
        }
    }
       
/**
 * Texture information monitor.
 */
NONSHARABLE_CLASS( CAlfTextureInfoMonitor ) : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aClient reference to client.
     */
    static CAlfTextureInfoMonitor* NewL( CAlfEnv& aEnv );
    
    /**
     * Destructor.
     */
    ~CAlfTextureInfoMonitor();
       
private:

    /**
     * Constructor.
     * @param aClient reference to client.
     */
    CAlfTextureInfoMonitor( CAlfEnv& aEnv );
    
    /**
     * Symbian OS constructor.
     */
    void ConstructL();

    /**
     * Issues a request.
     */
    void IssueRequest();
    
    /**
     * Called by active object framework when request is completed.
     */
    virtual void RunL();
    
    /**
     * Cancels pending request.
     */
    virtual void DoCancel();
    
private:
    
    /**
     * Reference to environment.
     */
    CAlfEnv& iEnv;    
    
    /**
     * Buffer containing texture info events.
     */
    TBuf8< KAlfTextureInfoMonitorBufferSize > iTextureEventBuffer;
    };

CAlfTextureInfoMonitor* CAlfTextureInfoMonitor::NewL( CAlfEnv& aEnv )
    {
    CAlfTextureInfoMonitor* self = 
        new (ELeave) CAlfTextureInfoMonitor( aEnv );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;    
    }
    
CAlfTextureInfoMonitor::~CAlfTextureInfoMonitor()
    {
    Cancel();
    }
    
CAlfTextureInfoMonitor::CAlfTextureInfoMonitor( CAlfEnv& aEnv )
    : CActive( CActive::EPriorityStandard ),
      iEnv( aEnv )
    {
    CActiveScheduler::Add( this );
    }
    
void CAlfTextureInfoMonitor::ConstructL()
    {
    IssueRequest();
    }

void CAlfTextureInfoMonitor::IssueRequest()
    {
    iEnv.Client().TextureNotifyInfo( iStatus, iTextureEventBuffer );
    SetActive();
    }
    
void CAlfTextureInfoMonitor::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        iEnv.HandleTextureInfo( iTextureEventBuffer );
        IssueRequest();
        }
    }
    
void CAlfTextureInfoMonitor::DoCancel()
    {
    iEnv.Client().TextureCancelNotifyInfo();
    }

// Private data
struct CAlfEnv::TPrivateData
    {
    // Structure used to hold owned objects in the extension array
    struct TObjectHolder
        {
        TInt iUid;
        MAlfEnvObject* iObject;
        };
    TPrivateData():iFlags(0),iClient(0),iSharedRoster(0),
                   iTextureManager(0),iTextStyleManager(0),iScheduler(0),
                   iMaxFrameRate(0),iRefreshMode(EAlfRefreshModeAutomatic),
                   iStatic(0),iLayoutMetricsUtility(0),iPointerEventAo(0),
                   iBatchBufferHandler(0){}
    TUint iFlags;                                   // Owned.
    RAlfClient* iClient;                            // Owned/not owned - see EOwnClient
    CAlfRoster* iSharedRoster;                      // Owned.
    RPointerArray<CAlfDisplay> iDisplays;           // Owned.
    RPointerArray<CAlfControlGroup> iLoadedGroups;  // Owned.
    CAlfTextureManager* iTextureManager;            // Owned.
    CAlfTextStyleManager* iTextStyleManager;        // Owned.
    RPointerArray<MAlfActionObserver> iActionObservers;   // Owned.
    RArray<TSharedTextureManagerEntry> iSharedTextureManagers; // Owned.
    CAlfCommandScheduler* iScheduler;               // Owned.
    TInt iMaxFrameRate;
    TAlfRefreshMode iRefreshMode;
    RFTokenClient   iTokenClient;                    // lazy bound, connected once first real request occurs
    CAlfStatic* iStatic;                              // Owned.
    CAlfLayoutMetricsUtility* iLayoutMetricsUtility;  // Owned.
    CAlfPtrEventFetcher* iPointerEventAo;             // Owned.
    CAlfSystemEventFetcher* iSystemEventAo;             // Owned.
    TVersion iApiVersion;
    CAlfBatchBuffer* iBatchBufferHandler;
        
    // Flags to avoid unnecessary skin & layout notify calls
    TBool iNotifySkinChangePending;  
    TBool iNotifyLayoutChangePending;  
    
    CAlfTextureInfoMonitor* iTextureInfoMonitor; // Owned.
    
    RArray<CAlfEnv::TPrivateData::TObjectHolder> iExtensionArray;
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfEnv::CAlfEnv()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfEnv::ConstructL(TVersion aVersion)
    {
    /*  
    // AknEventMonitor can be used if we want to know the target control also
    // Otherwise CoeMonitor should be used for better compatibility between 
    // releases 
      
    CAknAppUi* appUi = iAvkonAppUi; // actually macro using coe static...
    if (appUi && appUi->EventMonitor())
        {
        appUi->EventMonitor()->AddObserverL(this);
        appUi->EventMonitor()->Enable();
        }
*/

    // Enforces single CAlfEnv policy. Panics ONLY the client, NOT the server!!
    __ASSERT_ALWAYS( !CAlfEnv::Static(), User::Panic( KUIAcceltkClientPanic, EAlfEnvPanicMultipleAlfEnvironments ) );
    
    CCoeEnv* coeStatic = CCoeEnv::Static();
    if ( coeStatic )
        {
        coeStatic->AddMessageMonitorObserverL(*this);
        }

    // Create private data
    iData = new (ELeave) TPrivateData;

    iData->iApiVersion = aVersion;
    
    // NULL member data before anything else may leave.
    iData->iClient = NULL;
    iData->iSharedRoster = NULL;
    iData->iTextureManager = NULL;
    iData->iTextStyleManager = NULL;
    iData->iScheduler = NULL;
    iData->iStatic = NULL;
    iData->iLayoutMetricsUtility = NULL;
    iData->iPointerEventAo = NULL;
    iData->iSystemEventAo = NULL;
    iData->iBatchBufferHandler = NULL;
    iData->iTextureInfoMonitor = NULL;
                
    // Set flags
    iData->iFlags = KAlfEnvDefaultFlags;
    
    // Set client ( pekjokel: client can be passed from outside as well? )
    iData->iClient = new (ELeave) RAlfClient;
    iData->iFlags |= EOwnClient;
    iData->iClient->OpenL();
    
    if (coeStatic)
        {
        CAknTaskList* taskList = CAknTaskList::NewLC(coeStatic->WsSession());
        const RArray<RWsSession::TWindowGroupChainInfo>& chain = taskList->WgArray();
        const TInt chainCount = chain.Count();

        TInt parentId = 0;

        TInt wgId = coeStatic->RootWin().Identifier();
        for ( TInt ii = 0; ii < chainCount; ii++ )
            {
            const RWsSession::TWindowGroupChainInfo& info = chain[ ii ];
            if ( info.iId == wgId )
                {
                parentId = info.iParentId;
                break;
                }
            }
        
        if (parentId > 0)
            {
            iData->iClient->SetWgParent(parentId);
            }
  
        CleanupStack::PopAndDestroy();       
        }
        
     // Create a shared roster. This is used by all overlaid displays.
    iData->iSharedRoster = new (ELeave) CAlfRoster;
    iData->iSharedRoster->ConstructL(NULL);
    
    iData->iDisplays.Reset();
    iData->iLoadedGroups.Reset();
    
    iData->iMaxFrameRate = KAlfDefaultMaxFrameRate;
        
    // Create a texture manager.
    User::LeaveIfError( iData->iClient->TextureUpdateOwnerId() );
    iData->iTextureManager = CAlfTextureManager::NewL(*this, TUid::Uid(0));
    iData->iTextureManager->AddLoadObserverL(this);
    iData->iTextureManager->AddStateObserverL(this);
    iData->iSharedTextureManagers.Reset();
    
    // Create a text style manager.
    iData->iTextStyleManager = CAlfTextStyleManager::NewL(*this);
    
    // Command scheduler
    iData->iScheduler = CAlfCommandScheduler::NewL( *this ); 
    
    // Static data storage
    iData->iStatic = CAlfStatic::NewL(this);
        
    // Layout metrics utility
    iData->iLayoutMetricsUtility = CAlfLayoutMetricsUtility::NewL(*this);
    
    // Create one shared pointer event fetcher for all displays and start it
    // to listen to incoming pointer events.
    iData->iPointerEventAo = new (ELeave) CAlfPtrEventFetcher(Client(), *this); 
    iData->iPointerEventAo->Start();

    iData->iSystemEventAo = new (ELeave) CAlfSystemEventFetcher(Client(), *this); 
    iData->iSystemEventAo->Start();
    
    iData->iBatchBufferHandler = CAlfBatchBuffer::NewL( *this );

    iData->iNotifySkinChangePending = EFalse;
    iData->iNotifyLayoutChangePending = EFalse;

    iData->iTextureInfoMonitor = 
        CAlfTextureInfoMonitor::NewL( *this );
    
    }

// ---------------------------------------------------------------------------
// 2-phased contructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfEnv* CAlfEnv::NewL(TVersion aVersion)
    {
    CAlfEnv* self = CAlfEnv::NewLC(aVersion);
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 2-phased contructor. Object stays on the stack.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfEnv* CAlfEnv::NewLC(TVersion aVersion)
    {
    CAlfEnv* self = new( ELeave ) CAlfEnv;
    CleanupStack::PushL( self );    
    
    // Check client API compatibility. 
    if (aVersion.iMajor != self->Version().iMajor)
        {
        User::Leave(KErrNotSupported);    
        }
    
    self->ConstructL(aVersion);
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfEnv::~CAlfEnv()
    {
    // Delete struct items
    if ( iData )
        {
        delete iData->iBatchBufferHandler;
        iData->iBatchBufferHandler = NULL;
        
        // Destroy pointer event fetcher
        delete iData->iPointerEventAo;
        iData->iPointerEventAo = NULL;

        delete iData->iSystemEventAo;
        iData->iSystemEventAo = NULL;
                
        iData->iLoadedGroups.ResetAndDestroy();
        iData->iDisplays.ResetAndDestroy();
        delete iData->iSharedRoster;
        iData->iSharedRoster = NULL;        
        
        delete iData->iScheduler;
        iData->iScheduler= NULL;
        
        // Delete extension objects in reverse order.
        // (Objects should be added with dependent objects after the object on which they depend)
        for (TInt extensionIndex = iData->iExtensionArray.Count()-1 ; 
            extensionIndex >= 0; 
            extensionIndex--)
            {
            MAlfEnvObject* obj =  iData->iExtensionArray[extensionIndex].iObject;
            if ( obj)
                {
                obj->Release();   
                }
            }
        iData->iExtensionArray.Reset();
        iData->iExtensionArray.Close();
                
        if (iData->iTextureManager)
            {
            iData->iTextureManager->RemoveLoadObserver(this);
            iData->iTextureManager->RemoveStateObserver(this);
            }

        delete iData->iTextureManager;
        iData->iTextureManager = NULL;
    
        delete iData->iTextStyleManager;
        iData->iTextStyleManager = NULL;
        
        // Remove shared texture managers if any exists
   	    RArray<TSharedTextureManagerEntry>& managers = (iData->iSharedTextureManagers);
       	TInt count = iData->iSharedTextureManagers.Count();
        for(TInt i = 0; i < count; i++)
          	{
          	TSharedTextureManagerEntry entry = managers[i];
            entry.iTextureManager->RemoveLoadObserver(this);
            entry.iTextureManager->RemoveStateObserver(this);          	
          	delete entry.iTextureManager;
      		managers.Remove(i);
        	}  								                       

        iData->iSharedTextureManagers.Close();	    

        delete iData->iTextureInfoMonitor;
        iData->iTextureInfoMonitor = NULL;

        // ... close, release, delete other member data...
        iData->iActionObservers.Close();
        
        // Disconnect and delete client if owned
        if ( (iData->iFlags&EOwnClient) && iData->iClient )
            {
            iData->iClient->Disconnect();
            delete iData->iClient;
            }
        iData->iClient = NULL;
        iData->iFlags &= ~EOwnClient;
        
        delete iData->iLayoutMetricsUtility;
        
      

        delete iData->iStatic;
        }
    
    // Delete struct
    delete iData;
    iData = NULL;

/*  
    // AknEventMonitor can be used if we want to know the target control also
    // Otherwise CoeMonitor should be used for better compatibility between 
    // releases 
  
    CAknAppUi* appUi = iAvkonAppUi; // actually macro using coe static...
    if (appUi && appUi->EventMonitor())
        {
        appUi->EventMonitor()->RemoveObserver(this);
        appUi->EventMonitor()->Enable(EFalse);
        }
*/

    CCoeEnv* coeStatic = CCoeEnv::Static();
    if ( coeStatic )
        {
        coeStatic->RemoveMessageMonitorObserver(*this);
        }
    }


// ---------------------------------------------------------------------------
// Returns the client
// ---------------------------------------------------------------------------
//
EXPORT_C RAlfClient& CAlfEnv::Client()
    {
    __ASSERT_ALWAYS( iData, USER_INVARIANT() );
    __ASSERT_ALWAYS( iData->iClient, USER_INVARIANT() );
    __ASSERT_ALWAYS( iData->iClient->Handle(), USER_INVARIANT() );
    
    iData->iClient->SetAlfEnv(*this);
    return *iData->iClient;
    }

// ---------------------------------------------------------------------------
// Creates new display
// ---------------------------------------------------------------------------
//    
EXPORT_C CAlfDisplay& CAlfEnv::NewDisplayL(
    const TRect& aRect,
    TInt aFlags,
    TInt aDisplayType)
    {
    return NewDisplayL(aRect, aFlags, NULL, aDisplayType, KAlfUidBackBufferScreen0);    
    }  
    
// ---------------------------------------------------------------------------
// Creates new display
// ---------------------------------------------------------------------------
//    
EXPORT_C CAlfDisplay& CAlfEnv::NewDisplayL( 
	const TRect& aRect, 
	TInt aFlags, 
	CAlfDisplay* /*aRosterOwningDisplay*/, 
    TInt aDisplayType, 
    TUid aBackBufferUid )
    {
    CAlfRoster* roster = NULL;
    
    if(aFlags & ENewDisplayOverlaid)
        {
        roster = iData->iSharedRoster;
        }
    
    TBool asCoeControl = EFalse;
    if ( aFlags & ENewDisplayAsCoeControl )
        {
        asCoeControl = ETrue;
        }
           
    CAlfDisplay* display = new (ELeave) CAlfDisplay;
    CleanupStack::PushL(display);
    display->ConstructL(*this, asCoeControl,aRect, roster, aDisplayType, aBackBufferUid);
    CleanupStack::Pop(display);
    TInt err = iData->iDisplays.Append(display);
    if ( err != KErrNone )
        {
        delete display;
        User::Leave( err );
        }                
    return *display;
    }  
// ---------------------------------------------------------------------------
// Called when display is deleted.
// ---------------------------------------------------------------------------
// 
void CAlfEnv::RemoveDisplay(CAlfDisplay& aDisplay)
    {
    // Actually this is "DestroyDisplay"

    /** @todo  Make a proper observer. */
    TInt index = iData->iDisplays.Find(&aDisplay);
    if(index >= 0)
        {
        iData->iDisplays.Remove(index);
        iData->iDisplays.Compress();
        }
    // @todo: remove from the server side
    }

// ---------------------------------------------------------------------------
// Creates new control group
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfControlGroup& CAlfEnv::NewControlGroupL( TInt aId )
    {
    if ( FindControlGroup( aId ) )
        {
        User::Leave( KErrAlreadyExists );
        }
    
    CAlfControlGroup* group = new (ELeave) CAlfControlGroup;
    CleanupStack::PushL( group );
    group->ConstructL( aId, *this );
    CleanupStack::Pop( group );
    TInt err = iData->iLoadedGroups.Append( group );
    if ( err != KErrNone )
        {
        delete group;
        User::Leave( err );
        }
    
    return *group;
    }
 
// ---------------------------------------------------------------------------
// Deletes control group
// ---------------------------------------------------------------------------
//   
EXPORT_C TInt CAlfEnv::DeleteControlGroup(TInt aId)
    {
    TInt i;

    for(i = 0; i < iData->iLoadedGroups.Count(); ++i)
        {
        if(iData->iLoadedGroups[i]->ResourceId() == aId)
            {
            CAlfControlGroup* group = iData->iLoadedGroups[i];

            CancelCommands(group);

            for (TInt ii = iData->iDisplays.Count()-1; ii>=0; ii--)
                {
                CAlfRoster& r = iData->iDisplays[ii]->Roster();
                TInt index = r.Find(*group);
                if (index != KErrNotFound)
                    {
                    r.Hide(*group); // update server
                    r.Remove(group); // update client 
                    }
                }

            // This is control group to delete.
            iData->iLoadedGroups.Remove(i);
            delete group;
            return KErrNone;
            }
        }

    return KErrNotFound;
    }
    
// ---------------------------------------------------------------------------
// Returns control group with ID
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfControlGroup& CAlfEnv::ControlGroup(TInt aResourceId)
    {
    for(TInt i = 0; i < iData->iLoadedGroups.Count(); ++i)
        {
        if(iData->iLoadedGroups[i]->ResourceId() == aResourceId)
            {
            return *iData->iLoadedGroups[i];
            }
        }

    // Group hasn't been loaded yet
    /** @todo load automatically? */
    __ASSERT_DEBUG( EFalse, USER_INVARIANT() );
    return *iData->iLoadedGroups[0];
    }
   
// ---------------------------------------------------------------------------
// Returns control group with ID or NULL if not found
// ---------------------------------------------------------------------------
// 
EXPORT_C CAlfControlGroup* CAlfEnv::FindControlGroup(TInt aResourceId) const
    {
    for(TInt i = 0; i < iData->iLoadedGroups.Count(); ++i)
        {
        if(iData->iLoadedGroups[i]->ResourceId() == aResourceId)
            {
            return iData->iLoadedGroups[i];
            }
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// Returns control with ID
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfControl* CAlfEnv::FindControl(TInt aId, TBool aUserId) const
    {
    for(TInt i = 0; i < iData->iLoadedGroups.Count(); ++i)
        {
        CAlfControl* control = iData->iLoadedGroups[i]->FindControl(aId, aUserId);
        if(control)
            {
            return control;
            }
        }
        
    // If we cannot found it from any of the groups, check the connections
    for(TInt groupIndex = 0; groupIndex < iData->iLoadedGroups.Count(); ++groupIndex)
        {
        CAlfControlGroup* controlGroup = iData->iLoadedGroups[groupIndex];
        
        for ( TInt controlIndex = 0 ; controlIndex < controlGroup->Count() ; controlIndex++ )
            {
            CAlfControl* foundControl = FindConnectedControl(controlGroup->Control(controlIndex), aId, aUserId);
            if ( foundControl )
                {
                return foundControl;
                }
            }
        } 
        
    // The control does not exist.
    return NULL;
    }

// ---------------------------------------------------------------------------
// Returns display count
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfEnv::DisplayCount() const
    {
    return iData->iDisplays.Count();
    }
 
// ---------------------------------------------------------------------------
// Set refresh mode.
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfEnv::SetRefreshMode( TAlfRefreshMode aMode )
    {
    if(aMode == EAlfRefreshModeAutomatic)
        {
        // syncronoushly rasterize all the changed text visuals
        iData->iTextStyleManager->RefreshAllVisuals();
        }
    Client().EnvSetRefreshMode( aMode );
    iData->iRefreshMode = TAlfRefreshMode(aMode);
    }
    
// ---------------------------------------------------------------------------
// Set max frame rate.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfEnv::SetMaxFrameRate( TReal32 aFrameRate ) __SOFTFP
    {
    Client().EnvSetMaxFrameRate( aFrameRate );
    iData->iMaxFrameRate = (TInt)aFrameRate;
    }

// ---------------------------------------------------------------------------
// Continues refresh
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfEnv::ContinueRefresh()
    {
    Client().EnvContinueRefresh();
    }

// ---------------------------------------------------------------------------
// Pauses refresh
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfEnv::PauseRefresh()
    {
    Client().EnvPauseRefresh();
    }
  
// ---------------------------------------------------------------------------
// Called when a redraw is wanted
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CAlfEnv::RefreshCallBack(TAny* aInstance)
    {
    CAlfEnv* self = static_cast<CAlfEnv*>( aInstance );
    self->Client().EnvRefreshCallBack();
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// Returns the first display
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfDisplay& CAlfEnv::PrimaryDisplay() const
    {
    return *iData->iDisplays[0];
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextureManager& CAlfEnv::TextureManager() const
    {
    return *iData->iTextureManager;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextStyleManager& CAlfEnv::TextStyleManager() const
    {
    return *iData->iTextStyleManager;
    }

// ---------------------------------------------------------------------------
// From class MAknWsEventObserver
// Controls server based on raw ws events
// ---------------------------------------------------------------------------
//
void CAlfEnv::HandleWsEventL(const TWsEvent& aEvent, CCoeControl* /*aDestination*/)
    {
    __ASSERT_ALWAYS(iData, USER_INVARIANT());

	TInt eventType = aEvent.Type();
	// block these two here to avoid any extra execution beneath avkon level
    if (eventType == KAknFullOrPartialForegroundLost ||
        eventType == KAknFullOrPartialForegroundGained )
        {        
        const TBool foreground = (eventType == KAknFullOrPartialForegroundGained);      
         
        iData->iClient->ApplicationIsForeground(foreground);
        iData->iScheduler->AppicationOnForeground(foreground);

        if (foreground)
            {
            ReportWsEventAsActionCommand(KAlfActionIdForegroundGained); 
            }
        else
            {            
            ReportWsEventAsActionCommand(KAlfActionIdForegroundLost);    
            }                                    
        } 
    else if (eventType == EEventFocusGained)
        {
        ReportWsEventAsActionCommand(KAlfActionIdFocusGained);            
        }        
    else if (eventType == EEventFocusLost)
        {
        ReportWsEventAsActionCommand(KAlfActionIdFocusLost);                        
        }        
    else if (eventType == KEikInputLanguageChange)
        {
        ReportWsEventAsActionCommand(KAlfActionIdInputLanguageChanged);    
        }
    else if (eventType == EEventUser)
        {
        TApaSystemEvent* systemEvent = (TApaSystemEvent*) aEvent.EventData();
        if (*systemEvent == EApaSystemEventSecureShutdown)
            {
            ReportWsEventAsActionCommand(KAlfActionIdExitRequested);        
            }
        else if (CEikonEnv::Static())
            {                                         
            if (*systemEvent == EApaSystemEventShutdown && 
                !CEikonEnv::Static()->IsSystem())
                {
                ReportWsEventAsActionCommand(KAlfActionIdExitRequested);    
                }
            }
        }        
    else
        {
        // for PC lint
        }


    // - Skin change events are handled from CAlfSystemEventFetcher
    // - Layout change events are handled from CAlfDisplay
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::AddActionObserverL(MAlfActionObserver* aObserver)
    {
    if (iData->iActionObservers.Find(aObserver) == KErrNotFound)
        {
        iData->iActionObservers.AppendL(aObserver);        
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::RemoveActionObserver(MAlfActionObserver* aObserver)
    {
    TInt index = iData->iActionObservers.Find(aObserver); 
    if (index != KErrNotFound )
        {
        iData->iActionObservers.Remove(index);            
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfEnv::HandleActionL(const TAlfActionCommand& aActionCommand)
    { // handle in reverse order as action observer may remove it self during handling of command
    for (TInt ii = iData->iActionObservers.Count()-1; ii >= 0; ii-- )
        {
        iData->iActionObservers[ii]->HandleActionL(aActionCommand);
        }
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfEnv::Send(const TAlfCommand& aCommand, TInt aDelayMilliSeconds )
    {
    TRAPD( err , iData->iScheduler->ScheduleCommandL( aCommand, aDelayMilliSeconds ) );
    return err;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::CancelCommands( TAny* aObject )
    {
    if (iData->iScheduler)
        {
        iData->iScheduler->CancelCommands( aObject );
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::CancelCustomCommands(MAlfEventHandler* aObject)
    {
    if (iData->iScheduler)
        {
        iData->iScheduler->CancelCommands( aObject );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::CancelCommands(TAny* aObject, TAlfOp aCommandOperation)
    {
    if (iData->iScheduler)
        {
        iData->iScheduler->CancelCommands( aObject , aCommandOperation );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::CancelCommands( TAny* aObject, TAlfCommandType aCommandType )
    {
    // use overloaded CancelCustomCommands instead for custom commands
    __ASSERT_DEBUG( aCommandType != EAlfCommandTypeCustomEvent, USER_INVARIANT() );
    
    if (iData->iScheduler)
        {
        iData->iScheduler->CancelCommands( aObject, aCommandType, 0 );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfEnv::CancelCustomCommands(MAlfEventHandler* aObject, TInt aCustomParam )
    {
    if (iData->iScheduler)
        {
        iData->iScheduler->CancelCommands( aObject, EAlfCommandTypeCustomEvent, aCustomParam );
        }
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfEnv::MilliSecondsUntilCommand( TAny* aObject )
    {
    return iData->iScheduler->MilliSecondsUntilCommand( aObject ); 
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfEnv::MilliSecondsUntilCustomCommand(MAlfEventHandler* aObject)
    {
    return iData->iScheduler->MilliSecondsUntilCommand( aObject ); 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfEnv::MilliSecondsUntilCommand(TAny* aObject, TAlfOp aCommandOperation)
    {
    return iData->iScheduler->MilliSecondsUntilCommand( aObject , aCommandOperation ); 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfEnv::MilliSecondsUntilCommand( TAny* aObject, TAlfCommandType aCommandType )
    {
    // use overloaded CancelCustomCommands instead for custom commands
    __ASSERT_DEBUG( aCommandType != EAlfCommandTypeCustomEvent, USER_INVARIANT() );
    
    return iData->iScheduler->MilliSecondsUntilCommand( aObject, aCommandType, 0 ); 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CAlfEnv::MilliSecondsUntilCustomCommand(MAlfEventHandler* aObject, TInt aCustomParam )
    {
    return iData->iScheduler->MilliSecondsUntilCommand( aObject, EAlfCommandTypeCustomEvent, aCustomParam ); 
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CAlfEnv::HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType,
                                        CAlfDisplay* aAssocDisplay)
    {
    if(aAssocDisplay)
        {
        TAlfEvent event(*aAssocDisplay, aKeyEvent, aType);

        if(aAssocDisplay->Roster().HandleEventL(event))
            {
            return EKeyWasConsumed;
            }
        return EKeyWasNotConsumed;
        }

    return EKeyWasNotConsumed;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfEnv::BroadcastEventL(const TAlfEvent& aEvent)
    {
    for(TInt i = 0; i < iData->iDisplays.Count(); ++i)
        {
        TAlfEvent event = aEvent;
        event.SetDisplay( iData->iDisplays[i] );
        /** @todo  Musn't access the roster directly. */
        iData->iDisplays[i]->Roster().HandleEventL(event);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAlfEnv::ReportAction(const TAlfActionCommand& aCommand)
    {
    TInt resultError = KErrNone;

    for(TInt i = 0; i < iData->iActionObservers.Count(); ++i)
        {
        TRAPD(err, iData->iActionObservers[i]->HandleActionL(aCommand));
        if(err != KErrNone && resultError == KErrNone)
            {
            // The first error code is returned.
            resultError = err;
            }
        }
    return resultError;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextureManager* CAlfEnv::CreateSharedTextureManagerL(TUid aUid)
    {
    CAlfTextureManager* retVal = NULL;
    RArray<TSharedTextureManagerEntry>& managers = (iData->iSharedTextureManagers);
    TInt count = managers.Count();
    for(TInt i = 0; i < count; i++)
      	{
    	if (managers[i].iTextureManager->ManagerUid() == aUid)
    		{
    		managers[i].iRefCount++;    			
    		retVal = managers[i].iTextureManager;
    		}
    	}
    	
    // Does not yet exist, create new one	
    if (!retVal)
        {
       	CAlfTextureManager* manager = CAlfTextureManager::NewL(*this, aUid);	  								                       
        TSharedTextureManagerEntry entry(manager);
        managers.Append(entry);
        retVal = manager;    	          
        retVal->AddLoadObserverL(this);
        retVal->AddStateObserverL(this);        
        }                

    return retVal;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::DeleteSharedTextureManager(TUid aUid)
    {
    RArray<TSharedTextureManagerEntry>& managers = (iData->iSharedTextureManagers);
    TInt count = managers.Count();
    for(TInt i = 0; i < count; i++)
      	{
    	if (managers[i].iTextureManager->ManagerUid() == aUid)
    		{
    		if (managers[i].iRefCount <= 1)
    		    {
                managers[i].iTextureManager->RemoveLoadObserver(this);
                managers[i].iTextureManager->RemoveStateObserver(this);
    		    delete managers[i].iTextureManager;
       			managers.Remove(i);
       			managers.Compress();    			        
    		    }
    		else
    		    {
    		    managers[i].iRefCount--;        
    		    }    			    
    		break;
    		}  								                       
        }                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextureManager* CAlfEnv::SharedTextureManager(TUid aUid) const
    {
    CAlfTextureManager* retVal = NULL;

    RArray<TSharedTextureManagerEntry>& managers = (iData->iSharedTextureManagers);
    TInt count = managers.Count();
    for(TInt i = 0; i < count; i++)
      	{
    	if (managers[i].iTextureManager->ManagerUid() == aUid)
    		{
            retVal = managers[i].iTextureManager;
         	break;
    		}  								                       
        }                
        
    return retVal;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfEnv::TextureLoadingCompleted(CAlfTexture& /*aTexture*/,
                                         TInt /*aTextureId*/,
                                         TInt /*aErrorCode*/)
    {
    // This is not used at the moment, but in the future may be needed.            
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfEnv::TextureManagerStateChanged(const CAlfTextureManager& /*aManager*/)
    {
    // When texture manager is loading, reduce framerate little.
/*
    if (aManager.State() == CAlfTextureManager::EIdle)
        {
        Client().EnvSetMaxFrameRate( iData->iMaxFrameRate );
        }
    else
        {        
        Client().EnvSetMaxFrameRate( iData->iMaxFrameRate/2 );            
        }            
*/
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfRefreshMode CAlfEnv::RefreshMode()
    {
    return iData->iRefreshMode;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfRenderer CAlfEnv::Renderer() const
    {
    return TAlfRenderer(iData->iClient->EnvRenderer());            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::SetIdleThreshold(TInt aMilliSeconds)
    {
    Client().EnvSetIdleThreshold(aMilliSeconds);    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::Release(TBool aReleaseSharedTextures)
    {
    iData->iTextStyleManager->ReleaseAllVisuals();
    
    iData->iTextureManager->Release();
    if (aReleaseSharedTextures)
        {
        RArray<TSharedTextureManagerEntry>& managers = (iData->iSharedTextureManagers);
        TInt count = managers.Count();
        for(TInt i = 0; i < count; i++)
          	{
        	managers[i].iTextureManager->Release();    	  								                       
            }                            
        }        

    }
   
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::RestoreL()
    {
    iData->iTextureManager->RestoreL();
    RArray<TSharedTextureManagerEntry>& managers = (iData->iSharedTextureManagers);
    TInt count = managers.Count();
    for(TInt i = 0; i < count; i++)
      	{
    	managers[i].iTextureManager->RestoreL();    	  								                       
        }                            
    iData->iTextStyleManager->RefreshAllVisuals();
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::SetFullScreenDrawing(TBool aEnable)
    {
    Client().SetFullScreenDrawing(aEnable);
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::NotifySkinChangedL()
    {
    SetSkinChangePending(EFalse);       
    iData->iTextureManager->NotifySkinChangedL();
    RArray<TSharedTextureManagerEntry>& managers = (iData->iSharedTextureManagers);
    TInt count = managers.Count();
    for(TInt i = 0; i < count; i++)
      	{
    	managers[i].iTextureManager->NotifySkinChangedL();    	  								                       
        }                            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::NotifyLayoutChangedL()
    {
    SetLayoutChangePending(EFalse); 
    for(TInt i = 0; i < iData->iDisplays.Count(); ++i)
        {
        iData->iDisplays[i]->NotifyLayoutChangedL();
        }        
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C RFTokenClient* CAlfEnv::TokenClient()
    {
    ASSERT(iData);
    if (!(iData->iFlags&ETokenClientConnected))
        {
        TInt err = iData->iTokenClient.Connect();
        if (!err)
            {
            iData->iFlags |= ETokenClientConnected;
            }
        }
    
    if (iData->iFlags&ETokenClientConnected)
        {
        return &iData->iTokenClient;
        }
    else 
        {
        return NULL;
        }
    }

// ---------------------------------------------------------------------------
// for better compatibility with different S60 releases
// ---------------------------------------------------------------------------
//
void CAlfEnv::MonitorWsMessage(const TWsEvent& aEvent)
    {
    TRAP_IGNORE(CAlfEnv::HandleWsEventL(aEvent, 0))
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLayoutMetricsUtility* CAlfEnv::LayoutMetricsUtility()
    {
    return iData->iLayoutMetricsUtility;    
    }

// ---------------------------------------------------------------------------
// DEPRECATED: Widget code removed!
// ---------------------------------------------------------------------------
//
EXPORT_C IAlfWidgetFactory& CAlfEnv::WidgetFactory()const
    {
    IAlfWidgetFactory* ret(NULL); 
    return *ret;    
    }

// ---------------------------------------------------------------------------
// DEPRECATED: Widget code removed!
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfEnv::AppendWidgetL(IAlfWidget* /*aAlfWidget*/)
    {
    }

// ---------------------------------------------------------------------------
// DEPRECATED: Widget code removed!
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfEnv::DestroyWidget(IAlfWidget* /*aAlfWidget*/)
    {
    return 0;    
    }

// ---------------------------------------------------------------------------
// DEPRECATED: Widget code removed!
// ---------------------------------------------------------------------------
//
EXPORT_C IAlfWidget* CAlfEnv::FindWidget(const char* /*aWidgetName*/) const
    {
    IAlfWidget* ret(NULL);
    return ret;
    }

// ---------------------------------------------------------------------------
// Uses CAlfStatic directly. friend is needed to access dll static 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfEnv* CAlfEnv::Static()
    {
    if ( CAlfStatic::Data() )
        {
        return &CAlfStatic::Env();
        }
        
    return 0;
    }

// ---------------------------------------------------------------------------
// Support for extreme usecases where there are several controls but they don't
// belong to ctrl groups (so env cannot deliver the event by it self, but 
// application needs to fetch events based on notifications from environment) 
// ---------------------------------------------------------------------------
//
EXPORT_C TPointerEvent* CAlfEnv::LastReceivedPtrEvent()
    {
    if (iData && iData->iPointerEventAo)
        {
        return &iData->iPointerEventAo->iEvent.iEvent;
        }
    return 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TVersion CAlfEnv::Version() const
    {
    return TVersion(ALF_VERSION_MAJOR, ALF_VERSION_MINOR, ALF_VERSION_BUILD);    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TVersion CAlfEnv::ApiVersion() const
    {
    return iData->iApiVersion;    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBatchBuffer& CAlfEnv::BatchBufferHandler() const
    {
    return *iData->iBatchBufferHandler;
    }

void CAlfEnv::ReportWsEventAsActionCommand(TInt aActionCommandId)
    {
    if (aActionCommandId == KAlfActionIdDeviceLayoutChanged) 
        {            
        SetLayoutChangePending(ETrue); 
        ReportAction(TAlfActionCommand(aActionCommandId));      
        // If action obeserver did not call NotifyLayoutChangedL, we do it.
        if (LayoutChangePending())
            {
            TRAP_IGNORE(NotifyLayoutChangedL());    
            }
        }
    else
        {
        ReportAction(TAlfActionCommand(aActionCommandId));      
        }            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CAlfEnv::SkinChangePending() const
    {
    return iData->iNotifySkinChangePending;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfEnv::SetSkinChangePending(TBool aPending)
    {
    iData->iNotifySkinChangePending = aPending;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CAlfEnv::LayoutChangePending() const
    {
    return iData->iNotifyLayoutChangePending;   
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfEnv::SetLayoutChangePending(TBool aPending)
    {
    iData->iNotifyLayoutChangePending = aPending;    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfEnv::HandleTextureInfo( const TDesC8& aEventData )
    {
    // Notify texture manager with new information.

    TAlfTextureInfoEvent eventData;
    TPckg< TAlfTextureInfoEvent > eventBuf( eventData );
    const TInt eventBufMaxLength = eventBuf.MaxLength();
    TPtrC8 remainder( aEventData );
    
    while ( eventBufMaxLength <= remainder.Length() )
        {
        // Get event data - note that as data is copied to local buffer,
        // there is no need to worry about padding issues.
        eventBuf.Copy( remainder.Left( eventBufMaxLength ) );
        remainder.Set( remainder.Mid( eventBufMaxLength ) );
        
        // Inform texture manager.
        TRAP_IGNORE(iData->iTextureManager->ReportTextureInfoL( 
            eventData.iTextureId, 
            eventData.iTextureSize ));      
        }
            
    // Note: texture information events are not delivered to shared texture
    // managers (iData->iSharedTextureManagers) as this feature is not supported
    // for those.    
    }



EXPORT_C MAlfEnvObject* CAlfEnv::Extension( TInt aUid) const
    {
    TPrivateData::TObjectHolder holder;
    holder.iUid = aUid;
    TInt index = iData->iExtensionArray.Find(holder);
    if (index!=KErrNotFound)
        {
        return iData->iExtensionArray[index].iObject;
        }
    else
        {
        return NULL;
        } 
    }

EXPORT_C TInt CAlfEnv::AddExtension( TInt aUid, MAlfEnvObject* aNewExtension)
    {
    TPrivateData::TObjectHolder holder;
    holder.iUid = aUid;
    holder.iObject = aNewExtension;
    return iData->iExtensionArray.Append(holder);
    }

EXPORT_C void CAlfEnv::HandlePointerEventL(const TPointerEvent& aPointerEvent, 
                                   CAlfDisplay& aAssocDisplay)
    {
    aAssocDisplay.HandlePointerEventL(aPointerEvent);
    }

// ---------------------------------------------------------------------------
// Returns index of the display
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfEnv::FindDisplayIndex(const CAlfDisplay& aDisplay) const
    {
    TInt index = iData->iDisplays.Find(&aDisplay);
    return index;
    }
    
// ---------------------------------------------------------------------------
// Returns indexed display
// ---------------------------------------------------------------------------
//    
EXPORT_C CAlfDisplay& CAlfEnv::Display( TInt aIndex) const
    {
    return *iData->iDisplays[aIndex];
    }

