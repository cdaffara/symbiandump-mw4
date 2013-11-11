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
* Description:    Header declaration
*
*/










#include <e32base.h>


class MSenFilesObserver;
class MSenAuthenticationProvider;
class CSenIdentityProvider;
class MSenCoBrandingObserver;
#ifdef __ENABLE_ALR__
class MMobilityProtocolResp;
#endif //__ENABLE_ALR__

NONSHARABLE_CLASS(CSenConnAgentSession): public CSession2
    {
    private:
    ~CSenConnAgentSession();
    void ServiceL(const RMessage2& aMessage);
    void Disconnect(const RMessage2& aMessage);
    TInt ReauthenticationNeededL(const RMessage2& aMessage);
    TInt CoBrandingGoNoGoL(const RMessage2& aMessage);
    
    private:
    MSenFilesObserver* iObserver;
    MSenAuthenticationProvider* iAuthObserver;
    MSenCoBrandingObserver* ipCoBrandingObserver;
#ifdef __ENABLE_ALR__
    MMobilityProtocolResp* iMobiltyObserver;
#endif //__ENABLE_ALR__
    };
