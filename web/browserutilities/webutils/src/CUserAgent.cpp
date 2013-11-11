/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CUserAgent.
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <f32file.h>
#include <eikenv.h>

#include <Browser_platform_variant.hrh>
#include "WebUtilsCommon.h"
#include "CUserAgent.h"
#include "WebUtilsLogger.h"

#include "WebUtilsInternalCRKeys.h"


#define KPlaceHolderMozillaVer		_L("Mozilla/5.0")
#define KPlaceHolderComponent		_L("AppleWebKit/525 (KHTML, like Gecko) Version/3.0")
#define KPlaceHolderBrowserName		_L("BrowserNG")

#define KPlaceHolderSlash           _L("/")
#define KPlaceHolderSymbianOS       _L("SymbianOS")
#define KPlaceHolderSeries60        _L("Series60")

#define KPlaceHolderOpen            _L("(")
#define KPlaceHolderClose           _L(")")
#define KPlaceHolderSemiColon       _L(";")
#define KPlaceHolderPeriod          _L(".")

const TInt KMaxUserAgentStringLength = 256;
const TInt KMaxBrowserVersionStringLength = 20;
const TInt KMaxBrowserNameLength = 20;

// Space character.
LOCAL_D const TUint8 KSpaceChar = ' ';

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CUserAgent::CUserAgent()
// Constructor with parent
// ---------------------------------------------------------
//
CUserAgent::CUserAgent() 
    {
	LOG_CREATE
 	LOG_WRITE( "CUserAgent::CUserAgent" );
 	}

// ---------------------------------------------------------
// CUserAgent::~CUserAgent()
// Destructor
// ---------------------------------------------------------
//
CUserAgent::~CUserAgent()
    {
 	LOG_WRITE( "CUserAgent::~CUserAgent" );
 	
 	delete iRepository;
 	
 	delete iBrowserCenrepStr;
	delete iMMSCenrepStr;
	
	delete iUAProfURL;
	delete iUAProfURL3G;
	delete iBrowserVersionStr;
	delete iBrowserNameAndVersionStr;
	
	LOG_DELETE
    }

// ---------------------------------------------------------
// CUserAgent* CUserAgent::NewL()
// ---------------------------------------------------------
//
EXPORT_C CUserAgent* CUserAgent::NewL()
    {
	CUserAgent* self = new (ELeave) CUserAgent;
 	CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CUserAgent::ConstructL()
// Symbian constructor
// ---------------------------------------------------------
//
void CUserAgent::ConstructL()
    {
    LOG_ENTERFN("CUserAgent::ConstructL()");

	iBrowserCenrepStr = iMMSCenrepStr = NULL;
	iUAProfURL = iUAProfURL3G = NULL;
	iRepository = NULL;

	iRepository = CRepository::NewL(KCRUidWebUtils);
	iUAStrQueried = iMMSUAStrQueried = iURLUAProfQueried = FALSE;
	GetBrowserVersionL();
	GetBrowserNameAndVersionL();
	
    LOG_LEAVEFN("CUserAgent::ConstructL()");
    }

// ---------------------------------------------------------
// CUserAgent::UserAgentL()
// Returns complete constructed user agent string.
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CUserAgent::UserAgentL()
{
	LOG_ENTERFN("CUserAgent::UserAgent()");
    TRAPD( ErrorCode,ReadUserAgentStringL());
	if(ErrorCode != KErrNone)
	{
		LOG_WRITE_FORMAT("ERROR: ReadUserAgentStringL failed! (%d) Returning NULL",ErrorCode);
		LOG_LEAVEFN("CUserAgent::UserAgent()");
		return NULL;
	}
		
	HBufC8 *UAStringBuf = HBufC8::NewL(KMaxUserAgentStringLength);
	TPtr8 UAStringPtr = UAStringBuf->Des();
	UAStringPtr.Copy(iBrowserCenrepStr->Des());

	LOG_LEAVEFN("CUserAgent::UserAgent()");
	return UAStringBuf;
}

// ---------------------------------------------------------
// CUserAgent::MMSUserAgentL()
// Returns complete constructed MMS user agent string.
// ---------------------------------------------------------
EXPORT_C HBufC8* CUserAgent::MMSUserAgentL()
{
	LOG_ENTERFN("CUserAgent::MMSUserAgent()");

	TRAPD( ErrorCode,ReadMMSUserAgentStringL());
	if(ErrorCode != KErrNone)
	{
		LOG_WRITE_FORMAT("ERROR: ReadMMSUserAgentStringL failed! (%d) Returning NULL",ErrorCode);
		LOG_LEAVEFN("CUserAgent::MMSUserAgent()");
		return NULL;
	}

	HBufC8 *MMSUAStringBuf = HBufC8::NewL(KMaxUserAgentStringLength);
	TPtr8 UAStringPtr = MMSUAStringBuf->Des();
	UAStringPtr.Copy(iMMSCenrepStr->Des());

	LOG_LEAVEFN("CUserAgent::MMSUserAgent()");
	return MMSUAStringBuf;
}

// ---------------------------------------------------------
// CUserAgent::URLToUserAgentProfileL()
// Fetches URL to UA profile.
// ---------------------------------------------------------
EXPORT_C TInt CUserAgent::URLToUserAgentProfileL(TDes8 &aUAProfURL, TDes8 &aUAProfURL3G)
{
	LOG_ENTERFN("CUserAgent::URLToUserAgentProfileL()");

	TRAPD( ErrorCode,ReadURLUserAgentProfileL());
	if(ErrorCode != KErrNone)
	{
		LOG_WRITE_FORMAT("ERROR: ReadURLUserAgentProfileL failed! (%d) Returning NULL",ErrorCode);
		LOG_LEAVEFN("CUserAgent::URLToUserAgentProfileL()");
		return ErrorCode;
	}

	aUAProfURL.Copy(iUAProfURL->Des());
	aUAProfURL3G.Copy(iUAProfURL3G->Des());

	LOG_LEAVEFN("CUserAgent::URLToUserAgentProfileL()");
	
	return ErrorCode;
}

// ---------------------------------------------------------
// CUserAgent::ReadUserAgentStringL()
// Reads the UA string from cenrep
// ---------------------------------------------------------
void CUserAgent::ReadUserAgentStringL()
	{
	LOG_ENTERFN("CUserAgent::ReadUserAgentStringL()");
	TInt retCode (KErrNotReady);
	TBuf<64> ignoreConfig(0);

	// If the string queried already, dont read again
	if(iUAStrQueried)
		{
		LOG_WRITE("UA String already in cache. No read will be performed.");
		LOG_LEAVEFN("CUserAgent::ReadUserAgentDataL()");
		return;
		}

	if(!iRepository)
		{
		LOG_WRITE("Error: Repository doesn't exit or currupt!");
		LOG_LEAVEFN("CUserAgent::ReadUserAgentDataL()");
		User::Leave(KErrNotReady);
		}
	
	if(!iBrowserCenrepStr)
		{
		iBrowserCenrepStr = HBufC::NewL(KMaxUserAgentStringLength);	
		}

	TPtr tempPtr = iBrowserCenrepStr->Des();
	retCode = iRepository->Get(KWebUtilsIgnoreConfig, ignoreConfig);
	if(retCode != KErrNone || ignoreConfig == _L("0"))
		{/* ---------------- Read default UA string ---------------- */
			LOG_WRITE("Reading default UA string from cenrep.");
			
			retCode = iRepository->Get(EDefaultUserAgentString, tempPtr);
			if(retCode != KErrNone)
				{
				LOG_WRITE_FORMAT("Error: Reading default UA string from cenrep failed! (%d)",retCode);
				LOG_LEAVEFN("CUserAgent::ReadUserAgentDataL()");
				User::Leave(retCode);
				}
		
			if(!IsUAStringEmpty(tempPtr))
				{
				LOG_WRITE("Successfully read default UA string from cenrep, can use this as UA string");
		
				iUAStrQueried = TRUE;
		
				LOG_LEAVEFN("CUserAgent::ReadUserAgentDataL()");
				return;
				}
		}
	ReadAndCreateUAStringL(tempPtr);	
	LOG_WRITE("Successfully created a new UA string.");

	// Mark the flag to indicate successful read
	iUAStrQueried = TRUE;

    LOG_LEAVEFN("CUserAgent::ReadUserAgentDataL()");
    return;
	}

// ---------------------------------------------------------
// CUserAgent::IsUAStringEmpty()
// Checks if the UA string is empty or "NONE"
// ---------------------------------------------------------
bool CUserAgent::IsUAStringEmpty(const TPtr &aUAString)
	{
	return (aUAString.Compare(_L("NONE")) == 0 || aUAString.Length() == 0);
	}


// ---------------------------------------------------------
// CUserAgent::ReadMMSUserAgentStringL()
// Reads the MMS UA string from cenrep
// ---------------------------------------------------------

void CUserAgent::ReadMMSUserAgentStringL()
	{
	
	//  MMS UA Format: <HardwareType>; <Series60Version> <MIDP_CLDC Version>
	
	LOG_ENTERFN("CUserAgent::ReadMMSUserAgentStringL()");
	TInt retCode (KErrNotReady);

	// If the string queried already, dont read again
	if(iMMSUAStrQueried)
		{
		LOG_WRITE("MMS UA String already in cache. No read will be performed.");
		LOG_LEAVEFN("CUserAgent::ReadMMSUserAgentStringL()");
		return;
		}

	if(!iRepository)
		{
		LOG_WRITE("Error: Repository doesn't exit or currupt!");
		LOG_LEAVEFN("CUserAgent::ReadMMSUserAgentStringL()");
		
		User::Leave(KErrNotReady);// -1;	// Error code ?
		}

	/* ---------------- Read MMS default UA string ---------------- */
	if(!iMMSCenrepStr)
		{
		iMMSCenrepStr = HBufC::NewL(KMaxUserAgentStringLength);
		}

	LOG_WRITE("Default MMS UA string is to be read from cenrep");
	TPtr tempPtr = iMMSCenrepStr->Des();

	retCode = iRepository->Get(EDefaultMMSUserAgentString, tempPtr);
	if(retCode != KErrNone)
		{
		LOG_WRITE("Error: Reading default MMS UA string from cenrep failed!");
		LOG_LEAVEFN("CUserAgent::ReadMMSUserAgentStringL()");
		User::Leave(retCode);
		}

	if(!IsUAStringEmpty(tempPtr))
		{
		LOG_WRITE("Successfully read default MMS UA string from cenrep, can use this as MMS UA string");

		iMMSUAStrQueried = TRUE;
		LOG_LEAVEFN("CUserAgent::ReadMMSUserAgentStringL()");
		return;
		}

	/* ---------------- Read all variant fragments and build a UA string ---------------- */
	GetMMSFragmentsL();
			tempPtr.Append(iMMSHardwareType);
			tempPtr.Append(KSpaceChar);
			tempPtr.Append(iMMSS60Version);
			tempPtr.Append(KSpaceChar);
			tempPtr.Append(iMMSMIDPVersion);
			tempPtr.Append(KSpaceChar);
			tempPtr.Append(iMMSCLDCVersion);
				
	if(!IsUAStringEmpty(tempPtr))
		{	
			LOG_WRITE("Successfully created a new MMS UA string.");
		}
		else
		{
			LOG_WRITE("Failed to create a new MMS UA string...Exiting anyway");
		}
	
	iMMSUAStrQueried = TRUE;
  LOG_LEAVEFN("CUserAgent::ReadMMSUserAgentStringL()");
  return;
}

// ---------------------------------------------------------
// CUserAgent::GetFragmentsL()
// Gets all required fragments from CenRep
// ---------------------------------------------------------
TInt CUserAgent::GetCenRepFragment(TUserAgentStringKeys aFragmentType, TDes& fragmentValue)
	{
	TInt retCode(iRepository->Get(aFragmentType, fragmentValue));

	if(fragmentValue.Length() <= 0)
		{
		retCode = KErrNotReady;
		}

    return retCode;	
	}

// ---------------------------------------------------------
// CUserAgent::GetFragmentsL()
// Gets all required fragments from OSExt APIs/CenRep
// ---------------------------------------------------------
void CUserAgent::GetFragmentsL()
	{
	if(iSymbianVersion.Length() <= 0)
		{
		LOG_WRITE("Reading SymbianVersion from cenrep.");
		GetCenRepFragment(ESymbianOSVersion, iSymbianVersion);
		}
	if(iS60Version.Length() <= 0)
		{
		LOG_WRITE("Reading S60Version from cenrep.");
		GetCenRepFragment(ES60Version, iS60Version);
		}
	if(iHardwareType.Length() <= 0)
		{
		LOG_WRITE("Reading HardwareVersion from cenrep.");
		GetCenRepFragment(EHardwareType, iHardwareType);
		if(iHardwareType.Length() > 0)
			{ 		iHardwareType.Append(KPlaceHolderSemiColon); 
			}
		}
	if(iMIDPCLDCVersion.Length() <= 0)
		{
    	LOG_WRITE("Reading MIDPCLDCVersion from cenrep.");
    	GetCenRepFragment(EMIDP_CLDCVersion, iMIDPCLDCVersion);
		}
	//Only for 5.0 and above as 3.2 does not support OSExt APIs.
	#ifdef BRDO_BROWSER_50_FF
		if(iSymbianVersion.Length() <= 0 || iS60Version.Length() <= 0 || iHardwareType.Length() <= 0)
		{
			LOG_WRITE("Read from CenRep Failed... trying osext extended api");
			GetFragmentsFromOSExtL();
		}
	#endif
	}

// ---------------------------------------------------------
// CUserAgent::ReadAndCreateUAStringL()
// Reads verious fragments from OSExt APIs/CenRep and crates the UA string
// ---------------------------------------------------------
void CUserAgent::ReadAndCreateUAStringL(TPtr &aUAStringPtr)
	{
	GetFragmentsL();

	// Mozilla/5.0 (<Symbian Version> <Series60Version> <HardwareType> <MIDP_CLDC Version> ) AppleWebKit/413 (KHTML, like Gecko) BrowserName/Version
		
	aUAStringPtr.Copy(KPlaceHolderMozillaVer);
	aUAStringPtr.Append(KSpaceChar);
	if(iSymbianVersion.Length() > 0 || iS60Version.Length() > 0 || iHardwareType.Length() > 0 || iMIDPCLDCVersion.Length() > 0)
		{
			aUAStringPtr.Append(KPlaceHolderOpen);
			
			aUAStringPtr.Append(iSymbianVersion);
			aUAStringPtr.Append(KSpaceChar);
			
			aUAStringPtr.Append(iS60Version);
			aUAStringPtr.Append(KSpaceChar);
			
			aUAStringPtr.Append(iHardwareType);
			aUAStringPtr.Append(KSpaceChar);
			
			aUAStringPtr.Append(iMIDPCLDCVersion);
			aUAStringPtr.Append(KSpaceChar);
			
			aUAStringPtr.Append(KPlaceHolderClose);
		}
	aUAStringPtr.Append(KSpaceChar);
	aUAStringPtr.Append(KPlaceHolderComponent);	
	
	aUAStringPtr.Append(KSpaceChar);
	aUAStringPtr.Append(iBrowserNameAndVersionStr->Des());
	}

// ---------------------------------------------------------
// CUserAgent::ReadURLUserAgentProfileL()
// Reads the URL to UA profile from cenrep
// ---------------------------------------------------------

void CUserAgent::ReadURLUserAgentProfileL() 
{
    LOG_ENTERFN("CUserAgent::ReadURLUserAgentProfileL()");
    
	// If the string queried already, dont read again
	if(iURLUAProfQueried)
	{
		LOG_WRITE("URL to UA profile already in cache. No read will be performed.");
		LOG_LEAVEFN("CUserAgent::ReadURLUserAgentProfileL()");
		return;
	}

	if(!iRepository)
	{
		LOG_WRITE("Error: Repository doesn't exit or currupt!");
		LOG_LEAVEFN("CUserAgent::ReadURLUserAgentProfileL() - repository not instantiated");
		User::Leave(KErrNotReady);
	}

	/* ---------------- Read URL to 2G UA profile ---------------- */
	if(!iUAProfURL)
	{
		iUAProfURL = HBufC::NewL(KMaxUserAgentStringLength);
	}
	
	TPtr TempPtr2G = iUAProfURL->Des();
	TInt RetCode = iRepository->Get(EURLUAProfile, TempPtr2G);
	if(RetCode != KErrNone)
	{
		LOG_WRITE("Error: Reading URL to 2G UA profile from cenrep failed!");
		LOG_LEAVEFN("CUserAgent::ReadURLUserAgentProfileL()");
		User::Leave(RetCode);
	}
	

	/* ---------------- Read URL to 3G UA profile ---------------- */
	if(!iUAProfURL3G)
	{
		iUAProfURL3G = HBufC::NewL(KMaxUserAgentStringLength);
	}
	
	TPtr TempPtr3G = iUAProfURL3G->Des();
	RetCode = iRepository->Get(EURL3GUAProfile, TempPtr3G);
	if(RetCode != KErrNone)
	{
		delete iUAProfURL;
		iUAProfURL = NULL;
		
		LOG_WRITE("Error: Reading URL to 3G UA profile from cenrep failed!");
		LOG_LEAVEFN("CUserAgent::ReadURLUserAgentProfileL()");
		User::Leave(RetCode);
	}	

    iURLUAProfQueried = TRUE;
    LOG_LEAVEFN("CUserAgent::ReadURLUserAgentProfileL()");
    User::Leave(KErrNone);
}


#ifdef BRDO_BROWSER_50_FF

// ---------------------------------------------------------
// CUserAgent::GetFragmentsL()
// Gets all required fragments from OSExt
// ---------------------------------------------------------
void CUserAgent::GetFragmentsFromOSExtL()
	{
    RFs fs;
	CleanupClosePushL< RFs >( fs );
	User::LeaveIfError( fs.Connect() );

	if(iSymbianVersion.Length() <= 0)
		{
    	GetOSExtFragment(ESymbianOSVersion, iSymbianVersion, fs);
		}
		
	if(iS60Version.Length() <= 0)
		{
		GetOSExtFragment(ES60Version, iS60Version, fs);
		}
		
	if(iHardwareType.Length() <= 0)
		{
    	GetOSExtFragment(EHardwareType, iHardwareType, fs);
		}
	CleanupStack::PopAndDestroy(); // fs
	}


// ---------------------------------------------------------
// CUserAgent::GetOSExtFragment()
// Gets various fragments from OS Ext APIs
// ---------------------------------------------------------
TInt CUserAgent::GetOSExtFragment(TUserAgentStringKeys aFragmentType, TDes& fragmentValue, RFs& fs)
	{
	TInt retCode(0);
	
	switch(aFragmentType)
		{
		case ESymbianOSVersion:
			retCode = GetOSVersion( fragmentValue,fs );
			break;

		case ES60Version:
			retCode = GetS60Version( fragmentValue,fs );
			break;

		case EHardwareType:
			retCode = GetHardwareVersion( fragmentValue,fs );
			break;

		default:
			retCode = KErrNotFound;
			break;
		}
	if(fragmentValue.Length() <= 0)
		{
		retCode = KErrNotReady;
		}

	return retCode;
	}

// ---------------------------------------------------------
// CUserAgent::GetOSVersion()
// Gets the Symbian OS Version 
// ---------------------------------------------------------
TInt CUserAgent::GetOSVersion(TDes& osVersion,RFs& aFs )
    {
    LOG_ENTERFN("CUserAgent::GetOSVersion");

	SysVersionInfo::TSymbianOSVersion os;

	TInt retCode (SysVersionInfo::GetVersionInfo( os, aFs ));
	if(retCode != KErrNone)
	    {
		LOG_LEAVEFN("CUserAgent::GetOSVersion");
		return retCode;
	    }

	osVersion.Append(KPlaceHolderSymbianOS);
	osVersion.Append(KPlaceHolderSlash);
	osVersion.AppendNum(os.iMajorVersion);
	osVersion.Append('.');
	osVersion.AppendNum(os.iMinorVersion);
	osVersion.Append(';');

	LOG_LEAVEFN("CUserAgent::GetOSVersion");	
	return retCode;
    }

// ---------------------------------------------------------
// CUserAgent::GetS60Version()
// Gets the S60 Version 
// ---------------------------------------------------------
TInt CUserAgent::GetS60Version(TDes& s60Version, RFs& aFs)
	{
	VersionInfo::TPlatformVersion pVersion;

	TInt retCode (VersionInfo::GetVersion(pVersion,aFs));
	if(retCode != KErrNone)
		{
		return retCode;
		}
	s60Version.Append(KPlaceHolderSeries60);
	s60Version.Append(KPlaceHolderSlash);
	s60Version.AppendNum(pVersion.iMajorVersion);
	s60Version.Append('.');
	s60Version.AppendNum(pVersion.iMinorVersion);
	return retCode;
	}


// -----------------------------------------------------------------------------
// CUserAgent::GetHardwareVersion()
// Gets the Hardware Version including Manufacture, Model names with revision
// -------------------------------------------------------------------------------

TInt CUserAgent::GetHardwareVersion( TDes& aHardwareType, RFs& aFs )
	{
	SysVersionInfo::TProductVersion pVersion;

	TInt retCode(SysVersionInfo::GetVersionInfo( pVersion,aFs ));
	if(retCode != KErrNone)
		{
		return retCode;
		}
	aHardwareType.Append(pVersion.iManufacturer);
	aHardwareType.Append(pVersion.iModel);
	aHardwareType.Append(KPlaceHolderSlash);
	aHardwareType.Append(pVersion.iProduct);
	aHardwareType.Append(pVersion.iRevision);	
	aHardwareType.Append(KPlaceHolderSemiColon);

	return retCode;	
}

// ------------------------------------------------------------------------
// CUserAgent::GetVersion()
// This method gets the requested version information as string.
// @param aType A version information type specified by TVersionInfoType.
// @param aValue Stores version information.
// @return System wide error code.
// This api is implemented for future use, not used currently
// -------------------------------------------------------------------------
TInt CUserAgent::GetVersion(const SysVersionInfo::TVersionInfoType aType, TDes& aValue)
	{
	aValue.Zero();
	
	/* To get the firmware version that is composed of the version info of multiple images (core + variants images),        
    To get the customer software version. This identifies the user friendly version of e.g. operator/country/regional variant image.
    or To get the model version depending on aType EFWVersion/EOPVersion/EModelVersion.*/	

    TInt ret( SysVersionInfo::GetVersionInfo( aType , aValue) );
    //ret will return KErrNotSupported if osext extended api failed.  
       
    return ret;
	}

#endif  //#ifdef BRDO_BROWSER_50_FF

// -----------------------------------------------------------------------------
// CUserAgent::BrowserNameAndVersionL()
// Returns the Browser version
// -------------------------------------------------------------------------------

EXPORT_C HBufC* CUserAgent::BrowserVersionL() 
	{
	return iBrowserVersionStr;
	}

// -----------------------------------------------------------------------------
// CUserAgent::BrowserNameAndVersionL()
// Returns the Browser name and version
// -------------------------------------------------------------------------------

EXPORT_C HBufC* CUserAgent::BrowserNameAndVersionL() 
	{
	return iBrowserNameAndVersionStr;
	}


// -----------------------------------------------------------------------------
// CUserAgent::GetBrowserVersionL()
// Gets the Browser Version including major version, minor version and SVN rev
// -------------------------------------------------------------------------------
void CUserAgent::GetBrowserVersionL()
{

    TInt ret(0);
    TBuf<64> BrowserVersionMajor(0);
    TBuf<64> BrowserVersionMinor(0);
    TBuf<64> BrowserVersionSVNRev(0);
    
	iBrowserVersionStr = HBufC::NewL(KMaxBrowserVersionStringLength);
	
	TPtr BrowserVersionPtr = iBrowserVersionStr->Des();

   
    ret = iRepository->Get(KWebUtilsBrowserVersionMajor, BrowserVersionMajor);
    if(ret == KErrNone)
    	{   
	    ret = iRepository->Get(KWebUtilsBrowserVersionMinor, BrowserVersionMinor);
	    if(ret == KErrNone)
	    	{
			    ret = iRepository->Get(KWebUtilsBrowserVersionSVNRev, BrowserVersionSVNRev);
			    if(ret == KErrNone)
			    {
			   	BrowserVersionPtr.Append(BrowserVersionMajor);
    			BrowserVersionPtr.Append(KPlaceHolderPeriod);    
    	       	BrowserVersionPtr.Append(BrowserVersionMinor);  	
    			BrowserVersionPtr.Append(KPlaceHolderPeriod);  
    	    	BrowserVersionPtr.Append(BrowserVersionSVNRev); 	;	   		
			    }
	    	}
	    }
	    
	if(ret != KErrNone)
	{		
		LOG_WRITE("Error: Reading default Browser Version from cenrep failed!");
		LOG_LEAVEFN("CUserAgent::BrowserVersion()");
	}
}

// -----------------------------------------------------------------------------
// CUserAgent::GetBrowserNameAndVersionL()
// Gets the Browser Version including the Browser name, major version, minor version, SVN rev
// -------------------------------------------------------------------------------
void CUserAgent::GetBrowserNameAndVersionL()
{
  TInt ret(0);
  TBuf<64> BrowserName(0);
	TInt length = KMaxBrowserVersionStringLength  + KMaxBrowserNameLength;
	iBrowserNameAndVersionStr = HBufC::NewL( length);
	
	ret = iRepository->Get(KWebUtilsBrowserName, BrowserName);
	if(ret != KErrNone)
	{		
		LOG_WRITE("Error: Reading default Browser Name from cenrep failed!");
		iBrowserNameAndVersionStr->Des().Append(KPlaceHolderBrowserName);	
	}
	else
	{
		iBrowserNameAndVersionStr->Des().Append(BrowserName);		
	}
	iBrowserNameAndVersionStr->Des().Append(KPlaceHolderSlash);
	iBrowserNameAndVersionStr->Des().Append( *iBrowserVersionStr );
}

// ---------------------------------------------------------
// CUserAgent::GetFragmentsL()
// Gets all required fragments from OSExt APIs/CenRep
// ---------------------------------------------------------
void CUserAgent::GetMMSFragmentsL()
    {
    if(iMMSHardwareType.Length() <= 0)
        {
        LOG_WRITE("Reading HardwareVersion from cenrep.");
        iRepository->Get(KWebUtilsMMSUsrAg2, iMMSHardwareType);
        if(iMMSHardwareType.Length() > 0)
            {       iMMSHardwareType.Append(KPlaceHolderSemiColon); 
            }
        }
    if(iMMSS60Version.Length() <= 0)
        {
        LOG_WRITE("Reading S60Version from cenrep.");
        iRepository->Get(KWebUtilsMMSUsrAg3, iMMSS60Version);
        }
    if(iMMSMIDPVersion.Length() <= 0)
        {
        LOG_WRITE("Reading MIDPCLDCVersion from cenrep.");
        iRepository->Get(KWebUtilsMMSUsrAg4, iMMSMIDPVersion);
        }
    if(iMMSCLDCVersion.Length() <= 0)
        {
        LOG_WRITE("Reading MIDPCLDCVersion from cenrep.");
        iRepository->Get(KWebUtilsMMSUsrAg5, iMMSCLDCVersion);
        }
    //Only for 5.0 and above as 3.2 does not support OSExt APIs.
    #ifdef BRDO_BROWSER_50_FF
        if(iMMSS60Version.Length() <= 0 || iMMSHardwareType.Length() <= 0)
        {
            LOG_WRITE("Read from CenRep Failed... trying osext extended api");
            GetFragmentsFromOSExtL();
            iMMSHardwareType = iHardwareType;
            iMMSS60Version = iS60Version;
        }
    #endif
    }
// End of file
