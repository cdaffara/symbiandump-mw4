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
* Description:        
*
*/








// INCLUDE FILES
#include "senservicemanagerimpl.h"

#include <SenIdentityProvider.h>
#include <SenXmlUtils.h>
#include <SenServiceConnection.h>  // WSF error codes

#include "senservicemanagerdefines.h" // IPC enumerations
#include "sendebug.h" // filelogging MACROs
#include "senlogger.h"
#include "senguidgen.h"
#include "senchunk.h"
#include <SenCredential2.h>
#include "senxmldebug.h"

namespace
    {
    const TInt KMaxChunkNumDecimals = 64;
    _LIT8(KIdentityProviderName, "IdentityProvider");
    }


CSenServiceManagerImpl* CSenServiceManagerImpl::NewL()
    {
    CSenServiceManagerImpl* pNew = NewLC();
    CleanupStack::Pop();
    return(pNew) ;
    }

CSenServiceManagerImpl* CSenServiceManagerImpl::NewLC()
    {
    CSenServiceManagerImpl* pNew = new (ELeave) CSenServiceManagerImpl(NULL);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

CSenServiceManagerImpl* CSenServiceManagerImpl::NewL(MSenAuthenticationProvider& aAuthProvider)
    {
    CSenServiceManagerImpl* pNew = NewLC(aAuthProvider);
    CleanupStack::Pop();
    return(pNew) ;
    }

CSenServiceManagerImpl* CSenServiceManagerImpl::NewLC(MSenAuthenticationProvider& aAuthProvider)
    {
    CSenServiceManagerImpl* pNew = new (ELeave) CSenServiceManagerImpl(&aAuthProvider);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

void CSenServiceManagerImpl::ConstructL()
    {
    TInt connAttemp = 0;
    TInt connErr = KErrGeneral;

    ipGenerator = CSenGuidGen::NewL();
    ipChunkName = HBufC::NewL(KSenUuidMaxLength+KMaxChunkNumDecimals);

    TLSLOG_OPEN(KSenServiceManagerLogChannel, KSenServiceManagerLogLevel, KSenServiceManagerLogDir, KSenServiceManagerLogFile);
    connErr = iConnection.Connect();

    while ( (connErr == KErrServerBusy || connErr == KErrServerTerminated) 
                     & connAttemp < KSenMaxClientConnectionOpeningAttempts )
        {
        User::After(1000000); // wait for a second if server has been shut down
        connErr = iConnection.Connect();
        connAttemp++;
        }
    if ( connErr != KErrNone )
        {
        User::Leave( connErr );
        }

    iConnectionID = iConnection.ConnectionID();
    iConnection.SetChannel(KSenServiceManagerLogChannelBase+iConnectionID);   
    //iTlsLogChannel = KSenServiceManagerLogChannelBase + iConnectionID;


#ifdef _SENDEBUG
    RThread thread;
    RProcess process;
    TFileName logFile;
    logFile.Append( KSenServiceManagerLogFile().Left(KSenServiceManagerLogFile().Length()-4) ); // exclude ".log" file extension
    logFile.AppendNum( iConnectionID );
    logFile.Append( KSenUnderline );
    logFile.Append( process.Name().Left(32));
    logFile.Append( KSenUnderline );
    logFile.Append( thread.Name().Left(20));
    logFile.Append( KSenServiceManagerLogFile().Right(4) ); // postfix with ".log" file extension

    // Open connection to the file logger server
    TLSLOG_OPEN(KSenServiceManagerLogChannelBase+iConnectionID, KSenServiceManagerLogLevel, KSenServiceManagerLogDir, logFile);
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManager::ConstructL - Log file opened");
    TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("- Connection ID: (%d)"), iConnectionID));
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"- Service Manager client ready.");
#endif

    iReader = CSenXmlReader::NewL(KXmlParserMimeType); // use libxml2 sax parser
    }

CSenServiceManagerImpl::CSenServiceManagerImpl(MSenAuthenticationProvider* apAuthProvider)
:   iConnection(),//&iLog),
    iReader(NULL),
    iInBuffer(NULL,0),
    iOutBuffer(NULL,0),
    iInitialized(EFalse),
    ipGenerator(NULL),
    ipChunkName(NULL),
    iChunkNameNumber(0),
    ipAuthProvider(apAuthProvider),
    iConnectionID(KErrNotReady) //,
//    iTlsLogChannel(KSenServiceManagerLogChannelBase)
    {
    CActiveScheduler::Add(this);
    }

CSenServiceManagerImpl::~CSenServiceManagerImpl()
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::~CSenServiceManagerImpl");
    Cancel(); // Causes call to DoCancel()
    iConnection.Close();

    delete iReader;
    delete iOutBuf;
    delete ipGenerator;
    delete ipChunkName;

    // Close the log file and the connection to the server.
    TLSLOG(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,(_L("Log file closed.")));
    TLSLOG_CLOSE (KSenServiceManagerLogChannelBase+iConnectionID);
    TLSLOG_CLOSE (KSenServiceManagerLogChannel);
    }

void CSenServiceManagerImpl::RunL()
    {

    }

void CSenServiceManagerImpl::DoCancel()
    {
    TLSLOG(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceManagerImpl::DoCancel")));
    }

TInt CSenServiceManagerImpl::Identifier()
    {
    return iConnectionID;    
    }

/*
RFileLogger* CSenServiceManagerImpl::Log() const
    {
    return (RFileLogger*) &iLog;
    }
*/
TPtrC CSenServiceManagerImpl::NextChunkNameL()	//Codescannerwarnings
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::NextChunkNameL");
    if(ipChunkName)
        {
        iChunkNameNumber++;
        TPtr chunkname = ipChunkName->Des();
        chunkname.Zero();

        HBufC* pRandomGuid = ipGenerator->GetRandomGuidLC();
        TPtr idBuffer = pRandomGuid->Des();
        if(idBuffer.Length()>KSenUuidPrefixLength)
            {
            chunkname.Append(idBuffer.Right(idBuffer.Length()-KSenUuidPrefixLength));
            }
        CleanupStack::PopAndDestroy(); // pRandomGuid
        chunkname.AppendNum(iChunkNameNumber);

        // Remove dashes
         _LIT(KDash, "-");
        TInt pos = chunkname.Find(KDash);
        while (pos != KErrNotFound)
            {
            chunkname.Replace(pos, KDash().Length(), KNullDesC);
            pos = chunkname.Find(KDash);
            }
        TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KNormalLogLevel , _L("CSenServiceManagerImpl::NextChunkNameL(): %S"), &chunkname));
        return chunkname;
        }
    else
        {
        return KNullDesC();
        }  
    }



// Parameter aUri is contract of the service
TInt CSenServiceManagerImpl::ServiceDescriptionsL(const TDesC8& aUri, 
                                                  RServiceDescriptionArray& aList)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::ServiceDescriptionsL(aUri, aList)");
    TInt retVal(KErrNone);
    TPtrC8 descriptions;

    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());	//Codescannerwarnings
    //pSenChunk->SetLogger(Log());
    retVal = pSenChunk->CreateChunk();
    if (retVal == KErrNone)
        {
        retVal = pSenChunk->DescToChunk(aUri);
        if (retVal == KErrNone)
            {
            retVal = iConnection.ServiceDescriptionsByUriL(*pSenChunk);
            if (retVal == KErrNone)
                {
                retVal = pSenChunk->DescFromChunk(descriptions);
                if(retVal == KErrNone)
                    {
                    retVal = ExtractServiceDescriptionsL(descriptions, aList);
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk); // Close chunk
    TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceManagerImpl::ServiceDescriptionsL(aUri, aList) returns [%d]"), retVal));
    return retVal;
    }

TInt CSenServiceManagerImpl::ServiceDescriptionsL(MSenServiceDescription& aSD,
                                                  RServiceDescriptionArray& aList)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::ServiceDescriptionsL(aSD, aList)");
    TInt retVal(KErrNone);
    
    HBufC8* pPattern = aSD.AsXmlL();
    CleanupStack::PushL(pPattern);
    
    TPtrC8 descriptions;

    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());	//Codescannerwarnings
    //pSenChunk->SetLogger(Log());
    retVal = pSenChunk->CreateChunk();
    if (retVal == KErrNone)
        {
        retVal = pSenChunk->DescToChunk(*pPattern);
        if (retVal == KErrNone)
            {
            retVal = iConnection.ServiceDescriptionsByPatternL(*pSenChunk);
            if(retVal == KErrNone)
                {
                retVal = pSenChunk->DescFromChunk(descriptions);
                if(retVal == KErrNone)
                    {
                    retVal = ExtractServiceDescriptionsL(descriptions, aList);
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk); // Close chunk

    // In any case, destroy the pattern as XML buffer:
    CleanupStack::PopAndDestroy(); // pPattern
		TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceManagerImpl::ServiceDescriptionsL(aSD, aList) returns [%d]"), retVal));
    return retVal;
    }

TInt CSenServiceManagerImpl::RegisterServiceDescriptionL(MSenServiceDescription& aSD)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::RegisterServiceDescriptionL(aSD)");
    // do not register if both endpoint and contract are 
    // missing
    if(aSD.Endpoint().Length()==0 && aSD.Contract().Length()==0)
        {
        return KErrSenNoContractNoEndPoint;
        }
    
    else 
        {
        _LIT8(KProviderID,"ProviderID");
        HBufC8* pAsXml = NULL;
        if ( aSD.DescriptionClassType() ==
                MSenServiceDescription::EIdentityProvider )
            {
            TLSLOG(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,(_L("RegisterServiceDescriptionL with EIdentityProvider ")));
        
            CSenIdentityProvider* pIdP = (CSenIdentityProvider*)&aSD;
        
            CSenXmlServiceDescription* pSD = CSenXmlServiceDescription::NewLC(
                    pIdP->Endpoint(),
                    pIdP->Contract()  );
        
            pSD->SetFrameworkIdL( pIdP->FrameworkId() );
            pSD->AsElement().AddElementL(KProviderID).SetContentL(
                                                            pIdP->ProviderID() );	//CodeScannerWarnings
        
            pAsXml = pSD->AsXmlL(); // leaves if no memory available
            CleanupStack::PopAndDestroy(); // pSD
            }
        else    // ServiceDescription
            {
            pAsXml = aSD.AsXmlL(); // leaves if no memory available
            }
        
        CleanupStack::PushL(pAsXml); // must be the first line after if-clause
        TPtr8 ptr = pAsXml->Des();
        // now register the XML string (descriptor) to WSF Symbian server:
        TInt retVal(iConnection.RegisterServiceDescription(ptr));
        CleanupStack::PopAndDestroy(); // pAsXml
        TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceManagerImpl::RegisterServiceDescriptionL(aSD) returns [%d]"), retVal));
        return retVal;
        }
    }

TInt CSenServiceManagerImpl::UnregisterServiceDescriptionL(
                                                MSenServiceDescription& aSD)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::UnregisterServiceDescriptionL(aSD)");
    // do not unregister if both endpoint and contract are 
    // missing
    if(aSD.Endpoint().Length()==0 && aSD.Contract().Length()==0)
        {
        return KErrSenNoContractNoEndPoint;
        }
    else 
        {
        HBufC8* pAsXml = NULL;
        if ( aSD.DescriptionClassType() ==
            MSenServiceDescription::EIdentityProvider )
            {
            TLSLOG(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,(_L("UnregisterServiceDescriptionL with EIdentityProvider ")));
            
            CSenIdentityProvider* pIdP = (CSenIdentityProvider*)&aSD;
            
            CSenXmlServiceDescription* pSD = 
                CSenXmlServiceDescription::NewLC(pIdP->Endpoint(),
                                                 pIdP->Contract());
            
            pSD->SetFrameworkIdL( pIdP->FrameworkId() );
            if(pIdP->ProviderID().Length()>0)
                {
		        _LIT8(KProviderID,"ProviderID");
                CSenElement& providerIdElement = 
                    pSD->AsElement().AddElementL(KProviderID);	//CodeScannerWarnings
                providerIdElement.SetContentL(pIdP->ProviderID());
                }
            pAsXml = pSD->AsXmlL(); // leaves if no memory available
            CleanupStack::PopAndDestroy(); // pSD
            }
        else    // ServiceDescription
            {
            pAsXml = aSD.AsXmlL(); // leaves if no memory available
            }
        
        CleanupStack::PushL(pAsXml); // must be the first line after if-clause
        TPtr8 ptr = pAsXml->Des();
        TInt retVal(iConnection.UnregisterServiceDescription(ptr));
        CleanupStack::PopAndDestroy(); // pAsXml
        TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceManagerImpl::UnregisterServiceDescriptionL(aSD) returns [%d]"), retVal));
        return retVal;
        }
    }
// comment: remote developer interface to identity manager
TInt CSenServiceManagerImpl::RegisterIdentityProviderL(
                                            CSenIdentityProvider& aProvider)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::RegisterIdentityProviderL(aProvider)");
    HBufC8* provider = aProvider.AsXmlL();
    TPtr8 ptr = provider->Des();
    TInt retVal = iConnection.RegisterIdentityProvider(ptr);
    delete provider;
    provider = NULL;
	TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::RegisterIdentityProviderL(aProvider) Completed");    
    return retVal;
    }

TInt CSenServiceManagerImpl::UnregisterIdentityProviderL(
                                            CSenIdentityProvider& aProvider)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::UnregisterIdentityProviderL(aProvider)");
    HBufC8* provider = aProvider.AsXmlL();
    TPtr8 ptr = provider->Des();
    TInt retVal(iConnection.UnregisterIdentityProvider(ptr));
    delete provider;
    return retVal;
    }

TInt CSenServiceManagerImpl::AssociateServiceL(const TDesC8& aServiceID,
                                               const TDesC8& aProviderID)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::AssociateServiceL(aServiceID, aProviderID)");
    if (aServiceID.Length() <= 0 || aProviderID.Length() <= 0)
        {
        return KErrArgument;
        }
    TInt retVal( iConnection.AssociateService(  (TDesC8&)aServiceID,
                                                (TDesC8&)aProviderID) );
    return retVal;
    }


TInt CSenServiceManagerImpl::DissociateServiceL(const TDesC8& aServiceID,
                                                const TDesC8& aProviderID)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::DissociateServiceL(aServiceID, aProviderID)");
    if (aServiceID.Length() <= 0 || aProviderID.Length() <= 0)
        {
        return KErrArgument;
        }
    TInt retVal( iConnection.DissociateService( (TDesC8&)aServiceID,
                                                (TDesC8&)aProviderID) );
    return retVal;
    }

TInt CSenServiceManagerImpl::InstallFrameworkL(const TDesC8& aFrameworkId)
    {
    TLSLOG(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceManager::InstallFrameworkL")));
    TLSLOG(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,(aFrameworkId));
    if (aFrameworkId.Length() <= 0)
        {
        return KErrArgument;
        }
    TInt retVal( iConnection.InstallFramework((TDesC8&)aFrameworkId) );

    return retVal;
    }

TInt CSenServiceManagerImpl::ExtractServiceDescriptionsL(TPtrC8& aSDs, 
                                                          RServiceDescriptionArray& aList)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::ExtractServiceDescriptionsL");
    TInt retVal(KErrNotFound);

    
    if (aSDs.Length() > 0)
        {
        TPtrC8 single;
        TPtrC8 rest;
        _LIT8(KTab, "\t");
        TBool loop(ETrue);
        TInt pos(-1);
        CSenXmlServiceDescription* desc = NULL;
        rest.Set(aSDs);

        while (loop)
            {
            pos = rest.Find(KTab());
            if (pos > 0)
                {
                desc = CSenXmlServiceDescription::NewLC();
                iReader->SetContentHandler(*desc);
                desc->SetReader(*iReader);
                single.Set(rest.Left(pos));

                TInt leaveCode(KErrNone);
                TRAP(leaveCode, desc->ParseL(single));
                if(leaveCode!=KErrNone)
                    {
                    TInt count(aList.Count());
                    TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("-desc->ParseL(#%d): parsing failed: %d"), count+1, leaveCode));
                        
                    CleanupStack::PopAndDestroy(); // desc
                    if(count == 0)
                        {
                        retVal = KErrNotFound; // could not parse single service description
                        }

                    break;
                    }

                rest.Set(rest.Mid(pos+1));  // Check
                retVal = aList.Append(desc);
                if(retVal == KErrNone)
                    {
                    CleanupStack::Pop(); // desc
                    }
                else
                    {
                    CleanupStack::PopAndDestroy();
                    }
                desc = NULL;
                }
            else
                {
                desc = CSenXmlServiceDescription::NewLC();
                iReader->SetContentHandler(*desc);
                desc->SetReader(*iReader);

                TInt leaveCode(KErrNone);
                TRAP(leaveCode, desc->ParseL(rest));
                if(leaveCode!=KErrNone)
                    {
                    TInt count(aList.Count());
                    TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("-desc->ParseL(#%d): parsing failed: %d"), count+1, leaveCode));
                        
                    CleanupStack::PopAndDestroy(); // desc
                    if(count == 0)
                        {
                        retVal = KErrNotFound; // could not parse single service description
                        }

                    break;
                    }
                retVal = aList.Append(desc);
                if(retVal == KErrNone)
                    {
                    CleanupStack::Pop(); // desc
                    }
                else
                    {
                    CleanupStack::PopAndDestroy();
                    }
                desc = NULL;
                break;
                }

            if (rest.Length() == 0 )
                {
                break;
                }

            }
        } //  END OF: if aSD.Length() > 0
    return retVal;
    }

TInt CSenServiceManagerImpl::ExtractCredentialsL(TPtrC8& aCredentials, 
                                                 RCredentialArray& aList,
                                                 RCredentialPropertiesArray& aCredentialPropertiesList)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::ExtractCredentialsL");
    TInt retVal(KErrNotFound);
    
    if ( aCredentials.Length() > 0 )
        {
        TPtrC8 single;
        TPtrC8 rest;
        _LIT8(KTab, "\t");
        _LIT8(KNewline, "\n");
        _LIT8(KNoProperties, "-");

        TInt pos(KErrNone);
        CSenCredential2* pCredential = NULL;
        rest.Set(aCredentials);
        CSenParser* pParser = CSenParser::NewLC();

        while ( pos != KErrNotFound && rest.Length() > 0 )
            {
            pos = rest.Find(KTab());
            if (pos > 0)
                {
                single.Set(rest.Left(pos));
                }
            else
                {
                single.Set(rest);
                }

            if ( single.Length() > 0 )
                {
                pCredential = CSenCredential2::NewLC();

                TInt leaveCode(KErrNone);
                TRAP(leaveCode, pParser->ParseL(single, *pCredential));
                if ( leaveCode != KErrNone )
                    {
                    TInt count(aList.Count());
                    TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("- Credential(#%d): parsing failed: %d"), count+1, leaveCode));
                       
                    CleanupStack::PopAndDestroy(); // desc
                    if ( count == 0 )
                        {
                        retVal = KErrNotFound; // could not parse single credential
                        }

                    break;
                    }

                retVal = aList.Append(pCredential);
                if(retVal == KErrNone)
                    {
                    CleanupStack::Pop(); // desc
                    }
                else
                    {
                    CleanupStack::PopAndDestroy();
                    }

                rest.Set(rest.Mid(pos+1));  // Check

                pos = rest.Find(KNewline());
                if (pos > 0)
                    {
                    single.Set(rest.Left(pos));
                    }
                else
                    {
                    single.Set(rest);
                    }
                    
                if ( single == KNoProperties )
                    {
                    aCredentialPropertiesList.Append(NULL);
                    }
                else
                    {
                    if ( single.Length() > 0 )
                        {
                        CSenXmlProperties* pCredentialProperties = CSenXmlProperties::NewLC();
                        pCredentialProperties->SetReader(*iReader);
                        leaveCode = KErrNone;
                        TRAP(leaveCode, pCredentialProperties->ReadFromL(single));
                        if ( leaveCode != KErrNone )
                            {
                            TInt count(aList.Count());
                            TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("- CredentialProperties(#%d): parsing failed: %d"), count+1, leaveCode));
                                
                            CleanupStack::PopAndDestroy(pCredentialProperties);
                            if ( count == 0 )
                                {
                                retVal = KErrNotFound; // could not parse single credential
                                }
                                
                            retVal = aCredentialPropertiesList.Append(NULL);
                            break;
                            }
                        
                        retVal = aCredentialPropertiesList.Append(pCredentialProperties);
                        if(retVal == KErrNone)
                            {
                            CleanupStack::Pop(pCredentialProperties);
                            }
                        else
                            {
                            CleanupStack::PopAndDestroy(pCredentialProperties);
                            }
                        }
                    else
                        {
                        aCredentialPropertiesList.Append(NULL);
                        }
                    }
                    
                rest.Set(rest.Mid(pos+1));  // Check

                pCredential = NULL;
                }
            else
                {
                pos = KErrNotFound;
                }
            }

        CleanupStack::PopAndDestroy(pParser);
        } //  END OF: if aCredentials.Length() > 0
    return retVal;
    }

TInt CSenServiceManagerImpl::CredentialsL(const TDesC8& aEndpoint,
                                          RCredentialArray& aCredentials,
                                          RCredentialPropertiesArray& aCredentialProperties)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::CredentialsL(aEndpoint, aCredentials, aCredentialProperties)");
    TInt retVal(KErrNone);
    
    CSenXmlServiceDescription* pServDesc = CSenXmlServiceDescription::NewLC();
    pServDesc->SetEndPointL(aEndpoint);
    
    retVal = CredentialsL(*pServDesc, aCredentials, aCredentialProperties);
    
    CleanupStack::PopAndDestroy(pServDesc);
    
    return retVal;
    }

TInt CSenServiceManagerImpl::CredentialsL(const TDesC8& aEndpoint,
                                          const CSenIdentityProvider& aIdP,
                                          RCredentialArray& aCredentials,
                                          RCredentialPropertiesArray& aCredentialProperties)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::CredentialsL(aEndpoint, aIdP, aCredentials, aCredentialProperties)");
    TInt retVal(KErrNone);
    TPtrC8 credentials;
    
    CSenXmlServiceDescription* pServDesc = CSenXmlServiceDescription::NewLC();
    pServDesc->SetEndPointL(aEndpoint);
    HBufC8* pPatternAsXml = pServDesc->AsXmlL();
    CleanupStack::PopAndDestroy(pServDesc);
    CleanupStack::PushL(pPatternAsXml);
    if( &aIdP == NULL )
    {
      return KErrArgument;  
    }

    HBufC8* pIdPAsXml = ((CSenIdentityProvider&)aIdP).AsXmlL();
    CleanupStack::PushL(pIdPAsXml);

    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());	//Codescannerwarnings
    //pSenChunk->SetLogger(Log());
    retVal = pSenChunk->CreateChunk();
    if (retVal == KErrNone)
        {
        retVal = pSenChunk->DescsToChunk(*pPatternAsXml, *pIdPAsXml);
        if (retVal == KErrNone)
            {
            retVal = iConnection.CredentialsL(*pSenChunk);
            if (retVal == KErrNone)
                {
                retVal = pSenChunk->DescFromChunk(credentials);
                if(retVal == KErrNone)
                    {
                    retVal = ExtractCredentialsL(credentials, aCredentials, aCredentialProperties);
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk);
    
    CleanupStack::PopAndDestroy(pIdPAsXml);
    CleanupStack::PopAndDestroy(pPatternAsXml);
    
    return retVal;    
    }

TInt CSenServiceManagerImpl::CredentialsL(const TDesC8& aEndpoint,
                                          const TDesC8& aUserName,
                                          const TDesC8& aPassword,
                                          RCredentialArray& aCredentials,
                                          RCredentialPropertiesArray& aCredentialProperties)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::CredentialsL(aEndpoint, aUserName, aPassword, aCredentials, aCredentialProperties)");
    TInt retVal(KErrNone);
    TPtrC8 credentials;
    
    CSenXmlServiceDescription* pServDesc = CSenXmlServiceDescription::NewLC();
    pServDesc->SetEndPointL(aEndpoint);
    HBufC8* pPatternAsXml = pServDesc->AsXmlL();
    CleanupStack::PopAndDestroy(pServDesc);
    CleanupStack::PushL(pPatternAsXml);

    CSenIdentityProvider* pIdP = CSenIdentityProvider::NewLC(KIdentityProviderName);
    pIdP->SetUserInfoL(aUserName, aUserName, aPassword);
    HBufC8* pIdPAsXml = pIdP->AsXmlL();
    CleanupStack::PopAndDestroy(pIdP);
    CleanupStack::PushL(pIdPAsXml);

    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());	//Codescannerwarnings
    //pSenChunk->SetLogger(Log());
    retVal = pSenChunk->CreateChunk();
    if (retVal == KErrNone)
        {
        retVal = pSenChunk->DescsToChunk(*pPatternAsXml, *pIdPAsXml);
        if (retVal == KErrNone)
            {
            retVal = iConnection.CredentialsL(*pSenChunk);
            if (retVal == KErrNone)
                {
                retVal = pSenChunk->DescFromChunk(credentials);
                if(retVal == KErrNone)
                    {
                    retVal = ExtractCredentialsL(credentials, aCredentials, aCredentialProperties);
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk);
    
    CleanupStack::PopAndDestroy(pIdPAsXml);
    CleanupStack::PopAndDestroy(pPatternAsXml);
    
    return retVal;    
    }

TInt CSenServiceManagerImpl::CredentialsL(const MSenServiceDescription& aPattern,
                                          RCredentialArray& aCredentials,
                                          RCredentialPropertiesArray& aCredentialProperties)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::CredentialsL(aPattern, aCredentials, aCredentialProperties)");
    TInt retVal(KErrNone);
    TPtrC8 credentials;
    
    if(&aPattern == NULL)
    {
      return KErrArgument;  
    }
    HBufC8* pPatternAsXml = ((MSenServiceDescription&)aPattern).AsXmlL();
    CleanupStack::PushL(pPatternAsXml);

    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());	//Codescannerwarnings
    //pSenChunk->SetLogger(Log());
    retVal = pSenChunk->CreateChunk();
    if (retVal == KErrNone)
        {
        retVal = pSenChunk->DescsToChunk(*pPatternAsXml, KNullDesC8);
        if (retVal == KErrNone)
            {
            retVal = iConnection.CredentialsL(*pSenChunk);
            if (retVal == KErrNone)
                {
                retVal = pSenChunk->DescFromChunk(credentials);
                if(retVal == KErrNone)
                    {
                    retVal = ExtractCredentialsL(credentials, aCredentials, aCredentialProperties);
                    }
                }
            else if ( retVal == KErrSenNoPermission )
                {
                if ( ipAuthProvider )
                    {
                    TBool identityProviderCreated = EFalse;
                    CSenIdentityProvider* pIdP = NULL;
                    pIdP = (CSenIdentityProvider*)ipAuthProvider->IdentityProviderL();
                    if ( !pIdP )
                        {
                        const TPtrC8 userName = ipAuthProvider->UsernameL();
                        const TPtrC8 password = ipAuthProvider->PasswordL();
                        if ( userName != KNullDesC8 &&
                        	 password != KNullDesC8 )
                            {
                            pIdP = CSenIdentityProvider::NewLC(KIdentityProviderName);
                            pIdP->SetUserInfoL( userName ,
                            					userName ,
                            					password );
                            identityProviderCreated = ETrue;
                            }
                        }
                    
                    if ( pIdP )
                        {
                        HBufC8* pIdPAsXml = pIdP->AsXmlL();
                        CleanupStack::PushL(pIdPAsXml);

                        retVal = pSenChunk->DescsToChunk(*pPatternAsXml, *pIdPAsXml);
                        if (retVal == KErrNone)
                            {
                            retVal = iConnection.CredentialsL(*pSenChunk);
                            if (retVal == KErrNone)
                                {
                                retVal = pSenChunk->DescFromChunk(credentials);
                                if(retVal == KErrNone)
                                    {
                                    retVal = ExtractCredentialsL(credentials, aCredentials, aCredentialProperties);
                                    }
                                }
                            }
                        CleanupStack::PopAndDestroy(pIdPAsXml);
                        if ( identityProviderCreated )
                            {
                            CleanupStack::PopAndDestroy(pIdP);
                            }
                        //CleanupStack::PopAndDestroy(pIdPAsXml);
                        }
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk);
    
    CleanupStack::PopAndDestroy(pPatternAsXml);
    
    return retVal;
    }

TInt CSenServiceManagerImpl::CredentialsL(const MSenServiceDescription& aPattern,
                                          const CSenIdentityProvider& aIdP,
                                          RCredentialArray& aCredentials,
                                          RCredentialPropertiesArray& aCredentialProperties)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::CredentialsL(aPattern, aIdP, aCredentials, aCredentialProperties)");
    TInt retVal(KErrNone);
    TPtrC8 credentials;
    
    if(&aPattern == NULL || &aIdP == NULL)
    {
     return KErrArgument;
    }
    HBufC8* pPatternAsXml = ((MSenServiceDescription&)aPattern).AsXmlL();
    CleanupStack::PushL(pPatternAsXml);

    HBufC8* pIdPAsXml = ((CSenIdentityProvider&)aIdP).AsXmlL();
    CleanupStack::PushL(pIdPAsXml);

    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());	//Codescannerwarnings
    //pSenChunk->SetLogger(Log());
    retVal = pSenChunk->CreateChunk();
    if (retVal == KErrNone)
        {
        retVal = pSenChunk->DescsToChunk(*pPatternAsXml, *pIdPAsXml);
        if (retVal == KErrNone)
            {
            retVal = iConnection.CredentialsL(*pSenChunk);
            if (retVal == KErrNone)
                {
                retVal = pSenChunk->DescFromChunk(credentials);
                if(retVal == KErrNone)
                    {
                    retVal = ExtractCredentialsL(credentials, aCredentials, aCredentialProperties);
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk);
    
    CleanupStack::PopAndDestroy(pIdPAsXml);
    CleanupStack::PopAndDestroy(pPatternAsXml);
    
    return retVal;
    }
                                  
TInt CSenServiceManagerImpl::CredentialsL(const MSenServiceDescription& aPattern,
                                          const TDesC8& aUserName,
                                          const TDesC8& aPassword,
                                          RCredentialArray& aCredentials,
                                          RCredentialPropertiesArray& aCredentialProperties)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::CredentialsL(aPattern, aUserName, aPassword, aCredentials, aCredentialProperties)");
    TInt retVal(KErrNone);
    TPtrC8 credentials;

    if(&aPattern == NULL) 
    {
     return KErrArgument;   
    }
    HBufC8* pPatternAsXml = ((MSenServiceDescription&)aPattern).AsXmlL();
    CleanupStack::PushL(pPatternAsXml);

    CSenIdentityProvider* pIdP = CSenIdentityProvider::NewLC(KIdentityProviderName);
    pIdP->SetUserInfoL(aUserName, aUserName, aPassword);
    HBufC8* pIdPAsXml = pIdP->AsXmlL();
    CleanupStack::PopAndDestroy(pIdP);
    CleanupStack::PushL(pIdPAsXml);

    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());	//Codescannerwarnings
    //pSenChunk->SetLogger(Log());
    retVal = pSenChunk->CreateChunk();
    if (retVal == KErrNone)
        {
        retVal = pSenChunk->DescsToChunk(*pPatternAsXml, *pIdPAsXml);
        if (retVal == KErrNone)
            {
            retVal = iConnection.CredentialsL(*pSenChunk);
            if (retVal == KErrNone)
                {
                retVal = pSenChunk->DescFromChunk(credentials);
                if(retVal == KErrNone)
                    {
                    retVal = ExtractCredentialsL(credentials, aCredentials, aCredentialProperties);
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk);
    
    CleanupStack::PopAndDestroy(pIdPAsXml);
    CleanupStack::PopAndDestroy(pPatternAsXml);
    
    return retVal;
    }

TInt CSenServiceManagerImpl::AddCredentialL(const MSenServiceDescription& aPattern,
                                            const CSenIdentityProvider& aIdP,
                                            const CSenCredential2& aCredential)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::AddCredentialL(aPattern, aIdP, aCredentials)");
    TInt retVal(KErrNone);
    
    if(&aPattern == NULL || &aIdP == NULL || &aCredential== NULL)
    {
      return KErrArgument;  
    }
    HBufC8* pPatternAsXml = ((MSenServiceDescription&)aPattern).AsXmlL();
    CleanupStack::PushL(pPatternAsXml);

    HBufC8* pIdPAsXml = ((CSenIdentityProvider&)aIdP).AsXmlL();
    CleanupStack::PushL(pIdPAsXml);

    HBufC8* pCredentialAsXml = ((CSenCredential2&)aCredential).AsXmlL();
    CleanupStack::PushL(pCredentialAsXml);
    
    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());	//Codescannerwarnings
    //pSenChunk->SetLogger(Log());
    retVal = pSenChunk->CreateChunk();
    
    if (retVal == KErrNone)
        {
        retVal = pSenChunk->DescsToChunk(*pPatternAsXml, *pIdPAsXml);
        retVal = pSenChunk->DescToChunk(*pCredentialAsXml,  2);
        if (retVal == KErrNone)
            {
            retVal = iConnection.AddCredentialL(*pSenChunk);
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk);

    CleanupStack::PopAndDestroy(pCredentialAsXml);
    CleanupStack::PopAndDestroy(pIdPAsXml);
    CleanupStack::PopAndDestroy(pPatternAsXml);

    return retVal;
    }

TInt CSenServiceManagerImpl::AddCredentialL(const TDesC8& aEndpoint,
                                            const CSenIdentityProvider& aIdP,
                                            const CSenCredential2& aCredential)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::AddCredentialL(aEndpoint, aIdP, aCredentials)");
    CSenXmlServiceDescription* pServDesc = CSenXmlServiceDescription::NewLC();
    pServDesc->SetEndPointL(aEndpoint);
    
    TInt retVal = AddCredentialL(*pServDesc, aIdP, aCredential);
    
    CleanupStack::PopAndDestroy(pServDesc);

    return retVal;   
    }

TInt CSenServiceManagerImpl::AddCredentialL(const MSenServiceDescription& aPattern,
                                            const CSenIdentityProvider& aIdP,
                                            const CSenCredential2& aCredential,
                                            const CSenXmlProperties& aCredentialProperties)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::AddCredentialL(aPattern, aIdP, aCredentials, aCredentialProperties)");
    TInt retVal(KErrNone);
    
    if(&aPattern == NULL || &aIdP == NULL || &aCredential== NULL || &aCredentialProperties == NULL )
    {
      return KErrArgument;  
    }
    HBufC8* pPatternAsXml = ((MSenServiceDescription&)aPattern).AsXmlL();
    CleanupStack::PushL(pPatternAsXml);

    HBufC8* pIdPAsXml = ((CSenIdentityProvider&)aIdP).AsXmlL();
    CleanupStack::PushL(pIdPAsXml);

    HBufC8* pCredentialAsXml = ((CSenCredential2&)aCredential).AsXmlL();
    CleanupStack::PushL(pCredentialAsXml);

    HBufC8* pCredentialPropertiesAsXml = ((CSenXmlProperties&)aCredentialProperties).AsUtf8L();
    CleanupStack::PushL(pCredentialPropertiesAsXml);
    
    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());	//Codescannerwarnings
    //pSenChunk->SetLogger(Log());
    retVal = pSenChunk->CreateChunk();
    
    if (retVal == KErrNone)
        {
        retVal = pSenChunk->DescsToChunk(*pPatternAsXml, *pIdPAsXml);
        retVal = pSenChunk->DescToChunk(*pCredentialAsXml,  2);
        retVal = pSenChunk->DescToChunk(*pCredentialPropertiesAsXml,  3);
        if (retVal == KErrNone)
            {
            retVal = iConnection.AddCredentialL(*pSenChunk);
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk);


    CleanupStack::PopAndDestroy(pCredentialPropertiesAsXml);
    CleanupStack::PopAndDestroy(pCredentialAsXml);
    CleanupStack::PopAndDestroy(pIdPAsXml);
    CleanupStack::PopAndDestroy(pPatternAsXml);

    return retVal;
    }

TInt CSenServiceManagerImpl::AddCredentialL(const TDesC8& aEndpoint,
                                            const CSenIdentityProvider& aIdP,
                                            const CSenCredential2& aCredential,
                                            const CSenXmlProperties& aCredentialProperties)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::AddCredentialL(aEndpoint, aIdP, aCredentials, aCredentialProperties)");
    CSenXmlServiceDescription* pServDesc = CSenXmlServiceDescription::NewLC();
    pServDesc->SetEndPointL(aEndpoint);
    
    TInt retVal = AddCredentialL(*pServDesc, aIdP, aCredential, aCredentialProperties);
    
    CleanupStack::PopAndDestroy(pServDesc);

    return retVal;   
    }

TInt CSenServiceManagerImpl::RemoveCredentialsL(const CSenIdentityProvider& aIdP)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::RemoveCredentialsL(aIdp)");
    TInt retVal(KErrNone);
        
    if(&aIdP == NULL)
        {
        TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::RemoveCredentialsL(aIdp) returns KErrArgument");
        return  KErrArgument; 
        }
    
    HBufC8* pIdPAsXml = ((CSenIdentityProvider&)aIdP).AsXmlL();
    CleanupStack::PushL(pIdPAsXml);

    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());  //Codescannerwarnings
    retVal = pSenChunk->CreateChunk();
    
    if (retVal == KErrNone)
        {
        retVal = pSenChunk->DescsToChunk(KNullDesC8(), *pIdPAsXml);
        if (retVal == KErrNone)
            {
            retVal = iConnection.RemoveCredentialsL(*pSenChunk);
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk);

    CleanupStack::PopAndDestroy(pIdPAsXml);
    
		TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceManagerImpl::RemoveCredentialsL(aIdp) returns [%d]"), retVal));
    return retVal;
    }
TInt CSenServiceManagerImpl::RemoveCredentialsL(const MSenServiceDescription& aPattern,
                                                const CSenIdentityProvider& aIdP)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::RemoveCredentialsL(aPattern, aIdP)");
    TInt retVal(KErrNone);
    
    if(&aPattern == NULL || &aIdP == NULL)
    {
      return  KErrArgument; 
    }
    HBufC8* pPatternAsXml = ((MSenServiceDescription&)aPattern).AsXmlL();
    CleanupStack::PushL(pPatternAsXml);

    HBufC8* pIdPAsXml = ((CSenIdentityProvider&)aIdP).AsXmlL();
    CleanupStack::PushL(pIdPAsXml);

    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());	//Codescannerwarnings
    //pSenChunk->SetLogger(Log());
    retVal = pSenChunk->CreateChunk();
    
    if (retVal == KErrNone)
        {
        retVal = pSenChunk->DescsToChunk(*pPatternAsXml, *pIdPAsXml);
        if (retVal == KErrNone)
            {
            retVal = iConnection.RemoveCredentialsL(*pSenChunk);
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk);

    CleanupStack::PopAndDestroy(pIdPAsXml);
    CleanupStack::PopAndDestroy(pPatternAsXml);

    return retVal;
    }

TInt CSenServiceManagerImpl::RemoveCredentialsL(const MSenServiceDescription& aPattern,
                                                const TDesC8& aUserName,
                                                const TDesC8& aPassword)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::RemoveCredentialsL(aPattern, aUserName, aPassword)");
    CSenIdentityProvider* pIdP = CSenIdentityProvider::NewLC(KIdentityProviderName);
    pIdP->SetUserInfoL(aUserName, aUserName, aPassword);
    
    TInt retVal = RemoveCredentialsL(aPattern, *pIdP);
    
    CleanupStack::PopAndDestroy(pIdP);

    return retVal;
    }


TInt CSenServiceManagerImpl::IdentityProvidersL( RIdentityProviderArray& aList )
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::IdentityProvidersL(aList)");
    TInt retVal(KErrNone);
    TPtrC8 descriptions;

    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkNameL());
    retVal = pSenChunk->CreateChunk();
    if (retVal == KErrNone)
        {
        retVal = iConnection.IdentityProvidersL(*pSenChunk);
        if (retVal == KErrNone)
            {
            retVal = pSenChunk->DescFromChunk(descriptions);
            if(retVal == KErrNone)
                {
                retVal = ExtractIdentiyProvidersL(descriptions, aList);
                TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("-ExtractIdentiyProvidersL returned [%d]"), retVal));
                }
            }
        }
    CleanupStack::PopAndDestroy(pSenChunk); // Close chunk
    return retVal;
    }
TInt CSenServiceManagerImpl::ExtractIdentiyProvidersL(TPtrC8& aIdPs, 
                                                      RIdentityProviderArray& aList)
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::ExtractIdentiyProvidersL(aIdPs, aList)");
    TInt retVal(KErrNotFound);

    
    if (aIdPs.Length() > 0)
        {
        TPtrC8 single;
        TPtrC8 rest;
        _LIT8(KTab, "\t");
        TBool loop(ETrue);
        TInt pos(-1);
        CSenIdentityProvider* idp = NULL;
        rest.Set(aIdPs);

        while (loop)
            {
            pos = rest.Find(KTab());
            if (pos > 0)
                {
                idp = CSenIdentityProvider::NewLC(KNullDesC8);
                iReader->SetContentHandler(*idp);
                idp->SetReader(*iReader);
                single.Set(rest.Left(pos));

                TInt leaveCode(KErrNone);
                TLSLOG_ALL(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel, single);
                TRAP(leaveCode, idp->ParseL(single));
                if(leaveCode!=KErrNone)
                    {
                    TInt count(aList.Count());
                    TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("-idp->ParseL(#%d): parsing failed: %d"), count+1, leaveCode));
                        
                    CleanupStack::PopAndDestroy(); // desc
                    if(count == 0)
                        {
                        retVal = KErrNotFound; // could not parse single service description
                        }

                    break;
                    }

                rest.Set(rest.Mid(pos+1));  // Check
                retVal = aList.Append(idp);
                if(retVal == KErrNone)
                    {
                    CleanupStack::Pop(); // desc
                    }
                else
                    {
                    CleanupStack::PopAndDestroy();
                    }
                idp = NULL;
                }
            else
                {
                idp = CSenIdentityProvider::NewLC(KNullDesC8);
                iReader->SetContentHandler(*idp);
                idp->SetReader(*iReader);

                TInt leaveCode(KErrNone);
                TRAP(leaveCode, idp->ParseL(rest));
                if(leaveCode!=KErrNone)
                    {
                    TInt count(aList.Count());
                    TLSLOG_FORMAT((KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel , _L8("-idp->ParseL(#%d): parsing failed: %d"), count+1, leaveCode));
                        
                    CleanupStack::PopAndDestroy(); // desc
                    if(count == 0)
                        {
                        retVal = KErrNotFound; // could not parse single service description
                        }

                    break;
                    }
                retVal = aList.Append(idp);
                if(retVal == KErrNone)
                    {
                    CleanupStack::Pop(); // desc
                    }
                else
                    {
                    CleanupStack::PopAndDestroy();
                    }
                idp = NULL;
                break;
                }

            if (rest.Length() == 0 )
                {
                break;
                }

            }
        } //  END OF: if idp.Length() > 0
    else
       	{
		TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::ExtractIdentiyProvidersL aIdPs(aIdPs, aList) aIdPs = 0 length");
        }
    return retVal;
    }
TAny* CSenServiceManagerImpl::InterfaceByUid( TUid aUID )
    {
    TLSLOG_L(KSenServiceManagerLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceManagerImpl::InterfaceByUid(aUID)");
    if ( aUID == KSenInterfaceUidInternalServiceManager )
		{
		TLSLOG_FORMAT(( KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::InterfaceByUid(%d) == KSenInterfaceUidInternalServiceManager" ), aUID.iUid ));
		// Must be cast to M-class  (as the same C-class implements multiple M-classes):
		MSenInternalServiceManager* manager = (MSenInternalServiceManager*) this;
		return manager; 
		}
    return NULL;
    }

// END OF FILE
