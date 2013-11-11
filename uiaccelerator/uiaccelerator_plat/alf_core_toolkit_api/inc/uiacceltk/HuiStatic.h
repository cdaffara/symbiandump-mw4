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
* Description:   Defines CHuiStatic, static data access and utility functions 
*                such as timing.
*
*/



#ifndef __HUISTATIC_H__
#define __HUISTATIC_H__


#include <e32base.h>
#include <f32file.h>
#include <w32std.h>
#include <AknsUtils.h>
#include <bacntf.h>

/* Forward declarations. */
class CHuiEnv;
class CHuiRenderPlugin;
class CHuiBitgdiRenderPlugin;
class CHuiVg10RenderPlugin;
class CHuiGles10RenderPlugin;
class CHuiGles11RenderPlugin;
class CHuiGles20RenderPlugin;
class MHuiRenderSurface;
class MHuiProbe;
class MAknsSkinInstance;

/**
 * Miscellaneous globally used functions, and thread local storage space.
 * 
 * CHuiStatic contains all the functions that tend to be used across classes.
 * For example, global data access methods for timing functions, frame count,
 * time since start, etc. It also contains globally-used functions
 * such as printf. These functions are written in a non-OO fashion because
 * they are used everywhere, so passing data such as delta step down through the
 * entire class heirarchy is inefficient.
 */
NONSHARABLE_CLASS(CHuiStatic) : public CBase
    {
public:

    /* Constructors and destructor. */

    /**
     * Constructor. The user application should not explicitly
     * construct a CHuiStatic - it is constructed by CHuiEnv.
     *
     * @param aPrimaryEnv The toolkit environment. 
     */
    static CHuiStatic* NewLC(CHuiEnv* aPrimaryEnv = 0);

    /**
     * Constructor. The user application should not explicitly
     * construct a CHuiStatic - it is constructed by CHuiEnv.
     *
     * @param aPrimaryEnv The toolkit environment. 
     */
    static CHuiStatic* NewL(CHuiEnv* aPrimaryEnv = 0);


    /**
     * Destructor.
     */
    virtual ~CHuiStatic();


    /* Methods. */

    /**
     * Returns the current renderer.
     * @panic Renderer not yet set. @see SetRenderer()
     */
    IMPORT_C static CHuiRenderPlugin& Renderer();

    /**
     * Sets the current renderer.
     * @param aRenderer to be used with this HuiToolkit instance.
     */
    static void SetRenderer(CHuiRenderPlugin& aRenderer);

    /**
     * Returns a BITGDI renderer.
     * To create a new renderer of this type, a function pointer
     * to this method should be passed to CHuiEnv::NewL() when
     * the environment is first constructed.
     *
     * @see CHuiEnv::NewL()
     * @panic  BITGDI is not selected as the current renderer.
     */
    IMPORT_C static CHuiBitgdiRenderPlugin& BitgdiRenderer();

    /**
     * Returns a OpenVG 1.0 renderer.
     * To create a new renderer of this type, a function pointer
     * to this method should be passed to CHuiEnv::NewL() when
     * the environment is first constructed.
     *
     * @see CHuiEnv::NewL()
     *
     * @panic  OpenVG 1.0 is not selected as the current renderer.
     */
    IMPORT_C static CHuiVg10RenderPlugin& Vg10Renderer();

    /**
     * Returns a OpenGL ES 1.0 renderer.
     * To create a new renderer of this type, a function pointer
     * to this method should be passed to CHuiEnv::NewL() when
     * the environment is first constructed.
     *
     * @see CHuiEnv::NewL()
     *
     * @panic  OpenGL ES 1.0 is not selected as the current renderer.
     */
    IMPORT_C static CHuiGles10RenderPlugin& Gles10Renderer();

    /**
     * Returns a OpenGL ES 1.1 renderer.
     * To create a new renderer of this type, a function pointer
     * to this method should be passed to CHuiEnv::NewL() when
     * the environment is first constructed.
     *
     * @see CHuiEnv::NewL()
     *
     * @panic  OpenGL ES 1.1 is not selected as the current renderer.
     */
    IMPORT_C static CHuiGles11RenderPlugin& Gles11Renderer();

    /**
     * Returns a OpenGL ES 2.0 renderer.
     * To create a new renderer of this type, a function pointer
     * to this method should be passed to CHuiEnv::NewL() when
     * the environment is first constructed.
     *
     * @see CHuiEnv::NewL()
     *
     * @panic  OpenGL ES 2.0 is not selected as the current renderer.
     */
    IMPORT_C static CHuiGles20RenderPlugin& Gles20Renderer();

    /**
     * Returns the current rendering surface. Usually changes via
     * MHuiRenderSurface::MakeCurrent(). Current render surface is NULL before
     * it is set with the first MakeCurrent call.
     *
     * @return  Rendering surface.
     */
    IMPORT_C static MHuiRenderSurface* CurrentRenderSurface();

    /**
     * Sets the current rendering surface. Called from
     * MHuiRenderSurface::MakeCurrent().
     *
     * @param aSurface  Rendering surface.
     */
    IMPORT_C static void SetCurrentRenderSurface(MHuiRenderSurface* aSurface);

    /**
     * Generates a new identifier number. All the numbers returned by this
     * method are negative integers.
     * Used for assigning a unique ID to all new controls, among other purposes.
     *
     * @return a negative ID number, unique to all ID's generated by this method.
     */
    static TInt GenerateId();

    /**
     * Determines the primary CHuiEnv instance.
     */
    IMPORT_C static CHuiEnv& Env();

    /**
     * Called internally when a change occurs that requires active display
     * refreshing. All environments are notified. If only a specific
     * environment should be notified, its ContinueRefresh() method should
     * be called directly.
     */
    IMPORT_C static void ContinueRefresh();

    /**
     * Updates the toolkit's time counters. This includes the toolkit's
     * realtime clock, the internal absolute clock (which is affected by the
     * time factor), the amount of elapsed time since last UpdateTime()
     * invocation, and the amount of elapsed time since the first UpdateTime()
     * invocation (which was done in the beginning of the first refresh).
     */
    static void UpdateTime();
    static TUint32 MilliSecondsSinceUpdateTime();
    /**
     * Returns the internal absolute clock. This is affected by the time
     * factor.
     *
     * @see  TTime in Symbian OS Reference.
     *
     * @return  Internal absolute clock.
     */
    IMPORT_C static const TTime& Time();

    /**
     * Sets the time factor that affects the internal absolute clock, which
     * is returned by Time(). Zero stops the progress of time completely,
     * 0.5 means that time  progresses in half the speed compared to real
     * time (1.0).
     *
     * @param aTimeFactor  Time factor.
     */
    IMPORT_C static void SetTimeFactor(TReal32 aTimeFactor) __SOFTFP;

    /**
     * Determines the current time factor.
     *
     * @return  Time factor.
     */
    IMPORT_C static TReal32 TimeFactor() __SOFTFP;

    /**
     * Pauses the progress of time completely. The internal absolute clock does
     * not advance when time is paused. Does not affect the time factor.
     */
    IMPORT_C static void PauseTime();

    /**
     * Continues the progress of time with the same time factor as previously.
     * Starts the internal absolute clock at the same point in time when it was
     * paused.
     */
    IMPORT_C static void ContinueTime();

    /**
     * Determines if the time is paused.
     *
     * @return  <code>ETrue</code>, if time is paused.
     */
    IMPORT_C static TBool TimePaused();

    /**
     * Determines the number of seconds elapsed in the internal absolute clock
     * since the last UpdateTime(). This is affected by the time factor.
     *
     * @return  Seconds.
     */
    IMPORT_C static TReal32 ElapsedSeconds() __SOFTFP;

    /**
     * Determines the number of seconds since the first UpdateTime() invocation.
     * This is affected by the time factor.
     *
     * @return  Seconds since the first UpdateTime() invocation.
     */
    IMPORT_C static TReal32 SecondsSinceStart() __SOFTFP;

    /**
     * Determines the number of milliseconds since the first UpdateTime()
     * invocation. This is the same value as returned by SecondsSinceStart(),
     * only converted into milliseconds. This is affected by the time factor.
     *
     * Note that the value will wrap around back to zero in approximately
     * 50 days after the static instance has been constructed. If this is
     * a problem, you should handle the wraparound or use SecondsSinceStart()
     * instead.
     *
     * @return  Milliseconds. Note the 32-bit range.
     */
    IMPORT_C static TUint32 MilliSecondsSinceStart();

    /**
     * Determines the number of frames shown.
     *
     * @return  Frame count since the application first began rendering.
     */
    IMPORT_C static TUint FrameCount();

    /**
     * Determines the current average frame rate.
     *
     * @return  Frame rate since the start of execution.
     */
    IMPORT_C static TReal32 AverageFrameRate() __SOFTFP;

    /**
     * Determines the current frame rate. The rate is calculated at most
     * once per second. Calling this less frequently causes the rate to be
     * calculated as an average since the last time this was called.
     *
     * @param aTrianglesPerSecond
     * @param aDrawElementsPerSecond
     *
     * @return  Frames per second.
     * 
     */
    IMPORT_C static TReal32 FrameRate() __SOFTFP;

    /**
     * Print debug text to the console (RDebug). Will also write
     * debug output to a log file e:\HuiMessages.log (memory card)
     * if EnableLogging() has been called. If there is no memory card
     * available the log file will be written to c:\HuiMessages.log.
     *
     * @note Debugging to the log file is enabled by default for ARM debug builds.
     * So you don't have to explicitly enable log file debugging in that case.
     *
     * @see EnableLogging()
     * @note Uses 8-bit character strings, which may not be compatible
     * with RDebug output. Printing to log file will work fine, but
     * please use all purpose 16-bit variant if all possible.
     */
    IMPORT_C static void Printf(TRefByValue<const TDesC8> aFormat, ...);

    /**
     * Print debug text to the console (RDebug). Will also write
     * debug output to a log file e:\HuiMessages.log (memory card)
     * if EnableLogging() has been called. If there is no memory card
     * available the log file will be written to c:\HuiMessages.log.
     *
     * @note Debugging to the log file is enabled by default for ARM debug builds.
     * So you don't have to explicitly enable log file debugging in that case.
     * @see EnableLogging()
     * @param aFormat printf-style string and its parameters. The
     * formatted result must not exceed 256 characters (incl the terminating
     * null char "\0").
     */
    IMPORT_C static void Printf(TRefByValue<const TDesC16> aFormat, ...);

    /**
     * Increment the frame counter.
     */
    static void ReportNewFrame();

    /**
     * Enables or disables Printf message logging to a file.
     *
     * The log file is e:\HuiMessages.log (memory card), or if there is
     * no memory card available the log file will be written to
     * c:\HuiMessages.log.
     *
     * By default the log file logging is disabled for all other builds
     * than ARM debug builds. Note that you also have to have the macro
     * HUI_DEBUG_WITH_PRINTF enabled to make HUI_DEBUG*() macros use
     * CHuiStatic::Printf() calls.
     *
     * @param aEnable Set to true to enable logging, false to disable.
     *
     * @see Printf()
     */
    IMPORT_C static void EnableLogging(TBool aEnable = ETrue);

    /**
     * Determines whether logging to file is enabled. Note that you may
     * have RDebug logging even if this returns EFalse.
     *
     * @see EnableLogging() to enable/disable log file logging.
     * @return  <code>ETrue</code>, if logging to file is enabled.
     */
    IMPORT_C static TBool Logging();

    /**
     * Start timing with a clock. Specifies timing starting point.
     * Provides simple excution profiling capabilities.
     * @param aClock Clock number to start timing with, use values 0-9.
     * @see Toc()
     */
    IMPORT_C static void Tic(TInt aClock);

    /**
     * Determines seconds passed since the clock was started (with Tic()).
     * Provides simple execution profiling capabilities.
     * @param aClock Clock number used for profiling, use values 0-9.
     * @see Tic()
     */
    IMPORT_C static TReal32 Toc(TInt aClock) __SOFTFP;

    /**
     * Returns the file server session to be used by the application.
     * In most cases, this is the same as CEikonEnv::Static()->FsSession().
     */
    IMPORT_C static RFs& FsSession();

    /**
     * Returns the window server session to be used by the application.
     * In most cases, this is the same as CEikonEnv::Static()->WsSession().
     */
    IMPORT_C static RWsSession& WsSession();

    /**
     * Returns the screen device used by the application.
     * In most cases, this is the same as CEikonEnv::Static()->ScreenDevice().
     */
    IMPORT_C static CWsScreenDevice* ScreenDevice(TInt aScreenNumber = 0);

    /**
     * Returns the screen device used by the application.
     * In most cases, this is the same as CEikonEnv::Static()->ScreenDevice().
     */
    IMPORT_C static RWindowGroup* RootWin(TInt aScreenNumber = 0);



    /**
     * Sets the global transition time used for doing layout transitions. All
     * layout calculations that happen after the call to SetLayoutTransitionTime()
     * are affected by the new time. A layout calculations occurs immediately
     * when the dimensions of a layout visual are modified. By default, the
     * transition time is ero for all layout calculations, which means they will
     * become effective immediately.
     *
     * @param aTime  New global layout transition time in milliseconds.
     * @deprecated Will be removed after 2007 wk13.
     *             This API is ugly. This method should be removed altogether.
     *             Set transition time as a parameter instead.     
     */
    IMPORT_C static void SetLayoutTransitionTime(TInt aTime);

    /**
     * Returns the global layout transition time.
     *
     * @return  Layout transition time in milliseconds.
     * @deprecated Will be removed after 2007 wk13.
     *             This API is ugly. This method should be removed altogether.
     *             Set transition time as a parameter instead.     
     */
    IMPORT_C static TInt LayoutTransitionTime();

    /**
     * Sets the Hitchcock UI Probe object.
     *
     * @param aProbe The probe object
     */
    IMPORT_C static void SetProbe(MHuiProbe* aProbe);
    
    /**
     * Returns the Probe object
     *
     * @return Probe object
     */
    static MHuiProbe& Probe();
    
    IMPORT_C static MAknsSkinInstance* SkinInstance();

    // Internal utils 
    static TBool LayoutMirrored();
    static TInt CbaLocation();
    static void LayoutMetricsRect(TInt aType, TRect& aRect);
    static TInt GetCachedColor(TRgb& aRgb, const TAknsItemID& aID,
            const TInt aIndex );
    
    static TBool ConvertToVisualAndClipL(
        TDes& aLogicalText,
        const CFont& aFont,
        TInt aMaxWidthInPixels,
        TInt aMaxClippedWidthInPixels );
    
    static HBufC* ConvertToVisualAndWrapToArrayL( 
        const TDesC& aLogicalText,
        TInt aLineWidth,
        const CFont& aFont,
        CArrayFix<TPtrC>& aWrappedArray);
    
    static CFbsBitmap* GetBgBitmapLC(const TAknsItemID& aID);
    static void GetMaskedBitmapL(const TAknsItemID& aID, CFbsBitmap*& aBitmap, CFbsBitmap*& aBitmapMask,
                                 const TDesC& aFileName, TInt aBitmapId, TInt aMaskId, const TSize& aSize, TScaleMode aScaleMode);    
    static CAknsItemData* GetCachedSkinItemDataL(const TAknsItemID& aID, TInt aDataType);    
    
protected:

    /* Constructors. */

    /**
     * Constructor. Should not be called through the API.
     */
    CHuiStatic();

    /**
     * Second-phase constructor.
     * @param aPrimaryEnv Pass in the environment.
     */
    void ConstructL(CHuiEnv* aPrimaryEnv);


private:

    /* Private methods */

    /**
     * Returns a pointer to the thread-local data struct.
     */
    IMPORT_C static struct TTlsData* Data();

    /**
     * Updates the toolkit's time counters. This includes the toolkit's
     * realtime clock, the internal absolute clock (which is affected by the
     * time factor), the amount of elapsed time since last UpdateTime()
     * invocation, and the amount of elapsed time since the first UpdateTime()
     * invocation (which was done in the beginning of the first refresh).
     * 
     * @param aData TLS Data which encapsulates the internal time variables.
     */
    static void UpdateTime(struct TTlsData* aData);

    /**
     * Callback function.
     * On system time change, Locale change, HuiStatic time member variables need to be reset. 
     * CEnvironmentChangeNotifier gives notifications about the above mentioned changes.
     * Huistatic registers itself for these notifications and SettingChangedCallBack() 
     * is the callback function getting executed on these events.     
     *     
     */
	static TInt SettingChangedCallBack(TAny* aInstance);
	
    TInt DoChange();
    
private:

    /**
     * Thread local storage for this DLL, as we cannot declare static
     * global variables in Symbian.
     */
    struct TTlsData* iData;

    //For system time change event
    CEnvironmentChangeNotifier* iChangeNotifier;
    };


#endif  // __HUISTATIC_H__
