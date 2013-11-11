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











#include <s32mem.h>
#include <s32file.h>
#include <sysutil.h>

#include <SenBaseFragment.h>
#include <SenXmlUtils.h>

#include "msenidentitymanager.h"
#include "msenidentity.h"
#include "senbaseidentitymanager.h"
#include "sensecuritymechanismobserver.h"
#include "senlogger.h"
#include "sensecuritymechanism.h"  // internal  Framework\inc - ECOM interface

#include "senservicemanagerdefines.h" // KMaxTicks (1209600000)
                                      // 1 tick = 1 millisec
                                      // 86400 sec = 1 day
                                      // 86400*1000 millisec = 1 day
                                      // 86400*14*1000 = 1209600000 millisec = 2 weeks

#include "sendebug.h" // internal Utils\inc - filelogging MACROs
#include "sennotplugindlg.h" // internal NotifierPlugin\inc, KSenNotifierPluginUID

#include "senserviceinvocationframework.h"  // internal Framework\inc (framework event codes)

#ifndef __WINS__ // only required in release builds
// For reading IMEI. NOTE: not found from Series 60 2.1 public SDK
//Since plpvariant.h is depricated in TB9.2 from wk48 2009 on words.
//provided alternative solution  
//#include <plpvariant.h>
#endif
#include<SenBaseAttribute.h>

namespace
    {
    //_LIT8(KSenBaseIdentityManagerPanic, "SenBaseIdentityManager");

#if !defined( EKA2 ) && !defined( RD_SECURE_PRIV_DATA )
    _LIT(KIdentityFile, "C:\\system\\data\\senidentities.xml");
#else
    _LIT(KIdentityFile, "senidentities.xml");
#endif

    _LIT8(KSpace, " ");
    _LIT8(KUserName, "User");
    _LIT8(KUsersStart, "<Users><User>");
    _LIT8(KUsersEnd, "</User></Users>");
    _LIT8(KIdentityProvider, "IdentityProvider");
    _LIT8(KTouch, "touch");

    const TInt KFLATBUF_SIZE = 128;
    }


EXPORT_C CSenBaseIdentityManager* CSenBaseIdentityManager::NewL(
                                            MSenCoreServiceManager& aManager)
    {
    CSenBaseIdentityManager* pNew = NewLC(aManager);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenBaseIdentityManager* CSenBaseIdentityManager::NewLC(
                                            MSenCoreServiceManager& aManager)
    {
    CSenBaseIdentityManager* pNew =
                        new (ELeave) CSenBaseIdentityManager(aManager);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

CSenBaseIdentityManager::CSenBaseIdentityManager(
                                            MSenCoreServiceManager& aManager)
:   iIdentity(NULL),
    iProvider(NULL),
    iManager(aManager),
    iConfigReadonly(EFalse),
    iLog((RFileLogger*)aManager.Log()),
    iObserver(NULL),
    ipMechanicsNames(NULL),
    ipDeviceID(NULL),
    iShowPasswordDialog(EFalse)
    {
    }

EXPORT_C CSenBaseIdentityManager::~CSenBaseIdentityManager()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenBaseIdentityManager::~CSenBaseIdentityManager");        
    iEcomInfoArray.ResetAndDestroy();
    iSechMechNames.ResetAndDestroy();
    iSechMechCues.ResetAndDestroy();
    delete ipMechanicsNames;
    delete iObserver;
    
    delete iProvider;
    delete ipDeviceID;
    }


void CSenBaseIdentityManager::ConstructL()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenBaseIdentityManager::ConstructL - Version 2 [2006-05-09]");        
    BaseConstructL(KIdentityElementLocalName);


    delete ipDeviceID;
    ipDeviceID=NULL;

    #ifndef __WINS__
	//Since plpvariant.h is depricated in TB9.2 from wk48 2009 on words.
	//provided alternative solution  
    /*
    // This only works on target machine
    TPlpVariantMachineId imei;
    PlpVariant::GetMachineIdL(imei);


    ipDeviceID = SenXmlUtils::ToUtf8LC(imei);
    CleanupStack::Pop(); // ipDeviceID
	*/
	TPtrC8 imei ;
	imei.Set(iManager.IMEI());
	ipDeviceID = imei.AllocL();
		
    #else
    // Return a fake IMEI when working on emulator
    _LIT8(KEmulatorImei, "012345678901234");
    ipDeviceID = KEmulatorImei().AllocL();
    #endif

    // Device ID is now resolved, fake or not
    TPtr8 imeiTPtr = ipDeviceID->Des();
    iIdentity = CSenBaseIdentity::NewL(imeiTPtr);

    HBufC* file = KIdentityFile().AllocL();
    CleanupStack::PushL(file);
    LoadFromL(*file, *iManager.XMLReader());

    // ValidateDefaultIdentity()
    ReloadSenSecurityMechanismsL();
    iObserver = CSenSenSecurityMechanismObserver::NewL(*this);
    iObserver->Start();
    CleanupStack::PopAndDestroy(); // file
    }

RFileLogger* CSenBaseIdentityManager::Log()
    {
    return iLog;
    }

void CSenBaseIdentityManager::LoadFromL(TDesC& aFile,
                                                 CSenXmlReader& aReader)
    {
    RFs fss;
    User::LeaveIfError(fss.Connect());
    CleanupClosePushL(fss);
    
    aReader.SetContentHandler(*this);
    SetReader(aReader);

    TInt leaveCode(KErrNone);

#if defined( EKA2 ) || defined( RD_SECURE_PRIV_DATA )
    TBuf<KMaxPath> file;
    fss.CreatePrivatePath(EDriveC);
    fss.PrivatePath(file);
    file.Append(aFile);
    TRAP(leaveCode, aReader.ParseL(fss, file));
#else
    TRAP(leaveCode, aReader.ParseL(fss, aFile));
#endif

#ifdef _SENDEBUG
    if(leaveCode == KErrNotFound)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- The senidentities.xml does not exist!");
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- New file will be generated.");
        }
    else if(leaveCode != KErrNone)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Parsing of senidentities.xml leaved: %d"), leaveCode));
        }   
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Identity database (senidentities.xml) successfully read.");
        }
#else
    leaveCode=0; // this is a decision: we are not interested 
                 // of invalid input in read/parse phase
#endif

    WriteDatabase(); // Always validate by saving

    CleanupStack::PopAndDestroy(); // fss
    }

EXPORT_C TInt CSenBaseIdentityManager::WriteDatabase()
    {
    TInt retVal(KErrNone);
    TRAP(retVal, WriteConfigurationToL(KIdentityFile));

    return retVal;
    }

void CSenBaseIdentityManager::StartElementL(
                                          const TDesC8& aNsUri,
                                          const TDesC8& aLocalName,
                                          const TDesC8& aQName,
                                          const RAttributeArray& aAttributes
                                                  )
    {
    if (aLocalName ==  KIdentityElementLocalName)
        {
        delete iIdentity;   // there can be only one at the moment
        iIdentity = NULL;
        TPtr8 imei = ipDeviceID->Des();
        iIdentity = CSenBaseIdentity::NewL(imei);
        iIdentity->SetAttributesL(aAttributes);
        iIdentity->SetReader(*Reader());
        DelegateParsingL(*iIdentity);
        }
//  else if (aLocalName == KUsers)
//      {
//      }
    else
        {
        CSenBaseFragment::StartElementL(aNsUri, 
                                    aLocalName, aQName, aAttributes);
        }
    }

MSenIdentity& CSenBaseIdentityManager::IdentityL()
    {
    return (MSenIdentity&)*iIdentity;
    }

TInt CSenBaseIdentityManager::UserNameL(HBufC8*& aUserName)
    {
    delete aUserName;
    aUserName = NULL;
    aUserName = KUserName().AllocL();
    return KErrNone;
    }

CSenIdentityProvider* CSenBaseIdentityManager::IdentityProviderL()
    {
    return IdentityL().IdentityProviderL();
    }
const RPointerArray<CSenIdentityProvider>& CSenBaseIdentityManager::IdentityProvidersL()
    {
    return IdentityL().IdentityProvidersL();
    }

CSenIdentityProvider* CSenBaseIdentityManager::IdentityProviderL(
                            MSenServiceDescription& aServiceDescription )
    {
    return IdentityL().IdentityProviderL( aServiceDescription );
    }

CSenIdentityProvider* CSenBaseIdentityManager::IdentityProviderL(
                                                        const TDesC8& aURI )
    {
    return IdentityL().IdentityProviderL(aURI);
    }

CSenIdentityProvider* CSenBaseIdentityManager::IdentityProviderL(
                                                const CDesC8Array& aIdpList,
                                                TBool aStrict)
    {
    return IdentityL().IdentityProviderL(aIdpList,aStrict);
    }

CSenIdentityProvider* CSenBaseIdentityManager::IdentityProviderL(
                                MSenServiceDescription& aServiceDescription,
                                const CDesC8Array& aIdpList,
                                TBool aStrict)
    {
    return IdentityL().IdentityProviderL(aServiceDescription,aIdpList,aStrict);
    }



// Takes ownership of given IdentityProvider(!)
TInt CSenBaseIdentityManager::RegisterIdentityProviderL( CSenIdentityProvider* apIdp )
    {
    if(!apIdp)
        {
        return KErrArgument; // NULL given!
        }

    CleanupStack::PushL(apIdp);

    // try to register into default identity
    // null check done, above
    TInt retVal = IdentityL().RegisterIdentityProviderL(apIdp);
    CleanupStack::Pop(); // apIdp, ownership elsewhere

    // check success:
    if ( retVal == KErrNone)
        {
        // ownership taken, pop from stack:
        // try to serialize owned object
        TInt notifyLeaveCode(KErrNone);
        TRAP(notifyLeaveCode, iManager.NotifyFrameworksL(KSenEventRegisterIdentityProvider, apIdp));
        if ( WriteDatabase() != KErrNone)
            {
            retVal = KErrWrite;
            }
        }
    return retVal;
    }

TInt CSenBaseIdentityManager::UnregisterIdentityProviderL( CSenIdentityProvider& aIdp )
    {
    TInt retVal = IdentityL().UnregisterIdentityProviderL( aIdp );

    if ( retVal == KErrNone )
        {
        TInt notifyLeaveCode(KErrNone);
        TRAP(notifyLeaveCode, iManager.NotifyFrameworksL(KSenEventUnregisterIdentityProvider, &aIdp));
        
        
//      if (KErrNone != WriteDatabase() )
//          {
//          retVal = EFalse;
//          }

        retVal = WriteDatabase();
        }
    return retVal;
    }

TBool CSenBaseIdentityManager::AssociateServiceL( const TDesC8& aServiceID,
                                                  const TDesC8& aProviderID )
    {
    TBool retVal = IdentityL().AssociateServiceL(aServiceID, aProviderID);
    if (retVal)
        {
        if (KErrNone != WriteDatabase())
            {
            retVal = EFalse;
            }
        }
    return retVal;
    }

TBool CSenBaseIdentityManager::DissociateServiceL(
                                                   const TDesC8& aServiceID,
                                                   const TDesC8& aProviderID )
    {
    TBool retVal = IdentityL().DissociateServiceL(aServiceID, aProviderID);
    if (retVal)
        {
        if (KErrNone != WriteDatabase())
            {
            retVal = EFalse;
            }
        }
    return retVal;
    }

TInt CSenBaseIdentityManager::IdentitiesL(
                                            CDesC8Array& /* aIdentitiesList */)
    {
    return KErrNotSupported; // not yet supported.
    }


// NOTE! This code does not do as documentation states.
// Only second line text is shown and
// "Ok" button on the left side.
// return the value of button pressed (OK or CANCEL)
TInt CSenBaseIdentityManager::YesNoQueryL(const TDesC& aLine1,
                                          const TDesC& aLine2,
                                          const TDesC& aButton1,
                                          const TDesC& aButton2)
    {
    RNotifier notifier;
    User::LeaveIfError(notifier.Connect());
    CleanupClosePushL(notifier);

    TRequestStatus status;
    TInt buttonVal = -1;


    // NOTE! This code does not do as documentation states.
    // Only second line text is shown and
    // "Ok" button on the left side.

//    notifier.Notify(_L("Line 1 text"), _L("Line2 text"),
//        _L("B1Txt"), _L("B2Txt"), buttonVal, status);

    notifier.Notify(aLine1,
                    aLine2,
                    aButton1,
                    aButton2,
                    buttonVal,
                    status);


    User::WaitForRequest(status);
    CleanupStack::PopAndDestroy(); // Close notifier

#ifdef _SENDEBUG
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("User pressed button (%d)"), buttonVal));
#endif

    return buttonVal;
    }

TBool CSenBaseIdentityManager::AllowSavePasswordL()
    {
    TBool retVal = EFalse;

    RNotifier notifier;
    User::LeaveIfError(notifier.Connect());
    CleanupClosePushL(notifier);

    TPckgBuf<TAskSaveDlgRequest>* request = new(ELeave)TPckgBuf<TAskSaveDlgRequest>();
    CleanupStack::PushL(request);

    TPckgBuf<TAskSaveDlgResponse>* response = new(ELeave)TPckgBuf<TAskSaveDlgResponse>();
    CleanupStack::PushL(response);

    TRequestStatus reqStatus;
    notifier.StartNotifierAndGetResponse(reqStatus, KSenNotifierPluginUID, *request, *response);

    User::WaitForRequest(reqStatus);
    notifier.Close();

    if(reqStatus.Int() == KErrNone)
        {
        if((*response)().OkButtonPressed())
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("User pressed OK Button to allow password saving")));
            retVal = ETrue;
            }
#ifdef _SENDEBUG
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("User pressed Cancel Button to deny password saving")));
            }
#endif
        }
#ifdef _SENDEBUG
    else if(reqStatus.Int() == KErrNotFound)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("Notifier plug-in for 'Save WS data?' -dialog was not found")));
        }
    else
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("'Save WS data?' -dialog returned an error: %d"), 
                                                        reqStatus.Int()));
        }
#endif

    CleanupStack::PopAndDestroy(2); // request, response;
    //CleanupStack::PopAndDestroy(); // close the notifier
    CleanupStack::Pop(); // notifier
    return retVal;
    }

TInt CSenBaseIdentityManager::AuthenticationForL(
                            CSenIdentityProvider& aProvider, 
                            TPckgBuf<TSenAuthentication>& aResponse)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenBaseIdentityManager::AuthenticationForL");

    TPtrC8 user = aProvider.AuthzID();
    if (user == KNullDesC8)
        {
        user.Set(aProvider.AdvisoryAuthnID());
        }


    if (!iShowPasswordDialog)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"Not allowed to show password dialog");
        HBufC8* pDecodedPassword = SenXmlUtils::DecodeHttpCharactersLC(aProvider.Password());
        TPtrC8 decodedPassword = pDecodedPassword->Des();

        aResponse().iUsername.Append((const TUint8*)user.Ptr(), user.Size());
        aResponse().iPassword.Append((const TUint8*)decodedPassword.Ptr(), 
                                        decodedPassword.Size());
        CleanupStack::PopAndDestroy(); // delete pDecodedPassword
        return KErrNone;
        }


    RNotifier notifier;
    User::LeaveIfError(notifier.Connect());
    CleanupClosePushL(notifier);

    TPckgBuf<TAuthenticationDlgRequest>* request = 
                            new(ELeave)TPckgBuf<TAuthenticationDlgRequest>();
    CleanupStack::PushL(request);

    // convert username to unicode
    HBufC* pUserAsUnicode = SenXmlUtils::ToUnicodeLC(user); // push
    TPtrC username = pUserAsUnicode->Des();

    // decode password
    HBufC8* pDecodedPassword = SenXmlUtils::DecodeHttpCharactersLC(aProvider.Password());
    TPtrC8 decodedPassword = pDecodedPassword->Des();
    // convert decoded password to unicode
    HBufC* pPasswordAsUnicode = 
                SenXmlUtils::ToUnicodeLC(decodedPassword); // push
    TPtrC password = pPasswordAsUnicode->Des();

    // set data to request
    (*request)().SetData(username, password);

    CleanupStack::PopAndDestroy(3); // delete pPasswordAsUnicode, pDecodedPassword, pUserAsUnicode


    TPckgBuf<TAuthenticationDlgResponse>* response = 
                        new(ELeave)TPckgBuf<TAuthenticationDlgResponse>();
    CleanupStack::PushL(response);

    TRequestStatus reqStatus;
    notifier.StartNotifierAndGetResponse(reqStatus, 
                            KSenNotifierPluginUID, *request, *response);

    TBool illegalUsername = EFalse;

    HBufC8* pUsernameUtf8 = NULL;
    HBufC8* pPasswordUtf8 = NULL;
    User::WaitForRequest(reqStatus);

    notifier.Close();

    if(reqStatus.Int() == KErrNone)
        {
        if((*response)().OkButtonPressed())
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("User pressed OK Button in Password dialog")));
            pUsernameUtf8 = SenXmlUtils::ToUtf8LC((*response)().Username());
            pPasswordUtf8 = SenXmlUtils::ToUtf8LC((*response)().Password());
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("Username: %S"), pUsernameUtf8));
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("Password: %S"), pPasswordUtf8 ));
            HBufC8* pEncodedUsername = NULL;
            illegalUsername = 
                SenXmlUtils::EncodeHttpCharactersL(*pUsernameUtf8,
                                                    pEncodedUsername);
            if (illegalUsername) 
                {
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("Username included illegal characters.")));
                delete pEncodedUsername;
                pEncodedUsername = NULL;
                }
#ifdef _SENDEBUG
            else
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"Username did NOT include illegal characters.");
                }
#endif
       		
       		//Check if iilegal chars are there then return
            if (!illegalUsername)
                {
                
                if(user != *pUsernameUtf8)
                {
                	
	                // User modified the username in the dialog prompt(!)
	                // We have to save it right away, because there is
	                // no in-memory/session based member variables in
	                // components calling identity manager (sec mechs and
	                // in ID-WSF AS client!
	                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"Username changed. Saving new username.");

	                // Use Advisory only if AuthzID is not available
	                if (aProvider.AuthzID() == KNullDesC8 
	                    && aProvider.AdvisoryAuthnID() != KNullDesC8)
	                    {
	                    aProvider.SetUserInfoL(aProvider.AuthzID(),
	                                            *pUsernameUtf8, 
	                                            aProvider.Password());
	                    }
	                else 
	                    {
	                    // There was at least AuthzID available, and possibly
	                    // also advisory value. Any way, if 

	                    
	                    // We have to NULL advisory value (set it to KNullDesC8), 
	                    // because if both were available, then user was
	                    // prompted to allow modification of AuthzID AND 
	                    // if advisory was changed in service (is no longer
	                    // valid), there would NOT be any way for end-user
	                    // to change (remove) it(!)

	                    CSenElement& element = aProvider.AsElement();
	                    delete element.RemoveElement(KSenIdpAdvisoryAuthnIdLocalname);

	                    aProvider.SetUserInfoL(*pUsernameUtf8, 
	                                            KNullDesC8,
	                                            aProvider.Password());
	                    }
	                // serialize changed username into sensessions.xml database immediately
	                WriteDatabase();
                }
                
	            if (AllowSavePasswordL())
	                {
	                HBufC8* pEncodedPassword = SenXmlUtils::EncodeHttpCharactersLC(*pPasswordUtf8);
#ifdef _SENDEBUG
                if(pEncodedPassword)
                    {
                    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("Encoded password: %S"), pEncodedPassword));
                    }
#endif
					if(pEncodedPassword)
						{
							aProvider.SetUserInfoL(aProvider.AuthzID(),
	                                        aProvider.AdvisoryAuthnID(),
	                                        *pEncodedPassword);
						}
	                WriteDatabase();
	                CleanupStack::PopAndDestroy(1); // pEncodedPassword
	                }
                
                }

            aResponse().iUsername.Zero();
            aResponse().iPassword.Zero();
            if (!illegalUsername) 
            {
	            aResponse().iUsername.Copy(pUsernameUtf8->Des());
	            aResponse().iPassword.Copy(pPasswordUtf8->Des());
            }

            CleanupStack::PopAndDestroy(2); // pPasswordUtf8, pUsernameUtf8
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("User pressed Cancel Button in Password dialog")));
            }
        }
    else if(reqStatus.Int() == KErrNotFound)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("Password dialog plugin notifier impl. was not found")));
        }
    else
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8(" Notifier plugin for 'Password' dialog returned an error: %d"), 
                                                        reqStatus.Int()));
        }

    CleanupStack::PopAndDestroy(2); // request, response;
    CleanupStack::Pop(); // notifier

    if (illegalUsername) return KErrArgument;
    return reqStatus.Int();
    }


TInt CSenBaseIdentityManager::WriteConfigurationToL( const TDesC& aFile )
    {
    // First, collect everything into MEMORY
    CBufFlat *pBuf = CBufFlat::NewL(KFLATBUF_SIZE);
    CleanupStack::PushL(pBuf);

    RBufWriteStream bufWs(*pBuf);
    CleanupClosePushL(bufWs);

    bufWs.WriteL(KUsersStart);

    if(iIdentity)
        {
        iIdentity->WriteAsXMLToL(bufWs);
        }

    bufWs.WriteL(KUsersEnd);

    TPtrC8 p8 = pBuf->Ptr(0);

    CleanupStack::PopAndDestroy(1); // bufWs

    // Everything in MEMORY ok, prepare to write into file
    RFs fss;
    User::LeaveIfError(fss.Connect());
    CleanupClosePushL(fss);

    RFileWriteStream fileOutStream;
    CleanupClosePushL(fileOutStream);


    if(!SysUtil::FFSSpaceBelowCriticalLevelL(&fss, p8.Length()) )
        {
        // note, this will zero-length the file(!)
        // it is better to require that 2xfilesize is available and not to
        // dangerously zero the old file and find out
        // that there is no space left..

        //Data caging 2 implementation
#if defined( EKA2 ) || defined( RD_SECURE_PRIV_DATA )
        TBuf<KMaxPath> file;
        fss.CreatePrivatePath(EDriveC);
        fss.PrivatePath(file);
        file.Append(aFile);
        fileOutStream.Replace(fss, file, EFileWrite);
#else
        fileOutStream.Replace(fss, aFile, EFileWrite);
#endif
        // finally write the UTF-8 into the file. 
        fileOutStream.WriteL(p8);
        }

    CleanupStack::PopAndDestroy(3); // fileOutStream, fss, pBuf

    return KErrNone;
    }


void CSenBaseIdentityManager::ReloadSenSecurityMechanismsL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("Entering CSenXMLDAO::ReloadSenSecurityMechanismsL()..")));

    delete ipMechanicsNames;
    ipMechanicsNames = NULL;
    ipMechanicsNames = CBufFlat::NewL(KFLATBUF_SIZE);
    RBufWriteStream bufWs(*ipMechanicsNames);
    CleanupClosePushL(bufWs);

    iSechMechNames.ResetAndDestroy();
    iSechMechCues.ResetAndDestroy();

    REComSession::ListImplementationsL(KUidSechMech, iEcomInfoArray);


    TInt infoCount(iEcomInfoArray.Count());
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("%d Security Mechanism ECOM implementations found."),
                        infoCount));

    for (TInt i=0; i<infoCount; i++)
        {
        CImplementationInformation* pImplImfo = iEcomInfoArray[i];
        CSenSecurityMechanism* pSechMech =
            CSenSecurityMechanism::NewL(pImplImfo->DataType(),
                                        iManager);
        CleanupStack::PushL(pSechMech);
        iSechMechNames.Append(pSechMech->Name().AllocL());
        iSechMechCues.Append(pImplImfo->DataType().AllocL());
        bufWs.WriteL(pSechMech->Name());
        CleanupStack::PopAndDestroy(); // pSechMech
        if (i < (infoCount-1))
            {
            bufWs.WriteL(KSpace);
            }
        }
    iEcomInfoArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy(1); // bufWs
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("Leaving CSenXMLDAO::ReloadSenSecurityMechanismsL().. ")));

    }


TPtrC8 CSenBaseIdentityManager::SenSecurityMechanismNames8L()
    {
    TPtrC8 p8 = ipMechanicsNames->Ptr(0);
    return p8;
    }

CSenSecurityMechanism* CSenBaseIdentityManager::MechanismNamedL(
                                                        const TDesC8& aName)
    {
    CSenSecurityMechanism* pSaslMechanism = NULL;
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenIdWsfPlugin::MechanismNamedL");
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(aName));

    for (TInt i=0; i<iSechMechNames.Count(); i++)
        {
        if (*iSechMechNames[i] == aName)
            {
            pSaslMechanism = CSenSecurityMechanism::NewL(*iSechMechCues[i], 
                                                                iManager);
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenBaseIdentityManager::MechanismNamedL - successful");
            break;
            }
        }
    if(!pSaslMechanism)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenBaseIdentityManager::MechanismNamedL - lookup failed!");
        }

    return pSaslMechanism;
    }

HBufC8* CSenBaseIdentityManager::EncodeToBase64LC(const TDesC8& aSource)
    {
    return iManager.EncodeToBase64LC(aSource);
    }

HBufC8* CSenBaseIdentityManager::DecodeFromBase64LC(const TDesC8& aSource)
    {
    return iManager.DecodeFromBase64LC(aSource);
    }

void CSenBaseIdentityManager::SetShowPasswordDialog(const TBool aState)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenBaseIdentityManager::SetShowPasswordDialog");
    iShowPasswordDialog = aState;
    }

TInt CSenBaseIdentityManager::ContainsIdentityProviderL(
                                                   TBool& aContains,
                                                   CSenIdentityProvider& aIDP)
    {
    return IdentityL().ContainsIdentityProviderL(aContains, aIDP);
    }
    
TInt CSenBaseIdentityManager::FindMatchingIdentityProviderL(CSenIdentityProvider &aIdp,
                                                            CSenIdentityProvider*& apMatch)
    {
    return IdentityL().FindMatchingIdentityProviderL(aIdp, apMatch);
    }
        
TInt CSenBaseIdentityManager::UpdateIdentityProviderL(CSenIdentityProvider& aIdp)
    {
    TInt retVal = IdentityL().UpdateIdentityProviderL(aIdp);

    // check success:
    if ( retVal == KErrNone)
        {
        // ownership taken, pop from stack:
        // try to serialize owned object
        if ( WriteDatabase() != KErrNone)
            {
            retVal = KErrWrite;
            }
        }

    return retVal;
    }
    

EXPORT_C void CSenBaseIdentityManager::UpdateTouchIdentityDBL(MSenServiceDescription& asd)
	{
	CSenIdentityProvider* Idp = NULL;
	
	Idp = iIdentity->IdentityProviderL(asd);
	if(Idp != NULL)
		{
		TUint32 current_tick(0);
		TBuf8<32> tickBuf;
				
		CSenElement& IdpElem = Idp->AsElement();
		current_tick = User::NTickCount();
		tickBuf.Num(current_tick);
		IdpElem.AddAttrL(KTouch(), tickBuf);
		}
//	CleanupUnusedIdentityDBL();   
                                  // (don't de-serialize old items). Serialized 
	                              // objects cannot be de-allocated on the fly.
	}
	
void CSenBaseIdentityManager::CleanupUnusedIdentityDBL()
	{
	TInt count(0);
	
	RPointerArray<CSenElement> elemList;
	CleanupClosePushL(elemList);
	
	CSenElement& element = iIdentity->AsElement();
	element.ElementsL(elemList, KIdentityProvider);
	count = elemList.Count(); 
	// There can be many Identity Provider elements within Identity element
	for(TInt i = 0; i < count; i++)
		{
		CSenElement* elem = elemList[i];
		
		const TDesC8* attrValue = elem->AttrValue(KTouch());
		if(attrValue != NULL)
			{
			TUint32 current_tick(0);
			TUint32 db_ticks(0);
			TUint32 diff_ticks(0);
					
			TLex8 lex;
            lex.Assign(*attrValue);
            lex.Val(db_ticks, EDecimal);
            
            current_tick = User::NTickCount();
            diff_ticks = current_tick - db_ticks;
            if(KMaxTicks <= diff_ticks || current_tick < db_ticks)
				{
				TInt endpointCount(0);
				_LIT8(KEndpoint, "Endpoint");
												
				RPointerArray<CSenElement> endpointElemList;
				CleanupClosePushL(endpointElemList);
				
				elem->ElementsL(endpointElemList, KEndpoint);
				endpointCount = endpointElemList.Count();
				
				if(endpointCount > 0)
					{
					CSenIdentityProvider* pMatch = NULL;
					
					CSenElement* endpointElem = endpointElemList[0];
					TPtrC8 endpoint = endpointElem->Content();
					pMatch = IdentityProviderL(endpoint);
					if(pMatch != NULL)
						{
						// Ownership ?
						UnregisterIdentityProviderL(*pMatch);
						}
					}
				CleanupStack::PopAndDestroy(&endpointElemList);
				}
			}
		}
	CleanupStack::PopAndDestroy(&elemList);
	}
			
// End of file
