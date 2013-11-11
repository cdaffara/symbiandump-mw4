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
* Description: Key management daemon PFKEY message module.
*
*/

#ifndef PFKEYMSG_H
#define PFKEYMSG_H

#include <es_sock.h>
#include <in_sock.h>
#include <lib_pfkey.h>

#include <networking/pfkeyv2.h>
#include <networking/pfkeyext.h>

//
// Classes for handling PFKEY structures.
//

class TPfkeyBase
    {
public:
    const struct sadb_msg* iMsg;
    TPfkeyBase();

#ifdef _DEBUG       
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif   
    };

class TPfkeyAssociation
    {
public:
    const struct sadb_sa* iExt;
    TPfkeyAssociation();

#ifdef _DEBUG           
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif      
    };

class TPfkeyLifetime
    {
public:
    const struct sadb_lifetime* iExt;
    TPfkeyLifetime();

#ifdef _DEBUG           
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif          
    };

class TPfkeyAddress
    {
public:
    const struct sadb_address* iExt;
    const TInetAddr* iAddr;
    TPfkeyAddress();
    IMPORT_C const TInetAddr& Address() const;

#ifdef _DEBUG               
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif      
    };

class TPfkeyKey
    {
public:
    const struct sadb_key* iExt;
    TPtrC8 iData;
    TPfkeyKey();
    
#ifdef _DEBUG                   
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif  
    };

class TPfkeyIdentity
    {
public:
    const struct sadb_ident* iExt;
    TPtrC8 iData;
    TPfkeyIdentity();
    
#ifdef _DEBUG                   
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif  
    };

class TPfkeySensitivity
    {
public:
    const struct sadb_sens* iExt;
    TPtrC8 iSensBitmap;
    TPtrC8 iIntegBitmap;
    TPfkeySensitivity();
    
#ifdef _DEBUG                       
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif      
    };

class TPfkeyProposal
    {
public:
    const struct sadb_prop* iExt;
    const struct sadb_comb* iComb;
    TInt iNumComb;
    TPfkeyProposal();

#ifdef _DEBUG                       
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif			
    };

class TPfkeySupported
    {
public:
    const struct sadb_supported* iExt;
    const struct sadb_alg* iAlg;
    TInt iNumAlg;
    TPfkeySupported();

#ifdef _DEBUG                       
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif    
    virtual void AlgString( TDes &aStr,
                            TUint8 aAlg ) const = 0;
    };

class TPfkeySupportedAuth : public TPfkeySupported
    {                           
public:
    void AlgString( TDes &aStr,
                    TUint8 aAlg ) const;
    static void Alg2String( TDes &aStr,
                            TUint8 aAlg );          
    };

class TPfkeySupportedEncrypt : public TPfkeySupported
    {                  
public:
    void AlgString( TDes &aStr,
                    TUint8 aAlg ) const;
    static void Alg2String( TDes &aStr,
                            TUint8 aAlg );  
    };

class TPfkeySpirange
    {
public:
    const struct sadb_spirange* iExt;
    TPfkeySpirange();
    
#ifdef _DEBUG                   
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif
    };

class TPfKeySelector: public sadb_x_selector
    {
public:
    TInetAddr iSrc;
    TInetAddr iDst;    
    };
    
class TPfkeyTs
    {
public:
    const struct sadb_x_ts* iExt;
    TPfkeyTs();
    IMPORT_C TInt SelectorCount() const;
    IMPORT_C const TPfKeySelector& Selector( TInt aIndex ) const;
    
#ifdef _DEBUG
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif    
    };
    

class TPFkeyPrivExt
    {
public:
    const struct sadb_gen_ext* iExt;
    TPtrC8 iData;
    TPFkeyPrivExt();
    
#ifdef _DEBUG                   
    void String( TDes &aStr,
                 const TDesC &aLabel ) const;
#endif  
    };


//
// Internal presentation of the PF_KEY message
//
class TPfkeyMessage
    {
public:
    //
    // Construct internal presentation from the PFKEY bytestream message
    TPfkeyMessage( TPfkeyRecvMsg& aMsg );
    TPfkeyMessage();
    TInt iError;    // == KErrNone, if message format valid.
    TPfkeyBase iBase;
    TPfkeyAssociation iSa;
    TPfkeyLifetime iCurrent;
    TPfkeyLifetime iHard;
    TPfkeyLifetime iSoft;
    TPfkeyAddress iSrcAddr;
    TPfkeyAddress iDstAddr;
    TPfkeyAddress iProxyAddr;
    TPfkeyKey iAuthKey;
    TPfkeyKey iEncryptKey;
    TPfkeyIdentity iSrcIdent;
    TPfkeyIdentity iDstIdent;
    TPfkeySensitivity iSensitivity;
    TPfkeyProposal iProposal;
    TPfkeySupportedAuth iAuthAlgs;
    TPfkeySupportedEncrypt iEncryptAlgs;
    TPfkeySpirange iSpirange;
    TPfkeyTs iTs;
    TPFkeyPrivExt  iPrivateExtension;  // For ESP UDP encapsulation 
    };

#endif // PFKEYMSG_H 
