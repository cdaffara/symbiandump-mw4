/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Resolves entities.
*
*/


#ifndef XML_ENTITY_H
#define XML_ENTITY_H


// INCLUDES
#include <e32std.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  Resolves entities.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CXmlEntity: public CBase
    {
    public:  // Datatypes
        struct EntityEntry
            {
            const char*  entityName;
            TUint16      ucs2Value;
            };


    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CXmlEntity* NewL();
        
        /**
        * Destructor.
        */        
        virtual ~CXmlEntity();
        
        
    public:  // New Methods
        /**
        * Resolves the named entity into its char-value.  Can handle numeric entities.
        *
        * @since 3.0
        * @param aName The name of the entity.
        * @param aUcs2Value The resulting value.
        * @return void.
        */
        void ResolveL(const TDesC& aName, TUint16& aUcs2Value);

        /**
        * Resolves the named entity into its xmlEntity.  Can NOT handle numeric entities.
        *
        * @since 3.0
        * @param aName The name of the entity.
        * @return The LibXml2 entity.
        */
        const xmlEntity* ResolveL(const xmlChar *aName);


    private:  // New Methods
        /**
        * Looks up the named entity in the static table.  Can NOT handle numeric entities.
        *
        * @since 3.0
        * @param aName The name of the entity.
        * @return The EntityEntry.
        */
        const CXmlEntity::EntityEntry& LookupUsc2Value(const xmlChar *aName);

        /**
        * Resolves the numeric entity into it's value.
        *
        * @since 3.0
        * @param aName The name of the entity.
        * @param aUcs2Value The resulting value.
        * @return ETrue if the entity was resolved.
        */
        TBool ResolveNumericL(const TDesC& aName, TUint16& aUcs2Value);

        /**
        * Comparison method for iEntityMappings. 
        *
        * @since 3.0
        * @param aName The name of the entity.
        * @param aUcs2Value The resulting value.
        * @return ETrue if the entity was resolved.
        */
        static TInt LinearOrder(const xmlEntity& aFirst, const xmlEntity& aSecond);

        /**
        * Comparison method for iEntityMappings. 
        *
        * @since 3.0
        * @param aName The name of the entity.
        * @param aUcs2Value The resulting value.
        * @return ETrue if the entity was resolved.
        */
        static TInt LinearCaseOrder(const xmlEntity& aFirst, const xmlEntity& aSecond);


    private:
        /**
        * C++ default constructor.
        */
        CXmlEntity();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        
    private:  // Data
        TLeakTracker       iLeakTracker;
        
        RArray<xmlEntity>  iEntityMappings;        
    };

#endif      // XmlEntity
            
// End of File
