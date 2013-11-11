/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Icon convert to convert icon for png to mbm format
*
*/


#ifndef __ICONCONVERTER_H__
#define __ICONCONVERTER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATION
class CFbsBitmap;
class CBitmapScaler;
class CImageDecoder;

//namespace SwiUI
//{
class MConverterController;
/*
* Inherited CActive, performs a asynchronous conversion operation
*
* @lib WidgetInstallerUI.lib
* @since 3.1
*/
class CIconConverter : public CActive
    {
    // states for this engine
    enum TState
        {
        EIdle = 0,
        EDecoding,
        EConvertingFile,
        EScalingIcon,
        EScalingMask,
        EStoringIcon,
        EFinalize
        };

    public:

        public: // contructors/destructors

        /*
        * NewL
        *
        * Create a CIconConverter object and return a pointer to it.
        *
        * Params:
        *      aController Pointer to a MConverterController interface.
        *      The engine uses NotifyCompletionL callback from this interface
        *      to notify the controller about completions of coding or
        *      encoding requests.
        *
        * Returns:
        *       A pointer to the created engine
        *
        */
        static CIconConverter* NewL( MConverterController* aController , RFs& aFs );

        /*
        * ~CIconConverter
        *
        * Destructor
        *
        */
        ~CIconConverter();

    public: // interface methods

        /*
        * StartToDecodeL
        *
        * Starts to decode an image from a file. When completed calls
        * NotifyCompletionL, from iController.
        *
        * @Params:
        *       aFileName Full path and filename of the image to be decoded.
        *
        * @Returns:
        *       Nothing
        */
        void StartToDecodeL( const TDesC& aInputFileName, const TDesC& aOutputFileName );


    protected: // implementation of CActive

        /*
        * From CActive
        *
        * Handle the cancel operation
        *
        */
        void DoCancel();

        /*
        * From CActive
        *
        * Handle the conversion process
        *
        */
        void RunL();

        /*
        * From CActive
        * Handle the error situations
        */
        TInt RunError( TInt aError );

    private: // internal methods, constructors

        /*
        * CIconConverter
        *
        * C++ Constructor
        *
        */
        CIconConverter( MConverterController* aController, RFs& aFs );

        /*
        * ConstructL
        *
        * 2-phase constructor
        *
        * @Returns:
        *       Nothing
        */
        void ConstructL();

        /*
        * GetTempIconName
        *
        * Get temporary icon name
        *
        * @Returns:
        *       Nothing
        */
        void GetTempIconName( TInt aIndex, TFileName& aIconName );

        /*
        * DoCreateFinalIconL
        *
        * Create the final icon
        *
        * @Returns:
        *       Nothing
        */
        void DoCreateFinalIconL();

        /*
        * DoIconStoreL
        *
        * Store icon and mask files
        *
        * @Returns:
        *       Nothing
        */
        void DoIconStoreL();

        /*
        * ScalerL
        *
        * Create bitmap scalar
        *
        * @Returns:
        *       CBitmapScaler
        */
        CBitmapScaler& ScalerL();

        /*
        * DoScalingL
        *
        * Scale
        *
        * @Returns:
        *       Nothing
        */
        void DoScalingL( CFbsBitmap& aBitmapSource, CFbsBitmap& aBitmapTarget );

        /*
        * DoMaskScalingL
        *
        * Scale the mask
        *
        * @Returns:
        *       Nothing
        */
        void DoMaskScalingL();

        /*
        * DoIconScalingL
        *
        * Scale the bitmap
        *
        * @Returns:
        *       Nothing
        */
        void DoIconScalingL();

        /*
        * DoProcessMaskL
        *
        * Process the mask for the bitmap
        *
        * @Returns:
        *       Nothing
        */
        void DoProcessMaskL();

    private: // internal data

        // The client's pending status
        TRequestStatus*         iClientStatus;
        MConverterController*   iController; // ui controller, owned
        RFs                     iFs; // for opening/saving images from/to files, not owned
        CImageDecoder*          iImageDecoder; // decoder from ICL API, owned
        HBufC*                  iOutputFileName; // the resulted file name, owned
        TState                  iState; // state of the conversion
        CArrayFixFlat<TSize>*   iIconSizes;
        // Original bitmap pointers garnered from the PNG in the midlet
        CFbsBitmap*             iOriginalBitmap;
        CFbsBitmap*             iOriginalBitmapMask;
        // Scaled target bitmaps
        CFbsBitmap*             iTempBitmap;
        CFbsBitmap*             iTempBitmapMask;
        // Bitmap scaler
        CBitmapScaler*          iScaler;
        // The current size icon being converted
        TInt                    iCurrentSizeIndex;
        // Icon file
        RFile                   iIconFile;
        RFile                   iIconPngFile;
        // Path to create Temp icons
        const HBufC*            iTempPath;

    };
//}
#endif // #ifndef __ICONCONVERTER_H__
