/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  VPN PFKEY socket interface
*
*/

#ifndef C_PFKEYSOCKET_H
#define C_PFKEYSOCKET_H

#include <es_sock.h>
#include <in_sock.h>
#include <lib_pfkey.h>
#include "pfkeymsg.h"

// FORWARD DECLARATIONS
class TIpsecSAData;
class RSocketServ;
class MIkeDebug;

class MPFKeyMessageListener
    {
public:
    virtual void PfkeyMessageReceived( const TPfkeyMessage& aReq ) = 0;
    };


class TPendingSpiRequest
    {
public:
    TPendingSpiRequest(TUint32& aSpi, TRequestStatus& aClientStatus): 
                       iSpi(aSpi), iClientStatus(aClientStatus){}
    
    TUint32& iSpi;
    TRequestStatus& iClientStatus;   
    };

//
// Register for ESP
// and stay listening the PFKEY socket
//
class CPFKeySocketIf : public CActive
    {
public:
    IMPORT_C static CPFKeySocketIf* NewL( MPFKeyMessageListener* aListener,
                                          MIkeDebug& aDebug );
	~CPFKeySocketIf() ;

    //PFKEY related functions
	void GetSpi( const TUint8 aType,
                 const TUint32 aSeq,
                 const TInetAddr& aSrc,
                 const TInetAddr& aDst,
                 TUint32& aSpi,
                 TRequestStatus& aClientStatus);
	void CancelGetSpi(TRequestStatus& aClientStatus);
    
	IMPORT_C void AcquireSAError( const TIpsecSAData& aSAData,
                                  const TInt aError );
    
	IMPORT_C void UpdateSAL( const TIpsecSAData& aSAData );
    
	IMPORT_C void AddSAL( const TIpsecSAData& aSAData );
    
	IMPORT_C void DeleteSA( const TUint32 aSPI,
                            const TInetAddr& aSrc,
                            const TInetAddr& aDst,
                            const TUint8 aProtocol );
    
	IMPORT_C void FlushSAs();    
	
private:
    CPFKeySocketIf( MPFKeyMessageListener* aListener,
                    MIkeDebug& aDebug );
    void ConstructL();        

    void AddUpdateSAL( const TUint8 aType,
                       const TIpsecSAData& aSAData );
    
    TUint32 NewSpi();

#ifdef _DEBUG           
    void ShowMessageL( TPfkeyRecvMsg& aMsg );
#endif  
    
// from base class CActive    
    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );     

protected: // data
    MPFKeyMessageListener*  iListener;
    
private: // data
    
    RSocketServ             iSocketServer;
    RSADB                   iSadb;    
    
    TPfkeyRecvMsg           iMsg;
    TUint32                 iSeq;    
    TUint32                 iSpiBase;        
    
    MIkeDebug&              iDebug;
    
    RArray<TPendingSpiRequest> iPendingSpiRequests;
    };

#endif // C_PFKEYSOCKET_H
