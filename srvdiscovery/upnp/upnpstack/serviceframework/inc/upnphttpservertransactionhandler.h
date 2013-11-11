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
* Description:  CUpnpHttpServerTransactionHandler declaration.
*
*/

#ifndef UPNPDLNAFILTER_H
#define UPNPDLNAFILTER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h> 
#include "upnphttpservertransactioncreator.h"

// FORWARD DECLARATION
class CUpnpHttpMessage;
class CUpnpDeviceImplementationBase;
class TInetAddr;

// CLASS DECLARATION

/**
 *  Creates http server transactions and provides methods 
 *  for resolving source file by request URL
 *  Served files are: device and service description xml's and icons.
 *  
 */
NONSHARABLE_CLASS( CUpnpHttpServerTransactionHandler ) : public CBase,
        public MUpnpHttpServerTransactionCreator
    {
public:
    // Public Constructors and destructor

    enum TDescriptionType 
        {
        EDevice,
        EService,
        EIcon,
        ENotFound
        };
    
    /**
     * Destructor.
     */
    ~CUpnpHttpServerTransactionHandler();

    /**
     * Two-phased constructor.
     */
    static CUpnpHttpServerTransactionHandler* NewL( CUpnpDeviceImplementationBase& aDevice );

    /**
     * Two-phased constructor.
     */
    static CUpnpHttpServerTransactionHandler* NewLC( CUpnpDeviceImplementationBase& aDevice );

    /**
     * Checks correctness of path to, requested in HTTP message, file and 
     *  changes its format to be compliant with Symbian's file system.
     * @since Series60 2.6
     * @param aMsg HTTP message containing file request.
     * @param aPathWithNewMethod reference which is filled with formated path 
     *  to requested file.
     **/
    void FormatPathL( const TDesC8& aUri, TDes& aPathWithNewMethod );
            
    /**
     * Opens file depending on specified uri and IP address
     *
     * @since Series60 MCL
     * @param aFile File to open
     * @param aUri request uri
     * @param aInetAdress requestor IP address
     * @return error code
     */
    TInt GetFileL( RFile& aFile,
            const TDesC8& aUri, const TInetAddr& aInetAdress );

private:  // from MUpnpHttpServerTransactionCreator

    virtual void NewTransactionL( const TDesC8& aMethod, const TDesC8& aUri, 
        const TInetAddr& aSender, CUpnpHttpServerTransaction*& aResultTrans );


private:

    /**
     * Constructor for performing 1st stage construction
     */
    CUpnpHttpServerTransactionHandler( CUpnpDeviceImplementationBase& aDevice );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    
    TDescriptionType MatchType( const TDesC8& aUri );
   
private:
    CUpnpDeviceImplementationBase& iDevice;
    
    };

#endif // UPNPDLNAFILTER_H
// End Of File
