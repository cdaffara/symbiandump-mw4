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
* Description:  Class that logs ike messages in pcap format
*
*/

#ifndef IKEPCAPTRACE_H
#define IKEPCAPTRACE_H

#include <f32file.h>

class TInetAddr;

/**
 *  PCap tracer of IKE messages.
 *
 *  Class that logs ike messages in pcap format
 * 
 */
class CIkePcapTrace : public CBase
    {        
public:
    enum TEncryptionType
        {
        EEncrDes = 2, 
        EEncrDes3 = 3,
        EEncrAesCbc = 12
        };
    
    static CIkePcapTrace* NewL(const TDesC& aLogFolder);
    ~CIkePcapTrace();


    /**
     * Traces the IKE message. 
     *
     * @param aMessage Traced IKE message
     * @param aSourceAddress IP address of the message sender (only IPv4 is supported)
     * @param aDestinationAddress IP address of the message receiver  (only IPv4 is supported)
     * @param aEncryptionType Used encryption algorithm. 
     *                        If the aMessage datagram is IKEv1 or if the IKEv2 datagram
     *                        does not contain encryption payload this parameter is not used.
     */
    void TraceMessage(const TDesC8& aMessage, 
                      const TInetAddr& aSourceAddress, 
                      const TInetAddr& aDestinationAddress,
                      TEncryptionType aEncryptionType);
    
private:
    CIkePcapTrace();
    void ConstructL(const TDesC& aLogFolder);

    void DoTraceMessage(TPtr8& aMsgCopy, 
                        const TInetAddr& aSourceAddress, 
                        const TInetAddr& aDestinationAddress,
                        TEncryptionType aEncryptionType);
    
    
    TInt WritePcapHeader();
    
    TInt WriteRecordHeader(TUint32 aIkeMsgLength);
    TInt WriteIpAndUdpHeader(const TDesC8& aMessage,
                             TInetAddr aSourceAddress, 
                             TInetAddr aDestinationAddress);
    TInt WriteNonEspMarker();
    TInt WriteIkeMessage(TPtr8& aMsgCopy, TEncryptionType aEncryptionType);
    TInt WriteIkeV1Message(TPtr8& aMsgCopy);
    TInt WriteIkeV2Message(TPtr8& aMsgCopy, TEncryptionType aEncryptionType);

    
    RFs     iFileServer;
    RFile   iPcapFile;
    
    TBool iWriteTrace;
    };

#endif //IKEPCAPTRACE_H


