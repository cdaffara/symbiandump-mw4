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
#include <f32file.h>
#include <s32file.h>
#include <s32strm.h>
#include <e32std.h>
#include <sysutil.h>
#include <eikenv.h>
#include <s32mem.h>

#include <SenServiceConnection.h> // error codes and framework ids
#include <MSenServiceDescription.h>
#include "senwsdescription.h"
#include <SenXmlUtils.h>          // static attribute value getter
#include <SenXmlReader.h>

#include "senprovider.h"
#include "senxmldao.h"
#include "msencoreservicemanager.h"

#include "senservicemanagerdefines.h" // KMaxTicks (1209600000)
                                      // 1 tick = 1 millisec
                                      // 86400 sec = 1 day
                                      // 86400*1000 millisec = 1 day
                                      // 86400*14*1000 = 1209600000 millisec = 2 weeks

#include "senlogger.h"
#include "sendebug.h"                       // internal Utils\inc
#include "msenserviceinvocationframework.h" // internal Framework\inc
#include "senserviceinvocationframework.h"  // internal Framework\inc
#include "sensecuritymechanism.h"           // internal Framework\inc
#include "senservicesession.h"              // internal Framework\inc

#include "senprovider.h"

#include "senlogger.h" 


// CONSTANTS
namespace
    {
    const TInt KFlatBufSize = 128;

#if !defined( EKA2 ) && !defined( RD_SECURE_PRIV_DATA )
     _LIT(KSenXMLDAOFile,        "C:\\system\\data\\sensessions.xml");
     // since CoreSM & XMLDAO are singleton, single temp file is ok:
     _LIT(KSenXMLDAOTempFile,    "C:\\system\\data\\sensessions.temp.xml");
#else
    _LIT(KSenXMLDAOFile,        "sensessions.xml");
    // since CoreSM & XMLDAO are singleton, single temp file is ok:
    _LIT(KSenXMLDAOTempFile,    "sensessions.temp.xml");
#endif   

    //_LIT(KSenXmlDaoPanic, "SenXmlDao");

    _LIT8(KDefaultFrameworkCue1,
                "com.nokia.Sen.idwsf.IdentityBasedWebServicesFramework");
    _LIT8(KDefaultFrameworkCue2,
                "com.nokia.Sen.wsi.WSIBasicFramework");
    _LIT8(KDefaultFrameworkCue3,
                "com.nokia.Sen.rest.RestServiceFramework");
    _LIT8(KDefaultFrameworkCue4,
                "com.nokia.ws.wsstar.WSStarFramework");
    _LIT8(KDefaultFrameworkCue5,
                "com.nokia.ws.atompub.AtomPubFramework");
    _LIT8(KDefaultFrameworkCue6,
                "com.nokia.ws.ovi.OviFramework");

    _LIT8(KEqual, "=");
    _LIT8(KQuote, "\"");
    _LIT8(KCloseTag, ">");
    _LIT8(KEmptySpace, " ");

    _LIT8(KServiceDescription, "ServiceDescription");
    _LIT8(KFramework, "Framework");
    _LIT8(KClass, "class");
    _LIT8(KProxyHost, "proxyHost");
    _LIT8(KProxyPort, "proxyPort");


    _LIT8(KLocalName, "SenConfiguration");
    _LIT8(KTransport,   "Transport");
    _LIT8(KEndTag, "</SenConfiguration>");
    _LIT8(KStartTag,
            "<SenConfiguration xmlns=\"urn:com.nokia.Sen.config.1.0\">");
    _LIT8(KStartTagEndless,
            "<SenConfiguration xmlns=\"urn:com.nokia.Sen.config.1.0\"");
    _LIT8(KNamespaceName, "urn:com.nokia.Sen.config.1.0");

    _LIT8(KSpace, " ");
    _LIT8(KTouch, "touch");

    // Transport plug-in spesific constants:
    _LIT8(KLocalTransportScheme,                "local");
    _LIT8(KDefaultLocalTransportPluginCue,      "com.nokia.wsf.transport.plugin.local");

    _LIT8(KTcpTransportScheme,                   "tcp");
    _LIT8(KDefaultVirtualTcpTransportPluginCue,  "com.nokia.wsf.transport.plugin.virtualtcp");
    }

CSenXMLDAO* CSenXMLDAO::NewL(MSenCoreServiceManager& aManager)
    {
    CSenXMLDAO* pNew = CSenXMLDAO::NewLC(aManager);
    CleanupStack::Pop();
    return pNew;
    }

CSenXMLDAO* CSenXMLDAO::NewLC(MSenCoreServiceManager& aManager)
    {
    CSenXMLDAO* pNew = new (ELeave) CSenXMLDAO(aManager);
    CleanupStack::PushL(pNew);

    // SenConfiguration xmlns="urn:com.nokia.Sen.config.1.0">
    pNew->ConstructL();
    return pNew;
    }

CSenXMLDAO::CSenXMLDAO(MSenCoreServiceManager& aManager)
    :iManager(aManager),
    iReadingConfig(ESenIdle),
    iErrorState(KErrNone),
    iFileName(KSenXMLDAOFile),
    iSessions(2),
    iFrameworks(2),                     
    iServiceDescription(NULL),
    iFrameworkConfigParser(NULL),
    iDefaultFrameworkCheckedToExist(EFalse),
    iProxyHost(NULL),
    iProxyPort(0),
    iTransportMap(ETrue, ETrue),
    iDefaultTransportMapChecked(EFalse),
    iSharableProviders(2),              
    iUnsharableProviders(ETrue, ETrue) 
    {
    }

void CSenXMLDAO::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO ConstructL()")));
    BaseConstructL(KNamespaceName(), KLocalName());
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO Base done")));


    SetReader(*iManager.XMLReader());

    //Data caging 2 implementation
#if defined( EKA2 ) || defined( RD_SECURE_PRIV_DATA )
    TBuf<KMaxPath> privatePath;
    RFs fss;
        User::LeaveIfError(fss.Connect());
        CleanupClosePushL(fss);
    fss.CreatePrivatePath(EDriveC);
    fss.PrivatePath(privatePath);
    iFileName.Insert(0,privatePath);
    CleanupStack::PopAndDestroy(1);
#endif
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO ConstructL() done")));
    iCriticalSection.CreateLocal();
    }

CSenXMLDAO::~CSenXMLDAO()
    {
    // Save the XML
    CSenXMLDAO::Save();

    if(iServiceDescription)
        {
        TInt index = iSessions.Find(iServiceDescription);
        if(index==KErrNotFound)
            {
            // would otherwise NOT be taken care by:
            //             iSessions.ResetAndDestroy()
            delete iServiceDescription;
            }
        }

    iSessions.ResetAndDestroy();


    if(iFrameworkConfigParser)
        {
        CSIF* pSIF = (CSIF*) iFrameworkConfigParser;

        TInt index = iFrameworks.Find(pSIF);
        if(index==KErrNotFound)
            {
            // otherwise deleted in ResetAndDestroy() below
            delete iFrameworkConfigParser;
            }
        }
    iFrameworks.ResetAndDestroy();

    if ( iProxyHost )
        {
        delete iProxyHost;
        iProxyHost = NULL;
        }

    iTransportMap.Reset();

    iSharableProviders.ResetAndDestroy();
    iUnsharableProviders.Reset(); // destroys any unloaded / unreleased providers..
    iCriticalSection.Close();

    if( ipTransportDelegate )
        {
        ipTransportDelegate->ExtractElement();
        }
    delete ipTransportDelegate;    
    }

CSenXmlReader* CSenXMLDAO::XMLReader()
    {
    return iManager.XMLReader();
    }

void CSenXMLDAO::Load()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::Load()")));
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, ReadL());

#ifdef _SENDEBUG
    if(leaveCode!=KErrNone)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CSenXMLDAO::Load() - ReadL() leaved: %d"), leaveCode));
        }
#else
    leaveCode = 0; 
#endif


    // Default frameworks are checked (and loaded)after all the frameworks have
    // been parsed in the MIDDLE of PARSING, to PREVENT possibility of orphan
    // SD, if no ID-WSF plug-in was found, for example because ECOM cue
    // (default_data) was wrong. MOVED the installation of default
    // frameworks to be in StartElementL() instead(!)

    // Handle the case where no frameworks or service descriptions were found 
    // from sensessions.xml
    if(!iDefaultFrameworkCheckedToExist)
        {
        TInt retVal = CheckDefaultFrameworks();
        retVal = 0; // not used currently
        iDefaultFrameworkCheckedToExist=ETrue;
        }

    if(!iDefaultTransportMapChecked)
        {
        TInt retCode = CheckDefaultTransports();
        iDefaultTransportMapChecked=ETrue;
        retCode=0; // not used currently
        }

    iReadingConfig = ESenIdle;

    // This way the sensessions.xml -file is validated,
    // and expired sessions will be deleted, etc..
    TInt saveRetVal = Save();
    saveRetVal = 0; // not used currently
    }

TInt CSenXMLDAO::Save()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::SaveSessions()")));
    TInt notifyLeaveCode(KErrNone);
    TRAP(notifyLeaveCode, NotifyFrameworksL(KNullDesC8, KSenEventSessionsDbWriteBegin, NULL));
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, WriteL());
    TRAP(notifyLeaveCode, NotifyFrameworksL(KNullDesC8, KSenEventSessionsDbWriteComplete, NULL));
    
    iManager.SaveCredentialDB();
    
    notifyLeaveCode = 0; // not used
    return leaveCode;
    }

MSIF* CSenXMLDAO::Framework(const TDesC8& aFrameworkId)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::Framework");
    CSIF* pFramework = NULL;

    TInt count(iFrameworks.Count());
    for (TInt i=0; i<count; i++)
        {
        if(iFrameworks[i])
            {
            const TDesC8& frameworkId = iFrameworks[i]->Id();

            if(frameworkId == aFrameworkId)
                {
                pFramework = iFrameworks[i];
                break;
                }
            }
        }
    if(pFramework)  TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"  framework found:");//LOG_WRITE_L("  framework found:");
    else TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"  could not find:");        //   LOG_WRITE_L("  could not find:");
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aFrameworkId));

    return pFramework;
    }

MSIF* CSenXMLDAO::DefaultFrameworkL(const TDesC8& aFrameworkID, 
                                    const TDesC8& aCue)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::DefaultFramework()")));

    MSIF* pFramework = Framework(aFrameworkID);

    if(!pFramework)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::DefaultFramework() is not installed:")));
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aFrameworkID));
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aCue));

        TRAPD(leaveCode, iManager.InstallFrameworkL(aCue));
        if(leaveCode==KErrNone)
            {
            pFramework = Framework(aFrameworkID);
            if(pFramework)
                {
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(_L("CSenXMLDAO::DefaultFramework() - now installed.")));
                }
            }
#ifdef _SENDEBUG
        else
            {
            // install framework failed!
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::DefaultFramework() - install failed:")));
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"FATAL!");
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(aCue));
            }
#endif //_SENDEBUG
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(_L("CSenXMLDAO::DefaultFramework() - was already installed:")));
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aFrameworkID));
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aCue));
                   
        }
#endif //_SENDEBUG

    return pFramework;
    }

RFileLogger* CSenXMLDAO::Log() const
    {
    return iManager.Log();
    }

TInt CSenXMLDAO::SizeOfSessionsXmlInBytesL()
    {
    TInt size(0);

    TInt count = iSessions.Count();
    CBufFlat* pBuf = NULL;
    for(TInt i = 0; i < count; i++)
        {
        pBuf = CBufFlat::NewL(KFlatBufSize);
        CleanupStack::PushL(pBuf);
        RBufWriteStream bufWs(*pBuf);
        CleanupClosePushL(bufWs);
#ifdef _SENDEBUG
        TPtrC8 e = iSessions[i]->Endpoint();
        TPtrC8 c = iSessions[i]->Contract();
        TPtrC8 fid = iSessions[i]->FrameworkId();
        TInt leaveCode(KErrNone);
        TRAP( leaveCode, iSessions[i]->WriteAsXMLToL(bufWs); )
        if( leaveCode )
            {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("FATAL: CSenXMLDAO::SizeOfSessionsXmlInBytesL leaved:   %d"), leaveCode));
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("     : endpoint: '%S', contract: '%S', frameworkID: '%S'"), &e, &c, &fid));
            
            }
        User::LeaveIfError(leaveCode);
        
#else // release builds
        iSessions[i]->WriteAsXMLToL(bufWs);
#endif // _SENDEBUG        
        TPtr8 p8 = pBuf->Ptr(0);
        size += p8.Length();
        CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
        pBuf = NULL;
        }
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenXMLDAO::SizeOfSessionsXmlInBytesL:   %d"), size));

    return size;
    }

TInt CSenXMLDAO::SizeOfFrameworksXmlInBytesL()
    {
    TInt size(0);

    HBufC8* pTemp = NULL;

    TInt frameworks = iFrameworks.Count();

    for (TInt j = 0; j < frameworks; j++)
        {
        pTemp = iFrameworks[j]->AsXmlL();
        if(pTemp)
            {
            CleanupStack::PushL(pTemp);
            size += pTemp->Length();
            
            CleanupStack::PopAndDestroy(); // pTemp
            pTemp = NULL;
            }
        }
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenXMLDAO::SizeOfFrameworksXmlInBytesL: %d"), size));
    return size;
    }


void CSenXMLDAO::WriteL()
    {
    if (iReadingConfig != ESenIdle)
        {
        iReadingConfig = ESenSaveNeeded;
        return;
        }
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::WriteL()")));


    CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
    CleanupStack::PushL(pBuf);
    RBufWriteStream bufWs(*pBuf);
    CleanupClosePushL(bufWs);


    if ( !iProxyHost )
        {
        bufWs.WriteL(KStartTag);
        }
    else
        {
        bufWs.WriteL( KStartTagEndless );

        bufWs.WriteL( KEmptySpace );
        bufWs.WriteL( KProxyHost );
        bufWs.WriteL( KEqual );
        bufWs.WriteL( KQuote );

        bufWs.WriteL(*iProxyHost);
        bufWs.WriteL( KQuote );

        bufWs.WriteL( KEmptySpace );
        bufWs.WriteL( KProxyPort );
        bufWs.WriteL( KEqual );
        bufWs.WriteL( KQuote );

        HBufC8* pPort = HBufC8::NewLC(5);
        TPtr8 pPtrPort = pPort->Des();
        pPtrPort.AppendNum( iProxyPort );
        bufWs.WriteL( *pPort );

        CleanupStack::PopAndDestroy( pPort );

        bufWs.WriteL( KQuote );
        bufWs.WriteL( KCloseTag );

        }

    // Now ensure that we have enough disk space for
    // new configuration file!

    TInt configFileSize(0);

    TPtr8 p8 = pBuf->Ptr(0);

    // calculate and add required space for sessions as XML (in bytes)
    configFileSize += SizeOfSessionsXmlInBytesL(); 

    // calculate and add required space for frameworks as XML (in bytes)
    configFileSize += SizeOfFrameworksXmlInBytesL(); 

    // add size of root element start tag (in bytes)
    configFileSize += p8.Length();
    // add size of root element close tag (in bytes)
    configFileSize += KEndTag().Length();
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenXMLDAO::WriteL() - Base config size: %d bytes"), p8.Length()));

    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenXMLDAO::WriteL() - Config file size: %d bytes"),  configFileSize));


    // Now use SysUtils to ensure that we have room for temp file(!)
    RFs fss;
    User::LeaveIfError(fss.Connect());
    CleanupClosePushL(fss);

    // Create and open write stream to temp file
    TFileName tempFile(KSenXMLDAOTempFile);

    // this will create the file or replace and zero-length any existing file
    // and might free some space, if write failed last time..
    RFileWriteStream tempFileWriteStream;
    CleanupClosePushL(tempFileWriteStream);
    tempFileWriteStream.Replace(fss, tempFile, EFileWrite);

    
    TInt retVal(SysUtil::FFSSpaceBelowCriticalLevelL(&fss, configFileSize));
    if(retVal == KErrNone)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, "File system check: OK, there is enough space available.");
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"File system check: FAILED, NOT enough space available.");
        // release the root configuration XML buffer:
        // tempFileWriteStream.Close(), fss.Close(), bufWs.Close(), delete pBuf
        CleanupStack::PopAndDestroy(4); 
        User::Leave(retVal);
        }


    HBufC8* pTemp = NULL;

    // Write root level configuration XML into temp file
    tempFileWriteStream.WriteL(p8);

    // Write frameworks directly into temp file
    TInt frameworks = iFrameworks.Count();
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8(" - Framework count: (%d)"),  frameworks));

    for (TInt j = 0; j < frameworks; j++)
        {
        pTemp = iFrameworks[j]->AsXmlL();
        if(pTemp)
            {
            CleanupStack::PushL(pTemp);
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(*pTemp));
            tempFileWriteStream.WriteL(*pTemp);
            CleanupStack::PopAndDestroy(); // pTemp
            pTemp = NULL;
            }
        }

    // Write sessions directly into temp file
    TInt count = iSessions.Count();
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8(" - Description/Session count: (%d)"), count));
    for(TInt i = 0; i < count; i++)
        {
        iSessions[i]->WriteAsXMLToL(tempFileWriteStream);
        }

    // Write root level configuration element's end tag into temp file
    tempFileWriteStream.WriteL(KEndTag);

    // close the temp file write stream:
    CleanupStack::PopAndDestroy(); // tempFileWriteStream.Close()

    // create file manager to replace original database file
    CFileMan* pFileMan = CFileMan::NewL(fss);
    CleanupStack::PushL(pFileMan);

    // Now copy temp file over the real database. EOverWrite is default
    User::LeaveIfError(pFileMan->Move(KSenXMLDAOTempFile, KSenXMLDAOFile));

    // destroy file manager instance and close the fss
    CleanupStack::PopAndDestroy(2); // delete pFileMan, fss.Close()

    // release the root configuration XML buffer
    CleanupStack::PopAndDestroy(2); // bufWs.Close(), delete pBuf
    }


    /*
    Snippet from deprecated WriteL() version:

    TPtr8 p8 = pBuf->Ptr(0);

    HBufC8* pFileContentAsUtf8 = p8.AllocL();

    CleanupStack::PushL(pFileContentAsUtf8);

    // Everything in MEMORY ok, prepare to write into file


    RFileWriteStream fileOutStream;
    CleanupClosePushL(fileOutStream);


    if(!SysUtil::FFSSpaceBelowCriticalLevelL(&fss,
                            pFileContentAsUtf8->Length()) )
        {
        // note, this will zero-length the file(!) 
        fileOutStream.Replace(fss, iFileName, EFileWrite);

        // finally write the UTF-8 into the file.
        fileOutStream.WriteL(*pFileContentAsUtf8);
        }
    // else { User::Leave("KFSSpaceBelowCriticalLevel"); }

    */
void CSenXMLDAO::ReadL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::ReadL()")));

    NotifyFrameworksL(KNullDesC8, KSenEventSessionsDbReadBegin, NULL);


    RFs fss;
        User::LeaveIfError(fss.Connect());
        CleanupClosePushL(fss);

    iReadingConfig = ESenReading;

    XMLReader()->SetContentHandler(*this);
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, XMLReader()->ParseL(fss, iFileName));
    iReadingConfig = ESenIdle;

    CleanupStack::PopAndDestroy(1); // fss
    NotifyFrameworksL(KNullDesC8, KSenEventSessionsDbReadComplete, NULL);
    leaveCode = 0; // not used currently
    }

TInt CSenXMLDAO::Add(CSenWSDescription& aSD)
    {
    // Try to take ownership:
    TInt retVal(iSessions.Append(&aSD));
    if(retVal==KErrNone)
        {
        // Must be set so that it can be saved as XML fragment which 
        // has an owner. This prevents empty namespace to be written.
        aSD.SetOwner(*(MSenFragment*)this);
    TInt retValSave = Save();
    TInt notifyLeaveCode(KErrNone);
    TRAP(notifyLeaveCode, NotifyFrameworksL(KNullDesC8,
    		KSenEventRegisterNewServiceDescription, NULL));
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenXMLDAO::Add(SD), Save() returned: %d"), retValSave));
        retValSave = 0; // not in use in release builds
        }
    return retVal;
    }

// return an error code (Append might leave!)
TInt CSenXMLDAO::Add(CSIF& aFramework)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::Add(SIF)");
    TBool alreadyExists(EFalse);
    TInt sifCount = iFrameworks.Count();
    for (TInt i=0; i<sifCount; i++)
        {
        if (iFrameworks[i]->Id() == aFramework.Id())
            {
            alreadyExists = ETrue;
            break;
            }
        }
    TInt retCode(KErrNone);
    if (!alreadyExists)
        {
        retCode = iFrameworks.Append(&aFramework);
        if(retCode==KErrNone)
            {
            TInt saveRetVal = Save();
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenXMLDAO::Add(SIF), Save() returned: %d"), saveRetVal));
            saveRetVal = 0; // not used in release builds
            }
        }
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::Add(CSIF&) - complete.");
    return retCode;
    }

TBool CSenXMLDAO::Remove(CSenWSDescription& aSD)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::Remove(SD)")));
    TInt index(KErrNotFound);
    TInt count(iSessions.Count());

    // Due architectural change
    // REMOVE COMMENTED 3.0 CODE BELOW in 5.0:
    //TPtrC8 contract = aSD.Contract();
    //TPtrC8 endpoint = aSD.Endpoint();

    //if(contract.Length()>0)
    //    {

    TInt leaveCode(KErrNone);
    for(TInt i=0; i<count; i++)
        {
        // Due architectural change
        // REMOVE COMMENTED 3.0 CODE BELOW in 5.0:
        //TPtrC8 sessContract = iSessions[i]->Contract();
        //TPtrC8 sessEndpoint = iSessions[i]->Endpoint();

        //if ( sessContract == contract && sessEndpoint == endpoint )
        CSenWSDescription* pSD = iSessions[i];
        TBool equals(EFalse);
        TRAP(leaveCode, equals = pSD->HasEqualPrimaryKeysL(aSD));
#ifdef _SENDEBUG
        if(leaveCode!=KErrNone)
            {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CSenXMLDAO::Remove - HasEqualPrimaryKeysL leaved: %d"), leaveCode));
            }
#endif
        if(leaveCode==KErrNone && equals)
            {
                index = i;
                break;
            }
        leaveCode = KErrNone;
        }
    //    }

    if(index != KErrNotFound)
        {

#ifdef _SENDEBUG
        TRAP_IGNORE
            (
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"Removing service description from DAO: ");
            CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
            CleanupStack::PushL(pBuf);
            RBufWriteStream bufWs(*pBuf);
            CleanupClosePushL(bufWs);
            iSessions[index]->WriteAsXMLToL(bufWs);
        TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(pBuf->Ptr(0)));
            CleanupStack::PopAndDestroy(2); // bufWs, pBuf
            )
#endif // _SENDEBUG

        delete iSessions[index];
        iSessions.Remove(index);
        TInt saveRetVal = Save();
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenXMLDAO::Remove(SD), Save() returned: %d"), saveRetVal));
        saveRetVal = 0; // not used in release builds
        return ETrue;
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::Remove(SD): description not found.");
        }
#endif
    return EFalse;
    }


void CSenXMLDAO::Remove(CSIF& aFramework)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::Remove(SIF)")));
    TInt index(-1);
    TInt count(iFrameworks.Count());
    for(TInt i=0; i<count; i++)
        {
        if((iFrameworks[i])->Id() == aFramework.Id())
            {
            index = i;
            break;
            }
        }

    if(index != KErrNotFound)
        {
        delete iFrameworks[index];
        iFrameworks.Remove(index);
        TInt retValSave = Save();
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenXMLDAO::Remove(SIF), Save() returned: %d"), retValSave));
        retValSave = 0; // not in use in release builds
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::Remove(SIF): plugin not found.");
        }
#endif
    }



void CSenXMLDAO::StartElementL(const TDesC8& /* aNsURI */,
                              const TDesC8& aLocalName,
                              const TDesC8& /* aQName */,
                              const RAttributeArray& aAttributes)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("Entering CSenXMLDAO::StartElementL")));
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aLocalName));
    //LOG_WRITEFORMAT((_L8("Namespace: %S"), &aNsURI));

    if(aLocalName == KServiceDescription)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("Found a ServiceDescription")));

        // Default framework is checked after all the frameworks have been
        // parsed in the MIDDLE of PARSING, to PREVENT orphanizing the SD(s),
        // if no ID-WSF plugin was found; for example because ECOM cue
        // (default_data) was wrong. MOVED the installation of default
        // frameworks to be in here instead of LoadL()

        // NOTE: expect, that when first SD is encountered, all of
        // the frameworks have been parsed and installed properly
        // if such (plugins) exist.

        // No need to store the SIF* anywhere, just doing this to ensure the
        // installation of the ID-WSF framework
        if(!iDefaultFrameworkCheckedToExist)
            {
            // do only once per read configuration

            TInt retCode = CheckDefaultFrameworks();
            iDefaultFrameworkCheckedToExist=ETrue;
            retCode = 0; // not used currently
            }

        // Alike frameworks, default transport (scheme-to-cue) mapping
        // is ensured when first service description is recognized.
        // The list for scheme-cue mappings is expected to be found
        // before service descriptions (in the beginning of sensessions.xml)
        if(!iDefaultTransportMapChecked)
            {
            TInt retCode = CheckDefaultTransports();
            iDefaultTransportMapChecked=ETrue;
            retCode=0; // not used currently
            }


        // check, if for some reason the iServiceDescription
        // was NOT added to iSessions. If so, delete the
        // orphan SD
        if(iServiceDescription)
            {
            TInt index = iSessions.Find(iServiceDescription);
            if(index==KErrNotFound)
                {
                // would otherwise NOT be taken care by:
                //             iSessions.ResetAndDestroy()
                delete iServiceDescription;
                }
            iServiceDescription=NULL;
            }
        
        
        iServiceDescription = CSenWSDescription::NewL(
                            AsElement().NamespaceURI() );
        iServiceDescription->SetAttributesL(aAttributes);
        iServiceDescription->SetReader(*Reader());

#ifdef _SENDEBUG
        TPtrC8 frameworkID = iServiceDescription->FrameworkId();
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::StartElementL");
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8(" - %S service description now parsed by delegate."), &frameworkID));
#endif

        iState = KStateParsingServiceDescription;

        DelegateParsingL(*iServiceDescription);
        }
    else if(aLocalName == KFramework)
        {
        iFrameworkConfigParser = NULL;
        // we can take the framework id from attribute list,
        // no need to wait for end element...
        TPtrC8 fwId = SenXmlUtils::AttrValue(aAttributes, KClass);
        iFrameworkConfigParser = iManager.InstallFrameworkL(fwId);
        if(iFrameworkConfigParser)
            {
            iFrameworkConfigParser->SetOwner((MSenFragment&)*this);
            iFrameworkConfigParser->SetAttributesL(aAttributes);
            iFrameworkConfigParser->SetReader(*Reader());

            iState = KStateParsingFramework;

            DelegateParsingL(*iFrameworkConfigParser);
            }
        // Expect, that frameworks are first in the XML. Set boolean
        // "check-done" flag to EFalse, so that check will be issued
        // when first SD is encountered.
        iDefaultFrameworkCheckedToExist=EFalse;
        }
    else if (aLocalName == KLocalName)  // SenConfiguration starting tag
        {
        // check if HTTP proxy HOST -param was set:
        TPtrC8 host = SenXmlUtils::AttrValue(aAttributes, KProxyHost);
        if ( host.Length() > 0 &&  host != KSpace)
            {
            delete iProxyHost;
            iProxyHost = NULL; 
            iProxyHost = host.AllocL();
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("HTTP proxy HOST read: '%S'"), iProxyHost));
            }

        // check if HTTP proxy PORT -param was set:
        TPtrC8 port = SenXmlUtils::AttrValue(aAttributes, KProxyPort);
        if (port.Length() > 0 && port != KSpace)
            {
            TLex8 lex;
            lex.Assign(port);
            lex.Val( iProxyPort );
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("HTTP proxy PORT read: '%d'"), iProxyPort));
            }
        }
        /*
    else if ( aLocalName == KTransport && aNsURI == KNamespaceName )  // Transport starting tag
        {
        if ( ipTransportDelegate )
            {
            ipTransportDelegate->ExtractElement();
            delete ipTransportDelegate;
            ipTransportDelegate = NULL;
            }
        ipTransportDelegate = CSenDomFragment::NewL(KTransport);
        CSenElement& element = ipTransportDelegate->AsElement();
        this->AsElement().AddElementL(element);        
        iState = KStateParsingTransport;
        DelegateParsingL(*ipTransportDelegate); 
        }
        */
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("Leaving CSenXMLDAO::StartElementL")));

    }

void CSenXMLDAO::EndElementL(const TDesC8& aUri,
                             const TDesC8& aLocalName,
                             const TDesC8& aName)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::EndElementL()")));
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Namespace URI: %S"), &aUri));
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Local name URI: %S"), &aLocalName));


    switch(iState)
        {
        case KStateParsingServiceDescription:
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::EndElementL() - EParsingServiceDescription")));
            if(aLocalName == KServiceDescription)
                {
#ifdef _SENDEBUG
                HBufC8* pAsXmlUtf8 = iServiceDescription->AsXmlL();
                if(pAsXmlUtf8)
                    {
                    CleanupStack::PushL(pAsXmlUtf8);
                    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("SD as XML:")));
                    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(*pAsXmlUtf8));
                    CleanupStack::PopAndDestroy(1); // pAsXmlUtf8
                    }
#endif

                // transfer ownership
                iManager.RegisterServiceDescriptionL(iServiceDescription);
                iServiceDescription = NULL; // now owned elsewhere
                }
            break;
            }
        case KStateParsingFramework:
            {
            if (aLocalName == KFramework)
                {
                iState = KStateIgnore;
                }
            break;
            }
        case KStateParsingTransport:
            {
            if ( aLocalName == KTransport && aUri == KNamespaceName ) 
                {
                 // Transport ending tag
                iState = KStateIgnore;
                }
            break;
            }
        default:
            {
            CSenBaseFragment::EndElementL(aUri,aLocalName,aName);
            break;
            }
        }
    }


TBool CSenXMLDAO::Owns(CSenWSDescription* aSD)
    {
    TInt count(iSessions.Count());
    for(TInt i=0; i<count; i++)
        {
        if(iSessions[i]==aSD)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

TInt CSenXMLDAO::ProxyPort()
    {
    return iProxyPort;
    }

const TDesC8& CSenXMLDAO::ProxyHost()
    {
    if ( iProxyHost )
        {
        return *iProxyHost;
        }
    else
        {
        return KNullDesC8;
        }
    }
CSenWSDescription* CSenXMLDAO::FindExactServiceDescriptionL(
                                                CSenWSDescription& aSd)
    {
    TInt sessCount = iSessions.Count();
    for (TInt i=0; i<sessCount; i++)
        {
        CSenWSDescription* pCandidate = iSessions[i];

        // The concrete session implementation of framework "X" decides
        // what are the primary keys (unique) in a service description
        if(pCandidate && pCandidate->HasEqualPrimaryKeysL(aSd))
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::FindExactServiceDescriptionL:");
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - exact match found.");
            return pCandidate;
            }

        // It is better to let framework sessions classes to define the
        // "primary keys" of this comparison(!). That is more extensible.
        // SO: REMOVE BELOW COMMENTED CODE in 5.0:
        //if(pCandidate)
        //    {
        //    if (pCandidate->Endpoint() == aSd.Endpoint() &&
        //            pCandidate->Contract() == aSd.Contract() )
        //        {
        //
        //
        //        LOG_WRITE_L("CSenXMLDAO::FindExactServiceDescriptionL:");
        //        LOG_WRITE_L(" - exact match found.");
        //        return pCandidate;
        //        }
        //    }

        }
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::FindMatchingServiceDescription: no match found.");
    return NULL;
    }

CSenWSDescription* CSenXMLDAO::FindMatchingServiceDescriptionL(
                                                CSenWSDescription& aSd)
    {
    TInt sessCount = iSessions.Count();
    for (TInt i=0; i<sessCount; i++)
        {
        CSenWSDescription* pCandidate = iSessions[i];
        if(pCandidate)
            {
            if(pCandidate->Matches(aSd))
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::FindMatchingServiceDescription:");
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - matching SD found.");
                return pCandidate;
                }
            }
        }
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::FindMatchingServiceDescription: no match found.");
    return NULL;
    }
    
CSenWSDescription* CSenXMLDAO::FindMatchingSDAndBestScoreL(
                                            CSenWSDescription& aSd,
                                            TInt &aBestScore)	//CodeScannerWarnings
    {
    CSenWSDescription* pMatchingSD = NULL;
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"***************************************************************");
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"CSenXMLDAO::FindMatchingSDAndBestScore");
    TPtrC8 sdendpoint = aSd.Endpoint();
    TPtrC8 sdcontract = aSd.Contract();
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Endpoint URI: %S"), &sdendpoint ));
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Contract: %S"), &sdcontract ));
    
    TInt score( 0 );
    // Make sure that at least endpoint or contract should match
    aBestScore = 1;
    TInt sessCount = iSessions.Count();
    for (TInt i=0; i<sessCount; i++)
        {
        CSenWSDescription* pCandidate = iSessions[i];
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"----------------------------------------------");
        TPtrC8 endpoint = pCandidate->Endpoint();
        TPtrC8 contract = pCandidate->Contract();
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Endpoint URI: %S"), &endpoint ));
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Contract: %S"), &contract ));
        if(pCandidate)
            {
			score = pCandidate->ScoreMatchL(aSd);
			if (score > aBestScore) 
			    {
			    if ( pCandidate->Matches(aSd) )
			        {
			        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"-> Selected");
    				TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("[score: %d]"), score ));
			        pMatchingSD = pCandidate;
			        }
    			aBestScore = score;
			    }

            if ( !pMatchingSD && pCandidate->Matches(aSd) )
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"-> Selected");
                pMatchingSD = pCandidate;
                }
            }
        }
        
#ifdef _SENDEBUG
    if ( !pMatchingSD )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"CSenXMLDAO::FindMatchingSDAndBestScore: no match found.");
        }
#endif
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Best score: %d"), aBestScore ));
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"***************************************************************");
    
    return pMatchingSD;
    }

CSenWSDescription* CSenXMLDAO::FindScoreMatchingServiceDescriptionL(
                                            CSenWSDescription& aSd,
                                            TInt aBestScore)
    {
	/* We should now try the primary search again.
	 * Note that we try to pick a service that matches 
	 * at least as good as the given aBestScore
	 * but at least the contract or endpoint should
	 * match.
	 */ 
	TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"CSenXMLDAO::FindScoreMatchingServiceDescription");
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Best score: %d"), aBestScore ));
    TPtrC8 sdendpoint = aSd.Endpoint();
    TPtrC8 sdcontract = aSd.Contract();
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Endpoint URI: %S"), &sdendpoint ));
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Contract: %S"), &sdcontract ));
	 
	TInt score( 0 );
	// Make sure that at least endpoint or contract should match
	if (aBestScore < 1) aBestScore = 1; 
	
    TInt sessCount = iSessions.Count();
    for (TInt i=sessCount-1 ; i>=0 ; i--)
        {
        CSenWSDescription* pCandidate = iSessions[i];
        if(pCandidate)
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"----------------------------------------------");
            TPtrC8 endpoint = pCandidate->Endpoint();
            TPtrC8 contract = pCandidate->Contract();
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Endpoint URI: %S"), &endpoint ));
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("Contract: %S"), &contract ));
			score = pCandidate->ScoreMatchL(aSd);
			TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("score: %d"), score ));
			if (score >= aBestScore)
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"CSenXMLDAO::FindScoreMatchingServiceDescription:");
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8(" - Scorematching SD found. Score : %d"),
                                score));
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
                return pCandidate;
                }
            }
        }
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"CSenXMLDAO::FindScoreMatchingServiceDescription: no scorematch found.");
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    
    return NULL;
    }

TInt CSenXMLDAO::FindAllMatchingServiceDescriptions(RWSDescriptionArray& aMatches,
                                                    const TDesC8& aContract)
    {
    TInt retVal(KErrNone);
    TInt sessCount(iSessions.Count());

    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::FindAllMatchingServiceDescriptions(contract)");

    for(TInt i=0; i<sessCount; i++)
        {
        TPtrC8 sessContract = iSessions[i]->Contract();

        if ( sessContract.Length()>0 && aContract == sessContract
            ||
            aContract == KNullDesC8 // if no contract given, all match
            )
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel," - match found, appending to array.");
            TInt appendRetVal(aMatches.Append(iSessions[i]));
            if(appendRetVal!=KErrNone)
                {
                retVal = appendRetVal; // note: last error is retuned
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - FATAL: due OOM Append(SD) failed!");
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8(" - error code: %d"), appendRetVal));

                // Attempt to append other hits/matching SDs
                }
            }
        }
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8(" - total of %d matches found."), 
                        aMatches.Count()));

    return retVal; // last error from Append() is returned atm
    }

TInt CSenXMLDAO::FindAllMatchingServiceDescriptions(RWSDescriptionArray& aMatches,
                                                    MSenServiceDescription& aPattern)
    {
    TInt sessCount(iSessions.Count());
    TInt retVal(KErrNone);
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::FindAllMatchingServiceDescriptions(pattern)");
    for (TInt i=0; i<sessCount; i++)
        {
        if (iSessions[i]->Matches(aPattern))
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," -> matching service description found");

            TInt appendRetVal(aMatches.Append(iSessions[i]));
            if(appendRetVal!=KErrNone)
                {
                retVal = appendRetVal; // note: last error is retuned
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - FATAL: due OOM Append(SD) failed!");
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8(" - error code: %d"), appendRetVal));

                // Attempt to append other hits/matching SDs
                }
            }
        }
     TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8(" - total of %d matches found."), 
                        aMatches.Count()));

    return retVal; // last error from Append() is returned atm
    }


TInt CSenXMLDAO::AddServiceDescriptionToFrameworksL( CSenWSDescription& aPattern,
                                                     MSenRemoteServiceConsumer& aRemoteConsumer,
                                                     HBufC8*& aErrorMsg )
    {
    CSLOG_L(aRemoteConsumer.ConnectionId()  , KMinLogLevel,"CSenXMLDAO::AddServiceDescriptionToFrameworksL");

    TInt fwCount = iFrameworks.Count();
    CSLOG_FORMAT((aRemoteConsumer.ConnectionId() , KNormalLogLevel, _L8("Adding SD to (%d) SIF plug-ins."), fwCount));
    TInt retVal;
    TInt error(KErrNone);
    TInt sdCount(0);
    TPtrC8 patternFwId = aPattern.FrameworkId();
    HBufC8* pLastErrorMsg = NULL;
    for (TInt i=0; i<fwCount; i++ )
        {
        // altered the SIF inteface to return the count or leave
        // (no ref to int as argument)
        retVal = iFrameworks[i]->AddServiceDescriptionL( aPattern, aRemoteConsumer, pLastErrorMsg);
        if ( retVal > 0 )
            {
            sdCount += retVal;
            }
        
        TPtrC8 frameworkID = iFrameworks[i]->Id();
        
        // Store the latest error, in case that no match has yet been found(!)
        if ( (sdCount == 0) && 
             (retVal < 0 || (pLastErrorMsg && pLastErrorMsg->Length() > 0) )
           )
            {
            if( patternFwId.Length() == 0 || frameworkID == patternFwId )
                {
                // if pattern provides NO framework ID, or if it provides
                // mathing framework ID, then collect error message from
                // last SIF plugin in the array:
                if ( pLastErrorMsg )
                    {
                    delete aErrorMsg;
                    aErrorMsg = pLastErrorMsg;
                    pLastErrorMsg = NULL;
                    }
                if ( retVal < 0 )
                    {
                    error = retVal;
                    }
                }
            }
        CSLOG_L(aRemoteConsumer.ConnectionId()  , KNormalLogLevel,"CSenXMLDAO::AddServiceDescriptionToFrameworksL:");
        CSLOG_FORMAT((aRemoteConsumer.ConnectionId()  , KNormalLogLevel, _L8("- After %S SIF, the add count is [%d]"), &frameworkID, sdCount));
        }
    
    if ( pLastErrorMsg )
        {
        delete pLastErrorMsg;
        }
        
    if ( sdCount > 0 )
        {
        retVal = sdCount;
        delete aErrorMsg;
        aErrorMsg = NULL;
        }
    else
        {
        retVal = error;
        }
        
    return retVal;
    }


TInt CSenXMLDAO::NotifyFrameworksL(const TDesC8& aFrameworkID,
                                   const TInt aEvent,
                                    TAny* aArgument)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::NotifyFrameworksL");
    TInt fwCount = iFrameworks.Count();
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("There are %d loaded SIF plug-in(s)."), fwCount));
    TInt retVal(KErrNone);
    for (TInt i=0; i<fwCount; i++ )
        {
        TPtrC8 fwID = iFrameworks[i]->Id();
        if(aFrameworkID.Length()==0 || aFrameworkID == fwID)
            {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("+ Notified '%S' SIF. Event (%d)"),
                            &fwID,
                            aEvent));
            
            TInt pluginRetVal = iFrameworks[i]->OnEvent(aEvent, aArgument);
            if(retVal==KErrNone && pluginRetVal!=KErrNone)
                {
                retVal = pluginRetVal;
                }
            }   
#ifdef _SENDEBUG
        else
            {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("-Not notifying '%S' SIF [framework ID does not match]"),
                &fwID));
            }
#endif // _SENDEBUG
        }
    return retVal;
    }


TInt CSenXMLDAO::SaveL(const CSenServiceSession& /* aServiceSession */ )
    {
    Save(); //This implementation does not save single objects (yet).
    return KErrNone;
    }

TInt CSenXMLDAO::SaveL(const CSIF& /* aServiceInvocationFramework */)
    {
    Save(); //This implementation does not save single objects (yet).
    return KErrNone;
    }

CDesC8Array& CSenXMLDAO::SupportedFrameworksL()
    {
    TInt fwCount(iFrameworks.Count());
    CDesC8Array* retVal = new (ELeave) CDesC8ArraySeg(fwCount);
    for (TInt i=0; i<fwCount; i++)
        {
        retVal->AppendL(*iFrameworks[i]->Id().AllocL());
        }
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenXMLDAO::SupportedFrameworksL(): Count: (%d)"),
                        retVal->Count()));
    return *retVal;
    }

TInt CSenXMLDAO::ContainsServiceDescriptionL(TBool& aContains,
                                             CSenWSDescription& aPattern)
    {
    aContains = EFalse;
    TInt sessCount = iSessions.Count();
    for (TInt i=0; i<sessCount && !aContains; i++)
        {
        CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
        CleanupStack::PushL(pBuf);
        RBufWriteStream bufWs(*pBuf);
        CleanupClosePushL(bufWs);
        iSessions[i]->WriteAsXMLToL(bufWs);
        CSenWSDescription* pSd = CSenWSDescription::NewLC();
        pSd->SetReader(*Reader());
        pSd->ParseL(pBuf->Ptr(0));

        if (pSd->ConsistsOfL(aPattern))
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenXMLDAO::ContainsServiceDescriptionL(): matches given pattern:")));
            aContains = ETrue;
            }
        CleanupStack::PopAndDestroy(3); // pSd, bufWs, pBuf
        }
    
    return KErrNone;
    }


TInt CSenXMLDAO::CheckDefaultFrameworks()
    {
    // Do only once per read-text being parsed..
    TInt retCode(KErrNone);
    TInt leaveCode(KErrNone);

    TRAP(leaveCode, DefaultFrameworkL(KDefaultIdWsfFrameworkID, 
                                      KDefaultFrameworkCue1));
#ifdef _SENDEBUG
    if(leaveCode!=KErrNone)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CSenXMLDAO::Load() - DefaultFrameworkL() leaved: %d"),
            leaveCode));
        }
#else
    if(leaveCode!=KErrNone)
        {
        retCode = leaveCode;
        }
#endif

    leaveCode = KErrNone;
    TRAP(leaveCode, DefaultFrameworkL(KDefaultBasicWebServicesFrameworkID, 
                                      KDefaultFrameworkCue2));
#ifdef _SENDEBUG
    if(leaveCode!=KErrNone)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CSenXMLDAO::Load() - DefaultFrameworkL() leaved: %d"),
            leaveCode));
        }
#else
    if(leaveCode!=KErrNone)
        {
        retCode = leaveCode; // overrides possible leave from ID-WSF ECOM loading..
        }
#endif

    leaveCode = KErrNone;
    TRAP(leaveCode, DefaultFrameworkL(KDefaultRestServicesFrameworkID, 
                                      KDefaultFrameworkCue3));

#ifdef _SENDEBUG
    if(leaveCode!=KErrNone)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CSenXMLDAO::Load() - DefaultFrameworkL() leaved: %d"),
            leaveCode));
        }
#else
    if(leaveCode!=KErrNone)
        {
        retCode = leaveCode; // overrides possible leave from ID-WSF & WS-I ECOMs loading..
        }
#endif

    leaveCode = KErrNone;
    TRAP(leaveCode, DefaultFrameworkL(KDefaultWSStarFrameworkID, 
                                      KDefaultFrameworkCue4));

#ifdef _SENDEBUG
    if(leaveCode!=KErrNone)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CSenXMLDAO::Load() - DefaultFrameworkL() leaved: %d"),
            leaveCode));
        }
#else
    if(leaveCode!=KErrNone)
        {
        retCode = leaveCode; // overrides possible leave from ID-WSF & WS-I & REST ECOMs loading..
        }
#endif

    leaveCode = KErrNone;
    TRAP(leaveCode, DefaultFrameworkL(KDefaultAtomPubFrameworkID, 
                                      KDefaultFrameworkCue5));

#ifdef _SENDEBUG
    if(leaveCode!=KErrNone)
        {
		TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CSenXMLDAO::Load() - DefaultFrameworkL() leaved: %d"),
            leaveCode));
        }
#else
    if(leaveCode!=KErrNone)
        {
        retCode = leaveCode; // overrides possible leave from ID-WSF & WS-I & REST ECOMs loading..
        }
#endif

//################OVI PLUGIN
    leaveCode = KErrNone;
    TRAP(leaveCode, DefaultFrameworkL(KDefaultOviFrameworkID, 
                                      KDefaultFrameworkCue6));

#ifdef _SENDEBUG
    if(leaveCode!=KErrNone)
        {
        LOG_WRITEFORMAT((_L8("CSenXMLDAO::Load() - DefaultFrameworkL() leaved: %d"),
            leaveCode));
        }
#else
    if(leaveCode!=KErrNone)
        {
        retCode = leaveCode; // overrides possible leave from ID-WSF & WS-I & REST ECOMs loading..
        }
#endif
    return retCode;
    }



TPtrC8 CSenXMLDAO::TransportPluginCueBySchemeL(const TPtrC8& aScheme)
    {
    TPtrC8 cue = KNullDesC8();

    TInt index = iTransportMap.Find(aScheme);
    if(index!=KErrNotFound)
        {
        const HBufC8* pCue = iTransportMap.ValueAt(index);
        if(pCue)
            {
            cue.Set(*pCue);
            }
        }
    return cue;
    }

TInt CSenXMLDAO::CheckDefaultTransports()
    {
    // Note: HTTP Channel is the highest priority default, also in case
    // that scheme is not provided, so it does not need to be
    // added as default (CSenTransport::NewL() without cue returns the
    // default version HTTP Channel Transport plug-in).

    // Check whether local scheme default has been overridden by 
    // configuration file (sensessions.xml). The configuration file
    // has now been loaded, so one should not discard any changes to
    // default scheme-cue mappings that were read from that file.

    TInt retVal(KErrNone);
    TInt index = iTransportMap.Find(KLocalTransportScheme);
    if(index==KErrNotFound)
        {
        // apply & map the default local transport ECOM cue with its scheme
        HBufC8* pScheme = KLocalTransportScheme().Alloc();
        HBufC8* pCue = KDefaultLocalTransportPluginCue().Alloc();

        if(pScheme && pCue)
            {
            TInt retCode = iTransportMap.Append(pScheme, pCue);
            if(retCode!=KErrNone)
                {
                retVal = retCode;
                }
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenXMLDAO::CheckDefaultTransportsL - Append(local) returned: %d"), 
                retVal));
            }
        }

    index = iTransportMap.Find(KTcpTransportScheme);
    if(index==KErrNotFound)
        {
        // apply & map the default local transport ECOM cue with its scheme
        HBufC8* pScheme = KTcpTransportScheme().Alloc();
        HBufC8* pCue = KDefaultVirtualTcpTransportPluginCue().Alloc();
        if(pScheme && pCue)
            {
            TInt retCode = iTransportMap.Append(pScheme, pCue);
            if(retCode!=KErrNone)
                {
                retVal = retCode;
                }
              TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenXMLDAO::CheckDefaultTransportsL - Append(tcp) returned: %d"), 
                retVal));
            }
        }
    return retVal; // returns the last error code
    }

MSenProvider& CSenXMLDAO::LookupHostletForL(const TDesC8& aHostletEndpoint,
                                            const TDesC& aReqThreadId,
                                            const TDesC8& aReqConsumerId)
    {
    // This method needs to be wrapped inside critical section 
    // NOTE: do *NOT* file log before this line(!):

    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::LookupHostletForL:");
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aReqThreadId));
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aReqConsumerId));

    
    // LIST OF SHARABLE PROVIDERS IS USED TO LOOKUP A MATCHING,
    // SHARABLE PROVIDER, which is accessed by multiple requests,
    // possibly even by multiple threads at the same time (if
    // provider's type is non-threadsafe, meaning that it 
    // itself implements mutex for incoming ServiceL calls etc.
    TInt sharableCount(iSharableProviders.Count());
    CSenProvider* pHostlet = NULL;

    for(TInt s=0; s<sharableCount; s++)
        {
        pHostlet = iSharableProviders[s];
        if(pHostlet)
            {
            if(pHostlet->Endpoint()==aHostletEndpoint)
                {
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Matching sharable hostlet found from cache: 0x%X"), &pHostlet));
                return *pHostlet;
                }
            }
        }

    
    // Lookup unsharable providers, which EITHER are non-threadsafe
    // (shared between several simultaneous requests from same consumer)
    // OR which wish to stay on background and support reinitialization.
    CSenHostletLookupInfo* pLookupInfo = 
        CSenHostletLookupInfo::NewLC(aReqThreadId, aReqConsumerId);

    // It is not actually possible to get match with any threadsafe, 
    // cached provider from this find:
    TInt index = iUnsharableProviders.Find(*pLookupInfo);
    CleanupStack::PopAndDestroy(); // pLookupInfo
    pLookupInfo = NULL;

    if(index!=KErrNotFound)
        {
        pHostlet = (CSenProvider*) iUnsharableProviders.ValueAt(index);
        pLookupInfo = iUnsharableProviders.KeyAt(index);
        if(pHostlet && pLookupInfo && !pHostlet->Threadsafe()) // sanity check
            {
            // Next line is critical; when count goes to zero,
            // provider can be de-allocated from memory!
            pLookupInfo->IncrementLookupCount();
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- A non-threadsafe, unsharable local provider (hostlet) was found: 0x%X - lookup info: 0x%X, lookup count: %d"), pHostlet, pLookupInfo, pLookupInfo->LookupCount()));
            if(pHostlet->Reinitializable())
                {
                pHostlet->ReinitL();
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Provider was also reinitialized.");
                }
            return *pHostlet;
            }
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"- Matching unsharable provider was NOT found from the cache.");
        }
#endif

    
    // The provider has not yet been cached into memory, so load it:
    // This method call *can leave*, if ECOM lookup fails!
    // In case of leave Propagate meaningful error code to Remote Service Consumer 
    TInt leaveCode(KErrNone); 
    TRAP( leaveCode, pHostlet = CSenProvider::NewL(aHostletEndpoint) );
    if( !pHostlet )
    	{
    	if( leaveCode )
    		{
    		TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Could not load Local Provider plugin, leave: %d"), leaveCode));	
    		User::Leave( KErrSenHostNotAvailable ); // throw forward : meaningful errcode to consumer
    		}
    	}
    	
    CleanupStack::PushL(pHostlet);

    TInt append(KErrNone);
	if (pHostlet)
	{
    if(pHostlet->Sharable())
        {
        append = iSharableProviders.Append(pHostlet);
        if(append==KErrNone)
            {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Loaded new sharable hostlet instance: 0x%X"), &pHostlet));
            CleanupStack::Pop(); // pHostlet
            }
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- iSharableProviders.Append failed."); // OOM case
            CleanupStack::PopAndDestroy(); // pHostlet
            }
        }
    else // unsharable provider
        {
        // LIST OF UNSHARABLE PROVIDERS IS USED MAINLY TO MAKE IT
        // POSSIBLE TO DE-ALLOCATE LOADED ECOM PLUG-IN INSTANCIES.
        // In MCoreServiceManager there ReleaseHostletL() method gets
        // called, which means that such provider may be removed from
        // the map and destroyed. Otherwise, such hostlets are destroyed,
        // when server (main thread) goes down.
        
        pLookupInfo = NULL;
        if(pHostlet->Threadsafe())
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"- Adding new threadsafe, unsharable provider into cache.");
            pLookupInfo = CSenHostletLookupInfo::NewLC(aReqThreadId, aReqConsumerId);
            }
        else
            {
            // Any non-threadsafe provider should compare whether consumer ID
            // is equal. The thread ID is irrelevant in the matching.
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"- Adding new non-threadsafe, unsharable provider into cache.");
            pLookupInfo = CSenHostletLookupInfo::NewLC(KNullDesC, aReqConsumerId);
            }

        pLookupInfo->IncrementLookupCount();
        append = iUnsharableProviders.Append(pLookupInfo, pHostlet);
        if(append==KErrNone)
            {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("- Loaded new unsharable hostlet instance: 0x%X, lookup info: 0x%X, lookup count: %d"), pHostlet, pLookupInfo, pLookupInfo->LookupCount()));
            CleanupStack::Pop(2); // pLookupInfo, pHostlet
            }
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- iUnsharableProviders.Append failed."); // OOM case
            CleanupStack::PopAndDestroy(2); // pLookupInfo, pHostlet
            }
        }
	}
    User::LeaveIfError(append); // KErrNoMemory
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenXMLDAO::LookupHostletForL: Completed");	
    return *pHostlet;
    }

TInt CSenXMLDAO::ReleaseHostletL(const MSenProvider* aHostlet,
                                 const TDesC& aReqThreadId,
                                 const TDesC8& aReqConsumerId)
    {
    // This method needs to be wrapped inside critical section 
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"CSenXMLDAO::ReleaseHostletL:");
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aHostlet->Endpoint()));
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aHostlet->Contract()));
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aReqThreadId));
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aReqConsumerId));


    // Currently, sharable hostlets cannot be released
    if(aHostlet->Sharable())
        {
        return KErrNotSupported;
        }

    // This method simple looks for any unsharable providers, which
    TInt unsharableCount(iUnsharableProviders.Count());
    TInt retVal(KErrNotFound);
    for(TInt u=0; u<unsharableCount; u++)
        {
        CSenHostletLookupInfo* pLookupInfo = iUnsharableProviders.KeyAt(u);
        MSenProvider* pHostlet = (MSenProvider*)iUnsharableProviders.ValueAt(u);
        // Check whether the hostlet instances match
        if(pHostlet && aHostlet != pHostlet)
            {
            continue; // no match
            }
        else if(aHostlet->Threadsafe())
            {
            if(!(pLookupInfo && pLookupInfo->ThreadId() == aReqThreadId
                           && pLookupInfo->ConsumerId() == aReqConsumerId))
                {
                // mismatch either in thread or consumer id, or both; so
                // check next one from the cache (map)
                continue; // no match
                }
            }
        else if(!(pLookupInfo && pLookupInfo->ConsumerId() == aReqConsumerId))
            {
            // even though thread (requester) is allowed to be change, the 
            // hostlet still needs to be invoked using same consumer id,
            // Since this is *not* the case, check next one from the cache (map)
            continue; // no match
            }
            
        // match: decrement lookup count
        pLookupInfo->DecrementLookupCount();
        TInt lookupCount = pLookupInfo->LookupCount();
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- LookupInfo: 0x%X, lookup count after decrement: %d"), pLookupInfo, lookupCount));
        if(pHostlet && pHostlet->StayOnBackground())
            {
            if(pHostlet->Reinitializable() && lookupCount==0)
                {
                // no-one is consuming the service, 
                // so re-init this hostlet instance:
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("- Reinitializing unsharable hostlet instance: 0x%x"), pHostlet));
                retVal = pHostlet->ReinitL();
                }
            }
        else if(lookupCount==0) // this is the last consumer
            {
            // no need to keep this instance on background
            // and no-one is consuming it's service
            // destroy the ECOM plug-in instance:
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("- Deleting unsharable hostlet instance: 0x%x"), pHostlet));
            retVal = iUnsharableProviders.RemoveByKey(*pLookupInfo); 
            }
        break; // release complete
        } // for each map item..
    return retVal; // KErrNone or KErrNotFound, or some error from ReinitL()
  }
void CSenXMLDAO::UpdateTouchSessionDBL(MSenServiceDescription& asd)
	{
	TInt count(0);
	
	RWSDescriptionArray matches;
	CleanupClosePushL(matches);
	
	FindAllMatchingServiceDescriptions(matches, asd);
	count = matches.Count();
	
	for(TInt i=0; i < count; i++)
		{
		TUint current_tick(0);
		TBuf8<32> tickBuf;
		
		CSenWSDescription* sd = matches[i];
		CSenElement& sdElem = sd->AsElement();
		
		current_tick = User::NTickCount();
		tickBuf.Num(current_tick);
		sdElem.AddAttrL(KTouch(), tickBuf);
		}
	
	CleanupStack::PopAndDestroy(&matches);
	// CleanupUnusedSessionDBL(); 
                                  // (don't de-serialize old items). Serialized 
	                              // objects cannot be de-allocated on the fly.
	}
	
void CSenXMLDAO::CleanupUnusedSessionDBL()
	{
	TInt count(0);
	
	RPointerArray<CSenElement> sdElemList;
	CleanupClosePushL(sdElemList);
	
	count = iSessions.Count();
	
	TUint32 current_tick(0);
	TUint32 db_ticks(0);
	TUint32 diff_ticks(0);
	
	for(TInt i=0; i < count; i++)
		{
				
		CSenXmlServiceDescription* sd = iSessions[i];
		CSenElement& sdElem = sd->AsElement();
		TPtrC8 localName = sdElem.LocalName();
		const TDesC8* val = sdElem.AttrValue(KTouch());
		if(val != NULL)
			{
			TLex8 lex;
            lex.Assign(*val);
            lex.Val(db_ticks, EDecimal);
            
            current_tick = User::NTickCount();
            diff_ticks = current_tick - db_ticks;
                        	
			if(KMaxTicks <= diff_ticks || current_tick < db_ticks)
				{
				delete iSessions[i];
        		iSessions.Remove(i);
        		
        		i--;
        		count--;
				}
			}
		}
	
	CleanupStack::PopAndDestroy(&sdElemList);	
	}	

CSenHostletLookupInfo* CSenHostletLookupInfo::NewL(const TDesC& aThreadIdFullName,
                                                   const TDesC8& aConsumerIdUrn)
    {
    CSenHostletLookupInfo* pNew = CSenHostletLookupInfo::NewLC(aThreadIdFullName,
                                                               aConsumerIdUrn);
    CleanupStack::Pop();
    return pNew;
    }

CSenHostletLookupInfo* CSenHostletLookupInfo::NewLC(const TDesC& aThreadIdFullName,
                                                  const TDesC8& aConsumerIdUrn)
    {
    CSenHostletLookupInfo* pNew = new (ELeave) CSenHostletLookupInfo;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aThreadIdFullName, aConsumerIdUrn);
    return pNew;
    }

void CSenHostletLookupInfo::ConstructL(const TDesC& aThreadIdFullName,
                                      const TDesC8& aConsumerIdUrn)
    {
    ipFullThreadName = aThreadIdFullName.AllocL();
    ipUniqueConsumerId = aConsumerIdUrn.AllocL();
    ipLookupCount = new (ELeave) TInt(0);
    }


CSenHostletLookupInfo::CSenHostletLookupInfo()
: ipFullThreadName(NULL),
  ipUniqueConsumerId(NULL),
  ipLookupCount(NULL)
    {
    }

CSenHostletLookupInfo::~CSenHostletLookupInfo()
    {
    delete ipFullThreadName;
    delete ipUniqueConsumerId;
    delete ipLookupCount;
    }

TPtrC CSenHostletLookupInfo::ThreadId() const
    {
    return *ipFullThreadName;
    }

TPtrC8 CSenHostletLookupInfo::ConsumerId() const
    {
    return *ipUniqueConsumerId;
    }


TInt CSenHostletLookupInfo::LookupCount() const
    {
    return *ipLookupCount;
    }

void CSenHostletLookupInfo::IncrementLookupCount()
    {
    (*ipLookupCount)++;
    }

void CSenHostletLookupInfo::DecrementLookupCount()
    {
    (*ipLookupCount)--;
    }


TBool CSenHostletLookupInfo::operator==(const CSenHostletLookupInfo& aHostletRequestor)
    {
    return((ipFullThreadName->Length()==0 || aHostletRequestor.ThreadId() == *ipFullThreadName)
            && aHostletRequestor.ConsumerId() == *ipUniqueConsumerId);
    
    //    return (aHostletRequestor.ThreadId() == *ipFullThreadName
//            && aHostletRequestor.ConsumerId() == *ipUniqueConsumerId);
    }


// END OF FILE

