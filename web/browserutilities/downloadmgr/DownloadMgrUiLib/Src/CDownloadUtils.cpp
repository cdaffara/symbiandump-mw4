/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Supports download related utility functions
*
*/



// INCLUDE FILES
#include    "CDownloadUtils.h" 
#include    "UiLibLogger.h"
#include    <DownloadMgrClient.h>
#include    <DocumentHandler.h>
#include    <Oma2Agent.h>
#include    <DRMCommon.h>
#include	<apgcli.h>
#include    <caf/caf.h>
#include    <utf.h>
#include    <Browser_platform_variant.hrh>
// LOCAL CONSTANTS AND MACROS

#ifdef BRDO_WML_DISABLED_FF
_LIT8( KWmlType1, "text/vnd.wap.wml");
_LIT8( KWmlType2, "text/vnd.wap.wmlc");
_LIT8( KWmlType3, "application/vnd.wap.wmlscriptc");
_LIT8( KWmlType4, "application/vnd.wap.wmlc");
#endif 


_LIT8(KAudio, "audio/");
_LIT8(KVideo, "video/");
_LIT8(KImage, "image/");
_LIT8(KFlash, "application/x-shockwave-flash");
_LIT8(Ksdp, "application/sdp");
_LIT8(Krng, "application/vnd.nokia.ringing-tone");
_LIT8(Krn, "application/vnd.rn-realmedia");
_LIT8(Kpn, "application/x-pn-realmedia");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDownloadUtils::CDownloadUtils
// -----------------------------------------------------------------------------
//
CDownloadUtils::CDownloadUtils() 
    {
    }
    
// -----------------------------------------------------------------------------
// CDownloadUtils::ConstructL
// -----------------------------------------------------------------------------
//
void CDownloadUtils::ConstructL()
    {
    CLOG_ENTERFN("CDownloadUtils::ConstructL()");
    }

// -----------------------------------------------------------------------------
// CDownloadUtils::NewL
// -----------------------------------------------------------------------------
//
CDownloadUtils* CDownloadUtils::NewL()
    {
    CDownloadUtils* self = new ( ELeave ) CDownloadUtils();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CDownloadUtils::~CDownloadUtils()
    {
    delete iMimeType;
    CLOG_WRITE(" iMimeType OK");
    delete iContentURI;
    CLOG_WRITE(" iContentURI OK");
    }
    

// -----------------------------------------------------------------------------
// CDownloadUtils::DrmDownloadL
// -----------------------------------------------------------------------------
//
TBool CDownloadUtils::DrmDownloadL( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CDownloadUtils::DrmDownloadL");

    TBool isDrmDownload( EFalse );
    HBufC8* contentType = HBufC8::NewLC( KMaxContentTypeLength );
    TPtr8 temp( contentType->Des() );
    User::LeaveIfError( aDownload.GetStringAttribute( EDlAttrContentType, temp ) );
    CLOG_WRITE(" EDlAttrContentType got");
    if( !contentType->Compare(KOma1DrmMessageContentType) ||
        !contentType->Compare(KOma1DcfContentType) ||
        !contentType->Compare(KOma2DcfContentType) )
        {
        isDrmDownload = ETrue;
        }
    CleanupStack::PopAndDestroy( contentType );
    
    CLOG_WRITE_FORMAT(" ret: %d",isDrmDownload);
    CLOG_LEAVEFN("CDownloadUtils::DrmDownloadL");
    return isDrmDownload;
    }

// -----------------------------------------------------------------------------
// CDownloadUtils::DrmRightsOnThePhoneL
// -----------------------------------------------------------------------------
//
TBool CDownloadUtils::DrmRightsOnThePhoneL
    ( RHttpDownload& aDownload, TBool& aPreviewRights ) const
    {
    CLOG_ENTERFN("CDownloadUtils::DrmRightsOnThePhoneL");

    TBool drmRightsOnThePhone( EFalse );
    
    DRMCommon* drmCommon = DRMCommon::NewL();
    CLOG_WRITE(" DRMCommon::NewL OK");
    CleanupStack::PushL( drmCommon );
    User::LeaveIfError( drmCommon->Connect() );
    CLOG_WRITE(" Connect OK");

    HBufC* fileName = HBufC::NewLC( KMaxPath );
    TPtr fileNamePtr = fileName->Des();
    User::LeaveIfError
        ( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );
    CLOG_WRITE_FORMAT(" EDlAttrDestFilename: %S",&fileNamePtr);

    RPointerArray<CDRMRights>* rightsList(0);
    drmCommon->GetDetailedFileRights( *fileName, rightsList ); // Return val. ignored.
    // We are interested in if there is at least one right:
    if ( rightsList )
        {
        // BEGIN LEAVE safety: No LEAVE allowed!
        TInt rightsCnt = rightsList->Count();
        CLOG_WRITE_FORMAT(" rightsCnt: %d",rightsCnt);
        for ( TInt i=0; i<rightsCnt; ++i )
            {
            // Check: at least one right must be valid
            CDRMRights* r = (*rightsList)[i];
            TUint32 constraintSpec( 0 );
            CDRMRights::TRestriction restriction;
            CDRMRights::TExpiration expiration;
            TUint32 constType;
            TInt infoPri = r->GetRightsInfo( constraintSpec, restriction, expiration, constType );
            CLOG_WRITE_FORMAT(" infoPri: %d",infoPri);
            if ( infoPri != CDRMRights::ENoRights )
                {
                // Right must be valid
                if ( expiration == CDRMRights::EValidRights )
                    {
                    drmRightsOnThePhone = ETrue;
                    // Set output parameter
                    aPreviewRights = ( restriction == CDRMRights::EPreviewRights );
                    // We found the first, we are not interested in others
                    break;
                    }
                }
            }
        // END LEAVE safety: No LEAVE allowed!
            
        rightsList->Close();
        delete rightsList;
        rightsList = 0;
        }
    
    CleanupStack::PopAndDestroy( fileName );
    CleanupStack::PopAndDestroy( drmCommon );
    
    CLOG_WRITE_FORMAT(" ret: %d",drmRightsOnThePhone);
    CLOG_WRITE_FORMAT(" aPreviewRights: %d",aPreviewRights);
    CLOG_LEAVEFN("CDownloadUtils::DrmRightsOnThePhoneL");
    return drmRightsOnThePhone;
    }

// -----------------------------------------------------------------------------
// CDownloadUtils::IsContentTypeSupportedL
// -----------------------------------------------------------------------------
//
TBool CDownloadUtils::IsContentTypeSupportedL( RHttpDownload& aDownload )
    {
    TBool supported = EFalse;
    
    HBufC8* contentType = ContentTypeL( aDownload, ETrue );
    CleanupStack::PushL( contentType );

    // if we get content-type that appears to be SIS (not SISx) type, may be due to
    // misconfigured server: change content-type to octet-stream to allow
    // app installer to make decision if supported or not
    if( contentType->Find( KSisFileMimeType ) != KErrNotFound )
        {
        contentType->Des().Copy( KOctetStreamMimeType );
        }

    supported = IsContentTypeSupportedL( aDownload, *contentType );

    CleanupStack::PopAndDestroy( contentType );

    return supported;
    }

// -----------------------------------------------------------------------------
// CDownloadUtils::IsContentTypeSupportedL
// -----------------------------------------------------------------------------
//
TBool CDownloadUtils::IsContentTypeSupportedL( const TDesC8& aContentType )
    {
    if ( aContentType.Length() == 0 )
        {
        return EFalse;
        }
#ifdef BRDO_WML_DISABLED_FF
    else if ( !aContentType.Compare( KWmlType1() ) || !aContentType.Compare( KWmlType2() )
           || aContentType.Compare( KWmlType3() ) || !aContentType.Compare( KWmlType4() ) )
        {
    	return EFalse; 
        }
#endif 
    else
        {
        TBool canOpen( EFalse );
        CDocumentHandler* docHandler = CDocumentHandler::NewLC();
        TRAPD( err, canOpen = docHandler->CanOpenL( TDataType( aContentType ) ) );
        CleanupStack::PopAndDestroy( docHandler ); // docHandler

        if ( !(aContentType.Compare(KOma2RoContentType)) || !(aContentType.Compare(KOma2ProtectedRoType))
            || !(aContentType.Compare(KOma2TriggerContentType)) )
            {
            canOpen = EFalse;
            }
            
        if ( err != KErrNone ) 
            {
            if ( err == KMimeNotSupported ) 
                {
                canOpen = EFalse; 
                }
            else 
                {
                User::Leave( err ); 
                }
            }
        return canOpen;
        }
    }

// -----------------------------------------------------------------------------
// CDownloadUtils::IsContentTypeSupportedL
// -----------------------------------------------------------------------------
//
TBool CDownloadUtils::IsContentTypeSupportedL( RHttpDownload& aDownload, const TDesC8& aContentType )
    {
    if ( aContentType.Length() == 0 )
        {
        return EFalse;
        }
#ifdef BRDO_WML_DISABLED_FF
    else if ( !aContentType.Compare(KWmlType1()) || !aContentType.Compare(KWmlType2())
           || !aContentType.Compare(KWmlType3()) || !aContentType.Compare(KWmlType4()) )
        {
    	return EFalse; 
        }
#endif         
    else
        {
        TBool canOpen( EFalse );
        CDocumentHandler* docHandler = CDocumentHandler::NewLC();
        TRAPD( err, canOpen = docHandler->CanOpenL( TDataType( aContentType ) ) );
        if ( err == KMimeNotSupported )
            {
			      TUint8* contentTypeString = NULL;
			      FindContentTypeFromFileL(aDownload, contentTypeString);
			      if (contentTypeString != NULL)
				        {
                TRAPD( err1, canOpen = docHandler->CanOpenL( TDataType( TPtrC8(contentTypeString) ) ) );

				        if ( err1 == KMimeNotSupported )
                    {
					          canOpen = EFalse;
                    delete contentTypeString;
					          }
				        else if (err1 == KErrNone)
                    {
                    // Setting Download Content type to a recognized one
                    aDownload.SetStringAttribute( EDlAttrContentType, TPtrC8(contentTypeString) );
                    delete contentTypeString;
					          canOpen = ETrue;
					          }
				       else
					         {
                   delete contentTypeString;
					         User::Leave( err );
					          }
				       }
            }
        else
            {
            if (err != KErrNone )
                {
                User::Leave( err ); 
                }
            }

		    CleanupStack::PopAndDestroy(docHandler ); // docHandler
        return canOpen;
        }
    }

// -----------------------------------------------------------------------------
// CDownloadUtils::ContentTypeL
// -----------------------------------------------------------------------------
//
HBufC8* CDownloadUtils::ContentTypeL
    ( RHttpDownload& aDownload, TBool aDrmResolve, TInt mediaObjectIndex )
    {
    CLOG_ENTERFN("CDownloadUtils::ContentTypeL");
    
    HBufC8* retContentType = NULL;
    TInt32 numMediaObjects = 0;
    User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrNumMediaObjects, numMediaObjects ) );
    
     if ( aDrmResolve && DrmDownloadL( aDownload ) )
        {
        HBufC* fileName = HBufC::NewLC( KMaxPath );
        TPtr fileNamePtr = fileName->Des();
        // Check if this is album
        if ((numMediaObjects > KFirstMoIndex) && mediaObjectIndex)
            {
            User::LeaveIfError
                ( aDownload.GetStringAttribute( EDlAttrDestFilename, mediaObjectIndex, fileNamePtr ) );
            }
           else
            {
            User::LeaveIfError
                ( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );
            }
        CLOG_WRITE_FORMAT(" EDlAttrDestFilename: %S",&fileNamePtr);

		if(fileNamePtr.Compare(KNullDesC))
            {
		    using namespace ContentAccess;

		    CContent* content(NULL);
		    TRAPD( err, content = CContent::NewL(fileNamePtr,EContentShareReadWrite) );
            CleanupStack::PushL(content);

            /*
            If file not present CContent::NewL() leaves with KErrNotFound.
            Need to ignore this error because in case of DRM paused downloads the partial downloaded file will be deleted.
            */

		    if(err != KErrNotFound)
    		    {
                User::LeaveIfError( err );

				HBufC* mimeType = HBufC::NewL(256);
                TPtr ptr = mimeType->Des();
				TInt fileInfoErr = content->GetStringAttribute(EMimeType,ptr);
				TBuf8<256> buf8;
				CnvUtfConverter::ConvertFromUnicodeToUtf8(buf8,ptr);
				iMimeType = buf8.Alloc();
				
				if ( !fileInfoErr )
					{
					retContentType = iMimeType;
					iMimeType = NULL;
					}
                
				// Free temp storage
				delete iMimeType;
				iMimeType = NULL;
				delete iContentURI;
				iContentURI = NULL;
    		    }
            CleanupStack::PopAndDestroy(content);
		    }
		CleanupStack::PopAndDestroy( fileName );    
        }        

    // If it's not DRM or it is, but the content type could not been got
    if ( retContentType == NULL )
        {
        HBufC8* contentType = HBufC8::NewLC( KMaxContentTypeLength );
        TPtr8 temp( contentType->Des() );

        TInt err = 0;
        // Check if this is album
        if ((numMediaObjects > KFirstMoIndex) && mediaObjectIndex)
            {
            err = aDownload.GetStringAttribute( EDlAttrContentType, mediaObjectIndex, temp );
            }
        else
            {
            err = aDownload.GetStringAttribute( EDlAttrContentType, temp );
            }
        CLOG_WRITE_FORMAT(" err: %d",err);
        if ( err != KErrNone && err != KErrNotFound )
            {
            User::LeaveIfError( err );
            }
        if ( err == KErrNotFound )
            {
            contentType->Des().Copy( KNullDesC );
            }

        CleanupStack::Pop( contentType );
        retContentType = contentType;
        contentType = NULL;
        }
    
    CLOG_LEAVEFN("CDownloadUtils::ContentTypeL");
    return retContentType;
    }
    
    
// -----------------------------------------------------------------------------
// CDownloadUtils::FindContentTypeFromFileL
// -----------------------------------------------------------------------------
//
void CDownloadUtils::FindContentTypeFromFileL( RHttpDownload& aDownload, TUint8*& aContentTypeString)
{
    TDataRecognitionResult dataType;
    RApaLsSession apaSession;
    TInt ret;

    User::LeaveIfError(apaSession.Connect());

    // Create a buffer to hold data from the file
	  TInt bufferSize = 0;
    TInt seekPosition = 0;
	  apaSession.GetMaxDataBufSize(bufferSize);
	  HBufC8* buffer = HBufC8::NewLC(bufferSize);
	  TPtr8 buf = buffer->Des();

	  RFile file;
    HBufC* fileName = HBufC::NewLC( KMaxPath );
    TPtr fileNamePtr = fileName->Des();
    User::LeaveIfError
        ( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );

    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    User::LeaveIfError( file.Open( fs, fileNamePtr, 
										EFileShareAny |
										EFileRead ) );


	  // Find current file pointer position
    file.Seek(ESeekStart, seekPosition);
	  // Read from file
	  file.Read(buf);
	  // return file pointer to original position
    file.Seek(ESeekStart, seekPosition);
    // Ask the application architecture to find the file type
    ret = apaSession.RecognizeData(fileNamePtr, buf, dataType);
    apaSession.Close();

	  CleanupStack::PopAndDestroy(3); //fs, fileName, buffer
    
    if (ret == KErrNone &&
        (dataType.iConfidence == CApaDataRecognizerType::ECertain) ||
        (dataType.iConfidence == CApaDataRecognizerType::EProbable))
        {
        // If the file type was found, try to match it to a known file type
        TPtrC8 mimeTypePtr = dataType.iDataType.Des8();
        TInt len = mimeTypePtr.Length() + 1;
        aContentTypeString = new(ELeave) TUint8 [len];

        TPtr8 contentTypeStringPtr(aContentTypeString, len);
        contentTypeStringPtr.Copy(mimeTypePtr);
        contentTypeStringPtr.ZeroTerminate();
        return;
        }
}

// -----------------------------------------------------------------------------
// CDownloadUtils::IsGallerySupported
// -----------------------------------------------------------------------------
//

TBool  CDownloadUtils::IsGallerySupported(const TDesC8& aContentType)
{
	
	TBool found (aContentType.Find(KAudio)==0 || aContentType.Find(KVideo)==0 || aContentType.Find(KImage)==0 || aContentType.Find(KFlash)==0 ||
	            aContentType.Find(Ksdp)==0 || aContentType.Find(Krng)==0 || aContentType.Find(Krn)==0 || aContentType.Find(Kpn)==0);

	return found;
}    
