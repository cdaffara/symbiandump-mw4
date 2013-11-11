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
* Description:  Declares the CUpnpGenaMessage class. 
*
*/


#ifndef C_CUPNPGENAMESSAGE_H
#define C_CUPNPGENAMESSAGE_H

// INCLUDES

#include <e32base.h>
#include <in_sock.h>
#include "upnphttpmessage.h"
#include "upnphttpheader.h"
#include "upnpcons.h"

// CONSTANTS
/*
_LIT8( KNoHeader, "NoHeader" );
_LIT8( KDefaultHostPort, "80" );
_LIT8( KSidPrefix,"uuid:" );
_LIT8( KTimeoutPrefix,"Second-" );
*/
static const TInt KMaxGenaMessageLength = 20480;

// CLASS DECLARATION

/**
*  GENA-Message class. Used in sending and handling GENA-messages.
*
*  @since Series60 2.6
*/
class CUpnpGenaMessage : public CUpnpHttpMessage
{
public: // Constructors and destructor
    
    // Enumeration for GENA-message types:
    enum TGenaMessageType 
        {   
        EGenaSubscription = 1,
        EGenaResubscription,
        EGenaUnsubscription,
        EGenaEventing,
        EGenaSubscriptionResponse,
        EGenaResubscriptionResponse,
        EGenaUnsubscriptionResponse,
        EGenaEventingResponse
        };
    
    /**
    * Two-phased constructor.
    * @param aAddr the address
    */
    IMPORT_C static CUpnpGenaMessage* NewL( const TInetAddr& aAddr );
    
    /**
    * Two-phased constructor.
    * @param aAddr the address
    */
    IMPORT_C static CUpnpGenaMessage* NewLC( const TInetAddr& aAddr );
    
    /**
    * Virtual Destructor.
    */
    IMPORT_C virtual ~CUpnpGenaMessage();        
    
public: // New functions

    /**
    * Returns CALLBACK-header of GENA-message.
    * @since Series60 2.6
    * @return CALLBACK header of GENA message or NULL
    */
    IMPORT_C TDesC8& Callback();
    
    /**
    * Returns NT-header of GENA-message.
    * @since Series60 2.6
    * @return NT header of GENA message or NULL
    */
    IMPORT_C TDesC8& Nt();
    
    /**
    * Returns TIMEOUT-header of GENA-message.
    * @since Series60 2.6
    * @return TIMEOUT header of GENA message or NULL
    */
    IMPORT_C TDesC8& Timeout();
    
    /**
    * Returns SID-header of GENA-message.
    * @since Series60 2.6
    * @return SID header of GENA message or NULL
    */
    IMPORT_C TDesC8& Sid();
    
    /**
    * Returns NTS-header of GENA-message.
    * @since Series60 2.6
    * @return NTS header of GENA message or NULL
    */
    IMPORT_C TDesC8& Nts();
    
    /**
    * Returns SEQ-header of GENA-message.
    * @since Series60 2.6
    * @return SEQ header of GENA message or NULL
    */
    IMPORT_C const TDesC8& Seq();

private:
    
    /**
    * C++ default constructor.
    */
    IMPORT_C CUpnpGenaMessage(); 
    
    /**
    * Second phase construct.
    **/     
    void ConstructL(TInetAddr aSender);    
   
};

#endif // C_CUPNPGENAMESSAGE_H

// End of File