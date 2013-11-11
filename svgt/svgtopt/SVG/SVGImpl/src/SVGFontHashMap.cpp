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


#include "SVGFontHashMap.h"
#include "SVGDocumentImpl.h"

//SVG Font storage 

// ==========================================================================
// Need method description
// ==========================================================================
CSvgFontHashMap* CSvgFontHashMap::NewL()
    {
    CSvgFontHashMap*   self    = new ( ELeave ) CSvgFontHashMap;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgFontHashMap* CSvgFontHashMap::NewLC()
    {
    CSvgFontHashMap*   self    = new ( ELeave ) CSvgFontHashMap;
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgFontHashMap::ConstructL()
    {
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgFontHashMap::CSvgFontHashMap()

    {
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgFontHashMap::~CSvgFontHashMap()
    {	
    	iSvgFontMapItems.ResetAndDestroy();
    	iSvgFontMapItems.Close();
    	
    	iFontDocumentPointers.ResetAndDestroy();
    	iFontDocumentPointers.Close();
    }

// ==========================================================================
// CSvgFontHashMap::AddFontL( CSvgFontElementImpl* aFontPtr,
//                                 const TDesC& aFontFamilyName )
// return ETrue if Font added
// return EFalse if Font was already in map
// ==========================================================================
TBool CSvgFontHashMap::AddFontL( CSvgFontElementImpl* aFontPtr,
                                 const TDesC& aFontFamilyName )
{
	CSvgFontMapItem* lFontMapItem = GetFontMapItem( aFontFamilyName );
    	
    	if (lFontMapItem)
    	{
    		//already a reference to this font in the font map
    		AddReferenceToFontL( aFontFamilyName);
    		return EFalse;
    	}
    	else
    	{
    		//didnt find that font in the font map...add it
			CSvgFontMapItem* lNewSvgFontMapItem = CSvgFontMapItem::NewL( );
		
			lNewSvgFontMapItem->SetFontPtr( aFontPtr );
       		lNewSvgFontMapItem->CopyFontFamilyName( aFontFamilyName );
       		lNewSvgFontMapItem->IncrementReferenceCount();
       		
			iSvgFontMapItems.Append(lNewSvgFontMapItem);
			
			return ETrue;
		}
}

// --------------------------------------------------------------------------
// void CSvgFontHashMap::AddReferenceToFontL( CFbsBitmap* aFont,
// return ETrue if reference successfully added
// return EFalse if already in map so just incremented reference
// ---------------------------------------------------------------------------
TBool CSvgFontHashMap::AddReferenceToFontL( const TDesC& aFontFamilyName )
    {	
    	CSvgFontMapItem* lFontMapItem = GetFontMapItem( aFontFamilyName );
    	
    	if (lFontMapItem)
    	{
    		//already a reference to this Font in the Font map
    		lFontMapItem->IncrementReferenceCount();
    		return ETrue;
    	}
    	else
    	{
    		return EFalse;
		}
        
    }

// --------------------------------------------------------------------------
// TBool CSvgFontHashMap::HasFontName( const TDesC& aFontName
// ---------------------------------------------------------------------------
TBool CSvgFontHashMap::HasFontFamilyName( const TDesC& aFontFamilyName )
    {
        TBool lHaveFont = EFalse;
		
		TInt svgFontMapItemsCnt = iSvgFontMapItems.Count();
        for (TInt i=0; i < svgFontMapItemsCnt; i++)
        {
            if ( aFontFamilyName == ((CSvgFontMapItem*)iSvgFontMapItems[i])->GetFontFamilyName() )
            {
                lHaveFont = ETrue;
            }
        }

        return lHaveFont;

    }

// --------------------------------------------------------------------------
// CSvgFontMapItem* CSvgFontHashMap::GetFontMapItem( const TDesC& aFontName
// ---------------------------------------------------------------------------
CSvgFontMapItem* CSvgFontHashMap::GetFontMapItem( const TDesC& aFontFamilyName )
{
	TInt svgFontMapItemsCnt = iSvgFontMapItems.Count();
	for ( TInt i = 0; i < svgFontMapItemsCnt; i++ )
            {
            CSvgFontMapItem* lFontMap = (CSvgFontMapItem*)iSvgFontMapItems[i];

			TInt result = aFontFamilyName.Compare( lFontMap->GetFontFamilyName() );
			
            if ( result == 0 )
                {
					return lFontMap;
				}
			}
	
	return NULL;
}

// --------------------------------------------------------------------------
// TBool CSvgDocumentImpl::RemoveReferenceFromFontL( const TDesC& aFontName )
// returns True if there aren't any left
// returns False if there are references to that Font left
// ---------------------------------------------------------------------------
TBool CSvgFontHashMap::RemoveReferenceFromFontL( const TDesC& aFontFamilyName )
    {

		CSvgFontMapItem* lFontMapItem = GetFontMapItem( aFontFamilyName );
		
		if (lFontMapItem != NULL)
		{
			//we have this lFontMapElement
			TInt aReferenceCount = lFontMapItem->DecrementReferenceCount();
			
			if (aReferenceCount <= 0)
			{
				//no more references to this Font remove it from the map
				iSvgFontMapItems.Remove(iSvgFontMapItems.Find(lFontMapItem));
				delete lFontMapItem;
				return ETrue;
			}
		}
		
		return EFalse;
    }

// --------------------------------------------------------------------------
// START OF EXTERNAL SVG FONT DOCUMENT METHODS
// --------------------------------------------------------------------------
// TBool CSvgFontHashMap::AddFontDocument(CSvgDocumentImpl* newFontDocument, const TDesC& aFontFamily)
// ---------------------------------------------------------------------------
TBool CSvgFontHashMap::AddFontDocument(CSvgDocumentImpl* aFontDocument, const TDesC& aFontFamily)
{
    if (aFontDocument == NULL)
    {
        //no document passed in
        return EFalse;
    }

    CSvgElementImpl* myFontElement = (CSvgElementImpl*)GetFontPtr(aFontFamily);

    if (myFontElement == NULL)
    {
        //no font in of this name in map so we can't add its document handle
        return EFalse;
    }

    if ( !HasFontDocument(aFontDocument) )
    {
            //then add it to the list of font documents
            iFontDocumentPointers.Append(aFontDocument);

            //and add it to the current document's font map
            TInt errorcode = KErrNone;
            TRAP(errorcode, AddFontL((CSvgFontElementImpl*)myFontElement, aFontFamily));

            return ETrue;
    }

    return EFalse;
}

// --------------------------------------------------------------------------
// void CSvgFontHashMap::DeleteFontDocument(const TDesC& aFontFamily)
// ---------------------------------------------------------------------------
void CSvgFontHashMap::DeleteFontDocument(const TDesC& aFontFamily)
{
    CSvgElementImpl* myFontElement = NULL;

	TInt fontDocPointersCnt = iFontDocumentPointers.Count();
    for (TInt i=0; i < fontDocPointersCnt; i++)
    {
        CSvgDocumentImpl* myDoc = (CSvgDocumentImpl*)iFontDocumentPointers[i];

        myFontElement = (CSvgElementImpl*)GetFontPtr(aFontFamily);

        if (myFontElement != NULL)
        {
            iFontDocumentPointers.Remove(i);
            TInt error = KErrNone;
            TRAP(error,RemoveReferenceFromFontL(aFontFamily));
            if (myDoc)
            {
                delete myDoc;
                myDoc = NULL;
            }
        }
    }
}

// --------------------------------------------------------------------------
// TBool CSvgFontHashMap::HasFontDocument(CSvgDocumentImpl* myDoc)
// ---------------------------------------------------------------------------
TBool CSvgFontHashMap::HasFontDocument(CSvgDocumentImpl* myDoc)
{
    //check to see if we have this document pointer already
    TBool hasFontPointer = EFalse;

	TInt fontDocPointersCnt = iFontDocumentPointers.Count();
    for (TInt i=0; i < fontDocPointersCnt; i++)
    {
        if ((CSvgDocumentImpl*)iFontDocumentPointers[i] == myDoc)
        {
            hasFontPointer = ETrue;
        }
    }

    return hasFontPointer;
}

// --------------------------------------------------------------------------
//END OF EXTERNAL SVG FONT DOCUMENT
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// void CSvgFontHashMap::PrintFontHashMap( )
// ---------------------------------------------------------------------------
void CSvgFontHashMap::PrintFontHashMap()
{
	RDebug::Printf("----Font Hash Map----\n");
	TInt svgFontMapItemsCnt = iSvgFontMapItems.Count();
	for (TInt i=0; i< svgFontMapItemsCnt; i++)
	{
		CSvgFontMapItem* lFontMapItem = iSvgFontMapItems[i];
		
		if(lFontMapItem)
		{
			lFontMapItem->Print();	
		}	
	}
}

// --------------------------------------------------------------------------
// CFbsBitmap* CSvgDocumentImpl::GetFontPtr( const TDesC& aFontName )
// ---------------------------------------------------------------------------
CSvgFontElementImpl* CSvgFontHashMap::GetFontPtr( const TDesC& aFontFamilyName )
    {
    
    CSvgFontMapItem* lFontMapItem = GetFontMapItem( aFontFamilyName );
    
    if (lFontMapItem)
    	{
    	return (lFontMapItem->GetFontPtr());
    	}
    
    	return NULL;
 
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgFontMapItem* CSvgFontMapItem::NewL()
    {
    CSvgFontMapItem*   self    = new ( ELeave ) CSvgFontMapItem;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgFontMapItem* CSvgFontMapItem::NewLC()
    {
    CSvgFontMapItem*   self    = new ( ELeave ) CSvgFontMapItem;
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgFontMapItem::ConstructL()
    {
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgFontMapItem::CSvgFontMapItem()
    {
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgFontMapItem::~CSvgFontMapItem()
    {
    	if (iFontFamilyName)
    	{
    		delete iFontFamilyName;
    		iFontFamilyName = NULL;
    	}
    	/*if (iFontPtr)
    	{
    		delete iFontPtr;
    	}
    	if (iFontDocumentPtr)
    	{
    		delete iFontDocumentPtr;
    	}*/
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFontElementImpl* CSvgFontMapItem::GetFontPtr()
    {
    return iFontPtr;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgFontMapItem::GetFontFamilyName()
    {
    return ( const TDesC & ) *iFontFamilyName;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgFontMapItem::SetFontPtr( CSvgFontElementImpl* aFontPtr )
    {
    iFontPtr = aFontPtr;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgFontMapItem::CopyFontFamilyName( const TDesC& aFontFamilyName )
    {
    if (iFontFamilyName)
    {
    	delete iFontFamilyName;
    	iFontFamilyName = NULL;
    }
    
    iFontFamilyName = aFontFamilyName.Alloc();
    }

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
TInt CSvgFontMapItem::IncrementReferenceCount( )
	{
	iFontReferenceCount++;
	return iFontReferenceCount;
	}

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
TInt CSvgFontMapItem::DecrementReferenceCount()
	{
	iFontReferenceCount--;
	return iFontReferenceCount;
	}

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------	
void CSvgFontMapItem::Print()
{
	RDebug::RawPrint(iFontFamilyName->Des());
	RDebug::Printf("ref_count = %d fontptr = %d fontdocptr = %d \n", iFontReferenceCount, iFontPtr, iFontDocumentPtr);	
}
