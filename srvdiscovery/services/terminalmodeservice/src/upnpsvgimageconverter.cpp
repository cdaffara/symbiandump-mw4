/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpSvgImageConverter class implementation.
*
*/

#include "upnpsvgimageconverter.h"
#include "upnptmserverimpl.h"
#include <f32file.h> 
#include "upnpiconconversionactive.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnpsvgimageconverterTraces.h"
#endif

// Literals
_LIT(KLatin,               "LatinPlain12");
_LIT(KConvertedIconPath,   "public\\TmServerDevice1\\");
_LIT(KCross,               "x");
_LIT(KDotBmp,              ".bmp");
_LIT(KThreadName,          "ImageConverterThread");
_LIT(KSemaphoreName,       "ImageConverterSemaphore");

// Constant
const TUint KDot           = '.';

// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpSvgImageConverter::NewL
// Two-phased constructor.
// @param aIconWidth   Desired width of the serving icon.
// @param aIconHeight  Desired height of the serving icon.
// ---------------------------------------------------------------------------------
//
CUpnpSvgImageConverter* CUpnpSvgImageConverter::NewL( TInt aIconWidth, TInt aIconHeight )
    {
    OstTraceFunctionEntry0( CUPNPSVGIMAGECONVERTER_NEWL_ENTRY );
    OstTraceExt2( TRACE_NORMAL, CUPNPSVGIMAGECONVERTER_NEWL, "CUpnpSvgImageConverter::NewL;aIconWidth=%d;aIconHeight=%d", aIconWidth, aIconHeight );
    CUpnpSvgImageConverter* self = new (ELeave) CUpnpSvgImageConverter( );
    CleanupStack::PushL(self);
    self->ConstructL( aIconWidth,aIconHeight );
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CUPNPSVGIMAGECONVERTER_NEWL_EXIT );
    return self;    
    }

// ---------------------------------------------------------------------------------
// CUpnpSvgImageConverter::CUpnpSvgImageConverter
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------------
//
CUpnpSvgImageConverter::CUpnpSvgImageConverter( )                                      
    {

    }

// ---------------------------------------------------------------------------------
// CUpnpSvgImageConverter::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------------
//
void CUpnpSvgImageConverter::ConstructL(  TInt aIconWidth, TInt aIconHeight )
    {
    OstTraceFunctionEntry0( CUPNPSVGIMAGECONVERTER_CONSTRUCTL_ENTRY );
    //Geometric rectangle.
    TRect lMainRect( 0,0,aIconWidth,aIconHeight );
    /**
     * Specifies the font specification to use for text drawing.
     * Here "LatinPlain12" of height "10" is used.
     */ 
    TFontSpec lFontSpec( KLatin,UpnpString::KMaxTUintLength );
    // A bitmap managed by the font and bitmap server.
    User::LeaveIfError(iFbsSession.Connect());
    iBitmap = new ( ELeave ) CFbsBitmap();
    iMask = new ( ELeave ) CFbsBitmap();
    //Creates bitmap file of the desired dimensions with desired display mode(64,000 colour)
    User::LeaveIfError( iBitmap->Create( TSize( lMainRect.Width(),
                        lMainRect.Height() ),EColor64K ) );
    User::LeaveIfError( iMask->Create( TSize( lMainRect.Width(),
                            lMainRect.Height() ),EGray256 ) );  // 256 grayscales display mode as alpha values
    /**
     * Create a new Svg Engine interface.
     * @param iBitmap  bitmap to draw resulting svg image.
     * @param aReqObserver(NULL) interface for callbacks to retrieve info
     *                          only client can provide, such as opening files.
     * @param lFontSpecFont spec to use for text drawing.
     */
    iSvgModule = CSvgEngineInterfaceImpl::NewL( iBitmap,NULL,lFontSpec );
    User::LeaveIfError( iFileSession.Connect() );  
    TBuf<UpnpString::KMaxFilenameLength> privatePath;  
    User::LeaveIfError( iFileSession.PrivatePath(privatePath) );

    privatePath.Append(KConvertedIconPath());
    privatePath.AppendNum( aIconWidth );
    privatePath.Append( KCross);
    privatePath.AppendNum( aIconHeight );
    privatePath.Append( UpnpString::KDoubleBackSlash16() );
    TInt err = iFileSession.MkDir(privatePath);
    if( ( err != KErrNone ) && ( err != KErrAlreadyExists ) )
        {
        // Leaves if fails to create a directory
        User::Leave(err);
        }
    iBitMapFilePath.CreateL( UpnpString::KMaxFilenameLength );
    iBitMapFilePath.Append( privatePath );
    OstTraceFunctionExit0( CUPNPSVGIMAGECONVERTER_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpSvgImageConverter::~CUpnpSvgImageConverter
// Destructor
// ---------------------------------------------------------------------------------
// 
CUpnpSvgImageConverter::~CUpnpSvgImageConverter()
    {  
    OstTraceFunctionEntry0( CUPNPSVGIMAGECONVERTER_CUPNPSVGIMAGECONVERTER_ENTRY );
    iBitMapFilePath.Close();
    delete iMask;
    delete iBitmap;
    delete iSvgModule;        
    iFbsSession.Disconnect();
    iFileSession.Close();
    OstTraceFunctionExit0( CUPNPSVGIMAGECONVERTER_CUPNPSVGIMAGECONVERTER_EXIT );
    }

// -----------------------------------------------------------------------------------
// CUpnpSvgImageConverter::ConvertToBitmapL
// Method invokes svg to bitmap conversion.
// It takes svg file as an input and returns converted icon file( bmp ).
// Also prepares DOM for given SVG file.
// @param aSvgFile Input svg filepath
// @param aBitmapFile[out] Bitmap filepath ( converted file ).
// -----------------------------------------------------------------------------------
void CUpnpSvgImageConverter::ConvertToBitmapL( const TDesC& aSvgFile, RBuf& aBitmapFile )
    { 
    OstTraceFunctionEntry0( CUPNPSVGIMAGECONVERTER_CONVERTTOBITMAPL_ENTRY );
    /**
     * Extracts the filename( without filename extension ) from the input svg filepath and 
     * uses the same filename to create bitmap file ( eg: abc.bmp )
     */ 
    TPtrC iconFileNameWithExtension = aSvgFile.Mid((aSvgFile.LocateReverse(KDirectorySeparator))+1);
    iBitMapFilePath.Append( iconFileNameWithExtension.Left(iconFileNameWithExtension.LocateReverse(KDot)) );
    iBitMapFilePath.Append( KDotBmp );
    RFile iconFile;
    TInt err = iconFile.Create(iFileSession,iBitMapFilePath,EFileRead|EFileWrite|EFileShareAny);
    OstTrace1( TRACE_NORMAL, CUPNPSVGIMAGECONVERTER_CONVERTTOBITMAPL, "CUpnpSvgImageConverter::ConvertToBitmapL;err=%d", err );
    iconFile.Close();
    aBitmapFile.Close();
    if ( err == KErrAlreadyExists )
        {
        //If the converted file pre-exists, just return the existing filepath.
        aBitmapFile.CreateL(iBitMapFilePath);
        }
    else if ( err == KErrNone )
        {
        TInt domHandle;
        /**
         * Parses and Prepares DOM for given SVG file.
         * @param aSvgFile the name of the file  to be parsed
         * @param domHandle  Handle to the created DOM.
         * @return lsvgerr Error object specifying  the error occured during operation
         */
        MSvgError* lsvgerr = iSvgModule->PrepareDom( aSvgFile,domHandle ,NULL );
        if ( (! lsvgerr ) || ( lsvgerr && lsvgerr->HasError() &&!lsvgerr->IsWarning() ) )
            {    
            User::Leave( KErrCancel );
            }   
        /**
         *Initialization of the engine 
         *@param domHandle Handle to DOM Tree.
         *@param iBitmap Buffer for drawing the DOM Tree.
         *@param iMask  Buffer for mask (alpha values) of framebuffer result
         *@return lsvgerr Error object specifying the error occured.
         */
        lsvgerr = iSvgModule->UseDom( domHandle ,iBitmap,iMask );
        if ( (! lsvgerr ) || ( lsvgerr && lsvgerr->HasError() &&!lsvgerr->IsWarning() ) )
            {      
            User::Leave( KErrCancel );
            }
        /**
         * Request the SVG Engine to begin an animation. 
         */
        iSvgModule->Start( lsvgerr );
        if ( (! lsvgerr ) || ( lsvgerr && lsvgerr->HasError() &&!lsvgerr->IsWarning() ) )
            {      
            User::Leave( KErrCancel );
            }
        /**
          * Deletes the DOM tree associated with the Handle.
          */
        lsvgerr = iSvgModule->DeleteDom( domHandle ) ;
        if ( (! lsvgerr ) || ( lsvgerr && lsvgerr->HasError() &&!lsvgerr->IsWarning() ) )
            {      
            User::Leave( KErrCancel );
            }
        /** 
         * Creates and starts a new thread to handle asynchronous icon conversion.
         * A new or separate thread is needed for this purpose since the conversion happens 
         * in the same thread as of that of the one who invokes.
         */
        StartThreadL();
        // Sets the converted filepath
        aBitmapFile.CreateL(iBitMapFilePath);
        }
    else
        {
        User::Leave(err);
        }
    OstTraceFunctionExit0( CUPNPSVGIMAGECONVERTER_CONVERTTOBITMAPL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpSvgImageConverter::StartThreadL
// Creates a new thread where actual conversion occurs.
// Also creates a sempahore and waits on it.
// ---------------------------------------------------------------------------------
// 
void CUpnpSvgImageConverter::StartThreadL( )
    {   
    OstTraceFunctionEntry0( CUPNPSVGIMAGECONVERTER_STARTTHREADL_ENTRY );
    // Creates and opens a semaphore
    RSemaphore semaphore;
    CleanupClosePushL(semaphore);
    TInt error = semaphore.CreateGlobal( KSemaphoreName, KErrNone );
    if ( error == KErrAlreadyExists )
       {
       User::LeaveIfError(semaphore.OpenGlobal( KSemaphoreName ));
       }
    else
       {
       User::LeaveIfError(error);
       }   
    // Creates a new thread which will be in suspended state after creation
    RThread thread;
    CleanupClosePushL(thread);
    User::LeaveIfError( thread.Create( KThreadName,
                                       ImageConverter,
                                       KDefaultStackSize,
                                       NULL,
                                       this,
                                       EOwnerProcess));
    thread.SetPriority(EPriorityRealTime);
    //Resumes the newly created thread
    thread.Resume();
    semaphore.Wait(); 
    CleanupStack::PopAndDestroy(2,&semaphore);
    OstTraceFunctionExit0( CUPNPSVGIMAGECONVERTER_STARTTHREADL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpSvgImageConverter::Filepath
// Method to return the path of the icon file which needs to be converted.
// @return Returns reference to Bitmap file path.
// ---------------------------------------------------------------------------------
// 
const TDesC& CUpnpSvgImageConverter::Filepath( )const
    {
    OstTraceFunctionEntry0( CUPNPSVGIMAGECONVERTER_FILEPATH_ENTRY );
    OstTraceFunctionExit0( CUPNPSVGIMAGECONVERTER_FILEPATH_EXIT );
    return iBitMapFilePath; 
    }

// ---------------------------------------------------------------------------------
// CUpnpSvgImageConverter::BitmapObject
// Method to return the bitmap object.
// @return Returns reference to Bitmap object.
// ---------------------------------------------------------------------------------
// 
CFbsBitmap& CUpnpSvgImageConverter::BitmapObject()const 
    {
    OstTraceFunctionEntry0( CUPNPSVGIMAGECONVERTER_BITMAPOBJECT_ENTRY );
    OstTraceFunctionExit0( CUPNPSVGIMAGECONVERTER_BITMAPOBJECT_EXIT );
    return *iBitmap;
    }

// ---------------------------------------------------------------------------------
// CUpnpSvgImageConverter::ImageConverter
// Static method serving as thread's main function
// @param aParam  Pointer to any type object.
// @return Returns error code
// ---------------------------------------------------------------------------------
// 
TInt CUpnpSvgImageConverter::ImageConverter( TAny* aParam )
    {
    OstTraceFunctionEntry0( CUPNPSVGIMAGECONVERTER_IMAGECONVERTER_ENTRY );
    TInt err( KErrNone );
    CUpnpSvgImageConverter* svgConverter = static_cast <CUpnpSvgImageConverter*>( aParam );
    // Create cleanupstack
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( !cleanupStack )
        {
        err = KErrNoMemory;
        }
    else
        {
        TRAP( err, ImageConverterL(*svgConverter) );
        }
    // Create a matching semaphore for signalling the waiting semaphore
    RSemaphore semaphoreStop;
    semaphoreStop.OpenGlobal(KSemaphoreName);
    semaphoreStop.Signal();
    semaphoreStop.Close();
    
    delete cleanupStack;
    OstTraceFunctionExit0( CUPNPSVGIMAGECONVERTER_IMAGECONVERTER_EXIT );
    return err;
    }

// ---------------------------------------------------------------------------------
// CUpnpSvgImageConverter::ImageConverterL
// Called from thread's main function.
// Contains the code which can leave.
// @param aSvgConverter  Reference to class object.
// ---------------------------------------------------------------------------------
// 
void CUpnpSvgImageConverter::ImageConverterL( CUpnpSvgImageConverter& aSvgConverter )
    {
    OstTraceFunctionEntry0( CUPNPSVGIMAGECONVERTER_IMAGECONVERTERL_ENTRY );
    /** 
     * Add support for active objects
     * Create and install active scheduler
     */
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install( scheduler );
    // Opens the bitmap file and supplies it to the active object
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError(fs.Connect());
    RFile bitmapFile;
    CleanupClosePushL( bitmapFile );
    User::LeaveIfError( bitmapFile.Open( fs, aSvgConverter.Filepath(), EFileRead|EFileWrite|EFileShareAny ));
  
    // Create an active object that is executed in this thread
    CUpnpIconConversionActive* active = CUpnpIconConversionActive::NewL( bitmapFile );
    CleanupStack::PushL(active); 
    // Invoke the request to the active object
    active->Convert( aSvgConverter.BitmapObject() );
    // Thread execution starts
    CActiveScheduler::Start();
    User::LeaveIfError( active->FetchError() );  // Leaves if the status returned is not KErrNone
    
    /**
     * Thread execution ends (waiting for CActiveScheduler::Stop())
     * Cleanup the active object and scheduler
     */ 
    CleanupStack::PopAndDestroy(UpnpString::KDoubleLineFeedLength,scheduler);
    OstTraceFunctionExit0( CUPNPSVGIMAGECONVERTER_IMAGECONVERTERL_EXIT );
    }

// End of File
