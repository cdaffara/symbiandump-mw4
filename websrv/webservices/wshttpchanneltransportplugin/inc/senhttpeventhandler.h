/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Header declaration
*
*/








#ifndef WSF_HTTP_EVENT_HANDLER_H
#define WSF_HTTP_EVENT_HANDLER_H

// INCLUDES
#include <http/mhttptransactioncallback.h>
#include <flogger.h>

// FORWARD DECLARATIONS
class CSenHttpChannelImpl;

// CLASS DECLARATION
class CSenHttpEventHandler : public CBase, public MHTTPTransactionCallback
    {
    public: // Constructors and destructor
    
        static CSenHttpEventHandler* NewLC(CSenHttpChannelImpl* aChannel);//,
                                           //RFileLogger* aLog);
        static CSenHttpEventHandler* NewL(CSenHttpChannelImpl* aChannel);//,
                                          //RFileLogger* aLog);

        virtual ~CSenHttpEventHandler();

        // Functions from base classes
    
        // from MHTTPTransactionCallback
        virtual void MHFRunL(RHTTPTransaction aTransaction,
                             const THTTPEvent& aEvent);
        virtual TInt MHFRunError(TInt aError,
                                 RHTTPTransaction aTransaction,
                                const THTTPEvent& aEvent);

    protected:
    
        /**
        * C++ default constructor.
        */
        CSenHttpEventHandler(CSenHttpChannelImpl* aChannel);//,
                             //RFileLogger* aLog);
                             
        /**
        * By default Symbian 2nd phase constructor is private.
        */                   
        void ConstructL();

    private:
        
        //RFileLogger* Log() const;

    private: // Data
        CSenHttpChannelImpl* iHttpChannel;
        //RFileLogger* iLog;
        TInt iTries;
    };

#endif // WSF_HTTP_EVENT_HANDLER_H

// End of file
