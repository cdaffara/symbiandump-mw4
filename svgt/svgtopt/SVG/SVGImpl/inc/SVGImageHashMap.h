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
* Description:  SVG Implementation header file
 *
*/


#ifndef __IMAGEHASHMAP_H__
#define __IMAGEHASHMAP_H__

#include <e32std.h>
#include "fbs.h"

	class CSvgImageMapItem: public CBase
    	{
    	public:

		enum TSvgImageType
    	{
    	ESvgBase64Image,
    	ESvgExternalImage
    	};

		static CSvgImageMapItem* NewL();

        static CSvgImageMapItem* NewLC();

        CSvgImageMapItem();

        ~CSvgImageMapItem();

		void                    ConstructL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        CFbsBitmap*    GetImagePtr();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        const TDesC&    GetImageName();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void            SetImagePtr( CFbsBitmap* aImagePtr );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void            CopyImageNameL( const TDesC& aImageName );

		TInt			IncrementReferenceCount( );

		TInt			DecrementReferenceCount( );

		void			SetImageDecodingStatus( TBool aThisImageIsDecoding );

		TBool GetImageDecodingStatus( );

		void			Print();

    public:
        CFbsBitmap*     iImagePtr;
        HBufC*           iImageName;
        TInt			iImageReferenceCount;
        TBool 			iImageDecodingStatus;
        TSvgImageType   iImageType;
    };

class CSvgImageHashMap : public CBase
    {
    public:

   static CSvgImageHashMap* NewL();

   static CSvgImageHashMap* NewLC();

   CSvgImageHashMap();

   ~CSvgImageHashMap();

	void               ConstructL();

	//IMAGE MAPPING METHODS
	TBool	AddImageL ( CFbsBitmap* aImage, const TDesC& aImageName );

    TBool	RemoveReferenceFromImageL( const TDesC& aImageName );

	CFbsBitmap*     GetImagePtr( const TDesC& aImageName );

	CSvgImageMapItem* GetImageMapItem( const TDesC& aImageName );

    TBool            HasImageName( const TDesC& aImageName );

    void             PrintImageHashMap();

    private:
    RPointerArray<CSvgImageMapItem>		iSvgImageMapItems;

    //data item used in the ImageHashMap

};

#endif
