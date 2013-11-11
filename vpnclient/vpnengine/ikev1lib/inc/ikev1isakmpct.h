/*
* Copyright (c) 1999-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  key management daemon ISAKMP constants
*
*/


//
// ISAKMP constants defines in RFC 2409 (IKE) and 2407(IPSEC DOI)
//

#ifndef ISAKMPCT_H
#define ISAKMPCT_H

//
// All Headers with values in network byte order
//

#define DEFAULT_IPSEC_SA_LIFETIME   28000   //RFC 2407 (DOI)
#define MIN_ISAKMP_PAYLOAD_SIZE 4

//Version of the implementation 1.0
#define MAJOR   1
#define MINOR   0

//
//  ISAKMP HEADER 
//
//Flags
#define ISAKMP_HDR_EFLAG    0x1     // Encryption Bit
#define ISAKMP_HDR_CFLAG    0x2     // Commit Bit
#define ISAKMP_HDR_AFLAG    0x4     // Authentication Only Bit

//Payload types
#define ISAKMP_PAYLOAD_NONE 0       // (Terminator)
#define ISAKMP_PAYLOAD_SA   1       // Security Association
#define ISAKMP_PAYLOAD_P    2       // Proposal
#define ISAKMP_PAYLOAD_T    3       // Transform
#define ISAKMP_PAYLOAD_KE   4       // Key Exchange
#define ISAKMP_PAYLOAD_ID   5       // Identification
#define ISAKMP_PAYLOAD_CERT 6       // Certificate
#define ISAKMP_PAYLOAD_CR   7       // Certificate Request
#define ISAKMP_PAYLOAD_HASH 8       // Hash
#define ISAKMP_PAYLOAD_SIG  9       // Signature
#define ISAKMP_PAYLOAD_NONCE 10     // Nonce
#define ISAKMP_PAYLOAD_NOTIF    11      // Notification
#define ISAKMP_PAYLOAD_D    12      // Delete
#define ISAKMP_PAYLOAD_VID  13      // Vendor ID
#define ISAKMP_PAYLOAD_PRIVATE  128 // Private use (up to 255)

#define ISAKMP_EXCHANGE_NONE    0   // None
#define ISAKMP_EXCHANGE_BASE    1   // Base
#define ISAKMP_EXCHANGE_ID      2   // Identity Protection (Main mode in IKE)
#define ISAKMP_EXCHANGE_AUTH    3   // Authentication Only
#define ISAKMP_EXCHANGE_AGGR    4   // Agressive
#define ISAKMP_EXCHANGE_INFO    5   // Informational
//#define   ISAKMP_EXCHANGE_DOI     32  // DOI Specific (32..255)
//Additional Exchanges Defined
#define IKE_QUICK_MODE          32  //Quick Mode
#define IKE_NEW_GROUP_MODE      33  //New Group Mode


//Protocol number for IPSEC DOI (=1) //ProtocolId in Proposal Payload
#define PROTO_ISAKMP    1
#define PROTO_IPSEC_AH  2
#define PROTO_IPSEC_ESP 3
#define PROTO_IPCOMP    4

//Transform ID's    (RFC 2407)
//for ISAKMP
#define KEY_IKE     1   //Oakley
//for IPSEC_AH
#define AH_MD5      2   //MUST
#define AH_SHA      3   //MUST
#define AH_DES      4   
//for IPSEC_ESP
#define ESP_DES_IV64            1
#define ESP_DES                 2   //MUST
#define ESP_3DES                3   //Strongly encouraged
#define ESP_RC5                 4
#define ESP_IDEA                5
#define ESP_CAST                6
#define ESP_BLOWFISH            7
#define ESP_3IDEA               8
#define ESP_DES_IV32            9
#define ESP_RC4                 10
#define ESP_NULL                11  //MUST
//for IPCOMP
#define IPCOMP_OUI      1
#define IPCOMP_DEFLATE  2
#define IPCOMP_LZS      3   

//SA ATTRIBUTES Identifiers Phase I (RFC 2409)
#define OAKLEY_ATTR_TYPE_ENCR_ALG       1
#define OAKLEY_ATTR_TYPE_HASH_ALG       2
#define OAKLEY_ATTR_TYPE_AUTH_METH      3
#define OAKLEY_ATTR_TYPE_GROUP_DESC     4
#define OAKLEY_ATTR_TYPE_GROUP_TYPE     5
#define OAKLEY_ATTR_TYPE_GROUP_PRIME    6
#define OAKLEY_ATTR_TYPE_GROUP_GEN1     7
#define OAKLEY_ATTR_TYPE_GROUP_GEN2     8
#define OAKLEY_ATTR_TYPE_GROUP_CRVA     9   //Group curve A
#define OAKLEY_ATTR_TYPE_GROUP_CRVB     10  //Group curve B
#define OAKLEY_ATTR_TYPE_LIFE_TYPE      11
#define OAKLEY_ATTR_TYPE_LIFE_DUR       12
#define OAKLEY_ATTR_TYPE_PRF            13
#define OAKLEY_ATTR_TYPE_KEY_LEN        14
#define OAKLEY_ATTR_TYPE_FIELD_SIZE     15
#define OAKLEY_ATTR_TYPE_GROUP_ORDER    16

//Values for each of the attributes
//encription algorithms
#define DES_CBC             1
#define IDEA_CBC            2
#define BLOWFISH_CBC        3
#define RC5_R16_B64_CBC     4
#define DES3_CBC            5
#define CAST_CBC            6
#define AES_CBC             7   //From <draft-ietf-ipsec-ciph-aes-cbc-04.txt>

#define ESP_DES_CBC         2
#define ESP_3DES_CBC        3
#define ESP_NULL           11
#define ESP_AES_CBC        12

//hash algorithms
#define HASH_MD5                    1
#define HASH_SHA1                   2
#define HASH_TIGER               3
//authentication methods
#define PRE_SHARED          1   //pre-shared key
#define DSS_SIG             2   //DSS signatures
#define RSA_SIG             3   //RSA signatures
#define RSA_ENCR            4   //Encryption with RSA
#define RSA_REV_ENCR        5   //Revised encryption with RSA

//group descriptions
#define MODP_768        1       //default 768-bit MODP group (section 6.1)
#define MODP_1024       2       //alternate 1024-bit MODP group (section 6.2)
#define EC2N_155        3       //EC2N group on GP[2^155] (section 6.3)         3
#define EC2N_185        4       //EC2N group on GP[2^185] (section 6.4)         4
#define MODP_1536       5       //alternate 1536-bit MODP group (draft-ietf-ipsec-ike-modp-groups-04.txt)
#define MODP_2048      14       //IETF RFC 3526 
#define OAKLEY_DEFAULT_GROUP MODP_768   //default group. Not sure if needed.
//Group Types
#define MODP        1   //(modular exponentiation group)
#define ECP         2   //(elliptic curve group over GF[P])
#define EC2N        3   //(elliptic curve group over GF[2^N])
//Life Type
#define SECONDS     1
#define KBYTES      2

//PRF
#define OAKLEY_PRF_3DES_CBC_MAC         1   //Only one implemented by now

//Current DOI's in use
#define IPSEC_DOI   1

//SA ATTRIBUTES Identifiers Phase II (RFC 2407) iDOI
#define DOI_ATTR_TYPE_LIFE_TYPE     1
#define DOI_ATTR_TYPE_LIFE_DUR      2
#define DOI_ATTR_TYPE_GROUP_DESC    3
#define DOI_ATTR_TYPE_ENC_MODE      4
#define DOI_ATTR_TYPE_AUTH_ALG      5
#define DOI_ATTR_TYPE_KEY_LEN       6
#define DOI_ATTR_TYPE_KEY_ROUNDS    7
#define DOI_ATTR_TYPE_COMP_DIC_SIZE 8
#define DOI_ATTR_TYPE_COMP_PRIV_ALG 9
//Values for each of the attributes of PHASE_II

//Life type as Phase I
//Encapsulation Mode
#define DOI_TUNNEL      1
#define DOI_TRANSPORT   2
//Authentication Algorithm values
#define DOI_HMAC_MD5    1
#define DOI_HMAC_SHA    2
#define DOI_DES_MAC     3
#define DOI_KPDK        4

#define IPSEC_SIT_IDENTITY_ONLY 0x1     //As defined in RFC 2407 DOI for ISAKMP
#define IPSEC_SIT_SECRECY       0x2
#define IPSEC_SIT_INTEGRITY     0x4

//ID Types for IPSEC DOI
#define ID_IPV4_ADDR                        1
#define ID_FQDN                             2
#define ID_USER_FQDN                        3
#define ID_IPV4_ADDR_SUBNET                 4
#define ID_IPV6_ADDR                        5
#define ID_IPV6_ADDR_SUBNET                 6
#define ID_IPV4_ADDR_RANGE                  7
#define ID_IPV6_ADDR_RANGE                  8
#define ID_DER_ASN1_DN                      9
#define ID_DER_ASN1_GN                      10
#define ID_KEY_ID                           11

//Certificate Types
#define NONE                0
#define PKCS                1   //PKCS #7 wrapped X.509 certificate
#define PGP                 2   //PGP Certificate
#define DNS                 3   //DNS Signed Key
#define X509_CERT_SIG       4   //X.509 Certificate - Signature
#define X509_CERT_KE        5   //X.509 Certificate - Key Exchange
#define KERBEROS            6   //Kerberos Tokens
#define CRL                 7   //Certificate Revocation List (CRL)
#define ARL                 8   //Authority Revocation List (ARL)8
#define SPKI                9   //SPKI Certificate
#define X509_CERT_ATTR      10  //X.509 Certificate - Attribute

//NOTIFY MESSAGES - ERROR TYPES
#define INVALID_PAYLOAD_TYPE             1
#define DOI_NOT_SUPPORTED                2
#define SITUATION_NOT_SUPPORTED          3
#define INVALID_COOKIE                   4
#define INVALID_MAJOR_VERSION            5
#define INVALID_MINOR_VERSION            6
#define INVALID_EXCHANGE_TYPE            7
#define INVALID_FLAGS                    8
#define INVALID_MESSAGE_ID               9
#define INVALID_PROTOCOL_ID             10
#define INVALID_SPI                     11
#define INVALID_TRANSFORM_ID            12
#define ATTRIBUTES_NOT_SUPPORTED        13
#define NO_PROPOSAL_CHOSEN              14
#define BAD_PROPOSAL_SYNTAX             15
#define PAYLOAD_MALFORMED               16
#define INVALID_KEY_INFORMATION         17
#define INVALID_ID_INFORMATION          18
#define INVALID_CERT_ENCODING           19
#define INVALID_CERTIFICATE             20
#define CERT_TYPE_UNSUPPORTED           21
#define INVALID_CERT_AUTHORITY          22
#define INVALID_HASH_INFORMATION        23
#define AUTHENTICATION_FAILED           24
#define INVALID_SIGNATURE               25
#define ADDRESS_NOTIFICATION            26
#define NOTIFY_SA_LIFETIME              27
#define CERTIFICATE_UNAVAILABLE         28
#define UNSUPPORTED_EXCHANGE_TYPE       29
#define UNEQUAL_PAYLOAD_LENGTHS         30

//RESERVED (Future Use)        31 - 8191
//Private Use                8192 - 16383

//NOTIFY MESSAGES - STATUS TYPES 
#define CONNECTED                   16384
//RESERVED (Future Use)   16385 - 24575
//DOI-specific codes     24576 - 32767
#define DOI_RESPONDER_LIFETIME          24576
#define DOI_REPLAY_STATUS               24577
#define DOI_INITIAL_CONTACT             24578
//Private Use            32768 - 40959
//RESERVED (Future Use)  40960 - 65535

//
// Notify message types for Dead Peer Detection (DPD) defined in
// <draft-ietf-ipsec-dpd-04.txt>
//
#define DPD_R_U_THERE                   36136
#define DPD_R_U_THERE_ACK               36137

//
// IKE CRACK constants defines in <draft-harkins-ipsra-crack-00.txt>
//

//authentication method 
#define IKE_A_CRACK         128   //CRACK authentication

//Challenge/Response payload (CHRE)
#define ISAKMP_PAYLOAD_CHRE 128   //CHRE payload


//Legacy Authentication types
#define CRACK_PASSWORD           1

//LAM attributes (in CHRE payload)
#define CRACK_T_USERNAME         16390   //Variable
#define CRACK_T_SECRET           16391   //Variable
#define CRACK_T_DOMAIN           16392   //Variable
#define CRACK_T_PIN              16393   //Variable
#define CRACK_T_CHALLENGE        16394   //Variable
#define CRACK_T_MESSAGE          16395   //Variable
#define CRACK_T_FIN              16396   //Basic 

//CRACK Finish attribute values
#define CRACK_FIN_SUCCESS        1
#define CRACK_FIN_MORE           2

//
// Definitions for Private Internal Address payload
//

// Internal Address payload 
#define ISAKMP_INT_NETWORK  247   //INTNET payload

//Internal Address attributes 
#define PRI_INTERNAL_ADDRESS     24001   //Variable
#define PRI_INTERNAL_DNS         24002   //Variable
#define PRI_INTERNAL_WINS        24003   //Variable

//
// Definitions for expanded Vendor ID payload options
//
#define VENDOR_OPTION_HASH          1       
#define VENDOR_OPTION_NAT_TRAVERSAL 2
#define VENDOR_OPTION_VERSION       3

#define NOKIA_UDP_ENCAPS_PORT       9872
#define UDP_KEEPALIVE_TIME        30  // Default value 30 seconds

// NAT Discovery and NAT original address payloads ( 
#define IETF_NAT_DISCOVERY        130  // 15 in draft version 05 and later
#define IETF_NAT_ORIG_ADDR        131  // 16 in draft version 05 and later
#define IETF_RFC_NAT_DISCOVERY        20  // 15 in draft version 05 and later
#define IETF_RFC_NAT_ORIG_ADDR        21  // 16 in draft version 05 and later

// Encapsulation modes with NAT-traversal 
#define UDP_ENC_TUNNEL            61443   // 3 in draft version 03 and later
#define UDP_ENC_TRANSPORT         61444   // 4 in draft version 03 and later
#define UDP_RFC_ENC_TUNNEL            3   // 3 in draft version 03 and later
#define UDP_RFC_ENC_TRANSPORT         4   // 4 in draft version 03 and later

//
// Definitions related to Extended Authentication (XAUTH) (draft-beaulieu-ike-xauth-02.txt)
// and to The ISAKMP Configuration Method (MODE-CFG) (draft-dukes-ike-mode-cfg-01.txt)
// The ISAKMP Configuration Method defines an ISAKMP exchange called Transaction Exchange.
// Both XAUTH and MODE-CFG uses that ISAKMP exchange. 
//
#define ISAKMP_EXCHANGE_TRANSACT  6   // Transaction exchange

//
// XAUTH Notification via Authentication Method Types
// The following values relate to the ISAKMP authentication method
// attribute used in proposals.  They optionally allow an XAUTH
// implementation to propose use of extended authentication after the
// initial phase 1 authentication.  Values are taken from the private
// use range defined in [IKE] and should be used among mutually
// consenting parties. 
//
#define XAUTHInitPreShared         65001
#define XAUTHRespPreShared         65002
#define XAUTHInitDSS               65003
#define XAUTHRespDSS               65004
#define XAUTHInitRSA               65005
#define XAUTHRespRSA               65006
#define XAUTHInitRSAEncryption     65007
#define XAUTHRespRSAEncryption     65008
#define XAUTHInitRSARevisedEncr    65009
#define XAUTHRespRSARevisedEncr    65010
#define XAUTHInitIndicator         (TUint16)0x1
#define XAUTHMethodBase            (TUint16)65000
#define XAUTHScaler                (TUint16)0x1  

//
// Attribute Payload (draft-dukes-ike-mode-cfg-01.txt)
// 1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ! Next Payload  !   RESERVED    !         Payload Length        !
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// !     Type      !   RESERVED    !           Identifier          !
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// !                                                               !
// ~                           Attributes                          ~
// !                                                               !
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
#define ISAKMP_PAYLOAD_ATTRIBUTES 14  // Attributes payload

//
// Configuration message types used within the Type field of an Attribute ISAKMP payload
// (draft-dukes-ike-mode-cfg-01.txt)
//
#define ISAKMP_CFG_REQUEST         1  // Configure request
#define ISAKMP_CFG_REPLY           2  // Configure reply
#define ISAKMP_CFG_SET             3  // Configure set
#define ISAKMP_CFG_ACK             4  // Configure ack

//
// Configuration Attribute values within an Attributes Payload
// (draft-dukes-ike-mode-cfg-01.txt)
//
#define ATTR_INTERNAL_IP4_ADDR     1  // Internal IPv4 address (=Virtual IP)
#define ATTR_INTERNAL_IP4_MASK     2  // Internal IPv4 mask
#define ATTR_INTERNAL_IP4_DNS      3  // Internal DNS address 
#define ATTR_INTERNAL_IP4_NBNS     4  // Internal NBNS address
#define ATTR_INTERNAL_ADDR_EXPIRY  5  // Internal Address expiry time
#define ATTR_INTERNAL_IP4_DHCP     6  // Internal DHCP address
#define ATTR_APPLICATION_VERSION   7  // Application version data
#define ATTR_INTERNAL_IP6_ADDR     8  // Internal IPv6 address (=Virtual IP)
#define ATTR_INTERNAL_IP6_MASK     9  // Internal IPv6 mask
#define ATTR_INTERNAL_IP6_DNS      10 // Internal DNS address 
#define ATTR_INTERNAL_IP6_NBNS     11 // Internal NBNS address
#define ATTR_INTERNAL_IP6_DHCP     12 // Internal DHCP address
#define ATTR_INTERNAL_IP4_SUBNET   13 // Internal IPv4 subnet (=policy selector)
#define ATTR_SUPPORTED_ATTRIBUTES  14 // Supported attributes info data 
#define ATTR_INTERNAL_IP6_SUBNET   15 // Internal IPv6 subnet (=policy selector)

//
// Extended Authentication Attribute values within an Attributes Payload
// (draft-beaulieu-ike-xauth-02.txt)
//
#define ATTR_XAUTH_TYPE            16520   // Extended authentication type code
#define ATTR_USER_NAME             16521   // User name data
#define ATTR_PASSWORD              16522   // Password data
#define ATTR_PASSCODE              16523   // Passcode data
#define ATTR_MESSAGE               16524   // Message data
#define ATTR_CHALLENGE             16525   // Challenge data
#define ATTR_DOMAIN                16526   // Domain name data
#define ATTR_STATUS                16527   // Status value
#define ATTR_NEXT_PIN              16528   // Next PIN value
#define ATTR_ANSWER                16529   // Answer data

//
// Additional Extended Authentication Attribute values within an
// Attributes Payload. 
// (draft-ietf-ipsec-isakmp-xauth-04.txt)
//
#define ATTR_PIX_XAUTH_TYPE        13   // Extended authentication type code
#define ATTR_PIX_USER_NAME         14   // User name data
#define ATTR_PIX_PASSWORD          15   // Password data
#define ATTR_PIX_PASSCODE          16   // Passcode data
#define ATTR_PIX_MESSAGE           17   // Message data
#define ATTR_PIX_CHALLENGE         18   // Challenge data
#define ATTR_PIX_DOMAIN            19   // Domain name data
#define ATTR_PIX_STATUS            20   // Status value

//
// Extended Authentication type values
// (draft-beaulieu-ike-xauth-02.txt)
//
#define ATTR_XAUTH_GENERIC         0       // Generic authentication type code
#define ATTR_XAUTH_RADIUS_CHAP     1       // RADIUS CHAP authentication type code
#define ATTR_XAUTH_OTP             2       // OTP authentication type code
#define ATTR_XAUTH_SKEY            3       // S/KEY authentication type code

//
// Extended Authentication Set status values
// (draft-beaulieu-ike-xauth-02.txt)
//
#define ATTR_STATUS_OK             1      
#define ATTR_STATUS_FAIL           0      

#endif // ISAKMPCT_H
