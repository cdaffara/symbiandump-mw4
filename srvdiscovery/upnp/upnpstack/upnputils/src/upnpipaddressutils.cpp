/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  IP Address utils
*
*/


// INCLUDES
#include <e32base.h>
#include <commdb.h>
#include <in_sock.h>
#include "upnpipaddressutils.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"

// ================= MEMBER FUNCTIONS =======================
namespace UpnpIpAddressUtil
    {
// -----------------------------------------------------------------------------
// UpnpIpAddressUtil::GetIfNameForIap
// Reads the IF name for the specified IAP ID.
// -----------------------------------------------------------------------------
//
    EXPORT_C void GetIfNameForIap( TUint32 aIapId, TDes& aIfName )
        {
        CCommsDatabase* theDb;
        TUint32 iapId;
        TBool nameNotFound = ETrue;

        // First zero the name. If IF Name is not found, it will be zero lenght.
        aIfName.Zero();

        // Open CommDB
        theDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
        CleanupStack::PushL( theDb );

        theDb->ShowHiddenRecords();

        // Open IAP table and find the IAP ID given as method parameter.
        CCommsDbTableView* view = theDb->OpenTableLC( TPtrC( IAP ) );
        TInt res = view->GotoFirstRecord();
        while ( res == KErrNone && nameNotFound )
            {
            // Read IAP ID and match it to the one given as parameter
            view->ReadUintL( TPtrC( COMMDB_ID ), iapId );
            if ( iapId == aIapId )
                {
                // Right IAP found, read bearer ID and type
                TUint32 iapBearerId;
                view->ReadUintL( TPtrC( IAP_BEARER ),iapBearerId );
                view->ReadTextL( TPtrC( IAP_BEARER_TYPE ),aIfName );

                // Find the correct bearer table specified by bearer type
                CCommsDbTableView* ifView = theDb->OpenTableLC( 
                    TPtrC( aIfName ) );
                TInt bearerRes = ifView->GotoFirstRecord();
                TUint32 bearerId;
                while ( bearerRes == KErrNone && nameNotFound )
                    {
                    ifView->ReadUintL( TPtrC( COMMDB_ID ), bearerId );
                    // Find out right bearer record and read IF name
                    if ( bearerId == iapBearerId )
                        {
                        // Read the IF name, it is returned in the method
                        // parameter
                        ifView->ReadTextL( TPtrC( IF_NAME ),aIfName );
                        nameNotFound = EFalse;
                        }
                    // Bearer id does not match, check next record
                    else
                        {
                        bearerRes = ifView->GotoNextRecord();
                        }
                    }
                CleanupStack::PopAndDestroy( ifView );
                }
            // IAP id does not match, check next record
            else
                {
                res = view->GotoNextRecord();
                }
            }
        CleanupStack::PopAndDestroy( view );
        CleanupStack::PopAndDestroy( theDb );
        }

// -----------------------------------------------------------------------------
// UpnpFileUtil::MatchIfNameWithIpInterface
// Matches IF name with IP address of the TCP/IP stack interfaces.
// -----------------------------------------------------------------------------
//
    EXPORT_C TInetAddr MatchIfNameWithIpInterface( RSocketServ& aSocketServer,
                                                   TDesC& aIfName,
                                                   TInt& aInterfaceIndex )
        {
        // Buffer for IP address
        const TUint KAddrOutputLength = 50;
        TBuf<KAddrOutputLength> address;
        TPckgBuf<TSoInetInterfaceInfo> info;
        TInetAddr localIpAddress;        
        TInetAddr autoIpAddress;
        TInt autoIfIndex = 0;
        RSocket sock;
        TBool autoIpFound = EFalse;
        TBool ipFound = EFalse;

        _LIT( KUdpProtocol, "udp" );
        _LIT( KLoop4Interface, "loop4" );
        _LIT( KLoop6Interface, "loop6" );
        _LIT( KNoAddress, "0.0.0.0");
        _LIT( KLocalHost, "127.0.0.1");
        _LIT( KWlanIPv6IfSearchString, "wlan6[*");
        _LIT( KWlanIPv4IfSearchString, "wlan[*");
        _LIT( KWlanIfMatchString, "wlan*");
        _LIT( KEtherIfSearchString, "eth[*");
        _LIT( KEtherIfMatchString, "eth*");
        // BT PAN is the same for both info and NIF name
        _LIT( KBtPanIfSearchString, "btpannif*");
        _LIT( KAutoIpMatch, "169.254.*");
        
        //the default interface index
        aInterfaceIndex = 0;
        
        TInt ret= sock.Open( aSocketServer, KUdpProtocol );
        if ( KErrNone == ret )
            {
            ret = sock.SetOpt( KSoInetEnumInterfaces, KSolInetIfCtrl );
            }

        if ( KErrNone == ret )
            {
            while( KErrNone == sock.GetOpt( 
                       KSoInetNextInterface, KSolInetIfCtrl, info ) )
                {

                // If state is up and name is not empty, loop4 or loop6, 
                // then get the address
                if ( ( EIfUp == info().iState ) &&
                     ( 0 != info().iName.Length() ) &&
                     ( 0 != info().iName.Match( KLoop4Interface ) ) && 
                     ( 0 != info().iName.Match( KLoop6Interface ) ) )
                    {
                    // Check if we have valid IP
                    address.Zero();
                    info().iAddress.Output( address );

                    if ( ( 0 < address.Length() ) && 
                         ( 0 != address.Match( KNoAddress ) ) &&
                         ( 0 != address.Match( KLocalHost ) ) )
                        {
                        
                        // read index of interface 
                                        
                        TPckgBuf<TSoInetIfQuery> query;
    					query().iName = info().iName;
    					
    					TInt index;
    					ret=sock.GetOpt(KSoInetIfQueryByName, KSolInetIfQuery, query);
    					if(KErrNone==ret)
    					    {
    					    index = query().iIndex;
                   
                            // For ethernet, WLAN and BT PAN interfaces, 
                            // change the match string to suit right interfaces
                            if ( 0 <= info().iName.MatchC( KEtherIfSearchString ) )
                                {
                                // Set the IF name for ETH Match operation below.
                                info().iName.Copy( KEtherIfMatchString );
                                }
                            else if ( 0 <= info().iName.MatchC( 
                                KWlanIPv4IfSearchString ) )
                                {
                                // Set the IF name for WLAN Match operation below.
                                info().iName.Copy( KWlanIfMatchString );
                                }
                            else if ( 0 <= info().iName.MatchC( 
                                    KBtPanIfSearchString ) )
                                {
                                // Set the IF name for BT PAN Match operation below.
                                info().iName.Copy( KBtPanIfSearchString );
                                }

                            // Exlude IPv6 addresses, they are not supported.
                            // If IF name matches with the one given as parameter,
                            // then we have the same interface with IP.
                            if ( ( 0 > info().iName.MatchC( KWlanIPv6IfSearchString ) ) &&
                                 ( 0 <= aIfName.MatchC( info().iName ) ) )
                                {
                                // If the address is auto-ip, check whether there 
                                // is also DHCP IP in the same interface.
                                if ( 0 <= address.Match( KAutoIpMatch ) )
                                    {
                                    autoIpFound = ETrue;
                                    autoIpAddress = info().iAddress;
                                    autoIpAddress.ConvertToV4();
                                    autoIfIndex = index;
                                    }
                                // Other than auto IP
                                else
                                    {
                                    ipFound = ETrue;
                                    localIpAddress = info().iAddress;
                                    // we're using ipv4 addresses
                                    localIpAddress.ConvertToV4();
                                    aInterfaceIndex = index;
                                    }
                                }
    					    }
                        }
                    }
                }
            }
        sock.Close();

        // If only auto-IP was found, then return it.
        // If only DHCP IP was found or also auto-IP, then return the
        // DHCP IP which is already placed in localIpAddress variable.
        // It is not possible that there is no IP as the connection has
        // been started already by the UPnP application and this method can't
        // be called if there is no connection.
        if ( !ipFound && autoIpFound )
            {
            localIpAddress = autoIpAddress;
            aInterfaceIndex = autoIfIndex;
            }

        address.Zero();
        localIpAddress.Output( address );

        // return the address
        return localIpAddress;
        }
    }

//  End of File
