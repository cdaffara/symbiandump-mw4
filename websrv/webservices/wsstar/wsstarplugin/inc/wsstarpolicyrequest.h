/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/






#include <e32std.h>
#include <flogger.h>
#include "sensessionhandler.h"
#include "wsstarhandlercontext.h"
#include "wsstarsessioncontext.h"
#include "wsstartrustclient.h"
#include "wsstarcons.h"
#include "wspolicy.h"
#include "wspolicyregistry.h"

class CPolicyReq;

class CPolicyRequest : public CBase
{

public:

    static CPolicyRequest* NewL(const TDesC8& aContract);
     static CPolicyRequest* NewLC(const TDesC8& aContract);
    virtual ~CPolicyRequest();

    TPtrC8  Contract();
    CSenWSDescription* ServiceDescription();
    CSenElement* PolicyL();	//codescannerwarnings
    CSenElement* PolicyL(const TDesC8& aUri);	//codescannerwarnings
    CSenElement* OriginalPolicyL(const TDesC8& aUri);	//codescannerwarnings
    
    TInt AddMetadataUriL(const TDesC8& aUri);
    TInt AddMetadataUriL(const TDesC8& aUri, const TDesC8& aName);
    TInt AddMetadataUriL(const TDesC8& aUri, const TDesC8& aName, const TDesC8& aContents );
    
    TInt SetContentsL(const TDesC8& aUri, const TDesC8& aContent);
    TInt SetPolicyL(const TDesC8& aUri, CWSPolicy* aPolicy);
    void URIs(RArray<TPtrC8>& aURI);
        
    TInt SetSD(CSenWSDescription* aSD);    
    
private:
    TInt LookForUri(const TDesC8& aUri);

    CPolicyRequest();
    void ConstructL(const TDesC8& aContract);

    CSenWSDescription* iSD; //not owned        
    HBufC8* iContract; //service contract representing the policy    
    RPointerArray<CPolicyReq> iPolicies;
};  
  
class CPolicyReq : public CBase
{
    public:
    
    static CPolicyReq* NewL(const TDesC8& aUri);
    static CPolicyReq* NewLC(const TDesC8& aUri);
    virtual ~CPolicyReq();


    TPtrC8  Name();
    TPtrC8  URI();
    TPtrC8  Contents();
    CWSPolicy* Policy();

    TInt    SetNameL(const TDesC8& aName);
    TInt    SetContentsL(const TDesC8& aContent);
    TInt    SetPolicy(CWSPolicy* aPolicy);
    
    private:
    CPolicyReq();
    void ConstructL(const TDesC8& aUri);
    
    private:
    CWSPolicy* iPolicy;
    HBufC8* iName; //policy name
    HBufC8* iURI; //uri to download
    HBufC8* iContents; //contents
    
    
    
};    
class CPolicyReqMap : public CBase
{

public:
    static CPolicyReqMap* NewL(const TDesC8& aContract);
    static CPolicyReqMap* NewLC(const TDesC8& aContract);
    virtual ~CPolicyReqMap();


    TPtrC8  Contract();
    TPtrC8  Uri();
    TPtrC8  Contents();
    CSenWSDescription*    ServiceDescription();

    TInt    SetUriL(const TDesC8& aUri);
    TInt    SetContentsL(const TDesC8& aContent);
    TInt    SetSD(CSenWSDescription* aSD);
    
private:
    CPolicyReqMap();
    void ConstructL(const TDesC8& aUri);
    
private:
    HBufC8* iContract; //service contract
    HBufC8* iURI; //uri to download
    HBufC8* iContents; //contents
    CSenWSDescription* iSD; //not owned
};    
