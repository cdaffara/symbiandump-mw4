/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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






 

#ifndef WSOvi_HANDLER_VALIDATE_CONTEXT_H
#define WSOvi_HANDLER_VALIDATE_CONTEXT_H

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
    _LIT8(KDiff,		"diff");
    }

// FORWARD DECLARATIONS
class CWSOviPlugin;

/**
 * Class:       
 */
class CWSOviHandlerContext : public CBase, public MSenHandlerContext
    {
    public:
        static CWSOviHandlerContext* NewL();
        ~CWSOviHandlerContext();

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
         * Function:   CWSOviHandlerContext
         *
         * Discussion: Perform the first phase of two phase construction
         */
        CWSOviHandlerContext();

        /**
         * Function:   ConstructL
         *
         * Discussion: Perform the second phase construction of a
         *             CImplementationClassPlus object.
         */
        void ConstructL();
            
    private://data
        MSenCoreServiceManager* iCoreServiceManager;
        CWSOviPlugin* iWSOviPlugin;
        RFileLogger* iLogger;
        HBufC8* iVersion;//owned
        TInt* iDiff;     //owned
    };


#endif // WSOvi_HANDLER_VALIDATE_CONTEXT_H


    
// END OF FILE

