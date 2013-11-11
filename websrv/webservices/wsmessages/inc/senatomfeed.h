/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/






#ifndef SENATOMFEED_H_
#define SENATOMFEED_H_

/**
* 	Provides object and methods to manipulate <feed> tag from atom-pub API.
* 
*   @lib SenMessages.DLL
*   @since 5.0
*/



//  INCLUDES
#include <e32base.h>
#include <e32cmn.h>

#include "senatommessage.h"


class CSenAtomEntry;
class CDesC16ArrayFlat;


// CLASS DECLARATION


class CSenAtomFeed : public CSenAtomMessage
    {
    public:
        // Constructors and destructor
    
        /**
         * Two-phased constructor.
         */
        IMPORT_C static CSenAtomFeed* NewL(TInt64 aId = 0);

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CSenAtomFeed* NewLC(TInt64 aId = 0);

        /**
         * Destructor.
         */
        IMPORT_C ~CSenAtomFeed();   
        
        /*
        *		Method returns count of entries in the feed.
        *    	@return - number of tries
        * 
        */   
        IMPORT_C TInt EntryCount() ;//const;
        
        /*
        *		Method returns an entry at specified index from the feed. Index is made from inserting order or from order in the feed xml 
        * 		in case of parsing.
        * 		@param  aIndex - index of returned entry
        * 		@return - entry at index position
        */        
        IMPORT_C CSenAtomEntry* Entry( TInt aIndex ) const;
        
        /*
        *		Method adds the entry to the feed.
        * 		@param aEntry -  entry to insert
        * 		@return - error code
        */        
        IMPORT_C TInt AddEntryL(CSenAtomEntry& aEntry);
        
        /*
        *		Method removes from the feed an entry with 'id' tag specified as @aEntryId parameter.
        * 		@param aEntryId - id of an entry to remove
        */        
        IMPORT_C void RemoveEntryL( const TDesC8& aEntryId );
        
        /*
        *		Method removes from the feed first entry specified as @aEntry parameter. Distincion is made basing on 'id' tag of an entry.
        * 		@param aEntry - entry to remove  
        */        
        IMPORT_C void RemoveEntryL( CSenAtomEntry& aEntry);
        
        /*
        *		Method returns content of "subtitle" tag.
        * 		@return - descriptor containing value of subtitle.
        */
        IMPORT_C const TDesC8& SubtitleL();
        
        /*
        *		Method sets content of "subtitle" tag
        * 		@param aSubtitle - descriptor that contains value of subtitle. 
        */
        IMPORT_C void SetSubtitleL( const TDesC8& aTitle );
        
        /*
        *		Method sets content of "icon" tag
        * 		@param aUri - descriptor that contains value of icon's uri.
        */
        IMPORT_C void SetIconL( const TDesC8& aUri);
        
        /*
        *		Method returns content of "icon" tag.
        * 		@return - descriptor containing value of icon's uri.
        */
        IMPORT_C const TDesC8& IconL();
        
        /*
        *		Method sets content of "logo" tag
        * 		@param aUri - descriptor that contains value of logo tag.
        */
        IMPORT_C void SetLogoL( const TDesC8& logo);
        
        /*
        *		Method returns content of "logo" tag.
        *         @return - descriptor containing value of logo.        
        */
        IMPORT_C const TDesC8& LogoL();
        
        /*
        *        Method returns whole content of "generator" tag as TXmlEngElement.
        *         @return - generator tag as TXmlEngElement 
        */
        IMPORT_C TXmlEngElement GetGeneratorL();
        
        /*
        *        Method sets content of "generator" tag. Provided parameters correspond in name with subtags of the tag.
        *         @param aUri - uri value
        *          @param aVersion - version value
        *          @param aText - text value
        */
        IMPORT_C void SetGeneratorL( const TDesC8& aUri, const TDesC8& aVersion, const TDesC8& aText);
            

        //CALLBACK
        IMPORT_C virtual void OnStartElementL(const RTagInfo& aElement,
                                              const RAttributeArray& aAttributes,
                                              TInt aErrorCode);
        //CALLBACK
        IMPORT_C virtual void OnEndElementL(const RTagInfo& aElement,
                                            TInt aErrorCode);
        //CALLBACK
        IMPORT_C virtual void OnContentL(const TDesC8& aBytes, TInt aErrorCode);

       
    protected:
    
        IMPORT_C CSenAtomFeed();
        IMPORT_C void BaseConstructL();
        IMPORT_C void ParseEntryL(const RTagInfo& aElement, const RAttributeArray& aAttributes);

    protected: // Data
        CSenFragmentBase*            ipEntryFragment;
        RPointerArray<CSenAtomEntry> iEntries;
        CSenAtomEntry* iEntry;
        TXmlEngElement*    ipGroupFragment;

    };
#endif /*SENATOMFEED_H_*/
