/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares the CUpnpDevice class
*
*/


#ifndef C_CUPNPDESCRIPTIONPROPERTY_H
#define C_CUPNPDESCRIPTIONPROPERTY_H


// INCLUDES
#include <e32std.h>
#include <badesca.h>
#include <f32file.h>
#include <s32file.h> 


// FORWARD DECLARATIONS
class TUpnpDevice;
class TUpnpService;
class CUpnpIcon;


// CLASS DECLARATION
/**
*  This class stores properties of a device, like: manufacturer, UDN and so on.
*  @lib serviceframework.lib
*  @since Series60 3.2
*/

class CUpnpDescriptionProperty: public CBase
{
public:
    
    /**
    * Two-phased constructor.    
    * @since Series60 3.2 
    * @param aName new property name
    * @param aValue new property value
    */	
    static CUpnpDescriptionProperty* NewL( const TDesC8& aName, const TDesC8& aValue );
    
    /**
    * Two-phased constructor.    
    * @since Series60 3.2    
    */
    static CUpnpDescriptionProperty* NewL();
    
    /**
    * Default C++ constructor    
    * @since Series60 3.2    
    */
    CUpnpDescriptionProperty();
                    
    /**
    * Destructor of the class.     
    * @since Series60 3.2    
    */
    ~CUpnpDescriptionProperty();
    
    /**
    * 2nd phase constructor    
    * @since Series60 3.2
    * @param aName new property name
    * @param aValue new property value
    */
    void ConstructL( const TDesC8& aName, const TDesC8& aValue );
    
    /**
    * Setter for property name    
    * @since Series60 3.2
    * @param aName new property name
    */
    void SetNameL( const TDesC8& aName);
    
    /**
    * Setter for property value    
    * @since Series60 3.2
    * @param aValue new property value
    */
    void SetValueL( const TDesC8& aValue );
    
    /**
    * Getter for property name    
    * @since Series60 3.2
    * @return property name
    */
    const TDesC8& Name();
    
    /**
    * Getter for property value
    * @since Series60 3.2
    * @return property value
    */
    const TDesC8& Value();

private:
    HBufC8* iName;
    HBufC8* iValue;    
};

#endif  // C_CUPNPDESCRIPTIONPROPERTY_H

// End of File