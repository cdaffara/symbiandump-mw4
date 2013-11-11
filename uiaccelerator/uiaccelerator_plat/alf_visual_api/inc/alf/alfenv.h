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
* Description:   Environment class.
*
*/



#ifndef C_ALFENV_H
#define C_ALFENV_H

#include <e32base.h>
#include <aknappui.h>
#include <AknWsEventObserver.h>
#include <alf/alfcommand.h>
#include <alf/alftexturemanager.h>
#include <alf/alfversion.h>

class RAlfClient;
class CAlfDisplay;
class CAlfDisplayCoeControl;
class CAlfControlGroup;
class CAlfControl;
class TAlfEvent;
class CAlfTextureManager;
class CAlfTextStyleManager;
class RFTokenClient;
class CAlfLayoutMetricsUtility;
class CAlfBatchBuffer;
class MAlfEnvObject;

namespace Alf
    {
    class IAlfWidgetFactory;            
    class IAlfWidget;
    }

    
using namespace Alf; 

const TInt KAlfOrpheanPtrEventReceived(0x2000B4D0);


/** Device screensize or ui style has changed */
const TInt KAlfActionIdDeviceLayoutChanged(KEikDynamicLayoutVariantSwitch);

/** Ui skin graphics has been changed */
const TInt KAlfActionIdSkinChanged(KAknsMessageSkinChange);

/** Application is now in the focused app */
const TInt KAlfActionIdFocusGained(0x200110DB); 

/** Application is no more the focused app */
const TInt KAlfActionIdFocusLost(0x200110DC);

/** Application is now brought to foreground */
const TInt KAlfActionIdForegroundGained(0x200110DD);

/** Application is no more in foreground */
const TInt KAlfActionIdForegroundLost(0x200110DE);

/** Device input language has been changed */
const TInt KAlfActionIdInputLanguageChanged(0x200110DF);

/** Application has been requested to exit (e.g. from a task manager or OOM watcher) */
const TInt KAlfActionIdExitRequested(0x200110E0);


/**
 * TAlfRefreshMode modes can be used to define when the screen redraws
 * should be performed.
 * @see CAlfEnv::SetRefreshMode()
 */
enum TAlfRefreshMode
    {
    /** Refresh occurs only when requested by calling CAlfEnv::RefreshCallBack(). */
    EAlfRefreshModeManual,

    /** Refresh is done automatically when necessary. */
    EAlfRefreshModeAutomatic, // = default

    /** Refresh is done all the time, even if nothing has changed. Dirty regions
        are not used for determining what has changed. Instead, displays are
        always fully redrawn. */
    EAlfRefreshModeForced
    };
    
/**
 * @enum TAlfRenderer
 * 
 * Enumerates renderers. The renderer depends on the used hardware.
 * 
 * Different renderers may not support all features available.
 * For example the EAlfRendererBitgdi does not support any 3D-related
 * methods, whereas EAlfRendererGles11 supports most of them.
 * 
 */
enum TAlfRenderer
    {
    /** Symbian BITGDI renderer. */
    EAlfRendererBitgdi      = 1,

    /** OpenVG 1.0 renderer (Not implemented). */
    EAlfRendererVg10        = 2,

    /** OpenGL ES 1.0 renderer. */
    EAlfRendererGles10      = 3,

    /** OpenGL ES 1.1 renderer. */
    EAlfRendererGles11      = 4,

    /** OpenGL ES 1.1 DSA renderer. Implemented, but please use EAlfRendererGles11. */
    EAlfRendererGles11DSA   = 5,

    /** OpenGL ES 2.0 renderer (Not implemented). */
    EAlfRendererGles20      = 6
    };

    

class MAlfActionObserver
    {
public:

    /**
     * Called by the server when an action command is executed.
     *
     * @param aActionCommand  The command that is being executed.
     */
    virtual void HandleActionL(const TAlfActionCommand& aActionCommand) = 0;

    };

/**
 *  This is the UI Accelerator Toolkit environment object. It needs to be created before any other toolkit
 *  functionality can be used.
 *
 *  Through environment it is possible to create display objects that are required to display graphics.
 *  It is also possible to create control groups that are required to include controls in.
 *
 *  Usually an application creates an environment object, after which it creates a single display object
 *  to handle the drawing surface of the application. After this at least one control group is required
 *  that can be used to encapsulate application controls. Controls themselves need to be derived from
 *  CAlfControl - base class.
 *
 *  @code
 *  // Create CAlfEnv
 *  iEnv = CAlfEnv::NewL();
 *
 *  // Create a display
 *  CAlfDisplay& display = iEnv->NewDisplayL(aRect, CAlfEnv::ENewDisplayAsCoeControl);
 *	
 *  // Create a control group
 *  CAlfControlGroup& group = iEnv->NewControlGroupL(KIdMainGroup);
 *  CMyControl* control = CMyControl::NewL(aEnv);
 *  group.AppendL(control);
 *
 *  display.Roster().ShowL(group);
 *  @endcode 
 *  
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfEnv ): public CBase, public MAknWsEventObserver, 
    public MAlfActionObserver, public MAlfTextureManagerStateChangedObserver, public MAlfTextureLoadingCompletedObserver, public MCoeMessageMonitorObserver
    {

public:

    /**
     * Flags for creating new displays.
     * @see NewDisplayL()
     */
    enum TNewDisplayFlags
        {
        /** Newly created display will fill the whole device screen. */
        ENewDisplayFullScreen = 0x1,        
        ENewDisplayOverlaid = 0x2,
        /**
         * Newly created display will be created as a CCoeControl - derived object.
         * This will ensure that key events are automatically delivered to the
         * correct CAlfControl - derived objects within the roster of the newly
         * created display.
         */        
        ENewDisplayAsCoeControl = 0x4
        };


    /**
     * Constructor
     * @param aVersion Default parameters should always be used as parameters for compatibility reasons.
     */
    IMPORT_C static CAlfEnv* NewL(TVersion aVersion = TVersion(ALF_VERSION_MAJOR, ALF_VERSION_MINOR, ALF_VERSION_BUILD));

    /**
     * Constructor
     * @param aVersion Default parameters should always be used  as parameters for compatibility reasons.
     */
    IMPORT_C static CAlfEnv* NewLC(TVersion aVersion = TVersion(ALF_VERSION_MAJOR, ALF_VERSION_MINOR, ALF_VERSION_BUILD));

    /**
     * Destructor
     */
    IMPORT_C virtual ~CAlfEnv();

    /**
     * Returns client
     *
     * @return Client to Alfred server
     */
    IMPORT_C RAlfClient& Client();
    
    /**
     * Creates a new display. You have to use ENewDisplayAsCoeControl - flag if you want to receive key events
     * automatically to the CAlfControl - derived objects in your application. If display is created without
     * ENewDisplayAsCoeControl - flag the key events have to be passed to the system manually using 
     * the HandleKeyEventL() - method.
     *
     * @param aRect   Size of the display.
     * @param aFlags  Flags for the display as a combination of flags from CAlfEnv::TNewDisplayFlags enumerator.
     * @param aDisplayType Type of the display, flags from CAlfDisplay::TDisplayType enumerator.
     * @return        Newly created CAlfDisplay object.
     */
    IMPORT_C CAlfDisplay& NewDisplayL( const TRect& aRect,
                                       TInt aFlags = 0, 
                                       TInt aDisplayType = 0  );
                                      
    /**
     * Creates a new display. You have to use ENewDisplayAsCoeControl - flag if you want to receive key events
     * automatically to the CAlfControl - derived objects in your application. If display is created without
     * ENewDisplayAsCoeControl - flag the key events have to be passed to the system manually using 
     * the HandleKeyEventL() - method.
     *
     * @param aRect   Size of the display.
     * @param aFlags  Flags for the display as a combination of flags from CAlfEnv::TNewDisplayFlags enumerator.
     * @param aRosterOwningDisplay Not used. Display that owns shared roster. 
     * @param aDisplayType Type of the display, flags from CAlfDisplay::TDisplayType enumerator.
     * @param aBackBufferUid Screen back buffer uid
     * @return        Newly created CAlfDisplay object.
     */
	IMPORT_C CAlfDisplay& NewDisplayL( const TRect& aRect, 
									   TInt aFlags, 
									   CAlfDisplay* aRosterOwningDisplay, 
                                       TInt aDisplayType, 
                                       TUid aBackBufferUid );
                                      
    /**
     * Removes a display from the environment.
     *
     * @param aDisplay  The display to remove.
     */
    void RemoveDisplay( CAlfDisplay& aDisplay );
    
    /**
     * Constructs a new empty control group.
     * Leaves with KErrAlreadyExists if the group with the same ID is already
     * created.
     *
     * @param aId (Resource) Identifier of the new group.
     * @return  Reference to the new group.
     */                                  
    IMPORT_C CAlfControlGroup& NewControlGroupL( TInt aId );
    
    /**
     * Deletes a previously loaded control group. The controls inside the
     * groups will also be destroyed. Note that this will also destroy the
     * visuals of the controls, because they are owned by the controls.
     *
     * @param aId  Identifier of the group to be deleted.
     *
     * @return  <code>KErrNone</code>, if the deletion is successful.
     *          <code>KErrNotFound</code>, if there are no control groups
     *          with the specified identifier.
     */
    IMPORT_C TInt DeleteControlGroup(TInt aId);
    
    /**
     * Returns an existing control group based on its identifier.
     * @note Will panic with TAlfPanic::EInternal if the specified
     * control group is not found.
     *
     * @param aResourceId Control group ID.
     * @return Contorl group
     */
    IMPORT_C CAlfControlGroup& ControlGroup(TInt aResourceId);
    
    /**
     * Returns an existing control group based on its identifier if
     * it is found. Returns NULL if the group cannot be found.
     *
     * @param aResourceId Control group ID.
     * @return Contorl group. Ownership not transferred. NULL if not found.
     */ 
    IMPORT_C CAlfControlGroup* FindControlGroup(TInt aResourceId) const;
    
    /**
     * Finds a control from the control groups of this
     * environment.
     *
     * @param aId The id of the control.
     * @param aUserId searches user given id if set true, otherwise uses system generated identifier 
     *
     * @see CAlfControl::SetId()
     * @see CAlfControl::Id()
     * @see CAlfControl::Identifier()
     * @return The control instance, or NULL if no such control exists.
     */
    IMPORT_C CAlfControl* FindControl(TInt aId, TBool aUserId = ETrue) const;
    
    /**
     * Determines the number of displays currently created with NewDisplayL().
     *
     * @return  Display count.
     */
    IMPORT_C TInt DisplayCount() const;
    
    /**
     * Sets the refresh mode.
     *
     * @param aMode  New refresh mode.
     *
     * @see TAlfRefreshMode
     */
    IMPORT_C void SetRefreshMode(TAlfRefreshMode aMode);

    /**
     * Gets the refresh mode.
     *
     * @return  Current refresh mode.
     *
     * @see TAlfRefreshMode
     */
    IMPORT_C TAlfRefreshMode RefreshMode();
    
    /**
     * Sets the maximum allowed frame rate. Specify 0.0 to use the default limit.
     *
     * @param aFrameRate  New maximum frame rate in frames per second.
     */
    IMPORT_C void SetMaxFrameRate(TReal32 aFrameRate) __SOFTFP;
    
    /**
     * Continues normal display refreshing. If display refreshing is already
     * being done, calling this has no effect. This is called automatically
     * from inside the toolkit when something is done that may lead to a
     * visible change on a display. The application may also call this if it
     * has previously paused refresh with PauseRefresh().
     */
    IMPORT_C void ContinueRefresh();

    /**
     * Pauses display refreshing temporarily. This is called when it is clear
     * that any displays will not be updated soon. The application may call
     * this at times when it needs more processing time for other tasks.
     */
    IMPORT_C void PauseRefresh();
    
    /**
     * Refresh callback, that is called by a refresh timer. Called periodically
     * when there might be something to update on the display. Has the
     * responsibility to update time for all ALF environments.
     *
     * @param aInstance  A CAlfEnv instance.
     * @see StartRefresh() to modify how often updates are being made.
     * @see StopRefresh() to stop updates.
     */
    IMPORT_C static TInt RefreshCallBack(TAny* aInstance);
    
    /**
     * Returns the primary display, that is the display
     * that was created first.
     *
     * @return Display
     */
    IMPORT_C CAlfDisplay& PrimaryDisplay() const;

    /**
     * Returns the default texture manager.
     * @return Default texture manager of this environment.
     */
    IMPORT_C CAlfTextureManager& TextureManager() const;

    /**
     * Returns the text style manager.
     * @return Default text style manager of this environment.
     */
    IMPORT_C CAlfTextStyleManager& TextStyleManager() const;    
    
    /**
     * Creates a shared texture manager. If texture manager with given uid already 
     * exists, a pointer to it is returned and reference count is increased. 
     *
     * Shared texturemanager can be used to create/load textures that are 
     * shared between components/applications to preserve memory. 
     *
     * @param aUid Unique identifier for the shared texture manager
     * @return Shared texture manager
     *   
     * Example of shared texturemanager usage:
     *
     * CScrollBar::ConstructL()
     *     {
     *     // Create shared texturemanager instead of default texture manager to save RAM
     *     // because same texture is used in multiple components in different apps.
     *     CAlfTextureManager* sharedManager = 
     *         iEnv->CreateSharedTextureManagerL(KScrollBarTextureManagerUid);
     *     
     *     // Create texture, if it already exists inside this application or in some
     *     // other application, then bitmap from bitmapprovider and texture flags are ignored
     *     // and already existing texture is returned.
     *     iBackgroundTexture = sharedManager->CreateTexture(KScrollBarBackgroudId, 
     *         iBitmapProvider,
     *         EAlfTextureFlagDefault);
     *     
     *     // Create another texture...
     *     iHandleTexture = sharedManager->CreateTexture(KScrollBarHandleId, 
     *         iBitmapProvider,
     *         EAlfTextureFlagDefault);
     *     }
     *
     * CScrollBar::~CScrollBar()
     *     {
     *     // We created shared texture manager so we should delete it here.
     *     // If this is last instance of this shared texture manager in the device, 
     *     // all of its textures get unloaded.
     *     iEnv->DeleteSharedTextureManagerL(KScrollBarTextureManagerUid);
     *     }
     */
    IMPORT_C CAlfTextureManager* CreateSharedTextureManagerL(TUid aUid);

    /**
     * This method deletes shared texture manager and all its textures get released if there
     * are no other references to the texture manager. Otherwise only the reference count
     * is decreased.
     * 
     * @param aUid Unique identifier of the shared texture manager
     *
     */
    IMPORT_C void DeleteSharedTextureManager(TUid aUid);

    /**
     * Returns pointer to a shared texture manager, NULL if it does not exist.
     *
     * @param aUid Unique identifier of the shared texture manager
     * @return Shared texture manager
     *   
     */
    IMPORT_C CAlfTextureManager* SharedTextureManager(TUid aUid) const;

     /**
     * Adds action observer
     *
     * @param aObserver observer to be added to array
     * @leave any system wide error code
     */
    IMPORT_C void AddActionObserverL(MAlfActionObserver* aObserver);

     /**
     * Removes action observer, this must be done at latest when observer is being deleted
     *
     * @param aObserver observer to be added to array
     */
    IMPORT_C void RemoveActionObserver(MAlfActionObserver* aObserver);
    
    
    /**
     * Schedule a command.
     *
     * @param aCommand  Command to send.
     * @param aDelayMilliSeconds  Time in ms to wait before executing the command. If
     * set to zero, the command is executed immediately. Note that the point of
     * execution may not be exactly this amount of milliseconds.
     *
     * @return  System-wide error code. If successful, returns <code>KErrNone</code>.
     */
    IMPORT_C TInt Send(const TAlfCommand& aCommand, TInt aDelayMilliSeconds = 0);
    
    /**
     * Cancel commands where object is aObject.
     *
     * @param aObject Command object.
     */
    IMPORT_C void CancelCommands( TAny* aObject );
    
    /**
     * Cancel commands where object is aObject and operation is aCommandOperation.
     *
     * @param aObject Command object.
     * @param aCommandOperation Command operation.
     */
    IMPORT_C void CancelCommands( TAny* aObject, TAlfOp aCommandOperation);

    /**
     * Cancel commands where object is aObject and type is aCommandType.
     *
     * @param aObject Command object.
     * @param aCommandType Command type.
     */
    IMPORT_C void CancelCommands( TAny* aObject, TAlfCommandType aCommandType );
 
    /**
     * Cancel custom commands where object is aObject.
     *
     * @param aObject Command object.
     */   
    IMPORT_C void CancelCustomCommands( MAlfEventHandler* aObject ); 
    
    /**
     * Cancel commands where object is aObject and custom parater is aCustomParam 
     *
     * @param aObject Command object.
     * @param aCustomParam Custom paramerer.
     */
    IMPORT_C void CancelCustomCommands( MAlfEventHandler* aObject, TInt aCustomParam );         
    
    /**
     * Time until next command where object is aObject.
     *
     * @param aObject Command object.
     * @return Time left in milliseconds to the first found command, which 
     *         matched the criteria. KErrNotFound if the none found. The return
     *         value is set to 0 if the command timer has expired and waits
     *         execution from the active scheduler.
     */
    IMPORT_C TInt MilliSecondsUntilCommand( TAny* aObject );
    
    /**
     * Time until next command where object is aObject and operation is aCommandOperation 
     *
     * @param aObject Command object.
     * @param aCommandOperation Custom paramerer.
     * @return Time left in milliseconds to the first found command, which 
     *         matched the criteria. KErrNotFound if the none found. The return
     *         value is set to 0 if the command timer has expired and waits
     *         execution from the active scheduler.
     */
    IMPORT_C TInt MilliSecondsUntilCommand( TAny* aObject, TAlfOp aCommandOperation);

    /**
     * Time until next commands where object is aObject and type is aCommandType.
     *
     * @param aObject Command object.
     * @param aCommandType Command type.
     * @return Time left in milliseconds to the first found command, which 
     *         matched the criteria. KErrNotFound if the none found. The return
     *         value is set to 0 if the command timer has expired and waits
     *         execution from the active scheduler.
     */
    IMPORT_C TInt MilliSecondsUntilCommand( TAny* aObject, TAlfCommandType aCommandType );
        
    /**
     * Time until next custom commands where object is aObject.
     *
     * @param aObject Command object.
     * @return Time left in milliseconds to the first found command, which 
     *         matched the criteria. KErrNotFound if the none found. The return
     *         value is set to 0 if the command timer has expired and waits
     *         execution from the active scheduler.
     */     
    IMPORT_C TInt MilliSecondsUntilCustomCommand( MAlfEventHandler* aObject ); 
    
    /**
     * Time until next commands where object is aObject and custom parater is aCustomParam 
     *
     * @param aObject Command object.
     * @param aCustomParam Custom paramerer.
     * @return Time left in milliseconds to the first found command, which 
     *         matched the criteria. KErrNotFound if the none found. The return
     *         value is set to 0 if the command timer has expired and waits
     *         execution from the active scheduler.
     */
    IMPORT_C TInt MilliSecondsUntilCustomCommand( MAlfEventHandler* aObject, TInt aCustomParam ); 
                                  
    /**
     * Handles a key event received from CONE. This can be called directly by application
     * to pass in external key events. You will have to call this from an application to pass
     * key events to CAlfControl - derived objects unless you have created the CAlfDisplay 
     * class with the ENewDisplayAsCoeControl flag.
     *
     * @param aKeyEvent  Received key event to handle.
     * @param aType      Type of event.
     * @param aAssocDisplay  Associated CAlfDisplay, if there is one.
     *                   If specified, the event is handled by this display's
     *                   roster only. If <code>NULL</code>, event is offered
     *                   to all displays.
     */
    IMPORT_C TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType,
                                   CAlfDisplay* aAssocDisplay = 0);


    /**
     * Handles a pointer event received from CONE. This can be called directly by application
     * to pass in external pointer events. You will have to call this from an application to pass
     * pointer events to CAlfControl - derived objects if you have created the CAlfDisplay 
     * class as offscreen buffer that has no window.
     *
     * @param aPointerEvent  Received pointer event to handle. Note that iParentPosition of the 
     * pointerevent should always be in the same coordinate space as where the displays rect 
     * was defined when it was created.
     * @param aAssocDisplay  Associated CAlfDisplay.
     */
    IMPORT_C void HandlePointerEventL(const TPointerEvent& aPointerEvent, 
                                   CAlfDisplay& aAssocDisplay);
                                  

    /**
     * Determines which renderer is currently in use. Returns the real
     * explicit identifier of the renderer.
     *
     * @return  TAlfRenderer enum.
     */
    IMPORT_C TAlfRenderer Renderer() const;


    /**
     * Sets the interval before an idle notification is sent. Idling
     * begins when no input has been received after this delay.
     *
     * Also note that scheduled events may keep display refresh running even
     * when there's no input.
     *
     * @param aSeconds  Threshold in milliseconds.
     */
    IMPORT_C void SetIdleThreshold(TInt aMilliSeconds);

	/**
     * Releases as many resources as possible. Among the released resources 
     * may be all objects in the toolkit's ownership, for example texture 
     * images and drawing surfaces. This should be called when the Hitchcock 
     * user interface is not needed for a while, for example when the application 
     * goes to the background. 
     *
     * At the moment the Environment releases all textures. Shared textures
     * are also tried to release if so instructed in the params.
     * 
     * The resources need to be restored with a call to RestoreL() before
     * the user interface is used again.
     *
     * The AppUI can call this method to release resources. The AppUI can
     * overload HandleWsEventL() and react to KAknFullOrPartialForegroundLost
     * window server event by calling this method.
     *
     * @param aReleaseSharedTextures If ETrue also shared textures are released
     *  if there are no other apps using them.
     * @see RestoreL()
     */
    IMPORT_C void Release(TBool aReleaseSharedTextures=EFalse);
    
    /**
     * Restores all resources that were released with a call to Release().
     * After this the Hitchcock user interface should be in the same state
     * in terms of resources as it was prior to the Release() call.
     *     
     * The AppUI can call this method to Restore resources. The AppUI can
     * overload HandleWsEventL() and react to KAknFullOrPartialForegroundGained
     * window server event by calling this method.
     *
     * @see Release()
     */
    IMPORT_C void RestoreL();    

    /**
     * Notifies everyone of a change in the environment's skin. Note that this
     * method gets called automatically when system skin is changed.
     * 
     * Method is exported so that applications can call this in some special
     * situations when skinned graphics needs to be refreshed.
     */
    IMPORT_C void NotifySkinChangedL();    

    /**
     * Notifies everyone of a change in the environment's layout. Exported so that 
     * applications can call this from their HandleResourceChange() methods.
     */
    IMPORT_C void NotifyLayoutChangedL();    

    /**
    * defines if Alfred draws on top of (or below) application windowgroup 
    */
    IMPORT_C void SetFullScreenDrawing(TBool aEnable);

    IMPORT_C RFTokenClient* TokenClient();

    /**
     * @deprecated <b>Always returns an invalid reference!</b>
     * 
     * Widget factory for widget and their part creation 
     */
    IMPORT_C IAlfWidgetFactory& WidgetFactory()const;
    
    /**
     * @deprecated <b>Doesn't do anything anymore!</b>
     * 
     * Append widgets into environment
     */
    IMPORT_C void AppendWidgetL(IAlfWidget* aAlfWidget);

    /**
     * @deprecated <b>Doesn't do anything anymore!</b>
     * 
     * Destroy a widget from the environment
     */
    IMPORT_C TInt DestroyWidget(IAlfWidget* aAlfWidget);
    
    /**
     * @deprecated <b>Always returns an invalid reference!</b>
     * 
     * Find named widgets owned by environment 
     */
    IMPORT_C IAlfWidget* FindWidget(const char* aWidgetName)const;

    void BroadcastEventL(const TAlfEvent& aEvent);
    
    TInt ReportAction(const TAlfActionCommand& aCommand);

    // From MAlfTextureLoadingCompletedObserver
    void TextureLoadingCompleted(CAlfTexture& aTexture,
                                         TInt aTextureId,
                                         TInt aErrorCode);

    // From MAlfTextureManagerStateChangedObserver
    void TextureManagerStateChanged(const CAlfTextureManager& aManager);

    //From MCoeMessageMonitorObserver
	void MonitorWsMessage(const TWsEvent& aEvent);

    /**
     * Gets pointer into layout metrics utility.
     */
    IMPORT_C CAlfLayoutMetricsUtility* LayoutMetricsUtility();
    
    /**
     * Gets pointer to application enviroment singleton
     * @return singleton or NULL
     */
    IMPORT_C static CAlfEnv* Static();
    
    /**
     * !Direct access to Symbian pointer event data. This information is usually relevant only when
     * associated action command KAlfOrpheanPtrEventReceived is sent by environment 
     * @return null or last received pointer event
     */
     IMPORT_C TPointerEvent* LastReceivedPtrEvent();
    
    /**
     * Returns version of the Alf in runtime environment.
     * @return Version of the runtime Alf
     */
     IMPORT_C TVersion Version() const;

     /**
     * Returns version of the Alf that application was compiled against
     * @return Version of the compiletime Alf
     */
    TVersion ApiVersion() const;   
    
    /**
     * Returns the batch buffer handler.
     *
     * @return Batch buffer handler
     */
    IMPORT_C CAlfBatchBuffer& BatchBufferHandler() const;
   
   
    /**
     * Handles texture info changed event.
     * @param aEventData descriptor containing event data.
     */
    void HandleTextureInfo( const TDesC8& aEventData );
    
    TBool SkinChangePending() const;

    void SetSkinChangePending(TBool aPending);

    TBool LayoutChangePending() const;

    void SetLayoutChangePending(TBool aPending);

    void ReportWsEventAsActionCommand(TInt aActionCommandId);
    
    /**
    * Access AlfEnv extensions via their UID
    *
    * AlfEnv holds a number of extensions that have been set there
    * by users of AlfEnv.  They are owned by AlfEnv and are 
    * deleted by it when the environment is destroyed. The objects
    * must derive from MAlfEnvObject.
    * 
    * The objects are added to the environment with a Uid. This Uid is
    * used here to access them.
    * 
    * @see AddExtension
    *
    * @param aUid   Uid corresponding to the object being requested
    * @return pointer to object requested, or NULL
    */
    IMPORT_C MAlfEnvObject* Extension( TInt aUid) const;

    /**
    * Add an extension object to AlfEnv
    *
    * Extension objects, derived from MAlfEnvObject, can be given
    * to AlfEnv, so that other AlfEnv clients can access the
    * singleton objects.
    * 
    * The objects are added with an accompanying Uid. This Uid is used
    * to access the object.
    *
    * Objects added to AlfEnv must have their Release() method implemented
    * in DLLs that remain in memory until after AlfEnv is deleted.
    *
    * @see Extension
    *
    * @param aUid   Uid corresponding to the object being requested
    * @param aNewExtension   Pointer to the new object given to the system
    * @return an error code. KErrNone if there has been no error
    */
    IMPORT_C TInt AddExtension( TInt aUid, MAlfEnvObject* aNewExtension);

    /**
     * Finds the index of a display in the environment.
     *
     * @param aDisplay  Display.
     * @return Index. KErrNotFound if not found.
     */
    IMPORT_C TInt FindDisplayIndex(const CAlfDisplay& aDisplay) const;
    
    /**
     * Returns one of the displays owned by the environment.
     * Panics with error code USER 130, if aIndex is negative, or is greater
     * than the number of displays in the array.
     *
     * @param aIndex  Index number of the display to return.
     *
     * @return  Display.
     */
    IMPORT_C CAlfDisplay& Display( TInt aIndex) const;
    

private:

    // Constructor
    CAlfEnv();
    
    // ConstructL
    void ConstructL(TVersion aVersion);
    
    // from MAknWsEventObserver
    void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);

    // from MAlfActionObserver
    void HandleActionL(const TAlfActionCommand& aActionCommand);    
     
private:
    
    // Private data. Own.
    struct TPrivateData;
    TPrivateData* iData;

    };

#endif // C_ALFENV_H
