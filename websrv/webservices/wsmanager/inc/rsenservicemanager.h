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








#ifndef R_SEN_SERVICE_MANAGER_CLIENT_H
#define R_SEN_SERVICE_MANAGER_CLIENT_H

// INCLUDES
#include <e32base.h>
#include <flogger.h>

#include "SenServiceManager.h" // typedef for RServiceDescriptionArray;

// FORWARD DECLARATIONS
class CSenXmlServiceDescription;
class CSenChunk;

// CLASS DECLARATION
class RSenServiceManager : public RSessionBase
    {
    public: // New functions
        
        RSenServiceManager();//RFileLogger* aLog);
        TInt Connect();
        TVersion Version() const;
        TInt InstallFramework(TDesC8& aMessage);
        TInt AssociateService(TDesC8& aServiceID, TDesC8& aProviderID);
        TInt DissociateService(TDesC8& aServiceID, TDesC8& aProviderID);
        TInt RegisterIdentityProvider(TDesC8& aMessage);
        TInt UnregisterIdentityProvider(TDesC8& aMessage);

        /**
        * Method returns list of matching servicedescriptions, as XML
        * and delimetted by TAB char.
        * @param aUri is the URI used as "search mask"
        * @param aDescriptions if successful, this will be a 
        *        descriptor pointing to RChunk allocated memory,
        *        which contains list of returned service descriptions.
        *        SM implementation should copy this data and then 
        *        close the related RChunk.
        * @param aHandle contains handle to newly allocated RChunk
        *        if method was succesful.
        * @return KErrNone or some system-wide error code otherwise
        */
        TInt ServiceDescriptionsByUriL(CSenChunk& aSenChunk);

        /**
        * Method returns list of matching servicedescriptions, as XML
        * and delimetted by TAB char.
        * @param aPatternAsXml is the XML representation of the
        *        service description, which is used as "search 
        *        mask".
        * @param aDescriptions if successful, this will be a 
        *        descriptor pointing to RChunk allocated memory,
        *        which contains list of returned service descriptions.
        *        SM implementation should copy this data and then 
        *        close the related RChunk.
        * @param aHandle contains handle to newly allocated RChunk
        *        if method was succesful.
        * @return KErrNone or some system-wide error code otherwise
        */
        TInt ServiceDescriptionsByPatternL(CSenChunk& aSenChunk);

        TInt UnregisterServiceDescription(TDesC8& aMessage);
        TInt RegisterServiceDescription(TDesC8& aMessage);
        
        TInt CredentialsL(CSenChunk& aSenChunk);
        
        TInt AddCredentialL(CSenChunk& aSenChunk);
        
        TInt RemoveCredentialsL(CSenChunk& aSenChunk);
        
        TInt IdentityProvidersL(CSenChunk& aSenChunk);
        TInt ConnectionID();
        
        //void SetLog(RFileLogger* aLog);
        //RFileLogger* Log() const;
        void SetChannel(TInt aChannel);

    private: // Data
        //RFileLogger* iLog;
        TInt         iTLSLogChannel;
    };

#endif //R_SEN_SERVICE_MANAGER_CLIENT_H

// End of File

