/*
* Copyright (c) 2006, 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Check if file is OK for printing
*
*/



//External includes
#include <badesca.h>
#include <bautils.h>
#include <e32std.h>
#include <f32file.h>
#include <DRMCommon.h>
#include <imageconversion.h>
#include <pathinfo.h> 

//Internal includes
#include "caiwimageprintif.h"
#include "imageprintuidebug.h"


_LIT8( KJpegFileType, "image/jpeg" );
_LIT( KJpegFileType16, "image/jpeg" );


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAiwImagePrintIf::~CAiwImagePrintIf()
    {
    iEikEnv.FsSession().Delete(*iPrintFileName);
    iEikEnv.FsSession().Delete(*iUnsuppFileName);
    iEikEnv.DeleteResourceFile( iResourceOffset );
    delete iDRM;
    delete iPrintFileName;
    delete iUnsuppFileName;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAiwImagePrintIf::CAiwImagePrintIf():iEikEnv( *CEikonEnv::Static() )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAiwImagePrintIf::ConstructL()
    {
    
    TFileName file( KResource );
    file = PathInfo::RomRootPath();
    TBuf<KResource> length2(KResourceFile);    
    file.SetLength(KDriver + length2.Length());
    file.Replace(KDriver, length2.Length(), KResourceFile);
    
    BaflUtils::NearestLanguageFile( iEikEnv.FsSession(), file );
    iResourceOffset = iEikEnv.AddResourceFileL( file );
    iDRM = DRMCommon::NewL();
    User::LeaveIfError( iDRM->Connect() );
    iNumberOfUnSuppFiles = 0;
    
    
    TFileName printNameFile( KResource ); 
    printNameFile = PathInfo::PhoneMemoryRootPath(); 
    TBuf<KResource> length3(KParamFile);
    printNameFile.SetLength(KDriver + length3.Length());
    printNameFile.Replace(KDriver, length3.Length(), KParamFile); 
    
    iPrintFileName = HBufC::NewL(printNameFile.Length() );
    iPrintFileName->Des().Copy(printNameFile);
    
    TFileName unSuppFile( KResource ); 
    unSuppFile = PathInfo::PhoneMemoryRootPath(); 
    TBuf<KResource> lengthUn(KUnSuppFile);
    unSuppFile.SetLength(KDriver + lengthUn.Length());
    unSuppFile.Replace(KDriver, lengthUn.Length(), KUnSuppFile);
    
    iUnsuppFileName = HBufC::NewL(unSuppFile.Length() );
    iUnsuppFileName->Des().Copy(unSuppFile);
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CAiwImagePrintIf::IsPrintingSupported( const CAiwGenericParamList& aInParamList )
    {  
    TInt index(0);
    FLOG(_L("[CAiwImagePrintIf]  IsPrintingSupported "));
    const TAiwGenericParam* param = aInParamList.FindFirst( index, EGenericParamMIMEType );
	TBool printable( EFalse );
	
    if ( index == KErrNotFound )	
        {    
        index = 0;
   	    const TAiwGenericParam* param = aInParamList.FindFirst(index, EGenericParamFile, EVariantTypeDesC);

        //Check if there is any printable images available. At least image must be
        //JPEG and it also must be unprotected to be printable.
        while ( index != KErrNotFound && !printable)
            {
            FLOG(_L("[CAiwImagePrintIf]  IsPrintingSupported 3"));
            TRAP_IGNORE( printable = IsPrintingSupportedL( param->Value().AsDes() ));
   	        param = aInParamList.FindNext(index, EGenericParamFile, EVariantTypeDesC);        
            }
        FTRACE(FPrint(_L("[CAiwImagePrintIf] IsPrintingSupported  printable is  %d"), printable ));     
        return printable;    
        }
	
	while ( index != KErrNotFound && !printable )
		{
	   	if ( param->Value().TypeId() == EVariantTypeDesC &&
			 param->Value().AsDes() == KJpegFileType16 )
	    	{
	    	// MIME-type parameter follows filename parameter in parameter list.
	    	// Because of that previous item in list is used.
	    	if (index > 0)
	    	    {   
	    	    FLOG(_L("[CAiwImagePrintIf]  IsPrintingSupported 5")); 	    
    	      	printable = !IsProtected( aInParamList[index-1].Value().AsDes() );
	    	    }
	    	}

	    if ( !printable )
	        {        
    	   	param = aInParamList.FindNext(index, EGenericParamMIMEType);
	        }
       	}    
	FTRACE(FPrint(_L("[CAiwImagePrintIf] IsPrintingSupported 2 printable is  %d"), printable ));     
    return printable;	 
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CAiwImagePrintIf::IsPrintingSupportedL( const TDesC& aFileName )
    {       
    //1. check: Mimetype should be correct (JPEG)
    FLOG(_L("[CAiwImagePrintIf]  IsPrintingSupported TBool 1"));
    TBool printable = CheckMIMETypeL( KJpegFileType, aFileName ); 
       
    if (printable)
        {
        //2. check: file should not be protected
        printable = !IsProtected( aFileName ); 
        }     
    FTRACE(FPrint(_L("[CAiwImagePrintIf] IsPrintingSupportedL  TBool printable is  %d"), printable ));    
    return printable;                                      
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CAiwImagePrintIf::IsProtected( const TDesC& aFileName )
    {
    TBool fileProtect;
    TInt err = iDRM->IsProtectedFile( aFileName, fileProtect );
    FTRACE(FPrint(_L("[CAiwImagePrintIf] IsProtected   retVal is  %d"), err ? ETrue : fileProtect )); 
    return err ? ETrue : fileProtect;  
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TBool CAiwImagePrintIf::CheckMIMETypeL( const TDesC8& aMimeType,
											   const TDesC& aFileName )
    {
    TBuf8<128> mimeType(0);
    CImageDecoder::GetMimeTypeFileL(iEikEnv.FsSession(), aFileName, mimeType);

    // Check that MIME-type matches (compare returns 0), otherwise return EFalse
    FTRACE(FPrint(_L("[CAiwImagePrintIf] CheckMIMETypeL   retVal is  %d"), mimeType.CompareF( aMimeType ) ? EFalse : ETrue )); 
    return mimeType.CompareF( aMimeType ) ? EFalse : ETrue;
    }

// End of file
