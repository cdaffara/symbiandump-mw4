/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef SEN_PROPERTIES_FRAGMENT_H
#define SEN_PROPERTIES_FRAGMENT_H

//  INCLUDES
#include <SenDomFragment.h>

class CSenPropertiesFragment : public CSenDomFragment
    {
    public:  // Constructors and destructor
        
        /**
        * Basic contructor. Should be used only for parsing new fragments etc. 
        * Constructing DomFragments for other use should be done with at least 
        * localname parameter.
        * @since Series60
        */
        IMPORT_C static CSenPropertiesFragment* NewL(
                                            RStringPool *ipStringPool = NULL);

        /**
        * Basic constructor.
        * @since Series60
        * @param aElement Element to copy construction data from.
        */
        IMPORT_C static CSenPropertiesFragment* NewL(
                                            const CSenElement& aElement,
                                            RStringPool *ipStringPool = NULL);

        /**
        * Basic constructor.
        * @since Series60
        * @param aLocalName is the XML localname of this fragment
        * Leave codes:  
        *       KErrSenInvalidCharacters if aLocalName contains
        *       illegal characters.     
        *       KErrSenZeroLengthDescriptor if aLocalName is zero length.
        */
        IMPORT_C static CSenPropertiesFragment* NewL(
                                            const TDesC8& aLocalName,
                                            RStringPool *ipStringPool = NULL);

        /**
        * Basic constructor.
        * @since Series60
        * @param aNsUri is the XML namespace user of this fragment
        * @param aLocalName is the XML localname of this fragment
        * Leave codes:  
        *       KErrSenInvalidCharacters if aLocalName contains
        *       illegal characters.     
        *       KErrSenZeroLengthDescriptor if aLocalName is zero length.
        */
        IMPORT_C static CSenPropertiesFragment* NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            RStringPool *ipStringPool = NULL);

        /**
        * Basic constructor.
        * @since Series60
        * @param aNsUri is the XML namespace user of this fragment
        * @param aLocalName is the XML localname of this fragment
        * @param aQName is the XML qualifiedname of this fragment
        * Leave codes:  
        *       KErrSenInvalidCharacters if aLocalName or aQName contains
        *       illegal characters.     
        *       KErrSenZeroLengthDescriptor if aLocalName or aQName is zero length.
        */
        IMPORT_C static CSenPropertiesFragment* NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            RStringPool *ipStringPool = NULL);

        /**
        * Basic constructor.
        * @since Series60
        * @param aNsUri is the XML namespace of this fragment
        * @param aLocalName is the XML localname of this fragment
        * @param aQName is the qualifiedname of this fragment
        * @param aAttrs are the XML attributes of this fragment
        * Leave codes:  
        *       KErrSenInvalidCharacters if aLocalName or aQName contains
        *       illegal characters.     
        *       KErrSenZeroLengthDescriptor if aLocalName or aQName is zero length.
        */
        IMPORT_C static CSenPropertiesFragment* NewL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttrs,
                                            RStringPool *ipStringPool = NULL);

        /**
        * Basic constructor.
        * @since Series60
        * @param aNsUri is the XML namespace of this fragment
        * @param aLocalName is the XML localname of this fragment
        * @param aQName is the qualifiedname of this fragment
        * @param aAttrs are the XML attributes of this fragment
        * @param aParent is the parent XML element of this fragment
        * Leave codes:  
        *       KErrSenInvalidCharacters if aLocalName or aQName contains
        *       illegal characters.     
        *       KErrSenZeroLengthDescriptor if aLocalName or aQName is zero length.
        */
        IMPORT_C static CSenPropertiesFragment* NewL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttrs,
                                            CSenElement& aParent,
                                            RStringPool *ipStringPool = NULL);


        virtual void ExpandL(const TDesC8& aNsUri,
                                      const TDesC8& aLocalName,
                                      const TDesC8& aQName,
                                      const RAttributeArray& aAttrs);

        virtual void SetStringPool(RStringPool& aStringPool);
                           
        virtual RStringPool& StringPool();
        
        virtual TBool HasStringPool() const;
                                      
        /**
        * Destructor.
        */
        virtual ~CSenPropertiesFragment();

    protected:  
        
        /**
        * C++ default constructor.
        */
        CSenPropertiesFragment();

        void BaseConstructL(const CSenElement& aElement, 
                            RStringPool *ipStringPool);

        void BaseConstructL(const TDesC8& aLocalName,
                            RStringPool *ipStringPool);

        void BaseConstructL(const TDesC8& aNsUri,
                            const TDesC8& aLocalName,
                            RStringPool *ipStringPool);

        void BaseConstructL(const TDesC8& aNsUri,
                            const TDesC8& aLocalName,
                            const TDesC8& aQName,
                            RStringPool *ipStringPool);

        void BaseConstructL(const TDesC8& aNsUri,
                            const TDesC8& aLocalName,
                            const TDesC8& aQName,
                            const RAttributeArray& aAttrs,
                            RStringPool *ipStringPool);

        void BaseConstructL(const TDesC8& aNsUri,
                            const TDesC8& aLocalName,
                            const TDesC8& aQName,
                            const RAttributeArray& aAttrs,
                            CSenElement& aParent,
                            RStringPool *ipStringPool);

        void BaseConstructL(CSenXmlReader& aReader,
                            RStringPool *ipStringPool);
        
        virtual void StartElementL(const TDesC8& aNsUri,
                                   const TDesC8& aLocalName,
                                   const TDesC8& aQName,
                                   const RAttributeArray& aAttrs);

        virtual void EndElementL(const TDesC8& aNsUri,
                                          const TDesC8& aLocalName,
                                          const TDesC8& aQName);

        virtual void CharactersL(const TDesC8& aChars,TInt aStart,TInt aLength);
        
        virtual void AllocContentSaverL();     
        
    protected: // Data
        RStringPool* ipStringPool;
    };

#endif //SEN_PROPERTIES_FRAGMENT_H

// End of File

