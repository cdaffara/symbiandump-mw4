/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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










#include <e32std.h>
#include <utf.h>
#include <sysutil.h>
#include <ezcompressor.h>
#include "wsstardictionary.h"
#include "wsstarhttpclient.h"
#include "wsstarpolicy.h"
#include "sendebug.h"
#include "senxmldebug.h"
#include "senlogger.h"

using namespace WSPolicy;
namespace
{
    _LIT8(KHttp, "http");
    _LIT(KDictionaryFile, "dictionary.xml");
    _LIT8(KDictionaryType, "Type");
    _LIT8(KDictionaryId, "Id");
    _LIT8(KDictionaryChkSum, "Chksum");
    // helper methods          
    /*
    void TUint32ToDes(TDes8& aResult, TUint32 aData)
        {
        TUint32 part1= aData & 0xff000000 ;
        part1 = part1 >> 24;
        TUint32 part2= aData & 0x00ff0000;
        part2 = part2 >> 16;
        TUint16 part3= aData & 0x0000ff00;
        part3 = part3 >> 8;
        TUint16 part4= aData & 0x000000ff;
        
        TUint8 p1(part1);
        TUint8 p2(part2);
        TUint8 p3(part3);
        TUint8 p4(part4);
        
        aResult.Append(p1);
        aResult.Append(p2);
        aResult.Append(p3);
        aResult.Append(p4);
        }
    */
    
    /*
    TUint32 DesToTUint32(const TDesC8& aData)
        {
        TUint32 part1 = aData[0];
        part1 = part1-48;
        part1 = part1 << 24;
        TUint32 part2 = aData[1];
        part2 = part2-48;
        part2 = part2 << 16;
        TUint32 part3 = aData[2];
        part3 = part3-48;
        part3 = part3 << 8;
        TUint32 part4 = aData[3];
        part4 = part4-48;
       
        TUint32 ret = part1+part2+part3+part4;
        return ret;
        }
    */      
}
//---------------------------------------------------------------------------
// Create instance of this class
//---------------------------------------------------------------------------
//
CWSStarDictionary* CWSStarDictionary::NewL(CSIF& aSIF, RFileLogger& aLogger)
    {
    
    CWSStarDictionary* self   = new (ELeave) CWSStarDictionary(aSIF, aLogger);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
//
CWSStarDictionary::CWSStarDictionary(CSIF& aSIF, RFileLogger& aLogger) 
:iSIF(&aSIF), iLog(aLogger)
    {
    }

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
//
CWSStarDictionary::~CWSStarDictionary()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarDictionary::~CWSStarDictionary()")));
    iDictionaires.ResetAndDestroy();
    delete iSavePath;
    }

//---------------------------------------------------------------------------
// void CWSStarDictionary::ConstructL()
//---------------------------------------------------------------------------
//
void CWSStarDictionary::ConstructL()
    {   
     TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarDictionary::ConstructL()")));

#if !defined( EKA2 ) && !defined( RD_SECURE_PRIV_DATA )
        iSavePath = KPrivateFolder().AllocL();
#else
        TBuf<KMaxPath> privatePath;
        RFs fss;
            User::LeaveIfError(fss.Connect());
            CleanupClosePushL(fss);
        fss.CreatePrivatePath(EDriveC);
        fss.PrivatePath(privatePath);
        iSavePath = privatePath.AllocL();
        CleanupStack::PopAndDestroy(1);
#endif
    }
CZlibDictionary* CWSStarDictionary::Dictionary(const TDesC8& aName)
{
 
    TInt count(iDictionaires.Count());
    for (TInt i = 0; i< count; i++)
    {
        CZlibDictionary* tempDict = iDictionaires[i];
        if(tempDict->DictName() == aName)
        {
          return tempDict;  
        }
    }

    return NULL;
}    

TBool CWSStarDictionary::IfDictionaryExist(CZlibDictionary* aDict)
{
 
    TInt count(iDictionaires.Count());
    for (TInt i = 0; i< count; i++)
    {
        CZlibDictionary* tempDict = iDictionaires[i];
        if(tempDict->DictName() == aDict->DictName() &&  
        tempDict->DictCheckSumPtr() == aDict->DictCheckSumPtr()
        ) //Id and checksum is same, its same dictionary
        {
          return ETrue;  
        }
    }
    
    return EFalse;
}

//---------------------------------------------------------------------------
// void CWSStarDictionary::CreateZlibL
// create a CZlibDictionary object from given dictionary inforamtion
//---------------------------------------------------------------------------
//
void CWSStarDictionary::CreateMSZlibL(const TDesC8* name, CSenElement* aDict)
{

    const TDesC8* type = aDict->AttrValue(KMwsType);
    const TDesC8* checksum = aDict->AttrValue(KMwsId);

   if(aDict->HasContent())
    {
        TPtrC8 uri = aDict->Content();
            
        if(name && type && checksum && uri!= KNullDesC8())
        {

            CZlibDictionary* dict = CZlibDictionary::NewLC(uri, name, type, checksum);
            if(dict)
            {
                if(!IfDictionaryExist(dict))
                {
                    iDictionaires.Append(dict);    
                    CleanupStack::Pop( dict );                    
                }
                else //exist
                {
//                Reset only when we know that the uri has been changed
               
                    CZlibDictionary* tempDict = Dictionary(dict->DictName());
                    if(tempDict)                     
                    {
                        if( tempDict->DictUri() == dict->DictUri()
//                         && tempDict->DictType() == dict->DictType()
                            )
                        {
                            //do nuthhing
                        }
                        else
                        {
                            tempDict->ResetL(dict);
                        }
                    }
                    CleanupStack::PopAndDestroy( dict );
                }
            }
            else
            {
                CleanupStack::PopAndDestroy( dict );
            }
        }
        
    }    
    
}

void CWSStarDictionary::CreateZlibL(const TDesC8* name, CSenElement* aDict)
{
    if(!name || ! aDict)
        return;

    const TDesC8* type = aDict->AttrValue(KDictionaryType);
    const TDesC8* checksum = aDict->AttrValue(KDictionaryChkSum);

   if(aDict->HasContent())
    {
        TPtrC8 uri = aDict->Content();
            
        if(name && type && checksum && uri!= KNullDesC8())
        {

            CZlibDictionary* dict = CZlibDictionary::NewLC(uri, name, type, checksum);
            if(dict && !IfDictionaryExist(dict))
            {
                iDictionaires.Append(dict);
                CleanupStack::Pop( dict );
                
            }
            else
            {
                CleanupStack::PopAndDestroy( dict );
            }
        }
        
    }    
    
}

//---------------------------------------------------------------------------
// TInt CWSStarDictionary::InvokeL
// This method invoke the Http download of dictionaries. The dictionaries will 
// be downloaded adn saves to private folder of the serene
//---------------------------------------------------------------------------
//
TInt CWSStarDictionary::InvokeL(MSenHandlerContext* iHndlrCtx, CSenWSDescription* aSD)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarDictionary::InvokeL(MSenMessageContext& aCtx)")));
    TInt error(KErrNotFound);
//1..load dictionary.xml file
//2..load all of dictionaries and verify it
    LoadDictionariesL();


//3..get dictionary element from policy
//only sts policy will be considered at this point
   CSenElement* ret = CWSStarPolicy::GetElementL(KDictionaries, KNullDesC8(), *iHndlrCtx, aSD);
    if(ret)
    {
        RPointerArray<CSenElement>& children = ret->ElementsL();
        TInt count = children.Count();
        for (TInt i =0; i < count; i++)
        {
            CSenElement* ele = children[i];
            if(ele)
            {
                const TDesC8* name = ele->AttrValue(KMwsId);
                RPointerArray<CSenElement> dicts;
                CleanupClosePushL(dicts);
                ele->ElementsL(dicts, KDictionary);

                TInt count2 = dicts.Count();
                for (TInt j =0; j < count2; j++)
                {
                    CSenElement* dictEle = dicts[j];
                    if(dictEle)
                    {
                       CreateMSZlibL(name,dictEle);
                       
                    }
                    
                }
                CleanupStack::PopAndDestroy(); //dicts
                    
             }

        }

    }
//4..start putting dictionaries in xml file

        
 //5.. now use http to get all invalid dictionaries from iDictionaries
   
    TInt count(iDictionaires.Count());
    TPtrC8 finalUri(KHttp);
    if(finalUri != KNullDesC8)
        {
        TRAP(error, GetHttpDictionaryL(finalUri, iHndlrCtx));
        }
    else 
        {
        error = KErrPathNotFound;
        }

//6..write iDictionaries to xml file
    SaveConfigurationL();
    iDictionaires.ResetAndDestroy(); //we dont need it any more
    return error;
    }

//---------------------------------------------------------------------------
// TInt CWSStarDictionary::SaveL
// Saves a dictionary to a file
//---------------------------------------------------------------------------
//
TInt CWSStarDictionary::SaveL(TFileName aFileName, const TDesC8& aContents)
{
    if (aFileName.Length() < 1 || aContents.Length() < 1)
        return KErrArgument;
    RFs fss;
    User::LeaveIfError(fss.Connect());
    CleanupClosePushL(fss);

    TInt size = aContents.Length();

    TInt retVal(SysUtil::FFSSpaceBelowCriticalLevelL(&fss, size));
    if(retVal == KErrNone)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"File system check: OK, there is enough space available.");
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"File system check: FAILED, NOT enough space available.");
        CleanupStack::PopAndDestroy(); //fss
        User::Leave(retVal);
        }
        
        RFile  dictFile;
        CleanupClosePushL(dictFile);

        retVal = dictFile.Replace(fss , aFileName, EFileWrite );
        if(retVal == KErrNone)
            {
            retVal = dictFile.Write(aContents);        
            }

       
       CleanupStack::PopAndDestroy(2); //fss, dictFile
       return retVal;

}
//---------------------------------------------------------------------------
// CWSStarDictionary::SaveFileL
// Saves a dictionary to a file
//---------------------------------------------------------------------------
//

void CWSStarDictionary::SaveFileL(const TDesC8& aUri, const TDesC8& aContents)
{
    TFileName filepath(iSavePath->Des());

    TInt loc = aUri.LocateReverse('/');
    if(loc != KErrNotFound)
    {
        TPtrC8 name = aUri.Right(aUri.Length() - loc - 1);
        HBufC* nameBuf = HBufC::NewLC(name.Length());
        TPtr nameBufPtr(nameBuf->Des());
        TInt ret = CnvUtfConverter::ConvertToUnicodeFromUtf8(nameBufPtr, name);        
        if(ret == KErrNone)
        {
            TFileName filename; 
            filename.Append(nameBufPtr);
            filepath.Append(filename);
            ret = SaveL(filepath, aContents);
        }
        CleanupStack::PopAndDestroy(nameBuf);
    }
}
//---------------------------------------------------------------------------
// TInt CWSStarDictionary::GetHttpDictionaryL
//Method to download the dictionaries by HTTP
//---------------------------------------------------------------------------
//

TInt CWSStarDictionary::GetHttpDictionaryL( const TDesC8& aEndpoint, MSenHandlerContext* aHndlrCtx )
    {
    if( !aHndlrCtx )
        {
        return KErrArgument;
        }    
//find the service description from handlercontext i.e. STS service        
    TPtrC8 contract(KWSStarSTSContract);
    RPointerArray<CSenWSDescription> serviceDescriptions;
    CleanupClosePushL(serviceDescriptions);
    TInt retVal(KErrNone);
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, retVal = aHndlrCtx->GetSenCoreServiceManager()->ServiceDescriptionsL(serviceDescriptions, contract));
    CSenWSDescription* pSD = NULL;   
    
    if(leaveCode == KErrNone && retVal == KErrNone && serviceDescriptions.Count() > 0)
        {
         
        TUint32 iap(0);
        for(TInt j = 0; j < serviceDescriptions.Count(); j++)
            {
            pSD = serviceDescriptions[j];
            if( pSD )
                {
                TInt getIapRetCode = pSD->IapId(iap);    
                if( getIapRetCode == KErrNone )
                    {
                    pSD = serviceDescriptions[j]; // value IAP property over others (temp solution)
                    break;
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( );  // serviceDescriptions.Close()

    CSenWSDescription* pInitializer = CSenWSDescription::NewLC();
    pInitializer->SetEndPointL(aEndpoint);

    CWSStarHttpClient* pHttpClient = CWSStarHttpClient::NewLC(*(CSIF*)aHndlrCtx->GetAnyL(HandlerContextKey::KSIF()), pInitializer, *Log()); //*(CSIF*)iSenHandlerContext.GetAny(HandlerContextKey::KSIF()), pInitializer, *Log());//codescannerwarnings
        
    // Fix the first URI and request part of the request
    TInt count( iDictionaires.Count() );
    for (TInt i = 0; i< count; i++)
        {
        CZlibDictionary* tempDict = iDictionaires[i];   
        if(!tempDict->Verified())
            {
        
            TPtrC8 endpoint(tempDict->DictUri());        
            TPtrC8 request(endpoint.Right(endpoint.Length() - aEndpoint.Length()));        

            HBufC8* pResponse = NULL;
            TInt leaveCode(KErrNone);
            TInt submitRetCode(KErrNone);

            TRAP( leaveCode, submitRetCode = pHttpClient->GetHttpL( request, pResponse, pSD ); )
            pHttpClient->DataTrafficDetails(iDictDetails);
            if( submitRetCode && retVal == KErrNone )
                {
                // Store the error code from the 1st failing request 
                retVal = submitRetCode;
                }    
            if( leaveCode && retVal == KErrNone )
                {
                retVal = leaveCode;
                }    
            if(pResponse && pResponse->Length() > 0)
                {
                CleanupStack::PushL(pResponse);            
                TPtrC8 resPtr(pResponse->Des());
                if(tempDict->SetDictContentsL(resPtr) == KErrNone)
                {
                    SaveFileL(endpoint, resPtr);
                    
                }
                CleanupStack::PopAndDestroy();
                }    
            else // if(retVal == 404)    
                {
                delete pResponse;
                }
            }
        } // for each request
    CleanupStack::PopAndDestroy(pHttpClient); 
    CleanupStack::PopAndDestroy(pInitializer);  
    return retVal;
}

TInt CWSStarDictionary::LoadDictionariesL()
{
    
    TFileName dictXml(iSavePath->Des());
    dictXml.Append(KDictionaryFile);
    
    CDictXml* dict= CDictXml::NewL();
    CleanupStack::PushL(dict);
    TInt fileError = dict->ReadFileL(dictXml);
    if(fileError == KErrNone)
    {
        CSenElement* dictEle = dict->XMLDocL();
        if(dictEle)
        {
            LoadDictionariesL(dictEle);
        }
    
    }
    CleanupStack::PopAndDestroy(dict);
    return fileError;    
}
//---------------------------------------------------------------------------
// void CWSStarDictionary::LoadFileL
// This method Loads the dictionaries from the files present in private folder.
//---------------------------------------------------------------------------
//
void CWSStarDictionary::LoadFileL(CZlibDictionary* aDict)
{
    TFileName filepath(iSavePath->Des());
    
    TPtrC8 uri = aDict->DictUri();
    TInt loc = uri.LocateReverse('/');
    
    if(loc != KErrNotFound)
    {
        TPtrC8 name = uri.Right(uri.Length() - loc - 1);
        HBufC* nameBuf = HBufC::NewLC(name.Length());
        TPtr nameBufPtr(nameBuf->Des());
        TInt ret = CnvUtfConverter::ConvertToUnicodeFromUtf8(nameBufPtr, name);        
        if(ret == KErrNone)
        {
            TFileName filename; 
            filename.Append(nameBufPtr);
            filepath.Append(filename);
            {
                RFs fss;
                User::LeaveIfError(fss.Connect());
                CleanupClosePushL(fss);
                
                RFile  dictFile;
                TInt error = dictFile.Open(fss , filepath, EFileRead );
                CleanupClosePushL(dictFile);                
                if(error == KErrNone)
                {
                    TInt size;
                    dictFile.Size(size);
                    if(size > 0)
                    {
                        HBufC8 *iBuf=HBufC8::NewLC(size);
                        TPtr8 buf8ptr(iBuf->Des());
                        dictFile.Read(buf8ptr,size);
                        aDict->SetDictContentsL(buf8ptr);
                        aDict->DeleteDictContents();
                        CleanupStack::PopAndDestroy(iBuf);
                        
                    }
                }
                CleanupStack::PopAndDestroy(2); //fss, dictFile
            }
        }
        CleanupStack::PopAndDestroy(nameBuf);
    }
}

//---------------------------------------------------------------------------
// void CWSStarDictionary::LoadDictionariesLs
// This method Loads the dictionaries from the private folder. It reads the 
// policy file for the possible dictionaries and load those dictionaries from 
// priavte folder.
//---------------------------------------------------------------------------
//
TInt CWSStarDictionary::LoadDictionariesL(CSenElement*  aPolicy)
{
    if(aPolicy)
    {
        RPointerArray<CSenElement>& children = aPolicy->ElementsL();
        TInt count = children.Count();
        for (TInt i =0; i < count; i++)
        {
            CSenElement* ele = children[i];
            if(ele)
            {
                const TDesC8* name = ele->AttrValue(KDictionaryId);
                CreateZlibL(name,ele);
                       
            }
        }
        
    }

    TInt count(iDictionaires.Count());
    for (TInt i = 0; i< count; i++)
    {
        CZlibDictionary* tempDict = iDictionaires[i];
        if(tempDict)
            LoadFileL(tempDict);
    }

    return KErrNone;
}
TInt CWSStarDictionary::SaveConfigurationL()
{
    TInt error(KErrNone);
    HBufC8* ret = WriteAllAsXMlL();
    CleanupStack::PushL(ret);
    if(ret)
    {
        TPtrC filename(KDictionaryFile);
        TPtrC8 dataptr(ret->Des());
        error = SaveL(filename, dataptr);
    }
    CleanupStack::PopAndDestroy(ret);
    return error;   
}

TPtrC8 CWSStarDictionary::WriteToBufL(CBufBase& aBuf)
    {
    RBufWriteStream bufWs(aBuf);
    CleanupClosePushL(bufWs);
    WriteL(bufWs);
    CleanupStack::PopAndDestroy(); // bufWs.Close();
    return aBuf.Ptr(0);
    }
HBufC8* CWSStarDictionary::WriteAllAsXMlL()
{
    CBufFlat *pBuf = CBufFlat::NewL(128);
    CleanupStack::PushL(pBuf);
    TPtrC8 p = WriteToBufL(*pBuf);
    HBufC8* pRet = p.AllocL();
    CleanupStack::PopAndDestroy(); // pBuf;
    return pRet;    
}
         
TInt CWSStarDictionary::WriteL(RWriteStream& aWriteStream)
{
    aWriteStream.WriteL(KSenLessThan);
    aWriteStream.WriteL(KDictionaries);
    aWriteStream.WriteL(KSenGreaterThan);
    
    TInt count(iDictionaires.Count());
    for (TInt i = 0; i< count; i++)
    {
        CZlibDictionary* tempDict = iDictionaires[i];
        TPtrC8 endpoint(tempDict->DictUri());
        TPtrC8 type(tempDict->DictType());
        TPtrC8 id(tempDict->DictName());
        TPtrC8 chksum(tempDict->DictCheckSumPtr());
          
        if(id != KNullDesC8() && endpoint != KNullDesC8() && type != KNullDesC8() && chksum != KNullDesC8)
        {
            aWriteStream.WriteL(KSenLessThan);
            aWriteStream.WriteL(KDictionary);
            aWriteStream.WriteL(KSenSpace);
            aWriteStream.WriteL(KDictionaryId);
            aWriteStream.WriteL(KSenEqualsDblQuot);
            aWriteStream.WriteL(id);
            aWriteStream.WriteL(KSenDblQuot);
            aWriteStream.WriteL(KSenSpace);            
            aWriteStream.WriteL(KDictionaryType);
            aWriteStream.WriteL(KSenEqualsDblQuot);
            aWriteStream.WriteL(type);
            aWriteStream.WriteL(KSenDblQuot);
            aWriteStream.WriteL(KSenSpace);
            aWriteStream.WriteL(KDictionaryChkSum);
            aWriteStream.WriteL(KSenEqualsDblQuot);
            aWriteStream.WriteL(chksum);
            aWriteStream.WriteL(KSenDblQuot);
            aWriteStream.WriteL(KSenSpace);
            aWriteStream.WriteL(KSenGreaterThan);
            aWriteStream.WriteL(endpoint);
            
            aWriteStream.WriteL(KSenLessThanSlash);
            aWriteStream.WriteL(KDictionary);
            aWriteStream.WriteL(KSenGreaterThan);

        }
    }

    aWriteStream.WriteL(KSenLessThanSlash);
    aWriteStream.WriteL(KDictionaries);
	aWriteStream.WriteL(KSenGreaterThan);

return KErrNone;    
}
void CWSStarDictionary::DataTrafficDetails(TSenDataTrafficDetails& aDetails) 		
	{
		aDetails = iDictDetails;
	}
//---------------------------------------------------------------------------
// CWSStarDictionary::Log()
//---------------------------------------------------------------------------
//    
RFileLogger* CWSStarDictionary::Log() const
    {
    return &iLog;
//        return ((RFileLogger*)iHandlerContext.GetAny(HandlerContextKey::KLogger));
    }


//---------------------------------------------------------------------------
// A simple Dictionary Object and its implementation
//---------------------------------------------------------------------------
//
CZlibDictionary* CZlibDictionary::NewL( const TDesC8& aDictUri, 
                                        const TDesC8* aDictName, 
                                        const TDesC8* aDictType, 
                                        const TDesC8* aDictCheckSum )
{
    CZlibDictionary* zlib   = CZlibDictionary::NewLC(aDictUri, aDictName, aDictType, aDictCheckSum);
    CleanupStack::Pop();
    return zlib;
}
                                
CZlibDictionary* CZlibDictionary::NewLC( const TDesC8& aDictUri, 
                                         const TDesC8* aDictName, 
                                         const TDesC8* aDictType, 
                                         const TDesC8* aDictCheckSum )
{

    CZlibDictionary* zlib   = new (ELeave) CZlibDictionary();
    CleanupStack::PushL (zlib);
    zlib->ConstructL(aDictUri, aDictName, aDictType, aDictCheckSum);
    return zlib;
    
}

CZlibDictionary::~CZlibDictionary()
{
    delete iDictUri; //uri to download
    delete iDictName; //name or mws:Id tag
    delete iDictType; //zlib or mws:type
    delete iDictContents; //contents
    delete iDictCheckSum; //provided checksum  
}
CZlibDictionary::CZlibDictionary()
:iVerfied(EFalse)
{
}

void CZlibDictionary::ConstructL( const TDesC8& aDictUri, 
                                  const TDesC8* aDictName,
                                  const TDesC8* aDictType, 
                                  const TDesC8* aDictCheckSum )
    {
    iDictUri = aDictUri.AllocL();
    iDictName = aDictName->AllocL(); 
    iDictType = aDictType->AllocL();
    iDictCheckSum = aDictCheckSum->AllocL();
    }
    
TBool  CZlibDictionary::Verified()
{
    return iVerfied;
}
TPtrC8  CZlibDictionary::DictUri()
{
    return iDictUri->Des();    
}
TPtrC8  CZlibDictionary::DictName()
{
    return iDictName->Des();
}
TPtrC8  CZlibDictionary::DictType()
{
    return iDictType->Des();
}
TPtrC8  CZlibDictionary::DictContents()
{
    return iDictContents->Des();
}

TUint32 CZlibDictionary::DictCheckSum()
    {
    return iChkSum;
    }

TPtrC8 CZlibDictionary::DictCheckSumPtr()
    {
    return iDictCheckSum->Des();
    }

TInt CZlibDictionary::VerifyContentsL(const TDesC8& aDictContents)
{
    TPtrC8 dictChk(iDictCheckSum->Des());
    
    TLex8 lex(dictChk);
    TUint32 givenChkSum = 0;
    TInt error = lex.Val(givenChkSum, EDecimal);

    if(error == KErrNone)
    {
        if(givenChkSum == 0)
            return KErrArgument;
        
        iChkSum = givenChkSum;
        TUint32 calc_chksum(0);
        calc_chksum = adler32(calc_chksum,0,0);		// init Adler32 checksum
        calc_chksum = adler32(calc_chksum,aDictContents.Ptr(),aDictContents.Length()); // compute Adler32 checksum
        if(givenChkSum == calc_chksum) 
        {
            return KErrNone;
        }        
    }
    return error;
}
TInt CZlibDictionary::DeleteDictContents()
{
    if(iDictContents)
    {
        delete iDictContents;
        iDictContents = NULL;
    }
    return KErrNone;      
}
TInt CZlibDictionary::SetDictContentsL(const TDesC8& aDictContents)
{
    if(aDictContents == KNullDesC8())    
        return KErrArgument;

    if (VerifyContentsL(aDictContents) == KErrNone)
    {
        iVerfied = ETrue;
        if(iDictContents)
        {
            delete iDictContents;
            iDictContents = NULL;
        }
        
        iDictContents = aDictContents.AllocL();
        return KErrNone;
    }
    
    return KErrGeneral;
}

TInt CZlibDictionary::ResetL(CZlibDictionary* aDict)
{
    delete iDictUri; 
    iDictUri = NULL;
    delete iDictName; 
    iDictName = NULL;
    delete iDictType; 
    iDictType = NULL;
    delete iDictContents;
	iDictContents = NULL;
    delete iDictCheckSum;
    iDictCheckSum = NULL;
    iDictUri = aDict->DictUri().AllocL();
    iDictName = aDict->DictName().AllocL();
    iDictType = aDict->DictType().AllocL();
    iDictCheckSum = aDict->DictCheckSumPtr().AllocL();
    iVerfied = EFalse;
    iChkSum = 0;
    return KErrNone;
}

CDictXml* CDictXml::NewL()
{
     CDictXml* pSelf = CDictXml::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
}
CDictXml* CDictXml::NewLC()
{
       CDictXml* pSelf = new (ELeave) CDictXml();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
  
}
CDictXml::~CDictXml()
{
  delete iXmlReader;
}
CDictXml::CDictXml() :iParsed(EFalse)
{
    
}
void CDictXml::ConstructL()
{
  iXmlReader = CSenXmlReader::NewL(KXmlParserMimeType); // use libxml2 sax parser
}

CSenElement* CDictXml::XMLDocL()
{
    if(iParsed)
    {
        CSenElement* ele = &AsElement();
        if(ele)
            return ele;
    }
    
    return NULL;
}
TInt CDictXml::ReadFileL(TFileName aPath)
{
      RFs fss;
       User::LeaveIfError(fss.Connect());
        CleanupClosePushL(fss);

    TFileName iFileName1(aPath);
    RFile  xmlFile;
    TInt error = xmlFile.Open(fss , iFileName1, EFileRead );
    CleanupClosePushL(xmlFile);    

    if(error == KErrNone)
    {
        error = KErrGeneral;
                
        TInt size;
        xmlFile.Size(size);

        HBufC8 *pBuf=HBufC8::NewL(size);	//CodeScannerWarnings
        CleanupStack::PushL(pBuf);
        TPtr8 buf8ptr(pBuf->Des());
        xmlFile.Read(buf8ptr,size);
        size = pBuf->Length();
        if(size > 0)
            {
            TPtr8 buf8ptr(pBuf->Des());            
            TRAP( error, ParseL(buf8ptr);)
            if (error == KErrNone)
                iParsed = ETrue;
            }
        CleanupStack::PopAndDestroy(pBuf);
    }
    CleanupStack::PopAndDestroy(2); // fss  , xmlFile  
    return error;    
} 

// END OF FILE

