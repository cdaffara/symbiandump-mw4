/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Image loading observe rinterface
 *
*/


#ifndef _INC_CSVGIMAGELOADINGOBSERVER_
#define _INC_CSVGIMAGELOADINGOBSERVER_

/**
 * Helping interface used as callback for image loading.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSVGImageLoadingObserver
    {
    public:
        /**
         * This method is called by the CSVGImageLoaderUtil object after
         * a request (RequestForImageL) to load an image is made.
         * The possible values for aErrorStatus include:
         *      KErrNone = no errors,
         *      KErrCancel = loading of image has been canceled
         *
         * @since 1.0
         * @param aErrorStatus : error status if any.
         * @return
         */
        virtual void ImageLoadingCompleted( TInt aErrorStatus ) = 0;
  };

#endif

