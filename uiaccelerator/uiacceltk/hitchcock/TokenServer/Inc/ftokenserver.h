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
* Description:   Server declaration, internal for Nokia
*
*/



#ifndef __F_TOKEN_SERVER_H__
#define __F_TOKEN_SERVER_H__

class CFsTokenServerSession;

NONSHARABLE_CLASS(CFsTokenServer): public CServer2
    {
public:
    static void NewLC();
	~CFsTokenServer();
	void AddTokenL(const RMessage2& aMessage);
	void OpenFileL(const RMessage2& aMessage);
    void HandleClientExit(const CSession2* aClient);

private:
    void ConstructL();
    CFsTokenServer():CServer2(CActive::EPriorityStandard){}
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;

private:
    RFs iFs;

    struct TToken
        {
        TToken()
            {
            Mem::FillZ(this, sizeof(TToken));
            }
        TInt64 iToken;
        TSecurityPolicy iPolicy; 
        HBufC* iFileName;

       static TBool CompareToken(const TInt64* aTokenKey, const TToken& aToken)
            {
            return *aTokenKey == aToken.iToken;
            }
         
        static TBool CompareOwner(const CSession2* aSession, const TToken& aToken)
            {
            return aSession == aToken.iOwner;
            }
            
        CSession2* iOwner;
        TBool iHasSubToken;        
        };
        
    // ToDo: should use sorted array
    RArray<TToken> iTokens;
    };

NONSHARABLE_CLASS(CFsTokenServerSession): public CSession2
    {
    public:
    CFsTokenServerSession();
    ~CFsTokenServerSession();
    void ServiceL(const RMessage2& aMessage);
    };

#endif