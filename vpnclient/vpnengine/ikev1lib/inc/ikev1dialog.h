/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIkeDialog class headers
*
*/

#ifndef C_IKEV1DIALOG_H
#define C_IKEV1DIALOG_H


#include <e32base.h>
#include <f32file.h>
#include "vpnnotifierdefs.h"

class CIkev1Dialog;
class CIkev1PluginSession;
class RFs;
class MIkeDebug;

/** 
* IKE dialog complete
* @internalComponent
*/
class MIkeDialogComplete
{
	public:
		/** 
         * IKE dialog completed 
         * @internalComponent
         * 
         */
		virtual TInt DialogCompleteL(CIkev1Dialog* aDialog, TAny* aUserInfo, HBufC8* aUsername, HBufC8* aSecret, HBufC8* aDomain)=0; 
};



NONSHARABLE_CLASS(CDialogTimeout) : public CTimer
{
public:
    CDialogTimeout( MIkeDebug& aDebug );
    ~CDialogTimeout();
    void ConstructL(CIkev1Dialog* aDialog);
    
protected:
    // will cancel the current dialog
    void RunL();
    void DoCancel();
    
protected: // data
    CIkev1Dialog* iDialog; //Needed to cancel a dialog
    
private: // data
    MIkeDebug& iDebug;
};


NONSHARABLE_CLASS(CIkev1Dialog) : public CActive
{
public:
    static CIkev1Dialog* NewL( CIkev1PluginSession* aPluginSession,
                               CIkev1Dialog** aToQueAnchor,
                               MIkeDebug& aDebug );
    CIkev1Dialog( MIkeDebug& aDebug );
    ~CIkev1Dialog();

    void GetAsyncUNPWDialogL(TAny *aUserInfo, MIkeDialogComplete* aCallback);
    void GetAsyncUNAMEDialog(TAny *aUserInfo, MIkeDialogComplete* aCallback);    
    void GetAsyncSecureidDialogL(TAny *aUserInfo, MIkeDialogComplete*  aCallback);
    void GetAsyncSecureNextPinDialogL(TAny *aUserInfo, MIkeDialogComplete* aCallback);    
    void GetAsyncRespDialog(TPtr8 aChallenge, TAny *aUserInfo, MIkeDialogComplete* aCallback);
    void ShowErrorDialogL(TInt aDialogType, TAny *aUserInfo, MIkeDialogComplete*  aCallback);
	void StoreUserNameL(TPtr8 aUserName);
	void StartDialogL();
	
    static TInt GetSyncUNPWDialog(TDes& aUserName, TDes& aPassword);
    static void PurgeDialogQueue(CIkev1Dialog* aQueuedDialog);
	
	inline CIkev1PluginSession* PluginSession() { return iPluginSession;}
	inline CIkev1Dialog* NextDialog() { return iNext;}
	inline MIkeDialogComplete* Callback() { return iCallback;}
	inline TAny* UserInfo() { return iUserInfo;}		
	
protected:
    // active object stuff, completion and cancel callback functions
    void RunL();
    void DoCancel();
    
private:
    void ConstructL(CIkev1PluginSession* aPluginSession, CIkev1Dialog* *aToQueAnchor);
    void LaunchDialogL();
    TInt QueueDialog(CIkev1Dialog* aDialog);
    void DeQueueDialog(CIkev1Dialog* aDialog);
	HBufC8* CreateDialogInput(TIPSecDialogInfo& aDialogInfo, TBool aUserNameCache);
	HBufC8* GetUserNameFromFile();
	static TBool BuildEncryptionKey(const TDesC8& aSalt, TDes8& aEncryptionKey);		
    static TInt LauchSyncDialog(const TDesC8& aInput, TDes8& aOutput);  
    static HBufC8* ConvertPwdToOctetString(TDesC &aUnicodeBfr);
    
private:
    CIkev1PluginSession*   iPluginSession;
    RFs             iFs;
    CIkev1Dialog**  iToQueAnchor;   // Waiting queue anchor
    CIkev1Dialog*   iNext;          // For waiting queue
    TAny*           iUserInfo;
    MIkeDialogComplete* iCallback;
    TInt           iDialogType;
    CDialogTimeout* iTimeout;   
    RNotifier      iNotifier;
    HBufC8*        iInputData;
    TPckgBuf<TIPSecDialogOutput> iResponseBuf;
    MIkeDebug& iDebug;
};

/**--------------------------------------------------------------------------
 *
 * Structure TUserNameFileHdr
 * User name (encrypted) cache file header.
 * In this file is cached the last user name used in legacy
 * authentication. 
 *
 *--------------------------------------------------------------------------*/
#define USER_NAME_FILE_ID     0xcccccccc
struct TUserNameFileHdr
{
	TUint8      iSalt[8];
	TUint8      iIV[8];
	TUint32     iFileId;
};

#endif // C_IKEV1DIALOG_H
