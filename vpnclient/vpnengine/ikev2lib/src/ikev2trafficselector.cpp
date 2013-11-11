/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include "ikev2trafficselector.h"
#include "ikev2const.h"
#include "ikev2payloads.h"

TIkeV2TrafficSelector* TIkeV2TrafficSelector::NewL(const TDesC8& aIkeV2TrafficSector)
    {    
    static const TUint16 KIpv4SelectorLength = 16;
    static const TUint16 KIpv6SelectorLength = 48;

    if (aIkeV2TrafficSector.Length() != KIpv4SelectorLength &&
        aIkeV2TrafficSector.Length() != KIpv6SelectorLength)
        {
        User::Leave(KErrArgument);
        }
        
    
    const TTrafficSelector* selector = 
            reinterpret_cast<const TTrafficSelector*>(aIkeV2TrafficSector.Ptr());
        

    TInetAddr startAddr;
    TInetAddr endAddr;
    
    const TUint8* addresses = selector->Addresses();
    if ( selector->Type() ==  TS_IPV4_ADDR_RANGE )
        {
        if (aIkeV2TrafficSector.Length() != KIpv4SelectorLength)
            {
            User::Leave(KErrArgument);
            }        
        TUint32 startA = *(reinterpret_cast<const TUint32*>(addresses));
        TUint32 endA = *(reinterpret_cast<const TUint32*>(addresses + 4));
        
        BigEndian::Put32(reinterpret_cast<TUint8*>(&startA), startA);
        BigEndian::Put32(reinterpret_cast<TUint8*>(&endA), endA);
        
        startAddr.SetAddress(startA);              
        endAddr.SetAddress(endA);
        
        }
    else
        {
        if (aIkeV2TrafficSector.Length() != KIpv6SelectorLength ||
            selector->Type() !=  TS_IPV6_ADDR_RANGE)
            {
            User::Leave(KErrArgument);
            }
        TIp6Addr startA;
        TIp6Addr endA;
        
        Mem::Copy(startA.u.iAddr8, addresses, 16);
        Mem::Copy(endA.u.iAddr8, addresses+16, 16);

        startAddr.SetAddress(startA);              
        endAddr.SetAddress(endA);        
        }

    TUint16 startPort = selector->StartPort();
    BigEndian::Put16(reinterpret_cast<TUint8*>(&startPort), startPort);
    startAddr.SetPort(startPort);
    
    TUint16 endPort = selector->EndPort();
    BigEndian::Put16(reinterpret_cast<TUint8*>(&endPort), endPort);
    endAddr.SetPort(endPort);
    
    return new (ELeave) TIkeV2TrafficSelector(startAddr, endAddr, selector->Protocol());       
    }


TIkeV2TrafficSelector::TIkeV2TrafficSelector(TInetAddr aStartingAddress, TInetAddr aEndingAddress, TUint8 aProtocolId)
:iStartingAddress(aStartingAddress), iEndingAddress(aEndingAddress), iProtocolId(aProtocolId)
    {
    __ASSERT_DEBUG(iStartingAddress.Family() == iEndingAddress.Family() &&
                   iStartingAddress.IsV4Compat() == iEndingAddress.IsV4Compat() &&
                   iStartingAddress.IsV4Mapped() == iEndingAddress.IsV4Mapped(), User::Invariant());
    
    iMask = CalcuateMask();
    }


TIkeV2TrafficSelector::TIkeV2TrafficSelector(const TIkeV2TrafficSelector& aTrafficSelector)
    {
    iStartingAddress = aTrafficSelector.iStartingAddress;
    iEndingAddress = aTrafficSelector.iEndingAddress;
    
    iProtocolId = aTrafficSelector.iProtocolId;
    
    __ASSERT_DEBUG(iStartingAddress.Family() == iEndingAddress.Family() &&
                   iStartingAddress.IsV4Compat() == iEndingAddress.IsV4Compat() &&
                   iStartingAddress.IsV4Mapped() == iEndingAddress.IsV4Mapped(), User::Invariant());
    iMask = CalcuateMask();    
    }


TInetAddr TIkeV2TrafficSelector::StartingAddress() const
    {
    return iStartingAddress;
    }   


TInetAddr TIkeV2TrafficSelector::EndingAddress() const
    {
    return iEndingAddress;
    }
    
TInetAddr TIkeV2TrafficSelector::Mask() const
    {
    return iMask;
    }

TUint8 TIkeV2TrafficSelector::ProtocolId() const
    {
    return iProtocolId;
    }

TUint8 TIkeV2TrafficSelector::Type() const
    {
    __ASSERT_DEBUG(iStartingAddress.Family() == iEndingAddress.Family() &&
                   iStartingAddress.IsV4Compat() == iEndingAddress.IsV4Compat() &&
                   iStartingAddress.IsV4Mapped() == iEndingAddress.IsV4Mapped(), User::Invariant());
    
    if (iStartingAddress.Family() == KAfInet ||
        iStartingAddress.IsV4Compat() ||
        iStartingAddress.IsV4Mapped())
        {
        return TS_IPV4_ADDR_RANGE;
        }
    else
        {
        return TS_IPV6_ADDR_RANGE;
        }
    }


HBufC8* TIkeV2TrafficSelector::IdFromTsL() const
    {
    TInetAddr idAddr = iStartingAddress;
    idAddr.SetPort(0);
    TInt prefix = 0;
    if (Type() == TS_IPV4_ADDR_RANGE)
        {
        TUint32 startIp = iStartingAddress.Address();
        TUint32 endIp   = iEndingAddress.Address();
        TUint32 mask = ~(endIp ^ startIp);
        TUint32 m = 1;           
        while ( prefix < 32 )
        {
            if ( mask & ( m << prefix) )
               break;
            prefix ++;
        }
        prefix = 32 - prefix;
        idAddr = TInetAddr(startIp);        
        }
    else
        {        
        prefix = 128;  // NOT IMPLEMENTED YET
        }

    HBufC* idString = HBufC::NewLC(512);
    TPtr idStringPtr = idString->Des();
    idAddr.Output(idStringPtr);
    idStringPtr.AppendFormat(_L("/%d"), prefix);
    
    HBufC8* idString8 = HBufC8::NewL(idString->Length());
    TPtr8 idString8Ptr = idString8->Des();
    idString8Ptr.Copy(*idString);
    CleanupStack::PopAndDestroy(idString);
    
    return idString8;
    }

TInetAddr TIkeV2TrafficSelector::CalcuateMask() const
    {
    TInetAddr mask;
    if ( Type() ==  TS_IPV4_ADDR_RANGE )
        {
        TUint32 A = iStartingAddress.Address();
        TUint32 M = iEndingAddress.Address();
        TUint32 X = 0x80000000;
        TUint32 Y = 0;
        M = ~(A ^ M); //Gets the bits that are 1 in both addresses
        for ( TInt i = 0; i < 32; ++i )     
        {
            if ( (M & X) == 0 )
            {
               M &= Y;
               break;
            }
            Y |= X;         
            X  = (X >> 1);
        }
        mask.SetAddress(M);     
        }
    else
        {
        TIp6Addr startAddr = iStartingAddress.Ip6Address();
        TIp6Addr endAddr = iEndingAddress.Ip6Address();
        TIp6Addr m; 
        Mem::FillZ(m.u.iAddr8, 16);
        
        TUint8   c = 0xff;
        for ( TInt i = 0; i < 16; ++i )
            {
            if ( c == 0xff )
                c = (TUint8)(~(startAddr.u.iAddr8[i] ^ endAddr.u.iAddr8[i])); // Convert range to mask
            if ( (c != 0) && (c != 0xff) )
                {
                TUint8 z = 0x80;
                TUint8 w = 0;               
                for ( TInt j = 0; j < 8; ++j )       
                    {
                    if ( (c & z) == 0 )
                        {
                        c &= w;
                        break;
                        }
                    w |= z;                 
                    z  = (TUint8)(z >> 1);
                    }            
                m.u.iAddr8[i]  = c;
                c = 0;   
                }
            else
                {                            
                m.u.iAddr8[i]  = c;
                }       
            }
        mask.SetAddress(m);
        }
    return mask;
    }

bool TIkeV2TrafficSelector::operator>(const TIkeV2TrafficSelector& aOtherSelector) const
    {
    if (Type() != aOtherSelector.Type())
         {
         //types do not match.
         return false;
         }        

    if (operator==(aOtherSelector))
        {
        //selectors are equal
        return false;
        }   

    return operator>=(aOtherSelector);
    }


bool TIkeV2TrafficSelector::operator<(const TIkeV2TrafficSelector& aOtherSelector) const
    {
    if (operator==(aOtherSelector))
        {
        //selectors are equal
        return false;
        }   
    
    //The operator
    return operator<=(aOtherSelector); 
    }

bool TIkeV2TrafficSelector::operator!=(const TIkeV2TrafficSelector& aOtherSelector) const
    {
    return !operator==(aOtherSelector);
    }

bool TIkeV2TrafficSelector::operator==(const TIkeV2TrafficSelector& aOtherSelector) const
    {
    return (iStartingAddress == aOtherSelector.iStartingAddress &&
            iEndingAddress == aOtherSelector.iEndingAddress);
    }

bool TIkeV2TrafficSelector::operator>=(const TIkeV2TrafficSelector& aOtherSelector) const
    {
    if (Type() != aOtherSelector.Type())
         {
         //types do not match.
         return false;
         }        
    
    if (operator==(aOtherSelector))
        {
        //selectors are equal
        return true;
        }

    return !operator<(aOtherSelector);
    }

bool TIkeV2TrafficSelector::operator<=(const TIkeV2TrafficSelector& aOtherSelector) const
    {
    if (Type() != aOtherSelector.Type())
         {
         //types do not match.
         return false;
         }        
    
    if (operator==(aOtherSelector))
        {
        //selectors are equal
        return true;
        }
    
    if (iStartingAddress.Port() < aOtherSelector.iStartingAddress.Port() ||
        iEndingAddress.Port() > aOtherSelector.iEndingAddress.Port())
        {
        //Port range of this is bigger
        return false;
        }

    if (Type() == TS_IPV4_ADDR_RANGE)
         {        
         if (iStartingAddress.Address() >= aOtherSelector.iStartingAddress.Address() &&
             iEndingAddress.Address() <= aOtherSelector.iEndingAddress.Address())
             {
             return true;
             }
         else
             {
             return false;
             }
         }
     else
         {
         __ASSERT_DEBUG(Type() == TS_IPV6_ADDR_RANGE, User::Invariant());
         
         const TIp6Addr& thisStart = iStartingAddress.Ip6Address();
         const TIp6Addr& thisEnd = iEndingAddress.Ip6Address();
                           
         const TIp6Addr& otherStart = aOtherSelector.iStartingAddress.Ip6Address();
         const TIp6Addr& otherEnd = aOtherSelector.iEndingAddress.Ip6Address();
         
         TInt i;
         for (i = 0; i < 4; i++)
             {
             if (thisStart.u.iAddr32[i] > otherStart.u.iAddr32[i])
                 {
                 //this start address is bigger 
                 break;
                 }
             else if (thisStart.u.iAddr32[i] < otherStart.u.iAddr32[i])
                 {
                 //this start address is smaller.
                 return false; 
                 }             
             }
         
         for (i = 0; i < 4; i++)
             {
             if (thisEnd.u.iAddr32[i] < otherEnd.u.iAddr32[i])
                 {
                 //this end address is smaller 
                 break;
                 }
             else if (thisEnd.u.iAddr32[i] > otherEnd.u.iAddr32[i])
                 {
                 //this end address is bigger
                 return false; 
                 }             
             }         
         return true;
         }            
    }


