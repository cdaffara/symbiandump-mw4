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
* Description:  Declares main application class.
*
*/


#ifndef C_CUPNPSSDPMESSAGE_H
#define C_CUPNPSSDPMESSAGE_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "upnphttpmessage.h"
#include "upnphttpheader.h"

// CLASS DECLARATION

/**
*  Ssdp-Message class. Used in sending and handling Ssdp messages.
*
*  @since Series60 2.0
*/
class CUpnpSsdpMessage : public CUpnpHttpMessage
    {
    
    public:

        /**
        * Enumeration for message types:
        */
        enum TRequestType
            {
                // This value is used in notify-type of messages, such as 
                // ssdp:alive.
                ERequestNotify =1,
                // This value is used in ssdp:byebye messages.
                ERequestByeBye,
                // This value is used in search messages.
                ERequestSearch,
                // This value is used in replies to search messages.
                ERequestSearchResponse,
                
                ERequestUndefined
                
            };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aAddr Sender's IP address.
        * @param aRequestType Type of message to be constructed.
        */
        IMPORT_C static CUpnpSsdpMessage* NewL( TInetAddr& aAddr, 
                                            TRequestType aRequestType);
        
        /**
        * Two-phased constructor.
        * @param aRequestType Type of message to be constructed.
        */
        IMPORT_C static CUpnpSsdpMessage* NewL( TRequestType aRequestType );
        
        /**
        * Two-phased constructor.
        * @param aBuffer Buffer which contains the message.
        * @param aAddr Target IP address.
        */
        IMPORT_C static CUpnpSsdpMessage* NewL(TDesC8& aBuffer, TInetAddr& aAddr );
        
        /**
        * Destructor.
        */
        IMPORT_C ~CUpnpSsdpMessage();
        
    public: // New functions

        /**
        * Buffer to string.
        * @since Series60 2.0
        * @return HBufC8 pointer
        */
        HBufC8* ToStringL();

        /**
        * Cache control.
        * @since Series60 2.0
        * @return TInt
        */
        IMPORT_C TInt CacheControl();

        /**
        * Return UUID
        * @since Series60 2.0
        * @return TPtr8
        */
        IMPORT_C TPtr8 Uuid();

        /**
        * Return location
        * @since Series60 2.0
        * @return TDesC8
        */
        IMPORT_C TDesC8& Location();

        /**
        * Returns if SSDP notify.
        * @since Series60 2.0
        * @return TBool 
        */
        IMPORT_C TBool IsSsdpNotify();
        
        /**
        * Sets the host-header of Ssdp message.
        * @since Series60 2.0
        * @param aHost Value of host to be set.
        */
    IMPORT_C void AddHostL( TDesC8& aHost );
        
        /**
        * Sets the USN-header of Ssdp message.
        * @since Series60 2.0
        * @param aUsn Value of USN to be set.
        */
        void AddUsnL(TDesC8& aUsn);
        
        /**
        * Sets the Cache-control-header of Ssdp message.
        * @since Series60 2.0
        * @param aMaxAge Value of Cache-Control to be set.
        */
        void AddCacheControlL(TDesC8& aMaxAge);
        
        /**
        * Sets the Description-header of Ssdp message.
        * @since Series60 2.0
        * @param aDescriptionLocation Value of description to be set.
        */
        void AddLocationL(TDesC8& aDescriptionLocation);
        
        /**
        * Sets the ST-header of Ssdp message.
        * @since Series60 2.0
        * @param aSt Value of ST to be set.
        */
        void AddSearchTargetStringL(TDesC8& aSt);
        
        /**
        * Sets the NT-header of Ssdp message.
        * @since Series60 2.0
        * @param aNT Value of NT to be set.
        */
        void AddNotificationTypeL(TDesC8& aNT);
        
        /**
        * Sets the Server description-header of Ssdp message.
        * @since Series60 2.0
        * @param aServerType Value of server description to be set.
        */
        void AddServerL(TDesC8& aServerType);
        
        /**
        * Sets the MX-header of Ssdp message.
        * @since Series60 2.0
        * @param aMx Value of MX to be set.
        */
    IMPORT_C void AddMxL(TDesC8& aMx);
        
        /**
        * Search target.
        * @since Series60 2.0
        * @return TInt
        */
        IMPORT_C TInt SearchTarget();
        
        /**
        * Returns ETrue, if message is a Alive-message, else returns EFalse.
        * @since Series60 2.0
        * @return ETrue, if message is a Alive-message, 
        * else returns EFalse.
        */
        IMPORT_C TBool IsSsdpAlive();

        /**
        * Returns ETrue, if message is a Byebye-message, else returns EFalse.
        * @since Series60 2.0
        * @return ETrue, if message is a Byebye-message, 
        * else returns EFalse.
        */
        IMPORT_C TBool IsSsdpByebye();

        /**
        * Returns ETrue, if message is a search message, else returns EFalse.
        * @since Series60 2.0
        * @return ETrue, if message is a search message, else returns EFalse.
        */
        IMPORT_C TBool IsSsdpMSearch();

        /**
        * Return if SSDP response.
        * @since Series60 2.0
        * @return ETrue, if message is a response to search message, 
        * else returns EFalse.
        */
        IMPORT_C TBool IsSsdpResponse();
        
        /**
        * Return if search all.
        * @since Series60 2.0
        * @return ETrue, if message is a search message and 
        * search is for all devices, else returns EFalse.
        */
        IMPORT_C TBool IsSearchAll();

        /**
        * Return if search device.
        * @since Series60 2.0
        * @return ETrue, if message is a search message and 
        * search is for certain devices, else returns EFalse.
        */
        IMPORT_C TBool IsSearchDevice();

        /**
        * Return if search service.
        * @since Series60 2.0
        * @return ETrue, if message is a search message and 
        * search is for certain services, else returns EFalse.
        */
        IMPORT_C TBool IsSearchService();
        
        /**
        * Returns NT-header of Ssdp message.
        * @since Series60 2.0
        * @return NT-header of Ssdp message.
        */
        IMPORT_C TDesC8& Nt();
        
        /**
        * Returns USN-header of Ssdp message.
        * @since Series60 2.0
        * @return USN-header of Ssdp message.
        */
        IMPORT_C TDesC8& Usn();
        
        /**
        * Returns ST-header of Ssdp message.
        * @since Series60 2.0
        * @return ST-header of Ssdp message.
        */
        IMPORT_C TDesC8& St();

        /**
        * Returns NTS-header of Ssdp message.
        * @since Series60 2.0
        * @return NTS-header of Ssdp message.
        */
        IMPORT_C TDesC8& Nts();
        
        /**
        * Enumeration for search message types:
        * @since Series60 2.0
        */
        enum TSearchTarget 
            {
            
                // This message is not a search message.
                ESsdpNone = 1, 
                // This is a search all message.
                ESsdpAll,
                // This is a root device search.
                EUPnPRootDevice,
                // This is a search by specific uuid.
                EDeviceUid,
                // This is a search by specific device type.
                EDeviceType,
                // This is a search by specific service type.
                EServiceType

            };
        
    protected: // from CUpnpHttpMessage
		/**
		Append ContentLength when headers are dumped to string (using public method HeadersToStringL)
		@param aHeaders without "Content-Length"
		**/	
		IMPORT_C virtual void AppendContentLengthToHeadersL(HBufC8*& aHeaders);

    private:

        /**
        * C++ default constructor.
        */
        CUpnpSsdpMessage();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aRequestType Type of message to be constructed.
        */
        void ConstructL( TRequestType aRequestType );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aAddr Remote IP address
        */
		void HttpConstructL( TInetAddr& aAddr );

    private:

        /**
        * Parse buffer.
        * @since Series60 2.0
        * @param aBuffer buffer to be parsed.
        */
        void ParseL(TDesC8& aBuffer);
    
    };

#endif // C_CUPNPSSDPMESSAGE_H

// End of File