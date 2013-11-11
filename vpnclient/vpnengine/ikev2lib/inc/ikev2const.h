/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: IKEv2 constants.
*
*/


#ifndef _IKEV2CONST_H_
#define _IKEV2CONST_H_

#include <e32def.h>

//
// All Headers with values in network byte order
//
#define MIN_IKEV2_PAYLOAD_SIZE 4


//Version 
#define MAJORV2 2
#define MAJORV1 1
#define MINOR   0
#define MAJOR2MINOR0  (MAJORV2 << 4)

//
//  IKEv2 HEADER FLAGS (bits 0-2 reserved for IKEv1)
//
#define IKEV2_INITIATOR      0x8     // Original Initiator Bit
#define IKEV2_HIGHER_VERSION 0x10    // Higher version supported
#define IKEV2_RESPONSE_MSG   0x20    // Current message is a response

//
//  IKEv2 EXCHANGE TYPES 
//
#define IKE_SA_INIT          34   
#define IKE_AUTH             35
#define CREATE_CHILD_SA      36
#define INFORMATIONAL        37

//
//Payload types
//
#define IKEV2_PAYLOAD_NONE   0       // (Terminator)
#define IKEV2_PAYLOAD_SA     33      // Security Association
#define IKEV2_PAYLOAD_PROP   2       // Proposal
#define IKEV2_PAYLOAD_TRANS  3       // Transform
#define IKEV2_PAYLOAD_KE     34      // Key Exchange
#define IKEV2_PAYLOAD_ID_I   35      // Identification (Initiator)
#define IKEV2_PAYLOAD_ID_R   36      // Identification (Responder)
#define IKEV2_PAYLOAD_CERT   37      // Certificate
#define IKEV2_PAYLOAD_CR     38      // Certificate Request
#define IKEV2_PAYLOAD_AUTH   39      // Authentication
#define IKEV2_PAYLOAD_NONCE  40      // Nonce
#define IKEV2_PAYLOAD_NOTIF  41      // Notification
#define IKEV2_PAYLOAD_DELETE 42      // Delete
#define IKEV2_PAYLOAD_VID    43      // Vendor ID
#define IKEV2_PAYLOAD_TS_I   44      // Traffic selector (initiator)
#define IKEV2_PAYLOAD_TS_R   45      // Traffic selector (Responder)
#define IKEV2_PAYLOAD_ENCR   46      // Encrypted
#define IKEV2_PAYLOAD_CONFIG 47      // Configuration
#define IKEV2_PAYLOAD_EAP    48      // Extensible Authentication

#define IKEV2_PAYLOAD_PRIVATE  128 // Private use (up to 255)

//
// Critical bit in general paylaod header
// Encrypted bit is an internal definition to indicate that received
// payload was encrypted (=received inside encrypted payload)
//
#define IKEV2_PL_CRITICAL    0x80
#define IKEV2_PL_ENCRYPTED   0x01
#define IKEV2_PL_SELECTED    0x02 

//
// Protocol ID values
//
#define IKEV2_PROT_NONE      0
#define IKEV2_PROTOCOL       1
#define IKEV2_IPSEC_AH       2
#define IKEV2_IPSEC_ESP      3

//
// Transform type values
//
#define IKEV2_ENCR           1   // IKE and ESP
#define IKEV2_PRF            2   // IKE
#define IKEV2_INTEG          3   // IKE, AH, optional in ESP
#define IKEV2_DH             4   // IKE, optional AH and ESP
#define IKEV2_ESN            5   // optional AH and ESP

//
// Transform ID values for encryption algorithm type
//

#define  ENCR_DES_IV64       1   //  RFC1827
#define  ENCR_DES            2   //  RFC2405
#define  ENCR_3DES           3   //  RFC2451
#define  ENCR_RC5            4   //  RFC2451
#define  ENCR_IDEA           5   //  RFC2451
#define  ENCR_CAST           6   //  RFC2451
#define  ENCR_BLOWFISH       7   //  RFC2451
#define  ENCR_3IDEA          8   //  RFC2451
#define  ENCR_DES_IV32       9   //
#define  ENCR_NULL          11   //  RFC2410
#define  ENCR_AES_CBC       12   //  RFC3602
#define  ENCR_AES_CTR       13   //  RFC3664

//
// Attribute type values (used only with encryption algorithm transform)
//
#define IKEV2_ENCR_KEY_LTH  14   // 

//
// Transform ID values for Pseudo-random Function type
//
#define  PRF_HMAC_MD5        1   //  RFC2104
#define  PRF_HMAC_SHA1       2   //  RFC2104
#define  PRF_HMAC_TIGER      3   //  RFC2104
#define  PRF_AES128_CBC      4   //  RFC3664

//
// Transform ID values for Integrity Algorithm type
//
#define  AUTH_HMAC_MD5_96    1   //  RFC2403
#define  AUTH_HMAC_SHA1_96   2   //  RFC2403
#define  AUTH_DES_MAC        3   //
#define  AUTH_KPDK_MD5       4   //  RFC1826
#define  AUTH_AES_XCBC_96    5   //  RFC3566

//
// Transform ID values for Diffie-Hellman group type
//
#define  DH_GROUP_768        1   //  Appendix B
#define  DH_GROUP_1024       2   //  Appendix B
#define  DH_GROUP_1536       5   //  RFC3526
#define  DH_GROUP_2048      14   //  RFC3526


//
//NOTIFY MESSAGES - ERROR TYPES
//
#define UNSUPPORTED_CRITICAL_PAYLOAD     1
#define INVALID_IKE_SPI                  4
#define INVALID_MAJOR_VERSION            5
#define INVALID_SYNTAX                   7
#define INVALID_MESSAGE_ID               9
#define INVALID_SPI                     11
#define NO_PROPOSAL_CHOSEN              14
#define INVALID_KE_PAYLOAD              17
#define AUTHENTICATION_FAILED           24
#define SINGLE_PAIR_REQUIRED            34
#define NO_ADDITIONAL_SAS               35
#define INTERNAL_ADDRESS_FAILURE        36
#define FAILED_CP_REQUIRED              37
#define TS_UNACCEPTABLE                 38
#define INVALID_SELECTORS               39

//
// NOTIFY MESSAGES - STATUS TYPES
//
#define INITIAL_CONTACT                 16384
#define SET_WINDOW_SIZE                 16385
#define ADDITIONAL_TS_POSSIBLE          16386
#define IPCOMP_SUPPORTED                16387
#define NAT_DETECTION_SOURCE_IP         16388
#define NAT_DETECTION_DESTINATION_IP    16389
#define COOKIE                          16390
#define USE_TRANSPORT_MODE              16391
#define HTTP_CERT_LOOKUP_SUPPORTED      16392
#define REKEY_SA                        16393
#define ESP_TFC_PADDING_NOT_SUPPORTED   16394
#define NON_FIRST_FRAGMENTS_ALSO        16395

//
// NOTIFY MESSAGES CODES FOR MOBIKE
//
#define MOBIKE_SUPPORTED                16396
#define ADDITIONAL_IPV4_ADDRESS         16397
#define ADDITIONAL_IPV6_ADDRESS         16398
#define UPDATE_SA_ADDRESS               16400
#define COOKIE2                         16401
#define NAT_PREVENTION                  16402 //Is this the same as NO_NATS_ALLOWED?
// NOTIFY MESSAGES ERROR CODES FOR MOBIKE
#define UNACCPETABLE_ADDRESSES          9500
#define NAT_PREVENTED                   9501

//
// IKEv2 Identity type codes
//
#define ID_NOT_DEFINED                  0
#define ID_IPV4_ADDR                    1
#define ID_FQDN                         2
#define ID_RFC822_ADDR                  3
#define ID_IPV4_ADDR_SUBNET             4      // For IPSEC ID:s
#define ID_IPV6_ADDR                    5
#define ID_IPV6_ADDR_SUBNET             6      // For IPSEC ID:s
#define ID_DER_ASN1_DN                  9
#define ID_KEY_ID                      11

//
// IKEv2 Authentication methods 
//
#define RSA_DIGITAL_SIGN                1
#define PRESHARED_KEY                   2
#define DSS_DIGITAL_SIGN                3

//
// IKEv2 Traffic selector type values
//
#define TS_IPV4_ADDR_RANGE              7
#define TS_IPV6_ADDR_RANGE              8

//
// IKEv2 CFG Types (For Config payload)
//
#define CFG_REQUEST                     1
#define CFG_REPLY                       2
#define CFG_SET                         3
#define CFG_ACK                         4

//
// IKEv2 Configuration attributes
//
#define INTERNAL_IP4_ADDRESS            1 // 0 or 4 octets
#define INTERNAL_IP4_NETMASK            2 // 0 or 4 octets
#define INTERNAL_IP4_DNS                3 // 0 or 4 octets
#define INTERNAL_IP4_NBNS               4 // 0 or 4 octets
#define INTERNAL_ADDRESS_EXPIRY         5 // 0 or 4 octets
#define INTERNAL_IP4_DHCP               6 // 0 or 4 octets
#define APPLICATION_VERSION             7 // 0 or more
#define INTERNAL_IP6_ADDRESS            8 // 0 or 16
#define INTERNAL_IP6_DNS               10 // 0 or 16 octets
#define INTERNAL_IP6_NBNS              11 // 0 or 16 octets
#define INTERNAL_IP6_DHCP              12 // 0 or 16 octets
#define INTERNAL_IP4_SUBNET            13 // 0 or 8 octets
#define SUPPORTED_ATTRIBUTES           14 // Multiple of 2
#define INTERNAL_IP6_SUBNET            15 // 17 octets

//
// IKEv2 Certificate Encoding codes
//
#define PKCS7_WRAPPED_X509_CERT         1
#define PGP_CERTIFICATE                 2
#define DNS_SIGNED_KEY                  3
#define X509_CERTIFICATE_SIGN           4
#define KERBEROS_TOKEN                  6
#define CERT_REVOCATION_LIST            7
#define AUTHORITY_REVOCATION_LIST       8
#define SPKI_CERTIFICATE                9
#define X509_CERTIFICATE_ATTRIBUTE     10
#define RAW_RSA_KEY                    11
#define HASH_AND_URL_X509_CERT         12
#define HASH_AND_URL_X509_BUNDLE       13

#endif