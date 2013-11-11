/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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







#include "wsstarpolicyrequest.h"

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//     
CPolicyRequest* CPolicyRequest::NewL(const TDesC8& aContract)
{
    CPolicyRequest* self   = CPolicyRequest::NewLC(aContract);
    CleanupStack::Pop();
    return self;
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//                                
CPolicyRequest* CPolicyRequest::NewLC(const TDesC8& aContract)
{
    CPolicyRequest* self   = new (ELeave) CPolicyRequest();
    CleanupStack::PushL (self);
    self->ConstructL(aContract);
    return self;
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//    
CPolicyRequest::~CPolicyRequest()
{
    delete iContract; //service contract representing the policy
    iPolicies.ResetAndDestroy();
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CPolicyRequest::CPolicyRequest() :
iSD(NULL),iContract(NULL)
{
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
void CPolicyRequest::ConstructL(const TDesC8& aContract)
{
    if(aContract.Length() < 1)
        User::Leave (KErrArgument);
    
    iContract = aContract.AllocL();
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//    
TPtrC8  CPolicyRequest::Contract()
{
    if (iContract != NULL)
        return iContract->Des();
    else
        return KNullDesC8();

}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CSenWSDescription* CPolicyRequest::ServiceDescription()
{
    return iSD;    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt CPolicyRequest::SetSD(CSenWSDescription* aSD)
{
    if(aSD != NULL && aSD->Contract().Compare(iContract->Des()) == 0)
    {
        iSD = aSD; //not owned              
    }
    else
    {
        iSD = NULL;
    }
         
    return KErrNone;         
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
void CPolicyRequest::URIs(RArray<TPtrC8>& aURI)
{
    for(TInt i = 0; i <iPolicies.Count(); i++ )
    {
        CPolicyReq* req = iPolicies[i];
        if(req)
        {
            TPtrC8 uri = req->URI();    
            aURI.Append(uri);
        }
    }
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt CPolicyRequest::LookForUri( const TDesC8& aUri )
    {
    for (TInt i = 0; i< iPolicies.Count(); i++)
        {
        CPolicyReq* req = iPolicies[i];
        if (req->URI().Compare(aUri) == 0)
            {
            return i;   
            }
        }
    return KErrNotFound;
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt CPolicyRequest::AddMetadataUriL( const TDesC8& aUri )
{
    if(aUri.Length() < 1)    
        return KErrArgument;
    
    TInt indx = LookForUri(aUri);
    if(indx == KErrNotFound)
    {
        CPolicyReq* req = CPolicyReq::NewL(aUri);
        if(req)
            return iPolicies.Append(req);
    }
    else
    {
        return indx;
    }
    
    return KErrNotFound;        
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt CPolicyRequest::AddMetadataUriL( const TDesC8& aUri, const TDesC8& aName)
{

    if(aUri.Length() < 1 || aName.Length() < 1)    
        return KErrArgument;
    
    TInt indx = LookForUri(aUri);
    if(indx == KErrNotFound)
    {
        CPolicyReq* req = CPolicyReq::NewL(aUri);
        if(req)
        {
            req->SetNameL(aName);    
            return iPolicies.Append(req);
        }
    }
    else
    {
        CPolicyReq* req = iPolicies[indx];
        req->SetNameL(aName);
        return indx;
    }
    
    return KErrNotFound;
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt CPolicyRequest::AddMetadataUriL(const TDesC8& aUri, const TDesC8& aName, const TDesC8& aContents )
{

    if(aUri.Length() < 1 || aName.Length() < 1 || aContents.Length() < 1)    
        return KErrArgument;
    
    TInt indx = LookForUri(aUri);
    if(indx == KErrNotFound)
    {
        CPolicyReq* req = CPolicyReq::NewL(aUri);
        if(req)
        {
            req->SetNameL(aName);
            req->SetContentsL(aContents);
            return iPolicies.Append(req);
        }
    }
    else
    {
        CPolicyReq* req = iPolicies[indx];
        req->SetNameL(aName);
        req->SetContentsL(aContents);
        return indx;

    }
    return KErrNotFound;        
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt CPolicyRequest::SetContentsL( const TDesC8& aUri, const TDesC8& aContent )
    {
    if(aUri.Length() < 1 || aContent.Length() < 1)    
        {
        return KErrArgument;
        }
    TInt indx = LookForUri(aUri);
    if(indx != KErrNotFound)
        {
        CPolicyReq* req = iPolicies[indx];
        req->SetContentsL(aContent);
        return indx;
        }
    return KErrNotFound;    
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt CPolicyRequest::SetPolicyL( const TDesC8& aUri, CWSPolicy* aPolicy )
    {
    if(aUri.Length() < 1 || aPolicy == NULL)    
        {
        return KErrArgument;
        }
    TInt indx = LookForUri(aUri);
    if(indx != KErrNotFound)
        {
        CPolicyReq* req = iPolicies[indx];
        req->SetPolicy(aPolicy);
        return indx;
        }
    return KErrNotFound;    
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CSenElement* CPolicyRequest::PolicyL()	//codescannerwarnings
    {
    if(iPolicies.Count() > 0)
        {
        CPolicyReq* req = iPolicies[0];
         // return NULL if there was no way to fetch policy from network
        CWSPolicy* pol = req->Policy();
        if(pol)
            {
                return pol->PolicyL();	//codescannerwarning
            }
            else
            {
                return NULL;
            }
        }
    else
        {
        return NULL;
        }
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CSenElement* CPolicyRequest::PolicyL( const TDesC8& aUri )	//codescannerwarnings
    {
    if( aUri.Length() < 1 )    
        {
        return NULL;
        }
    TInt indx = LookForUri(aUri);
    if(indx != KErrNotFound)
        {
        CPolicyReq* req = iPolicies[indx];
        CWSPolicy* pol = req->Policy();
        if(pol)
            {
            return pol->PolicyL();	//codescannerwarning
            }
        }
    return NULL;
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CSenElement* CPolicyRequest::OriginalPolicyL( const TDesC8& aUri )	//codescannerwarnings
    {
    if( aUri.Length() < 1 )    
        {
        return NULL;
        }
    TInt indx = LookForUri( aUri );
    if(indx != KErrNotFound)
        {
        CPolicyReq* req = iPolicies[indx];
        CWSPolicy* pol = req->Policy();
        if( pol )
            {
            return pol->NonNormalizedPolicyL();		//codescannerwarning
            }
        }
    return NULL;    
    }

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//  
CPolicyReq* CPolicyReq::NewL(const TDesC8& aUri)
{
    CPolicyReq* self   = CPolicyReq::NewLC(aUri);
    CleanupStack::Pop();
    return self;
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CPolicyReq* CPolicyReq::NewLC(const TDesC8& aUri)
{
    CPolicyReq* self   = new (ELeave) CPolicyReq();
    CleanupStack::PushL (self);
    self->ConstructL(aUri);
    return self;
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CPolicyReq::~CPolicyReq()
{
    delete iPolicy;
    delete iName; //policy name
    delete iURI; //uri to download
    delete iContents; //contents
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TPtrC8  CPolicyReq::Name()
{
    if (iName != NULL)
    return iName->Des();
    else
    return KNullDesC8();
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TPtrC8  CPolicyReq::URI()
{
        if (iURI != NULL)
        return iURI->Des();
    else
        return KNullDesC8();
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TPtrC8  CPolicyReq::Contents()
{
    if (iContents != NULL)
        return iContents->Des();
    else
        return KNullDesC8();
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt    CPolicyReq::SetNameL(const TDesC8& aName)
{
    if(aName.Length() < 1)    
    return KErrArgument;

    if (iName != NULL)
    {
        delete iName;
        iName = NULL;
    }

   iName = aName.AllocL(); 
   return KErrNone;
     
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt    CPolicyReq::SetContentsL(const TDesC8& aContent)
{
    if(aContent.Length() < 1)    
    return KErrArgument;

    if (iContents != NULL)
    {
        delete iContents;
        iContents = NULL;
    }

   iContents = aContent.AllocL();     
   return KErrNone;
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CPolicyReq::CPolicyReq() : 
iPolicy(NULL), iName(NULL),iURI(NULL),iContents(NULL)
{
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
void CPolicyReq::ConstructL(const TDesC8& aUri)
{
    if(aUri.Length() < 1)    
    User::Leave (KErrArgument);
        
    iURI = aUri.AllocL();
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CWSPolicy* CPolicyReq::Policy()
{
    return iPolicy;    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt CPolicyReq::SetPolicy(CWSPolicy* aPolicy)
{
    if(iPolicy)    
    {
        delete iPolicy;
        iPolicy = NULL;
    }
    
    iPolicy = aPolicy;
    return KErrNone;
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CPolicyReqMap* CPolicyReqMap::NewL(const TDesC8& aContract)
{
    CPolicyReqMap* self   = CPolicyReqMap::NewLC(aContract);
    CleanupStack::Pop();
    return self;
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CPolicyReqMap* CPolicyReqMap::NewLC(const TDesC8& aContract)
{
    CPolicyReqMap* self   = new (ELeave) CPolicyReqMap();
    CleanupStack::PushL (self);
    self->ConstructL(aContract);
    return self;
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CPolicyReqMap::CPolicyReqMap(): iURI(NULL), iContents(NULL), iSD(NULL) 
{
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CPolicyReqMap::~CPolicyReqMap()
{
    delete iContract; //service contract
    delete iURI; //uri to download
    delete iContents; //contents
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
void CPolicyReqMap::ConstructL(const TDesC8& aContract)
{
    if (aContract.Length() > 0)    
        iContract = aContract.AllocL();
        
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TPtrC8  CPolicyReqMap::Contract()
{
    if(iContract)
        return iContract->Des();
    else
        return KNullDesC8();
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TPtrC8  CPolicyReqMap::Uri()
{
    if(iURI)
        return iURI->Des();
    else
        return KNullDesC8();
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TPtrC8  CPolicyReqMap::Contents()
{
    if(iContents)
        return iContents->Des();
    else
        return KNullDesC8();
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt    CPolicyReqMap::SetUriL(const TDesC8& aUri)
{
    if(aUri.Length() > 0)
    {
        iURI = aUri.AllocL();
        return KErrNone;
    }
    return KErrArgument;
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt    CPolicyReqMap::SetContentsL(const TDesC8& aContent)
{

    if(iContents)
    {
        delete iContents;
        iContents = NULL;
    }

    if(aContent.Length() > 0)
    {
        iContents = aContent.AllocL();
    }
    return KErrNone;
    
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
CSenWSDescription* CPolicyReqMap::ServiceDescription()
{
    return iSD;
}
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt CPolicyReqMap::SetSD(CSenWSDescription* aSD)
{
    iSD  =   aSD; 
    return KErrNone;
}
// END OF FILE




