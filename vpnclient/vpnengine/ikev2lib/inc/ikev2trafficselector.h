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
* Description:  Class that represents traffic selectors in IKEv2
*
*/


#ifndef IKEV2TRAFFICSELECTOR_H_
#define IKEV2TRAFFICSELECTOR_H_

#include <e32base.h>
#include <in_sock.h>

class TIkeV2TrafficSelector
    {
public:    
    
    /**
     * Parses the traffic selector from IKEv2 datagrams 
     * TrafficSelector payloads TrafficSelector
     * 
     * If the traffic selector can not be parsed, the method leaves with
     * KErrArgument.
     */
    static TIkeV2TrafficSelector* NewL(const TDesC8& aIkeV2TrafficSector);
    
    TIkeV2TrafficSelector(TInetAddr aStartingAddress, TInetAddr aEndingAddress, TUint8 aProtocolId);
    TIkeV2TrafficSelector(const TIkeV2TrafficSelector& aTrafficSelector);    
    
    TInetAddr StartingAddress() const;
    TInetAddr EndingAddress() const;
    TInetAddr Mask() const;
    
    TUint8 ProtocolId() const;
    
    /**
     *  Possible values are:
     *  TS_IPV4_ADDR_RANGE and TS_IPV6_ADDR_RANGE
     */
    TUint8 Type() const;        
    HBufC8* IdFromTsL() const;

    bool operator>(const TIkeV2TrafficSelector& aOtherSelector) const;
    bool operator<(const TIkeV2TrafficSelector& aOtherSelector) const;
    bool operator!=(const TIkeV2TrafficSelector& aOtherSelector) const;
    bool operator==(const TIkeV2TrafficSelector& aOtherSelector) const;
    bool operator>=(const TIkeV2TrafficSelector& aOtherSelector) const;
    bool operator<=(const TIkeV2TrafficSelector& aOtherSelector) const;
    
private:    
    
    TInetAddr CalcuateMask() const;
    
    TInetAddr iStartingAddress;  //start address and port
    TInetAddr iEndingAddress;    //end address and port
    
    TInetAddr iMask; //Calculated from iStartingAddress and iEndingAddress 
    TUint8 iProtocolId;
    };


#endif /* IKEV2TRAFFICSELECTOR_H_ */
