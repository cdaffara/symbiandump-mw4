/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     This file contains the declaration of the Client interface of Download Mgr Server.
*
*/


// INCLUDE FILES
#include "DownloadMgrLogger.h"
#include "DownloadMgrClient.h"
#include "DownloadMgrServer.h"
#include "DownloadMgrHandler.h"
#include "DownloadMgrStart.h"
#include "DownloadMgrDef.h"
#include "DownloadMgrTransObserver.h"
#include "DownloadMgrCod.h"
#include "DownloadMgrDefAttrib.h"
#include "DownloadDataServ.h"

#include <hash.h>
#include <e32svr.h>
#include <CodDownload.h>
#include <AiwGenericParam.h>
#include <eikenv.h>
#include <httpfiltercommonstringsext.h>
#include <escapeutils.h>

// CONSTANTS
_LIT8( KTransactionCallback, "TransactionCallback" );
const TInt KInitPrevCodEvent = -1;

// GLOBAL FUNCTIONS

// -----------------------------------------------------------------------------
// HashL
// Calculates hash value
// -----------------------------------------------------------------------------
//
void HashL( const TDesC8& aMessage, TDes8& aHash )
	{
	// Calculate the 128 bit (16 byte) hash
	CMD5* md5Calculator = CMD5::NewL();
	CleanupStack::PushL( md5Calculator );
	
	md5Calculator->Reset();
	TPtrC8 hash = md5Calculator->Hash( aMessage );
	// Now print it as a 32 byte hex number
	aHash.Zero();
	_LIT8(formatStr, "%02x");
	for (TInt ii = 0; ii < KRawHashLength; ii++)
		{
		TBuf8<2> scratch;
		scratch.Zero();
		scratch.Format( formatStr, hash[ii] );
		aHash.Append( scratch );
		}
	CleanupStack::PopAndDestroy( md5Calculator );  // MD5Calculator
	}


/**
*
* Extension class.
*
* This class will be used to cache the closed subsession information.
* Required subsession information will be stored in this class before closing the subsession.
*
*/
NONSHARABLE_CLASS( CRHttpDlExtension ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRHttpDlExtension* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CRHttpDlExtension();

    protected: // Constructors

        /**
        * C++ default constructor.
        */
        CRHttpDlExtension();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:

		/**
		* Sets the value for a TBool attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
        * @return None.
		*/        
        void SetBoolAttribute( const TUint aAttribute, TBool aValue );
        
		
		/**
		* Sets the value for a TInt attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
        * @return None.
		*/        
        void SetIntAttribute( const TUint aAttribute, TInt32 aValue );
        
		/**
		* Sets the value for a TDesC8 attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
        * @return None.
		*/        
        void SetStringAttribute( const TUint aAttribute, const TDesC8& aValue );
        
        
        
        /**
		* Sets the value for a TDesC16 attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aMoIndex Identifies the media object index.
		* @param aValue The value to be set.
        * @return None.
		*/        
		
        void SetStringAttribute( const TUint aAttribute,  
                                                 const TInt32& aMoIndex,
                                                 const TDesC16& aValue );
        

		/**
		* Sets the value for a TDesC8 attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
        * @return None.
		*/        
        void SetStringAttribute( const TUint aAttribute, const TDesC16& aValue );
        
        /**
		* Gets the value for a TBool attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested TInt attribute.
        * @return TBool - attribute found.
		*/        
        TInt GetBoolAttribute( const TUint aAttribute, TBool& aValue );

        /**
		* Gets the value for a TBool attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aMoIndex Identifies the media object index.
		* @param aValue On completion, contains the requested TInt attribute.
        * @return TBool - attribute found.
		*/        
        TInt GetBoolAttribute( const TUint aAttribute,
                                               const TInt32& aMoIndex,
                                               TBool& aValue );
        /**
		* Gets the value for a TInt attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested TInt attribute.
        * @return TBool - attribute found.
		*/        
        TInt GetIntAttribute( const TUint aAttribute, TInt32& aValue );
        
        /**
		* Gets the value for a TInt attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aMoIndex Identifies the media object index.
		* @param aValue On completion, contains the requested TInt attribute.
        * @return TBool - attribute found.
		*/
		TInt GetIntAttribute( const TUint aAttribute, 
                                              const TInt32& aMoIndex,
                                              TInt32& aValue );
        
		/**
		* Gets the value for a TDes8 attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested TDes16 attribute.
        * @return TBool - attribute found.
		*/        
        TInt GetStringAttribute( const TUint aAttribute, TDes8& aValue );
        
        /**
		* Gets the value for a TDes8 attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aMoIndex Identifies the media object index.
		* @param aValue On completion, contains the requested TDes16 attribute.
        * @return TBool - attribute found.
		*/        
        TInt GetStringAttribute( const TUint aAttribute,
                                                 const TInt32& aMoIndex,
                                                 TDes8& aValue );
        
		/**
		* Gets the value for a TDes16 attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested TDes16 attribute.
        * @return TBool - attribute found.
		*/         
        TInt GetStringAttribute( const TUint aAttribute, TDes16& aValue );
        
        /**
		* Gets the value for a TDes16 attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aMoIndex Identifies the media object index.
		* @param aValue On completion, contains the requested TDes16 attribute.
        * @return TBool - attribute found.
		*/
        TInt GetStringAttribute( const TUint aAttribute,
                                                 const TInt32& aMoIndex,
                                                 TDes16& aValue );
        
        /**
		* Sets the value for a TInt attribute.
		* @param aCodDlData The Cod downloaded data.
		* @return None.
		*/        
        void SetCodaData( CDownloadDataServ* aCodDlData );
                
		
    private: // Data

        //EDlAttrCodDownload,
        TBool iCodDownload;
        //EDlAttrCodPdAvailable,
        TBool   iCodPdAvailable;
        // EDlAttrNoMedia
        TBool   iNoMedia;
        // EDlAttrHidden
        TBool   iHidden;
        // EDlAttrProgressive
        TBool   iProgressiveDownload;  
        // EDlAttrPausable
        TBool   iPausable;             
        // EDlAttrDestRemovable
        TBool   iRemovableDest;

        //  EDlAttrState THttpDownloadState
         TInt32 iDlState;
        // EDlAttrProgressState
        TInt32  iProgState;

		// EDlAttrContentType
        HBufC8*     iContentType;
        // EDlAttrCurrentUrl
        HBufC8* iCurrentUrl;  
        // EDlAttrReqUrl
        HBufC8*	iReqUrl;
        
		TBuf8<KHashLength> iHashedMsg;

        // EDlAttrId
        TInt32  iId;    
        // EDlAttrDownloadedSize
        TInt32      iDownloadedSize;
        //iDownloadLength
        TInt32	iDownloadLength;

        //EDlAttrName
        HBufC*  iDlName;
        //EDlAttrDestFilename<KMaxPath>
        HBufC* iDestFileName;
        
        //Album Data
        CDownloadDataServ* iCodDlData;

        //Number of media objects
        TInt32 iNumMediaObjects;
        
        //Active Download
        TInt32 iActiveDownload;
        
        //EDlAttrErrorId
        TInt32 iLastError;
        //EDlAttrGlobalErrorId
        TInt32 iGlobalErrorId;        
    };

        
// -----------------------------------------------------------------------------
// CRHttpDlExtension::CRHttpDlExtension
// -----------------------------------------------------------------------------
//
CRHttpDlExtension::CRHttpDlExtension()
    {
    CLOG_CREATE;
    }

// -----------------------------------------------------------------------------
// CRHttpDlExtension::ConstructL
// -----------------------------------------------------------------------------
//
void CRHttpDlExtension::ConstructL()
    {
    CLOG_ENTERFN("CRHttpDlExtension::ConstructL");
    iContentType = HBufC8::NewMaxL( KMaxContentTypeLength );
    iDestFileName = HBufC::NewMaxL( KMaxPath );
    iDlName = HBufC::NewMaxL( KDownloadNameMaxSize );
    iCurrentUrl = HBufC8::NewMaxL( KMaxUrlLength );
    iReqUrl = HBufC8::NewMaxL( KMaxUrlLength );
    iCodDlData = NULL;
    }

// -----------------------------------------------------------------------------
// CRHttpDlExtension::NewL
// -----------------------------------------------------------------------------
//
CRHttpDlExtension* CRHttpDlExtension::NewL()
    {
    CRHttpDlExtension* self = new(ELeave)CRHttpDlExtension();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CRHttpDlExtension::~CRHttpDlExtension
// -----------------------------------------------------------------------------
//
CRHttpDlExtension::~CRHttpDlExtension()
    {
    delete iContentType;
    delete iDestFileName;
    delete iDlName;
    delete iCurrentUrl;
    delete iReqUrl;
    
    if(iCodDlData )
        {
        delete iCodDlData;	
        }
        

    
    CLOG_CLOSE;
    }

// -----------------------------------------------------------------------------
// CRHttpDlExtension::SetBoolAttribute
// -----------------------------------------------------------------------------
//
void CRHttpDlExtension::SetBoolAttribute( const TUint aAttribute, TBool aValue )
    {
    switch( aAttribute )
        {
        case EDlAttrCodDownload:
            {
            iCodDownload = aValue;
            }
            break;
            
        case EDlAttrCodPdAvailable:
            {
            iCodPdAvailable = aValue;
            }
            break;
        case EDlAttrNoMedia:
            {
            iNoMedia = aValue;
            }
            break;
            
        case EDlAttrHidden:
            {
            iHidden = aValue;
            }
            break;
      
       case EDlAttrProgressive:
            {
            iProgressiveDownload = aValue;
            }
            break;
            
        case EDlAttrPausable:
            {
            iPausable = aValue;
            }
            break;
                  
       case EDlAttrDestRemovable:
            {
       	     iRemovableDest = aValue;
            }
            
        default:
            {
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CRHttpDlExtension::SetIntAttribute
// -----------------------------------------------------------------------------
//
void CRHttpDlExtension::SetIntAttribute( const TUint aAttribute, TInt32 aValue )
    {
    switch( aAttribute )
        {
        case EDlAttrDownloadedSize:
        case EDlAttrMultipleMODownloadedSize:
            {
            iDownloadedSize = aValue;
            }
            break;
            
        case EDlAttrLength:
        case EDlAttrMultipleMOLength:
            {
            iDownloadLength = aValue;
            }
            break;
        
        case EDlAttrId:
            {
			iId = aValue;
            }
            
        case EDlAttrState:
            {
            iDlState = (TInt32) aValue;
            }
            break;
        
        case EDlAttrProgressState:
            {
			iProgState = aValue;
            }
            break;
            
		case EDlAttrNumMediaObjects:         
			{
			iNumMediaObjects = aValue;
			}
			break;
			
		case EDlAttrErrorId:
            {
            iLastError = aValue;
            }
            break;
            
        case EDlAttrGlobalErrorId:
            {
            iGlobalErrorId = aValue;
            }
            break;

        case EDlAttrActiveDownload:  
            {
            iActiveDownload = aValue;
            }
            break;
            
        default:
            break;
        }
    }
    
// ---------------------------------------------------------
// CRHttpDlExtension::SetStringAttribute
// ---------------------------------------------------------
//
void CRHttpDlExtension::SetStringAttribute( const TUint aAttribute, const TDesC8& aValue )
    {
    switch( aAttribute )
		{
		case EDlAttrName:
            {
			iDlName->Des().Copy(aValue);
            }
            break;
                     
		case EDlAttrDestFilename:
            {
			iDestFileName->Des().Copy(aValue);
            }
            break;

		case EDlAttrContentType:
            {
            iContentType->Des().Copy(aValue);
            }
            break;

        case EDlAttrCurrentUrl:
            {
			iCurrentUrl->Des().Copy(aValue);
            }
            break;
            
		case EDlAttrReqUrl:
			{
			iReqUrl->Des().Copy(aValue);
			}
			break;
			
		case EDlAttrHashedMsgBody:
			{
			iHashedMsg.Copy(aValue);
			}
			break;			
			
		default:
            break;
        }    
    }

// ---------------------------------------------------------
// CRHttpDlExtension::SetStringAttribute
// ---------------------------------------------------------
//
void CRHttpDlExtension::SetStringAttribute( const TUint aAttribute, const TDesC16& aValue )
    {
    switch( aAttribute )
        {
		case EDlAttrName:
            {
			iDlName->Des().Copy(aValue);
            }
            break;
                     
		case EDlAttrDestFilename:
            {
			iDestFileName->Des().Copy(aValue);
            }
            break;

		case EDlAttrContentType:
            {
            iContentType->Des().Copy(aValue);
            }
            break;

        case EDlAttrCurrentUrl:
            {
			iCurrentUrl->Des().Copy(aValue);
            }
            break;

		case EDlAttrReqUrl:
			{
			iReqUrl->Des().Copy(aValue);
			}
			break;
			
        case EDlAttrAlbumName:
		    {
		    if (iCodDlData)
		        {
		        iCodDlData->SetNameL(aValue);
		        }
		    }
		    break;

        default:
            break;
        }    
    }

    
 

// ---------------------------------------------------------
// CRHttpDlExtension::GetBoolAttribute
// ---------------------------------------------------------
//

void CRHttpDlExtension::SetStringAttribute( const TUint aAttribute,  
                                                 const TInt32& aMoIndex,
                                                 const TDesC16& aValue )
    {

    if (!iCodDlData)
    	{
    	return ;
    	}
    if ((aMoIndex <= KNonMoIndex) || (aMoIndex > iCodDlData->Count()))
    	{
    	return;
    	}
    	
    CMediaDataBase* mediaData = (*iCodDlData)[aMoIndex];
    
     
     switch( aAttribute )
        {
		             
		case EDlAttrDestFilename:
            {
			mediaData->SetDestFilenameL(aValue);
            }
            break;

		
        default:
            break;
        }
     }
       
 
// ---------------------------------------------------------
// CRHttpDlExtension::GetBoolAttribute
// ---------------------------------------------------------
//
TInt CRHttpDlExtension::GetBoolAttribute( const TUint aAttribute, TBool& aValue )
    {
    TInt ret( KErrNone );
    switch( aAttribute )
        {
		case EDlAttrCodDownload:
			{
            aValue = iCodDownload ;
            }
            break;
            
		case EDlAttrCodPdAvailable:
            {
            aValue = iCodPdAvailable ;
            }
            break;
            
		case EDlAttrNoMedia:
            {
			aValue = iNoMedia ;
            }
            break;
            
		case EDlAttrHidden:
			{
            aValue = iHidden ;
            }
            break;
      
		case EDlAttrProgressive:
            {
            aValue = iProgressiveDownload;
            }
            break;
            
		case EDlAttrPausable:
            {
            aValue = iPausable ;
            }
            break;
                  
		case EDlAttrDestRemovable:
            {
       	    aValue = iRemovableDest ;
            }
            break;    
        default:
            {
            ret = KErrNotFound;
            }
            break;
        }
    return ret;  
    }
    


// ---------------------------------------------------------
// CRHttpDlExtension::GetBoolAttribute
// ---------------------------------------------------------
//
TInt CRHttpDlExtension::GetBoolAttribute( const TUint aAttribute,
                                               const TInt32& aMoIndex,
                                               TBool& aValue )
    {
    TInt ret( KErrNone );
    
    if (!iCodDlData)
    	{
    	aValue = 0;
    	return ret;
    	}
    if ((aMoIndex <= KNonMoIndex) || (aMoIndex > iCodDlData->Count()))
    	{
    	return KErrArgument;
    	}
    	
    CMediaDataBase* mediaData = (*iCodDlData)[aMoIndex];
    
    
    switch( aAttribute )
        {
	        case EDlAttrRedirected:
            {
            aValue = mediaData->Redirected();
            }
            break;

        case EDlAttrPausable:
            {
            aValue = mediaData->Pausable();
            }
            break;

        case EDlAttrProgressive:
            {
            aValue = mediaData->ProgressiveDownload();
            }
            break;
   
        case EDlAttrDestRemovable:
            {
            aValue = mediaData->DesRemovable();
            }
            break;
                
        default:
            {
            ret = KErrNotFound;
            }
            break;
        }
    return ret;  
    }
    
// ---------------------------------------------------------
// CRHttpDlExtension::GetIntAttribute
// ---------------------------------------------------------
//
TInt CRHttpDlExtension::GetIntAttribute( const TUint aAttribute, TInt32& aValue )
    {
    TInt ret( KErrNone );
    switch( aAttribute )
        {
        case EDlAttrDownloadedSize:
        case EDlAttrMultipleMODownloadedSize:
            {
              aValue = iDownloadedSize;
            }
            break;
            
        case EDlAttrLength:
        case EDlAttrMultipleMOLength:
            {
              aValue = iDownloadLength;
            }
            break;
        
        case EDlAttrId :
            {
             aValue = iId;
            }
            break;
      
        case EDlAttrState:
            {
             aValue = iDlState;
            }
            break;
        
        case EDlAttrProgressState:
            {
             aValue = iProgState;
            }
      
            break;
            
		case EDlAttrNumMediaObjects:         
			{
			aValue = iNumMediaObjects;
			break;
			}
			
		case EDlAttrErrorId:
            {
            aValue = iLastError;
            }
            break;
            
        case EDlAttrGlobalErrorId:
            {
            aValue = iGlobalErrorId;
            }
            break;
             
        case EDlAttrActiveDownload:  
            {
            aValue = iActiveDownload;
            }
            break;
            
        default:
            {
            ret = KErrNotFound;
            }
            break;
        }
    return ret;  
    }

// ---------------------------------------------------------
// CRHttpDlExtension::GetIntAttribute
// ---------------------------------------------------------
//
TInt CRHttpDlExtension::GetIntAttribute( const TUint aAttribute, 
                                              const TInt32& aMoIndex,
                                              TInt32& aValue  )
    {
    TInt ret( KErrNone );
    
    if (!iCodDlData)
    	{
    	aValue = 0;
    	return ret;
    	}
    if ((aMoIndex <= KNonMoIndex) || (aMoIndex > iCodDlData->Count()))
    	{
    	return KErrArgument;
    	}
    
    CMediaDataBase* mediaData = (*iCodDlData)[aMoIndex];
    
    
    switch( aAttribute )
        {
        
        case EDlAttrDownloadedSize:
            {
              aValue = iDownloadedSize;
            }
            break;
            
        case EDlAttrLength:
            {
              aValue = iDownloadLength;
            }
            break;
            
        case EDlAttrMultipleMOLength:
            {
			aValue = mediaData->Size();
            }
            break;

        case EDlAttrMultipleMODownloadedSize:
            {
            // Calculate downloaded size of current media object from
            // album downloaded size.
            aValue = mediaData->DownloadedSize();
            }
            break;

        case EDlAttrErrorId:
            {
            aValue = mediaData->LastErrorId();
            }
            break;

        case EDlAttrGlobalErrorId:
            {
            aValue = mediaData->GlobalErrorId();
            }
            break;

        case EDlAttrMethod:
            {
            aValue = mediaData->Method();
            }
            break;

        default:
            {
            ret = KErrNotFound;
        
            }
            break;
        }
    
    return ret;  
   
   }


// ---------------------------------------------------------
// CRHttpDlExtension::GetStringAttribute
// ---------------------------------------------------------
//
TInt CRHttpDlExtension::GetStringAttribute( const TUint aAttribute, TDes8& aValue )
    {
    TInt ret( KErrNone );
    switch( aAttribute )
        {
        case EDlAttrContentType:
            {
            aValue.Copy(iContentType->Des());
            }
            break;
         case EDlAttrCurrentUrl:
            {
            aValue.Copy(iCurrentUrl->Des());
            }
            break;
                 
		case EDlAttrName:
			{
			aValue.Copy(iDlName->Des());
			}
			break;
		case EDlAttrDestFilename:
            {
            aValue.Copy(iDestFileName->Des());
            }
            break;

		case EDlAttrReqUrl:
			{
			aValue.Copy(iReqUrl->Des());
			}
          	break;
          	
		case EDlAttrHashedMsgBody:
			{
			aValue.Copy(iHashedMsg);
			}
			break;			
          	
        default:
            {
            ret = KErrNotFound;
            }
            break;
        }
    return ret;  
    }


// ---------------------------------------------------------
// CRHttpDlExtension::SetCodaData
// ---------------------------------------------------------
//
void CRHttpDlExtension::SetCodaData( CDownloadDataServ* aCodDlData )
{
	iCodDlData = aCodDlData;
}
   

        
// ---------------------------------------------------------
// CRHttpDlExtension::GetStringAttribute
// ---------------------------------------------------------
//
TInt CRHttpDlExtension::GetStringAttribute( const TUint aAttribute,
                                                 const TInt32& aMoIndex, TDes8& aValue )
    {

    TInt ret( KErrNone );

    if (!iCodDlData)
    {
    return (KErrNotFound);
    }
    
    if ((aMoIndex <= KNonMoIndex) || (aMoIndex > iCodDlData->Count()))
    	{
    	return KErrArgument;
    	}
    	
    CMediaDataBase* mediaData = (*iCodDlData)[aMoIndex];
    switch( aAttribute )
        {
        case EDlAttrReqUrl:
            {
            aValue.Copy(mediaData->Url());
            }
            break;

        case EDlAttrRedirUlr:
            {
             aValue.Copy(mediaData->RedirUrl());
            }
            break;

        case EDlAttrContentType:
            {
            aValue.Copy(mediaData->Type());
            }
            break;

         default:
            {
            ret = KErrNotFound;
            }
            break;
     
        }
    
 
    return ret; 	
    }

// ---------------------------------------------------------
// CRHttpDlExtension::GetStringAttribute
// ---------------------------------------------------------
//
TInt CRHttpDlExtension::GetStringAttribute( const TUint aAttribute, TDes16& aValue )
    {
	TInt ret( KErrNone );
	switch( aAttribute )
		{
		case EDlAttrContentType:
            {
			aValue.Copy(iContentType->Des());
            }
            break;

        case EDlAttrCurrentUrl:
            {
			aValue.Copy(iCurrentUrl->Des());
            }
            break;

		case EDlAttrName:
			{
			aValue.Copy(iDlName->Des());
			}
			break;
			
		case EDlAttrDestFilename:
            {
            aValue.Copy(iDestFileName->Des());
            }
            break;

		case EDlAttrReqUrl:
			{
			aValue.Copy(iReqUrl->Des());
			}
            break;
            
        case EDlAttrAlbumName:
            {
            if (iCodDlData && (iCodDlData->Name().Compare(KNullDesC)))
                {
                aValue.Copy(iCodDlData->Name());
                }
            else
                {
                aValue.Copy(iDlName->Des());
                }
            }
            break;
            
        default:
            {
            ret = KErrNotFound;
            }
            break;
        }
    return ret;  
    }


// ---------------------------------------------------------
// CRHttpDlExtension::GetStringAttribute
// ---------------------------------------------------------
//    
TInt CRHttpDlExtension::GetStringAttribute( const TUint aAttribute,
                                                 const TInt32& aMoIndex,
                                                 TDes16& aValue )
    {
    TInt ret( KErrNone );

    if (!iCodDlData)
    {
    return (KErrNotFound);
    }
    
    if ((aMoIndex <= KNonMoIndex) || (aMoIndex > iCodDlData->Count()))
    	{
    	return KErrArgument;
    	}
    	
    CMediaDataBase* mediaData = (*iCodDlData)[aMoIndex];
    switch( aAttribute )
        {
        case EDlAttrReqUrl:
            {
            aValue.Copy(mediaData->Url());
            }
            break;

        case EDlAttrRedirUlr:
            {
             aValue.Copy(mediaData->RedirUrl());
            }
            break;

        case EDlAttrContentType:
            {
            aValue.Copy(mediaData->Type());
            }
            break;

        case EDlAttrDestFilename:
            {
            aValue.Copy(mediaData->DestFilename());
            }
            break;

        case EDlAttrName:
            {
            if (aMoIndex == 1)
            	{
            	aValue.Copy(iDlName->Des());
            	}
            else
	            {
	            aValue.Copy(mediaData->Name());
    	        }
            }
            break;
         
         default:
            {
            ret = KErrNotFound;
            }
            break;
    
  	
        }
        return ret;
    }


/**
* Extension class.
*/
NONSHARABLE_CLASS( CRHttpDownloadExtension ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRHttpDownloadExtension* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CRHttpDownloadExtension();

    protected: // Constructors

        /**
        * C++ default constructor.
        */
        CRHttpDownloadExtension();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:

		/**
		* Sets event type.
		* @param aEventType.
        * @return None.
		*/
        void SetEventType( THttpDownloadState aEventType );
        
		/**
		* Allocate buffer.
		* @param aIsBuffered - if it's true all the buffered attribs must be handled.
        * @return HBufC8*.
		*/        
        HBufC8* AllocAttribBufL( TBool aIsBuffered = EFalse );
        
		/**
		* Unpack event attributes.
		* @param aPack Package to be unpacked.
        * @return None.
		*/         
        void UnpackAttribL( const TDesC8& aPack );
       
		/**
		* Sets the value for a TInt attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
        * @return None.
		*/        
        void SetIntAttribute( const TUint aAttribute, TInt32 aValue );
        
		/**
		* Sets the value for a TDesC8 attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
        * @return None.
		*/        
        void SetStringAttribute( const TUint aAttribute, const TDesC8& aValue );
        
		/**
		* Sets the value for a TDesC16 attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
        * @return None.
		*/        
        void SetStringAttribute( const TUint aAttribute, const TDesC16& aValue );

		/**
		* Gets the value for a TInt attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested TInt attribute.
        * @return TBool - attribute found.
		*/        
        TBool GetIntAttribute( const TUint aAttribute, TInt32& aValue );
        
		/**
		* Gets the value for a TDes8 attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested TDes16 attribute.
        * @return TBool - attribute found.
		*/        
        TBool GetStringAttribute( const TUint aAttribute, TDes8& aValue );
        
		/**
		* Gets the value for a TDes16 attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested TDes16 attribute.
        * @return TBool - attribute found.
		*/         
        TBool GetStringAttribute( const TUint aAttribute, TDes16& aValue );
        
		/**
		* Unpack event attributes.
		* @param aPack Package.
        * @return None.
		*/         
        void UnPackEventAttribL( const TDesC8& aPack );    

    public: // Data

        // Event type
        THttpDownloadState  iEventType;
        // EDlAttrId
        TInt32              iId;
        // EDlAttrUserData
        TInt32              iUserData;
        // EDlAttrReqUrl
        HBufC8*             iUrl;
        //EDlAttrName
        HBufC16*            iName;
        // EDlAttrDownloadedSize
        TInt32              iDownloadedSize;
        // EDlAttrLength
        TInt32              iTotalSize;
        // Media Object Index
        TInt32              iMoIndex;
        // Indicates that download should be deleted
        TBool               iToBeDeleted;
        // Indicates that COD is in progress.
        RHttpDownload::TCodProgressState   iCodInProgress;
        // Delete COD download when done.
        TBool               iDeleteCodWhenDone;
        //EDlAttrMultipleMODownloadedSize
        TInt32              iMoDownloadedSize;
        //EDlAttrMultipleMOLength
        TInt32              iMoLength;
        
        CArrayPtrFlat< CDefaultAttrib >* iAttribs;
    };

// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CRHttpDownloadExtension::CRHttpDownloadExtension
// -----------------------------------------------------------------------------
//
CRHttpDownloadExtension::CRHttpDownloadExtension()
:iToBeDeleted( EFalse )
,iCodInProgress( RHttpDownload::ENotActive )
,iDeleteCodWhenDone( EFalse )
    {
    CLOG_CREATE;
    }

// -----------------------------------------------------------------------------
// CRHttpDownloadExtension::ConstructL
// -----------------------------------------------------------------------------
//
void CRHttpDownloadExtension::ConstructL()
    {
    CLOG_ENTERFN("CRHttpDownloadExtension::ConstructL");
    iUrl = HBufC8::NewMaxL( KMaxUrlLength );
    iName = HBufC16::NewL( KDownloadNameMaxSize );
    }

// -----------------------------------------------------------------------------
// CRHttpDownloadExtension::NewL
// -----------------------------------------------------------------------------
//
CRHttpDownloadExtension* CRHttpDownloadExtension::NewL()
    {
    CRHttpDownloadExtension* self = new(ELeave)CRHttpDownloadExtension();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CRHttpDownloadExtension::~CRHttpDownloadExtension()
    {
    delete iUrl;
    delete iName;
    if( iAttribs )
        {
        iAttribs->ResetAndDestroy();
        delete iAttribs;
        iAttribs = NULL;
        }
    CLOG_CLOSE;
    }

// -----------------------------------------------------------------------------
// CRHttpDownloadExtension::SetEventType
// -----------------------------------------------------------------------------
//
void CRHttpDownloadExtension::SetEventType( THttpDownloadState aEventType )
    {
    CLOG_ENTERFN("CRHttpDownloadExtension::SetEventType");
    iEventType = aEventType;
    }
// ---------------------------------------------------------
// CRHttpDownloadExtension::AllocAttribBufL
// ---------------------------------------------------------
//
HBufC8* CRHttpDownloadExtension::AllocAttribBufL( TBool aIsBuffered )
    {
    iAttribs = 
        new (ELeave) CArrayPtrFlat< CDefaultAttrib >( 1 );
    
    CDefaultAttrib* attrib = CDefaultAttrib::NewL( EDlAttrUserData, iUserData );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib ); 
    
    attrib = CDefaultAttrib::NewL( EDlAttrMultipleMODownloadedSize, iMoDownloadedSize );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib ); 
    
    attrib = CDefaultAttrib::NewL( EDlAttrDownloadedSize, iDownloadedSize );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib ); 
    
    attrib = CDefaultAttrib::NewL( EDlAttrMultipleMOLength, iMoLength );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib ); 
    
    attrib = CDefaultAttrib::NewL( EDlAttrLength, iTotalSize );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib ); 
    
    if( aIsBuffered )
        {
        attrib = CDefaultAttrib::NewL( EDlAttrId, iId );
        CleanupStack::PushL( attrib );
        iAttribs->AppendL( attrib );
        CleanupStack::Pop( attrib ); 
        
        attrib = CDefaultAttrib::NewL( EDlAttrReqUrl, iUrl->Des() );
        CleanupStack::PushL( attrib );
        iAttribs->AppendL( attrib );
        CleanupStack::Pop( attrib );              
        }
    attrib = CDefaultAttrib::NewL( EDlAttrActiveDownload, iMoIndex );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib ); 
  
    HBufC8* buf = TDMgrUtils::PackedAttributesL( iAttribs );
    iAttribs->ResetAndDestroy();
    delete iAttribs;
    iAttribs = NULL;
    return buf;
    }

// ---------------------------------------------------------
// CRHttpDownloadExtension::UnPackEventAttribL
// ---------------------------------------------------------
//
void CRHttpDownloadExtension::UnPackEventAttribL( const TDesC8& aPack )
    {
    TDMgrUtils::UnPackEventAttribL( aPack, iUserData, iMoDownloadedSize, iDownloadedSize, 
                                    iMoLength, iTotalSize, iMoIndex 
                                  );
    }

// ---------------------------------------------------------
// CRHttpDownloadExtension::UnpackAttribL
// ---------------------------------------------------------
//
void CRHttpDownloadExtension::UnpackAttribL( const TDesC8& aPack )
    {
    CDefaultAttrib* attrib = CDefaultAttrib::NewL();
    CleanupStack::PushL( attrib );

    TDMgrUtils util;
    util.iPtr.Set( aPack );
    util.iCurrent = 0;
    
    while( util.NextAttribL( attrib, aPack ) )
        {
        switch( attrib->iType )
            {
            case CDefaultAttrib::EString8:
                {
                SetStringAttribute( attrib->iAttribute,
                                    attrib->AsTPtrC8L() );
                }
                break;
            case CDefaultAttrib::EString16:
                {
                /*
                SetStringAttributeL( attrib->iAttribute,
                                     attrib->AsTPtrC16L() );
                                     */
                }
                break;
            case CDefaultAttrib::EInt:
                {
                SetIntAttribute( attrib->iAttribute,
                                 attrib->AsTInt32L() );
                }
                break;
            case CDefaultAttrib::EBool:
                {
                /*
                SetBoolAttributeL( attrib->iAttribute,
                                   attrib->AsTBoolL() );*/
                }
                break;
            default:
                break;
            }
        }

    CleanupStack::PopAndDestroy( attrib ); // attrib;    
    }

// ---------------------------------------------------------
// CRHttpDownloadExtension::SetIntAttribute
// ---------------------------------------------------------
//
void CRHttpDownloadExtension::SetIntAttribute( const TUint aAttribute, TInt32 aValue )
    {
    switch( aAttribute )
        {
        case EDlAttrUserData:
            {
            iUserData = aValue;
            }
            break;
        case EDlAttrDownloadedSize:
            {
            iDownloadedSize = aValue;
            }
            break;
        case EDlAttrLength:
            {
            iTotalSize = aValue;
            }
            break;
        case EDlAttrId:
            {
            iId = aValue;
            }
            break;
            
        case EDlAttrMultipleMODownloadedSize:
            {
            iMoDownloadedSize = aValue;
            }
            break;
            
        case EDlAttrMultipleMOLength:
            {
            iMoLength = aValue;
            } 
            break;
            
        case EDlAttrActiveDownload:
            {
            iMoIndex = aValue;
            }
            break;
            
        default:
            {
            }
            break;
        }
    }
// ---------------------------------------------------------
// CRHttpDownloadExtension::SetStringAttribute
// ---------------------------------------------------------
//
void CRHttpDownloadExtension::SetStringAttribute( const TUint aAttribute, const TDesC8& aValue )
    {
    switch( aAttribute )
        {
        case EDlAttrReqUrl:
            {
            iUrl->Des().Copy( aValue );
            }
            break;
        default:
            {
            }
            break;
        }    
    }

// ---------------------------------------------------------
// CRHttpDownloadExtension::SetStringAttribute
// ---------------------------------------------------------
//
void CRHttpDownloadExtension::SetStringAttribute( const TUint aAttribute, const TDesC16& aValue )
    {
    switch( aAttribute )
        {
        case EDlAttrName:
            {
            iName->Des().Copy( aValue );
            }
            break;
        default:
            {            
            }
            break;
        }
    }

// ---------------------------------------------------------
// CRHttpDownloadExtension::GetIntAttribute
// ---------------------------------------------------------
//
TBool CRHttpDownloadExtension::GetIntAttribute( const TUint aAttribute, TInt32& aValue )
    {
    TBool ret( ETrue );
    switch( aAttribute )
        {
        case EDlAttrUserData:
            {
            aValue = iUserData;
            }
            break;
        case EDlAttrDownloadedSize:
            {
            aValue = iDownloadedSize;
            }
            break;
        case EDlAttrLength:
            {
            aValue = iTotalSize;
            }
            break;
        case EDlAttrId:
            {
            aValue = iId;
            }
            break;
            
        case EDlAttrMultipleMODownloadedSize:
            {
             aValue = iMoDownloadedSize;
            }
            break;
            
        case EDlAttrMultipleMOLength:
            {
            aValue = iMoLength;
            } 
            break;
            
        case EDlAttrActiveDownload:
        	{
        	aValue = iMoIndex;
        	}
        	break;
                                              
        default:
            {
            ret = EFalse;
            }
            break;
        }
    return ret;  
    }

// ---------------------------------------------------------
// CRHttpDownloadExtension::GetIntAttribute
// ---------------------------------------------------------
//
TBool CRHttpDownloadExtension::GetStringAttribute( const TUint aAttribute, TDes8& aValue )
    {
    TBool ret( ETrue );
    switch( aAttribute )
        {
        case EDlAttrReqUrl:
            {
            aValue.Copy( iUrl->Des() );
            }
            break;
        default:
            {
            ret = EFalse;
            }
            break;
        }
    return ret;  
    }


// ---------------------------------------------------------
// CRHttpDownloadExtension::GetIntAttribute
// ---------------------------------------------------------
//
TBool CRHttpDownloadExtension::GetStringAttribute( const TUint aAttribute, TDes16& aValue )
    {
    TBool ret( ETrue );
    switch( aAttribute )
        {
        case EDlAttrReqUrl:
            {
            aValue.Copy( iUrl->Des() );
            }
            break;
        case EDlAttrName:
            {
            aValue.Copy( iName->Des() );
            if(!aValue.Length())
                return EFalse;
            }
            break;

        default:
            {
            ret = EFalse;
            }
            break;
        }
    return ret;  
    }
//==========================================================================================

#ifdef DEAD_CODE
// ---------------------------------------------------------
// RHttpDownload::AllocEventAttribBufL
// ---------------------------------------------------------
//
HBufC8* RHttpDownload::AllocEventAttribBufL()
    {
    return iExtension->AllocAttribBufL();
    }
#endif

// ---------------------------------------------------------
// RHttpDownload::UnpackEventAttribL
// ---------------------------------------------------------
//
void RHttpDownload::UnpackEventAttribL( const TDesC8& aPack )
    {
    return iExtension->UnPackEventAttribL( aPack );
    }
    
// ---------------------------------------------------------
// RHttpDownload::CreateL
// ---------------------------------------------------------
//
void RHttpDownload::CreateL( const TDesC8& aUrl, const TPtr8 aPackedAttribs  )
    {
    CLOG_ENTERFN( "RHttpDownload::CreateL" )

    iExtension = CRHttpDownloadExtension::NewL();
    iDlExtension = CRHttpDlExtension::NewL();
    
    CLOG_ATTACH( iExtension, this );
    
    iDownloadHandler = new (ELeave) CDownloadMgrHandler ( iDownloadMgr, this );
    CLOG_ATTACH( iDownloadHandler, this );
    // 0st param - the URL needs to be downloaded
    // 1st param - will contain the unique handle
    // 2nd param - packed attributes 
    TPckg<TInt> handlePckg( iHandle );

	User::LeaveIfError( CreateSubSession( *iDownloadMgr, 
                                          EHttpDownloadMgrCreateDownload, 
                                          TIpcArgs( &aUrl, &handlePckg, &aPackedAttribs ) ) );

    iClosed = EFalse;
    
    iDownloadHandler->StartL();
    BufferAttributesL();
    
    CLOG_NAME_2( _L("Download_%d_%d"), 
                 iDownloadMgr->SessionId(),
                 iExtension->iId );
	}

// ---------------------------------------------------------
// RHttpDownload::AttachL
// ---------------------------------------------------------
//
void RHttpDownload::AttachL( TInt aHandle )
    {
    CLOG_ENTERFN( "RHttpDownload::AttachL" )
    
    iExtension = CRHttpDownloadExtension::NewL();
	iDlExtension = CRHttpDlExtension::NewL();
    iDownloadHandler = new (ELeave) CDownloadMgrHandler ( iDownloadMgr, this );
    // 0st param - application UID
    // 1st param - a handle idetifies a download subsession in server
    TPckg<TUid> uidPckg( iDownloadMgr->iAppUid );

    User::LeaveIfError( CreateSubSession( *iDownloadMgr, 
                                          EHttpDownloadAttach, 
                                          TIpcArgs( &uidPckg, aHandle ) ) );

    iHandle = aHandle;
    iClosed = EFalse;    
    iDownloadHandler->StartL();

    BufferAttributesL();

    CLOG_NAME_2( _L("Download_%d_%d"), 
                 iDownloadMgr->SessionId(),
                 iExtension->iId );
	}

// ---------------------------------------------------------
// RHttpDownload::AttachToDownloadL
// ---------------------------------------------------------
//
void RHttpDownload::AttachToDownloadL( const TInt32 aDlId )
    {
    CLOG_ENTERFN( "RHttpDownload::AttachToDownloadL" )
    
    iExtension = CRHttpDownloadExtension::NewL();
	iDlExtension = CRHttpDlExtension::NewL();
    iDownloadHandler = new (ELeave) CDownloadMgrHandler ( iDownloadMgr, this );
    // 0st param - application UID
    // 1st param - a handle idetifies a download subsession in server
    TPckg<TInt> handlePckg( iHandle );
    User::LeaveIfError( CreateSubSession( *iDownloadMgr, 
                                          EHttpDownloadAttachToDownload, 
                                          TIpcArgs( aDlId, &handlePckg ) ) );
    iClosed = EFalse;                                          
    iDownloadHandler->StartL();
	}

// ---------------------------------------------------------
// RHttpDownload::Close
// ---------------------------------------------------------
//
void RHttpDownload::Close()
	{
    CLOG_WRITE_FORMAT( "RHttpDownload::Close - iDownloadHandler %d", iDownloadHandler );
    
	if(iWait)
		{
		delete iWait;
		iWait = NULL;
		}
    
    if ( iDownloadHandler )
        {
        CLOG_WRITE( "Cancel handler" );
        iDownloadHandler->Cancel();
        CLOG_WRITE( "handler cancelled" );
        delete iDownloadHandler;
        iDownloadHandler = NULL;
        }

    if( iExtension )
        {
        delete iExtension;
        iExtension = NULL;
        }

    if( iDlExtension )
        {
        delete iDlExtension;
        iDlExtension = NULL;
        }

    DeleteTransaction();
    iClosed = ETrue;

    if( iCodDownload )
    	{
    	SDMgrCodUserData* ptr = (SDMgrCodUserData*)iCodDownload->UserData();
    	if( ptr )
    	    {
    	    delete ptr;
    	    }
    	delete iCodDownload;
    	iCodDownload = NULL;
    	iDownloadMgr->DecrementEventPriorityFlag();
    	}

    if( iCodObserver )
    	{
    	delete iCodObserver;
    	iCodObserver = NULL;
    	}    	

    CLOG_WRITE( "Close subsession" );
	RSubSessionBase::CloseSubSession( EHttpDownloadClose );
	
	CLOG_CLOSE;
	}

// ---------------------------------------------------------
// RHttpDownload::CloseSubSession
// ---------------------------------------------------------
//
void RHttpDownload::CloseSubSession()
    {
   	CLOG_WRITE_FORMAT( "RHttpDownload::CloseSubSession - iDownloadHandler %d", iDownloadHandler );
   	
   	if(iClosedSubSession)
   		{
   		return;	
   		}
   		
	TInt32 downloadState(0);
	GetIntAttribute(EDlAttrState, downloadState);	
    if( EHttpDlCompleted != downloadState )
    	{
    	return;
    	}

    if ( iDownloadHandler )
        {
        CLOG_WRITE( "Cancel handler" );
        iDownloadHandler->Cancel();
        CLOG_WRITE( "handler cancelled" );
        delete iDownloadHandler;
        iDownloadHandler = NULL;
        }

    DeleteTransaction();

    if( iCodDownload )
    	{
    	SDMgrCodUserData* ptr = (SDMgrCodUserData*)iCodDownload->UserData();
    	if( ptr )
    	    {
    	    delete ptr;
    	    }
    	delete iCodDownload;
    	iCodDownload = NULL;
    	iDownloadMgr->DecrementEventPriorityFlag();
    	}

    if( iCodObserver )
    	{
    	delete iCodObserver;
    	iCodObserver = NULL;
    	}    	

    CLOG_WRITE( "Close subsession" );
	RSubSessionBase::CloseSubSession( EHttpDownloadCloseCompleted );
	iClosedSubSession = ETrue;
    }

// ---------------------------------------------------------
// RHttpDownload::SaveDownloadInfoL
// ---------------------------------------------------------
//
void RHttpDownload::SaveDownloadInfoL()
    {
    
    CLOG_WRITE_FORMAT( "-->RHttpDownload::SaveDownloadInfoL 0x%x", this  );
   	
   	__ASSERT_DEBUG( iDlExtension, DMPanic( KErrGeneral ) );
   	
   	
    TBool bCodDownLoad;
    GetBoolAttribute(EDlAttrCodDownload, bCodDownLoad);
    iDlExtension->SetBoolAttribute( EDlAttrCodDownload, bCodDownLoad );
    
    TInt32 downloadedSize(0);
    GetIntAttribute(EDlAttrMultipleMODownloadedSize, downloadedSize);
    iDlExtension->SetIntAttribute( EDlAttrMultipleMODownloadedSize, downloadedSize );
    
    TInt32 downloadLen;
	GetIntAttribute(EDlAttrMultipleMOLength, downloadLen);
    iDlExtension->SetIntAttribute( EDlAttrMultipleMOLength, downloadLen );
    
    TInt32 id;
	GetIntAttribute(EDlAttrId, id);
    iDlExtension->SetIntAttribute( EDlAttrId, id );
    
    
    TInt32 state;
	GetIntAttribute(EDlAttrState, state);
    iDlExtension->SetIntAttribute( EDlAttrState, state );
    
    
    TInt32 progressState;
	GetIntAttribute(EDlAttrProgressState, progressState);
    iDlExtension->SetIntAttribute( EDlAttrProgressState, progressState );
    
    
   	if(bCodDownLoad && iCodDownload )
		{
   		HBufC8* mediaInfo8 = NULL;
	    TRAPD( err, mediaInfo8 = iCodDownload->UpdatedDownloadDataL() );
	    if ((err == KErrNone) && mediaInfo8)
	        {
	        CDownloadDataServ *CodDlData = CDownloadDataServ::NewL(*mediaInfo8);
	        iDlExtension->SetCodaData(CodDlData);
			delete mediaInfo8;
			TInt count = CodDlData->Count();
	
			
	        iDlExtension->SetIntAttribute( EDlAttrNumMediaObjects, count );
	        
			for(TInt index = 1; index <= count; index++)
				{
				  HBufC* destFileName = HBufC::NewL( KMaxPath );
                  TPtr pFileName( destFileName->Des() );
                  
                  err = SendReceive( EHttpDownloadGetStringAttribute, 
                        TIpcArgs( EDlAttrDestFilename,index, &pFileName  ));
                  
                  iDlExtension->SetStringAttribute( EDlAttrDestFilename, index, pFileName );
	              delete destFileName;
    
				}
	        
	        }
		}
		
	
	
	
	
    TBool bCodPdAvailable;
    GetBoolAttribute(EDlAttrCodPdAvailable, bCodPdAvailable);
    iDlExtension->SetBoolAttribute( EDlAttrCodPdAvailable, bCodPdAvailable );
    
    TBool bNoMedia;
    GetBoolAttribute(EDlAttrNoMedia, bNoMedia);
    iDlExtension->SetBoolAttribute( EDlAttrNoMedia, bNoMedia );
    
    TBool bHidden;
    GetBoolAttribute(EDlAttrHidden, bHidden);
    iDlExtension->SetBoolAttribute( EDlAttrHidden, bHidden );
    
    TBool bProgressive;
    GetBoolAttribute(EDlAttrProgressive, bProgressive);
    iDlExtension->SetBoolAttribute( EDlAttrProgressive, bProgressive );
    
    
    TBool bPausable;
    GetBoolAttribute(EDlAttrPausable, bPausable);
    iDlExtension->SetBoolAttribute( EDlAttrPausable, bPausable );
    
    TBool bRemovableDest;
    GetBoolAttribute(EDlAttrDestRemovable, bRemovableDest);
    iDlExtension->SetBoolAttribute( EDlAttrDestRemovable, bRemovableDest );
    
    //    
        
    iDlExtension->SetIntAttribute(EDlAttrActiveDownload, KNonMoIndex);

    HBufC8* contentType = HBufC8::NewL( KMaxContentTypeLength );
    TPtr8 pContentType( contentType->Des() );
    TInt err = SendReceive( EHttpDownloadGetString8Attribute, 
                        TIpcArgs( EDlAttrContentType,KNonMoIndex, &pContentType  ));
    iDlExtension->SetStringAttribute( EDlAttrContentType, pContentType );
	delete contentType;
    
    HBufC* attrName = HBufC::NewL( KDownloadNameMaxSize );
    TPtr pName( attrName->Des() );
	err = SendReceive( EHttpDownloadGetStringAttribute, 
                        TIpcArgs( EDlAttrName,KNonMoIndex, &pName  ));
    iDlExtension->SetStringAttribute( EDlAttrName, pName );
	delete attrName;

    HBufC* destFileName = HBufC::NewL( KMaxPath );
    TPtr pFileName( destFileName->Des() );
     err = SendReceive( EHttpDownloadGetStringAttribute, 
                        TIpcArgs( EDlAttrDestFilename,KNonMoIndex, &pFileName  ));
    iDlExtension->SetStringAttribute( EDlAttrDestFilename, pFileName );
	delete destFileName;
    //
    
    HBufC8* currentURL = HBufC8::NewL( KMaxUrlLength );
    TPtr8 pCurrentURL( currentURL->Des() );
    err = SendReceive( EHttpDownloadGetString8Attribute, 
                        TIpcArgs( EDlAttrCurrentUrl,KNonMoIndex, &pCurrentURL  ));
    iDlExtension->SetStringAttribute( EDlAttrCurrentUrl, pCurrentURL );
	delete currentURL;

    HBufC8* reqURL = HBufC8::NewL( KMaxUrlLength );
    TPtr8 pReqURL(reqURL->Des());
    err = SendReceive( EHttpDownloadGetString8Attribute, 
                        TIpcArgs( EDlAttrReqUrl,KNonMoIndex, &pReqURL  ));
    iDlExtension->SetStringAttribute( EDlAttrReqUrl, pReqURL );
	delete reqURL;
	
    TBuf8<KHashLength> hash_owned;
	GetStringAttribute( EDlAttrHashedMsgBody, hash_owned );
	iDlExtension->SetStringAttribute( EDlAttrHashedMsgBody, hash_owned );
    
    GetIntAttribute(EDlAttrDownloadedSize, downloadedSize);
    iDlExtension->SetIntAttribute( EDlAttrDownloadedSize, downloadedSize );
    
	GetIntAttribute(EDlAttrLength, downloadLen);
    iDlExtension->SetIntAttribute( EDlAttrLength, downloadLen );
    
    TInt32 errorId;
	GetIntAttribute( EDlAttrErrorId, errorId);
    iDlExtension->SetIntAttribute( EDlAttrErrorId, errorId );
    
    TInt32 globalErrorId; 
	GetIntAttribute( EDlAttrGlobalErrorId, globalErrorId );
    iDlExtension->SetIntAttribute( EDlAttrGlobalErrorId, globalErrorId );


    //TInt32 activeIndex; 
	//GetIntAttribute( EDlAttrNumMediaObjects, activeIndex );
    //iDlExtension->SetIntAttribute(EDlAttrActiveDownload, activeIndex);
    //iDlExtension->SetIntAttribute(EDlAttrActiveDownload, KFirstMoIndex);
    //CLOG_WRITE_FORMAT( "<--RHttpDownload::SaveDownloadInfoL 0x%x", this );
    
    HBufC* albumName = HBufC::NewL( KDownloadNameMaxSize );
    TPtr pAlbumName( albumName->Des() );
	err = SendReceive( EHttpDownloadGetStringAttribute, 
                        TIpcArgs( EDlAttrAlbumName, KNonMoIndex, &pAlbumName  ));
    iDlExtension->SetStringAttribute( EDlAttrAlbumName, pAlbumName );
	delete albumName;
	
    CLOG_WRITE_FORMAT( "<--RHttpDownload::SaveDownloadInfoL 0x%x", this );	
    }

// ---------------------------------------------------------
// RHttpDownload::RHttpDownload
// ---------------------------------------------------------
//
RHttpDownload::RHttpDownload( RHttpDownloadMgr* aServer )
    {
    CLOG_CREATE;
    
    // Reset data members as R-objects are not zero'd.
    iDownloadHandler = NULL;
    iExtension = NULL;
    iDownloadMgr = aServer;
    iHandle = 0;
    iTransactionObserver = NULL;
    iHttpTransaction = NULL;
    iCodDownload = NULL;
    iCodObserver = NULL;
    iClientSideDownload = EFalse;
    iClosedSubSession = EFalse;
    iDlExtension = NULL;
   	iWait = NULL;
   	iASReady = EFalse;
    }

// ---------------------------------------------------------
// RHttpDownload::Start
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::Start()
    {
    CLOG_ENTERFN( "RHttpDownload::Start" )

    TRAPD( err, DoStartL() );
    
    if( err )
        {
        DeleteTransaction();
        }

    return err;
    }
    
// ---------------------------------------------------------
// RHttpDownload::DoStartL
// ---------------------------------------------------------
//
void RHttpDownload::DoStartL()
    {
    CLOG_ENTERFN( "RHttpDownload::DoStartL" )
    if( iCodDownload )
    	  {    
        TBool isCod( EFalse );    
        GetBoolAttribute( EDlAttrCodDownload, isCod );
        SDMgrCodUserData* ptr =(SDMgrCodUserData*)( iCodDownload->UserData() );

        if( isCod )
            {
            // processing of start command depends the state of COD Handler
            if( KInitPrevCodEvent == ptr->iPrevCodEvent )
                {
                DoStartCodL();
                }            
            else if( MCodDownloadObserver::EEndLoad == ptr->iPrevCodEvent )
                {
    			      const TCodDownloadProgress& progress = iCodDownload->Progress();
                if(progress.iState == TCodDownloadProgress::EPaused)
                    {
                    }
                else
                    {
        	          iCodDownload->ServiceFlowL();
            	      return;
                    }
                }
            else
                {
    			      const TCodDownloadProgress& progress = iCodDownload->Progress();
                if(progress.iState == TCodDownloadProgress::EPaused)
                    {
                    }                
					      else
					          {
		                // Already beeing downloaded
		                THttpDownloadEvent event;
		                event.iDownloadState = EHttpDlAlreadyRunning;
		                event.iProgressState = EHttpProgNone;
		                iDownloadMgr->BroadcastEvent( *this, event );
		                return;
					          }
                }
            }
    	  }
    // start active object to get events
    CLOG_WRITE_FORMAT( "Start handler of 0x%x", this );
    iDownloadHandler->StartL();
    User::LeaveIfError( SendReceive( EHttpDownloadStart, TIpcArgs() ) );
    }

// ---------------------------------------------------------
// RHttpDownload::DoStartCodL
// ---------------------------------------------------------
//
void RHttpDownload::DoStartCodL()
	{
	CLOG_ENTERFN( "RHttpDownload::DoStartCodL" )
	
	if( TCodDownloadProgress::EInProgress == iCodDownload->Progress().iState )
		{
		return;
		}
	
	CAiwGenericParamList* params = CAiwGenericParamList::NewLC();
	HBufC8* buf = NULL;
	
	TInt32 intValue;
	TInt err = iDownloadMgr->GetIntAttribute( EDlMgrIap, intValue );
	if( !err )
		{
	    TAiwGenericParam iap(  EGenericParamAccessPoint, intValue );
	    params->AppendL( iap );		
		}
    CLOG_WRITE_FORMAT( "EGenericParamAccessPoint %d", intValue );
    
    buf = HBufC8::NewLC( KMaxUrlLength );
    TPtr8 urlBuf = buf->Des();
    err = GetStringAttribute( EDlAttrReqUrl, urlBuf );
    if( !err )
    	{
		TAiwGenericParam url(  EGenericParamURL, urlBuf );
		params->AppendL( url );   	
    	}
    CleanupStack::PopAndDestroy( buf ); // buf

    buf = HBufC8::NewLC( KMaxGeneralHeaderFieldLength );    
    TPtr8 charsetBuf = buf->Des();
    err = GetStringAttribute( EDlAttrResponseCharSet, charsetBuf );
    if( !err )
    	{
	    TAiwGenericParam charset(  EGenericParamCharSet, charsetBuf );
	    params->AppendL( charset );               
    	}
    CleanupStack::PopAndDestroy( buf ); // buf
     
    if( NULL != iDownloadMgr->NextUriObserver() )
        {
        // Set CODHandler not to go to the next uri page
        TBool suppress( ETrue );
        TAiwGenericParam nextUrl(  EGenericParamSuppressNextUrl, suppress );
        params->AppendL( nextUrl );        
        }
    
	TBool silent( EFalse );
    /*err = */GetBoolAttribute( EDlAttrSilent, silent );

    TInt32 fota( KDefaultFotaPckgId );
    /*err = */GetIntAttribute( EDlAttrFotaPckgId, fota ); 

	if( KDefaultFotaPckgId < fota )
	    {
        TAiwGenericParam fotaID(  EGenericParamFotaPkgId, fota );
        params->AppendL( fotaID );
	    }
	    
    TAiwGenericParam silentMode(  EGenericParamSilentDownload, silent );
    params->AppendL( silentMode );     
	
	TInt32 action( ELaunch );
	/*err = */GetIntAttribute( EDlAttrAction, action );
	if( EDoNothing == action )
	    {
        TAiwGenericParam actionOnExit( EGenericParamSuppressLaunch, ETrue );
        params->AppendL( actionOnExit );	    
	    }

	TInt32 autoAccept( 0 );
	err = iDownloadMgr->GetIntAttribute( EDlMgrAutoAcceptCod, autoAccept );
    if( err == KErrNoMemory )
        {
        User::Leave( KErrNoMemory );
        }
    else if( err == KErrNone )
	    {
        TAiwGenericParam autoAcceptParam( EGenericParamSuppressDownloadConfirmation, autoAccept );
        params->AppendL( autoAcceptParam );	    
	    }
	    
	// Append DownloadId
    TInt32 download; 
    err = GetIntAttribute(EDlAttrId , download);
    if(!err)
        {
        TAiwGenericParam downloadId( EGenericParamDownloadId, download );
        params->AppendL( downloadId );	          
        }
        

    TAiwGenericParam appUid( EGenericParamApplication, iDownloadMgr->iAppUid.iUid );
    params->AppendL(appUid);  
  
	iCodDownload->StartL( params );
	
	CleanupStack::PopAndDestroy( params ); // params
	}

// ---------------------------------------------------------
// RHttpDownload::StartEventNotification
// ---------------------------------------------------------
//
void RHttpDownload::StartEventNotification( TDes8& aDownloadState,
                                            TDes8& aProgressState,
                                            TDes8& aEventAttrib,                                            
                                            TRequestStatus& aStatus )
    {
    if(iClosedSubSession)
    	{
    	return;
    	}
    CLOG_ENTERFN( "RHttpDownload::StartEventNotification" )

    CLOG_WRITE_FORMAT( "iDownloadHandler %d", iDownloadHandler );

    SendReceive( EHttpDownloadEventSubscription, 
                 TIpcArgs( &aDownloadState, &aProgressState, &aEventAttrib ), 
                 aStatus );
    }

// ---------------------------------------------------------
// RHttpDownload::CancelEventNotification
// ---------------------------------------------------------
//
void RHttpDownload::CancelEventNotification()
    {
    if(iClosedSubSession)
    	{
    	return;
    	}
    CLOG_WRITE( "RHttpDownload::CancelEventNotificationL" )

    SendReceive( EHttpDownloadEventCancel, TIpcArgs() );
    }

// ---------------------------------------------------------
// RHttpDownload::PauseL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::Pause()
    {
    if(iClosedSubSession)
    	{
    	return KErrNotSupported;
    	}
    CLOG_WRITE( "RHttpDownload::Pause" )

    DeleteTransaction();

    return SendReceive( EHttpDownloadPause, TIpcArgs() );
    }

// ---------------------------------------------------------
// RHttpDownload::Reset
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::Reset()
    {
    if(iClosedSubSession)
    	{
    	return KErrNotSupported;
    	}
    CLOG_WRITE( "RHttpDownload::Reset" )

    DeleteTransaction();

    return SendReceive( EHttpDownloadReset, TIpcArgs() );
    }

// ---------------------------------------------------------
// RHttpDownload::IsDeleteCodWhenDone
// ---------------------------------------------------------
//
TBool RHttpDownload::IsDeleteCodWhenDone() const
    {
    CLOG_WRITE( "RHttpDownload::IsDeleteCodWhenDone" )
    
    return iExtension->iDeleteCodWhenDone;
    }
    
// ---------------------------------------------------------
// RHttpDownload::SetCodInProgress
// ---------------------------------------------------------
//
void RHttpDownload::SetCodInProgress( TCodProgressState aVal )
    {
    CLOG_WRITE( "RHttpDownload::SetCodInProgress" )
    
    iExtension->iCodInProgress = aVal;
    }

// ---------------------------------------------------------
// RHttpDownload::DeleteL
// ---------------------------------------------------------
//

EXPORT_C TInt RHttpDownload::Delete()
    {
    CLOG_WRITE( "RHttpDownload::Delete" )
    TInt err( KErrNone );
    
    
    if( iCodDownload && iExtension->iCodInProgress == RHttpDownload::EInProgress )
    	{
		
		//The download is still is not accepted by the user. Allow the the user to cancel the
		// download from Accpet-Cancel dialog or if any dialog displayed before it
		if(!iASReady)
		{
		return KErrInUse; 		
		}
			
    	// COD in progress; Stop instead of deleting. This leaves
    	// time for install-notify.
    	// Download will be deleted on EDone COD event.
        CLOG_WRITE_FORMAT( "RHttpDownload delete when COD done: 0x%x", this )
    	iCodDownload->Stop();
        iExtension->iDeleteCodWhenDone = ETrue;
        
        if(iASReady && iWait && !iWait->IsStarted())
        	{
        	iWait->Start();	
        	}   
       	}

    if( !iDownloadMgr->IsEventBroadcasting() )
        {
        CLOG_WRITE_FORMAT( "RHttpDownload is deleting: 0x%x", this );
        iExtension->iToBeDeleted = EFalse;
        THttpDownloadEvent event;
        event.iDownloadState = EHttpDlDeleting;
        event.iProgressState = EHttpProgNone;
        TRAP( err, iDownloadMgr->HandleDMgrEventL( *this, event ) );
        if( KErrNone != err )
            {
            return err;
            }

    	DeleteTransaction();
       
      	if(!iClosedSubSession)
      		{
        	err = SendReceive( EHttpDownloadDelete, TIpcArgs() );
      		}

        // Server deleted it in server-side, so close this down in client-side:
        iDownloadMgr->CloseDownload( this );            
        }
    else
        {
        CLOG_WRITE_FORMAT( "RHttpDownload to be deleted: 0x%x", this )
        iExtension->iToBeDeleted = ETrue;
        }
        
    return err;
    }

// ---------------------------------------------------------
// RHttpDownload::Move
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::Move()
    {
    if(iClosedSubSession)
    	{
    	return KErrNotSupported;
    	}
    CLOG_WRITE( "RHttpDownload::Move" )

    return SendReceive( EHttpDownloadMove, TIpcArgs() );
    }

// ---------------------------------------------------------
// RHttpDownload::GetIntAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::GetIntAttribute( const TUint aAttribute, 
                                              TInt32& aValue )
    {
    CLOG_WRITE( "RHttpDownload::GetIntAttributeL" )

    if(iClosedSubSession)
    	{
    	if(!iDlExtension)
    		{
    		return KErrGeneral;
    		}
    	return iDlExtension->GetIntAttribute( aAttribute, aValue);;
	    }
    
    // IPC optimalization; some of the atributes are kept in the client side
    // and either refreshed with the events or they are not changed.
	TBool isProgressive (EFalse);
	GetBoolAttribute( EDlAttrProgressive, isProgressive );
	if (!isProgressive)
		{
		if( iExtension->GetIntAttribute( aAttribute, aValue ) )
			{
			return KErrNone;
			}
		}

    TPckg<TInt32> pckg( aValue );

    return SendReceive( EHttpDownloadGetIntAttribute, TIpcArgs( aAttribute, KNonMoIndex, &pckg ) );
    }
    
// ---------------------------------------------------------
// RHttpDownload::GetIntAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::GetIntAttribute( const TUint aAttribute, 
                                              const TInt32& aMoIndex,
                                              TInt32& aValue )
    {
    CLOG_WRITE( "RHttpDownload::GetIntAttributeL" )

    TPckg<TInt32> pckg( aValue );

    return SendReceive( EHttpDownloadGetIntAttribute, TIpcArgs( aAttribute, aMoIndex, &pckg ) );
    }

// ---------------------------------------------------------
// RHttpDownload::GetBoolAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::GetBoolAttribute( const TUint aAttribute, 
                                                TBool& aValue )
    {
    CLOG_WRITE( "RHttpDownload::GetBoolAttributeL" )
    
    if(iClosedSubSession)
    	{
    	if(!iDlExtension)
    		{
    		return KErrGeneral;
    		}
    	return iDlExtension->GetBoolAttribute( aAttribute, aValue);;
	    }

    TPckg<TBool> pckg( aValue );

    return SendReceive( EHttpDownloadGetBoolAttribute, TIpcArgs( aAttribute, KNonMoIndex, &pckg ) );
    }
    
// ---------------------------------------------------------
// RHttpDownload::GetBoolAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::GetBoolAttribute( const TUint aAttribute,
                                               const TInt32& aMoIndex,
                                               TBool& aValue )
    {
    CLOG_WRITE( "RHttpDownload::GetBoolAttributeL" )
    
    if(iClosedSubSession)
    	{
    	if(!iDlExtension)
    		{
    		return KErrGeneral;
    		}
    	return iDlExtension->GetBoolAttribute( aAttribute, aMoIndex, aValue);;
	    }
    

    TPckg<TBool> pckg( aValue );

    return SendReceive( EHttpDownloadGetBoolAttribute, TIpcArgs( aAttribute, aMoIndex, &pckg ) );
    }

// ---------------------------------------------------------
// RHttpDownload::GetStringAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::GetStringAttribute( const TUint aAttribute,
                                                 TDes16& aValue )
    {
    CLOG_WRITE( "RHttpDownload::GetStringAttributeL" )

    if(iClosedSubSession)
    	{
    	if(!iDlExtension)
    		{
    		return KErrGeneral;
    		}
    	return iDlExtension->GetStringAttribute( aAttribute, aValue);
    	}

    // IPC optimalization; some of the atributes are kept in the client side
    // and either refreshed with the events or they are not changed.    
    if( iExtension->GetStringAttribute( aAttribute, aValue) )
        {
        return KErrNone;
        }

    return SendReceive( EHttpDownloadGetStringAttribute, 
                        TIpcArgs( aAttribute, KNonMoIndex, &aValue ) );
    }
    
// ---------------------------------------------------------
// RHttpDownload::GetStringAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::GetStringAttribute( const TUint aAttribute,
                                                 const TInt32& aMoIndex,
                                                 TDes16& aValue )
    {
    CLOG_WRITE( "RHttpDownload::GetStringAttributeL" )
    
    if(iClosedSubSession)
    	{
    	if(!iDlExtension)
    		{
    		return KErrGeneral;
    		}
    	return iDlExtension->GetStringAttribute( aAttribute, aMoIndex, aValue);
    	}

    return SendReceive( EHttpDownloadGetStringAttribute, 
                        TIpcArgs( aAttribute, aMoIndex, &aValue ) );
    }

// ---------------------------------------------------------
// RHttpDownload::GetStringAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::GetStringAttribute( const TUint aAttribute, TDes8& aValue  )
    {
   
   CLOG_WRITE( "RHttpDownload::GetStringAttributeL" )
 
    if(iClosedSubSession)
    	{
    	if(!iDlExtension)
    		{
    		return KErrGeneral;
    		}
    	return iDlExtension->GetStringAttribute( aAttribute, aValue);
    	}
  
    // IPC optimalization; some of the atributes are kept in the client side
    // and either refreshed with the events or they are not changed.
    if( iExtension->GetStringAttribute( aAttribute, aValue) )
        {
        return KErrNone;
        }

    return SendReceive( EHttpDownloadGetString8Attribute, 
                        TIpcArgs( aAttribute, KNonMoIndex, &aValue ) );
    }
    
// ---------------------------------------------------------
// RHttpDownload::GetStringAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::GetStringAttribute( const TUint aAttribute,
                                                 const TInt32& aMoIndex,
                                                 TDes8& aValue  )
    {
    CLOG_WRITE( "RHttpDownload::GetStringAttributeL" )
    
     if(iClosedSubSession)
    	{
    	if(!iDlExtension)
    		{
    		return KErrGeneral;
    		}
    	return iDlExtension->GetStringAttribute( aAttribute, aMoIndex, aValue);
    	}
  

    return SendReceive( EHttpDownloadGetString8Attribute, 
                        TIpcArgs( aAttribute, aMoIndex, &aValue ) );
    }

// ---------------------------------------------------------
// RHttpDownload::GetFileHandleAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::GetFileHandleAttribute( RFile& aFile )
    {
    CLOG_WRITE("RHttpDownload::GetFileHandleAttribute")
    
    if(iClosedSubSession)
	    {
    	return KErrNotSupported;
    	}
    	
    TInt ssh;			// sub-session (RFile) handle
	TInt fsh;			// session (RFs) handle
	TPckgBuf<TInt> fh;		
	fsh = SendReceive(EHttpDownloadGetRFile, TIpcArgs(&fh));
	if (fsh < 0)	// error ?
		{
		return fsh;
		}

	ssh = fh();

	// Adopt the file using the returned handles
    return aFile.AdoptFromServer(fsh, ssh);
    }


// ---------------------------------------------------------
// RHttpDownload::SetIntAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::SetIntAttribute( const TUint aAttribute, 
                                              TInt32 aValue )
    {
    CLOG_WRITE( "RHttpDownload::SetIntAttributeL" )
    
    if(iClosedSubSession)
    	{
    	return KErrNotSupported;
    	}

    TInt err = SendReceive( EHttpDownloadSetIntAttribute, 
                            TIpcArgs( aAttribute, aValue ) );
    if( KErrNone == err )
        {
        iExtension->SetIntAttribute( aAttribute, aValue );
        }
    
    return err;
    }

// ---------------------------------------------------------
// RHttpDownload::SetBoolAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::SetBoolAttribute( const TUint aAttribute, TBool aValue )
    {
    CLOG_WRITE( "RHttpDownload::SetBoolAttributeL" )
	
	if(iClosedSubSession)
    	{
    		
    	if(!iDlExtension)
    		{
    		return KErrGeneral;
    		}
    		iDlExtension->SetBoolAttribute( aAttribute, aValue);
    		return;
    	}
    	
	if (aAttribute == EDlAttrProgressive)
	{
		if (iCodDownload)
		{
        // set attached attribute to handle progressive downloads in the COD Handler (for OMA2)
		iCodDownload->SetCodDlAttached(aValue);
		}
	}

    return SendReceive( EHttpDownloadSetBoolAttribute, 
                        TIpcArgs( aAttribute, aValue ) );
    }

// ---------------------------------------------------------
// RHttpDownload::SetStringAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::SetStringAttribute( const TUint aAttribute, 
                                                  const TDesC16& aValue )
    {
    CLOG_WRITE( "RHttpDownload::SetStringAttributeL" )
    
    if(iClosedSubSession)
    	{
    	return KErrNotSupported;
    	}

    TInt err = SendReceive( EHttpDownloadSetStringAttribute, 
                        TIpcArgs( aAttribute, KNonMoIndex, &aValue ) );
    if( KErrNone == err )
        {
        iExtension->SetStringAttribute( aAttribute, aValue );
        } 
    return err;                           
    }

// ---------------------------------------------------------
// RHttpDownload::SetStringAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::SetStringAttribute( const TUint aAttribute,  
                                                 const TInt32& aMoIndex,
                                                 const TDesC16& aValue )
    {
    CLOG_WRITE( "RHttpDownload::SetStringAttributeL" )
    
    if(iClosedSubSession)
    	{
    	return KErrNotSupported;
    	}
    return SendReceive( EHttpDownloadSetStringAttribute, 
                        TIpcArgs( aAttribute, aMoIndex, &aValue ) );
    }

// ---------------------------------------------------------
// RHttpDownload::SetStringAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::SetStringAttribute( const TUint aAttribute, const TDesC8& aValue )
    {
    CLOG_WRITE( "RHttpDownload::SetStringAttributeL" )

    if(iClosedSubSession)
    	{
    	return KErrNotSupported;
    	}
    	
    TInt err = SendReceive( EHttpDownloadSetString8Attribute, 
                            TIpcArgs( aAttribute, &aValue ) );

    if( KErrNone == err )
        {
        iExtension->SetStringAttribute( aAttribute, aValue );
        }

    return err;
    }

// ---------------------------------------------------------
// RHttpDownload::SetStringAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::SetFileHandleAttribute( RFile& aFile )
    {
    if(iClosedSubSession)
    	{
    	return KErrNotSupported;
    	}
    if( !aFile.SubSessionHandle() )
        // file is not opened
        {
        return KErrBadHandle;
        }
    // store the RFs handle in message slot 0 and the RFile handle in slot 1
	TIpcArgs ipcArgs;
	aFile.TransferToServer(ipcArgs, 0, 1);		

	// send to server
	return SendReceive(EHttpDownloadSetRFile, ipcArgs); 
    }

// ---------------------------------------------------------
// RHttpDownload::SetDownloadDataAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::SetDownloadDataAttribute( const TDesC8& aValue )
    {
    CLOG_WRITE( "RHttpDownload::SetDownloadDataAttribute" )
    
    return SendReceive( EHttpDownloadDataAttribute, TIpcArgs( &aValue ) );
    }
    
// ---------------------------------------------------------
// RHttpDownload::SetTrackDataAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownload::SetTrackDataAttribute( const TInt aIndex, const TDesC8& aValue )
    {
    CLOG_WRITE( "RHttpDownload::SetTrackDataAttribute" )
    
    return SendReceive( EHttpDownloadTrackAttribute, TIpcArgs( aIndex, &aValue ) );
    }

// ---------------------------------------------------------
// RHttpDownload::IsDownload
// ---------------------------------------------------------
//
TBool RHttpDownload::IsDownload( TInt aHandle )
    {
    CLOG_WRITE( "RHttpDownload::IsDownload" )

    return (iHandle == aHandle) ? ETrue : EFalse;
    }
// ---------------------------------------------------------
// RHttpDownload::IsDownload
// ---------------------------------------------------------
//
TBool RHttpDownload::IsDownloadL( const TUriParser8& aParsedReqUrl, const TDesC8& aHashedMsgBody )
    {
    CLOG_WRITE( "RHttpDownload::IsDownload" );

    TBuf8<KMaxUrlLength> url_Own;
	TBuf8<KHashLength> hash_owned;
	
	TInt errURL(KErrNone);
	TInt errHashedMsg(KErrNone);
	
	if( iClosedSubSession)
    	{
    	errURL = iDlExtension->GetStringAttribute( EDlAttrReqUrl, url_Own );
    	if(!errURL)
    		{
    		errHashedMsg = iDlExtension->GetStringAttribute( EDlAttrHashedMsgBody, hash_owned );	
    		}
    	}
    else
    	{
    	errURL = GetStringAttribute( EDlAttrReqUrl, url_Own );
    	if(!errURL)
    		{
	    	errHashedMsg = GetStringAttribute( EDlAttrHashedMsgBody, hash_owned );
    		}
    	}
    	
    TBool retVal( EFalse );
    
    if( !errURL )
        {
        HBufC8* escaped_own = EscapeUtils::EscapeDecodeL( url_Own );
        CleanupStack::PushL( escaped_own );
        
        TUriParser8 parsed_Own;
        
        parsed_Own.Parse( *escaped_own );
        
        if( !aParsedReqUrl.Extract( EUriHost ).CompareF( parsed_Own.Extract( EUriHost ) ) &&
            aParsedReqUrl.Extract( EUriPath ) == parsed_Own.Extract( EUriPath ) &&
            aParsedReqUrl.Extract( EUriQuery ) == parsed_Own.Extract( EUriQuery ) &&
            aParsedReqUrl.Extract( EUriFragment ) == parsed_Own.Extract( EUriFragment ) )
            {


            if( !errHashedMsg )
                //
                {
                if( 0 == aHashedMsgBody.Compare( hash_owned ) )
                    {
                    retVal = ETrue;
                    }
                }
            else
                // hashed message body cannot be compared due to some unknown reason.
                // Lets assume that the contents are the same.
                {
                retVal = ETrue;
                }
            }
            
        CleanupStack::PopAndDestroy( escaped_own );
        }
        
    return retVal;
    }
    
// ---------------------------------------------------------
// RHttpDownload::BufferAttributesL
// ---------------------------------------------------------
//
void RHttpDownload::BufferAttributesL()
    {
    CLOG_ENTERFN("RHttpDownload::BufferAttributesL");
 
       if(iClosedSubSession)
    	{
    	return;
    	}
 
   
    HBufC8* buf = iExtension->AllocAttribBufL( ETrue );
    CleanupStack::PushL( buf );
    TPtr8 ptr = buf->Des();
    
    User::LeaveIfError( SendReceive( EHttpDownloadBufferAttributes, 
                        TIpcArgs( &ptr ) ) );
    iExtension->UnpackAttribL( ptr );                    
    CleanupStack::PopAndDestroy( buf );    // buf
    }

// -----------------------------------------------------------------------------
// RHttpDownload::SetEventType
// -----------------------------------------------------------------------------
//
void RHttpDownload::SetEventType( THttpDownloadState aEventType )
    {
    CLOG_ENTERFN("RHttpDownload::SetEventType");
    
     if(iClosedSubSession)
    	{
    	return ;
    	}
    	
    if( iExtension )
        {
        iExtension->SetEventType( aEventType );
        }
    }

// -----------------------------------------------------------------------------
// RHttpDownload::SetHttpTransactionL
// -----------------------------------------------------------------------------
//
void RHttpDownload::SetHttpTransactionL( TInt aHttpTransaction )
    {
    iDownloadMgr->IncrementEventPriorityFlag();
    CLOG_ENTERFN("RHttpDownload::SetHttpTransactionL");
    
    THTTPHdrVal hdrVal;

    iClientSideDownload = ETrue;
    
    iHttpTransaction = REINTERPRET_CAST( RHTTPTransaction* , aHttpTransaction );
    iTransactionObserver = CTransactionObserver::NewL( this );

    RHTTPTransactionPropertySet propSet = iHttpTransaction->PropertySet();

    RStringPool strPool = iHttpTransaction->Session().StringPool();
    // set transaction property with the event handler callback functions' pointer
    RStringF transactionCallbackStr = strPool.OpenFStringL( KTransactionCallback );
    CleanupClosePushL( transactionCallbackStr );
    THTTPHdrVal tokenVal = (TInt)(MHTTPTransactionCallback*)iTransactionObserver;
    iHttpTransaction->PropertySet().RemoveProperty( transactionCallbackStr );
    iHttpTransaction->PropertySet().SetPropertyL( transactionCallbackStr, tokenVal );

    CleanupStack::PopAndDestroy(); // transactionCallbackStr

    //
    RHTTPHeaders headers( iHttpTransaction->Response().GetHeaderCollection() );
    THTTPHdrFieldIter it = headers.Fields();
    TInt length( 0 );
    HBufC8* header = NULL;

    for( TInt i = 0; i < 2; ++i )
        {
        while ( !it.AtEnd() )
            {
            RStringTokenF fieldName = it();
            RStringF fieldNameStr = strPool.StringF (fieldName );

            TPtrC8 rawData;

            headers.GetRawField( fieldNameStr, rawData );

            if( !header )
                {
                length += fieldNameStr.DesC().Length() + 
                          1 + // KColon
                          rawData.Length() +
                          KHttpFieldSeparator().Length();
                }
            else
                {
                header->Des().Append( fieldNameStr.DesC() );
                header->Des().Append( KColon );
                header->Des().Append( rawData );
                header->Des().Append( KHttpFieldSeparator );
                }

            ++it;
            }

        if( !header )
            {
            header = HBufC8::NewL( length );
            it.First();
            }
        }

    if( header )
        {
        SetStringAttribute( EDlAttrResponseHeader, *header );
        }

    delete header; header = NULL;
    
    //realm
    if (propSet.Property(strPool.StringF(
		HTTP::ERealm, RHTTPSession::GetTable()), hdrVal))
        {
        SetStringAttribute( EDlAttrRealm, hdrVal.Str().DesC() );
        }

    // username
    if (propSet.Property(strPool.StringF(
        HTTP::EUsername, RHTTPSession::GetTable()), hdrVal))
        {
        SetStringAttribute( EDlAttrUsername, hdrVal.Str().DesC() );
        }

    // password
    if (propSet.Property(strPool.StringF(
        HTTP::EPassword, RHTTPSession::GetTable()), hdrVal))
        {
        SetStringAttribute( EDlAttrPassword, hdrVal.Str().DesC() );
        }

    // proxy realm    
    if (propSet.Property(strPool.StringF(
	    HttpFilterCommonStringsExt::EProxyRealm,
	    HttpFilterCommonStringsExt::GetTable()), hdrVal))
        {
        SetStringAttribute( EDlAttrProxyRealm, hdrVal.Str().DesC() );
        }

    // proxy username
    if (propSet.Property(strPool.StringF(
	    HttpFilterCommonStringsExt::EProxyUsername,
	    HttpFilterCommonStringsExt::GetTable()), hdrVal))
        {
        SetStringAttribute( EDlAttrProxyUsername, hdrVal.Str().DesC() );
        }

    // proxy password
    if (propSet.Property(strPool.StringF(
	    HttpFilterCommonStringsExt::EProxyPassword, RHTTPSession::GetTable()), hdrVal))
        {
        SetStringAttribute( EDlAttrProxyPassword, hdrVal.Str().DesC() );
        }

    RStringF method = iHttpTransaction->Request().Method();
    if( method == strPool.StringF( HTTP::EGET, RHTTPSession::GetTable()) )
        {
        SetIntAttribute( EDlAttrMethod, EMethodGET );
        }
    else if( method == strPool.StringF( HTTP::EHEAD, RHTTPSession::GetTable()) )
        {
        SetIntAttribute( EDlAttrMethod, EMethodHEAD );
        }
    else if( method == strPool.StringF( HTTP::EPOST, RHTTPSession::GetTable()) )
        {
        SetIntAttribute( EDlAttrMethod, EMethodPOST );
        }

    // Hashed message body
    TBool hasBody = iHttpTransaction->Request().HasBody();
    if( hasBody )
        {
        TPtrC8 dataPart;
		MHTTPDataSupplier* reqBody = iHttpTransaction->Request().Body();
		reqBody->Reset();
        reqBody->GetNextDataPart( dataPart );
        TBuf8<KHashLength> hash;
        HashL( dataPart, hash );
        SetStringAttribute( EDlAttrHashedMsgBody, hash );
        }

    if( iHttpTransaction->PropertySet().Property( 
                               strPool.StringF(HttpFilterCommonStringsExt::EContentInflated, 
                               HttpFilterCommonStringsExt::GetTable()), 
                               hdrVal ))
        // Content was original encoded -> we don't know the actual content size.
        {
        SetIntAttribute( EDlAttrMultipleMOLength, KDefaultContentLength );
        }
    }

// -----------------------------------------------------------------------------
// RHttpDownload::DeleteTransaction
// -----------------------------------------------------------------------------
//
void RHttpDownload::DeleteTransaction()
    {
    CLOG_ENTERFN("RHttpDownload::DeleteTransaction");
    
    iClientSideDownload = EFalse;
    
    if( iHttpTransaction )
        {
        iHttpTransaction->Close();
        delete iHttpTransaction;
        iHttpTransaction = NULL;
        }

    if( iTransactionObserver )
        {
        delete iTransactionObserver;
        iTransactionObserver = NULL;
        }
        
    iDownloadMgr->DecrementEventPriorityFlag();
    }

// -----------------------------------------------------------------------------
// RHttpDownload::InitCodDownloadL
// -----------------------------------------------------------------------------
//    
void RHttpDownload::InitCodDownloadL( const TDesC8& aBuf, 
                                      const TDesC8& aMimeType,
                                      CEikonEnv* aEikEnv )
	{
	CLOG_ENTERFN("RHttpDownload::InitCodDownloadL");

    if(iCodDownload)
        {
        delete iCodDownload;
        iCodDownload = NULL;
        }

    if(iCodObserver)
        {
        delete iCodObserver;
        iCodObserver = NULL;
        }

	SetBoolAttribute( EDlAttrCodDownload, ETrue );

    if(!iCodObserver)
        {
        iCodObserver = CCodObserver::NewL( iDownloadMgr );
        }

    TBool silent( EFalse );
    iDownloadMgr->GetBoolAttribute( EDlMgrSilentMode, silent );
    if( silent )
        {
        aEikEnv = NULL;
        }

    if( 0 == aMimeType.Compare( KMultiPartMimeType() ) )
        {
        TInt cleanupcount( 0 );
        HBufC8* boundbuff = HBufC8::NewLC( KMaxContentTypeLength ); cleanupcount++;
        TPtr8 boundary = boundbuff->Des();
        // Leave if media type doesn't exist; there is no boundary
        User::LeaveIfError( GetStringAttribute( EDlAttrMediaTypeBoundary, boundary ) );                
        HBufC* buf16 = HBufC::NewLC( KMaxUrlLength ); cleanupcount++;       
        TPtr buf16ptr = buf16->Des();
        User::LeaveIfError( GetStringAttribute( EDlAttrReqUrl, buf16ptr ) );

        if(!iCodDownload)                                 
            {
            iCodDownload = CCodDownload::NewL( aBuf,
                                           aMimeType,
                                           *iCodObserver, 
                                           aEikEnv,
                                           &boundary,
                                           &buf16ptr );
            }
        CleanupStack::PopAndDestroy( cleanupcount );      
        }
    else
        {

        if(!iCodDownload)                                 	    
            iCodDownload = CCodDownload::NewL( aBuf, aMimeType, *iCodObserver, aEikEnv );
        }

	if( !iWait )
		{
		iWait = new (ELeave) CActiveSchedulerWait;
		}

	SDMgrCodUserData* userData = new (ELeave) SDMgrCodUserData;
	userData->iHandle = iHandle;
	userData->iPrevCodEvent = (MCodDownloadObserver::TEvent)KInitPrevCodEvent;
	iCodDownload->SetUserData( (TAny*)userData );
	iDownloadMgr->IncrementEventPriorityFlag();	
	}
	

void RHttpDownload::InitPausedCodDownloadL( const TUid aAppUid )
	{

    TBuf8<KMaxContentTypeLength> contentType; 
	
    GetStringAttribute( EDlAttrDDType , contentType );	
	
	CEikonEnv* eikenv = CEikonEnv::Static();
	
	
    TInt32 download; 
    GetIntAttribute(EDlAttrId , download);

    
    if(iCodDownload)
        {
        delete iCodDownload;
        iCodDownload = NULL;
        }

    if(iCodObserver)
        {
        delete iCodObserver;
        iCodObserver = NULL;
        }

	SetBoolAttribute( EDlAttrCodDownload, ETrue );

    if(!iCodObserver)
        {
        iCodObserver = CCodObserver::NewL( iDownloadMgr );
        }

    TBool silent( EFalse );
    iDownloadMgr->GetBoolAttribute( EDlMgrSilentMode, silent );
    if( silent )
        {
        eikenv = NULL;
        }
	
    if( 0 == contentType.Compare( KMultiPartMimeType() ) )
        {
        /*
        TInt cleanupcount( 0 );
        HBufC8* boundbuff = HBufC8::NewLC( KMaxContentTypeLength ); cleanupcount++;
        TPtr8 boundary = boundbuff->Des();
        // Leave if media type doesn't exist; there is no boundary
        User::LeaveIfError( GetStringAttribute( EDlAttrMediaTypeBoundary, boundary ) );                
        HBufC* buf16 = HBufC::NewLC( KMaxUrlLength ); cleanupcount++;       
        TPtr buf16ptr = buf16->Des();
        User::LeaveIfError( GetStringAttribute( EDlAttrReqUrl, buf16ptr ) );

        if(!iCodDownload)                                 
            {
            iCodDownload = CCodDownload::NewL( aBuf,
                                           aMimeType,
                                           *iCodObserver, 
                                           aEikEnv,
                                           &boundary,
                                           &buf16ptr );
            }
        CleanupStack::PopAndDestroy( cleanupcount );    */  
        }
    else
        {

        if(!iCodDownload)                                 	    
            iCodDownload = CCodDownload::NewL( download, aAppUid, contentType, *iCodObserver, eikenv );
        }

	SDMgrCodUserData* userData = new (ELeave) SDMgrCodUserData;
	userData->iHandle = iHandle;
	userData->iPrevCodEvent = (MCodDownloadObserver::EDone);
	if(iCodDownload)
	    {
	    iCodDownload->SetUserData( (TAny*)userData );
	    iDownloadMgr->IncrementEventPriorityFlag();
	    }
	
	}
// ---------------------------------------------------------
// RHttpDownload::CheckContentTypeAndCreateCodDownloadL
// ---------------------------------------------------------
//
TBool RHttpDownload::CheckContentTypeAndCreateCodDownloadL()
	{
	CLOG_ENTERFN("RHttpDownload::CheckContentTypeAndCreateCodDownloadL");
	
	TBool ret( ETrue ); // False if it is not a cod descriptor
	                    // set in the if statement

    TBuf8<KMaxContentTypeLength> ddContentType;    

    GetStringAttribute( EDlAttrDDType, ddContentType );	

    if( ( 0 == ddContentType.Compare( KCodMimeType() ) ) ||
        ( 0 == ddContentType.Compare( KDdMimeType() ) ) ||
        ( 0 == ddContentType.Compare( KDd2MimeType() ) ) ||
        ( 0 == ddContentType.Compare( KMultiPartMimeType() ) ) )
        {
               
        // This way we save IPC
        TBuf<KMaxPath>               fileName;
        RFile                        file;
        RFs                          fs;
        TInt                         size( 0 );
        
        CleanupClosePushL< RFs >( fs );
        CleanupClosePushL< RFile >( file );
        
        // Get DD Filename for creating the download
		User::LeaveIfError
            ( GetStringAttribute( EDlAttrDdFileName, fileName ) );
            
        User::LeaveIfError( fs.Connect() );
        
        TInt err = file.Open( fs, fileName,
            EFileShareReadersOnly | EFileStream | EFileRead );
        if ( err == KErrInUse )
            {
            err = file.Open( fs, fileName,
                EFileShareAny | EFileStream | EFileRead );
            }
        User::LeaveIfError( err );         
    	User::LeaveIfError( file.Size( size ) );
    	HBufC8* buf = HBufC8::NewLC( size );
    	TPtr8 ptr = buf->Des();
    	User::LeaveIfError( file.Read( ptr ) );
    	CEikonEnv* eikenv = CEikonEnv::Static();
    	
        InitCodDownloadL( ptr, 
                          ddContentType, 
                          eikenv );
                                   
        CleanupStack::PopAndDestroy( 3 ); //  buf, file, fs
        }
    else
        {
        ret = EFalse;
        }
        
	return ret;
	}


// ---------------------------------------------------------
// RHttpDownload::SetOnError
// ---------------------------------------------------------
//
TInt RHttpDownload::SetOnError( TInt aError,
                                THttpDownloadMgrError aDlError )
    {
    CLOG_WRITE( "RHttpDownload::SetOnError" )
     
     if(iClosedSubSession)
    	{
    	return KErrNotSupported;
    	}
    	    
    return SendReceive( EHttpDownloadSetOnError, 
                        TIpcArgs( aError, aDlError ) );
    }

// -----------------------------------------------------------------------------
// RHttpDownload::IsClientSideDownload
// -----------------------------------------------------------------------------
// 
TBool RHttpDownload::IsClientSideDownload() const
    {
    return iClientSideDownload;
    }

// ---------------------------------------------------------
// RHttpDownload::IsToBeDeleted
// ---------------------------------------------------------
//
TBool RHttpDownload::IsToBeDeleted()
    {
    CLOG_WRITE( "RHttpDownload::IsToBeDeleted" )
    return iExtension->iToBeDeleted;
    }

// ---------------------------------------------------------
// RHttpDownload::EventPriorityFlag
// ---------------------------------------------------------
//       
TInt RHttpDownload::EventPriorityFlag() const
    {
    return iDownloadMgr->EventPriorityFlag();
    }

// ---------------------------------------------------------
// RHttpDownload::ActiveMoIndex
// ---------------------------------------------------------
//    
TInt32 RHttpDownload::ActiveMoIndex() const
	{
	return iExtension->iMoIndex;
	}

// ---------------------------------------------------------
// RHttpDownload::GetProductDownloadedSize
// ---------------------------------------------------------
//  
TInt RHttpDownload::GetProductDownloadedSize( )
    {
    TPckg<TInt32> pckg( iExtension->iMoDownloadedSize );
    return SendReceive( EHttpDownloadGetIntAttribute, TIpcArgs( EDlAttrMultipleMODownloadedSize, KNonMoIndex, &pckg ) );
    }
    
// ---------------------------------------------------------
// RHttpDownload::StopWaitingAS()
// ---------------------------------------------------------
//       
void RHttpDownload::StopWaitingAS()
    {
    if(iWait && iWait->IsStarted())
    	{
    	iWait->AsyncStop();
    	}
    }
