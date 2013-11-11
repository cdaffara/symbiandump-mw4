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
* Description:  Declares the CUpnpArgument class
*
*/


#ifndef C_CUPNPARGUMENT_H
#define C_CUPNPARGUMENT_H

// INCLUDES 

#include <e32base.h>
#include <badesca.h>
#include "upnpcons.h"

// ENUMERATIONS

enum { EIn, EOut };

// FORWARD DECLARATIONS

class CUpnpSoapMessage;
class CUpnpService;

// CLASS DECLARATION

/**
*  Used to make a common body for actions.
*  This class is only used when an action is sent or received.
*  This class makes a common body for actions, which can then be modified by user.
*
*  @since Series60 2.6
*/
class CUpnpArgument : public CBase
{
public: // Constructors and destructor
    static CUpnpArgument* NewL( CUpnpService& aParentService );
        
    /**
    * Two-phased constructor for building from a XML node.
    * Internally uses the function NewL(CSereneElement*)
    */
    static CUpnpArgument* NewL( CUpnpArgument& aArgument, 
                                         CUpnpService& aParentService );
    
    /** 
    * Destructor.
    */
    virtual ~CUpnpArgument();
    
public: // New functions

    /**
    * Returns type of the argument.
    * @since Series60 2.6
    * @return TArgumentType.
    */
    IMPORT_C TArgumentType Type();

    /**
    * Returns direction of the argument.
    * @since Series60 2.6
    * @return EIn or EOut.
    */
    IMPORT_C TInt Direction() const;

    /**
    * Returns related state variable.
    * @since Series60 2.6
    * @return the related state variable
    */
    IMPORT_C TDesC8& RelatedStateVariable();

    /**
    * Returns the name of the argument.
    * @since Series60 2.6
    * @return argument name
    */
    IMPORT_C TDesC8& Name();

    /**
    * Returns the value of the argument.
    * @since Series60 2.6
    * @return Value of argument.
    */
    IMPORT_C TDesC8& Value();

    /**
    * Sets the value of argument.
    * @since Series60 2.6
    * (If the value is inproper, the method leaves with EInvalidArgs,
	* which is the internal upnp error code used by UPnP Stack)
    * @param aValue Value that is to be set for this argument.
    */
    IMPORT_C void SetValueL( const TDesC8& aValue );
    
    void SetDirectionL( TInt aDirection );
    
    void SetNameL(const TDesC8& aDirection );
    
    void SetRelatedStateVarL(const TDesC8& aRelatedStateVar);
 
    void ConstructL();
     
    void SetType( TInt aType );
    
private: // Constructors

    /**
    * C++ default constructor.
    * @param aParentService Parent service.
    */
    CUpnpArgument( CUpnpService& aParentService );
        
    /**
    * By default Symbian 2nd phase constructor is private.
    * Adds the arguments and their default values according to the XML node
    * @param aNode CSenElement.
    */
    void ConstructL( CUpnpArgument& aArgument );

    /**
    * Delete and null iValue member.
    */
    void DeleteAndNullValue();
    
    /**
    * Checks the error code for the SetValue method and leaves if needed.
    */
    void CheckErrorL(TInt aError, const TDesC8& aValue);
private: // Data

    // Argument name buffer, owned
    HBufC8*     iName;

    // Related state varable buffer, owned
    HBufC8*     iRelatedStateVariable;

    // Direction, owned
    TInt        iDirection;
    
    // Value of the related state variable
    HBufC8*     iValue;

    // Type of the related state variable
    TInt        iType;
    
    // Parent service, owned
    CUpnpService&   iParentService;
};

#endif // C_CUPNPARGUMENT_H

// End of File