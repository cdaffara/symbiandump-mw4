/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object that retrieves an SPI value for IPsec SA
*
*/


#ifndef IPSECSASPIRETRIEVER_H_
#define IPSECSASPIRETRIEVER_H_

#include <e32base.h>

/**
 * A Callback interface for IPsec SA SPI value retriever.
 *  Callback interface which is used by CIpsecSaSpiRetriever object to
 *  notify about completion of IPsec SA SPI retrieve
 * 
 *  @lib ikeutils.lib
 */
class MIpsecSaSpiRetrieverCallback
    {
public:
    
    /**
     * Notifies about completion of IPsec SPI SA retrieve.
     * 
     * @param aSpiRequestId Id of the SPI retrieve request.
     * @param aStatus Completion status
     * @param aSpi SPI value
     */    
    virtual void IpsecSaSpiRetrieved(TUint32 aSpiRequestId, 
                                     TInt aStatus, 
                                     TUint32 aSpi) = 0;
    
    };

class CPFKeySocketIf;
class TInetAddr;

/**
 *  Retrieves new IPsec SA SPI value from the IPsec
 * 
 *  @lib ikeutils.lib
 */
class CIpsecSaSpiRetriever : public CActive
    {
public:
    
    /**
     * Constructs new IPsec SA SPI retriever.
     * 
     * @param aRetrieverCallback Used callback interface
     * @param aSocketIf Used PFKey socket interface.
     */
    IMPORT_C static CIpsecSaSpiRetriever* NewL(MIpsecSaSpiRetrieverCallback& aRetrieverCallback,
                                               CPFKeySocketIf& aSocketIf);
    ~CIpsecSaSpiRetriever();
    
    /**
     * Issues new IPsec SA SPI retrieve request.
     * The completation of the SPI retrieve request is notified by using 
     * MIpsecSaSpiRetrieverCallback interface.
     * 
     * @param aSpiRequestId Id for the SPI request. 
     * @param aIpsecProtocol Used IPsec protocol. 
     *                       Possible values are SADB_SATYPE_AH and SADB_SATYPE_ESP.
     * @param aSrc Source address of the IPsec SA. 
     *             If SRC_SPECIFIC is not set in the IPsec policy can be set to Unspecified. 
     * @param aDst Destination address of the IPsec SA.
     */
    IMPORT_C void GetIpsecSaSpi(const TUint32 aSpiRequestId,
                                const TUint8 aIpsecProtocol,
                                const TInetAddr& aSrc,
                                const TInetAddr& aDst);
    
private:    
    CIpsecSaSpiRetriever(MIpsecSaSpiRetrieverCallback& aRetrieverCallback,
                         CPFKeySocketIf& aSocketIf);
    
    void RunL();
    void DoCancel();
    TInt RunError(TInt aStatus);            
        
    MIpsecSaSpiRetrieverCallback& iRetrieverCallback;
    CPFKeySocketIf&             iSocketIf;
    
    TUint32 iSpi;
    TUint32 iSpiRequestId;
    };

#endif /* IPSECSASPIRETRIEVER_H_ */
