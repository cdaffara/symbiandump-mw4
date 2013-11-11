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
* Description:  Declares the RUpnpGenaMessageFactory class. 
*
*/


#ifndef C_RUPNPGENAMESSAGEFACTORY_H
#define C_RUPNPGENAMESSAGEFACTORY_H

// INCLUDES

#include <e32base.h>
#include "upnpgenamessage.h"

// CLASS DECLARATION

/**
*  @brief Gena Message Factory.
*  
*  Used to create standard GENA-messages.
*
*  @since Series60 2.6
*/
class RUpnpGenaMessageFactory
{
public: // New functions

    /**
    * Constructor function; Creates subscription message with following 
    * parameters.
    * @since Series60 2.6
    * @param aPath Publisher path.
    * @param aHost Publisher host and port.
    * @param aCallback Callback address.
    */
    IMPORT_C static CUpnpGenaMessage* SubscribeL( const TDesC8& aPath,
                                                  TInetAddr& aHost,
                                                  const TDesC8& aCallback );
        
    /**
    * Constructor function; Creates subscription message with following 
    * parameters.
    * @since Series60 2.6
    * @param aPath Publisher path.
    * @param aHost Publisher host and port.
    * @param aCallback Callback address.
    * @param aTimeout Requested subscription timeout.
    */
    IMPORT_C static CUpnpGenaMessage* SubscribeL( const TDesC8& aPath,
                                                  TInetAddr& aHost,
                                                  const TDesC8& aCallback,
                                                  const TDesC8& aTimeout );

    /**
    * Constructor function; Creates resubscription message with following
    * parameters.
    * @since Series60 2.6
    * @param aPath Publisher path.
    * @param aHost Publisher host and port.
    * @param aSid Subscription id.
    */
    IMPORT_C static CUpnpGenaMessage* ResubscribeL( const TDesC8& aPath,
                                                    TInetAddr& aHost,
                                                    const TDesC8& aSid );

    /**
    * Constructor function; Creates resubscription message with following 
    * parameters.
    * @since Series60 2.6
    * @param aPath Publisher path.
    * @param aHost Publisher host and port.
    * @param aSid Subscription id.
    * @param aTimeout Requested subscription timeout.
    */
    IMPORT_C static CUpnpGenaMessage* ResubscribeL( const TDesC8& aPath,
                                                    TInetAddr& aHost,
                                                    const TDesC8& aSid,
                                                    const TDesC8& aTimeout );

    /**
    * Constructor function; Creates unsubscription message with following 
    * parameters.
    * @since Series60 2.6
    * @param aPath Publisher path.
    * @param aHost Publisher host and port.
    * @param aSid Subscription id.
    */
    IMPORT_C static CUpnpGenaMessage* UnsubscribeL( const TDesC8& aPath,
                                                    TInetAddr& aHost,
                                                    const TDesC8& aSid );

    /**
    * Constructor function; Creates  subscription response message with
    * following parameters.
    * @since Series60 2.6
    * @param aSessionId Sennion id to respond.
    * @param aDestination Destination address.
    * @param aSid Subscription id.
    * @param aTimeout Timeout defined by the Service. Integer or Infinite.
    * @return Response which is ready to send.
    */
    IMPORT_C static CUpnpGenaMessage* SubscribeResponseL( TInt aSessionId,
                                                          const TInetAddr& aDestination,
                                                          const TDesC8& aSid,
                                                          const TDesC8& aTimeout );

    /**
    * Constructor function; Creates  subscription response message with 
    * following parameters.
    * @since Series60 2.6
    * @param aSessionId Sennion id to respond.
    * @param aDestination Destination address.
    * @param aTimeStamp ETrue appends date-header to the message.
    * @param aSid Subscription id.
    * @param aTimeout Timeout defined by the Service. Integer or Infinite.
    * @return Response which is ready to send.
    */
    IMPORT_C static CUpnpGenaMessage* SubscribeResponseL(
                                                 TInt aSessionId,
                                                 const TInetAddr& aDestination,
                                                 TBool aTimeStamp,
                                                 const TDesC8& aSid,
                                                 const TDesC8& aTimeout );

    /**
    * Constructor function; Creates  unsubscription response message with
    * following parameters.
    * @since Series60 2.6
    * @param aSessionId Sennion id to respond.
    * @param aDestination Destination address.
    */
    IMPORT_C static CUpnpGenaMessage* UnsubscribeResponseL(
                                                         TInt aSessionId,
                                                         const TInetAddr& aDestination );

    /**
    * Constructor function; Creates a event message header with following 
    * parameters.
    * @since Series60 2.6
    * @param aPath Publisher path.
    * @param aHost Publisher host and port.
    * @param aSid Subscription ID.
    * @param aSeq Sequence key.
    * @return Event-message without body. Attach body using SetBodyL-function.
    */
    IMPORT_C static CUpnpGenaMessage* EventingLC( const TDesC8& aPath,
                                                  TInetAddr& aHost,
                                                  const TDesC8& aSid,
                                                  TUint32 aSeq );
};

#endif // C_RUPNPGENAMESSAGEFACTORY_H

// End of File
