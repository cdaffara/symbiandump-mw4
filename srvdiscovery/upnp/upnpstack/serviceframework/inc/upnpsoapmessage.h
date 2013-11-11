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
* Description:  Declares the CUpnpSoapMessage class
*
*/


#ifndef C_CUPNPSOAPMESSAGE_H
#define C_CUPNPSOAPMESSAGE_H

// INCLUDES

//#include <e32base.h>
//#include <in_sock.h>
#include "upnphttpmessage.h"
#include "upnphttpheader.h"
#include "upnperrors.h"

// CLASS DECLARATION

/**
*  SOAP-Message class.
*  Used in sending and handling SOAP messages.
*
*  @since Series60 2.6
*/
class CUpnpSoapMessage : public CUpnpHttpMessage
{
public: // Constructors and destructor
    
    /**
    * Two-phased constructor.
    * @param aBuffer Buffer which contains the message.
    * @param aAddr Target IP address.
    */
    IMPORT_C static CUpnpSoapMessage* NewL( TDesC8& aBuffer, const TInetAddr& aAddr );
    
    /**
    * Destructor.
    */
    IMPORT_C ~CUpnpSoapMessage();
    
public: // New functions

    /**
    * Returns SOAPACTION header
    * @since Series60 2.6
    * @return SOAPACTION header or NULL
    */
    IMPORT_C const TPtrC8 SoapAction();

    /**
    * Returns service type
    * @since Series60 2.6
    * @return service type or NULL
    */
    IMPORT_C const TPtrC8 ServiceType();

    /**
    * Returns action name
    * @since Series60 2.6
    * @return action name or NULL
    */
    IMPORT_C const TPtrC8 ActionName();

private: // Constructor

    /**
    * C++ default constructor.
    */
    CUpnpSoapMessage();
        
    /**
    * Second phase construct.
    * @param aSender
    **/     
    void ConstructL(TInetAddr aSender);    
};

#endif // C_CUPNPSOAPMESSAGE_H

// End of File