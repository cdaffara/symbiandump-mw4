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








#include "sencredentialmanager.h"
#include "seninternalcredential.h"

#include <s32mem.h>
#include <s32file.h>
#include <sysutil.h>
#include <SenXmlProperties.h>
#include <SenXmlConstants.h>

#include "sendebug.h"               // internal \Utils\inc - filelogging MACROs
#include "senlogger.h"
#include "msencoreservicemanager.h" // internal \Framework\inc

#include "senservicemanagerdefines.h" // KMaxTicks (1209600000)
                                      // 1 tick = 1 millisec
                                      // 86400 sec = 1 day
                                      // 86400*1000 millisec = 1 day
                                      // 86400*14*1000 = 1209600000 millisec = 2 weeks

#include "SenParser.h"
#include "SenDateUtils.h"
#include "senwsdescription.h"
#include "senwebservicesession.h"

#include <SenServiceConnection.h> // KErrSenNoPermission ( -30315 )
#include <xml/dom/xmlengnodelist.h> 
#include <SenCredential.h>

#include <SenXmlUtils.h>
#ifdef __CRYPTO_HW__
#include "sencryptoutils.h"
#include <SecEnv.h>
#include <SecEnvDevice.h>
#include <SecEnvType.h>
#include <SecEnvMgr.h>
const TInt KCryptoPADataLen     = 16;
const TInt KCryptoPACipherLen   = 48;
const TInt KPaCryptAuthLen     = 32;
const TInt KEncryptNoSalt = 3;
const TInt KDecrypt       = 2;
const TUint KPadding = 0;
const TUint KEncrypedLen = 128;
_LIT8(KCryptoPA, "PA_CRYPT");
#endif // __CRYPTO_HW__

namespace
    {
#if !defined( EKA2 ) && !defined( RD_SECURE_PRIV_DATA )
    _LIT(KCredentialFile,           "C:\\system\\data\\sencredentials.xml");
#else
    _LIT(KCredentialFile,           "sencredentials.xml");
#endif

    _LIT8(KCredentials,             "Credentials");
    _LIT8(KCredentialsMaxID,        "CredentialsMaxID");
    _LIT8(KCredentialContainer,     "CredentialContainer");
    _LIT8(KCredentialIdentifier,    "Identifier");
    _LIT8(KCredentialProperties,    "Properties");
    _LIT8(KTouch,    "touch");
    _LIT8(KCredentialId, "CredentialId");
    
    const TInt KStateParsingCredentialContainer   = 99;
    const TInt KStateParsingCredentialIdentifier  = 100;
    const TInt KStateParsingCredentialProperties  = 102;
    const TInt KStateParsingCredential            = 104;
    const TInt KFlatBufSize                       = 128;
    
    /* microseconds before actual notOnOrAfter (or validUntil) time
     * when credentials are treated as expired.
     */
    const TInt KClockSlipMicroSeconds = 3*60*1000*1000;
    }

EXPORT_C CSenCredentialManager* CSenCredentialManager::NewL(MSenCoreServiceManager& aManager)
    {
    CSenCredentialManager* pNew = NewLC(aManager);
    CleanupStack::Pop(pNew);
    return(pNew);
    }

EXPORT_C CSenCredentialManager* CSenCredentialManager::NewLC(MSenCoreServiceManager& aManager)
    {
    CSenCredentialManager* pNew = new (ELeave) CSenCredentialManager(aManager);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

CSenCredentialManager::CSenCredentialManager(MSenCoreServiceManager& aManager)
: iManager(aManager),
  iLog((RFileLogger*)aManager.Log()),
  iMaxCredentialID(0)
    {
    }

EXPORT_C CSenCredentialManager::~CSenCredentialManager()
    {
    SaveCredentialDB();
    
    TInt count = iCredentialArray.Count();
    for (TInt i=0; i < count; i++)
        {
        iCredentialArray[i].Close();
        }
    iCredentialArray.Close();
    
    delete ipParser;
    
    // Sanity check for Delegates (which are not handled correctly
    //                             during parsing)
    delete ipCredentialContainer;
    delete ipCredentialIdentifier;
    delete ipCredentialProperties;
    delete ipCredential;
    }

void CSenCredentialManager::ConstructL()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCredentialManager::ConstructL");
    BaseConstructL(KCredentials);
    LoadDB();
    }

RFileLogger* CSenCredentialManager::Log()
    {
    return iLog;
    }
    
void CSenCredentialManager::OnStartElementL(const RTagInfo& aElement,
                                            const RAttributeArray& aAttributes,
                                            TInt aErrorCode)
    {
    const TPtrC8 localName   = aElement.LocalName().DesC();
    
    switch (iState)
        {
        case KSenStateSave:
            {
            const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
            
            if (saxLocalName == KCredentialContainer)
                {
                if ( ipCredentialContainer )
                    {
                    TXmlEngElement element  = ipCredentialContainer->AsElementL();
                    element.Remove(); // Deletes Container from Dom tree
                    delete ipCredentialContainer;
                    ipCredentialContainer = NULL;
                    }
                    
                const TPtrC8 saxNsUri       = aElement.Uri().DesC();
                const TPtrC8 saxPrefix      = aElement.Prefix().DesC();

                TXmlEngElement element = AsElementL();
                RSenDocument   document = AsDocumentL();

				ipCredentialContainer = CSenFragmentBase::NewL(
					                         saxNsUri, saxLocalName,
					                         saxPrefix, aAttributes,
					                         element, document 
					                         );
					                         
                iState = KStateParsingCredentialContainer;
                }

            break;
            }
        case KStateParsingCredentialContainer: // KStateParsingCredentialContainer 
                                               // is also Save state
            {
            const TPtrC8 saxLocalName   = aElement.LocalName().DesC();

            if (saxLocalName == KCredentialIdentifier)
                {
                if ( ipCredentialIdentifier )
                    {
                    TXmlEngElement element  = ipCredentialIdentifier->AsElementL();
                    element.Remove(); // Deletes Identifier from Dom tree
                    delete ipCredentialIdentifier;
                    ipCredentialIdentifier = NULL;
                    }
                
                const TPtrC8 saxNsUri       = aElement.Uri().DesC();
                const TPtrC8 saxPrefix      = aElement.Prefix().DesC();

                TXmlEngElement element  = ipCredentialContainer->AsElementL();
                RSenDocument   document = AsDocumentL();

				ipCredentialIdentifier = CSenCredentialIdentifier::NewL(
					                         saxNsUri, saxLocalName,
					                         saxPrefix, aAttributes,
					                         element, document 
					                         );

                iState = KStateParsingCredentialIdentifier;

                OnDelegateParsingL(*ipCredentialIdentifier);
                }
            else if (saxLocalName == KCredentialProperties)
                {
                if ( ipCredentialProperties )
                    {
                    TXmlEngElement element  = ipCredentialProperties->AsElementL();
                    element.Remove(); // Deletes Properties from Dom tree
                    delete ipCredentialProperties;
                    ipCredentialProperties = NULL;
                    }
                
                const TPtrC8 saxNsUri       = aElement.Uri().DesC();
                const TPtrC8 saxPrefix      = aElement.Prefix().DesC();

                TXmlEngElement element  = ipCredentialContainer->AsElementL();
                RSenDocument   document = AsDocumentL();

				ipCredentialProperties = CSenCredentialProperties::NewL(
					                         saxNsUri, saxLocalName,
					                         saxPrefix, aAttributes,
					                         element, document 
					                         );

                iState = KStateParsingCredentialProperties;

                OnDelegateParsingL(*ipCredentialProperties);
                }
            else 
                {
                // Parsing Credential
                // 
                // <=> Container can have only three elements:
                //     1) CredentialIdentifier,
                //     2) CredentialProperties and
                //     3) Credential (which LocalName we do not know)
                //
                //     If we see that LocalName was not CredentialIdentifier
                //                                      or CredentialProperties
                //     we know that we are handling Credential.
                
                if ( ipCredential )
                    {
                    TXmlEngElement element  = ipCredential->AsElementL();
                    element.Remove(); // Deletes Credential from Dom tree
                    delete ipCredential;
                    ipCredential = NULL;
                    }
                
                const TPtrC8 saxNsUri       = aElement.Uri().DesC();
                const TPtrC8 saxPrefix      = aElement.Prefix().DesC();

                TXmlEngElement element  = ipCredentialContainer->AsElementL();
                RSenDocument   document = AsDocumentL();

				ipCredential = CSenInternalCredential::NewL(
					                         saxNsUri, saxLocalName,
					                         saxPrefix, aAttributes,
					                         element, document 
					                         );
		        ipCredential->SetIdentifier(ipCredentialIdentifier);
		        ipCredentialIdentifier = NULL;

		        ipCredential->SetProperties(ipCredentialProperties);
		        ipCredentialProperties = NULL;

                iState = KStateParsingCredential;

                OnDelegateParsingL(*ipCredential);
                }
            break;
            }
        default:
            {
            const TPtrC8 saxLocalName   = aElement.LocalName().DesC();

            if (saxLocalName == KCredentials)
                {
                TInt count(aAttributes.Count());

                for (TInt i=0; i<count; i++)
                    {
                    TPtrC8 localName = aAttributes[i].Attribute().LocalName().DesC();
                    TPtrC8 value = aAttributes[i].Value().DesC();
                    
                    if ( localName == KCredentialsMaxID )
                        {
                        TLex8 lex;
                        lex.Assign(value);
                        lex.Val( iMaxCredentialID );                        
                        break;
                        }
                    }
                }
                
            CSenFragmentBase::OnStartElementL( aElement, aAttributes, aErrorCode );
            break;
            }
        }
    }
    
void CSenCredentialManager::OnEndElementL( const RTagInfo& aElement, TInt aErrorCode )

    {
    const TPtrC8 localName   = aElement.LocalName().DesC();

    switch(iState)
        {
        case KStateParsingCredentialIdentifier:
            {
#ifdef __CRYPTO_HW__

            TInt leaveCode(KErrNone);  
 			TPtrC8 pass;
            TBuf8<KEncrypedLen> decPwd;
            ipCredentialIdentifier->PropertyL(KSenIdpPasswordLocalname, pass);
            TRAP(leaveCode, DecryptPasswordL(pass, decPwd));
            if (leaveCode == KErrNone)
                {
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("SenCredentialManager::DecryptPasswordL success")));
                }
            else
                {
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("SenCredentialManager::DecryptPasswordL Failed"))); 
                }
			ipCredentialIdentifier->SetPropertyL(KSenIdpPasswordLocalname, decPwd);       			
			
#endif // __CRYPTO_HW__			
            iState = KStateParsingCredentialContainer;
            break;
            }
        case KStateParsingCredentialProperties:
            {
            iState = KStateParsingCredentialContainer;
            break;
            }
        case KStateParsingCredential:
            {
            TTime now;
            now.UniversalTime();
            
            TPtrC8 value;
            TTimeIntervalMicroSeconds clientServerInterval = 0;
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"Checking 'ServiceInterval' property.");
            _LIT8(KServiceInterval,"ServiceInterval");	//CodeScannerWarnings
            TInt ret = ipCredential->PropertiesL().PropertyL(KServiceInterval, value);
            
            TBool propertiesExist(ETrue);
            
            if ( ret == KErrNone )
                {
                TLex8 lex(value);
                TInt64 val;
                ret = lex.Val(val);
                TTimeIntervalMicroSeconds clientServerInterval = val;
                if ( ret == KErrNone )
                    {
#ifdef _SENDEBUG
                    TInt leaveCode(KErrNone);
                    TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
                    TRAP(leaveCode, SenDateUtils::ToXmlDateTimeUtf82L(ts, now);)
                    if (leaveCode == KErrNone)
                        {
                        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("Client time : %S"), &ts));
                        }
#endif // _SENDEBUG
                    now += clientServerInterval;
#ifdef _SENDEBUG
                    TBuf8<64> buf;
                    buf.AppendNum(clientServerInterval.Int64());
                    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("Client-Server Interval in microseconds: %S"),&buf));
                    TRAP(leaveCode, SenDateUtils::ToXmlDateTimeUtf82L(ts, now);)
                    if (leaveCode == KErrNone)
                        {
                        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("Fixed client time : %S"), &ts));
                        }
                    leaveCode = 0; // not used
#endif // _SENDEBUG
                    }
                }
            else if ( ret == KErrNotFound )
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- 'ServiceInterval' not found.");
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"Checking 'ValidUntil' property.");
                TInt ret2 = ipCredential->PropertiesL().PropertyL(_L8("ValidUntil"), value);
                if ( ret2 == KErrNotFound )
                    {
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- 'ValidUntil' not found.");
                    propertiesExist = EFalse;
                    }
                }                
            
            if ( !propertiesExist ||
                ( ( ipCredential->PropertiesL().ValidUntilL() != Time::NullTTime() ) &&
                 ( now > (ipCredential->PropertiesL().ValidUntilL()-TTimeIntervalMicroSeconds(KClockSlipMicroSeconds)) ) )
               )
                {
                
#ifdef _SENDEBUG
                if ( propertiesExist )
                    {
                    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("- Loaded credential is expired:")));
                    TInt leaveCode(KErrNone);
                    TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts2;
                    TRAP(leaveCode, SenDateUtils::ToXmlDateTimeUtf82L(ts2, ipCredential->PropertiesL().ValidUntilL());)//codescannerwarnings
                    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- Credential time : %S, clockslip: %d micros"), &ts2, KClockSlipMicroSeconds));
                    leaveCode = 0; // not used
                    }
                else
                    {
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Neither 'ValidUntil' or 'ServiceInterval' exist: credential should be discarded(!)");
                    }
#endif // _SENDEBUG
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("-- Invalid credential will be removed from CredentialManager.")));

                
                delete ipCredential;            
                ipCredential = NULL;         
                }
            else
                {
                if ( ipCredential->PropertiesL().ValidUntilL() != Time::NullTTime() )
                    {
                    TTime validUntil = ipCredential->PropertiesL().ValidUntilL();
                    ipCredential->SetValidUntil(validUntil);
                    }
                
                RSenCredentialPtr credentialPtr;
                credentialPtr.OpenL(ipCredential);
                CleanupClosePushL(credentialPtr);
                iCredentialArray.Append(credentialPtr);
                CleanupStack::Pop(&credentialPtr);
                }
                
            ipCredential = NULL;

            iState = KStateParsingCredentialContainer;
            break;
            }
        case KStateParsingCredentialContainer:
            {
            if(localName == KCredentialContainer)
            	{
            if ( ipCredentialContainer )
                {
                TXmlEngElement element  = ipCredentialContainer->AsElementL();
                RXmlEngNodeList<TXmlEngElement> children;
                
                CleanupClosePushL(children);
                element.GetChildElements(children);
    
                // Element should always have 3 child elements:
                //         1) CredentialIdentifier and
                //         2) Credential
                //         3) CredentialProperties
                if ( children.Count() < 2 )
                    {
                    if ( ipCredentialIdentifier )
                        {
                        TXmlEngElement element  = ipCredentialIdentifier->AsElementL();
                        element.Remove(); // Deletes Identifier from Dom tree
                        delete ipCredentialIdentifier;
                        ipCredentialIdentifier = NULL;
                        }
                    if ( ipCredentialProperties )
                        {
                        TXmlEngElement element  = ipCredentialProperties->AsElementL();
                        element.Remove(); // Deletes Identifier from Dom tree
                        delete ipCredentialProperties;
                        ipCredentialProperties = NULL;
                        }
                    if ( ipCredential )
                        {
                        TXmlEngElement element  = ipCredential->AsElementL();
                        element.Remove(); // Deletes Credential from Dom tree
                        delete ipCredential;
                        ipCredential = NULL;
                        }
                    element.Remove();
                    }
                CleanupStack::PopAndDestroy(&children);

                delete ipCredentialContainer;
                ipCredentialContainer = NULL;
                if ( ipContentBuf )
    				{
	                
    		    	delete ipContentBuf;
        			ipContentBuf = NULL;
        
        			delete ipContentWriteStream;
        			ipContentWriteStream = NULL;
        			}
                }
                
            iState = KSenStateSave;
            break;
            	}
            	else
            	{
            		return;
            	}
            }
        default:
            {
            CSenFragmentBase::OnEndElementL(aElement, aErrorCode);
            
            break;
            }
        }
    }    
    
TInt CSenCredentialManager::LoadDB()
    {
    TInt retVal(KErrNone);
	if ( !ipParser )
        {
        TRAP( retVal, ipParser = CSenParser::NewL(); )
        if( retVal == KErrNone )
            {
            ipParser->EnableFeature(EReportNamespaceMapping);
            }
        }
    if( retVal == KErrNone )
        {
        TRAP(retVal, LoadFromL(KCredentialFile));
        }
    return retVal;
    }

void CSenCredentialManager::LoadFromL(const TDesC& aFile)
    {
    RFs fss;
    User::LeaveIfError(fss.Connect());
    CleanupClosePushL(fss);

    TInt leaveCode(KErrNone);

#if defined( EKA2 ) || defined( RD_SECURE_PRIV_DATA )
    TBuf<KMaxPath> file;
    fss.CreatePrivatePath(EDriveC);
    fss.PrivatePath(file);
    file.Append(aFile);
    TRAP(leaveCode, ipParser->ParseL(fss, file, *this));
#else
    TRAP(leaveCode, aReader.ParseL(fss, aFile, *this));
#endif

    iState = KSenStateSave;
    
#ifdef _SENDEBUG
    if(leaveCode == KErrNotFound)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- The sencredentials.xml does not exist!");
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- New file will be generated.");
        }
    else if(leaveCode != KErrNone)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Parsing of sencredentials.xml leaved: %d"), leaveCode));
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Credential database (sencredentials.xml) successfully read.");
        }
#else
    leaveCode=0; // this is a decision: we are not interested
                 // of invalid input in read/parse phase
#endif

    SaveCredentialDB(); // Always validate by saving

    CleanupStack::PopAndDestroy(); // fss
    }

TInt CSenCredentialManager::SaveCredentialDB()
    {
    TInt retVal(KErrNone);
    TRAP(retVal, SaveToL(KCredentialFile));
    return retVal;
    }

TInt CSenCredentialManager::SaveToL(const TDesC& aFile)
    {
    TInt count = iCredentialArray.Count();
    TInt maxId = 0;
#ifdef __CRYPTO_HW__

	RArray<RBuf8 *> password;
	TPtrC8 tempPass;
	TInt leaveCode(KErrNone);

#endif // __CRYPTO_HW__
    for (TInt i=0; i<count; i++)
        {
#ifdef __CRYPTO_HW__		
        iCredentialArray[i].Credential()->IdentifierL().PropertyL(KSenIdpPasswordLocalname, tempPass);
        RBuf8 *pTempBuf = new RBuf8;
        pTempBuf->Create(tempPass);
        password.Append(pTempBuf);
                
        leaveCode = KErrNone;
        TBuf8<KEncrypedLen> encPwd;
        TRAP(leaveCode, EncryptPasswordL(tempPass, encPwd));
        if (leaveCode == KErrNone)
            {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("SenCredentialManager::EncryptPasswordL success")));
            }
        else
            {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("SenCredentialManager::EncryptPasswordL Failed")));
            }
       // EncryptPasswordL(tempPass, encPwd);
        iCredentialArray[i].Credential()->IdentifierL().SetPropertyL(KSenIdpPasswordLocalname, encPwd);
#endif // __CRYPTO_HW__		
        if ( iCredentialArray[i].Credential()->IdentifierL().IdL() > maxId )
            {
            maxId = iCredentialArray[i].Credential()->IdentifierL().IdL();
            }
        }
    iMaxCredentialID = maxId;
    UpdateMaxCredentialIdL();

    // First, collect everything into MEMORY
    CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
    CleanupStack::PushL(pBuf);

    RBufWriteStream bufWs(*pBuf);
    CleanupClosePushL(bufWs);

    WriteAsXMLToL(bufWs);

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
#ifdef __CRYPTO_HW__
    for (TInt i=0; i<count; i++)
        {
        iCredentialArray[i].Credential()->IdentifierL().SetPropertyL(KSenIdpPasswordLocalname, *password[i]);
        password[i]->Close();
        delete password[i];
        }
#endif // __CRYPTO_HW__	
    return KErrNone;
    }
    
TInt CSenCredentialManager::CredentialsL(const CSenWSDescription& aPattern,
								         RSenCredentialArray& aCredentials)
    {
    TInt retVal(KErrNotFound);
 //   RefreshMTL(aPattern);
    
    TInt count = iCredentialArray.Count();
    
    for (TInt i=0; i<count; i++)
        {
        if(IsApplicableOrShareableL(aPattern, iCredentialArray[i].Credential()))
//        if ( iCredentialArray[i].Credential()->IsApplicable(aPattern) )
            {
            TPtrC8 providerId;
            retVal = iCredentialArray[i].Credential()->IdentityProviderIdL(providerId);
            if ( retVal == KErrNone )
                {
                CSenWSDescription* pPattern = CSenWSDescription::NewLC();
                pPattern->SetEndPointL(providerId);
                CSenIdentityProvider* pProvider = iManager.IdentityProviderL(*pPattern);
                CleanupStack::PopAndDestroy(pPattern);
                if ( pProvider )
                    {
                    if ( ( pProvider->UserName() == KNullDesC8 ) &&
                         ( pProvider->Password() == KNullDesC8 ) )
                        {
                        aCredentials.AppendL(iCredentialArray[i].Credential());
                        }
                    else
                        {
                        retVal = KErrSenNoPermission;
                        }
                    }
                else
                    {
                    // Note: Serene DB should not contain Credential
                    //       which has IdentityProviderId which points
                    //       to non-existent IdentityProvider.
                    // After all we cannot give access to Credential
                    // which has IdentityProviderId, even though actual
                    // IdentityProvider can not be found anymore.
                    retVal = KErrSenNoPermission;
                    }
                }
            else
                {
                aCredentials.AppendL(iCredentialArray[i].Credential());                
                }
            }
        }

    if ( aCredentials.Count() == 0 )
        {
        return retVal;
        }
        
    return KErrNone;
    }

TBool CSenCredentialManager::IsAuto(CSenIdentityProvider& aIdp)
    {
    CSenElement* el(NULL);
    TPtrC8 ext = aIdp.AccountExtensions(el);
    if (ext == KNullDesC8)
        {
        if (el)
            {
            el = el->Element(KSenAccAutoSignIn);
            if (el && el->Content() == KSenPropertyTrue)
                {
                return ETrue;
                }
            }
        }
    else
        {
        RBuf8 mask;
        mask.Create(KSenAccAutoSignIn().Length() +
                    KSenGreaterThan().Length() +
                    KSenPropertyTrue().Length());
        mask.CleanupClosePushL();
        mask.Append(KSenAccAutoSignIn);
        mask.Append(KSenGreaterThan);
        mask.Append(KSenPropertyTrue);
        if (ext.FindF(mask) != KErrNotFound)
            {                                    
            return ETrue;
            }
        CleanupStack::PopAndDestroy(&mask);
        }          
    return EFalse;
    }
TInt CSenCredentialManager::CredentialsL(const CSenWSDescription& aPattern,
								         const CSenIdentityProvider& aIdP,
								         RSenCredentialArray& aCredentials)
    {
 //   RefreshMTL(aPattern);
    TInt count = iCredentialArray.Count();
    for (TInt i=0; i<count; i++)
        {
        if(IsApplicableOrShareableL(aPattern, iCredentialArray[i].Credential()))
//        if ( iCredentialArray[i].Credential()->IsApplicable(aPattern) )
            {
            TPtrC8 providerId;
            TInt retVal = iCredentialArray[i].Credential()->IdentityProviderIdL(providerId);
            
            if ( retVal == KErrNone )
                {
                CSenWSDescription* pPattern = CSenWSDescription::NewLC();
                pPattern->SetEndPointL(providerId);
                CSenIdentityProvider* pProvider = iManager.IdentityProviderL(*pPattern);
                CleanupStack::PopAndDestroy(pPattern);
                CSenCredentialIdentifier& identifier = iCredentialArray[i].Credential()->IdentifierL();
                
                TPtrC8 usernameIdent;
                TInt retVal = identifier.PropertyL(KSenIdpAuthzIDLocalname, usernameIdent);
                TPtrC8 passwordIdent;
                retVal = identifier.PropertyL(KSenIdpPasswordLocalname, passwordIdent);
                
                TPtrC8 usernameProv  =  ((CSenIdentityProvider&)aIdP).UserName();
                TPtrC8 passwordProv  = ((CSenIdentityProvider&)aIdP).Password();

                HBufC8* decPasswordIden = SenXmlUtils::DecodeHttpCharactersLC(passwordIdent);
                HBufC8* decPasswordProv = SenXmlUtils::DecodeHttpCharactersLC(passwordProv);          
                
                if( ( usernameIdent.Length() == 0 || usernameIdent == usernameProv ) &&
                        (
                        ( decPasswordIden->Length() == 0 || *decPasswordIden == *decPasswordProv ) 
                        ||
                        (IsAuto(*pProvider) && *decPasswordProv == KNullDesC8)
                        )
                    )
                    {
                    aCredentials.AppendL(iCredentialArray[i].Credential());
                    }
                CleanupStack::PopAndDestroy(2); // decode x2
                }
            }
        }
        
    if ( aCredentials.Count() == 0 )
        {
        return KErrNotFound;
        }
        
    return KErrNone;
    }

TInt CSenCredentialManager::CredentialsL(const CSenWSDescription& aPattern,
								         RSenCredentialPtrArray& aCredentials)
    {
    TInt retVal(KErrNotFound);
    
    TInt count = iCredentialArray.Count();
    
    for (TInt i=0; i<count; i++)
        {
        if(IsApplicableOrShareableL(aPattern, iCredentialArray[i].Credential()))
//        if ( iCredentialArray[i].Credential()->IsApplicable(aPattern) )
            {
            TPtrC8 providerId;
            retVal = iCredentialArray[i].Credential()->IdentityProviderIdL(providerId);
            if ( retVal == KErrNone )
                {
                CSenWSDescription* pPattern = CSenWSDescription::NewLC();
                pPattern->SetEndPointL(providerId);
                CSenIdentityProvider* pProvider = iManager.IdentityProviderL(*pPattern);
                CleanupStack::PopAndDestroy(pPattern);
                if ( pProvider )
                    {
                    if ( ( pProvider->UserName() == KNullDesC8 ) &&
                         ( pProvider->Password() == KNullDesC8 ) )
                        {
                        aCredentials.AppendL(iCredentialArray[i]);
                        }
                    else
                        {
                        retVal = KErrSenNoPermission;
                        }
                    }
                else
                    {
                    // Note: Serene DB should not contain Credential
                    //       which has IdentityProviderId which points
                    //       to non-existent IdentityProvider.
                    // After all we cannot give access to Credential
                    // which has IdentityProviderId, even though actual
                    // IdentityProvider can not be found anymore.
                    retVal = KErrSenNoPermission;
                    }
                }
            else
                {
                aCredentials.AppendL(iCredentialArray[i]);                
                }
            }
        }

    if ( aCredentials.Count() == 0 )
        {
        return retVal;
        }
        
    return KErrNone;
    }

TInt CSenCredentialManager::CredentialsL(const CSenWSDescription& aPattern,
								         const CSenIdentityProvider& aIdP,
								         RSenCredentialPtrArray& aCredentials)
    {
    TInt count = iCredentialArray.Count();
    for (TInt i=0; i<count; i++)
        {
        if(IsApplicableOrShareableL(aPattern, iCredentialArray[i].Credential()))
//        if ( iCredentialArray[i].Credential()->IsApplicable(aPattern) )
            {
            TPtrC8 providerId;
            TInt retVal = iCredentialArray[i].Credential()->IdentityProviderIdL(providerId);
            if ( retVal == KErrNone )
                {
                /*CSenWSDescription* pPattern = CSenWSDescription::NewLC();
                pPattern->SetEndPointL(providerId);
                CSenIdentityProvider* pProvider = iManager.IdentityProviderL(*pPattern);
                CleanupStack::PopAndDestroy(pPattern);
                if ( pProvider )
                    {
                    if ( ( ((CSenIdentityProvider&)aIdP).UserName() == pProvider->UserName() ) &&
                         ( ((CSenIdentityProvider&)aIdP).Password() == pProvider->Password() ) )
                        {
                        aCredentials.AppendL(iCredentialArray[i]);
                        }
                    }*/
                CSenCredentialIdentifier& identifier = iCredentialArray[i].Credential()->IdentifierL();

                TPtrC8 username;
                TInt retVal = identifier.PropertyL(KSenIdpAuthzIDLocalname, username);
                TPtrC8 passwordId;
                retVal = identifier.PropertyL(KSenIdpPasswordLocalname, passwordId);
                HBufC8* decPasswordId = SenXmlUtils::DecodeHttpCharactersLC(passwordId);
                
                TPtrC8 passwordProv  = ((CSenIdentityProvider&)aIdP).Password();
                HBufC8* decPasswordProv = SenXmlUtils::DecodeHttpCharactersLC(passwordProv);
                                
                if( ( username.Length() == 0 || username == ((CSenIdentityProvider&)aIdP).UserName() ) &&
                    ( decPasswordId->Length() == 0 || *decPasswordId ==  *decPasswordProv) )
                    {
                    aCredentials.AppendL(iCredentialArray[i]);
                    }
                CleanupStack::PopAndDestroy(2); // encodehttp
                }
            }
        }
        
    if ( aCredentials.Count() == 0 )
        {
        return KErrNotFound;
        }
        
    return KErrNone;
    }

RSenCredentialPtr CSenCredentialManager::AddCredentialL(CSenInternalCredential* apCredential,
                                                        TInt& aErrorTo)
    {
    if ( !apCredential )
        {
        SENDEBUG_L("AddCredentialL(*) - Illegal argument: NULL");
        aErrorTo = KErrArgument;
        return RSenCredentialPtr();
        }
        
    CleanupStack::PushL(apCredential);
    RSenCredentialPtr credentialPtr;
    credentialPtr.OpenL(apCredential);
    CleanupStack::Pop(apCredential);
    CleanupClosePushL(credentialPtr);
    iCredentialArray.AppendL(credentialPtr);
    CleanupStack::Pop(&credentialPtr);
    
    TXmlEngElement element = this->AsElementL().AddNewElementL(KCredentialContainer);
    apCredential->IdentifierL().AsElementL().MoveTo(element);
    apCredential->IdentifierL().SetDocument(AsDocumentL());
    apCredential->IdentifierL().SetIdL(NextCredentialIdL());
    apCredential->PropertiesL().AsElementL().MoveTo(element);
    apCredential->PropertiesL().SetDocument(AsDocumentL());
    apCredential->AsElementL().MoveTo(element);
    apCredential->SetDocument(AsDocumentL());
    
    SaveCredentialDB();
    
    aErrorTo = KErrNone;
    return credentialPtr;
    }

RSenCredentialPtr CSenCredentialManager::AddCredentialL( CSenIdentityProvider* apIdP,
									                     CSenInternalCredential* apCredential,
									                     TInt& aErrorTo )
    {
    CleanupStack::PushL(apCredential);
    CleanupStack::PushL(apIdP);
    
    if ( !apCredential )
        {
        SENDEBUG_L("AddCredentialL(*) - Illegal argument: NULL");
        CleanupStack::PopAndDestroy(apIdP);
        CleanupStack::PopAndDestroy(apCredential);
        return RSenCredentialPtr();
        }
        
    CSenIdentityProvider* pIdP = NULL;
    // Note: Following function returns either KErrNotFound or the index of found
    //       IdentityProvider (index >= 0).
    TInt retVal = ((MSenIdentityManager&)iManager).FindMatchingIdentityProviderL(*apIdP, pIdP);
    if ( retVal > KErrNone )
        {
        retVal = KErrNone;
        }

    if ( retVal == KErrNotFound )
        {
        CleanupStack::PopAndDestroy(apIdP);
        CleanupStack::PopAndDestroy(apCredential);
        aErrorTo = retVal;
        return RSenCredentialPtr();
        }
    else
        {
        CleanupStack::PopAndDestroy(apIdP);
        TPtrC8 providerId = pIdP->ProviderID();
        apCredential->SetIdentityProviderIdL(providerId);        
        TPtrC8 userName = pIdP->UserName();
        if(userName.Length() > 0)
	        {
			TPtrC8 password = pIdP->Password();	        	
			apCredential->SetAuthInfo(userName, password);        			
	        }        	
        }
        
    RSenCredentialPtr credentialPtr;
    credentialPtr.OpenL(apCredential);
    CleanupStack::Pop(apCredential);
    CleanupClosePushL(credentialPtr);
    iCredentialArray.AppendL(credentialPtr);
    CleanupStack::Pop(&credentialPtr);


    TXmlEngElement element = this->AsElementL().AddNewElementL(KCredentialContainer);
    apCredential->IdentifierL().AsElementL().MoveTo(element);
    apCredential->IdentifierL().SetDocument(AsDocumentL());
    apCredential->IdentifierL().SetIdL(NextCredentialIdL());
    apCredential->PropertiesL().AsElementL().MoveTo(element);
    apCredential->PropertiesL().SetDocument(AsDocumentL());
    apCredential->AsElementL().MoveTo(element);
    apCredential->SetDocument(AsDocumentL());
    
    SaveCredentialDB();
    
    aErrorTo = retVal;
    return credentialPtr;
    }

RSenCredentialPtr CSenCredentialManager::AddCredentialL(const TDesC8& aCredential,
                                                         TInt& aErrorTo)
    {
    CSenInternalCredential* pCredential = CSenInternalCredential::NewLC();
    ipParser->ParseL(aCredential, *pCredential);
    CleanupStack::Pop(pCredential);

    // Following takes the ownership of pCredential    
    RSenCredentialPtr credPtr = AddCredentialL(pCredential, aErrorTo);
    
    return credPtr;
    }

RSenCredentialPtr CSenCredentialManager::AddCredentialL(CSenIdentityProvider* apIdP,
									                     const TDesC8& aCredential,
									                     TInt& aErrorTo)
    {
    CSenInternalCredential* pCredential = CSenInternalCredential::NewLC();
    ipParser->ParseL(aCredential, *pCredential);
    CleanupStack::Pop(pCredential);

    // Following takes the ownership of pCredential    
    RSenCredentialPtr credPtr = AddCredentialL(apIdP, pCredential, aErrorTo);
    
    return credPtr;
    }

// Very efficient method that removes all the credentials of certain *service*,
// independent of provider ID and/or user account
TInt CSenCredentialManager::RemoveCredentialsL(const CSenWSDescription& aPattern)
    {
    TInt retVal(KErrNotFound);
    
    TInt count = iCredentialArray.Count();
    for (TInt i=0; i<count; i++)
        {
        if ( iCredentialArray[i].Credential() && 
             iCredentialArray[i].Credential()->IsApplicableL(aPattern) )
            {
            // Get CredentialContainer element
            TXmlEngNode element = iCredentialArray[i].Credential()->AsElementL().ParentNode();
            // Delete and destroy both CredentialIdentifier and Credential
            iCredentialArray[i].CloseAndDestroyCredential();
            // Delete and destroy CredentialContainer element
            iCredentialArray.Remove(i);
            element.Remove();
            i--;
            count--;
            
            retVal = KErrNone;
            }
        }

    SaveCredentialDB();
        
    return retVal;
    }

TInt CSenCredentialManager::RemoveCredentialsL( const CSenWSDescription& aPattern,
										        const CSenIdentityProvider& aIdP )
    {
    TInt retVal(KErrNotFound);
    
    TInt count = iCredentialArray.Count();
    for (TInt i=0; i<count; i++)
        {
        if ( iCredentialArray[i].Credential() && 
             iCredentialArray[i].Credential()->IsApplicableL(aPattern) )
            {
            TPtrC8 providerId;
            retVal = iCredentialArray[i].Credential()->IdentityProviderIdL(providerId);
            if ( retVal == KErrNone )
                {
                CSenWSDescription* pPattern = CSenWSDescription::NewLC();
                pPattern->SetEndPointL(providerId);
                CSenIdentityProvider* pProvider = iManager.IdentityProviderL(*pPattern);
                CleanupStack::PopAndDestroy(pPattern);
                /*if ( pProvider )                
                    {
                    if ( ( ((CSenIdentityProvider&)aIdP).UserName() == pProvider->UserName() ) &&
                         ( ((CSenIdentityProvider&)aIdP).Password() == pProvider->Password() ) )
                        {
                        // Get CredentialContainer element
                        TXmlEngNode element = iCredentialArray[i].Credential()->AsElementL().ParentNode();
                        // Delete and destroy both CredentialIdentifier and Credential
                        
                        iCredentialArray[i].CloseAndDestroyCredential();
                        // Delete and destroy CredentialContainer element
                        iCredentialArray.Remove(i);
                        element.Remove();
                        i--;
                        count--;
                        
                        retVal = KErrNone;
                        }
                    }*/
                CSenCredentialIdentifier& identifier = iCredentialArray[i].Credential()->IdentifierL();                
                TPtrC8 username;
                TInt retVal = identifier.PropertyL(KSenIdpAuthzIDLocalname, username);
                TPtrC8 password;
                retVal = identifier.PropertyL(KSenIdpPasswordLocalname, password);
                
                HBufC8* decPasswordIden = SenXmlUtils::DecodeHttpCharactersLC(password);
                HBufC8* decPasswordProv = SenXmlUtils::DecodeHttpCharactersLC(((CSenIdentityProvider&)aIdP).Password());
                
                if( ( username.Length() == 0 || username == ((CSenIdentityProvider&)aIdP).UserName() ) &&
                        (( decPasswordIden->Length() == 0 || *decPasswordIden == *decPasswordProv ) 
                           ||
                        (IsAuto(*pProvider) && ((CSenIdentityProvider&)aIdP).Password() == KNullDesC8)))
                    {    
                    // Get CredentialContainer element
                    TXmlEngNode element = iCredentialArray[i].Credential()->AsElementL().ParentNode();
                    // Delete and destroy both CredentialIdentifier and Credential
                    
                    iCredentialArray[i].CloseAndDestroyCredential();
                    // Delete and destroy CredentialContainer element
                    iCredentialArray.Remove(i);
                    element.Remove();
                    i--;
                    count--;
                    
                    retVal = KErrNone;
                    }
                CleanupStack::PopAndDestroy(2); // decode http x2
                }
            }
        }
        
    if( retVal == KErrNone )    
        {
        // At least one credential was removed, serialize the db
        SaveCredentialDB();
        }
    return retVal;
    }
    
TInt CSenCredentialManager::RemoveCredentialsL(const TDesC8& aProviderId)
    {
    TInt retVal(KErrNotFound);
    
    TInt count = iCredentialArray.Count();
    for (TInt i=0; i<count; i++)
        {
        TPtrC8 providerId;
        if (iCredentialArray[i].Credential())
            {
            retVal = iCredentialArray[i].Credential()->IdentityProviderIdL(providerId);
            }
        if ( ( retVal == KErrNone ) && ( aProviderId == providerId ) )
            {
            CSenCredentialIdentifier& identifier = iCredentialArray[i].Credential()->IdentifierL();                
            
            TPtrC8 username;
            TInt retVal = identifier.PropertyL(KSenIdpAuthzIDLocalname, username);
            TPtrC8 password;
            retVal = identifier.PropertyL(KSenIdpPasswordLocalname, password);
            
            CSenWSDescription* pPattern = CSenWSDescription::NewLC();
            pPattern->SetEndPointL(providerId);
            CSenIdentityProvider* pProvider = iManager.IdentityProviderL(*pPattern);
            CleanupStack::PopAndDestroy(pPattern);
        
            if ( pProvider )
                {
                HBufC8* decPasswordIden = SenXmlUtils::DecodeHttpCharactersLC(password);
                HBufC8* decPasswordProv = SenXmlUtils::DecodeHttpCharactersLC(pProvider->Password());
                
                if( ( username.Length() == 0 || username == pProvider->UserName() ) &&
                    ( decPasswordIden->Length() == 0 || *decPasswordIden == *decPasswordProv ) )
                    {
                    // Permission to remove this credential is granted(!)
                    
                    // Get CredentialContainer element
                    TXmlEngNode element;
                    if (iCredentialArray[i].Credential())
                        {
                        element = iCredentialArray[i].Credential()->AsElementL().ParentNode();
                        }
                    // Delete and destroy both CredentialIdentifier and Credential
                    iCredentialArray[i].CloseAndDestroyCredential();
                    // Delete and destroy CredentialContainer element
                    iCredentialArray.Remove(i);
                    element.Remove();
                    i--;
                    count--;
                    
                    retVal = KErrNone;
                    }
                CleanupStack::PopAndDestroy(2); // decode http chars x2
                }
/*                
            else
                {
                // no IDP (account) exists, credential will be removed at some later time
                continue;
                }        
*/                    
            }
        }
    
    if( retVal == KErrNone )    
        {
        // At least one credential was removed, serialize the db
        SaveCredentialDB();
        }
    
    return retVal;
    }
    
TInt CSenCredentialManager::NextCredentialIdL()
    {
    iMaxCredentialID++;

    UpdateMaxCredentialIdL();    
    
    return iMaxCredentialID;
    }
    
void CSenCredentialManager::UpdateMaxCredentialIdL()
    {
    TBool found = EFalse;
    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngAttr> attrList;
    element.GetAttributes(attrList);
    CleanupClosePushL(attrList);
    while ( attrList.HasNext() && !found )
        {
        TXmlEngAttr attr = attrList.Next();
        if ( attr.Name() ==  KCredentialsMaxID )
            {
            TBuf8<KFlatBufSize> buffer;
            buffer.Num( iMaxCredentialID );
            attr.SetValueL(buffer);
            found = ETrue;
            }
        }
    CleanupStack::PopAndDestroy(&attrList); // Close()
    
    if ( !found )
        {
        TBuf8<KFlatBufSize> buffer;
        buffer.Num(iMaxCredentialID);
        element.AddNewAttributeL(KCredentialsMaxID, buffer);
        }
    }
    
// Very powerful method, that removes the credential without checking 
// account details (username & password), and/or provider ID 
// (low level, private method)
TInt CSenCredentialManager::RemoveCredentialL(TInt aInternalCredentialId)
    {
    TInt retVal(KErrNotFound);
    
    TInt credId( KErrNotFound );
    TInt count = iCredentialArray.Count();
    for (TInt i=0; i<count; i++)
        {
        CSenInternalCredential* pCred = iCredentialArray[i].Credential();
        if( pCred )
            {
                        
                
            //TRAP( leaveCode, credId = pCred->IdentifierL().IdL(); )
            //if( !leaveCode && (credId == aInternalCredentialId) )
            
            credId = pCred->IdentifierL().IdL();
            if( credId == aInternalCredentialId )
                {
                // Get CredentialContainer element
                TXmlEngNode element = iCredentialArray[i].Credential()->AsElementL().ParentNode();
                // Delete and destroy both CredentialIdentifier and Credential
                iCredentialArray[i].CloseAndDestroyCredential();
                // Delete and destroy CredentialContainer element
                iCredentialArray.Remove(i);
                element.Remove();
                retVal = KErrNone;
                break;
                }
            }
        }
    return retVal;
    }
        
RSenCredentialPtr CSenCredentialManager::CredentialL( TInt aInternalCredentialId,
                                                      TInt& aErrorTo )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCredentialManager::CredentialL()");
    TInt count = iCredentialArray.Count();
    for (TInt i=0; i<count; i++)
        {
        if ( iCredentialArray[i].Credential() && iCredentialArray[i].Credential()->IdentifierL().IdL() == aInternalCredentialId )
            {
            
            // Check if current account has permission to use this credential
            CSenCredentialIdentifier& identifier = iCredentialArray[i].Credential()->IdentifierL();
            
            // Collect significant properties that are checked in order to
            // grant (or not gran)t permission to consume this credential:
            TPtrC8 providerID;
            TInt retVal = iCredentialArray[i].Credential()->IdentityProviderIdL(providerID);
            //TInt retVal = identifier.PropertyL(KSenIdpProviderIdLocalname, providerID);

            TPtrC8 username;
            retVal = identifier.PropertyL(KSenIdpAuthzIDLocalname, username);
            
            TPtrC8 password;
            retVal = identifier.PropertyL(KSenIdpPasswordLocalname, password);

            if( username.Length() > 0 && password.Length() > 0 )
                {
                CSenWSDescription* pPattern = CSenWSDescription::NewLC();
                pPattern->SetEndPointL(providerID);
                CSenIdentityProvider* pProvider = iManager.IdentityProviderL(*pPattern);
                CleanupStack::PopAndDestroy(pPattern);
                if ( pProvider )
                    {
                    HBufC8* passProvDec = SenXmlUtils::DecodeHttpCharactersLC(pProvider->Password());
                    HBufC8* passIdDec = SenXmlUtils::DecodeHttpCharactersLC(password);
                    if ( ( pProvider->UserName() != username ) ||
                         ( *passProvDec != *passIdDec ) )
                        {
                        CleanupStack::PopAndDestroy(2); //decode x2
                        // userinfo does not match with stored credential's username & password
                        continue;
                        }
                    else
                       	{
                       	CleanupStack::PopAndDestroy(2); //decode x2
                       	}
                    }
                else
                    {
                    // No IDP exists, but credential was protected
                    continue; // no permission to use this credential
                    }
                }
            // either credential is not protected in fist place, or both password and username match
            // (permission was granted)
            aErrorTo = KErrNone;
            return iCredentialArray[i];
            }
        }
    aErrorTo = KErrNotFound;
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCredentialManager::CredentialL() Completed"); 
    return RSenCredentialPtr();
    }

RSenCredentialPtr CSenCredentialManager::UpdateCredentialL( TInt aInternalCredentialId,
                                                            CSenInternalCredential* apCredential,
                                                            TInt& aErrorTo )
    {
    // Call to CredentialL (search method) will check the userinfo (account) /
    // permission to manipulate the credential
    RSenCredentialPtr credPtr = CredentialL( aInternalCredentialId, aErrorTo );
    if ( aErrorTo == KErrNone )
        {
        // Get CredentialContainer element
        if (credPtr.Credential())
            {
            TXmlEngNode containerElement =
                credPtr.Credential()->AsElementL().ParentNode();
            // Get Credential element
            TXmlEngNode element = 
                credPtr.Credential()->AsElementL();
            element.Remove();
            credPtr.SetCredential(apCredential);
            apCredential->AsElementL().MoveTo(containerElement);
            apCredential->SetDocument(AsDocumentL());
            aErrorTo = KErrNone;
            }
        }
    return credPtr;
    }
        
RSenCredentialPtr CSenCredentialManager::UpdateCredentialL( TInt aInternalCredentialId,
                                                            const TDesC8& aCredential,
                                                            TInt& aErrorTo )
    {
    CSenInternalCredential* pCredential = CSenInternalCredential::NewLC();
    ipParser->ParseL(aCredential, *pCredential);

    // Following takes the ownership of pCredential    
    RSenCredentialPtr credPtr = UpdateCredentialL( aInternalCredentialId,
                                                   pCredential, 
                                                   aErrorTo );
    CleanupStack::Pop(pCredential);
    
    return credPtr;
    }

TBool CSenCredentialManager::IsApplicableOrShareableL(const CSenWSDescription& aPattern,CSenInternalCredential* aCredential)
    {
    TBool retVal = ETrue;
    if (aCredential && !aCredential->IsApplicableL(aPattern))	//codescannerwarings
        {
        retVal = EFalse;
        RWSDescriptionArray arr;
        CleanupClosePushL(arr);

        CSenWSDescription* pSD = CSenWSDescription::NewLC();
        pSD->SetEndPointL(((CSenWSDescription&)aPattern).Endpoint());

        iManager.ServiceDescriptionsL(arr, *pSD);
        CleanupStack::PopAndDestroy(pSD);
        for (TInt i=0;i<arr.Count();i++)
            {
            MSenServiceDescription::TDescriptionClassType dt = arr[i]->DescriptionClassType();
            if (dt == MSenServiceDescription::EWSStarServiceSession ||
                dt == MSenServiceDescription::EOviServiceSession    )
                {
                //CWSStarServiceSession* session = (CWSStarServiceSession*)arr[i];
                CSenWebServiceSession* session = (CSenWebServiceSession*)arr[i];
                TInt credId = session->GetCredentialIdL();
                TInt id = aCredential->IdentifierL().IdL();	//codescannerwarings
                if (credId==id)
                    {
                    retVal = ETrue;
                    }
                }
            }
        CleanupStack::PopAndDestroy(&arr);
        }

    return retVal;
    }

TInt CSenCredentialManager::RefreshMTL(const CSenWSDescription& aPattern)
    {
    TInt retVal(KErrNone);
    
    RWSDescriptionArray arr;
    CSenWSDescription* pSD = CSenWSDescription::NewLC();
    pSD->SetEndPointL(((CSenWSDescription&)aPattern).Endpoint());

    iManager.ServiceDescriptionsL(arr, *pSD);
    CleanupStack::PopAndDestroy(pSD);
    CleanupClosePushL(arr);
    for (TInt i=0;i<arr.Count();i++)
        {
        MSenServiceDescription::TDescriptionClassType dt = arr[i]->DescriptionClassType();
        if (dt == MSenServiceDescription::EWSStarServiceSession)
            {
            CSenWebServiceSession* session = (CSenWebServiceSession*)arr[i];
            HBufC8* revalidationError = NULL;
            retVal = session->RefreshMTL(revalidationError);
            delete revalidationError;
            }
        }
    CleanupStack::PopAndDestroy(); //arr.Close() is enough   
    return retVal;
    }

EXPORT_C void CSenCredentialManager::UpdateTouchCredDBL(MSenServiceDescription& asd)
	{
	RXmlEngNodeList<TXmlEngElement> credContElemList;
	CleanupClosePushL(credContElemList);
    RWSDescriptionArray arr;
    CleanupClosePushL(arr);
	
	iManager.ServiceDescriptionsL(arr, asd);
    AsElementL().GetElementsByTagNameL(credContElemList, KCredentialContainer());
    
	for (TInt i=0;i<arr.Count();i++)
    	{
        CSenWebServiceSession* session = (CSenWebServiceSession*)arr[i];
        TInt credId = session->GetCredentialIdL();
                
        while(credContElemList.HasNext())
			{
			RXmlEngNodeList<TXmlEngElement> credIdentElemList; // credential identifier element
			CleanupClosePushL(credIdentElemList);
			
					
			TXmlEngElement credContElem = credContElemList.Next();
			credContElem.GetElementsByTagNameL(credIdentElemList, KCredentialIdentifier());
			
			if(credIdentElemList.HasNext())
				{
				RXmlEngNodeList<TXmlEngElement> credIdElemList;
				CleanupClosePushL(credIdElemList);
				
				TXmlEngElement credIdentElem = credIdentElemList.Next();
				credIdentElem.GetElementsByTagNameL(credIdElemList, KCredentialId());
				
				if(credIdElemList.HasNext())
					{
					TInt res(0);
					TInt credIdDb(0);
					
					TXmlEngElement credIdElem = credIdElemList.Next();
					
					TLex8 lex;
					lex.Assign(credIdElem.Text());
					lex.Val(credIdDb);
					res = credId - credIdDb;
					if(res == 0)
						{
						TUint32 current_tick(0);
						TBuf8<32> tickBuf;
						
						current_tick = User::NTickCount();
						tickBuf.Num(current_tick);	
						credContElem.SetAttributeL(KTouch(), tickBuf);	
						}
					}
				
				CleanupStack::PopAndDestroy(&credIdElemList);
				}
			CleanupStack::PopAndDestroy(&credIdentElemList);	
			}
       }
                 
	CleanupStack::PopAndDestroy(&arr);       
    CleanupStack::PopAndDestroy(&credContElemList);    
//    CleanupUnusedCredDBL();     
                                  
	                          
	}        
	
void CSenCredentialManager::CleanupUnusedCredDBL()
	{
	TInt result(1);
	TInt loc(KErrNotFound); // Not found
	TInt res(0);
	TUint32 current_tick(0);
	TUint32 db_ticks(0);
	TUint32 diff_ticks(0);
		
	RXmlEngNodeList<TXmlEngElement> credContElemList;
	CleanupClosePushL(credContElemList);
	
	AsElementL().GetElementsByTagNameL(credContElemList, KCredentialContainer());

	while(credContElemList.HasNext()) // Can be many credential containers
		{
		RXmlEngNodeList<TXmlEngAttr> attrList;
		CleanupClosePushL(attrList);
		
		TXmlEngElement credContElem = credContElemList.Next();
		credContElem.GetAttributes(attrList);
		while(attrList.HasNext()) // Only one attribute now i.e touch attribute
			{
			TXmlEngAttr attrElem = attrList.Next();
			res = attrElem.Name().Compare(KTouch);
			if(res == 0)
				{
				TPtrC8 val = attrElem.Value();
				
				TLex8 lex;
            	lex.Assign(val);
            	lex.Val(db_ticks, EDecimal);
            	
            	current_tick = User::NTickCount();
            	diff_ticks = current_tick - db_ticks;
            	if(KMaxTicks <= diff_ticks || current_tick < db_ticks)
					{
					RXmlEngNodeList<TXmlEngElement> credIdentElemList; //credential Identifier element
					CleanupClosePushL(credIdentElemList);
					RXmlEngNodeList<TXmlEngElement> credIdElemList; //credentialId element
					CleanupClosePushL(credIdElemList);
					
					credContElem.GetElementsByTagNameL(credIdentElemList, KCredentialIdentifier());
					// There will be only one Credential Identifier within a Credential Container
					if(credIdentElemList.HasNext()) 
						{
						TXmlEngElement credIdentElem = credIdentElemList.Next();
						credIdentElem.GetElementsByTagNameL(credIdElemList, KCredentialId());
						
						// There will be only one Credential Id element within a Credential Identifier
						if(credIdElemList.HasNext())
							{
							TInt credId(0);
							TInt credCount(0);
							TInt credIdDb(0);
							
							credCount = iCredentialArray.Count();
							for(TInt i=0; i < credCount; i++)
								{
								credId = iCredentialArray[i].Credential()->IdentifierL().IdL();
								TXmlEngElement credIdElem = credIdElemList.Next();
								lex.Assign(credIdElem.Text());
								lex.Val(credIdDb);
								result = credId - credIdDb;
								if(result == 0)
									{
									loc = i;
									break;	
									}
								}
							}
						}
					if(result == 0 && loc != KErrNotFound)
						{
						// Get CredentialContainer element
	            		TXmlEngNode element = iCredentialArray[loc].Credential()->AsElementL().ParentNode();
	            		// Delete and destroy both CredentialIdentifier and Credential
	            		iCredentialArray[loc].CloseAndDestroyCredential();
	            		// Delete and destroy CredentialContainer element
	            		iCredentialArray.Remove(loc);
	            		element.Remove();
						}
				
					CleanupStack::PopAndDestroy(&credIdElemList);
					CleanupStack::PopAndDestroy(&credIdentElemList);
					
					break;
					}
				}
			}
		CleanupStack::PopAndDestroy(&attrList);
		}
    CleanupStack::PopAndDestroy(&credContElemList);
	}
// End of file
#ifdef __CRYPTO_HW__
void CSenCredentialManager::EncryptPasswordL(const TDesC8& aData, TDes8& aCipherText)
    {
#ifndef __WINSCW__
		  TUint cipherLen;
		  TBuf8<KEncrypedLen> temp;
		  
		  // add padding if length is  less than 16
		if (aData.Length() < KCryptoPADataLen) 
			  {
			  const TInt remainder = aData.Length() % KCryptoPADataLen;
			  temp.Copy(aData);
			  temp.AppendFill( KPadding, KCryptoPADataLen - remainder );
			  cipherLen = temp.Length() + KPaCryptAuthLen ;
			  aCipherText.SetLength(cipherLen);
			  }
		else
			  {
			  temp.Copy(aData);
			  cipherLen = aData.Length() + KPaCryptAuthLen ;
			  aCipherText.SetLength(cipherLen);
			  }
		    
	    	TBuf8<20> paId;
		    paId.Copy(KCryptoPA);
		   
		    CSecEnv* secEnv = CSecEnv::NewL();
		    TInt err = secEnv->ProtectedApplicationExecute(paId, (TAny*)temp.Ptr(), temp.Length(), 
		                                                     (TAny*)aCipherText.Ptr(), cipherLen, KEncryptNoSalt); 
		    delete secEnv;
	    
	    // Encode data into Base64 format
	    	HBufC8 *b64encPwd = SenCryptoUtils::EncodeBase64L(aCipherText);
    	if(b64encPwd)
	      {
	      aCipherText = (*b64encPwd);
	      delete b64encPwd;
	      }
	    	
	          
#else

aCipherText = aData;
//return KErrNotSupported;
#endif
    
    }

void CSenCredentialManager::DecryptPasswordL(const TDesC8& aCipherText, TDes8& aData)
    {
#ifndef __WINSCW__    	
    TUint dataLen = aCipherText.Length() - KPaCryptAuthLen ;
    aData.SetLength(dataLen);
    TBuf8<20> paId;
    paId.Copy(KCryptoPA);
   // Decode data from Base64 format
    HBufC8 *b64decPwd = SenCryptoUtils::DecodeBase64L(aCipherText);
    	TDesC8 aTmpTxt= (*b64decPwd);
    // Decrypt using PA	
    CSecEnv* secEnv = CSecEnv::NewL();
    TInt err = secEnv->ProtectedApplicationExecute(paId, (TAny*)aTmpTxt.Ptr(), aTmpTxt.Length(), 
                                                     (TAny*)aData.Ptr(), dataLen, KDecrypt);
    // remove padding
    if (aData.Length() == KCryptoPADataLen)
        {
        RBuf8 decPwd;
        decPwd.Create(KCryptoPADataLen);
        decPwd.Copy(aData);
        TInt pos = decPwd.Locate(KPadding);
        if(pos != KErrNotFound)
          {
          decPwd.SetLength(pos);
          aData.SetLength(decPwd.Length());
          aData = decPwd;
          }
        decPwd.Close();
        }
    delete secEnv;                                                  
		if(b64decPwd)
				{
				delete b64decPwd;
				}
				
#else

aData = aCipherText ;
//return KErrNotSupported;
#endif    
}  
#endif // __CRYPTO_HW__
// End of file 



