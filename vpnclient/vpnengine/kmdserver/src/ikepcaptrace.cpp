/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bautils.h>
#include <in_sock.h>

#include "ikepcaptrace.h"

_LIT(KLogDirectoryFormat, "c:\\logs\\%S\\");
_LIT(KTraceFileName, "ikemsg.pcap");

static const TUint KIpAndUdpHeaderLength = 28;
static const TUint KFixedHdrLength       = 28;
static const TUint KNonEspMarkerLength   = 4;
static const TInt KNatPort = 4500;

_LIT(KUnixTimeZeroDes, "19700101:000000.000000");
static const TTime KUnixTimeZero(KUnixTimeZeroDes);

#define SWAP_BYTE_ORDER32(a) ((a) >> 24) | ((a) >> 8 & 0xff00) |((a) << 8 & 0xff0000) | ((a) << 24);
#define SWAP_BYTE_ORDER16(a) ((a) >> 8 | (a) << 8)

CIkePcapTrace* CIkePcapTrace::NewL(const TDesC& aLogFolder)
    {
    CIkePcapTrace* self = new (ELeave) CIkePcapTrace();
    CleanupStack::PushL(self);
    self->ConstructL(aLogFolder);    
    CleanupStack::Pop(self);
    
    return self;
    }


CIkePcapTrace::CIkePcapTrace()
    {   
    }


void CIkePcapTrace::ConstructL(const TDesC& aLogFolder)
    {
    User::LeaveIfError(iFileServer.Connect());   
    
    TFileName* traceFileName = new (ELeave) TFileName;
    CleanupDeletePushL(traceFileName);
    traceFileName->Format(KLogDirectoryFormat, &aLogFolder);
    
    if (BaflUtils::FolderExists(iFileServer, *traceFileName))
        {
        traceFileName->Append(KTraceFileName);
        
        if (BaflUtils::FileExists(iFileServer, *traceFileName))
            {
            TInt position = 0;
            User::LeaveIfError(iPcapFile.Open(iFileServer, *traceFileName, EFileWrite));
            User::LeaveIfError(iPcapFile.Seek(ESeekEnd, position));
            }
        else
            {
            User::LeaveIfError(iPcapFile.Create(iFileServer, *traceFileName, EFileWrite));
            TInt err = WritePcapHeader();
            if (err != KErrNone)
                {
                iPcapFile.Close();
                User::LeaveIfError(iFileServer.Delete(*traceFileName));
                User::Leave(err);
                }                            
            }
        iWriteTrace = ETrue;
        }
    else
        {
        iWriteTrace = EFalse;
        }         
    CleanupStack::PopAndDestroy(traceFileName);
    }


CIkePcapTrace::~CIkePcapTrace()
    {
    iPcapFile.Close();
    iFileServer.Close();
    }

void CIkePcapTrace::TraceMessage(const TDesC8& aMessage, 
                                 const TInetAddr& aSourceAddress, 
                                 const TInetAddr& aDestinationAddress,
                                 TEncryptionType aEncryptionType)
    {
    if (iWriteTrace)
        {
        HBufC8* msgCopy = aMessage.Alloc();        
        if (msgCopy != NULL)
            {
            TPtr8 msgCopyPtr = msgCopy->Des();
            DoTraceMessage(msgCopyPtr, aSourceAddress, aDestinationAddress,
                           aEncryptionType);
            }
        delete msgCopy;
        msgCopy = NULL;
        }
    }



void CIkePcapTrace::DoTraceMessage(TPtr8& aMsgCopy, 
                                   const TInetAddr& aSourceAddress, 
                                   const TInetAddr& aDestinationAddress,
                                   TEncryptionType aEncryptionType)
    { 
       
    if ((aSourceAddress.Family() == KAfInet ||
         aSourceAddress.IsV4Compat() || 
         aSourceAddress.IsV4Mapped()) &&
        (aDestinationAddress.Family() == KAfInet ||  
         aDestinationAddress.IsV4Compat() || 
         aDestinationAddress.IsV4Mapped()))
        {
            TInt length = aMsgCopy.Length();
            if (aSourceAddress.Port() == KNatPort)
                {
                length+=KNonEspMarkerLength;
                }
            WriteRecordHeader(length);
            WriteIpAndUdpHeader(aMsgCopy, aSourceAddress, aDestinationAddress);
            if ( aSourceAddress.Port() == KNatPort )
                {
                WriteNonEspMarker();
                }
            WriteIkeMessage(aMsgCopy, aEncryptionType);
            iPcapFile.Flush();
        }
    }


TInt CIkePcapTrace::WriteRecordHeader(TUint32 aIkeMsgLength)
    {
    static const TUint KRecordHeaderLength = 4;
    TUint32 recordHeader[KRecordHeaderLength];
        
    TUint32 currentSeconds = 0;
    TTime currentTime;
    currentTime.HomeTime();
    
    TTimeIntervalSeconds secondsFrom;    
    if (currentTime.SecondsFrom(KUnixTimeZero, secondsFrom) == KErrNone)
        {
        currentSeconds = secondsFrom.Int();
        }
    
    TUint32 microseconds = currentTime.DateTime().MicroSecond();
    
    recordHeader[0] = currentSeconds;
    recordHeader[1] = microseconds;             
    recordHeader[2] = aIkeMsgLength + KIpAndUdpHeaderLength;
    recordHeader[3] = aIkeMsgLength + KIpAndUdpHeaderLength;
    
    TPtrC8 recordHdrPtr((TUint8*)recordHeader, KRecordHeaderLength * sizeof(TUint32));
    return iPcapFile.Write(recordHdrPtr);
    }

TInt CIkePcapTrace::WriteIpAndUdpHeader(const TDesC8& aMessage,
                                        TInetAddr aSourceAddress, 
                                        TInetAddr aDestinationAddress)
    {
    static const TUint KIpHeaderLength = 20;
    static const TUint KUdpHeaderLength = KIpAndUdpHeaderLength - KIpHeaderLength; 
        
    //Generate IP header
    aSourceAddress.ConvertToV4();
    aDestinationAddress.ConvertToV4();
    
    TUint32 source      = SWAP_BYTE_ORDER32(aSourceAddress.Address());    
    TUint32 destination = SWAP_BYTE_ORDER32(aDestinationAddress.Address()); 
    
    TUint8 ipAndUdpHeader[] = { 0x45, 0x00, 0x13, 0x88, 
                                0x00, 0x28, 0x00, 0x00, 
                                0xfe, 0x11, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, //source IP address
                                0x00, 0x00, 0x00, 0x00, //destination IP address
                                0x00, 0x00, 0x00, 0x00, //udp header
                                0x00, 0x00, 0x00, 0x00}; 
                          
    Mem::Copy(ipAndUdpHeader + 12, &source, sizeof(source)); //copies the source address to header
    Mem::Copy(ipAndUdpHeader + 16, &destination, sizeof(destination)); //copies the source address to header
   
    
    //Generate UDP header
    TUint16 sourcePort          = aSourceAddress.Port();
    TUint16 destinationPort     = aDestinationAddress.Port();
    TUint16 udpDatagramLength   = KUdpHeaderLength + aMessage.Length();
    if ( sourcePort == KNatPort )
        {
        udpDatagramLength += KNonEspMarkerLength; 
        }
    sourcePort          = SWAP_BYTE_ORDER16(sourcePort);
    destinationPort     = SWAP_BYTE_ORDER16(destinationPort);
    udpDatagramLength   = SWAP_BYTE_ORDER16(udpDatagramLength);
    
    TUint8* udpHeader = ipAndUdpHeader + KIpHeaderLength;
    Mem::Copy(udpHeader, &sourcePort, sizeof(sourcePort));
    Mem::Copy(udpHeader + 2, &destinationPort, sizeof(destinationPort));
    Mem::Copy(udpHeader + 4, &udpDatagramLength, sizeof(udpDatagramLength));
    
    TPtrC8 headerPtr(ipAndUdpHeader, KIpAndUdpHeaderLength);
    return iPcapFile.Write(headerPtr);
    }

TInt CIkePcapTrace::WriteNonEspMarker()
    {
    TUint8 nonEspMarker[] = { 0x00, 0x00, 0x00, 0x00 };

    TPtrC8 ptr(nonEspMarker, KNonEspMarkerLength);
    return iPcapFile.Write(ptr);
    }

TInt CIkePcapTrace::WriteIkeMessage(TPtr8& aMsgCopy, TEncryptionType aEncryptionType)
    {
    const TInt KVersionPosition = 17;
    
    TInt err = KErrNone;     
    if (aMsgCopy.Length() < KFixedHdrLength)
        {
        //This is a bit too short for an IKE packet.
        //Just write the packet to log anyway. It might give some info to someone.
        err = iPcapFile.Write(aMsgCopy);
        }
    else
        {
        //Version check:
        if (aMsgCopy[KVersionPosition] == 0x10)
            {   
            err = WriteIkeV1Message(aMsgCopy);
            }
        else
            {
            err = WriteIkeV2Message(aMsgCopy, aEncryptionType);
            }
        }
    
    return err;
    }


TInt CIkePcapTrace::WriteIkeV1Message(TPtr8& aMsgCopy)
    {
    //This should already be checked by the caller.
    __ASSERT_DEBUG(aMsgCopy.Length() >= KFixedHdrLength, User::Invariant());
        
    const TUint KEncryptionBitPosition    = 19;
    const TUint KFirstNextPayloadPosition = 16;    
    const TUint KFixedPayloadHdrLength    = 4;
    
    const TUint8 KPayloadNone               = 0x00;
    const TUint8 KPayloadHash               = 0x08;
    const TUint8 KPayloadSignature          = 0x09;
    const TUint8 KPayloadReservedRangeStart = 0x0E;
    
    aMsgCopy[KEncryptionBitPosition] = aMsgCopy[KEncryptionBitPosition] & 0xFE;

    TUint8 nextPayloadId = aMsgCopy[KFirstNextPayloadPosition];
    TPtr8 msgEnd(aMsgCopy.MidTPtr(KFixedHdrLength));
    
    while(nextPayloadId != KPayloadNone && 
          msgEnd.Length() > KFixedPayloadHdrLength)
        {
        //Read the lenght of the payload
        TUint16 payloadLength = ((TUint16)msgEnd[2]) << 8 | msgEnd[3]; 
      
        if (nextPayloadId == KPayloadHash ||
            nextPayloadId == KPayloadSignature ||
            nextPayloadId >= KPayloadReservedRangeStart)
            {
            if(msgEnd.Length() >= payloadLength &&
               payloadLength >= KFixedPayloadHdrLength )
                {
                // Zero out payload data.
                TUint16 dataLength = payloadLength - KFixedPayloadHdrLength;
                TPtr8 payloadData = msgEnd.MidTPtr(KFixedPayloadHdrLength, dataLength);
                payloadData.FillZ();
                }
            else
                {
                  //There seems to be something wrong with the packet.
                  //Zero out the rest of the packet and write it to the log.
                  msgEnd.FillZ();
                  msgEnd[0] = KPayloadNone;
                }                
            }
        
        nextPayloadId = msgEnd[0];
        if (nextPayloadId != KPayloadNone && 
            msgEnd.Length() >= payloadLength)
            {
            msgEnd.Set(msgEnd.MidTPtr(payloadLength));
            }
        else
            {
            msgEnd.Set(msgEnd.MidTPtr(msgEnd.Length()));
            }        
        }
    
    return iPcapFile.Write(aMsgCopy);
    }


TInt CIkePcapTrace::WriteIkeV2Message(TPtr8& aMsgCopy, TEncryptionType aEncryptionType)
    {
    //This should already be checked by the caller.
    __ASSERT_DEBUG(aMsgCopy.Length() >= KFixedHdrLength, User::Invariant());
    
    const TUint KFirstNextPayloadPosition        = 16;
    const TUint KFixedPayloadHdrLength           = 4;
    
    const TUint8 KPayloadNone      = 0x00;
    const TUint8 KEncryptedPayload = 0x2e;
    const TUint8 KAuthPayload      = 0x27;
    const TUint8 KEapPayload       = 0x30;    
    
    TUint8 nextPayloadId = aMsgCopy[KFirstNextPayloadPosition];
    TPtr8 msgEnd(aMsgCopy.MidTPtr(KFixedHdrLength));
    while(nextPayloadId != KPayloadNone && 
          msgEnd.Length() > KFixedPayloadHdrLength)
        {
        //Read the lenght of the payload
        TUint16 payloadLength = ((TUint16)msgEnd[2]) << 8 | msgEnd[3]; 
        
        switch(nextPayloadId)
            {
            case KEncryptedPayload:
                //change the encrypted payload length to match
                //the initialization vector length.
                msgEnd[2] = 0x00;
                switch(aEncryptionType)
                    {   
                    case EEncrDes: //falls through                    
                    case EEncrDes3:                        
                        msgEnd[3] = 0x0c;
                        break;
                    case EEncrAesCbc:                       
                        msgEnd[3] = 0x14;
                        break;
                    }
                payloadLength = msgEnd[3]; 
                break;
            case KAuthPayload:
                {
                const TUint8 KAuthPayloadHdrLength = 8;
                if (payloadLength > KAuthPayloadHdrLength && 
                    msgEnd.Length() >= payloadLength)
                    {
                    // Zero out payload data.
                    TPtr8 authData = msgEnd.MidTPtr(KAuthPayloadHdrLength, 
                                                    payloadLength - KAuthPayloadHdrLength);
                    authData.FillZ();
                    }
                }
                break;
            case KEapPayload:
                {
                const TUint8 KTotalFixEapHdrLength = 9;
                if (payloadLength > KTotalFixEapHdrLength && 
                    msgEnd.Length() >= payloadLength)
                    {
                    // Zero out payload data.
                    TPtr8 eapTypeData = msgEnd.MidTPtr(KTotalFixEapHdrLength,
                                                       payloadLength - KTotalFixEapHdrLength);
                    eapTypeData.FillZ();
                    }
                }
                break;
            }
        nextPayloadId = msgEnd[0];
        if (nextPayloadId != KPayloadNone && 
            msgEnd.Length() >= payloadLength)
            {
            msgEnd.Set(msgEnd.MidTPtr(payloadLength));
            }
        else
            {
            msgEnd.Set(msgEnd.MidTPtr(msgEnd.Length()));
            }
        }
    
    return iPcapFile.Write(aMsgCopy);
    }


TInt CIkePcapTrace::WritePcapHeader()
    {
    static const TUint KPcapHeaderLength = 24;
    TUint8 pCapHeader[] = { 0xd4, 0xc3, 0xb2, 0xa1,  // magic number
                            0x02, 0x00, 0x04, 0x00,  //major, minor version
                            0x00, 0x00, 0x00, 0x00,  //time offset
                            0x00, 0x00, 0x00, 0x00,  // accuracy of timestamps
                            0xff, 0xff, 0x00, 0x00,  // max length of captured packets, in octets 
                            0x0c, 0x00, 0x00, 0x00,  // data link type 
                           };
    
    TPtrC8 pCapHeaderPtr(pCapHeader, KPcapHeaderLength);    
    return iPcapFile.Write(pCapHeaderPtr);       
    }


