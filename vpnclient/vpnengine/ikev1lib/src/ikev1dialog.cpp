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
* Description:   CIkeDialog class implementation
*
*/

#include <random.h>

#include "ikev1dialog.h"
#include "dhparameters.h"
#include "ikedebug.h"
#include "ikev1pluginsession.h"
#include "ikev1crypto.h"
#include "ikev1filesdef.h"


CIkev1Dialog::CIkev1Dialog( MIkeDebug& aDebug )
 : CActive( EPriorityStandard ),
   iDebug( aDebug )
{
    CActiveScheduler::Add(this);    //Added to the Active Scheduler
}

CIkev1Dialog::~CIkev1Dialog()
{
    DEBUG_LOG(_L("CIkev1Dialog destructed"));
	    		
    DeQueueDialog(this);
    
    if(iTimeout)
        {
        iTimeout->Cancel();
        }
    Cancel();           // Dialog itself    		

    delete iTimeout;
    delete iInputData;
    
    iFs.Close();
}

void CIkev1Dialog::PurgeDialogQueue(CIkev1Dialog* aQueuedDialog)
{
    CIkev1Dialog* NextDialog;
    while ( aQueuedDialog ) {
        NextDialog = aQueuedDialog->iNext;
        delete aQueuedDialog;
        aQueuedDialog = NextDialog;
    }   
}   

void CIkev1Dialog::DoCancel()
{
    iNotifier.CancelNotifier(KUidVpnDialogNotifier);       
    iNotifier.Close();
	DEBUG_LOG(_L("CIkev1Dialog::DoCancel() OK"));				
}

void CIkev1Dialog::ConstructL(CIkev1PluginSession* aPluginSession, CIkev1Dialog* *aToQueAnchor)
{
    User::LeaveIfError(iFs.Connect());
    iTimeout = new (ELeave) CDialogTimeout( iDebug );
    iTimeout->ConstructL(this);
    iToQueAnchor = aToQueAnchor;
	iPluginSession = aPluginSession;
}

CIkev1Dialog* CIkev1Dialog::NewL( CIkev1PluginSession* aPluginSession,
                              CIkev1Dialog** aToQueAnchor,
                              MIkeDebug& aDebug )
{
    CIkev1Dialog* Dialog = new (ELeave) CIkev1Dialog( aDebug );
    Dialog->ConstructL( aPluginSession, aToQueAnchor );
     
    return Dialog;
}

void CIkev1Dialog::StoreUserNameL(TPtr8 aUserName)
{
/*--------------------------------------------------------------------
 *
 *  Store specified user name into cache file (used as init value in
 *  the next user name specific dialog).
 *  User name shall be encrypted (DES) before stored into cache file. 
 *
 *---------------------------------------------------------------------*/

 
	if (aUserName.Length() == 0)
	    {
	    User::Leave(KErrArgument);
	    }
		
	//
	// Allocate buffer for file header and encrypted key
	//

	HBufC8* HeaderBfr = HBufC8::NewLC(aUserName.Length() + sizeof(TUserNameFileHdr) + 32);
    
	TUserNameFileHdr* FileHeader = (TUserNameFileHdr*)HeaderBfr->Ptr();
	//
	// Get random data values for salt and IV. 
	//
	TPtr8 ptr((TUint8*)FileHeader, sizeof(TUserNameFileHdr));
	ptr.SetLength(sizeof(TUserNameFileHdr));
	TRandom::RandomL(ptr);

	FileHeader->iFileId = USER_NAME_FILE_ID;	
	//
	// Build encryption key from just created salt data and fixed
	// secret passphrase using MD5 hash
	//
	TBuf8<16>  EncryptionKey;
	TPtr8 SaltPtr((TUint8*)FileHeader->iSalt, 8, 8);
	User::LeaveIfError(CIkev1Dialog::BuildEncryptionKey(SaltPtr, EncryptionKey));
	   //
	   // Encrypt user name data with just created key. 
	   // Because DES is used as encryption algorithm, the eight first
	   // octets of created encryption octets is used as encryption key.
	   //
	    TInt EncrLth = 0;
    EncrLth = SymmetricCipherL((TUint8*)aUserName.Ptr(),
					          ((TUint8*)FileHeader + sizeof(TUserNameFileHdr)),
           					    aUserName.Length(), FileHeader->iIV, (TUint8*)EncryptionKey.Ptr(), ETrue);
	if ( EncrLth ) 
	    {
        //
        // Write encrypted data into user name file
        //
		RFile NameFile;		  

		TBuf<128> Ppath;	
		User::LeaveIfError(iFs.PrivatePath(Ppath));

	    Ppath.Append(USER_NAME_CACHE_FILE);
	    TInt err = iFs.CreatePrivatePath(EDriveC);
	    if (err != KErrNone &&
	        err != KErrAlreadyExists)
	        {
	        User::Leave(err);
	        }
	    User::LeaveIfError(NameFile.Replace(iFs, Ppath, EFileShareAny|EFileWrite));

		TPtrC8 EncryptedData((TUint8*)FileHeader, sizeof(TUserNameFileHdr) + EncrLth); 

		NameFile.Write(EncryptedData);
		NameFile.Close();
	    }

    CleanupStack::PopAndDestroy(); // Delete encryption buffer
}

/*--------------------------------------------------------------------
 *
 *  Asynchronous dialog is completed. 
 *
 *---------------------------------------------------------------------*/
void CIkev1Dialog::RunL()
{
    TInt   delete_obj = 1;
    HBufC8 *un_bfr    = NULL;
    HBufC8 *pw_bfr    = NULL;
    CIkev1Dialog* NextDialog = iNext;

    iNotifier.CancelNotifier(KUidVpnDialogNotifier);            
    iNotifier.Close();

    if ( iStatus.Int() == KErrNone )
	{
       if ( iCallback )
	   {
          TIPSecDialogOutput& resp = iResponseBuf();
          un_bfr = ConvertPwdToOctetString(resp.iOutBuf);
		  pw_bfr = ConvertPwdToOctetString(resp.iOutBuf2);		  
       }          
    }

    if ( iCallback )
	{
		TInt err;
		TRAP(err, delete_obj = iCallback->DialogCompleteL(this, iUserInfo,
								                          un_bfr,               //User name
			                                              pw_bfr,               //Password
			                                              NULL));               //domain
	    delete un_bfr; 
	    delete pw_bfr;
	    if ( err != KErrNone )
		  delete_obj = 1;
    }
    
    if ( delete_obj )
	{
       delete this; 
    }
    
    //
    //  Start a dialog from wait queue if there is some 
    //
    if ( NextDialog )
       NextDialog->StartDialogL();
}


/*--------------------------------------------------------------------
 *
 *  Get user name and password data for Legacy authentication
 *  This is a synchronous dialog which does NOT convert user name and
 *  password data into the 8-bit ASCII text 
 *
 *---------------------------------------------------------------------*/
TInt CIkev1Dialog::GetSyncUNPWDialog(TDes& aUserName, TDes& aPassword)
{
TIPSecDialogOutput output;


    TIPSecDialogInfo dialog_input(TKMDDialog::EUserPwd, 0);
	
    TPckgBuf<TIPSecDialogInfo> InfoBuf(dialog_input);//package it in appropriate buf
	
    TPckgBuf<TIPSecDialogOutput> ResponseBuf(output);//create the buf to receive the response

    TInt status = LauchSyncDialog(InfoBuf, ResponseBuf);
    if ( status == KErrNone ) {
       TIPSecDialogOutput& resp = ResponseBuf();
       aUserName = resp.iOutBuf;
       aPassword = resp.iOutBuf2;      
    }   

    return status;
}

void CIkev1Dialog::ShowErrorDialogL(TInt aDialogText, TAny *aUserInfo, MIkeDialogComplete*  aCallback )
{
    iDialogType = TNoteDialog::EInfo;
    iUserInfo   = aUserInfo;
    iCallback   = aCallback; // For asynchronous dialog RunL

	TIPSecDialogInfo dialog_input(TNoteDialog::EInfo, aDialogText);		
	iInputData = CreateDialogInput(dialog_input, EFalse);// FALSE = Do not use user name cache    
    if ( iInputData ) 
       LaunchDialogL();   //launch the dialog 
}

/*--------------------------------------------------------------------
 *
 *  Get user name and password data for Legacy authentication
 *
 *---------------------------------------------------------------------*/
void CIkev1Dialog::GetAsyncUNPWDialogL(TAny *aUserInfo, MIkeDialogComplete*  aCallback)
{
	DEBUG_LOG2(_L("CIkev1Dialog::GetAsyncUNPWDialogL(), aUserInfo =  %x, aCallback = %x"), aUserInfo, aCallback);
	
    iDialogType = TKMDDialog::EUserPwd;
    iUserInfo   = aUserInfo;
    iCallback   = aCallback; // For asynchronous dialog RunL

	TIPSecDialogInfo dialog_input(TKMDDialog::EUserPwd, 0);		
	iInputData = CreateDialogInput(dialog_input, ETrue);// TRUE = Use user name cache    	
    if ( iInputData )
       LaunchDialogL();   //launch the dialog
}

/*--------------------------------------------------------------------
 *
 *  Get user name and Secure ID pin data for Legacy authentication
 *
 *---------------------------------------------------------------------*/
void CIkev1Dialog::GetAsyncSecureidDialogL(TAny *aUserInfo, MIkeDialogComplete*  aCallback)
{
	DEBUG_LOG2(_L("CIkev1Dialog::GetAsyncSecureidDialogL(), aUserInfo =  %x, aCallback = %x"), aUserInfo, aCallback);
	
    iDialogType = TKMDDialog::ESecurIdPin;
    iUserInfo   = aUserInfo;
    iCallback   = aCallback; // For asynchronous dialog RunL

	TIPSecDialogInfo dialog_input(TKMDDialog::ESecurIdPin, 0);		
	iInputData = CreateDialogInput(dialog_input, ETrue);// TRUE = Use user name cache    	
    if ( iInputData )
       LaunchDialogL();   //launch the dialog
}

/*--------------------------------------------------------------------
 *
 *  Get user name and Secure ID next pin data for Legacy authentication
 *
 *---------------------------------------------------------------------*/
void CIkev1Dialog::GetAsyncSecureNextPinDialogL(TAny *aUserInfo,  MIkeDialogComplete* aCallback)
{
	DEBUG_LOG2(_L("CIkev1Dialog::GetAsyncSecureNextPinDialogL(), aUserInfo =  %x, aCallback = %x"), aUserInfo, aCallback);
	
    iDialogType = TKMDDialog::ESecurIdNextPin;
    iUserInfo   = aUserInfo;
    iCallback   = aCallback; // For asynchronous dialog RunL
    
	TIPSecDialogInfo dialog_input(TKMDDialog::ESecurIdNextPin, 0);		
	iInputData = CreateDialogInput(dialog_input, ETrue);// TRUE = Use user name cache    	
    if ( iInputData )
       LaunchDialogL();   //launch the dialog
}


/*--------------------------------------------------------------------
 *
 * For future use (for challenge/response type  Legacy authentication)
 *
 *---------------------------------------------------------------------*/
void CIkev1Dialog::GetAsyncUNAMEDialog(TAny* /*aUserInfo*/, MIkeDialogComplete*  /*aCallback*/)
{
}

void CIkev1Dialog::GetAsyncRespDialog(TPtr8 /*aChallenge*/, TAny* /*aUserInfo*/, MIkeDialogComplete* /*aCallback*/)
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Private methods
//
///////////////////////////////////////////////////////////////////////////////
HBufC8* CIkev1Dialog::CreateDialogInput(TIPSecDialogInfo& aDialogInfo, TBool aUserNameCache)
{
	//
	// Create dialog input data buffer. Concatenate cached user name
	// string into input data if requested and if cached name exists
	//
	HBufC8* DialogInput;
	HBufC8* UserName = NULL;
	TInt    UserNameLth = 0;	
	TPckgBuf<TIPSecDialogInfo> infoBuf(aDialogInfo);
	
	if ( aUserNameCache ) {
	   UserName = GetUserNameFromFile();
	   if ( UserName )
		  UserNameLth = UserName->Length();
	}

	DialogInput = HBufC8::New(sizeof(TIPSecDialogInfo) + UserNameLth);
	if ( DialogInput ) {
	   DialogInput->Des().Copy(infoBuf);
	   if ( UserName ) {
	      DialogInput->Des().Append(UserName->Des());
		  delete UserName;
	   }	  
	}   

	return DialogInput;
}   

void CIkev1Dialog::LaunchDialogL()
{
    //
    // Launch the dialog if there is no dialog already going 
    //
    if ( QueueDialog(this) == 1 ) 
    {
        StartDialogL();
    }      
}   

void CIkev1Dialog::StartDialogL()
{
    //
    // Start an asynchronous dialog 
    //
    User::LeaveIfError(iNotifier.Connect());
    iNotifier.StartNotifierAndGetResponse(iStatus,
                                          KUidVpnDialogNotifier,
                                         (TPckgBuf<TIPSecDialogInfo>&)(*iInputData),
                                          iResponseBuf);
    SetActive();
}   


TInt CIkev1Dialog::LauchSyncDialog(const TDesC8& aInput, TDes8& aOutput)
{
    RNotifier notifier;
    TInt err = notifier.Connect();
    if(err != KErrNone)
        {
        return err;
        }
    
    TRequestStatus status;
    notifier.StartNotifierAndGetResponse(status, KUidVpnDialogNotifier, aInput, aOutput);
    User::WaitForRequest( status );
    
    notifier.CancelNotifier(KUidVpnDialogNotifier);
    notifier.Close();
    
    return status.Int();
}

TInt CIkev1Dialog::QueueDialog(CIkev1Dialog* aDialog)
{
    TInt DialogCount = 1;
    aDialog->iNext   = NULL;
    CIkev1Dialog* QueuedDialog = *aDialog->iToQueAnchor;
    
    if ( QueuedDialog  ) 
    {
       DialogCount ++;  
       while ( QueuedDialog->iNext ) {
           QueuedDialog = QueuedDialog->iNext;
           DialogCount ++;
       }
       QueuedDialog->iNext = aDialog;
    }    
    else *aDialog->iToQueAnchor = aDialog;
    
    return DialogCount;
}

void CIkev1Dialog::DeQueueDialog(CIkev1Dialog* aDialog)
{
    CIkev1Dialog* PreviousDialog = NULL;
    CIkev1Dialog* QueuedDialog   = *aDialog->iToQueAnchor;
    
    while ( QueuedDialog ) {
        if ( QueuedDialog == aDialog ) {
           if ( PreviousDialog )
                PreviousDialog->iNext  = QueuedDialog->iNext;
           else *aDialog->iToQueAnchor = QueuedDialog->iNext;
        }
        PreviousDialog = QueuedDialog;
        QueuedDialog   = QueuedDialog->iNext;       
    }    
}   

HBufC8* CIkev1Dialog::GetUserNameFromFile()
{
/*--------------------------------------------------------------------
 *
 *  Get user name default value from encrypted cache file
 *
 *---------------------------------------------------------------------*/
	//
	// Allocate buffer for file header and encrypted key
	//
	HBufC8* UserNameBfr = NULL;
	RFile UserNameFile;
	if ( UserNameFile.Open(iFs, USER_NAME_CACHE_FILE, EFileRead) == KErrNone ) {
		TInt FileSize = 0;
		UserNameFile.Size(FileSize);
		if ( (FileSize > 0) && (FileSize < 256) )  {    
			HBufC8* FileData = HBufC8::New(FileSize);
			if ( FileData ) {
               //
               // Read encrypted file data into the allocated buffer.
	           //
			   TPtr8 FileDataPtr(FileData->Des());
			   if ( UserNameFile.Read(FileDataPtr) == KErrNone )  {
				  //
				  // Build decryption key and decrypt user name data.
				  // Both salt data needed in key generation and IV
				  // value required in decryption are found from
				  // encrypted file header
				  //
				  TUserNameFileHdr* FileHeader = (TUserNameFileHdr*)FileData->Ptr();
				  if ( FileHeader->iFileId == USER_NAME_FILE_ID ) {
    			     TBuf8<16>  DecryptionKey;
				     TPtr8 SaltPtr((TUint8*)FileHeader->iSalt, 8, 8);				  
				     if ( CIkev1Dialog::BuildEncryptionKey(SaltPtr, DecryptionKey) ) {
					    TInt EncrLth = FileSize - sizeof(TUserNameFileHdr);
					    TUint8* UserNameRawPtr = (TUint8*)FileHeader + sizeof(TUserNameFileHdr); 					 
					    TInt err;
					    TRAP(err, EncrLth = SymmetricCipherL(UserNameRawPtr, UserNameRawPtr, EncrLth,
						                                     FileHeader->iIV, (TUint8*)DecryptionKey.Ptr(), EFalse));
    				    if ( (err == KErrNone) && EncrLth ) {
						   //
						   // Allocate a HBufC8 for decrypted user name
						   //
					       UserNameBfr = HBufC8::New(EncrLth);
						   if ( UserNameBfr )
						      UserNameBfr->Des().Copy(UserNameRawPtr, EncrLth);
						}
					 }	 
				  }  
			   }
			   delete FileData;
			}
		}	
	}
	
	UserNameFile.Close();
	return UserNameBfr;
}


HBufC8 *CIkev1Dialog::ConvertPwdToOctetString(TDesC &aUnicodeBfr)
{
/*--------------------------------------------------------------------
 *
 *  Convert password from Unicode string to 8-bit octet string
 *
 *---------------------------------------------------------------------*/
    HBufC8 *octet_data = HBufC8::New(aUnicodeBfr.Length());
	if ( octet_data ) {
       TPtr8 ptr8(octet_data->Des());
       ptr8.Copy(aUnicodeBfr);
    }   
    return octet_data;
}



TBool CIkev1Dialog::BuildEncryptionKey(const TDesC8& aSalt, TDes8& aEncryptionKey)
{
/*--------------------------------------------------------------------
 *
 *  Build encryption key for user name data cipher.
 *  The encryption key is created as follows:
 *  DH group 5 (MODP 1536) prime is used as passphrase seed so
 *  that MODP_1536_PRIME_LENGTH/4 octets of seed is taken from prime
 *  starting from position MODP_1536_PRIME_LENGTH/2.
 *  The specified salt is concatenated with that data.
 *  The MD5 hash over that shall be the encryption key (max key then
 *  128 bits)
 *
 *---------------------------------------------------------------------*/
	//
	// Allocate buffer for key seed data
	//
	HBufC8* SeedDataBfr = HBufC8::New(aSalt.Length() + MODP_1536_PRIME_LENGTH/4);
	if ( !SeedDataBfr )
	   return EFalse;
	
	TPtr8 SeedDataPtr(SeedDataBfr->Des()); 
	TPtrC8 PassPhrasePtr((TUint8 *)&MODP_1536_PRIME[MODP_1536_PRIME_LENGTH/2],
	                               MODP_1536_PRIME_LENGTH/4);
	SeedDataPtr.Copy(PassPhrasePtr);
	SeedDataPtr.Append(aSalt);

	TInt err;
	TRAP(err, MD5HashL(SeedDataPtr, aEncryptionKey));
	
	delete SeedDataBfr;
	
	if ( err == KErrNone )
		 return ETrue;		
    else return EFalse;		

}



/**-------------------------------------------------------
 *
 *  CDialogTimeout class
 *  This timeout class used to check user dialog displayed
 *  shall be completed in reasonable time (now 90 seconds).
 *  This class is used the following way:
 *  -- When a CIkev1Dialog class object is constructed one
 *     CDialogTimeout object is constructed as well. These
 *     objects are linked together.
 *  -- If user dialog completes normally (within 90 seconds)
 *     CDialogTimeout is cancelled in CIkev1Dialog.RunL().
 *  -- If timeout expires, CIkev1Dialog is completed via CDialogTimeout.RunL()
 *
 *--------------------------------------------------------*/
CDialogTimeout::CDialogTimeout( MIkeDebug& aDebug )
 : CTimer( EPriorityStandard ),
   iDebug( aDebug )
{
    CActiveScheduler::Add(this);    //Adds itself to the scheduler only the first time
}

CDialogTimeout::~CDialogTimeout()
{
    DEBUG_LOG(_L("CDialogTimeout destructed"));		
    if (IsActive())
        Cancel();
}

void CDialogTimeout::ConstructL(CIkev1Dialog *aDialog)
{
    CTimer::ConstructL();
    iDialog = aDialog;
    After(90*1000000);  //Start dialog timer
}

void CDialogTimeout::DoCancel()
{
	DEBUG_LOG(_L("CDialogTimeout cancelled"));		
    CTimer::DoCancel();
}

void CDialogTimeout::RunL()
{
	DEBUG_LOG(_L("CKmdDialog timeout occurred"));
    TInt delete_dialog    = 1;
    CIkev1Dialog* NextDialog = iDialog->NextDialog();
	MIkeDialogComplete* Callback = iDialog->Callback();
    
    if ( Callback ) 
    {
		TInt err;
		DEBUG_LOG2(_L("Calling DialogCompleteL(), UserInfo = %x, Callback = %x"), (TUint32)iDialog->UserInfo(), (TUint32)Callback);					
        TRAP(err, delete_dialog = Callback->DialogCompleteL(iDialog,
                                                            iDialog->UserInfo(),
                                                            NULL,               //User name
                                                            NULL,               //Password
                                                            NULL));             //domain
		DEBUG_LOG2(_L("DialogCompleteL() completed, err = %d, delete_dialog = %d"), err, delete_dialog);			
		if ( err != KErrNone )
			delete_dialog = 1;
    }  
    if ( delete_dialog )
        {
        delete iDialog;
        iDialog = NULL;
        }

    //
    //  Start a dialog from wait queue if there is some 
    //
    if ( NextDialog )
	{
	   DEBUG_LOG(_L("Next dialog started from dialog timer"));	
       NextDialog->StartDialogL();
	}    
}

