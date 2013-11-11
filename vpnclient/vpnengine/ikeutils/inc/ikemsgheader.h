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
* Description: IKE message header class
*
*/

#ifndef _IKE_MSG_HEADER_H_
#define _IKE_MSG_HEADER_H_

#include <e32base.h>

#define ISAKMP_COOKIE_SIZE 8
#define IKEV2_SPI_SIZE     8
typedef TBuf8<ISAKMP_COOKIE_SIZE> TCookie;
typedef TBuf8<IKEV2_SPI_SIZE> TIkeSPI;

#define ISAKMP_HDR_SIZE sizeof(ThdrISAKMP)
#define IKEV2_HDR_SIZE  sizeof(ThdrISAKMP)

#define IKE_PORT                  500
#define FLOATED_IKE_PORT          4500     
#define NON_ESP_MARKER            0x0     
#define NON_ESP_MARKER_SIZE       4   // Corresponds IPSEC SPI size

//
//  NAT flags
//
#define LOCAL_END_NAT       0x1
#define REMOTE_END_NAT      0x2
#define MOBIKE_USED         0x4

//
// These macro definitions takes care of 16- and 32-bit data packing and
// unpacking.
// The following assumptions are in use for macros:
// -- The connection memory is a little-endian configured (= Intel format)
// -- A macro does always an unconditional conversion for the parameter data
//    PUTxx(d, s) = Source data is supposed to be in "Network order". Data
//                  is stored to *p as little-endian.
//    GETxx(s)    = Source data is supposed to be in memory as little-endian.
//                  Macro return data in "Network order"
//
#define PUT16(d, s)  \
   (*(unsigned char*)((unsigned char*)(d)+1)) = (unsigned char)((s) & 0xff);\
   (*(unsigned char*)(d))                     = (unsigned char)(((s) >> 8 ) & 0xff)

#define PUT32(d, s)  \
    (*(unsigned char*)((unsigned char*)(d)+3)) = (unsigned char)((s) & 0xff);\
    (*(unsigned char*)((unsigned char*)(d)+2)) = (unsigned char)(((s) >> 8 ) & 0xff);\
    (*(unsigned char*)((unsigned char*)(d)+1)) = (unsigned char)(((s) >> 16) & 0xff);\
    (*(unsigned char*)(d))                     = (unsigned char)(((s) >> 24) & 0xff)

#define GET16(s)  \
     (((unsigned short)(*((unsigned char*)(s)+1)))     | \
      ((unsigned short)(*(unsigned char*)(s)) << 8 ))

#define GET32(s)  \
     (((unsigned int)(*((unsigned char*)(s)+3)))         | \
     (((unsigned int)(*((unsigned char*)(s)+2))) << 8  ) | \
     (((unsigned int)(*((unsigned char*)(s)+1))) << 16 ) | \
      ((unsigned int)(*(unsigned char*)(s)) << 24 ))


//
//   IKEv2 MESSAGE FIXED HEADER
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                       IKE_SA Initiator's SPI                  !
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                       IKE_SA Responder's SPI                  !
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !  Next Payload ! MjVer ! MnVer ! Exchange Type !     Flags     !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                          Message ID                           !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                            Length                             !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//   IKEv1 MESSAGE FIXED HEADER
//
//                     1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                          Initiator                            !
//   !                            Cookie                             !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                          Responder                            !
//   !                            Cookie                             !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !  Next Payload ! MjVer ! MnVer ! Exchange Type !     Flags     !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                          Message ID                           !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                            Length                             !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class ThdrISAKMP
{
public:
    static inline const ThdrISAKMP* Ptr(const TDes8& aMsg, TInt aOffset) { return (ThdrISAKMP*)(aMsg.Ptr() + aOffset); }
    static inline const ThdrISAKMP* Ptr(const TDes8& aMsg) { return (ThdrISAKMP*)aMsg.Ptr(); }
	static inline TInt Size() { return sizeof(ThdrISAKMP); }	
    inline const ThdrISAKMP* GotoOffset(TUint8 aOffset) const { return (const ThdrISAKMP*)((char*)this + aOffset); }
    inline TUint8* Next() const { return (TUint8*)((char*)this + sizeof(ThdrISAKMP)); }
	inline TCookie GetCookieI() const
		{ TCookie c;
          c.Copy(&u.iData8[0],ISAKMP_COOKIE_SIZE);
	      return c;
		}
	inline void GetSPI_I(TIkeSPI& aSPI) const { aSPI.Copy(&u.iData8[0], IKEV2_SPI_SIZE); }
	inline TUint32 GetSPI_I_Low32() const  { return GET32(&u.iData32[1]); }
	inline TUint32 GetNegotiationID_I() const  { return GET32(&u.iData32[0]); }
	inline TCookie GetCookieR() const
		{ TCookie c;
	      c.Copy(&u.iData8[8],ISAKMP_COOKIE_SIZE);
	      return c;
		}
	inline void GetSPI_R(TIkeSPI& aSPI) const { aSPI.Copy(&u.iData8[8], IKEV2_SPI_SIZE); }
	inline TUint32 GetSPI_R_Low32() const  { return GET32(&u.iData32[3]); }	
	inline TUint32 GetNegotiationID_R() const  { return GET32(&u.iData32[2]); }
	inline void SetCookieI(const TCookie& aCookie)
		{ Mem::Copy(&u.iData8[0], aCookie.Ptr(), ISAKMP_COOKIE_SIZE); }
	inline void SetSPI_I(const TIkeSPI& aSPI)	{Mem::Copy(&u.iData8[0],aSPI.Ptr(),IKEV2_SPI_SIZE); }
	inline void SetCookieR(const TCookie& aCookie)
		{ Mem::Copy(&u.iData8[8],aCookie.Ptr(),ISAKMP_COOKIE_SIZE); }
	inline void SetSPI_R(const TIkeSPI& aSPI)	{ Mem::Copy(&u.iData8[8],aSPI.Ptr(),IKEV2_SPI_SIZE); }
	inline TUint8 GetPayload() const { return u.iData8[16]; }
	inline void SetPayload(TUint8 aPayload) { u.iData8[16] = aPayload; }
	inline TUint8 GetVersion() const { return u.iData8[17]; }
	inline TUint8 GetMajorVersion() const { return (TUint8)(u.iData8[17] >> 4); }
	inline TUint8 GetMinorVersion() const { return (TUint8)(u.iData8[17] & 0xf); }		
	inline void SetVersion(TUint8 aVersion) { u.iData8[17] = aVersion; }
	inline TUint8 GetExchange() const { return u.iData8[18]; }
	inline void SetExchange(TUint8 aType) { u.iData8[18] = aType; }
	inline TUint8 GetFlags() const { return u.iData8[19]; }
	inline void SetFlags(TUint8 aFlags) { u.iData8[19] = aFlags; }
	inline TUint32 GetMessageId() const { return GET32(&u.iData32[5]); }
	inline void SetMessageId(TUint32 aId) { PUT32(&u.iData32[5], aId); }
	inline TUint32 GetLength() const { return GET32(&u.iData32[6]); }
	inline void SetLength(TUint32 aLength) { PUT32(&u.iData32[6], aLength); }
private:
    union
        {
        TUint32 iData32[7];
        TUint16 iData16[14];
        TUint8  iData8[28];
        } u;
};


#endif
