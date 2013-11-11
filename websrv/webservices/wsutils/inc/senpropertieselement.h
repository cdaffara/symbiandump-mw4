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








#include <SenXmlElement.h>
#include "MSenProperty.h"

#ifndef SEN_PROPERTIES_ELEMENT_H
#define SEN_PROPERTIES_ELEMENT_H

class CSenPropertiesElement : public CSenXmlElement, public MSenProperty
    {
    friend class CSenPropertiesFragment;
    
    public:  // Constructors and destructor

        /**
         *  Standard constructor.
         *  @param aLocalName   the local name for this element.
         *  Leave codes:    
         *      KErrSenInvalidCharacters if aLocalName contains illegal characters.     
         *      KErrSenZeroLengthDescriptor if aLocalName is zero length.
         */

        IMPORT_C static CSenPropertiesElement* NewL(const TDesC8& aLocalName,
                                                    RStringPool* aStringPool);
        /**
         *  Standard constructor.
         *  @param aNsUri       the namespace URI for this element.
         *  @param aLocalName   the local name for this element.
         *  Leave codes:    
         *      KErrSenInvalidCharacters if aLocalName contains illegal characters.     
         *      KErrSenZeroLengthDescriptor if aLocalName is zero length.
         */
        IMPORT_C static CSenPropertiesElement* NewL(const TDesC8& aNsUri,
                                                    const TDesC8& aLocalName,
                                                    RStringPool* aStringPool);

        /**
         *  Standard constructor.
         *  @param aNsUri       the namespace URI for this element.
         *  @param aLocalName   the local name for this element.
         *  @param aQName       the qualified name for this element.
         *  Leave codes:    
         *      KErrSenInvalidCharacters if aLocalName or aQName contains
         *      illegal characters.     
         *      KErrSenZeroLengthDescriptor if aLocalName or aQName is zero length.
         */
        IMPORT_C static CSenPropertiesElement* NewL(const TDesC8& aNsUri,
                                                    const TDesC8& aLocalName,
                                                    const TDesC8& aQName,
                                                    RStringPool* aStringPool);

        /**
         *  Standard constructor.
         *  @param aNsUri       the namespace URI for this element.
         *  @param aLocalName   the local name for this element.
         *  @param aQName       the qualified name for this element.
         *  @param apAttrs      the attributes for this element.
         *  Leave codes:    
         *      KErrSenInvalidCharacters if aLocalName or aQName contains
         *      illegal characters.     
         *      KErrSenZeroLengthDescriptor if aLocalName or aQName is zero length.
         */
        IMPORT_C static CSenPropertiesElement* NewL(const TDesC8& aNsUri, 
                                                    const TDesC8& aLocalName,
                                                    const TDesC8& aQName,
                                                    const RAttributeArray& apAttrs,
                                                    RStringPool* aStringPool);

        /**
         *  Standard constructor.
         *  @param aNsUri       the namespace URI for this element.
         *  @param aLocalName   the local name for this element.
         *  @param aQName       the qualified name for this element.
         *  @param apAttrs      the attributes for this element.
         *  @param aParent: the parent element for the new element
         *  Leave codes:    
         *      KErrSenInvalidCharacters if aLocalName or aQName contains
         *      illegal characters.     
         *      KErrSenZeroLengthDescriptor if aLocalName or aQName is zero length.
         */
        IMPORT_C static CSenPropertiesElement* NewL(const TDesC8& aNsUri,
                                                    const TDesC8& aLocalName,
                                                    const TDesC8& aQName,
                                                    const RAttributeArray& apAttrs,
                                                    CSenElement& aParent,
                                                    RStringPool* aStringPool);
  
        // From CSenXmlElement:

        virtual CSenElement* CreateElementL(const TDesC8& aNsPrefix,
                                            const TDesC8& aLocalName);
                                             
        virtual CSenElement& AddElementL(const TDesC8& aNsUri,
                                                  const TDesC8& aLocalName);

        virtual CSenElement& AddElementL(const TDesC8& aNsUri,
                                                  const TDesC8& aLocalName,
                                                  const TDesC8& aQName);

        virtual CSenElement& AddElementL(const TDesC8& aLocalName);

        virtual void WriteAsXMLToL(RWriteStream& aWriteStream);
        
        void CopyFromL(CSenElement& aSource);
        
        virtual const TDesC8& LocalName() const;

        virtual void Set( const TDesC8& aNsUri, 
                          const TDesC8& aLocalname,
                          const TDesC8& aQName );

        virtual TBool HasContent() const;

        virtual TPtrC8 Content() const;
        
        virtual TPtrC8 SetContentL(const TDesC8& aContent);

        virtual RWriteStream& ContentWriteStreamL();
        
        /**
        * Destructor.
        */
        virtual ~CSenPropertiesElement();
        
        // From MSenProperty
        virtual TPtrC8 Name();
        virtual TPtrC8 Type();
        virtual TPtrC8 Value();
        virtual TInt IntValue(TInt& aValue);
        virtual TInt BoolValue(TBool& aValue);

        /**
        * @param aTokens will contain tokanized values,
        * which ownership is NOT transferred to the caller(!)
        */
        virtual TInt ValueTokensL(const TDesC8& aDelimiter,
                                  RPointerArray<TPtrC8>& aTokens);
                                  
        virtual void SetStringPool(RStringPool& aStringPool);
        
    protected:  // New functions
    
        /**
        * C++ default constructor
        */
        CSenPropertiesElement();

        void BaseConstructL(const TDesC8& aLocalName,
                            RStringPool* aStringPool);

        void BaseConstructL(const TDesC8& aNsUri, const TDesC8& aLocalName,
                            RStringPool* aStringPool);
        
        void BaseConstructL(const TDesC8& aNsUri, const TDesC8& aLocalName,
                            const TDesC8& aQName, RStringPool* aStringPool);

        void BaseConstructL(const TDesC8& aNsUri, const TDesC8& aLocalName,
                            const TDesC8& aQName,
                            const RAttributeArray& apAttrs,
                            RStringPool* aStringPool);

        void BaseConstructL(const TDesC8& aNsUri, const TDesC8& aLocalName,
                            const TDesC8& aQName,
                            const RAttributeArray& apAttrs,
                            CSenElement& aParent,
                            RStringPool* aStringPool);
                                              
        virtual void WriteContentToL(RWriteStream& aWriteStream);
        
        virtual void AllocContentBufL();
        
        virtual RStringPool& StringPool();
        
    protected: // Data
        RStringPool*    ipStringPool;   // Not owned
        RString         iLocalName;     // Owned
        RString         iContent;       // Owned
    };        

#endif // SEN_PROPERTIES_ELEMENT_H

// End of File

