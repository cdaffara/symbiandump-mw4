/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares the upnpsettings class
 *
 *
*/


#ifndef C_CUPNPSETTING_H
#define C_CUPNPSETTING_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS
const TUid KHnCRUidUPnPStack = { 0x2000F881 }; //for parallel stack
const TUid KCRUidUPnPStack   = { 0x101F9784 };

// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION

/**
 *  Configuration API for UPnP stack operation
 *
 *  @lib serviceframework.lib
 *  @since Series 60 3.1
 */

class CUpnpSettings: public CBase
    {
public:
    
    /** Central repository value for IAP key */     
    static const TUint  KUPnPStackIapId = 1;
    
    /** Central repository value for customized UserAgent key */         
    static const TUint  KUPnPStackCustomizedUserAgent = 2;    

    /** Central repository value for file buffer size key */         
    static const TUint  KUPnPStackFileBufferSize = 3;    
    
    /** Central repository value for customized ServerHeader key */         
    static const TUint  KUPnPStackCustomizedServerHeader = 4;    

    /** Central repository value for customized M-Search string key */         
    static const TUint  KUPnPStackMSearchConfig = 5;    

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    IMPORT_C static CUpnpSettings* NewL(TUid aRepositoryUid);

    /**
    * Destructor.
    */
    virtual ~CUpnpSettings();

public: // New functions
    /**
    * Sets numeric value.
    * @param aId parameter id
    * @return error code
    */
    IMPORT_C TInt Set(TUint aId, TInt& aValue);

    /**
    *  Gets numeric value.
    * @param aId parameter id
    * @return error code
    */
    IMPORT_C TInt Get(TUint aId, TInt& aValue);

    /**
    * Gets numeric value.
    * @param aId parameter id
    * @return error code
    */
    IMPORT_C static TInt GetIapL();
    
    /**
    * Sets string value.
    * @param aId parameter id
    * @return error code
    */
    IMPORT_C TInt Set(TUint aId, const TDesC& aValue);

    /**
    * Sets string value.
    * @param aId parameter id
    * @return error code
    */
    IMPORT_C TInt SetL(TUint aId, const TDesC8& aValue);    
    
    /**
    * Gets string value.
    * @param aId parameter id
    * @return error code
    */
    IMPORT_C TInt Get(TUint aId, TDes& aValue);

    /**
    * Gets string value.
    * @param aId parameter id
    * @return string value
    */
    IMPORT_C HBufC8* GetL(TUint aId);    
    
    /**
    * Gets User-Agent string value.
    * @return string value
    */
    IMPORT_C static HBufC8* GetUserAgentL();

    /**
    * Gets Product string value.
    * @return string value
    */
    IMPORT_C static TInt GetFileBufferSizeL();
    
    /**
    * Gets Server header string value.
    * @return string value
    */
    IMPORT_C static HBufC8* GetServerHeaderL();

    /**
    * Gets Server header string value.
    * @return string value
    */
    static HBufC8* GetMSearchConfigurationL();

private:
    /**
    * By default Symbian 2nd phase constructor is private.
    * @param aRepositoryId  
    */   
    void ConstructL(TUid aRepositoryId);

private: 
    // Central repository client
    CRepository*        iRepository;
    };

#endif // C_CUPNPSETTING_H

// End of File