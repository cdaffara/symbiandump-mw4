/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Defines CHuiEnv, a common working environment for the Toolkit.
*
*/



#ifndef __HUIENV_H__
#define __HUIENV_H__

#include <e32base.h>
#include <w32std.h>
#include <barsread.h>
#include <coemain.h>

#include <uiacceltk/HuiTextureManager.h>
#include <uiacceltk/huifontmanager.h>
#include <uiacceltk/HuiCommand.h>
#include <uiacceltk/HuiObserverArray.h>
#include <uiacceltk/HuiRenderer.h>
#include <uiacceltk/huilowmemoryobserver.h>

/* Forward declarations */
class CCoeControl;
class CHuiStatic;
class CHuiDisplay;
class CHuiRosterImpl;
class CHuiSkin;
class CHuiScheduler;
class CHuiControl;
class CHuiControlGroup;
class CHuiVisualFactory;
class CHuiVisual;
class MHuiActionObserver;
class THuiEvent;
class CDirectScreenAccess;
class MHuiEventHandler;
class CHuiTextStyleManager;
class CHuiThemeManager;
class CHuiCanvasTextureCache;
class CHuiFxEngine;
class MHuiSynchronizationObserver;
class CAppIconCache;

class CHighResTimer;
const TInt KHuiEnvReleasedActionId = 0x200100F8;
const TInt KHuiEnvRestoredActionId = 0x200100F9;


/**
 * THuiRefreshMode modes can be used to define when the screen redraws
 * should be performed.
 * @see CHuiEnv::SetRefreshMode()
 */
enum THuiRefreshMode
    {
    /** Refresh occurs only when requested by calling CHuiEnv::RefreshCallBack(). */
    EHuiRefreshModeManual,

    /** Refresh is done automatically when necessary. */
    EHuiRefreshModeAutomatic, // = default

    /** Refresh is done all the time, even if nothing has changed. Dirty regions
        are not used for determining what has changed. Instead, displays are
        always fully redrawn. */
    EHuiRefreshModeForced
    };


/**
 * HUI environment observer. Instance of this class can be created
 * by user application and passed in constructor to CHuiEnv. The
 * observer class will be called for certain events.
 */
class MHuiEnvObserver
	{
public:

    /**
     * Called to create a new CHuiSkin derived object for the use
     * of the calling CHuiEnv object. This method can return
     * either an application specific Skin - object or NULL for
     * allowing CHuiEnv to create it's default Skin object. The calling
     * CHuiEnv will take responsibility of the created CHuiSkin object.
     *
     * @param aEnv The CHuiEnv object to which to create the
     *             new skin object.
     *
     * @return New instance of a CHuiSkin - derived object.
     */
    virtual CHuiSkin* CreateSkinL(CHuiEnv& aEnv) = 0;

    };


/**
 * Interface for observing action commands. When the scheduler executes an
 * action command, the registered listeners that implement this interface
 * will receive a notification of the action.
 */
class MHuiActionObserver
    {
public:

    /**
     * Called by the scheduler when an action command is executed.
     *
     * @param aActionCommand  The command that is being executed.
     */
    virtual void HandleActionL(const THuiActionCommand& aActionCommand) = 0;

    };


/**
 * CHuiEnv is a common working environment for the Toolkit. CHuiEnv manages
 * the common data for a HUITK application, such as displays, the texture
 * manager, visual factory and currently used skin.
 *
 * An application that uses HUITK will need to create one CHuiEnv. Also
 * destroying the CHuiEnv instance will clean up all resources associted
 * with the UI.
 *
 * @see NewL()
 * @see NewLC()
 */
NONSHARABLE_CLASS(CHuiEnv) : public CBase,
                             public MHuiTextureManagerStateChangedObserver,
                             public MHuiTextureLoadingCompletedObserver
    {
public:

    /* Types. */

    /** Flags for creating new displays. */
    enum TNewDisplayFlags
        {
        ENewDisplayFullScreen = 0x1,
        ENewDisplayOverlaid = 0x2
        };


    /** @beginAPI */

    /* Constructors and destructor. */

    /**
     * Constructs a new HUI environment. An application that uses HUI will
     * need to create one CHuiEnv. This will not start the timing used
     * inside Hitchcock UI Toolkit. The animation timing will be started
     * from the point in time when the first frame is drawn. So UI build up
     * and precalculation can be done without affecting the animation time.
     *
     * @param aRenderer  Rendering interface to use in the environment.
     * @param aObserver  Pointer to observer class instance that will be called
     *                   to perform application specific configurations. Can be
     *                   NULL to create a default CHuiEnv.
     *
     * @return  HUI environment.
     */
    IMPORT_C static CHuiEnv* NewL(THuiRenderer aRenderer = EHuiRendererDefault,
                                  MHuiEnvObserver* aObserver = NULL);

    /**
     * Constructs a new HUI environment and leaves it on the cleanup stack.
     * This will not start the timing used inside Hitchcock UI Toolkit. 
     * The animation timing will be started from the point in time when 
     * the first frame is drawn. So UI build up and precalculation can be 
     * done without affecting the animation time. 
     *
     * @param aRenderer  Rendering interface to use in the environment.
     * @param aObserver  Pointer to observer class instance that will be called
     *                   to perform application specific configurations. Can be
     *                   NULL to create a default CHuiEnv.
     *
     * @return  HUI environment.
     */
    IMPORT_C static CHuiEnv* NewLC(THuiRenderer aRenderer = EHuiRendererDefault,
                                   MHuiEnvObserver* aObserver = NULL);

    /** @endAPI */


    /**
     * Destructor.
     */
    virtual ~CHuiEnv();


    /* Methods. */

    /** @beginAPI */

    /**
     * Determines which renderer is currently in use. Always returns the real
     * explicit identifier of the renderer, and not an ambiguous identifier
     * like "Default".
     *
     * @return  THuiRenderer enum.
     */
    IMPORT_C THuiRenderer Renderer() const;

    /**
     * Creates a new HuiDisplay.
     *
     * @param aRect  Size of the display.
     * @param aNativeControl  Native control to which to associate the display.
     * @param aFlags  Flags for the display (TNewDisplayFlags).
     * @param aRosterOwningDisplay  The display that will share its roster.
     * @param aDisplayType  Display type.
     *
     * @return  Display.
     */
    IMPORT_C CHuiDisplay& NewDisplayL(const TRect& aRect,
                                      CCoeControl* aNativeControl = 0,
                                      TInt aFlags = 0,
                                      CHuiDisplay* aRosterOwningDisplay  = NULL,
                                      TInt aDisplayType = 0 );

    /**
     * Creates a new HuiDisplay.
     *
     * @param aRect  Size of the display.
     * @param aNativeControl  Native control to which to associate the display.
     * @param aFlags  Flags for the display (TNewDisplayFlags).
     * @param aRosterOwningDisplay  The display that will share its roster.
     * @param aDisplayType  Display type.
     * @param aBackBufferUid  Screen back buffer uid.
     *
     * @return  Display.
     */
    IMPORT_C CHuiDisplay& NewDisplayL(const TRect& aRect,
                                      CCoeControl* aNativeControl,
                                      TInt aFlags,
                                      CHuiDisplay* aRosterOwningDisplay,
                                      TInt aDisplayType,
                                      TUid aBackBufferUid );


    /**
     * Creates a new HuiDisplay.
     *
     * @param aNativeWindow  Native window to which to associate the display.
     * @param aFlags  Flags for the display (TNewDisplayFlags).
     * @param aRosterOwningDisplay  The display that will share its roster.
     * @param aDisplayType  Display type.
     * @param aBackBufferUid  Screen back buffer uid.
     *
     * @return  Display.
     */
    IMPORT_C CHuiDisplay& NewDisplayL(RWindow* aNativeWindow,
                                      TInt aFlags,
                                      CHuiDisplay* aRosterOwningDisplay,
                                      TInt aDisplayType,
                                      TUid aBackBufferUid );

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
     * @see THuiRefreshMode
     */
    IMPORT_C void SetRefreshMode(THuiRefreshMode aMode);

    /**
     * Determines the environment's display refresh mode.
     *
     * @return  Refresh mode.
     */
    IMPORT_C THuiRefreshMode RefreshMode() const;

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
     * This does not affect any timing. 
     *
     * @see CHuiStatic
     */
    IMPORT_C void ContinueRefresh();

    /**
     * Continues normal display refreshing. If display refreshing is already
     * being done, calling this has no effect. This is called automatically
     * from inside the toolkit when something is done that may lead to a
     * visible change on a display. The application may also call this if it
     * has previously paused refresh with PauseRefresh().
     * @param aFastFirstRefresh If ETrue and refresh timer is not active,
     * first refresh will be done as soon as possible. Otherwise first refresh
     * will occur after refresh timer has expred.  
     *
     * @see CHuiStatic
     */
    void ContinueRefresh(TBool aFastFirstRefresh);

    /**
     * Pauses display refreshing temporarily. This is called when it is clear
     * that any displays will not be updated soon. The application may call
     * this at times when it needs more processing time for other tasks.
     */
    IMPORT_C void PauseRefresh();
    
    /**
     * Constructs a new empty control group.
     *
     * @param aId (Resource) Identifier of the new group.
     *
     * @return  Reference to the new group.
     */
    IMPORT_C CHuiControlGroup& NewControlGroupL(TInt aId);

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
     * Load controls and a group from a resource definition and create a
     * HuiControlGroup out of them.
     *
     * @param aResourceId  Resource identifier of the group.
     *
     * @return  Control group with controls inside.
     */
    IMPORT_C CHuiControlGroup& LoadControlGroupL(TInt aResourceId);

    /**
     * Returns an existing control group based on its identifier.
     * @note Will panic with THuiPanic::EInternal if the specified
     * control group is not found.
     */
    IMPORT_C CHuiControlGroup& ControlGroup(TInt aResourceId);

    /**
     * "Loads" bitmaps specified in a resource. Works with all
     * supported image formats, including BMP, PNG and JPG. The resource
     * defines a numerical identifier for each bitmap. The bitmaps
     * are added to the texture manager in this environment and can be
     * accessed using the numerical identifiers.
     * @see TextureManager()
     *
     * @note Does not really _load_ the bitmaps to textures. Instead, the
     * bitmap filenames are bound to the id:s specified by the resource. The
     * texture ids may be later used to load the texture resource (by calling
     * CHuiTextureManager::LoadTextureL() with the id).
     *
     * @see CHuiTextureManager::LoadTextureL()
     * @see CHuiTextureManager::TextureL()
     * @see CHuiTextureManager::Texture()
     *
     * @todo The bitmap resource file format is not documented.
     *
     * @param aResourceId Resource containing a list of bitmap file names.
     */
    IMPORT_C void LoadBitmapsL(TInt aResourceId);

    /**
     * Returns the visual factory.
     */
    IMPORT_C CHuiVisualFactory& VisualFactory() const;

    /**
     * Returns the texture manager.
     */
    IMPORT_C CHuiTextureManager& TextureManager() const;

    /**
     * Returns the font manager object.
     */
    IMPORT_C THuiFontManager& FontManager() const;
    
    /**
     * Returns the text style manager.
     */
    IMPORT_C CHuiTextStyleManager& TextStyleManager() const;

    /**
     * Finds a control from the control groups of this
     * environment.
     * @param aId The id of the control to find.
     * @see CHuiControl::SetId()
     * @see CHuiControl::Id()
     * @return The control instance, or NULL if no such control exists.
     */
    IMPORT_C CHuiControl* FindControl(TInt aId) const;

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
    IMPORT_C TInt Send(const THuiCommand& aCommand, TInt aDelayMilliSeconds = 0);

    /**
     * Schedule a command. Leaves if there is a failure when posting the command
     * to the scheduler.
     *
     * @param aCommand  Command to send.
     * @param aDelayMilliSeconds  Time to wait before executing the command.
     */
    IMPORT_C void SendL(const THuiCommand& aCommand, TInt aDelayMilliSeconds = 0);

    /**
     * Cancel commands related to an object. This object may be a timed value,
     * a visual, MHuiEventHandler, or anything you can do commands on.
     *
     * @param aObject Any commands owned by this object will be cancelled.
     *                Note that you must cast derived classes that are passed,
     *                otherwise the pointer will point to the base class.
     */
    IMPORT_C void CancelCommands(TAny* aObject);

    /**
     * Overloaded version of CancelCommands, especially for classes derived from
     * MHuiEventHandler, so that correct pointer will be used.
     * @see CHuiEnv::CancelCommands
     */
    IMPORT_C void CancelCommands(MHuiEventHandler* aObject);

    /**
     * Overloaded version of CancelCommands, especially for classes derived from
     * MHuiEventHandler, so that correct pointer will be used.
     * @see CHuiEnv::CancelCommands
     */
    IMPORT_C void CancelCommands(TAny* aObject,
                                 THuiOp aCommandOperation);

    /**
     * Overloaded version of CancelCommands, especially for classes derived from
     * MHuiEventHandler, so that correct pointer will be used.
     * @see CHuiEnv::CancelCommands
     */
    IMPORT_C void CancelCommands(TAny* aObject,
                                 THuiCommandType aCommandType,
                                 TInt aParam);

    /**
     * Time until the given command will be executed on the given object.
     * 
     * @param aObject The object that is waiting for a command to be executed.
     *                Note that you must cast derived classes that are passed,
     *                otherwise the pointer will point to the base class.     
     * @param aCommandType  Type of command.     
     * @return  Time interval in milliseconds. -1, if no commands found.
     */
    IMPORT_C TInt TimeUntilCommand(TAny* aObject,
                                   THuiCommandType aCommandType);

    /**
     * Returns the skin that is used by the environment. Only one skin can be
     * used at a time.
     */
    IMPORT_C CHuiSkin& Skin() const;

    /**
     * Starts the refresh timer with given tick interval. Can be used to
     * control how often the toolkit updates itself.
     *
     * @param aMilliSeconds  Milliseconds between ticks.
     */
    IMPORT_C void StartRefresh(TInt aMilliSeconds);

    /**
     * Starts the refresh timer with given tick interval. Can be used to
     * control how often the toolkit updates itself.
     *
     * @param aMilliSeconds  Milliseconds between ticks.
     * @param aDelayInMilliSeconds Milliseconds until first refrsh.
     */
     void StartRefresh(TInt aIntervalInMilliSeconds, TInt aDelayInMilliSeconds);

    /**
     * Handles a key event received from CONE.
     *
     * @param aKeyEvent  Received key event to handle.
     * @param aType      Type of event.
     * @param aAssocDisplay  Associated CHuiDisplay, if there is one.
     *                   If specified, the event is handled by this display's
     *                   roster only. If <code>NULL</code>, event is offered
     *                   to all displays.
     */
    IMPORT_C TBool HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType,
                                   CHuiDisplay* aAssocDisplay = 0);

    /**
     * Sets the interval before an idle notification is sent. Idling
     * begins when no input has been received after this delay. 
     * 
     * If you want to reset the idle time counter you have to actually 
     * generate a key event (for example by using TApaTask::SendKey(),
     * or calling HandleKeyEventL() directly).
     *
     * @todo Add a convenience method to reset the idle time counter.
     *
     * @note Scheduled events keep display refresh running even
     * when there's no input.
     *
     * @param aSeconds  Threshold in milliseconds. However the 
     * granularity is one second, so use thousands of ms as your 
     * input value.
     */
    IMPORT_C void SetIdleThreshold(TInt aMilliSeconds);

    /**
     * Broadcasts an event to all controls in the environment.
     *
     * @param aEvent  Event to broadcast.
     */
    IMPORT_C void BroadcastEventL(const THuiEvent& aEvent);

    /**
     * Returns the primary display, that is the display
     * that was created first.
     */
    IMPORT_C CHuiDisplay& PrimaryDisplay() const;
    
    /**
     * Returns the requested display.
     *
     * @param aIndex  Index of the display array.     
     */
    IMPORT_C CHuiDisplay& CHuiEnv::Display(TInt aIndex) const;

    /**
     * Notifies everyone of a change in the environment's skin. Called by the
     * skin instance when a change occurs in it, or when the environment
     * switches the skin instance. Exported so that application-implemented
     * custom skins can call this, too.
     */
    IMPORT_C void NotifySkinChangedL();    


    /* Static methods. */

    /**
     * Refresh callback, that is called by a refresh timer. Called periodically
     * when there might be something to update on the display. Has the
     * responsibility to update time for all HUI environments.
     *
     * @param aInstance  A CHuiEnv instance.
     * @see StartRefresh() to modify how often updates are being made.
     * @see StopRefresh() to stop updates.
     */
    IMPORT_C static TInt RefreshCallBack(TAny* aInstance);

	/**
     * Releases as many resources as possible. Among the released resources 
     * may be all objects in the toolkit's ownership, for example texture 
     * images and drawing surfaces. This should be called when the Hitchcock 
     * user interface is not needed for a while, for example when the application 
     * goes to the background. 
     *
     * At the moment the Environment releases all rendering surface and all displays,
     * but not the rendering plugin. All textures are left unreleased.
     * 
     * The resources need to be restored with a call to RestoreL() before
     * the Hitchcock user interface is used again.
     *
     * The AppUI can call this method to release resources. The AppUI can
     * overload HandleWsEventL() and react to KAknFullOrPartialForegroundLost
     * window server event by calling this method.
     *
     * @see RestoreL()
     */
    IMPORT_C void Release();
    
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
     * Set environment scheduling to adapt on device load.
     * Adaptive interval can only quarantee time for active objects inside 
     * the same thread, though the interval calculation balances the
     * effect of thread priority based schedulation and context switches as well 
     *
     * @param aPercent maximum time spent in RefreshCallBack() in percents. 
     */
    IMPORT_C void SetMaxCpuTime(TUint aPercent);


    /** Gets a pointer to a CHuiEnv which can be used to access CHuiEnv's functions.
     *  It allows application code to access CHuiEnv's functions even where there is
     *  no direct access to a ChuiEnv object.
     *
     *  For example:
     *
     *  CHuiEnv::Static()->SetMaxCpuTime(TUint aPercent);
     *
     *  @return  CHuiEnv * The pointer for accessing CHuiEnv's functions. or
     *           NULL if CHuiEnv has not been constructed yet. 
     *
     */
     static IMPORT_C CHuiEnv *Static();



     /**
     * Returns the theme manager.
     */
    IMPORT_C CHuiThemeManager& ThemeManager() const;

     /**
      * Returns the effects engine or NULL if effects are not supported
      * by the current rendering plugin.
      */
    IMPORT_C CHuiFxEngine* EffectsEngine() const;

     /**
     * RnD utility, shows FPS using User::InfoPrint using sample rate
     * given in milliseconds
     */
    IMPORT_C void SetFPSCounterThreshold(TUint aMilliseconds);

     /**
     * Allows application to balance rendering loop active object priority
     * compared to other active objects running within same scheduler
     *
     * Default value is CActive::EPriorityStandard
     */
    IMPORT_C void ChangeRefreshLoopPriority(CActive::TPriority aPriority);
    
    /**
     * Enables or disables debug features, by setting Rnd flag bits of KUIAccelTKEffectsComplexityFactor, 
     * of central repository key.
     * 
     * If 0x800 is set, Texturememory calculation is enabled. Calculates total memory used by texturemanager 
     * for all the textures it holds. 
     * Enabling this feature makes TextureManager to write info messages,
     * with total memory used by total number of textures and average bit depth of textures. For debugging purpose.
     * 
     * @param aRndFlag holds the Rnd flag. 
     */
    IMPORT_C void EnableDebugFlag( TUint aRndFlag );

    /** @endAPI */


    /* Methods. */
    
    /**
     * Sets the texture manager of this environment.
     *
     * @param aManager
     */
    void SetTextureManager(CHuiTextureManager& aManager);

    /**
     * Returns a reference to the scheduler of the environment.
     */
    CHuiScheduler& Scheduler();

    /**
     * Removes a display from the environment.
     *
     * @param aDisplay  The display to remove.
     */
    void RemoveDisplay(CHuiDisplay& aDisplay);

    /**
     * Update all displays by swapping an image drawn to a background display
     * buffer to the foreground (visible) screen buffer. This is the last stage
     * of display refresh.
     */
    void SwapBuffers(const RArray<TBool>& aDisplayRefreshed);

    /**
     * Called when input is received.
     *
     * @param aEvent  The received input event.
     */
    void NotifyInputReceivedL(const THuiEvent& aEvent);

    /**
     * Notify all displays of an idle event, ie. that
     * we are entering or leaving an idle state.
     *
     * @param aIdleBegins  ETrue if we are entering an idle state.
                           EFalse if we are leaving the idle state.
     */
    void SendIdleL(TBool aIdleBegins);

    /**
     * Notifies action observers of an action command.
     *
     * @param aCommand  Action command that is being executed.
     *
     * @return  System-wide error code from the first obserser that leaves.
     *          If no leaves occur, returns <code>KErrNone</code>.
     */
    TInt ReportAction(const THuiActionCommand& aCommand);

    /**
     * Returns the array of all displays in the environment.
     *
     * @return  Array of all displays in the environment.
     */
    RPointerArray<CHuiDisplay> Displays() const;

    IMPORT_C TInt& GlesRefCounter();

    void SetTimeFromLastUpdate(TUint aTimeFromLastUpdate);

    /**
     * Gets the active display.
     *
     * @return Current HuiDisplay.
     */
    CHuiDisplay* CurrentDisplay() const;

    CHuiCanvasTextureCache& CanvasTextureCache() const;
    IMPORT_C CAppIconCache& AppIconCache() const;

    /**
	 * Performs drawing synchronization.
	 * @internal
	 * @param aId identifier.
	 * @param aObserver observer to be informed of completion.
	 */
    IMPORT_C void Synchronize(TInt aId, MHuiSynchronizationObserver* aObserver);

     /**
     	 * Removes the controlgroup
	 * @param aId identifier.

     */	   	
     void RemoveTheControlGroup(TInt aId); 		
public:

    /* Implementaton of MHuiTextureLoadingCompletedObserver: */

    void TextureLoadingCompleted(CHuiTexture& aTexture,
                                    TInt aTextureId,
                                    TInt aErrorCode);

    /* Implementaton of MHuiTextureManagerStateChangedObserver: */

    void TextureManagerStateChanged(const CHuiTextureManager& aManager);


private:

	/* Enumerations. */
	
	/** 
	 * Environment state.
	 */
	enum TState
		{
		/** Normal state, where rendering is occuring. */
		ENormal,
		/** Released state, where displays are released and rendering is not occuring. */			
		EReleased
		};

    /* Constructors. */

    /**
     * Constructor.
     *
     * @param aObserver  Pointer to observer class instance that will be called
     *                   to perform application specific configurations. Can be
     *                   NULL to create a default CHuiEnv.
     */
    CHuiEnv(MHuiEnvObserver* aObserver = NULL);

    /**
     * Second-phase constructor.
     *
     * @param aRenderer The renderer being used by the environment.
     */
    void ConstructL(THuiRenderer aRenderer);


    /* Private methods. */

    /**
     * Create a resource reader.
     *
     * @param aReader      Resource reader.
     * @param aResourceId  Identifier of the resource to read.
     */
    void CreateResourceReaderLC(TResourceReader& aReader, TInt aResourceId) const;

    /**
     * Stops the refresh timer.
     * This does not affect animation timers.
     *
     * @see CHuiStatic
     */
    void StopRefresh();

    /**
     * Called when time has passed. The amount of elapsed time is passed
     * to the scheduler and the displays are checked for refresh.
     *
     * @param aElapsedTime  Number of seconds elapsed.
     */
    void AdvanceTime(TReal32 aElapsedTime);

    /**
     * Makes the render surface of the display active.
     *
     * @param aDisplay  HuiDisplay to make current.
     */
    void MakeCurrent(const CHuiDisplay& aDisplay) const;

    /**
     * Checks if device supports thread CPU time measurements.
     */
    TBool CPUTimeSupported();
    
    /**
     * Opens handle to null thread.
     */
    TBool OpenHandleToIdleCPUValueThread();
    
    /**
     * Closes handle to null thread.
     */
    void CloseHandleToIdleCPUValueThread();

public:

    /** Action observers. */
    RHuiObserverArray<MHuiActionObserver> iActionObservers;


private:

	/** Current environment state. */
	TState iState;

    /** Renderer instance. */
    CHuiRenderPlugin* iRenderer;

    /** Public identifier of the renderer selected at construction time. */
    THuiRenderer iRendererIdentifier;

    /** Current refresh mode. */
    THuiRefreshMode iRefreshMode;

    /** Refresh interval in milliseconds. */
    TInt iRefreshInterval;
    TInt iRefreshIntervalTarget;

    /** Instance of the static data area. */
    CHuiStatic* iStatic;

    /** Array of all displays in the environment. Owned. */
    RPointerArray<CHuiDisplay> iDisplays;

    /** Array of all overlaid displays in the environment. Points to the
        displays in iDisplays. */
    RPointerArray<CHuiDisplay> iOverlaidDisplays;

    /** The roster used by all overlaid displays. */
    CHuiRosterImpl* iSharedRoster;

    /** Refresh timer. */
    CHighResTimer* iPeriodic;
    //CPeriodic* iPeriodic;
    
    /** Number of refreshes that had no effect on anything. */
    TInt iIdleRefreshCount;

    /** The scheduler. */
    CHuiScheduler* iScheduler;

    /** All the loaded control groups are owned by the environment. */
    RPointerArray<CHuiControlGroup> iLoadedGroups;

    /** Texture manager. */
    CHuiTextureManager* iTextureManager;
    /** Theme Interface */
    CHuiThemeManager* iThemeManager;

    /** Font manager. */
    THuiFontManager* iFontManager;
    
    /** Text Style manager. */
    CHuiTextStyleManager* iTextStyleManager;

    /** Effects engine */
    CHuiFxEngine* iEffectsEngine;
    
    /** Skin. */
    CHuiSkin* iSkin;

    /** Visual factory. */
    CHuiVisualFactory* iVisualFactory;

    /** Environment observer object */
    MHuiEnvObserver* iEnvObserver;

    /** Time when input was last received. */
    TTime iLastInputTime;

    /** If ETrue, the input idle has been activated, so we are in idle state. */
    TBool iInputIdleIsActive;

    /** Number of seconds before sending an idle input notification. */
    TInt iIdleThreshold;

    TReal32 iPreviousFpsTime;
		
    TUint iMaxCPUUtilization;
    
    TInt iGlesRefCounter;

    // FPS counter data    
    TUint iFrames;
    TUint iMillisecondFromFPSUpdate;
    TUint iRefreshIntervalReal;
    TUint iFpsCounterThreshold;
    
    mutable CHuiDisplay* iCurrentDisplay;
    
    CActive::TPriority iRefreshLoopPriority;

	TBool iRefreshLoopActive;

    
    /** For measuring idle between frames */
    TBool    iIdleCPUValueMonitored;
    TInt64   iIdleCPUValuePreviousTime;
    TInt64   iIdleCPUValue;
    RThread  iIdleCPUValueThread;
    
    CHuiCanvasTextureCache* iCanvasTextureCache;    
    CAppIconCache* iAppIconCache;
    
    RPointerArray<MHuiLowMemoryObserver> iLowMemoryObservers;
    RPointerArray<MHuiMemoryLevelObserver> iMemoryLevelObservers;
    THuiMemoryLevel iMemoryLevel;
    
public:
    class MHuiSwapObserver
        {
        public: 
            virtual void PrepareSwap()=0;
            virtual void SwapComplete()=0;
            virtual void ReleaseWindowServer(TBool aRelease = ETrue) = 0;
            virtual void SetWgIdArray(TInt* aArray) =0; 
        };
    
    MHuiSwapObserver* iSwapObserver;
    
    IMPORT_C void AddLowMemoryObserver(MHuiLowMemoryObserver * aObserver);
    IMPORT_C void RemoveLowMemoryObserver(MHuiLowMemoryObserver * aObserver);
    IMPORT_C void NotifyLowMemory(TBool aEnable);

    IMPORT_C void AddMemoryLevelObserver(MHuiMemoryLevelObserver * aObserver);
    IMPORT_C void RemoveMemoryLevelObserver(MHuiMemoryLevelObserver * aObserver);
    IMPORT_C void NotifyMemoryLevel(THuiMemoryLevel aMemoryLevel);
    IMPORT_C THuiMemoryLevel MemoryLevel();
    
    void DoSynchronize();
    TBool iPauseDrawing;
private:
    RPointerArray<MHuiSynchronizationObserver> iSynchObservers;
    RArray<TInt> iSynchIds;
    };

#endif  // __HUIENV_H__
