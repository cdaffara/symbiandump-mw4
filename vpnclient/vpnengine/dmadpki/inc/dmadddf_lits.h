/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   String literals for OMA DM PKI use
*
*/



#ifndef __DMADDDF_LITS_H__
#define __DMADDDF_LITS_H__


//------------------------------------------------------------------------------------------------
// DDF version
//------------------------------------------------------------------------------------------------
_LIT8(KDmAdDdfVersion, "1.0"); 

//------------------------------------------------------------------------------------------------
// ./NokiaPKI Type property
//------------------------------------------------------------------------------------------------
_LIT8(KDmAdNokiaPkiRootTypeProperty, "com.nokia.devman/1.0/pki"); 

//------------------------------------------------------------------------------------------------
// URIs
//------------------------------------------------------------------------------------------------
_LIT8(KDmAdOwnAdUriForGetLuidAllocLFix, "NokiaPKI/Cert");  // URI not object group level?
_LIT8(KDmAdOwnAdUriForPrivateKeys, "NokiaPKI/PrivKey");  
_LIT8(KDmAdOwnAdUriForPKCS12, "NokiaPKI/PKCS12");

    //------- root -----------------
_LIT8(KDmAdNodeNokiaPki, "NokiaPKI");

    //------- common -----------------
_LIT8(KDmAdNodeRt, "");
_LIT8(KDmAdNodeGeneral, "General");

_LIT8(KDmAdLeafCertApplications, "CertApplications");
_LIT8(KDmAdLeafLogon, "Logon");
_LIT8(KDmAdLeafLogoff, "Logoff");
_LIT8(KDmAdLeafKeyStore, "KeyStore");
_LIT8(KDmAdLeafCertStore, "CertStore");


_LIT8(KDmAdLeafContent, "Content");
_LIT8(KDmAdLeafSubjectName, "SubjectName");
_LIT8(KDmAdLeafKeyId, "KeyID");
_LIT8(KDmAdLeafKeyLength, "KeyLength");
_LIT8(KDmAdLeafKeyURI, "KeyURI");

    //------- Cert -----------------
_LIT8(KDmAdNodeCert, "Cert");
_LIT8(KDmAdLeafType, "Type");
_LIT8(KDmAdLeafFormat, "Format");
_LIT8(KDmAdLeafSerialNumber, "SerialNumber");
_LIT8(KDmAdLeafIssuerName, "IssuerName");
_LIT8(KDmAdLeafFingerprintAlg, "FingerprintAlg");
_LIT8(KDmAdLeafFingerprintValue, "FingerprintValue");
_LIT8(KDmAdLeafValidityBegin, "ValidityBegin");
_LIT8(KDmAdLeafValidityEnd, "ValidityEnd");
_LIT8(KDmAdLeafSubjectAltName, "SubjectAltName");
_LIT8(KDmAdLeafKeyUsage, "KeyUsage");
_LIT8(KDmAdLeafDeletable, "Deletable");
_LIT8(KDmAdLeafTrusted, "Trusted");
_LIT8(KDmAdLeafApplicability, "Applicability");

    //------- CertReq -----------------
_LIT8(KDmAdNodeCertReq, "CertReq");
_LIT8(KDmAdLeafRfc822Name, "RFC822Name");

    //------- PrivKey -----------------
_LIT8(KDmAdNodePrivKey, "PrivKey");
_LIT8(KDmAdLeafKeyType, "KeyType");

    //------- PKCS#12 -----------------
_LIT8(KDmAdNodePKCS12, "PKCS12");
_LIT8(KDmAdLeafPKCS12Password, "Password");
// Note: Nodes "Deletable" (KDmAdLeafDeletable),
// "Applicability" (KDmAdLeafApplicability) and 
// "Content" (KDmAdLeafContent) are valid in PKCS#12 
// context as well.

//------------------------------------------------------------------------------------------------
// URI descriptions
//------------------------------------------------------------------------------------------------
    //------- root -----------------
_LIT8(KDmAdDescNodeNokiaPki, "NokiaPKI");

    //------- common -----------------
_LIT8(KDmAdDescNodeRt, "Placeholder for one set of settings");
_LIT8(KDmAdDescNodeGeneral, "General");

_LIT8(KDmAdDescLeafCertApplications, "CertApplications");
_LIT8(KDmAdDescLeafLogon, "Logon");
_LIT8(KDmAdDescLeafLogoff, "Logoff");
_LIT8(KDmAdDescLeafKeyStore, "KeyStore");
_LIT8(KDmAdDescLeafCertStore, "CertStore");

_LIT8(KDmAdDescLeafType, "Type");
_LIT8(KDmAdDescLeafContent, "Content");
_LIT8(KDmAdDescLeafStatus, "Status");
_LIT8(KDmAdDescLeafIssuerName, "IssuerName");
_LIT8(KDmAdDescLeafSubjectName, "SubjectName");
_LIT8(KDmAdDescLeafSubjectAltName, "SubjectAltName");
_LIT8(KDmAdDescLeafRfc822Name, "RFC822Name");
_LIT8(KDmAdDescLeafKeyId, "KeyID");
_LIT8(KDmAdDescLeafKeyLength, "KeyLength");

    //------- Cert -----------------
_LIT8(KDmAdDescNodeCert, "Cert");
_LIT8(KDmAdDescLeafFormat, "Format");
_LIT8(KDmAdDescLeafSerialNumber, "SerialNumber");
_LIT8(KDmAdDescLeafFingerprintAlg, "FingerprintAlg");
_LIT8(KDmAdDescLeafFingerprintValue, "FingerprintValue");
_LIT8(KDmAdDescLeafValidityBegin, "ValidityBegin");
_LIT8(KDmAdDescLeafValidityEnd, "ValidityEnd");
_LIT8(KDmAdDescLeafKeyURI, "KeyURI");
_LIT8(KDmAdDescLeafKeyUsage, "KeyUsage");
_LIT8(KDmAdDescLeafDeletable, "Deletable");
_LIT8(KDmAdDescLeafTrusted, "Trusted");
_LIT8(KDmAdDescLeafApplicability, "Applicability");

    //------- CertReq -----------------
_LIT8(KDmAdDescNodeCertReq, "CertReq");
_LIT8(KDmAdDescLeafKeyUri, "KeyURI");

    //------- PrivKey -----------------
_LIT8(KDmAdDescNodePrivKey, "PrivKey");
_LIT8(KDmAdDescLeafKeyType, "KeyType");
_LIT8(KDmAdDescLeafKeyFormat, "KeyFormat");

    //------- PKCS#12 -----------------
_LIT8(KDmAdDescNodePKCS12, "PKCS12");
_LIT8(KDmAdDescLeafPKCS12Password, "Password");

//------------------------------------------------------------------------------------------------
// ChildURIListL constants
//------------------------------------------------------------------------------------------------
_LIT8(KDmAdListOfNokiaPkiChildren, "Logon/Logoff/KeyStore/CertStore/General/Cert/CertReq/PrivKey/PKCS12");
_LIT8(KDmAdListOfCertXChildren, "Type/Format/SerialNumber/IssuerName/FingerprintAlg/FingerprintValue/ValidityBegin/ValidityEnd/SubjectName/SubjectAltName/KeyURI/KeyID/KeyUsage/Deletable/Trusted/Applicability/Content");
_LIT8(KDmAdListOfCertReqXChildren, "SubjectName/RFC822Name/KeyURI/KeyLength/Content");
_LIT8(KDmAdListOfPrivKeyXChildren, "KeyType/KeyLength/KeyID");

_LIT8(KDmAdListOfPKCS12XChildren, "Password/Deletable/Applicability/Content");

_LIT8(KDmAdListOfPkiGeneralChildren, "CertApplications");

//------------------------------------------------------------------------------------------------
// TDFFormat - EBool
//------------------------------------------------------------------------------------------------
_LIT8(KDmAdDfFormatBoolTrue, "True" );
_LIT8(KDmAdDfFormatBoolFalse, "False" );

#endif
