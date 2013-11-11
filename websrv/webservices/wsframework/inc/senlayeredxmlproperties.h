/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef SEN_LAYERED_XML_PROPERTIES_H
#define SEN_LAYERED_XML_PROPERTIES_H

#include <e32std.h>
#include <SenDomFragment.h>

#include "msenlayeredproperties.h"

// FORWARD DECLARATIONS
class RWriteStream;
class CSenXmlReader;
class CSenPropertiesFragment;

_LIT8(KSenLayeredXmlPropertiesLocalname,      "Properties");

class CSenLayeredXmlProperties : public CBase, public MSenLayeredProperties
    {
    public:

        static CSenLayeredXmlProperties* NewL();
        static CSenLayeredXmlProperties* NewLC();

        static CSenLayeredXmlProperties* NewL(const TDesC8& aXmlUtf8, CSenXmlReader& aParser);
        static CSenLayeredXmlProperties* NewLC(const TDesC8& aXmlUtf8, CSenXmlReader& aParser);

        static CSenLayeredXmlProperties* NewL(const CSenElement& aElement);
        static CSenLayeredXmlProperties* NewLC(const CSenElement& aElement);

        static CSenLayeredXmlProperties* NewL(RStringPool& aStringPool);
        static CSenLayeredXmlProperties* NewLC(RStringPool& aStringPool);

        static CSenLayeredXmlProperties* NewL(const TDesC8& aXmlUtf8, CSenXmlReader& aParser, RStringPool& aStringPool);
        static CSenLayeredXmlProperties* NewLC(const TDesC8& aXmlUtf8, CSenXmlReader& aParser, RStringPool& aStringPool);

        static CSenLayeredXmlProperties* NewL(const CSenElement& aElement, RStringPool& aStringPool);
        static CSenLayeredXmlProperties* NewLC(const CSenElement& aElement, RStringPool& aStringPool);

        // From MSenProperties
        virtual void SetReader(CSenXmlReader& aReader);
        virtual TSenPropertiesClassType PropertiesClassType();
        virtual void WriteToL(RWriteStream& aWriteStream);
        virtual void ReadFromL(const TDesC8& aBuffer);
        virtual HBufC8* AsUtf8L();
        virtual HBufC8* AsUtf8LC();
        virtual TBool IsSafeToCast(TSenPropertiesClassType aClass);
        /**
        * @see MSenProperties
        * Leave codes: 
        *      KErrSenInvalidCharacters if aName contains illegal characters.     
        *      KErrSenZeroLengthDescriptor if aName is zero length.
        */
        virtual TInt SetPropertyL(const TDesC8& aName,
                                  const TDesC8& aValue);
                                  
        virtual TInt PropertyL(const TDesC8& aName, 
                               TPtrC8& aValue);
               
        /**
        * @see MSenProperties
        * Leave codes: 
        *      KErrSenInvalidCharacters if aName contains illegal characters.     
        *      KErrSenZeroLengthDescriptor if aName is zero length.
        */
        virtual TInt SetIntPropertyL(const TDesC8& aName,
                                     const TInt aValue);
                                     
        virtual TInt IntPropertyL(const TDesC8& aName,
                                  TInt& aValue);
        /**
        * @see MSenProperties
        * Leave codes: 
        *      KErrSenInvalidCharacters if aName contains illegal characters.     
        *      KErrSenZeroLengthDescriptor if aName is zero length.
        */
        virtual TInt SetBoolPropertyL(const TDesC8& aName,
                                      const TBool aValue);
        virtual TInt BoolPropertyL(const TDesC8& aName,
                                   TBool& aValue);
        /**
        * @see MSenProperties
        * Leave codes: 
        *      KErrSenInvalidCharacters if aName contains illegal characters.     
        *      KErrSenZeroLengthDescriptor if aName is zero length.
        */
        virtual TInt SetOmittedL(const TDesC8& aName, TBool aValue);
        virtual TInt RemovePropertyL(const TDesC8& aName);
                                   
        // From MSenLayeredProperties 
        virtual void WriteToL(RWriteStream& aWriteStream, TSenPropertiesLayer aLayer);
        virtual void ReadFromL(const TDesC8& aBuffer, TSenPropertiesLayer aLayer);
        virtual HBufC8* AsUtf8L(TSenPropertiesLayer aLayer);
        virtual HBufC8* AsUtf8LC(TSenPropertiesLayer aLayer);        
        virtual TInt ShrinkTo(TSenPropertiesLayer aLayer);
        virtual void ExpandToL(TSenPropertiesLayer aLayer);
        virtual TSenPropertiesLayer TopLayer();

        /**
        * @see MSenLayeredProperties
        * Leave codes: 
        *      KErrSenInvalidCharacters if aName contains illegal characters.     
        *      KErrSenZeroLengthDescriptor if aName is zero length.
        */
        virtual TInt SetPropertyL(const TDesC8& aName, 
                                  const TDesC8& aValue,
                                  TSenPropertiesLayer aLayer);
                                  
        virtual TInt PropertyL(const TDesC8& aName, 
                               TPtrC8& aValue,
                               TSenPropertiesLayer aLayer);
                               
        virtual TInt PropertyL(const TDesC8& aName,
                               MSenProperty*& aValue,
                               TSenPropertiesLayer aLayer);
        
        virtual TInt PropertyL(const TDesC8& aName,
                               MSenProperty*& aValue);      
                               
        /**
        * Sets new property with certain type. 
        *
        * If property with given name does not exist, 
        * new property with given value will be added. Otherwise the value of 
        * existing property will be updated.
        *
        * @since Series60
        * @param aName         Name of the property, which can be later used to
        *                      refer the given value.
        * @param aValue        Is the value of this property.
        * @param aType         Is the type of this property.
        * @return              KErrNone if no error, or some of the system wide
        *                      error codes.
        */
        virtual TInt SetPropertyL(const TDesC8& aName, 
                                  const TDesC8& aValue,
                                  const TDesC8& aType);

        /**
        * Gets the value of the property behind certain name.
        * @since Series60
        * @param aName    The name identifying this property.
        * @param aValue   A TPtrC8 reference to be filled in with the value of
        *                 the property.
        * @param aType    A TPtrC8 reference to be filled in with the type of
        *                 the property.
        * @return         KErrNone if no error, or some of the system wide
        *                 error codes.     
        */
        virtual TInt PropertyL(const TDesC8& aName, 
                               TPtrC8& aValue,
                               TPtrC8& aType);                                                        

        virtual TInt PropertiesByTypeL(const TDesC8& aTypeName,
                                       RPointerArray<MSenProperty>& aArray);
                               
        /**
        * @see MSenLayeredProperties
        * Leave codes: 
        *      KErrSenInvalidCharacters if aName contains illegal characters.     
        *      KErrSenZeroLengthDescriptor if aName is zero length.
        */
        virtual TInt SetIntPropertyL(const TDesC8& aName, const TInt aValue,
                                     TSenPropertiesLayer aLayer);
                                     
        virtual TInt IntPropertyL(const TDesC8& aName, TInt& aValue,
                                  TSenPropertiesLayer aLayer);
                                  
        virtual MSenProperties* CloneL() const;
        
        virtual MSenProperties* Clone(TInt& aOkOrError) const;
        
        /**
        * @see MSenLayeredProperties
        * Leave codes: 
        *      KErrSenInvalidCharacters if aName contains illegal characters.     
        *      KErrSenZeroLengthDescriptor if aName is zero length.
        */
        virtual TInt SetBoolPropertyL(const TDesC8& aName, const TBool aValue,
                                      TSenPropertiesLayer aLayer);
                                      
        virtual TInt BoolPropertyL(const TDesC8& aName, TBool& aValue,
                                   TSenPropertiesLayer aLayer);        
                                   
        /**
        * @see MSenLayeredProperties
        * Leave codes: 
        *      KErrSenInvalidCharacters if aName contains illegal characters.     
        *      KErrSenZeroLengthDescriptor if aName is zero length.
        */
        virtual TInt SetOmittedL(const TDesC8& aName, TBool aValue,
                                 TSenPropertiesLayer aLayer);

        virtual TInt RemovePropertyL(const TDesC8& aName,
                                     TSenPropertiesLayer aLayer);
                                     
        /**
        * Destructor.
        */
        virtual ~CSenLayeredXmlProperties();

    protected: // base class functions

        virtual void BaseConstructL(const TDesC8& aLocalname, 
                                    const TDesC8& aXml,
                                    CSenXmlReader* aParser = NULL,
                                    RStringPool* aStringPool = NULL);

        virtual void BaseConstructL(const TDesC8& aNamespace, 
                                    const TDesC8& aLocalname, 
                                    const TDesC8& aXml,
                                    CSenXmlReader* aParser = NULL,
                                    RStringPool* aStringPool = NULL);

        virtual void BaseConstructL(const TDesC8& aNamespace, 
                                    const TDesC8& aLocalname, 
                                    const TDesC8& aQualifiedName, 
                                    const TDesC8& aXml,
                                    CSenXmlReader* aParser = NULL,
                                    RStringPool* aStringPool = NULL);

        virtual void BaseConstructL(const CSenElement& aElement,
                                    RStringPool* aStringPool = NULL);

        virtual TInt AllPropertiesByTypeL(const TDesC8& aTypeName,
                                          RPointerArray<MSenProperty>& aArray);

    protected:
        /**
        * Constructor.
        */
        CSenLayeredXmlProperties();
        
    private:
        void CreateChildPropertiesL(); //codescannerwarnings
        void ElementsL(RPointerArray<CSenElement>& aProps) const;
        
    protected:
        CSenPropertiesFragment*     ipFragment;
        CSenLayeredXmlProperties*   ipChildProperties;
        
    private:
        TSenPropertiesLayer         iLayer;
        CSenXmlReader*              ipReader;
        CSenLayeredXmlProperties*   ipParentProperties;
    };

#endif // SEN_LAYERED_XML_PROPERTIES_H

// End of File

