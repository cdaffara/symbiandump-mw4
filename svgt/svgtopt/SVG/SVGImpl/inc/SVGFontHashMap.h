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


#ifndef __FONTHASHMAP_H__
#define __FONTHASHMAP_H__

#include <e32std.h>
#include "SVGFontElementImpl.h"
    
	class CSvgFontMapItem: public CBase
    	{
    	public:

		enum TSvgFontType
    	{
    	ESvgSystemFont,
    	ESvgSvgFont,
    	ESvgExternalDocumentFont
    	};
    
		static CSvgFontMapItem* NewL();

        static CSvgFontMapItem* NewLC();
        
        CSvgFontMapItem();
        
        ~CSvgFontMapItem();
	
		void                    ConstructL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        CSvgFontElementImpl*    GetFontPtr();

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        const TDesC&    GetFontFamilyName();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void            SetFontPtr( CSvgFontElementImpl* aFontPtr );

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void            CopyFontFamilyName( const TDesC& aFontFamilyName );

		TInt			IncrementReferenceCount( );
		
		TInt			DecrementReferenceCount( );
		
		void			Print();
		
    public:
        CSvgFontElementImpl*    iFontPtr;
        CSvgDocumentImpl*		iFontDocumentPtr;
        HBufC*          		iFontFamilyName;
        TInt					iFontReferenceCount;
        TSvgFontType			iFontType;
    };   
    
class CSvgFontHashMap : public CBase
    {
    public:
   
   static CSvgFontHashMap* NewL();

   static CSvgFontHashMap* NewLC();
        
   CSvgFontHashMap();
        
   ~CSvgFontHashMap();
	
	void    ConstructL();
				
	//IMAGE MAPPING METHODS
    TBool   AddReferenceToFontL( const TDesC& aFontFamilyName );
	
	TBool	AddFontL ( CSvgFontElementImpl* aImage, const TDesC& aFontFamilyName );
	
    TBool	RemoveReferenceFromFontL( const TDesC& aFontFamilyName );

	CSvgFontElementImpl*     GetFontPtr( const TDesC& aFontFamilyName );
	
    TBool            HasFontFamilyName( const TDesC& aFontFamilyName );
    
    //FONT MAPPING METHODS

    TBool AddFontDocument(CSvgDocumentImpl* newFontDocument, const TDesC& aFontFamily);

    void DeleteFontDocument(const TDesC& aFontFamily);

    TBool HasFontDocument(CSvgDocumentImpl* aDoc);
        
    void			PrintFontHashMap();
        
    private:
    
    CSvgFontMapItem* GetFontMapItem( const TDesC& aFontFamilyName );
    
    RPointerArray<CSvgFontMapItem>		iSvgFontMapItems;
    
    RPointerArray<CSvgDocumentImpl> iFontDocumentPointers;
    
    //data item used in the FontHashMap
		
};

#endif
