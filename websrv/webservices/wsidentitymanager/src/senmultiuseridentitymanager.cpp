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
* Description:        
*
*/







#include <s32mem.h>
#include <s32file.h>
#include <sysutil.h>

#include "senmultiuseridentitymanager.h"

#include "senlogger.h"
#include "SenXmlUtils.h"

namespace
    {
    _LIT8(KUser, "User");
    _LIT8(KUsers, "Users");
    _LIT8(KUserName, "username");
    _LIT8(KPIN, "PIN");
    _LIT8(KActive, "active");
    _LIT8(KTrue, "1");
    _LIT8(KFalse, "0");
    _LIT8(KUsersStart, "<Users>");
    _LIT8(KUsersEnd, "</Users>");
    _LIT8(KUserStartFmt, "<User username=\"%S\" PIN=\"%S\" active=\"%d\">");
    _LIT8(KUserEnd, "</User>");
    const TInt KFLATBUF_SIZE = 128;
    const TInt KActiveValueSize = 1;
    
    const TInt KStateParsingIdentity = 100;
    }


EXPORT_C CSenMultiUserIdentityManager* CSenMultiUserIdentityManager::NewL(
                                            MSenCoreServiceManager& aManager)
    {
    CSenMultiUserIdentityManager* pNew = NewLC(aManager);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenMultiUserIdentityManager* CSenMultiUserIdentityManager::NewLC(
                                            MSenCoreServiceManager& aManager)
    {
    CSenMultiUserIdentityManager* pNew =
                        new (ELeave) CSenMultiUserIdentityManager(aManager);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }
                              
CSenMultiUserIdentityManager::CSenMultiUserIdentityManager(
                                            MSenCoreServiceManager& aManager)
    : CSenBaseIdentityManager(aManager)
    {
    }

EXPORT_C CSenMultiUserIdentityManager::~CSenMultiUserIdentityManager()
    {
    iUsers.ResetAndDestroy();
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenMultiUserIdentityManager::~CSenMultiUserIdentityManager");        
    }

void CSenMultiUserIdentityManager::ConstructL()
    {
    CSenBaseIdentityManager::ConstructL();
    //db was empty, take default one from Base class(loaded during Base::Contructl)
    if (!iActiveIdentity)
        {
        iActiveIdentity = iIdentity;
        iActiveUser = CSenUser::NewL(KNullDesC8, KNullDesC8, ETrue);    
        iActiveUser->Identites().Append(iActiveIdentity);
        iUsers.Append(iActiveUser);
        }
    else
        {
        //parsing from CSenBaseIdentityManager::ConstructL()
        //  caused, that iActiveIdentity is already set (because has been set by below "StartElement" method).
        //  So, orginal iIdentity should be replaced by iActiveIdentity (used by UpdateTouch functionality)
        delete iIdentity;
        iIdentity = NULL;
        iIdentity = iActiveIdentity;
        };
        
    }

void CSenMultiUserIdentityManager::StartElementL(
                                          const TDesC8& /*aNsUri*/,
                                          const TDesC8& aLocalName,
                                          const TDesC8& /*aQName*/,
                                          const RAttributeArray& aAttributes
                                          )
    {
    switch (iState)
        {
        case KStateIgnore:
            {
            if (aLocalName == KUser)
                {
                TPtrC8 userName = SenXmlUtils::AttrValue(aAttributes, KUserName);
                TPtrC8 pin = SenXmlUtils::AttrValue(aAttributes, KPIN);
                TPtrC8 active = SenXmlUtils::AttrValue(aAttributes, KActive);
                if (active == KTrue)
                    {
                    iParsedUser = CSenUser::NewL(userName, pin, ETrue);    
                    }
                else
                    {
                    iParsedUser = CSenUser::NewL(userName, pin);
                    }
                
                if (active == KTrue)
                    {
                    //2nd time active user?? within db
                    __ASSERT_ALWAYS(!iActiveUser, User::Panic(KSenUser, KErrAlreadyExists));
                    iActiveUser = iParsedUser;
                    }
                iUsers.Append(iParsedUser);
                iState = KStateParsingIdentity;
                }
            }
            break;
        
        case KStateParsingIdentity:
            {
            if (aLocalName ==  KIdentityElementLocalName)
                {
                CSenBaseIdentity* identity = CSenBaseIdentity::NewLC(ipDeviceID->Des());
                identity->SetAttributesL(aAttributes);
                identity->SetReader(*Reader());
                DelegateParsingL(*identity);
                __ASSERT_ALWAYS(iParsedUser, User::Panic(KSenUser, KErrAlreadyExists));
                iParsedUser->Identites().Append(identity);
                
                //current parsed user is active
                if (iParsedUser == iActiveUser)
                    {
                    iActiveIdentity = identity;
                    }
                CleanupStack::Pop(identity);
                }
            }
            break;
            
        default:
            break;
        }
    }
void CSenMultiUserIdentityManager::EndElementL(
    const TDesC8& /*aNsUri*/,
    const TDesC8& aLocalName,
    const TDesC8& /*aQName*/)
    {
    switch (iState)
        {
        case KStateIgnore:
            {
            if (aLocalName ==  KUsers)//end parsing
                {
                if (iUsers.Count() == 1)
                    {
                    CSenUser* user = iUsers[0];
                    iActiveUser = user;
                    iActiveIdentity = iActiveUser->Identites()[0];
                    }
                }
            break;
            }
        case KStateParsingIdentity:
            {
            if (aLocalName ==  KUser)
                {
                iState = KStateIgnore;    
                }
            }
            break;
        }
    }
        

MSenIdentity& CSenMultiUserIdentityManager::IdentityL()
    {
    return (MSenIdentity&)*iActiveIdentity;
    }

TInt CSenMultiUserIdentityManager::UserNameL(HBufC8*& aUserName)
    {
    delete aUserName;
    aUserName = NULL;
    aUserName = iActiveUser->UserName().AllocL();
    return KErrNone;
    }





TInt CSenMultiUserIdentityManager::WriteConfigurationToL( const TDesC& aFile )
    {
    // First, collect everything into MEMORY
    CBufFlat *pBuf = CBufFlat::NewL(KFLATBUF_SIZE);
    CleanupStack::PushL(pBuf);

    RBufWriteStream bufWs(*pBuf);
    CleanupClosePushL(bufWs);
    bufWs.WriteL(KUsersStart);


//############# main loop #########
    TInt count = iUsers.Count();
    for (TInt i=0; i<count; i++)
        {
        RBuf8 userTag;
        CSenUser* user = iUsers[i];
        userTag.CreateL(KUserStartFmt().Length() + 
                     user->PIN().Length() + 
                     user->UserName().Length() + 
                     KActiveValueSize //1
                     );
    
        if (user->Active())
            {
            userTag.Format(KUserStartFmt, &(user->UserName()),  &(user->PIN()), KTrue); 
            }
        else
            {
            userTag.Format(KUserStartFmt, &(user->UserName()),  &(user->PIN()), KFalse);
            }
        
                     
        bufWs.WriteL(userTag);
        TInt count2 = iUsers[i]->Identites().Count();
        for (TInt j=0; j<count2; j++)
            {
            CSenBaseIdentity* identity = iUsers[i]->Identites()[j];
            identity->WriteAsXMLToL(bufWs);
            }
        bufWs.WriteL(KUserEnd);
        userTag.Close();
        }

//#############
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
    CleanupStack::PopAndDestroy(3);
    return KErrNone;
    }

	
			
// End of file
