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
*      Declaration of view images popup dialog.
*
*
*/


#ifndef __VIEWIMAGESLISTBOX_H
#define __VIEWIMAGESLISTBOX_H

// INCLUDES
#include <e32base.h>
#include <e32def.h>
#include <aknPopup.h>
#include <aknlists.h>
#include <mdaimageconverter.h>

// Browser as a Plugin includes
#include <BrCtlDialogsProvider.h>

// FORWARD DECLARATIONS
class CCoeControl;
class TCoeEvent;
class CEikListBox;
class TListBoxEvent;
class CAknsListBoxBackgroundControlContext;
class TBrCtlImageCarrier;
class CPhotoOperationWait;

// use large icon list box with double item text
class CBrowserViewImagesListBox :
                        public CAknDoubleLargeGraphicPopupMenuStyleListBox, 
                        public MMdaImageUtilObserver
    {
    
    public:
    
        // construction support
        CBrowserViewImagesListBox();
        virtual ~CBrowserViewImagesListBox();
        void ConstructL( CCoeControl& aParent, 
                            CArrayFixFlat<TBrCtlImageCarrier>& iImages);

        // start thumbnailing
        // call just before ExecuteLD()
        void StartConversionL();

        const TBrCtlImageCarrier& CurrentlySelectedImage();

        // MMdaImageUtilObserver
        void MiuoConvertComplete(TInt aError);
        void MiuoOpenComplete(TInt aError);
        void MiuoCreateComplete(TInt aError);

        // leaving versions of used MMdaImageUtilObserver functions
        void MiuoOpenCompleteL(TInt aError);
        void MiuoConvertCompleteL(TInt aError);

    private:

        CGulIcon* CreatePlaceholderIconL();
        CGulIcon* CreateIconL(CFbsBitmap* aBitmap, TBool aShrinkIt=EFalse);

        void ConvertNextImageL();
        void DeleteCurrentBitmap();
        TInt CreateCurrentBitmapL(TBool aUseBitmapSize=EFalse);

        TSize Fit( const TSize& aSrc, const TSize& aTgt );

    private:
        TMdaWbmpClipFormat iFormat;
        TMdaWbmpCodec iCodec;

        TUint8* iDRMOutputBuf;          // owned
        TPtrC8 iImageDataPtr;           // Raw unconverted image data
        
        TFrameInfo iFrameInfo;
        TBool iRetryingConvert;
        TInt iImageIndex;       // points to the current image in iImages

        CArrayFixFlat<TBrCtlImageCarrier>* iImages; // not owned
        CMdaImageDescToBitmapUtility* iConverter;

        CFbsBitmap* iCurrentBitmap;             // NOT DELETED
        CArrayPtrFlat<CGulIcon>* iIcons;        // NOT DELETED
        CDesCArray* iItemList;                  // NOT DELETED
    };

#endif // __VIEWIMAGESLISTBOX_H

// End of file
