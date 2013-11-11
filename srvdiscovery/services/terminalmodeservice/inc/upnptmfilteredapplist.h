/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpTmFilteredAppList class declaration
*
*/

#ifndef __UPNPTMFILTEREDAPPLIST_H__
#define __UPNPTMFILTEREDAPPLIST_H__


//System Includes
#include <inetprottextutils.h>

//User Includes
#include "upnpterminalmodeicon.h"
#include "upnpremotableappstore.h"
#include "upnpappfilterinfo.h"

//Forward Declaration
class CUpnpTmServerImpl;


/**
 * Constructs the filtered GetApplicationList action Response.
 * Applies the AppListingFilter criteria while creating the soap 
 * response buffer.
 *
 */
class CUpnpTmFilteredAppList : public CBase 
    {
public:
    static CUpnpTmFilteredAppList* NewL( CUpnpTmServerImpl& aTmServerImpl ); 
	
	~CUpnpTmFilteredAppList();	
    void ParseAppFilterStringL( const TDesC8& aAppFilter, TTerminalModeErrorCode& aErr );
    void ConstructFilteredGetAppListResponseL( TInt aAppIndex );
    const TDesC8& SignatureElement()const;
    
private:
    CUpnpTmFilteredAppList( CUpnpTmServerImpl& aTmServerImpl );
    void ConstructL( );
    
    TBool TerminalModeInfoMatchingL( const CUpnpTmInfoElement& aTmInfoElement )const;
    TBool TmInfoElementMatchingL( const CUpnpTmInfoElement& aTmInfoElement,TInt aIndex )const;                                                                       
    void IconMatchingL( TInt aAppIndex );              
    void ConstructFilteredResponseL( TInt aAppIndex );                              
    TBool IconLookUp( CUpnpTerminalModeIcon& aIcon );
    const TDesC8& RemoveWhiteSpace( TPtrC8& aData );

private:
    CUpnpAppFilterInfo*                   iFilterInfo;
    CUpnpTmServerImpl&                    iTmServerImpl;
    };

#endif  //  __UPNPTMFILTEREDAPPLIST_H__
