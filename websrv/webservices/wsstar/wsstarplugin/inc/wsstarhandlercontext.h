/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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






 

#ifndef WSSTAR_HANDLER_VALIDATE_CONTEXT_H
#define WSSTAR_HANDLER_VALIDATE_CONTEXT_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "msenhandlercontext.h"

// CONSTANTS
namespace HandlerContextKey
    {
    _LIT8(KSIF,         "SIF");
    _LIT8(KLogger,      "Logger");
    _LIT8(KVersion,     "version");
    }

// FORWARD DECLARATIONS
class CWSStarPlugin;

/**
 * Class:       
 */
class CWSStarHandlerContext : public CBase, public MSenHandlerContext
    {
    public:
        static CWSStarHandlerContext* NewL();
        ~CWSStarHandlerContext();

        //from HandlerContext    
        virtual TInt Add(MSenCoreServiceManager& aValue);
        virtual MSenCoreServiceManager* GetSenCoreServiceManager();
            
        //from MContext    
        virtual SenContext::TContextType Type() const;        
        virtual SenContext::TContextDirection Direction() const;
        virtual TInt Add(const TDesC8& aKey, const TDesC8& aValue);
        virtual TInt Update(const TDesC8& aKey, const TDesC8& aValue);
        virtual const TDesC8* GetDesC8L(const TDesC8& aKey);
        virtual TInt Add(const TDesC8& aKey, TInt aValue);
        virtual TInt Update(const TDesC8& aKey, TInt aValue);
        virtual const TInt* GetIntL(const TDesC8& aKey);
        virtual TInt Add(const TDesC8& aKey, CSenElement* aValue);
        virtual TInt Update(const TDesC8& aKey, CSenElement* aValue);
        virtual const CSenElement* GetSenElementL(const TDesC8& aKey) ;
        virtual TInt Add(const TDesC8& aKey, TAny* aValue);
        virtual TInt Update(const TDesC8& aKey, TAny* aValue) ;
        virtual TAny* GetAnyL(const TDesC8& aKey);		//codescannerwarnings
        virtual const CSenXmlReader* GetParser();
        virtual TInt Remove(const TDesC8& aKey);         
        virtual TInt Count() const;
        virtual TPtrC8 KeyAtL(TInt aIndex);
        virtual void Reset() ;
        
        inline virtual TInt SetDirection( SenContext::TContextDirection /* aDirection */ ) { return KErrNotSupported; } // not implemented

            
    protected:
        /**
         * Function:   CWSStarHandlerContext
         *
         * Discussion: Perform the first phase of two phase construction
         */
        CWSStarHandlerContext();

        /**
         * Function:   ConstructL
         *
         * Discussion: Perform the second phase construction of a
         *             CImplementationClassPlus object.
         */
        void ConstructL();
            
    private://data
        MSenCoreServiceManager* iCoreServiceManager;
        CWSStarPlugin* iWSStarPlugin;
        RFileLogger* iLogger;
        HBufC8* iVersion;//owned
    };


#endif // WSSTAR_HANDLER_VALIDATE_CONTEXT_H


    
// END OF FILE

