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

#include <es_sock.h>
#include <in_sock.h>
#include <eikenv.h>

#include "pfkeymsg.h"

// ======== MEMBER FUNCTIONS ========

#ifdef _DEBUG
//
//  Convert to String methods
//
//  TPfkeyBase::String
//      Convert Base Message Header to printable string
//

void TPfkeyBase::String(TDes &aStr, const TDesC &aLabel) const
    {

    if (!iMsg)
        return;

    aStr.Append(aLabel);
    aStr.AppendFormat(_L("#%d.%d "),
            (int)iMsg->sadb_msg_seq,
            (int)iMsg->sadb_msg_pid);
    switch (iMsg->sadb_msg_type)
        {
    case SADB_GETSPI:
        aStr.Append(_L("GETSPI"));
        break;
    case SADB_UPDATE:
        aStr.Append(_L("UPDATE"));
        break;
    case SADB_ADD:
        aStr.Append(_L("ADD"));
        break;
    case SADB_DELETE:
        aStr.Append(_L("DELETE"));
        break;
    case SADB_GET:
        aStr.Append(_L("GET"));
        break;
    case SADB_ACQUIRE:
        aStr.Append(_L("ACQUIRE"));
        break;
    case SADB_REGISTER:
        aStr.Append(_L("REGISTER"));
        break;
    case SADB_EXPIRE:
        aStr.Append(_L("EXPIRE"));
        break;
    case SADB_FLUSH:
        aStr.Append(_L("FLUSH"));
        break;
    case SADB_DUMP:
        aStr.Append(_L("DUMP"));
        break;
    default:
        aStr.AppendFormat(_L("UNKNOWN(%d)"),
            iMsg->sadb_msg_type);
        break;
        }
    if (iMsg->sadb_msg_version != PF_KEY_V2)
        aStr.AppendFormat(_L("(V%d)"), (int)iMsg->sadb_msg_version);

    if (iMsg->sadb_msg_errno) 
        {
        aStr.AppendFormat(_L("(Errno%d)"), (int)iMsg->sadb_msg_errno);      
        }
    else
        aStr.Append(_L("[ok] "));
    switch (iMsg->sadb_msg_satype)
        {
    case SADB_SATYPE_AH:
        aStr.Append(_L("AH"));
        break;
    case SADB_SATYPE_ESP:
        aStr.Append(_L("ESP"));
        break;
    case SADB_SATYPE_UNSPEC:
        aStr.Append('*');
        break;
    default:
        aStr.AppendFormat(_L("UNKNOWN=%d"), (int)iMsg->sadb_msg_satype);
        break;
        }
    }

void TPfkeyAssociation::String(TDes &aStr, const TDesC &aLabel) const
    {
    if (!iExt)
        return;
    aStr.Append(aLabel);
    if (iExt->sadb_sa_encrypt || iExt->sadb_sa_auth)
        {
        aStr.Append('(');
        if (iExt->sadb_sa_encrypt)
            TPfkeySupportedEncrypt::Alg2String(aStr, iExt->sadb_sa_encrypt);
        aStr.Append(',');
        if (iExt->sadb_sa_auth)
            TPfkeySupportedAuth::Alg2String(aStr, iExt->sadb_sa_auth);
        aStr.Append(')');
        }

    switch (iExt->sadb_sa_state)
        {
    case SADB_SASTATE_LARVAL:
        aStr.Append(_L(" LARVAL"));
        break;
    case SADB_SASTATE_MATURE:
        aStr.Append(_L(" MATURE"));
        break;
    case SADB_SASTATE_DYING:
        aStr.Append(_L(" DYING"));
        break;
    case SADB_SASTATE_DEAD:
        aStr.Append(_L(" DEAD"));
        break;
    default:
        aStr.AppendFormat(_L(" UNKNOWN=%d"), (int)iExt->sadb_sa_state);
        break;
        }
    aStr.AppendFormat(_L(" SPI=%x"), (int)ByteOrder::Swap32(iExt->sadb_sa_spi));
    if (iExt->sadb_sa_replay)
        aStr.AppendFormat(_L(" Replay=%d"), (int)iExt->sadb_sa_replay);
    }

void TPfkeyAddress::String(TDes &aStr, const TDesC &aLabel) const
    {
    if (!iExt)
        return;
    aStr.Append(aLabel);
    if (iAddr)
        {
        TBuf<39> addr;
        iAddr->OutputWithScope(addr);
        aStr.Append(addr);
        if (iExt->sadb_address_proto)
            aStr.AppendFormat(_L(" proto=%d"), iExt->sadb_address_proto);
        if (iAddr->Port())
            aStr.AppendFormat(_L(" port=%d"), iAddr->Port());
        }
    }

void TPfkeyKey::String(TDes &aStr, const TDesC &aLabel) const
    {
    if (iExt)
        aStr.Append(aLabel);
    }

void TPfkeyLifetime::String(TDes &aStr, const TDesC &aLabel) const
    {
    if (!iExt)
        return;
    aStr.Append(aLabel);
    aStr.Append((TChar)'(');
    aStr.AppendNum(iExt->sadb_lifetime_allocations);
    aStr.Append((TChar)',');
    aStr.AppendNum(iExt->sadb_lifetime_bytes);
    aStr.Append((TChar)',');
    aStr.AppendNum(iExt->sadb_lifetime_addtime);
    aStr.Append((TChar)',');
    aStr.AppendNum(iExt->sadb_lifetime_usetime);
    aStr.Append((TChar)')');
    }

void TPfkeySupported::String(TDes &aStr, const TDesC &aLabel) const
    {
    if (!iExt)
        return;
    aStr.Append(aLabel);
    for (int i = 0; i < iNumAlg; ++i)
        {
        AlgString(aStr, iAlg[i].sadb_alg_id);
        if (iAlg[i].sadb_alg_minbits == iAlg[i].sadb_alg_maxbits)
            aStr.AppendFormat
                (_L("(IV=%d,key=%d)"),
                (int)iAlg[i].sadb_alg_ivlen,
                (int)iAlg[i].sadb_alg_maxbits);
        else
            aStr.AppendFormat
                (_L("(IV=%d,%d<=key<=%d)"),
                (int)iAlg[i].sadb_alg_ivlen,
                (int)iAlg[i].sadb_alg_minbits,
                (int)iAlg[i].sadb_alg_maxbits);
        }
    }

void TPfkeyIdentity::String(TDes &aStr, const TDesC &aLabel) const
{

#ifdef _UNICODE
    if (iExt)
    {
        aStr.Append(aLabel);
        if (iData.Length() == 0)
            return;
        HBufC *unibuf = HBufC::New(iData.Length());
        if (!unibuf)
            return;
        unibuf->Des().Copy(iData);
        aStr.Append(unibuf->Des());
        delete unibuf;
    }
#else
    if (iExt)
    {
        aStr.Append(aLabel);
        aStr.Append(iData);
    }
#endif
}

void TPfkeySensitivity::String(TDes &aStr, const TDesC &aLabel) const
    {
    if (iExt)
        {
        aStr.Append(aLabel);
        }
    }

void TPfkeyProposal::String(TDes &aStr, const TDesC &aLabel) const
    {
    if (!iExt)
        return;
    aStr.Append(aLabel);
    if (iExt->sadb_prop_replay)
        aStr.AppendFormat(_L("replay=%d"), (int)iExt->sadb_prop_replay);
    for (int i = 0; i < iNumComb; i++)
        {
        aStr.AppendFormat(_L(" %d:("), i+1);
        if (iComb[i].sadb_comb_flags & SADB_SAFLAGS_PFS)
            aStr.Append(_L("PFS "));
        if (iComb[i].sadb_comb_encrypt)
            {
            TPfkeySupportedEncrypt::Alg2String(aStr, iComb[i].sadb_comb_encrypt);
            aStr.AppendFormat(_L("[%d..%d]"),
                iComb[i].sadb_comb_encrypt_minbits,
                iComb[i].sadb_comb_encrypt_maxbits);
            }
        if (iComb[i].sadb_comb_auth)
            {
            aStr.Append(',');
            TPfkeySupportedAuth::Alg2String(aStr, iComb[i].sadb_comb_auth);
            aStr.AppendFormat(_L("[%d..%d]"),
                iComb[i].sadb_comb_auth_minbits,
                iComb[i].sadb_comb_auth_maxbits);
            }
        if (iComb[i].sadb_comb_soft_allocations ||
            iComb[i].sadb_comb_soft_bytes != 0 ||
            iComb[i].sadb_comb_soft_addtime != 0||
            iComb[i].sadb_comb_soft_usetime != 0)
            {
            aStr.AppendFormat(_L(" soft=(%d,"), (int)iComb[i].sadb_comb_soft_allocations);
            aStr.AppendNum(iComb[i].sadb_comb_soft_bytes);
            aStr.Append(',');
            aStr.AppendNum(iComb[i].sadb_comb_soft_addtime);
            aStr.Append(',');
            aStr.AppendNum(iComb[i].sadb_comb_soft_usetime);
            aStr.Append(')');
            }
        if (iComb[i].sadb_comb_hard_allocations ||
            iComb[i].sadb_comb_hard_bytes != 0 ||
            iComb[i].sadb_comb_hard_addtime != 0 ||
            iComb[i].sadb_comb_hard_usetime != 0)
            {
            aStr.AppendFormat(_L(" hard=(%d,"), (int)iComb[i].sadb_comb_hard_allocations);
            aStr.AppendNum(iComb[i].sadb_comb_hard_bytes);
            aStr.Append(',');
            aStr.AppendNum(iComb[i].sadb_comb_hard_addtime);
            aStr.Append(',');
            aStr.AppendNum(iComb[i].sadb_comb_hard_usetime);
            aStr.Append(')');
            }
        aStr.Append(')');
        }
    }

void TPfkeySpirange::String(TDes &aStr,const TDesC &aLabel) const
    {
    if (iExt)
        {
        aStr.Append(aLabel);
        }
    }

void TPfkeyTs::String(TDes &aStr,const TDesC &aLabel) const
    {       
    if (iExt)
        {            
        for (TInt i = 0; i < SelectorCount(); ++i)
            {                              
            const TPfKeySelector& selector = Selector(i);
            
            TBuf<50> src;
            TBuf<50> dst;
            
            selector.iSrc.OutputWithScope(src);
            selector.iDst.OutputWithScope(dst);
                    
            aStr.AppendFormat(_L("%S[%d] proto=%d src=%S:%d, dst=%S:%d" ), 
                              &aLabel, i, selector.sadb_x_selector_proto,
                              &src, selector.iSrc.Port(),
                              &dst, selector.iDst.Port());
                            
                                                                                                          
            }
        }
    }

void TPFkeyPrivExt::String(TDes &aStr, const TDesC &aLabel) const
    {
    if (iExt)
        aStr.Append(aLabel);
    }

#endif   //#ifdef _DEBUG 


TPfkeyBase::TPfkeyBase()
 : iMsg( 0 )
    {    
    }

TPfkeyAssociation::TPfkeyAssociation() 
 : iExt( 0 )
     {     
     }

TPfkeyLifetime::TPfkeyLifetime()
 : iExt( 0 )
     {     
     }

TPfkeyAddress::TPfkeyAddress()
 : iExt(0), iAddr(0)
     {     
     }

EXPORT_C const TInetAddr& TPfkeyAddress::Address() const
    {
    return *iAddr;
    }

TPfkeyKey::TPfkeyKey()
 : iExt( 0 )
     {     
     }

TPfkeyIdentity::TPfkeyIdentity()
 : iExt( 0 )
     {     
     }

TPfkeySensitivity::TPfkeySensitivity()
 : iExt(0)
     {     
     }

TPfkeyProposal::TPfkeyProposal()
 : iExt( 0 ),
   iComb( 0 ),
   iNumComb( 0 )
    {    
    }

TPfkeySupported::TPfkeySupported()
 : iExt( 0 ),
   iAlg( 0 ),
   iNumAlg( 0 )
       {       
       }

void TPfkeySupportedAuth::AlgString( TDes &aStr,
                                     TUint8 aAlg ) const
       {
       Alg2String( aStr, aAlg ); 
       }

void TPfkeySupportedAuth::Alg2String(TDes &aStr, TUint8 aAlg)
    {
    switch (aAlg)
        {
    case SADB_AALG_MD5HMAC:
        aStr.Append(_L("md5hmac"));
        break;
    case SADB_AALG_SHA1HMAC:
        aStr.Append(_L("sha1hmac"));
        break;
    default:
        aStr.AppendFormat(_L("%d"), (int)aAlg);
        }
    }

void TPfkeySupportedEncrypt::AlgString( TDes &aStr,
                                        TUint8 aAlg ) const
       {
       Alg2String( aStr, aAlg );
       }

void TPfkeySupportedEncrypt::Alg2String(TDes &aStr, TUint8 aAlg)
    {
    switch (aAlg)
        {
    case SADB_EALG_DESCBC:
        aStr.Append(_L("descbc"));
        break;
    case SADB_EALG_3DESCBC:
        aStr.Append(_L("3descbc"));
        break;
    case SADB_EALG_NULL:
        aStr.Append(_L("null"));
        break;
    case 4:
        aStr.Append(_L("rc5"));
        break;
    case 5:
        aStr.Append(_L("idea"));
        break;
    case 6:
        aStr.Append(_L("cast"));
        break;
    case 7:
        aStr.Append(_L("blowfish"));
        break;
    case 8:
        aStr.Append(_L("3idea"));
        break;
    case 9:
        aStr.Append(_L("desiv32"));
        break;
    case 10:
        aStr.Append(_L("rc4"));
        break;
    case 12:
        aStr.Append(_L("aes"));
        break;
        
    default:
        aStr.AppendFormat(_L("%d"), (int)aAlg);
        }
    }

TPfkeySpirange::TPfkeySpirange()
 : iExt( 0 )
     {     
     }

TPfkeyTs::TPfkeyTs()
 : iExt(0)
     {     
     }

EXPORT_C TInt TPfkeyTs::SelectorCount() const
    {
    return (iExt != NULL) ? iExt->sadb_x_ts_numsel : 0;
    }

EXPORT_C const TPfKeySelector& TPfkeyTs::Selector(TInt aIndex) const
    {        
    __ASSERT_DEBUG(iExt != NULL, User::Invariant());
    __ASSERT_DEBUG(iExt->sadb_x_ts_numsel > aIndex, User::Invariant());
    
    TPfKeySelector *selector = (TPfKeySelector*)((TUint8*)iExt + sizeof(struct sadb_x_ts));
    return selector[aIndex];
    }

TPFkeyPrivExt::TPFkeyPrivExt()
 : iExt( 0 )
     {     
     }

//
//  TPfkeyMessage
//
TPfkeyMessage::TPfkeyMessage()
 : iError( KErrNone )
     {     
     }

//
//  Construct TPfkeyMesage from a PF_KEY v2 byte stream (aMsg)
//
TPfkeyMessage::TPfkeyMessage(TPfkeyRecvMsg& aMsg)
    {
	const TUint8 *p = aMsg.Ptr();
	TInt length = aMsg.Length();

	iError = KErrArgument;
	if (length < (TInt)sizeof(sadb_msg))
		return;		// EMSGSIZE (impossible message size)

	// Base Message Header
	iBase.iMsg = (struct sadb_msg *)p;
	if (iBase.iMsg->sadb_msg_version != PF_KEY_V2)
		return;		// EINVAL
	// SADB_ACQUIRE response can have sadb_msg_errno set to non-zero value  
	 if (iBase.iMsg->sadb_msg_errno && (iBase.iMsg->sadb_msg_type != SADB_ACQUIRE))
	 	return;                   // EINVAL (should be set zero by sender) 		
	if (iBase.iMsg->sadb_msg_len * 8 != length)
		return;		// EMSGSIZE (incorrect message length)
	// SADB_ACQUIRE response can have sadb_msg_reserved set to non-zero value            
	if (iBase.iMsg->sadb_msg_reserved && (iBase.iMsg->sadb_msg_type != SADB_ACQUIRE))		
		return;		// EINVAL (unused parts must be zeroed)
	p += sizeof(struct sadb_msg);
	length -= sizeof(struct sadb_msg);

	// Extension headers
	// Some general rules:
	// - only one instance of an extension type is valid
	while (length > 0)
		{
		struct sadb_ext *ext = (struct sadb_ext *)p;
		int ext_len = ext->sadb_ext_len;
		int data_len, data_len2;

		if (ext_len < 1)
			return;		// EINVAL (bad message format)
		ext_len *= 8;
		if (ext_len > length)
			return;		// EINVAL
		switch (ext->sadb_ext_type)
        {
            case SADB_EXT_RESERVED:
                return;     // EINVAL (bad mesage format)

            case SADB_EXT_SA:
                if (iSa.iExt)
                    return; // EINVAL
                iSa.iExt = (struct sadb_sa *)p;
                break;

            case SADB_EXT_LIFETIME_CURRENT:
                if (iCurrent.iExt)
                    return; // EINVAL;
                iCurrent.iExt = (struct sadb_lifetime *)p;
                break;

            case SADB_EXT_LIFETIME_HARD:
                if (iHard.iExt)
                    return;
                iHard.iExt = (struct sadb_lifetime *)p;
                break;

            case SADB_EXT_LIFETIME_SOFT:
                if (iSoft.iExt)
                    return;
                iSoft.iExt = (struct sadb_lifetime *)p;
                break;

            case SADB_EXT_ADDRESS_SRC:
                if (iSrcAddr.iExt)
                    return;
                if (ext_len != sizeof(struct sadb_address) + sizeof(TInetAddr))
                    return;
                iSrcAddr.iExt = (struct sadb_address *)p;
                iSrcAddr.iAddr = (TInetAddr *)(p + sizeof(struct sadb_address));
                break;

            case SADB_EXT_ADDRESS_DST:
                if (iDstAddr.iExt)
                    return;
                if (ext_len != sizeof(struct sadb_address) + sizeof(TInetAddr))
                    return;
                iDstAddr.iExt = (struct sadb_address *)p;
                iDstAddr.iAddr = (TInetAddr *)(p + sizeof(struct sadb_address));
                break;

            case SADB_EXT_ADDRESS_PROXY:
                if (iProxyAddr.iExt)
                    return;
                if (ext_len != sizeof(struct sadb_address) + sizeof(TInetAddr))
                    return;
                iProxyAddr.iExt = (struct sadb_address *)p;
                iProxyAddr.iAddr = (TInetAddr *)(p + sizeof(struct sadb_address));
                break;

            case SADB_EXT_KEY_AUTH:
                if (iAuthKey.iExt)
                    return;
                iAuthKey.iExt = (struct sadb_key *)p;
                data_len = (iAuthKey.iExt->sadb_key_bits + 7) / 8;
                if (data_len == 0 || data_len + (int)sizeof(struct sadb_key) > ext_len)
                    return;
                iAuthKey.iData.Set(p + sizeof(struct sadb_key), data_len);
                    break;

            case SADB_EXT_KEY_ENCRYPT:
                if (iEncryptKey.iExt)
                    return;
                iEncryptKey.iExt = (struct sadb_key *)p;
                data_len = (iEncryptKey.iExt->sadb_key_bits + 7) / 8;
                if (data_len == 0 || data_len + (int)sizeof(struct sadb_key) > ext_len)
                    return;
                iEncryptKey.iData.Set(p + sizeof(struct sadb_key), data_len);
                break;

            case SADB_EXT_IDENTITY_SRC:
                {
                if (iSrcIdent.iExt)
                    return;
                iSrcIdent.iExt = (struct sadb_ident *)p;
                data_len = ext_len - sizeof(struct sadb_ident);
                if (data_len < 0)
                    return;
                iSrcIdent.iData.Set(p + sizeof(struct sadb_ident), data_len);
                TInt i = iSrcIdent.iData.Locate((TChar)0);
                if (i >= 0)
                    iSrcIdent.iData.Set(iSrcIdent.iData.Ptr(), i);
                break;
                }

            case SADB_EXT_IDENTITY_DST:
                {
                if (iDstIdent.iExt)
                    return;
                iDstIdent.iExt = (struct sadb_ident *)p;
                data_len = ext_len - sizeof(struct sadb_ident);
                if (data_len < 0)
                    return;
                iDstIdent.iData.Set(p + sizeof(struct sadb_ident), data_len);
                TInt i = iDstIdent.iData.Locate((TChar)0);
                if (i >= 0)
                    iDstIdent.iData.Set(iDstIdent.iData.Ptr(), i);
                break;
                }

            case SADB_EXT_SENSITIVITY:
                if (iSensitivity.iExt)
                    return;
                iSensitivity.iExt = (struct sadb_sens *)p;
                data_len = iSensitivity.iExt->sadb_sens_sens_len * 8;
                iSensitivity.iSensBitmap.Set(p + sizeof(struct sadb_sens), data_len);
                data_len2 = iSensitivity.iExt->sadb_sens_integ_len * 8;
                iSensitivity.iSensBitmap.Set(p + (sizeof(struct sadb_sens) + data_len),
                         data_len2);
                if (data_len + data_len2 + (int)sizeof(struct sadb_sens) > ext_len)
                    return;
                break;

            case SADB_EXT_PROPOSAL:
                if (iProposal.iExt)
                    return;
                iProposal.iExt = (struct sadb_prop *)p;
                iProposal.iNumComb = (ext_len - sizeof(struct sadb_prop)) / sizeof(struct sadb_comb);
                iProposal.iComb = (struct sadb_comb *)(p + sizeof(struct sadb_prop));
                break;

            case SADB_EXT_SUPPORTED_AUTH:
                if (iAuthAlgs.iExt)
                    return;
                iAuthAlgs.iExt = (struct sadb_supported *)p;
                iAuthAlgs.iNumAlg = (ext_len - sizeof(struct sadb_supported)) / sizeof(struct sadb_alg);
                iAuthAlgs.iAlg = (struct sadb_alg *)(p + sizeof(struct sadb_supported));
                break;

            case SADB_EXT_SUPPORTED_ENCRYPT:
                if (iEncryptAlgs.iExt)
                    return;
                iEncryptAlgs.iExt = (struct sadb_supported *)p;
                iEncryptAlgs.iNumAlg = (ext_len - sizeof(struct sadb_supported)) / sizeof(struct sadb_alg);
                iEncryptAlgs.iAlg = (struct sadb_alg *)(p + sizeof(struct sadb_supported));
                break;

            case SADB_EXT_SPIRANGE:
                if (iSpirange.iExt)
                    return;
                iSpirange.iExt = (struct sadb_spirange *)p;
                break;
                
			/**---------------------------------------------------------------
			 *
			 *  PFKEY API general private extension.
			 *
			 *----------------------------------------------------------------*/                
            case SADB_PRIV_GENERIC_EXT:
                if (iPrivateExtension.iExt)
                    return;
                iPrivateExtension.iExt = (struct sadb_gen_ext *)p;
                data_len = (ext_len - sizeof(struct sadb_gen_ext));
                if (data_len > ext_len)
                    return;
                iPrivateExtension.iData.Set(p + sizeof(struct sadb_gen_ext), data_len);
                break;
                

            case SADB_X_EXT_TS:
                if (iTs.iExt)
                    return;
                iTs.iExt = (struct sadb_x_ts *)p;                                
                break;

            default:
                // Unknown extensions must be ignored, not an error!
                break;
            }
            p += ext_len;
			length -= ext_len;
        }
	if (length != 0)
		return;

    iError = KErrNone;  // Message unpacked successfully
    }
