/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef M_SEN_LAYERED_PROPERTIES_H
#define M_SEN_LAYERED_PROPERTIES_H

// INCLUDES
#include <e32std.h>
#include "MSenProperties.h"
#include "MSenProperty.h"

class MSenLayeredProperties : public MSenProperties // Does not implement ReadFromL(UTF-8), but DOES implement WriteToL() in a way, where a flat property is generated looping through all the layers..
    {
    public:
        /**
        * PropertiesClassType enumeration.
        */
        enum TSenPropertiesLayer
        	{
        	ESenTransportLayer          = 1,
        	ESenSereneLayer             = 2,
        	ESenFrameworkLayer          = 3,
        	ESenProviderSessionLayer    = 4,
        	ESenConsumerSessionLayer    = 5,
        	ESenMessageLayer            = 6,
        	ESenLastLayer               = ESenMessageLayer
        	};
        	
        	     
        	
        /**
        * Writes given layer from this properties object as descriptor
        * into a stream.
        * UTF-8 charset encoding will be used.
        * @since Series60
        * @param aLayer The layer to be used.
        * @param aWriteStream The stream to write to.
        */
        virtual void WriteToL(RWriteStream& aWriteStream, TSenPropertiesLayer aLayer) = 0;

        /**
        * Reads this properties object from the descriptor to given
        * layer.
        * UTF-8 charset encoding should be used.
        * @since Series60
        * @param aBuffer The buffer to read from.
        * @param aLayer The layer to be used.
        */
        virtual void ReadFromL(const TDesC8& aBuffer, TSenPropertiesLayer aLayer) = 0;

        /**
        * Get the given layer from these properties as UTF-8 charset
        * encoded descriptor.
        * @since Series60
        * @param aLayer The layer to be used.
        * @return a pointer to UTF-8 encoded descriptor.
        * Caller takes ownership.
        */
        virtual HBufC8* AsUtf8L(TSenPropertiesLayer aLayer) = 0;

        /**
        * Get the given layer from these properties as UTF-8 charset
        * encoded descriptor.
        * @since Series60
        * @param aLayer The layer to be used.
        * @return a pointer to UTF-8 encoded descriptor, which is
        * has been left on cleanup stack.
        * Caller takes ownership.
        */
        virtual HBufC8* AsUtf8LC(TSenPropertiesLayer aLayer) = 0;
        
        virtual TInt ShrinkTo(TSenPropertiesLayer aLayer) = 0;
        
        virtual void ExpandToL(TSenPropertiesLayer aLayer) = 0;
        
        virtual TSenPropertiesLayer TopLayer() = 0;
        
        /**
        * Sets new property.
        *
        * If property with given name does not exist, 
        * new property with given value will be added. Otherwise the value of
        * existing property will be updated.
        *
        * @since Series60
        * @param aName         Name of the property, which can be later used
        *                      to refer the given value.
        * @param aValue        Is the value of this property.
        * @param aLayer        The layer to be used.
        * @return              KErrNone if no error, or some of the system wide
        *                      error codes.
        * Leave codes:
        *                      According to implementing classes. 
        */
        virtual TInt SetPropertyL(const TDesC8& aName,
                                  const TDesC8& aValue,
                                  TSenPropertiesLayer aLayer) = 0;
        
        /**
        * Gets the value of the property behind certain name.
        * @since Series60
        * @param aName    The name identifying this property.
        * @param aValue   A TPtrC8 reference to be filled in with the value of
        *                 the property.
        * @param aLayer   The layer to be used.
        * @return         KErrNone if no error, or some of the system wide
        *                 error codes.
        */
        virtual TInt PropertyL(const TDesC8& aName,
                               TPtrC8& aValue,
                               TSenPropertiesLayer aLayer) = 0;
                               
        /**
        * Gets the MSenProperty of the property behind certain name.
        * @since Series60
        * @param aName    The name identifying this property.
        * @param aValue   A MSenProperty reference to pointer to be filled in with
        *                 the pointer to the property.
        * @param aLayer   The layer to be used.
        * @return         KErrNone if no error, or some of the system wide
        *                 error codes.
        */
        virtual TInt PropertyL(const TDesC8& aName,
                               MSenProperty*& aValue,
                               TSenPropertiesLayer aLayer) = 0;                               
        
        /**
        * Gets the MSenProperty of the property behind certain name.
        * @since Series60
        * @param aName    The name identifying this property.
        * @param aValue   A MSenProperty reference to pointer to be filled in with
        *                 the pointer to the property.
        * @return         KErrNone if no error, or some of the system wide
        *                 error codes.
        */
        virtual TInt PropertyL(const TDesC8& aName,
                               MSenProperty*& aValue) = 0;                               

        /**
        * Gets the array of MSenProperties which match to given property type.
        * @since Series60
        * @param aTypeName The name identifying property type.
        * @param aArray    An array containing MSenProperties which macth
        *                  to given property type.
        * @return          KErrNone if no error, or some of the system wide
        *                  error codes.
        */
        virtual TInt PropertiesByTypeL(const TDesC8& aTypeName,
                                      RPointerArray<MSenProperty>& aArray) = 0;

        /**
        * Sets new integer property.
        *
        * If property with given name does not exist, 
        * new property with given value will be added. Otherwise the value of
        * existing property will be updated.
        *
        * @since Series60
        * @param aName         Name of the property, which can be later used
        *                      to refer the given value.
        * @param aValue        Is the integer value of this property.
        * @param aLayer        The layer to be used.
        * @return              KErrNone if no error, or some of the system wide
        *                      error codes.
        * Leave codes:
        *                      According to implementing classes. 
        */
        virtual TInt SetIntPropertyL(const TDesC8& aName,
                                     const TInt aValue,
                                     TSenPropertiesLayer aLayer) = 0;
        
        /**
        * Gets the integer value of the property behind certain name.
        * @since Series60
        * @param aName    The name identifying this property.
        * @param aValue   A TInt reference to be filled in with the value of
        *                 the property.
        * @param aLayer   The layer to be used.
        * @return         The value of the property, or KNullDesC8 if
        *                 property is not found.
        */
        virtual TInt IntPropertyL(const TDesC8& aName,
                                  TInt& aValue,
                                  TSenPropertiesLayer aLayer) = 0;
        
        /**
        * Sets new boolean property.
        *
        * If property with given name does not exist, 
        * new property with given value will be added. Otherwise the value of
        * existing property will be updated.
        *
        * @since Series60
        * @param aName         Name of the property, which can be later used
        *                      to refer the given value.
        * @param aValue        The boolean value of this property.
        * @param aLayer        The layer to be used.
        * @return              KErrNone if no error, or some of the system wide
        *                      error codes.
        * Leave codes:
        *                      According to implementing classes. 
        */
        virtual TInt SetBoolPropertyL(const TDesC8& aName,
                                      const TBool aValue,
                                      TSenPropertiesLayer aLayer) = 0;
        
        /**
        * Gets the boolean value of the property behind certain name.
        * @since Series60
        * @param aName    The name identifying this property.
        * @param aValue   A TBool reference to be filled in with the value of
        *                 the property.
        * @param aLayer   The layer to be used.
        * @return         KErrNone if no error, or some of the system wide
        *                 error codes.
        */
        virtual TInt BoolPropertyL(const TDesC8& aName,
                                   TBool& aValue,
                                   TSenPropertiesLayer aLayer) = 0;
        
    };

#endif // M_SEN_LAYERED_PROPERTIES_H


// End of File

