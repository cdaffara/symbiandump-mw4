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
* Description:  SVG Implementation source file
 *
*/


#include "SVGImageHashMap.h"

// DECODED IMAGE STORAGE FOR IMAGE ELEMENTS TO SHARE
// SO IMAGES WITH THE SAME XLINK-HREF don't have to decode twice
// SVG FONT STORAGE FOR JSR 226 ONLY
// BECAUSE WE DONT HAVE AN ENGINE AT PARSE TIME
// FOR SVG FONTS

// ==========================================================================
// Need method description
// ==========================================================================
CSvgImageHashMap* CSvgImageHashMap::NewL()
    {
    CSvgImageHashMap*   self    = new ( ELeave ) CSvgImageHashMap;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgImageHashMap* CSvgImageHashMap::NewLC()
    {
    CSvgImageHashMap*   self    = new ( ELeave ) CSvgImageHashMap;
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgImageHashMap::ConstructL()
    {
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgImageHashMap::CSvgImageHashMap()

    {
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgImageHashMap::~CSvgImageHashMap()
    {
    	iSvgImageMapItems.ResetAndDestroy();
    	iSvgImageMapItems.Close();
    }

// ==========================================================================
// CSvgImageHashMap::AddImageL( CFbsBitmap* aImagePtr,
//                                 const TDesC& aImageName )
// return ETrue if image added
// return EFalse if image was already in map
// ==========================================================================
TBool CSvgImageHashMap::AddImageL( CFbsBitmap* aImagePtr,
                                 const TDesC& aImageName )
{
	CSvgImageMapItem* lImageMapItem = GetImageMapItem( aImageName );

    	if (lImageMapItem)
    	{
    		//already a reference to this image in the image map
    		lImageMapItem->IncrementReferenceCount();
    		return EFalse;
    	}
    	else
    	{
    		//didnt find that image in the image map...add it
			CSvgImageMapItem* lNewSvgImageMapItem = CSvgImageMapItem::NewL( );

			lNewSvgImageMapItem->SetImagePtr( aImagePtr );
       		lNewSvgImageMapItem->CopyImageNameL( aImageName );
       		lNewSvgImageMapItem->IncrementReferenceCount();

			iSvgImageMapItems.Append(lNewSvgImageMapItem);

			return ETrue;
		}
}

// --------------------------------------------------------------------------
// TBool CSvgImageHashMap::HasImageName( const TDesC& aImageName
// ---------------------------------------------------------------------------
TBool CSvgImageHashMap::HasImageName( const TDesC& aImageName )
    {
        TBool lHaveImage = EFalse;

		TInt svgImageMapItemsCnt = iSvgImageMapItems.Count();
        for (TInt i=0; i < svgImageMapItemsCnt; i++)
        {
            if ( aImageName == ((CSvgImageMapItem*)iSvgImageMapItems[i])->GetImageName() )
            {
                lHaveImage = ETrue;
            }
        }

        return lHaveImage;

    }

// --------------------------------------------------------------------------
// CSvgImageMapItem* CSvgImageHashMap::GetImageMapItem( const TDesC& aImageName
// ---------------------------------------------------------------------------
CSvgImageMapItem* CSvgImageHashMap::GetImageMapItem( const TDesC& aImageName )
{
	TInt svgImageMapItemsCnt = iSvgImageMapItems.Count();
	for ( TInt i = 0; i < svgImageMapItemsCnt; i++ )
            {
            CSvgImageMapItem* lImageMap = (CSvgImageMapItem*)iSvgImageMapItems[i];

			TInt result = aImageName.Compare( lImageMap->GetImageName() );

            if ( result == 0 )
                {
					return lImageMap;
				}
			}

	return NULL;
}

// --------------------------------------------------------------------------
// TBool CSvgDocumentImpl::RemoveReferenceFromImageL( const TDesC& aImageName )
// returns True if there aren't any left
// returns False if there are references to that image left
// ---------------------------------------------------------------------------
TBool CSvgImageHashMap::RemoveReferenceFromImageL( const TDesC& aImageName )
    {

		CSvgImageMapItem* lImageMapItem = GetImageMapItem( aImageName );

		if (lImageMapItem != NULL)
		{
			//we have this lImageMapElement
			TInt aReferenceCount = lImageMapItem->DecrementReferenceCount();

			if (aReferenceCount <= 0)
			{
				//no more references to this image remove it from the map
				iSvgImageMapItems.Remove(iSvgImageMapItems.Find(lImageMapItem));
				delete lImageMapItem;
				return ETrue;
			}
		}

		return EFalse;
    }

// --------------------------------------------------------------------------
// CFbsBitmap* CSvgDocumentImpl::GetImagePtr( const TDesC& aImageName )
// ---------------------------------------------------------------------------
CFbsBitmap* CSvgImageHashMap::GetImagePtr( const TDesC& aImageName )
    {

    CSvgImageMapItem* lImageMapItem = GetImageMapItem( aImageName );

    if (lImageMapItem)
    	{
    	return (lImageMapItem->GetImagePtr());
    	}

    	return NULL;

    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgImageMapItem* CSvgImageMapItem::NewL()
    {
    CSvgImageMapItem*   self    = new ( ELeave ) CSvgImageMapItem;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgImageMapItem* CSvgImageMapItem::NewLC()
    {
    CSvgImageMapItem*   self    = new ( ELeave ) CSvgImageMapItem;
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgImageMapItem::ConstructL()
    {
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgImageMapItem::CSvgImageMapItem(): iImageDecodingStatus(EFalse)

    {
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgImageMapItem::~CSvgImageMapItem()
    {
    	if (iImageName->Length())
    	{
    		//delete the xlink-href (base64 image data
    		delete iImageName;
    		iImageName = NULL;
    	}
    	/*if (iImagePtr)
    	{
    		//delete the bitmap
    		delete iImagePtr;
    	}*/
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CFbsBitmap* CSvgImageMapItem::GetImagePtr()
    {
    return iImagePtr;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgImageMapItem::GetImageName()
    {
    return ( const TDesC & ) *iImageName;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgImageMapItem::SetImagePtr( CFbsBitmap* aImagePtr )
    {
    iImagePtr = aImagePtr;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgImageMapItem::CopyImageNameL( const TDesC& aImageName )
    {
    iImageName = aImageName.AllocL();
    }

//----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
void CSvgImageMapItem::SetImageDecodingStatus( TBool aThisImageIsDecoding )
	{
	iImageDecodingStatus = aThisImageIsDecoding;
	}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
TBool CSvgImageMapItem::GetImageDecodingStatus( )
	{
	return iImageDecodingStatus;
	}

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
TInt CSvgImageMapItem::IncrementReferenceCount( )
	{
	iImageReferenceCount++;
	return iImageReferenceCount;
	}

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
TInt CSvgImageMapItem::DecrementReferenceCount()
	{
	iImageReferenceCount--;
	return iImageReferenceCount;
	}


