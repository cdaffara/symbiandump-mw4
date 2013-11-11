/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the Browser Control's DataLoadInfo
*
*/



#ifndef BRCTLDATALOADINFO_H
#define BRCTLDATALOADINFO_H

//  INCLUDES
#include <BrCtlInterface.h>

//  CONSTANTS

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

NONSHARABLE_CLASS(CDataLoadInfo) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDataLoadInfo* NewL(MBrCtlDataLoadSupplier* aBrCtlDataLoadSupplier, 
			TBool aOwnDataLoadSupplier);
        
        /**
        * Destructor.
        */
        virtual ~CDataLoadInfo();

    private:

        /**
        * C++ default constructor.
        */
        CDataLoadInfo();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( MBrCtlDataLoadSupplier* aBrCtlDataLoadSupplier, TBool aOwnDataLoadSupplier );

    public:     // Data       
        MBrCtlDataLoadConsumer* iDataConsumer;
        MBrCtlDataLoadSupplier* iDataSupplier;
		TBool iOwnsDataSupplier;
    };


NONSHARABLE_CLASS(CBrCtlDataLoadSupplier) :  public CBase, public MBrCtlDataLoadSupplier 
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CBrCtlDataLoadSupplier* NewL(CDataLoadInfo& aDataLoadInfo);
        
        /**
        * Destructor.
        */
        ~CBrCtlDataLoadSupplier();

    public: // New functions

        /**
        * Continue with passing the data to the loaders 
        * @since 3.1
        * @param aData The data to be loaded
        * @return void
        */
        void DataLoad(const TDesC8& aData);


    public: // Functions from base classes
        /**
        * From MBrCtlDataLoadSupplier Cancel the load operation
        * @since 3.1
        * @return void
        */
        virtual void CancelLoad();
 
    private:
    
        /**
        * C++ default constructor.
        */
        CBrCtlDataLoadSupplier(CDataLoadInfo& aDataLoadInfo);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:   //  Data

        CDataLoadInfo* iDataLoadInfo;
    };

#endif      // BRCTLDATALOADINFO_H
            
// End of File
