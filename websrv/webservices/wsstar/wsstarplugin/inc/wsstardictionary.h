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
* Description:    Header declaration
*
*/











#ifndef WSSTAR_DICTIONARY_H
#define WSSTAR_DICTIONARY_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "sensessionhandler.h"
#include "wsstarservicesession.h"

#include "seninternalserviceconsumer.h"
#include "SenXmlElement.h"
#include "sentransportbase.h"


class CWSStarDictHttpClient;
class CZlibDictionary;
class CDictXml;

#if !defined( EKA2 ) && !defined( RD_SECURE_PRIV_DATA )
    _LIT(KPrivateFolder,        "C:\\system\\data\\");
#endif

/**
 * Class:       
 *
 */
class CWSStarDictionary : public CBase
    {

    public:


       /**
        * Function:    NewL
        *
        * Description: Create instance of concrete implementation. Note that ECOM
        * Returns:    Instance of this class.
        */
        static CWSStarDictionary* NewL(CSIF& aSIF, RFileLogger& aLogger);

        /**
        */
        TInt InvokeL(MSenHandlerContext* iHndlrCtx, CSenWSDescription* aSD);
        TInt LoadDictionariesL();
//        TInt LoadDictionariesL(CSenElement*  aPolicy);
  		void DataTrafficDetails( TSenDataTrafficDetails& aDetails); 		      
        ~CWSStarDictionary();
        RFileLogger* Log() const;
            
    protected:
        /**
         * Function:   CWSStarDictionary
         *
         * Discussion: Perform the first phase of two phase construction
         */
        CWSStarDictionary(CSIF& aSIF, RFileLogger& aLogger);
        
        /**
         * Function:   ConstructL
         *
         * Discussion: Perform the second phase construction of a
         *             CImplementationClassPlus object.
         */
        void ConstructL();

    private:
        void CreateZlibL(const TDesC8* name, CSenElement* aDict);
        TInt GetHttpDictionaryL(const TDesC8& aEndpoint, MSenHandlerContext* iHndlrCtx);
        void SaveFileL(const TDesC8& aUri, const TDesC8& aContents);
        TInt SaveL(TFileName aFileName, const TDesC8& aContents);
        TInt LoadDictionariesL(CSenElement*  aPolicy);
        void LoadFileL(CZlibDictionary* aDict);
        TInt SaveConfigurationL();
        TBool IfDictionaryExist(CZlibDictionary* aDict);
        void CreateMSZlibL(const TDesC8* name, CSenElement* aDict);
        CZlibDictionary* Dictionary(const TDesC8& aName);
        
        TPtrC8 WriteToBufL(CBufBase& aBuf);
        HBufC8* WriteAllAsXMlL();
        TInt WriteL(RWriteStream& aWriteStream);
        


        RPointerArray<CZlibDictionary> iDictionaires;
        CSIF* iSIF;
        HBufC* iSavePath;
        TUint32 iIapId;
        RFileLogger& iLog;                      //not owned
	    TSenDataTrafficDetails iDictDetails;    
    };
    
class CZlibDictionary : public CBase
{
    public:
    static CZlibDictionary* NewL( const TDesC8& aDictUri, 
                                  const TDesC8* aDictName, 
                                  const TDesC8* aDictType, 
                                  const TDesC8* aDictCheckSum );
                                
     static CZlibDictionary* NewLC( const TDesC8& aDictUri, 
                                    const TDesC8* aDictName, 
                                    const TDesC8* aDictType, 
                                    const TDesC8* aDictCheckSum );
    
    virtual ~CZlibDictionary();
    
    TBool  Verified();
    TPtrC8  DictUri();
    TPtrC8  DictName();
    TPtrC8  DictType();
    TPtrC8  DictContents();    
    TUint32 DictCheckSum();
    TPtrC8 DictCheckSumPtr();
    
    TInt ResetL(CZlibDictionary* aDict);
    
    TInt SetDictContentsL(const TDesC8& aDictContents);
    TInt DeleteDictContents();
    
private:
    TInt VerifyContentsL(const TDesC8& aDictContents);
    CZlibDictionary();
    void ConstructL( const TDesC8& aDictUri, 
                     const TDesC8* aDictName, 
                     const TDesC8* aDictType, 
                     const TDesC8* aDictCheckSum );
private:
    
    HBufC8* iDictUri; //uri to download
    HBufC8* iDictName; //name or mws:Id tag
    HBufC8* iDictType; //zlib or mws:type
    HBufC8* iDictContents; //contents
    HBufC8* iDictCheckSum; //provided checksum
    TUint32 iChkSum; //chksum in integer form
    
    TBool iVerfied;
}; 

class CDictXml :public CSenDomFragment
{
public:    
    static CDictXml* NewL();
    static CDictXml* NewLC();
    ~CDictXml();
    
    TInt ReadFileL(TFileName aPath);
    CSenElement* XMLDocL();
private:

    CDictXml();
    void ConstructL();
    TInt count;
    TBool iParsed;
};   
#endif // WSSTAR_DICTIONARY_H



