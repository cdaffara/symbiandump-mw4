/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Inline method definitions of class CHttpWapSession.   
*      
*
*/


#ifndef HTTP_WAP_SESSION_INL
#define HTTP_WAP_SESSION_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CHttpWapSession::StringF()
// ---------------------------------------------------------
//
RStringF CHttpWapSession::StringF( HTTP::TStrings aId )
    {
    return iSess.StringPool().StringF( aId, RHTTPSession::GetTable() );
    }

#endif /* def HTTP_WAP_SESSION_INL */
