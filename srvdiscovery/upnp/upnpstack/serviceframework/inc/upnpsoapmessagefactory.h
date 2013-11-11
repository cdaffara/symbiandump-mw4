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
* Description:  Declares the RUpnpSoapMessageFactory class
*
*/


#ifndef C_RUPNPSOAPMESSAGEFACTORY_H
#define C_RUPNPSOAPMESSAGEFACTORY_H

//  INCLUDES

//#include <e32base.h>
#include "upnphttpmessage.h"
#include "upnperrors.h"

// FORWARD DECLARATIONS

class CUpnpSoapMessage;
class CUpnpAction;

// CLASS DECLARATION

/**
*  SSDP Message Factory. Used to create standard SSDP messages.
*
*  @since Series60 2.6
*/
class RUpnpSoapMessageFactory
{
public: // New functions
    
    /**
    * SOAP request
    * @since Series60 2.6
    * @param aAction the action requested
    */
    IMPORT_C static CUpnpSoapMessage* SoapRequestL( CUpnpAction* aAction );
    
    /**
    * SOAP response
    * @since Series60 2.6
    * @param aAction the action response
    * @param aCode the error code
    */
    IMPORT_C static CUpnpSoapMessage* SoapResponseL( CUpnpAction* aAction, 
                             TUpnpErrorCode aCode );
    
    /**
    * SOAP response
    * @since Series60 2.6
    * @param aAction the action response
    * @param aCode the error code
    * @param aDescription the error code description
    */
    IMPORT_C static CUpnpSoapMessage* SoapResponseL( CUpnpAction* aAction, 
                             TUpnpErrorCode aCode, 
                             const TDesC8& aDescription);
                                 
    /**
    * SOAP response
    * @since Series60 2.6
    * @param aAction the action response
    */
    IMPORT_C static CUpnpSoapMessage* SoapResponseL( CUpnpAction* aAction );
    
    /**
    * SOAP response
    * @since Series60 2.6
    * @param aMessage the message response
    * @param aCode the error code response
    */
    IMPORT_C static CUpnpSoapMessage* SoapResponseL( CUpnpSoapMessage* aMessage, 
                             TUpnpErrorCode aCode );
       
};

#endif  // C_RUPNPSOAPMESSAGEFACTORY_H

// End of File