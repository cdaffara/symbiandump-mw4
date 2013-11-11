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








#ifndef SEN_SECURITY_MECHANISMOBSERVER_H
#define SEN_SECURITY_MECHANISMOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <ecom.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CSenBaseIdentityManager;
class RFileLogger;

// CLASS DECLARATION
class CSenSenSecurityMechanismObserver :    public CActive
{
    public: // Constructors and destructor
    
        static CSenSenSecurityMechanismObserver* NewL(
                                            CSenBaseIdentityManager& aOwner);
        static CSenSenSecurityMechanismObserver* NewLC(
                                            CSenBaseIdentityManager& aOwner);

        virtual ~CSenSenSecurityMechanismObserver();

        // New functions
        
        void Start();
 
        // Functions from base classes
        
        virtual void RunL();
        virtual TInt RunError(TInt aError);

    protected: // New functions
        
        virtual void DoCancel();

    private:
        
        /**
        * C++ default constructor.
        */
        CSenSenSecurityMechanismObserver(CSenBaseIdentityManager& aOwner);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        RFileLogger* Log();

    private: // Data
        CSenBaseIdentityManager& iOwner;
        REComSession iEcomSession;
    };

#endif // SEN_SECURITY_MECHANISMOBSERVER_H

// End of File
