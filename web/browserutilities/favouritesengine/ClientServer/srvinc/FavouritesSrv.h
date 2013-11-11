/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Declaration of class FavouritesSrv and CFavouritesSrv
*      
*
*/


#ifndef FAVOURITES_SRV_H
#define FAVOURITES_SRV_H

//  INCLUDES

#include <e32base.h>

// FORWARD DECLARATION

class CTimeout;
class CBackupObserver;

// CLASS DECLARATION

/**
* Static methods to launch the Favourites Engine Server.
*/
class FavouritesSrv
    {
    public:     // New methods

        /**
        * Start the FavouritesEngine Server thread / process.
        * @return Error code.
        */
        static TInt Start();

        /**
        * Run the FavouritesEngine Server (entry function).
        * @return Error code.
        */
        IMPORT_C static TInt Run();

    };

/**
* Favourites Engine Server.
*/
NONSHARABLE_CLASS(CFavouritesSrv) : public CPolicyServer
    {
    public:     // Constructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @return The constructed server.
        */
        static CFavouritesSrv* NewL();

        /**
        * Destructor.
        */
        virtual ~CFavouritesSrv();

    public:     // New methods

        /**
        * A session is closed.
        */
        void SessionClosed();

        /**
        * Access received message.
        * @return Received message.
        */
        inline const RMessage2& ReceivedMessage() const;

        /**
        * Set received message.
        * @param aMessage Received message.
        */
        inline void ReceivedMessage( const RMessage2& aMessage );

        /**
        * Panic client.
        * @param aCode Panic code.
        */
        void PanicClient( TInt aCode );

    private:    // Constructors and destructor

        /**
        * Constructor.
        */
        CFavouritesSrv();

        /**
        * Second-phase constructor.
        */
        void ConstructL();

    private:    // From CServer2

        /**
        * Create new session.
        * @param aVersion Version.
        * @param aMessage Message (unused).
        * @return New session.
        */
        CSession2* NewSessionL
            ( const TVersion& aVersion, const RMessage2& aMessage ) const;
        
        /** 
        * Performs a custom security check.
        * @param	aMsg The message to check.
		* @param	aAction A reference to the action to take if the security
		*   check fails.
		* @param 	aMissing A reference to the list of security attributes missing
		*	from the checked process.
		* @return A value from TCustomResult.
		*/
        TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, 
        	TInt& aAction, TSecurityInfo& aMissing);

    private:    // Data

        RMessage2           iReceivedMessage; ///< Received message.
        CTimeout*           iExitTimer;       ///< Exit timer. Owned.
        CBackupObserver*    iBackupObserver;
};

#include "FavouritesSrv.inl"

#endif

// End of File
