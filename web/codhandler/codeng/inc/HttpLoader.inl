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
*      Inline method definitions of class CHttpLoader.   
*      
*
*/


#ifndef HTTP_LOADER_INL
#define HTTP_LOADER_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CHttpLoader::SetObserver()
// ---------------------------------------------------------
//
void CHttpLoader::SetObserver( MCodLoadObserver* aCodLoadObserver )
    {
    iCodLoadObserver = aCodLoadObserver;
    }

// ---------------------------------------------------------
// CHttpLoader::StringF()
// ---------------------------------------------------------
//
RStringF CHttpLoader::StringF( HTTP::TStrings aId )
    {
    return iSess->Sess().StringPool().StringF( aId, RHTTPSession::GetTable() );
    }

#endif /* def HTTP_LOADER_INL */
