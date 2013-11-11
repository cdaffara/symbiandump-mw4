/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Launch Image Print App when file is marked or open MG
*
*/



#include "caiwprintingprovider.h"

#include <AiwMenu.h>
#include <AiwCommon.h>
#include <AiwCommon.hrh>
#include <coemain.h>
#include <barsread.h>
#include <imageprintuiprovider.rsg>

#include <e32property.h>
#include <s32strm.h>
#include <s32file.h>
#include <f32file.h>

#include <apacmdln.h>
#include <apgtask.h>
#include <apaid.h>
#include <apgcli.h>
#include <w32std.h>
#include <imageconversion.h>

#include <aknViewAppUi.h>
#include <aknnotedialog.h>
#include <aknstaticnotedialog.h>
#include <eikprogi.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <pathinfo.h> 

#include "caiwimageprintif.h"
#include "imageprintuidebug.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAiwPrintingProvider* CAiwPrintingProvider::NewL()
    {

    CAiwPrintingProvider* self = new( ELeave ) CAiwPrintingProvider;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAiwPrintingProvider::~CAiwPrintingProvider()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAiwPrintingProvider::InitialiseL( MAiwNotifyCallback& aFrameworkCallback,
                                       const RCriteriaArray& /*aInterest*/ )
    {
    iNotifyCallback = &aFrameworkCallback;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAiwPrintingProvider::HandleServiceCmdL( const TInt& aCmdId,
                               const CAiwGenericParamList& aInParamList,
                               CAiwGenericParamList& aOutParamList,
                               TUint aCmdOptions,
                               const MAiwNotifyCallback* aCallback )

    {
    FLOG(_L("[CAiwPrintingProvider]<<<  HandleServiceCmdL BEGIN"));
    DoHandleCmdL( aCmdId, aInParamList, aOutParamList, aCmdOptions, aCallback );
    FLOG(_L("[CAiwPrintingProvider]<<<  HandleServiceCmdL END"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAiwPrintingProvider::InitializeMenuPaneL
        ( CAiwMenuPane& aMenuPane, TInt aIndex,
         TInt /*aCascadeId*/, const CAiwGenericParamList& aInParamList )
    {
    FLOG(_L("[CAiwPrintingProvider]<<<  InitializeMenuPaneL BEGIN"));
    if ( IsPrintingSupported( aInParamList ) )
        {
        TResourceReader reader;
        iEikEnv.CreateResourceReaderLC
                ( reader, R_AIW_PRINTING_PROVIDER_MENU );
        aMenuPane.AddMenuItemsL( reader, KAiwCmdPrint, aIndex );
        CleanupStack::PopAndDestroy(); // aResourceId 
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAiwPrintingProvider::HandleMenuCmdL
        ( TInt aMenuCmdId, const CAiwGenericParamList& aInParamList,
         CAiwGenericParamList& aOutParamList, TUint aCmdOptions,
         const MAiwNotifyCallback* aCallback)
    {
    FLOG(_L("[CAiwPrintingProvider]<<<  HandleMenuCmdL BEGIN"));
    DoHandleCmdL( aMenuCmdId, aInParamList, aOutParamList, aCmdOptions, aCallback );
    FLOG(_L("[CAiwPrintingProvider]<<<  HandleMenuCmdL END"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAiwPrintingProvider::DoHandleCmdL(TInt aMenuCmdId,
                            const CAiwGenericParamList& aInParamList,
                            CAiwGenericParamList& aOutParamList,
                            TUint /*aCmdOptions*/,
                            const MAiwNotifyCallback* aCallback)
    {
    if ( aMenuCmdId == KAiwCmdPrint || aMenuCmdId == KAiwCmdPrintPreview  )
        {
        FLOG(_L("[CAiwPrintingProvider]<<<  DoHandleCmdL"));
        
        CAiwGenericParamList* checkedParams = CAiwGenericParamList::NewL();
        
        iConsumerInParamList = &aInParamList;
        iConsumerOutParamList = &aOutParamList;
        iConsumerCallback = aCallback;
        
        TInt index( 0 );
	    const TAiwGenericParam* param = aInParamList.FindFirst(index,
	            EGenericParamFile,
	            EVariantTypeDesC);
	    while ( index != KErrNotFound )
			{
	        TFileName filename( param->Value().AsDes() );
 	        TInt err = KErrNone;
	        TBool result = EFalse;
	        TRAP( err, result = IsPrintingSupportedL( filename ) );
	        if ( err == KErrNone && result )
	        	{
	            FLOG(_L("[CAiwPrintingProvider] DoHandleCmdL; supported file"));
	        	checkedParams->AppendL(*param);
	        	}
	        else
	        	{
	        	FLOG(_L("[CAiwPrintingProvider] DoHandleCmdL; not supported"));
	        	++iNumberOfUnSuppFiles;
	        	iUnsupportedFiles = ETrue;
	        	}	
	        param = aInParamList.FindNext(index,
	            EGenericParamFile,
	            EVariantTypeDesC);
	        }
   
                
		FTRACE(FPrint(_L("[CAiwPrintingProvider] UnSuppFiles is %d"), iNumberOfUnSuppFiles )); 

		RFileWriteStream stream;
	  	CleanupClosePushL(stream);
		if((stream.Replace(iEikEnv.FsSession(), *iUnsuppFileName ,EFileWrite)) == KErrNone)
			{
			stream.WriteInt16L(iNumberOfUnSuppFiles);
			stream.CommitL();
			}
		CleanupStack::PopAndDestroy(&stream); 
		
        FLOG(_L("[IMAGEPRINTUI]<<< CAiwPrintingProvider;Save iUnsupportedFiles  is done"));
	
        
        RFileWriteStream writeStream;
        User::LeaveIfError( writeStream.Replace(iEikEnv.FsSession(),
        										*iPrintFileName , EFileWrite) );
        writeStream.PushL();
        checkedParams->ExternalizeL(writeStream);
        writeStream.CommitL();
        CleanupStack::PopAndDestroy( &writeStream );
        
        iNumberOfUnSuppFiles = 0;
        delete checkedParams;
        checkedParams = NULL;
        
        LaunchImagePrintApplicationL();
        FLOG(_L("[CAiwPrintingProvider]>>> DoHandleCmdL "));
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAiwPrintingProvider::StartApplicationL()
	{
	FLOG(_L("[CAiwPrintingProvider] StartApplicationL"));
	LaunchImagePrintApplicationL();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//

TBool CAiwPrintingProvider::IsImagePrintUiRunning()
    {
    TFindThread findt(KImagePrintUiSearchPatternBySID);
    TFullName result;
    TBool running(EFalse);
    if (!findt.Next(result))
        {
        FTRACE(FPrint(_L("[CAiwPrintingProvider] Thread '%S'is found"), &result));
        running = ETrue;
        }
     return running;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAiwPrintingProvider::ShowNoteL()
    {
    FLOG(_L("[CAiwPrintingProvider]>>> App launch error "));
    HBufC* str = StringLoader::LoadLC( R_USB_INFO_APP_IN_USE  );
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote;
    note->ExecuteLD( *str );
    CleanupStack::PopAndDestroy( str );  // str	
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAiwPrintingProvider::LaunchImagePrintApplicationL()
    {
    FLOG(_L("[CAiwPrintingProvider]<<< LaunchImagePrintApplicationL"));
    TUid appuid;
    appuid.iUid = KImagePrintUID;
    if (!IsImagePrintUiRunning())
        { 
        TRAPD( err, iService = CAknLaunchAppService::NewL(
        appuid,
        this,
        NULL ));
	   
	    if ( err != KErrNone )
		   { 
		   FTRACE(FPrint(_L("[CAiwPrintingProvider] error when launch is  %d"), err ));
	       FLOG(_L("[CAiwPrintingProvider] LaunchImagePrintApplicationL  error"));
		   }   
        }
    else
        {
     	ShowNoteL(); 
		}
    
    FLOG(_L("[CAiwPrintingProvider]>>> LaunchImagePrintApplicationL"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAiwPrintingProvider::HandleServerAppExit( TInt aReason )
    {
    delete iService;
    iService = NULL;
    FLOG(_L("[CAiwPrintingProvider]<<<  HandleServerAppExit"));

    // In the case that we want to exit also the parent application,
    // ImagePrint is exited with User::Exit() which is seen here as
    // KErrServerTerminated (-15).
    if( iConsumerInParamList && iConsumerOutParamList && iConsumerCallback && iConsumerCallback )
        {
        // Const cast is used to fix weirdness in AIW header files. MWaiNotifyCallback
        // does not define any const function but CAiwServiceIfMenu interface provides
        // const callback object.
        TRAP_IGNORE( const_cast<MAiwNotifyCallback*>(iConsumerCallback)->HandleNotifyL(
            KAiwCmdPrintPreview, KAiwEventCompleted,
            *iConsumerOutParamList, *iConsumerInParamList ));
        iConsumerInParamList = NULL;
        iConsumerOutParamList = NULL;
        iConsumerCallback = NULL;
        }
    if ( aReason == KErrServerTerminated )
        {
        FLOG(_L("[CAiwPrintingProvider]  HandleServerAppExit KErrServerTerminated"));
        }
    }

// End of file

