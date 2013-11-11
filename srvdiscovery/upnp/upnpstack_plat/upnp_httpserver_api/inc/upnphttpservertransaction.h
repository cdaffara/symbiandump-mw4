/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpHttpServerTransaction declaration.
*
*/

#ifndef UPNPHTTPSERVERTRANSACTION_H_
#define UPNPHTTPSERVERTRANSACTION_H_

// INCLUDES
#include <f32file.h> 
#include <e32base.h>
#include <in_sock.h>


// FORWARD DECLARATIONS

class CUpnpHttpSession;
class CUpnpHttpMessage;
class CUpnpDlnaFilter;

// CLASS DECLARATION

/**
 * Base abstract class for HTTP server transactions.
 * Client has to derive from this class to implement required 
 * behavior.
 * 
 * @lib dlnawebserver.lib
 * @since S60 5.1
 */
class CUpnpHttpServerTransaction : public CBase
    {
public:

    /** Http Event type enumerator */
    enum TUpnpHttpServerEvent
        {
        EOnRequestStart = 0, 
        EOnResponseStart, 
        EOnComplete
        };
    
    /**
     * Destructor.
     */
    IMPORT_C ~CUpnpHttpServerTransaction();

    /**
     * Handler for transaction event.
     *    
     * @since S60 5.1
     * @param aEvent server event
     */
    virtual void OnCallbackL( TUpnpHttpServerEvent aEvent ) = 0;

 
    /**
     * Returns response header buffer.
     * 
     * @since S60 5.1
     * @return descriptor with data
     */
    IMPORT_C const TDesC8& QueryResponseHeader();

    /**
     * Returns request header value.
     * When header not found, returns KNullDesC8.
     * 
     * @since S60 5.1
     * @param aHeaderName header name
     * @return descriptor with data
     */
    IMPORT_C TDesC8& QueryRequestHeader( const TDesC8& aHeaderName );

    /**
     * Adds pair : name - value to response header
     * To add HTTP method and HTTP code, pass it as aValue parameter
     * and as aName pass KNullDesC8. 
     * 
     * @since S60 5.1
     * @param aName header name
     * @param aValue header value
     */    
    IMPORT_C void AddResponseHeaderL( const TDesC8& aName, const TDesC8& aValue );
    
    // For internal usage only
    IMPORT_C void SetRequest( CUpnpHttpMessage* aRequest );
    IMPORT_C TInt Error();   
    RFile& DataSource();
    RFile& DataSink();

protected:

    /**
     * C++ constructor.
     */
    IMPORT_C CUpnpHttpServerTransaction();
    
    /**
     * Sets data source.
     * 
     * @since S60 5.1
     * @param aFile File handle, may not be on cleanupstack
     */
    IMPORT_C void SetDataSourceL( RFile& aFile );

    /**
     * Sets data sink.
     * 
     * @since S60 5.1
     * @param aFile File handle, may not be on cleanupstack
     */
    IMPORT_C void SetDataSinkL( RFile& aFile );

    /**
     * Sets http code.
     * 
     * @since S60 5.1
     * @param aCode http code
     */
    IMPORT_C void SetHttpCode( TInt aCode );
        
private:    // data
    
    // File handle.
    RFile iFile;
    
    // Buffer for header data.
    RBuf8 iHeaderBuffer;
    
    // Http code.
    TInt iHttpCode;
    
    // Http message.
    CUpnpHttpMessage* iRequest;    
    };

#endif /* UPNPHTTPSERVERTRANSACTION_H_ */
