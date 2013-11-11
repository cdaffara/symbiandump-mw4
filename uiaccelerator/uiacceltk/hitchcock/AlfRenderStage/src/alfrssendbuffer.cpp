/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Handles serializing of data and sending it to alf streamer server.
*
*/



// Maybe...
// - EAlfCancelClippingRect is many times followed by EAlfSetClippingRect. Could the EAlfCancelClippingRect be ignored?
// - EAlfSetBrushStyle (etc. other non drawing commands) are followed by AlfReset and no drawing is done. Could the Set
//   commands be ignored.
// Can ignoring be done faster than executing useless commands?

#include <barsread.h>
#include <barsc.h>
#include <e32std.h>
#include <alfstreamerconsts.h>
#include <huiwscanvascommands.h>
#include <uiacceltk/HuiUtil.h>

#include <alfdecoderserverclient.h>
#include <alflogger.h>
#include "alf/alfcompositionclient.h"
#include "alfrssendbuffer.h"
#include "alfrenderstage.h"
#include "alfcommanddebug.h"
#include <alfnonsupportedwscommands.rsg>

#include <data_caging_path_literals.hrh>

_LIT( KRendererSupportedCommands,"alfnonsupportedwscommands.rsc" );

const TInt KFrameHeaderSizeWhenCacheChunks = 500;
const TInt KFrameHeaderSize = 5000;
const TInt KFrameOffsetTemplate = 12345678;
const TInt KCacheChunkMinSize = 50000;
const TUint8 KDivisibleByX = 8;
const TInt KWindowFrameHeader1 = 6; // bytes

#ifdef RD_SUBWINDOW_EFFECTS 
const TInt KWindowFrameHeader2 = 23 + 10; // bytes // TP +10 
#else
const TInt KWindowFrameHeader2 = 23;
#endif
const TInt KAllRenderersMask = 63;
const TInt KPossiblePerformanceProblemInWindow = 64;
const TInt KPossiblePerformanceProblemInWindowThreshold = 1000;

const TInt KFlushBufferTimeout = 50*1000; // 50ms

enum TPatternSearchStates
    {
    ESeekSetClippingRegion = 0,
    ESeekBitBlit = 1,
    ESeekResetClipping = 2,
    ESeekBrushStyle1 = 3,
    ESeekBrushStyle2 = 4
    };


#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_
const TInt TSearchPatternBitBlit[2][3] = { 
                // Search command           // state                // alternative command in this state
                { EAlfSetClippingRegion,    ESeekSetClippingRegion, KErrNotFound },
                { EAlfBitBltMasked,         ESeekBitBlit,           EAlfBitBltRect }};
#else
const TInt TSearchPatternBitBlit[5][3] = { 
				// Search command			// state				// alternative command in this state
        		{ EAlfSetClippingRegion, 	ESeekSetClippingRegion, KErrNotFound },
        		{ EAlfBitBltMasked, 		ESeekBitBlit, 			EAlfBitBltRect },
        		{ EAlfResetClippingRegion, 	ESeekResetClipping, 	KErrNotFound },
        		{ EAlfSetBrushStyle, 		ESeekBrushStyle1, 		KErrNotFound },
        		{ EAlfSetBrushStyle, 		ESeekBrushStyle2, 		EAlfSetPenStyle} };
#endif
                

// ---------------------------------------------------------------------------
// class CAlfObserver
// ---------------------------------------------------------------------------
//

NONSHARABLE_CLASS(CAlfObserver):public CActive
    {
    public:

        // ---------------------------------------------------------------------------
        // SetL
        // ---------------------------------------------------------------------------
        //
        static void SetL(CAlfRsSendBuffer* aHost, TBool aStartServer )
             {
             CAlfObserver* me = new (ELeave)  CAlfObserver(aHost, aStartServer);
             me->SetActive();
             // breahe one cycle to allow wserv kick its server object alive
             TRequestStatus* status = &me->iStatus; 
             User::RequestComplete(status, KErrNone);   
             }
  
        // ---------------------------------------------------------------------------
        // NewL
        // ---------------------------------------------------------------------------
        //
        CAlfObserver(CAlfRsSendBuffer* aHost, TBool aStartServer):CActive(2*EPriorityHigh),
            iHost(aHost), iStartServer(aStartServer),  iBreathe(ETrue)
            {
            CActiveScheduler::Add(this);
            iTimer.CreateLocal();
            }
        
        // ---------------------------------------------------------------------------
        // RunL
        // ---------------------------------------------------------------------------
        //
        void RunL()
            {
            if (iBreathe && iStartServer)
                {
                iBreathe = EFalse;
                SetActive();
                TRAPD(err, AlfServerStarter::StartL(iStatus, EFalse));
                if (err)
                    {
                    // Important, and thus enabled even in release builds. Other option would be letting leave to fall through 
                    // render stage stack and let window server panic the boot
                    RDebug::Print(_L("********* Error occurred when starting alf: %d"),err);
                    }
                else
                    {
                    return;
                    }
                }
            else
                {
                if( !iHost->ConnectL() ) // if connecting fails, try again a bit later
                    {
                    SetActive();
                    if( iStartServer )
                        {
                        iTimer.After( iStatus, 250000); // primary display is more important
                        }
                    else
                        {
                        iTimer.After( iStatus, 1000000);
                        }
                    }
                else
                    {
                    __ALFLOGSTRING("********************FLUSHING WSERV");    
					TRAP_IGNORE(iHost->PrepareBufferL());
					TRAP_IGNORE(iHost->SendL( NULL ));
                    iTimer.Close();
                    delete this;
                    }
                }
            }

        // ---------------------------------------------------------------------------
        // DoCancel
        // ---------------------------------------------------------------------------
        //
        void DoCancel()
            {
            // not applicable atm
            }
        
    private: // Data
        
        CAlfRsSendBuffer* iHost;
        TBool iStartServer;
        TBool iBreathe;
        RTimer iTimer;
    };
        
// ---------------------------------------------------------------------------
// doFlushBuffer
// Local function
// ---------------------------------------------------------------------------
//
TInt doFlushBuffer( TAny* aPtr )
    {
    ((CAlfRsSendBuffer*)aPtr)->FlushBuffer();
    return 0; // must return something
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfRsSendBuffer* CAlfRsSendBuffer::NewL(CAlfRenderStage& aParent, TInt aScreenNumber )
	{
	CAlfRsSendBuffer* self = new(ELeave) CAlfRsSendBuffer( aParent, aScreenNumber );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfRsSendBuffer::~CAlfRsSendBuffer()
    {
    if ( iAlfBridgerClient )
        {
        iAlfBridgerClient->Close();
        delete iAlfBridgerClient;
        iAlfBridgerClient = NULL;
        delete iAlfCompositionCntrlClient;
        }
#ifdef _OLD_STREAM    
    if ( iBufStream )
        {
        iBufStream->Close();
        delete iBufStream;
        iBufStream = NULL;
        }
#endif    
    iChunk.Close();
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::ConstructL()
    {
    // If this is not the primary screen (0) do not send any commands to Alf server
    iDisabled = (iScreenNumber != 0);
    
    ReadNonSupportedCommandsL();
    iChunkInUse = 0;
    iBooting = ETrue;
#ifdef _ALF_PRINT_WS_COMMANDS_
    iLog = 0;
    iCommandDebugger = CAlfCommandDebug::NewL();
#endif
    
    TBool runInsideWserv = ETrue; // EFalse;
    if( runInsideWserv )
        {
        TBool startServer = ETrue;
        if (iScreenNumber != 0) // we need one instance only
            {
            startServer = EFalse;
            }
        CAlfObserver::SetL(this, startServer);
        }
    else
        {
        CAlfObserver::SetL(this, EFalse);
        }
    
    MAlfCompositionController* compcntrl = (MAlfCompositionController*)iParent.ResolveObjectInterface(KAlfCompositionControllerIfUid);
    if (compcntrl)
      {
      compcntrl->AlfBridgeCallback(MAlfBridge::EAlfBridgeCreated, (MAlfBridge*)this);  
      }
    }

// ---------------------------------------------------------------------------
// ReadNonSupportedCommandsL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::ReadNonSupportedCommandsL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RResourceFile resFile;    
    TInt result;
    // resolve the absolute path
    // Read unsupported command resource
    TFileName resourceFileName;
    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
    resourceFileName.Copy( driveLetter );  
    resourceFileName.Append( KDC_ECOM_RESOURCE_DIR );
    resourceFileName.Append( KRendererSupportedCommands );

    TRAP( result, resFile.OpenL(fs,resourceFileName); );
    if ( result == KErrNone )
        {
        CleanupClosePushL(resFile);
        resFile.ConfirmSignatureL(0); // offset value.

        TInt resId = R_ALF_NON_SUPPORTED_COMMANDS_ARRAY;

        HBufC8* readData = resFile.AllocReadL(resId);
        // now first get rid of RResourceFile and RFs as they are not needed any more
        CleanupStack::PopAndDestroy(); // resFile
        CleanupStack::PopAndDestroy(); // fs
        CleanupStack::PushL(readData);

        // now parse it and store the values in cmdTextArray
        TResourceReader reader;
        reader.SetBuffer(readData);
        const TInt count = reader.ReadInt16();
        TInt8 command;

        TInt8 value;
        TInt nonSupportedParameterCount = 0;
        // resource file contains only non supported commands by renderers. Patch them to the iSupportedCommands
        TInt supportedParameterIndex = 0;
        for ( TInt i = 0; i < count ; i++ )
            {
            command = reader.ReadInt8();
            value = ConvertToBitsL( 7, &reader );

            nonSupportedParameterCount = reader.ReadInt16();
            if ( nonSupportedParameterCount > 0 )
                {
                iSupportedCommandParameters[command][0] = supportedParameterIndex;
                for ( TInt l = 0 ; l < nonSupportedParameterCount ; l ++ )
                    {
                    TInt8 parameterValue = reader.ReadInt8();
                    iSupportedParameters[supportedParameterIndex][0] = parameterValue;
                    TInt8 value2 = ConvertToBitsL( 6, &reader  );
                    iSupportedParameters[supportedParameterIndex][1] = value2;
                    supportedParameterIndex++;
                    }
                iSupportedCommandParameters[command][1] = supportedParameterIndex;
                }
            iNonSupportedCommands[command] = value;
            }
        CleanupStack::PopAndDestroy(readData);
        }
    else
        {
        __ALFLOGSTRING1("CAlfRsSendBuffer::ConstructL - WARNING! Failed to read unsupported commands from resource file: %S! Error code:st %d.", &resourceFileName );
        CleanupStack::PopAndDestroy(); // fs
        }
    }

// ---------------------------------------------------------------------------
// ConvertToBitsL
// ---------------------------------------------------------------------------
//
TUint8 CAlfRsSendBuffer::ConvertToBitsL( TInt aCount, TResourceReader* aReader )
    {
#ifdef _DEBUG    
    if ( aCount > 7 )
        {
        __ALFLOGSTRING("CAlfRsSendBuffer::ConvertToBitsL - Can convert only 7 sequantial bytes to one byte");
        USER_INVARIANT(); //
        }
#endif    
    TInt8 value = 0;
    TInt8 notSupported[7];
    aReader->Read( notSupported, aCount );
    TUint8 mask = 1;
    for (TInt j = 0; j < aCount ; j ++)
        {
        if ( notSupported[j] ){ value |= mask ; }
        mask = mask << 1;
        }
    return value;
    }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::RunL()
    {
    switch( iState )
        {
        case EWaitingAck:
            {
            if ( iStatus == EAlfBridgerAsyncronousData || iStatus < 0 )
                {
                TInt chunkIndex = 0;
                // The following is written to the last created chunk.
                TInt cacheChunkCount = iCacheChunks.Count();
                
                if ( cacheChunkCount > 0 )
                    {
                    // request destruction of all temporary chunks
                    while ( chunkIndex < cacheChunkCount )
                        {
                        WriteInt8L( EAlfDestroyChunk );
                        WriteInt8L( 0 ); // screen number, not used.
                        WriteInt32L( iCacheChunks[chunkIndex].Handle() );
                        WriteInt8L( EAlfCommandEndMarker );
                        
                        chunkIndex++;
                        }
                    if ( !iBooting )
                        {
                        // This is not necessary in the first boot, because jump 
                        // request has been already created in PrepareBufferL
                        
                        JumpToAnotherChunkL( 0, iPrimaryChunkMaxSize );
                        OpenPrimaryChunkForWritingL();
                        SeekL(0); // rewind stream to the beginning
                        WriteInt8L( EAlfCommandEndMarker );
                        CommitL();
                        FlushBuffer();
                        }
                    // destroy renderstage side handles to the chunk. alfhierarchy
                    // has still open handles for a while.
                    while( iCacheChunks.Count() )    
                        {
                        iCacheChunks[ 0 ].Close();
                        iCacheChunks.Remove( 0 );
                        }
                    }
                iUsedChunkMaxSize = iPrimaryChunkMaxSize - sizeof( TChunkHeader);
                iChunkInUse = 0;
                iBooting = EFalse; // alf has acknowledged the first frame
                iParent.EndCallBack( iQuedStatus );
                iState = EIdle;
                }
            break;
            }
        case EWaitingHandle:
            {
            break;
            }
        case EIdle:
            {
            break;
            }
        default:;
        }
    }

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::DoCancel()
    {
    // TODO
    }

// ---------------------------------------------------------------------------
// WriteCommandL
// writes 1 TInt value to the stream
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::WriteCommandL( const TUint8& aCommand, TInt aSize )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    if ( 
#ifdef _OLD_STREAM
            iBufStream->Sink() && 
#endif
            aCommand == EAlfPacketReady )
        {
        iReceivingDrawingCommands = EFalse;
        // if offscreen content has not been flushed, we'll keep the frame in not completed state.
        // it will be delivered to CHuiCanvasVisual, but not processed until finished packet arrives.
        // make sure, that frame flags (iFlags) follow this command.
        }

    if (! InitCommandL( aCommand, aSize)){ return;}
#ifdef _ALF_PRINT_WS_COMMANDS_  
    if ( !iReceivingDrawingCommands )
        {
        aSize += sizeof(TUint8);
        }
    iCommandDebugger->SetDescriptionAndSize( aCommand, aSize, R_ALF_COMMAND_DESCRIPTION_ARRAY  );
    iCommandDebugger->Print();
#endif   
    if ( !iReceivingDrawingCommands )
        {
        WriteInt8L( EAlfCommandEndMarker ); 
        }
    if ( aCommand == EAlfPacketReady )
        {
        iReceivingDrawingCommands = EFalse;
        iWrappingFrame = EFalse;
        
#ifdef _ALF_PRINT_WS_COMMANDS_    
        iCommandDebugger->PrintStatistic();
        iCommandDebugger->EndFrame();
#endif
       
        }
    }

// ---------------------------------------------------------------------------
// WriteIntsL
// writes aCount amount of variables to the stream and updates stream index.
// ---------------------------------------------------------------------------
//
// @todo Reference to Tint parameter was removed, because it did not pass armv5 compiler

void CAlfRsSendBuffer::WriteIntsL( const TUint8& aCommand, TInt aCount, TRefByValue<const TInt> aFirst,... )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    const TInt size = sizeof(TInt32) * aCount;
    if (! InitCommandL( aCommand, size )){ return;}
#ifdef _ALF_PRINT_WS_COMMANDS_    
    iCommandDebugger->SetDescriptionAndSize( aCommand, size, R_ALF_COMMAND_DESCRIPTION_ARRAY  );
    iCommandDebugger->Print();
#endif
    
	if ( aCount > 0 )
	    {
	    aCount--; // first item is serialized separately. It seems to exist at different location as rest of the parameters.
	    WriteInt32L( aFirst );
	    WriteL( (TUint8*)&aFirst + sizeof(TInt32), aCount * sizeof(TInt32) );
	    //TInt size = sizeof(TInt32) * aCount;
	    //iBufStream->WriteL( (TUint8*)&aFirst + sizeof(TInt32), size ); iOffset += size ; 
	    }
	if ( !iReceivingDrawingCommands )
	    {
	    WriteInt8L( EAlfCommandEndMarker );
	    }
    }

void CAlfRsSendBuffer::WriteIntsL(TUint8 aCommand, TInt aCount, TInt* aArray)
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    const TInt size = sizeof(TInt32) * aCount;
    if (!InitCommandL( aCommand, size ))
      { 
      return;
      }

#ifdef _ALF_PRINT_WS_COMMANDS_    
    iCommandDebugger->SetDescriptionAndSize( aCommand, size, R_ALF_COMMAND_DESCRIPTION_ARRAY  );
    iCommandDebugger->Print();
#endif
    
    for(TInt i = 0; i < aCount; i++)
        {	    
        WriteInt32L( aArray[i] );
        }
        
	if ( !iReceivingDrawingCommands )
	    {
	    WriteInt8L( EAlfCommandEndMarker );
	    }
    }

// ---------------------------------------------------------------------------
// WriteIntL
// All possibly non-supported commands that have a parameter are using this version
// of WriteIntL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::WriteIntL( const TUint8& aCommand, TInt aValue )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    TBool supported = EFalse;
    if (iNonSupportedCommands[aCommand])
        {        
        for( TInt i = iSupportedCommandParameters[aCommand][0]; i < iSupportedCommandParameters[aCommand][1]; i++ )
            {
            if ( !supported && aValue == iSupportedParameters[i][0])
                {
                supported = ETrue;
                iNonSupportedCommands[aCommand] = iSupportedParameters[i][1];
                }
            }
        }
    else
        {
        supported = ETrue;
        }
    
     if ( !supported )
         {
         iNonSupportedCommands[aCommand] = KAllRenderersMask;
         }
    if (! InitCommandL( aCommand, sizeof(TInt32) )){ return;}
#ifdef _ALF_PRINT_WS_COMMANDS_    
    iCommandDebugger->SetDescriptionAndSize( aCommand, sizeof(TInt32), R_ALF_COMMAND_DESCRIPTION_ARRAY );
    iCommandDebugger->Print();
#endif    
    // only commands, that may not be supported use this function
    if ( aCommand == EAlfSetPenStyle )
        {
        iPenStyle = aValue; // for patter search
        }
    WriteInt32L( aValue);
    if ( !iReceivingDrawingCommands )
        {
        WriteInt8L( EAlfCommandEndMarker );
        }
    }
 
// ---------------------------------------------------------------------------
// WriteRegionL
// writes aCount amount of variables to the stream and updates stream index.
// ---------------------------------------------------------------------------
//
// @todo Reference to Tint parameter was removed, because it did not pass armv5 compiler

void CAlfRsSendBuffer::WriteRegionL( const TUint8& aCommand, const TRegion& aRegion )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    TInt count = aRegion.Count();
    const TInt size = sizeof(TInt32) * ( 4 * count + 1);

    if (! InitCommandL( aCommand, size )){ return;}
#ifdef _ALF_PRINT_WS_COMMANDS_    
    iCommandDebugger->SetDescriptionAndSize( aCommand, size, R_ALF_COMMAND_DESCRIPTION_ARRAY );
    iCommandDebugger->SetRegion( aRegion );
    iCommandDebugger->Print();
#endif
    WriteInt32L( count );
    TInt i = 0;
    while( i < count )
        {
        WriteL( (TUint8*)&aRegion[i].iTl.iX, 4 * sizeof(TInt32 ));
        if ( aCommand == EAlfSetClippingRegion )
            {
            iSearchPatternClipRegion.AddRect(aRegion[i]);
            }
        i++;
        }
    
    if ( !iReceivingDrawingCommands )
        {
        WriteInt8L( EAlfCommandEndMarker );
        }
  
    }

// ---------------------------------------------------------------------------
// WriteRegionIntsL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::WriteRegionIntsL( const TUint8& aCommand, const TRegion& aRegion, 
        TInt aCount, TRefByValue<const TInt> aFirst, ...)
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    TInt count = aRegion.Count();
    const TInt size = sizeof(TInt32) * ( 4 * count + 1 + aCount);

    if (! InitCommandL( aCommand, size )){ return;}
#ifdef _ALF_PRINT_WS_COMMANDS_    
    iCommandDebugger->SetDescriptionAndSize( aCommand, size, R_ALF_COMMAND_DESCRIPTION_ARRAY );
    iCommandDebugger->SetRegion( aRegion );
    iCommandDebugger->Print();
#endif

	if ( aCount > 0 )
	    {
	    aCount--; // first item is serialized separately. It seems to exist at different location as rest of the parameters.
	    WriteInt32L( aFirst );
	    WriteL( (TUint8*)&aFirst + sizeof(TInt32), aCount * sizeof(TInt32) );
	    }

    WriteInt32L( count );
    TInt i = 0;
    while( i < count )
        {
        WriteL( (TUint8*)&aRegion[i].iTl.iX, 4 * sizeof(TInt32 ));
        i++;
        }
    
    if ( !iReceivingDrawingCommands )
        {
        WriteInt8L( EAlfCommandEndMarker );
        }    
    }

// ---------------------------------------------------------------------------
// WriteDescriptorAndIntsL
// writes aCount amount of variables to the stream and updates stream index.
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::WriteDescriptorAndIntsL(
        const TUint8& aCommand,
        const TDesC& aText,
        const CGraphicsContext::TTextParameters* aTextParameter,
                TInt aCount,
        TRefByValue<const TInt> aFirst,... )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    if ( aText.Length() == 0 )
        {
        return;
        }
   
   TBool useCachedText( EFalse );
   if ( iCachedText 
           && iCachedText->Length() == aText.Length() 
           && iCachedText->Compare( aText ) == 0 )
       {
       useCachedText = ETrue;
       }
   else
       {
       delete iCachedText;
       iCachedText = aText.AllocL();
       }
   // calculate size for the command
   TInt  size = sizeof(TInt32) * ( 2 + aCount ) + //  ints
                + 2 * sizeof(TInt8);  // existances of: text and TextParameters
               
   if ( !useCachedText )
       {
       size += aText.Size() + sizeof(TInt32); // text + its length  
       }
    
    if ( aTextParameter )
        {
        size += ( 2 * sizeof(TInt32) )+ sizeof(TUint16);
        }

    if (! InitCommandL( aCommand, size )){ return;}
#ifdef _ALF_PRINT_WS_COMMANDS_    
    iCommandDebugger->SetDescriptionAndSize( aCommand, size, R_ALF_COMMAND_DESCRIPTION_ARRAY );
    iCommandDebugger->Print();
#endif                
    WriteInt8L( useCachedText );
    if ( !useCachedText )
        {
        WriteInt32L( aText.Size() );

        TInt offset = iOffset + 2 * sizeof(TUint8);
        TInt startPadding = offset % KDivisibleByX;
        startPadding = startPadding == 0 ? 0 : KDivisibleByX - startPadding;
        WriteInt8L( startPadding ); 

        TInt endPadding = (offset + startPadding + aText.Size() ) % KDivisibleByX;
        endPadding = endPadding == 0 ? 0 : KDivisibleByX - endPadding;
        WriteInt8L( endPadding );
        TUint8 padding[KDivisibleByX];
        WriteL( (TUint8*)padding, startPadding );
        WriteL( (TUint8*)aText.Ptr(), aText.Size() );
        WriteL( (TUint8*)padding, endPadding ); 
        // aTextParameter can be null.
        }
    if ( aTextParameter )
        { 
        // TODO: Remove commented code, when this is tested.
        if ( aTextParameter->iStart == 0 
                && aTextParameter->iEnd == aText.Length()
                && aTextParameter->iFlags == 0 )
            {
			// actually, this never comes so far...
            WriteInt8L( 2 ); // Use the default values
            }
        else
            {
            WriteInt8L( 1 ); 
            TInt size =  2 * sizeof (TInt) + sizeof (TUint16) ;
            WriteL( (TUint8*)&aTextParameter->iStart, size);
            }
        }
    else
        {
        WriteInt8L( 0 );
        }
	if ( aCount > 0 )
	{
	    aCount--;
	    WriteInt32L( aFirst );
	    TInt size = aCount * sizeof(TInt32);
	    WriteL( (TUint8*)&aFirst + sizeof(TInt32), size );
    }
    if ( !iReceivingDrawingCommands )
        {
        WriteInt8L( EAlfCommandEndMarker );
        }
    }

// ---------------------------------------------------------------------------
// WritePointArrayL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::WritePointArrayL( const TUint8& aCommand, const CArrayFix<TPoint>* aPoWriteIntList )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    TInt count = aPoWriteIntList->Count();
    const TInt size = sizeof(TInt32) * (2 * count + 1);

    if (! InitCommandL( aCommand, size )){ return;}

    TInt index(0);
    WriteInt32L( count );
	while( index < count )
        {
        WriteInt32L( aPoWriteIntList->At(index).iX );
		WriteInt32L( aPoWriteIntList->At(index).iY );
		index++;
        }
    if ( !iReceivingDrawingCommands )
        {
        WriteInt8L( EAlfCommandEndMarker );
		}
    }

// ---------------------------------------------------------------------------
// WritePointArrayL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::WritePointArrayL( const TUint8& aCommand, const TPoint* aPoWriteIntList, TInt aNumPoints )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    const TInt size = sizeof(TInt32) * (2 * aNumPoints + 1);
    if (! InitCommandL( aCommand, size )){ return;}

    TInt index(0);
    WriteInt32L( aNumPoints );
	while( index < aNumPoints )
        {
        WriteInt32L( aPoWriteIntList[index].iX );
		WriteInt32L( aPoWriteIntList[index].iY );
		index++;
        }
    if ( !iReceivingDrawingCommands )
        {
        WriteInt8L( EAlfCommandEndMarker );
        }
    }

// ---------------------------------------------------------------------------
// WritePointArrayL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::WritePointArrayL( const TUint8& aCommand, const TArray<TPoint>* aPoWriteIntList  )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    const TInt size = sizeof(TInt32) * (2 * aPoWriteIntList->Count() + 1);
    if (! InitCommandL( aCommand, size )){ return;}

    TInt index(0);
    TInt count = aPoWriteIntList->Count(); 
    WriteInt32L( count );
    TArray<TPoint> list = *aPoWriteIntList;
            
    while( index < count )
        {
        const TPoint& point = list[index]; 
        WriteInt32L( point.iX );
		WriteInt32L( point.iY );
		 index++;
        }   
    if ( !iReceivingDrawingCommands )
        {
        WriteInt8L( EAlfCommandEndMarker );
		}
    }

// ---------------------------------------------------------------------------
// Synchronize
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::Synchronize(TInt aId)
    {
    if ( iAlfBridgerClient )
        {
        TIpcArgs args(aId);
        iAlfBridgerClient->SendSynch(EAlfSynchronize, args); // error ignored
        }
    }

// ---------------------------------------------------------------------------
// ConnectL
// ---------------------------------------------------------------------------
//
TBool CAlfRsSendBuffer::ConnectL()
    {
/*    if (iDisabled) // return if this send buffer is not in use
        {
        return ETrue;
        }
  */  
    if ( iAlfBridgerClient )
        {
        return ETrue; // already connected
        }
    iAlfBridgerClient = new(ELeave)RAlfBridgerClient();
    TInt result = iAlfBridgerClient->Connect();
    if ( result != KErrNone )
        {
        // server is not available
        delete iAlfBridgerClient;
        iAlfBridgerClient = NULL;
        return EFalse;
        }
    TIpcArgs args(iScreenNumber);
    User::LeaveIfError(iAlfBridgerClient->SendSynch(KAlfCompositionWServScreenNumber, args));

    MAlfCompositionController* compcntrl = (MAlfCompositionController*)iParent.ResolveObjectInterface(KAlfCompositionControllerIfUid);
    if (compcntrl)
      {
#ifndef USE_UI_LAYER_FOR_ALF
        TAlfNativeWindowData data;
        TPckg<TAlfNativeWindowData> pkg(data);

        if( iScreenNumber == 0) // only for primary screen
            {
            User::LeaveIfError(iAlfBridgerClient->SendSynch(EAlfGetNativeWindowHandles, TIpcArgs(&pkg)));
            compcntrl->AlfBridgeCallback(MAlfBridge::EAlfNativeWindowCreated, &data); 
            }
        }
 #endif //#ifdef USE_UI_LAYER_FOR_ALF   
    iAlfCompositionCntrlClient = CAlfCompositionCntrlClient::NewL(iAlfBridgerClient, compcntrl);
    return ETrue;
    }

// ---------------------------------------------------------------------------
//  WriteWindowIdentifierL
//  Convenience function
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::WriteWindowIdentifierL( )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
#ifdef _OLD_STREAM
   if ( !iBufStream->Sink() )
        {
        return;
        }
#endif    
    // <HEADER1>
    // if you change <HEADER1>, you must update its size in the beginning of this file
    WriteInt8L( EAlfSetWindowId );
	ResetPatternSearch();
    
    WriteInt8L( iScreenNumber );
    WriteInt32L( iWindowId );
	// </HEADER1>
    WriteFollowingFrameOffsetTemplateL();
     }

// ---------------------------------------------------------------------------
//  WriteWindowDataL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::WriteWindowDataL(
		TUint32 aWindowUid,
		TInt aRegionSize, 
		TInt aShapeRegionSize )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
	// TODO: TERO: END	
    iNonSupportedCommandsInWindow = 0;
    iPerformanceIssueCommandCount = 0;
    iFlags = 0;
    delete iCachedText;
    iCachedText = NULL;

    iPreviousCommand = EAlfCommandNotInitialized;
	// Note, the size tells the maximum space needed for the command
    TInt size = sizeof(TUint32) + // windowId
        sizeof(TUint32) + // next frame offset
        3 * sizeof(TUint8) + // command + contains unsupported commands + end marker
        sizeof(TUint8) + sizeof(TUint32) + // orientation
        sizeof(TUint8) + // screen number ( in WriteFollowingFrameOffsetTemplate)
        sizeof(TUint32) * ( 4 * aRegionSize + 1 ) +  // updateregion 
        sizeof(TUint8) * KDivisibleByX + // possible padding
        sizeof(TUint32) * ( 4 * aShapeRegionSize + 1) + // possible shape region
        sizeof(EAlfCommandEndMarker); // endmarker for update region and this command
    if (! InitCommandL( EAlfSetWindowId, size )){ return;}
#ifdef _ALF_PRINT_WS_COMMANDS_    
    iCommandDebugger->StartFrame();
#endif
    ResetPatternSearch();
    iWindowId = aWindowUid;
    WriteInt32L( aWindowUid );
	WriteFollowingFrameOffsetTemplateL();
    iReceivingDrawingCommands = ETrue; // WriteCommand will set to false, when ERedrawComplete is received
    }

// ---------------------------------------------------------------------------
//  WriteFollowingFrameOffsetTemplateL
//  Convenience function
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::WriteFollowingFrameOffsetTemplateL()
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
		
#ifdef _OLD_STREAM
    if ( iBufStream->Sink() )
#endif        
        {
        iTemplateOpen++;
        if ( iTemplateOpen != 1 )
            {
            USER_INVARIANT();
            }
        iNextFrameOffsetPos = iOffset;
        
        // If you add anything to <HEADER2>, you must update its size in the beginning of the file.
        // <HEADER2>
        TInt chunkId = 0;
        if ( iChunkInUse)
            {
            chunkId = iCacheChunks[iChunkInUse-1].Handle(); 
            }
        WriteInt32L( chunkId );
		WriteInt32L( KFrameOffsetTemplate ); // we'll come back later on, and write the correct frame size to this location
        // determine need for padding
        TInt pos = iOffset; 
        TInt offset = pos + 2 * sizeof(TUint8) + sizeof(TUint8) + sizeof(TInt32); // padding and endmarker.
        TInt startPadding = offset % KDivisibleByX;
        startPadding = startPadding == 0 ? 0 : KDivisibleByX - startPadding;
        WriteInt8L( EAlfFrameFlags );
		WriteInt32L( 0 ); 
		
        WriteInt8L( startPadding );
		iFramePadding = startPadding;
        WriteInt8L( EAlfCommandEndMarker );
		// actual padding
        while ( startPadding-- )
            {
            WriteInt8L( 0 );
			}
#ifdef RD_SUBWINDOW_EFFECTS         
        iArrayImplOffset = iOffset;
        InitMarker(iMarker);
#endif        
		WriteInt8L( EAlfFrameContainsUnsupportedCommands );
        WriteInt8L( 0 );
		WriteInt8L( EAlfFrameOrientation );
		WriteInt32L( iOrientation ); 
        // These are for subwindow effects, that are not currently enabled
#ifdef RD_SUBWINDOW_EFFECTS         
        WriteArrayHeaderTemplateL();
        InitTOffsetElemArray(iOffsetArray);
#endif
        // </HEADER2>
        }
    }

// ---------------------------------------------------------------------------
//  WriteFollowingFrameOffsetL
// ---------------------------------------------------------------------------
//
TBool CAlfRsSendBuffer::WriteFollowingFrameOffsetL(TBool aSendArray)
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return ETrue;
        }
#ifdef RD_SUBWINDOW_EFFECTS  
    TOffsetElem e;
    if (aSendArray)
        {
        e = WriteIndexArrayL(iOffsetArray);
        }
#endif
    
#ifdef _OLD_STREAM    
    if ( iBufStream->Sink() )
#endif        
        {
        if ( iPerformanceIssueCommandCount > KPossiblePerformanceProblemInWindowThreshold )
            {
            if ( iPerformanceIssueCommandCount > iMaxPerformanceIssueCommandCount )
                {
                iMaxPerformanceIssueCommandCount = iPerformanceIssueCommandCount;
                }
            iNonSupportedCommandsInWindow |= KPossiblePerformanceProblemInWindow; 
            }
        
        iTemplateOpen--;
        // Empty frames should not be serialized as they cause unnecessary work on Alfserver side. 
        // WServ sends empty frames from time to time
        TInt previousPos = iOffset;
        // Remember to update KWindowFrameHeaderX, if you add stuff to frame header
        TInt frameSize = previousPos - iNextFrameOffsetPos - KWindowFrameHeader2 - iFramePadding; 
        if ( !frameSize ) 
            {
            SeekL( iNextFrameOffsetPos - KWindowFrameHeader1 );
            iReceivingDrawingCommands = EFalse;
            return EFalse;  
            }
        SeekL( iNextFrameOffsetPos );
        // <HEADER2>
        TInt chunkId = 0;
        if ( iChunkInUse)
            {
            chunkId = iCacheChunks[iChunkInUse-1].Handle(); 
            }
        WriteInt32L( chunkId );
		WriteInt32L( previousPos );
		WriteInt8L( EAlfFrameFlags );
		WriteInt32L( iFlags );
		
        WriteInt8L( iFramePadding ); 
		WriteInt8L( EAlfCommandEndMarker ); 
		// Padding
        while ( iFramePadding-- )
            {
            WriteInt8L( 0xff );
			}
        WriteInt8L( EAlfFrameContainsUnsupportedCommands );
		WriteInt8L( iNonSupportedCommandsInWindow );
	    WriteInt8L( EAlfFrameOrientation );
	    WriteInt32L( iOrientation ); 
#ifdef RD_SUBWINDOW_EFFECTS 
		if (aSendArray) 
		    {
		    WriteArrayHeaderL(e);
		    }
#endif
		/*iWindowHeaderStruct.iWindowEndOffset = previousPos.Offset();
        iWindowHeaderStruct.iUnsupportedCommandsInWindow = iNonSupportedCommandsInWindow;
        iBufStream->WriteL( (TUint8*)&iWindowHeaderStruct, sizeof(TWindowHeaderStruct) );*/
        // <HEADER2>
        // return to end of frame
        SeekL( previousPos );
        
        if ( iTemplateOpen )
            {
            USER_INVARIANT();
            }
        }
    return ETrue; 
    }
//
// ARRAY IMPLEMENTATION
//

#ifdef RD_SUBWINDOW_EFFECTS

const TInt KArrayOffsetTemplate = 23456789;
const TInt KArraySizeTemplate = 23456789;

void CAlfRsSendBuffer::WriteArrayHeaderTemplateL()
{
    WriteInt8L( EAlfCommandIndexArrayHeader );
    WriteInt8L( 0 ); // align
    WriteInt32L( KArrayOffsetTemplate ); // these will be rewritten in WriteArrayHeader2
    WriteInt32L( KArraySizeTemplate );   // these will be rewritten in WriteArrayHeader2
}


void CAlfRsSendBuffer::InitTOffsetElemArray(RArray<TOffsetElem> &aOffsets)
{
    TInt count = aOffsets.Count();
    for(TInt i=0;i<count;i++)
        aOffsets.Remove(0);
}

CAlfRsSendBuffer::TOffsetElem CAlfRsSendBuffer::WriteIndexArrayL(const RArray<TOffsetElem> &aOffsets)
{
    TInt marker = iBufStream->Sink()->TellL( MStreamBuf::EWrite ).Offset();
    if (iPreviousBlockOffset != marker && iLayersEnabled)
        {
        TInt size = marker - iPreviousBlockOffset;
        TOffsetElem e;
        e.iOffset = iPreviousBlockOffset - iArrayImplOffset; //iFrameBeginOffset + KWindowFrameHeader1 + KWindowFrameHeader2;
        e.iSize = size;
        e.iBoundingRectangle = TRect(0,0,0,0);
        e.iLayerId = iExtraLayerId++;
        iOffsetArray.AppendL( e ); 
        }
    InsertPaddingL();

   // first command to allow wspainter to skip the index array section
   WriteInt8L(EAlfCommandIndexArray);
   WriteInt8L(aOffsets.Count());
   WriteInt8L(0);
   WriteInt8L(0);
   TInt offset = iOffset;
   TInt count = aOffsets.Count();

   // then the actual index array
   for(TInt i = 0; i<count; i++)
	{
	WriteInt32L( aOffsets[i].iOffset );
	WriteInt32L( aOffsets[i].iSize );	
	WriteInt32L( aOffsets[i].iLayerId);
	WriteL( (TUint8*)&aOffsets[i].iBoundingRectangle, sizeof(TRect) );
	}
   TOffsetElem e;
   e.iOffset = offset  - iArrayImplOffset; //iFrameBeginOffset + KWindowFrameHeader1 + KWindowFrameHeader2;
   e.iSize = aOffsets.Count()*sizeof(TOffsetElem);
   return e;
}

void CAlfRsSendBuffer::WriteArrayHeaderL(const TOffsetElem &aIndexArrayOffset)
{
   WriteInt8L( EAlfCommandIndexArrayHeader );
   WriteInt8L( 0 ); // align
   WriteInt32L( aIndexArrayOffset.iOffset );
   WriteInt32L( aIndexArrayOffset.iSize );
}

void CAlfRsSendBuffer::InitMarker(TInt &aMarker)
{
   aMarker = -1;
   iPreviousBlockOffset = iBufStream->Sink()->TellL( MStreamBuf::EWrite ).Offset();
   iExtraLayerId = 667;
   iLayersEnabled = EFalse;
}
void CAlfRsSendBuffer::StartMarkerL(TInt &aMarker, TRect &aRectangle, TInt &aLayer, TRect aBoundingRectangle, TInt aLayerId)
{
   ASSERT(aMarker == -1);
   aMarker = iBufStream->Sink()->TellL( MStreamBuf::EWrite ).Offset();
   if (iPreviousBlockOffset != aMarker)
       {
       TInt size = aMarker - iPreviousBlockOffset;
       TOffsetElem e;
       e.iOffset = iPreviousBlockOffset - iArrayImplOffset; //iFrameBeginOffset + KWindowFrameHeader1 + KWindowFrameHeader2;
       e.iSize = size;
       e.iBoundingRectangle = TRect(0,0,0,0);
       e.iLayerId = iExtraLayerId++;
       iOffsetArray.AppendL( e ); 
       }
   
   //aMarker = iOffset;
   aRectangle = aBoundingRectangle;
   aLayer = aLayerId;
   iLayersEnabled = ETrue;
}
void CAlfRsSendBuffer::EndMarkerL(RArray<TOffsetElem> &aOffset, TInt &aMarker, const TRect &aRectangle, TInt aLayerId)
{
   //TODO ASSERT(aMarker != -1);
   if (aMarker==-1) return; 
   TInt offset = iBufStream->Sink()->TellL( MStreamBuf::EWrite ).Offset();

   TInt size = offset - aMarker;
   TOffsetElem e;
   e.iOffset = aMarker - iArrayImplOffset; //iFrameBeginOffset + KWindowFrameHeader1 + KWindowFrameHeader2;
   e.iSize = size;
   e.iBoundingRectangle = aRectangle;
   e.iLayerId = aLayerId;
   aOffset.AppendL( e ); 
   aMarker = -1;
   iPreviousBlockOffset = offset;
}
void CAlfRsSendBuffer::StartMarkerL(TRect aBoundingRectangle, TInt aLayerId)
    {
    StartMarkerL(iMarker, iBoundingRectangle, iLayerId, aBoundingRectangle, aLayerId);
    }
void CAlfRsSendBuffer::EndMarkerL()
    {
    EndMarkerL(iOffsetArray, iMarker, iBoundingRectangle, iLayerId);
    }
#endif // RD_SUBWINDOW_EFFECTS

// ---------------------------------------------------------------------------
// SendL
// sends data syncronously in one or more packets to the streamer server
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::SendL( TRequestStatus* aStatus )
	{
	// dont send to alf, if chunk is not available or nothing was written
	if (iDisabled || !iChunk.Handle() )
        {
        if ( aStatus )
            {
            // AlfRenderStage may complete the request, because we wont do anything with the data
            iParent.EndCallBack( aStatus );
            }
        return;
        }

    WriteInt8L( EAlfCommitBatch );
	WriteInt8L( iScreenNumber );
	WriteInt8L( EAlfCommandEndMarker );
	
    iState = EWaitingAck;
    TInt lastWrittenOffset( iOffset );
    Commit();
    
    TIpcArgs args( lastWrittenOffset, iCompositionModified );
    //RDebug::Printf("CAlfRsSendBuffer::SendL, offset %d",lastWrittenOffset );
    if ( iFlushBufferTimer ) 
        {
        iFlushBufferTimer->Cancel();
        }
    iAlfBridgerClient->SendAsynchronous( EAlfBridgerAsyncronousData, args, iStatus );
    iCompositionModified = EFalse;

    if ( aStatus )  // aStatus is null, if this was event notification and not drawing
        {
        iQuedStatus = aStatus;
        *aStatus = KRequestPending;
        }

    if ( !IsActive() )
        {
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// FlushBuffer
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::FlushBuffer()
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    TBool connected = (iAlfBridgerClient != NULL);
    if (!connected)
        {
        TRAP_IGNORE(connected = ConnectL());
        }
    if (!connected)
        {
        // Cannot connect
        return;
        }
    TIpcArgs args( 0 );
    TInt result = iAlfBridgerClient->SendBlind( EAlfBridgerBlindSend, args  );
    if ( result != KErrNone && result != KErrServerBusy)
    	{
    	__ALFLOGSTRING1("CAlfRsSendBuffer::FlushBuffer, err %d", result );
    	// This means that AlfDecoderServer has died and is not coming back. 
    	// There is no point of continuing with non updating UI.
   		USER_INVARIANT();
    	}
    if ( iFlushBufferTimer ) 
    	{
    	iFlushBufferTimer->Cancel();
    	}

	// during boot time, server is known to answer with this. Just try again little later.
	if ( result == KErrServerBusy)
		{
		CommitL();
		}
    }

// ---------------------------------------------------------------------------
// CommitL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::CommitL( )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    if ( iBooting )
        {
        return;
        }
    Commit();
    // RDebug::Print(_L("CAlfRsSendBuffer::CommitL - %d %d %d "), iChunkHeader, iChunkHeader->iCommittedWriteOffset, iOffset );
      if ( !iFlushBufferTimer )
        {
        iFlushBufferTimer = CPeriodic::NewL( EPriorityNormal );
        iFlushBufferTimer->Start( KFlushBufferTimeout, 10 * 1000000 , TCallBack( doFlushBuffer, this ));
        }
    if ( !iFlushBufferTimer->IsActive() )
        {
        //__ALFLOGSTRING("CAlfRsSendBuffer::CommitL, activating timer");
        iFlushBufferTimer->After( KFlushBufferTimeout );
        }
    else
    	{
        iFlushBufferTimer->Cancel();
        iFlushBufferTimer->After( KFlushBufferTimeout );
    	//__ALFLOGSTRING("CAlfRsSendBuffer::CommitL, timer already active ");
    	}
    }

// ---------------------------------------------------------------------------
// SendAsyncCmd
// Send asynchronous command
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::SendAsyncCmd(TInt aCmd, TDes8& aBuf, TRequestStatus& aStatus)
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
   iAlfBridgerClient->SendAsynchronous(aCmd, TIpcArgs(&aBuf), aStatus);
    }

// ---------------------------------------------------------------------------
// OpenPrimaryChunkForWritingL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::OpenPrimaryChunkForWritingL()
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    iChunkHeader = reinterpret_cast<TChunkHeader*>(iChunk.Base());
    if ( iChunkHeader->iMemWriteStream )
        {
#ifdef _OLD_STREAM        
        iBufStream->Close();
        delete iBufStream;
        iBufStream=NULL;
        iBufStream = new(ELeave)RMemWriteStream;
        // iBufStream = iChunkHeader->iMemWriteStream;
        iBufStream->Open( iChunk.Base() + sizeof( TChunkHeader), iPrimaryChunkMaxSize );
#else
        delete iStreamPtr;
        iStreamPtr = NULL;
        iStreamPtr = new(ELeave)TPtr8( (TUint8*)(iChunk.Base() + sizeof( TChunkHeader)), iPrimaryChunkMaxSize - sizeof( TChunkHeader));
        iChunkHeader->iMemWriteStream = (RMemWriteStream*)1;
#endif
        SeekL( iOffset );
        iChunkInUse = 0;
        }
    else
        {
#ifdef _OLD_STREAM
        iBufStream->Open( iChunk.Base() + sizeof( TChunkHeader), iPrimaryChunkMaxSize );
        iChunkHeader->iMemWriteStream = iBufStream;
#else
        iStreamPtr = new(ELeave)TPtr8( (TUint8*)(iChunk.Base() + sizeof( TChunkHeader)), iPrimaryChunkMaxSize - sizeof( TChunkHeader) );
        iChunkHeader->iMemWriteStream = (RMemWriteStream*)1;
#endif        
        SeekL(0);
        iChunkHeader->iMemWriteStreamUsers++;
        }
    WriteInt8L( EAlfCommandEndMarker );
    Commit();
    
    }

// ---------------------------------------------------------------------------
// Commit
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::Commit()
    {
    iChunkHeader->iCommittedWriteOffset = iOffset;
    // RDebug::Print(_L("CAlfRsSendBuffer::Commit - %d"), iChunkHeader->iCommittedWriteOffset );
    }

// ---------------------------------------------------------------------------
// PrepareBufferL
// Sets up the stream
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::PrepareBufferL()
	{
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
	// is alfstreamerserver running?
	if ( !ConnectL() )
        {
        return;
        }
	iBeginTime.HomeTime();
	 
	// alfstreamerserver is now running, but do we already have chunk for serialization
	if ( iChunk.Handle() == 0 )
	    {// Note, ConnectL() == ETrue and iChunk.Handle() == 0  is true ONLY ONCE in runtime.  

	    // write jump to the primary chunk. This chunk jump gets special treatment in RunL. Look for iBooting variable.
	    JumpToAnotherChunkL( 0, iPrimaryChunkMaxSize ); // 2nd parameter is ignored on alfside, because it knows the primary chunksize
	    Commit();
#ifdef _OLD_STREAM	    
	    iBufStream->Close();
#endif
	    // Request chunk from server: handle and lenght of chunk come as response
	    TPckg<TInt> pkgLength(  iUsedChunkMaxSize );
	    TIpcArgs args( &pkgLength, iScreenNumber );
	    TInt handle;
	    handle = iAlfBridgerClient->SendSynch( EAlfBridgerRequestDataBlock, args);
	    iPrimaryChunkMaxSize = iUsedChunkMaxSize;
	    iChunk.SetReturnedHandle( handle );

	    if ( iChunk.Handle() > 0 )
	        {
	        OpenPrimaryChunkForWritingL();
	        }
	    else
	        {
	        // Chunk was not received from server. We cannot continue
	        USER_INVARIANT();
	        }
	    }
	// The first boot cache chunk is sent here. 
	if ( iBooting && iCacheChunks.Count() && iChunkInUse )
	    {
	    if ( iChunkHeader->iMemWriteStreamUsers > 1 ) 
            {
            // make "sure", that there will be space for all the cached data
            CommitL();
            }

	    while ( iChunkInUse )
             {
             iChunkInUse--;
             TInt size( KCacheChunkMinSize );
             TIpcArgs args( size, iCacheChunks[iChunkInUse], iChunkInUse == 0 /* open chunk for reading */);
             iAlfBridgerClient->SendSynch( EAlfBridgerSendChunk, args);
             }	    
        }
	iFrameBeginOffset = iOffset; // iBufStream->Sink()->TellL( MStreamBuf::EWrite);
	}

// ---------------------------------------------------------------------------
// FrameContainsDataL
// ---------------------------------------------------------------------------
//
TBool CAlfRsSendBuffer::FrameContainsDataL()
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return EFalse; // Must return false as no draw commands are allowed 
        }
		
#ifdef _OLD_STREAM    
    if ( iBufStream->Sink() )
#endif        
        {
#ifdef _OLD_STREAM        
        iBufStream->CommitL();
#endif        
        return ( iFrameBeginOffset != iOffset );
        }
#ifdef _OLD_STREAM
    else
        {
        return EFalse;
        }
#endif    
    }

// ---------------------------------------------------------------------------
// JumpToAnotherChunkL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::JumpToAnotherChunkL( TInt32 aChunkId, TInt aChunkSize )
    {
    WriteInt8L( EAlfJumpToAnotherChunk );
	WriteInt8L( 0 ); // screen number, not used.
    WriteInt32L( aChunkId );
	WriteInt32L( aChunkSize );
	WriteInt8L( EAlfCommandEndMarker );
	Commit();
    }

// ---------------------------------------------------------------------------
// DoCreateTemporaryChunkL
//
// If wserv pushes more data in frame that we can handle,  separate one time use 
// "temporary" chunks are taken into use. 
//  1. Big enough chunk to contain atleast the command + minimum chunk size is created
//  2. Jump of from the current chunk to the temporary chunk is created
//  3. new chunk is taken as the active chunk
//  4. chunk is rewinded
//  5. commands can be serialized as before
//
//  Same starts from the step 1, if temporary chunk turns out to be not big enough.
//
//  Chunks are destroyed in RunL, when asyncronous (frame processed) complete is
//  received. In RunL, (see RunL for the code)
//  
//  The following things are serialized to the active (the last temporary chunk created)
//  1. destroy command for each existing temporary chunk is created
//  2. if not in first boot, jump to the primary chunk is created.
//  3. handles to the temporary chunks on this side are closed 
//     and chunks removed.
//
//  After AlfHierarchy has read the jump command, serialization will be in the normal mode.
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::DoCreateTemporaryChunkL( TInt aMinimumRequiredSize )
    {
    __ALFLOGSTRING2("CAlfRsSendBuffer::DoWaitWrapperL iReceivingDrawing commands %d chunk in use: %d", iReceivingDrawingCommands, iChunkInUse );
    // Optimize bitblits backtracks in a _single chunk_, so changing or wrapping a chunk is 
    // a very bad idea. Lets skip the optimization, if this rare occation happens.
    ResetPatternSearch(); 
            
    if ( iReceivingDrawingCommands )
         {
         // if window drawing is active, the window commands are split. 
         InsertPaddingL(); // padding is required, that the combined parts on the other side have correct alignment for descriptors
         WriteFollowingFrameOffsetL(EFalse);
         WriteInt8L( EAlfPacketNotReady );
		 WriteInt8L( EAlfCommandEndMarker );
		 }
    else
        {
        __ALFLOGSTRING("CAlfRsSendBuffer::DoCreateTemporaryChunkL, creating during hierarchy commands");
        }
    
    // rest of this frame will be serilized to cache chunk
    CommitL();
    if ( ConnectL() )
        {
        FlushBuffer();
	    }
    TInt chunkSize = aMinimumRequiredSize;
    if ( CreateTemporaryChunkL( chunkSize ) == KErrNone )
        {
        if ( ConnectL() )
           {
           TInt size( chunkSize );
           TIpcArgs args( size, iCacheChunks[iChunkInUse], EFalse /* open chunk for reading */ );
           iAlfBridgerClient->SendSynch( EAlfBridgerSendChunk, args); // alf server must open the chunk before it can be read. reading is asyncronous
           }
        JumpToAnotherChunkL( iCacheChunks[iChunkInUse].Handle(), chunkSize );
        OpenRewindChunkL( chunkSize );
        WriteInt8L( EAlfCommandEndMarker ); // This is must command after jumping to another chunk.
		CommitL();

        if ( iReceivingDrawingCommands )
            {
            WriteWindowIdentifierL();
            }
        }
    else
        {
        // TODO: We could not reserve memory. The only option will be to ignore this frame.
        // 1. rollback to the beginning of the frame
        // 2. skip all sequential drawing commands to this frame.
        // problem: what if OOM during hierarchy tree commmands
        }
 }

// ---------------------------------------------------------------------------
// ReserveSpaceL
//
// Check, if buffer has enough space for this command.
// If we encounter the end of buffer, then do a wrap around if possible.
// If wrap is not possible, create a temporary space for command
// ---------------------------------------------------------------------------
//
TBool CAlfRsSendBuffer::ReserveSpaceL( TInt aCommandSize )
    {
    // TInt minumumHeaderSize = iCacheChunks.Count() > 0 ? KFrameHeaderSizeWhenCacheChunks : KFrameHeaderSize;
    TInt minumumHeaderSize = KFrameHeaderSize;
    if ( iCacheChunks.Count() > 0 )
        {
        // if cache chunks still exists, then wserv is trying to push commands out of redrawstart/end loop and we hope that 
        minumumHeaderSize = KFrameHeaderSizeWhenCacheChunks; 
        }
    TInt writeOffset = iOffset;
    TInt commandTailOffset = writeOffset + aCommandSize + KFrameHeaderSize;
    TInt readOffset = iChunkHeader->iReadOffset;
    TInt bytesFree(0);
    if ( readOffset > writeOffset)
        {//                       V
        // |RRRRRRRRR|W---------RRRRRRRRRRRR | 
       // ASSERT( iWrappingFrame );
       bytesFree = readOffset - writeOffset;
       }
    if ( readOffset < writeOffset )
       {
       //       V              W
       // | ----RRRRRRRRRRRRRRRW------ |
       iWrappingFrame = EFalse;
       bytesFree = iUsedChunkMaxSize - writeOffset; // there might be more free and the beginning of the chunk, but this is not relevant. The entire command needs to fit as one piece
       }
    if ( readOffset == writeOffset )
       {
       if ( iWrappingFrame )
           {
           //                V
           // | RRRRRRRRRRRRRWRRRRRRRRRRRRR |
           bytesFree = 0;
           }
       else
           {
           //  V
           // | --------------------------- |
           // the entire chunk is free
           bytesFree = iUsedChunkMaxSize;
           }
       }
    // are we trying to overwrite unprocessed data
    if ( bytesFree < aCommandSize + minumumHeaderSize && commandTailOffset <= iUsedChunkMaxSize 
            || ( bytesFree < aCommandSize + minumumHeaderSize && iWrappingFrame ))
        {
        // __ALFLOGSTRING("CAlfRsSendBuffer::CanWriteToBufferL -> DoCreateTemporaryChunkL");
        DoCreateTemporaryChunkL( aCommandSize );
        }
    else if ( commandTailOffset > iUsedChunkMaxSize )
        {
        if ( !iChunkInUse )
            {
            DoWrapL( aCommandSize,  aCommandSize > readOffset );
            }
        else
            {
            // we are already using temporary chunks and those cannot be wrapped. The only option is to create 
            // yet another temporary chunk.
            DoCreateTemporaryChunkL( aCommandSize );
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// DoWrapL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::DoWrapL( TInt aCommandSize, TBool aCreateTempororaryChunk )
    {
    if ( iCacheChunks.Count() )
        {
        DoCreateTemporaryChunkL( aCommandSize );
        }
    __ALFLOGSTRING2("CanWriteToBufferL Read/Write %d/%d", iChunkHeader->iReadOffset, iChunkHeader->iWriteOffset);

    if ( aCreateTempororaryChunk )
        {
        __ALFLOGSTRING("CAlfRsSendBuffer::DoWrapL -> DoCreateTemporaryChunkL");
        DoCreateTemporaryChunkL( aCommandSize );
        return;
        }
    else
        {
        if ( iReceivingDrawingCommands )
            {
            InsertPaddingL();
            WriteFollowingFrameOffsetL(EFalse);
            WriteInt8L( EAlfPacketNotReady );
			WriteInt8L( EAlfCommandEndMarker );
			}
        else
            {
            __ALFLOGSTRING("CAlfRsSendBuffer::CanWriteToBufferL, non drawing command");
            }
        }
    WriteInt8L( EAlfWrap );
	WriteInt8L( iScreenNumber );
	WriteInt8L( EAlfCommandEndMarker );
	// Optimize bitblits backtracks in a _single chunk_, so changing or wrapping a chunk is 
    // a very bad idea. Lets skip the optimization, if this rare occation happens.
    ResetPatternSearch(); 

    iWrappingFrame = ETrue;
    __ALFLOGSTRING1("CAlfRsSendBuffer::CanWriteToBufferL, Wrap at offset %d", iChunkHeader->iWriteOffset);
    SeekL(0);
    WriteInt8L( EAlfCommandEndMarker );
	
    if ( iReceivingDrawingCommands )
        {
        WriteWindowIdentifierL();
        }
    else
        {
        __ALFLOGSTRING("CAlfRsSendBuffer::CanWriteToBufferL Wrapping while not receiving drawing commands");
        }
    }

// ---------------------------------------------------------------------------
// SetSupportedCommand
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::SetSupportedCommand( TInt aIndex, TInt8 aSupport )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    iNonSupportedCommands[aIndex] = aSupport;
    }

// ---------------------------------------------------------------------------
// InitCommandL
// Takes care that data fits to the stream
// ---------------------------------------------------------------------------
//
TBool CAlfRsSendBuffer::InitCommandL( const TUint8& aCommand, TInt aCommandSize  )
    {
	// THIS IS DIRTY HACK UNTIL WSERV STOPS DOING DRAWING OUTSIDE WINDOW
	// RE-EVALUATE FOR WK14 RELEASE
    if ( ( aCommand >= EAlfDrawCommandsStart && aCommand <= EAlfDrawCommandsEnd && !iReceivingDrawingCommands ) && 
            ( aCommand != EAlfPacketReady && aCommand != EAlfPacketNotReady ))
        {
#ifdef _DEBUG
        RDebug::Print(_L("CAlfRsSendBuffer::InitCommandL - Drawing outside window, Cmd: %d"), aCommand );
#endif
        return EFalse;
		}
	// END OF DIRTY HACK    
    if ( iReceivingDrawingCommands )
        {
        aCommandSize += sizeof(EAlfCommandEndMarker);
        }
    iNonSupportedCommandsInWindow |= iNonSupportedCommands[aCommand] & KAllRenderersMask;
    if ( iNonSupportedCommands[aCommand] & KPossiblePerformanceProblemInWindow ) // indicates possible performance problem if frame has many of this of commands
        {
        iPerformanceIssueCommandCount++;
        }
     
    // Check if chunk has been opened. Must for streaming.
    if ( 
#ifdef _OLD_STREAM
            !iBufStream && 
#else
            !iChunkHeader  &&
#endif            
            iCacheChunks.Count() == 0 )
        {
        TInt chunkSize = aCommandSize;
        CreateTemporaryChunkL( chunkSize );
        OpenRewindChunkL( chunkSize );
        }


    // EAlfPacketReady is the last command in the window, and it is guaranteened to fit without checking. Checking would possible slice the command stream unnecessary.
    if ( aCommand != EAlfPacketReady )
        {
        // check for running out of space in the chunk
        if ( !ReserveSpaceL( aCommandSize + 2*sizeof(TUint8)) )
            {
            return EFalse;
            }
        }
    // write command to a chunk. There will be space.
    // __ALFLOGSTRING3("Command %d, Offset: %d Screen: %d Command: %d",  aCommand, iScreenNumber, aCommand );
    WriteInt8L( aCommand );
	iPreviousCommand = aCommand;
    if ( aCommand < EAlfDrawCommandsStart || aCommand > EAlfDrawCommandsEnd )
    	{
    	WriteInt8L( iScreenNumber );
		}
    else
        {
        // Uncomment following line to do chaff bitblit optimization
#ifndef __NVG // TODO: Implement the 9-piece drawing for NVG
        // DoPatternSearch( aCommand, aCommandSize );
#endif
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CreateTemporaryChunkL
// ---------------------------------------------------------------------------
//
TBool CAlfRsSendBuffer::CreateTemporaryChunkL( TInt& aSize )
    {
    aSize+= KCacheChunkMinSize;
    iCacheChunks.Append( RChunk() );
    TInt result = iCacheChunks[iChunkInUse].CreateDisconnectedGlobal( KNullDesC, 0, aSize, aSize );
    return result;
    }

// ---------------------------------------------------------------------------
// OpenRewindChunkL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::OpenRewindChunkL( TInt aSize )
    {
#ifdef _OLD_STREAM    
    if ( iBufStream && iChunkInUse) 
        {
        iBufStream->Close();
        }
    iBufStream = new(ELeave)RMemWriteStream;
    iBufStream->Open( iCacheChunks[iChunkInUse].Base() + sizeof( TChunkHeader), aSize );
#else
    iStreamPtr = new(ELeave)TPtr8( (TUint8*)(iCacheChunks[iChunkInUse].Base() + sizeof( TChunkHeader)), aSize - sizeof( TChunkHeader));
    
#endif
    iChunkHeader = reinterpret_cast<TChunkHeader*>(iCacheChunks[iChunkInUse].Base());
    SeekL(0);
    memset( iChunkHeader, 0, sizeof( TChunkHeader ) );
    iUsedChunkMaxSize = aSize -  sizeof( TChunkHeader);
    iChunkInUse++; 
    }

// ---------------------------------------------------------------------------
// DoPatternSearch
//
// Pattern search seeks for predefined sequence of commands. In TSearchPatternBitBlit case the seqence is 
// EAlfSetBrushStyle -> EAlfSetBrushStyle -> EAlfSetClippingRegion -> EAlfBitBltMasked -> EAlfResetClippingRegion .
//
// When possible pattern is found, the start offset of the pattern is saved. If pattern is
// noticed 8 or more times in sequence, the pattern is complete. Functions seeks back to the
// start offset and serializes cached data of the pattern. Some data can be dropped as it has 
// no significance.
// ---------------------------------------------------------------------------
//
#ifdef _ALF_PRINT_WS_COMMANDS_
void CAlfRsSendBuffer::DoPatternSearch( const TUint8& aCommand, TInt aSize )
#else
void CAlfRsSendBuffer::DoPatternSearch( const TUint8& aCommand, TInt )
#endif
    {
    TInt sizeOfPattern = sizeof(TSearchPatternBitBlit) / (3 * sizeof(TInt32)); // rows = size / width
    if ( ( TSearchPatternBitBlit[iPatternSearchState][0] == aCommand  
    		|| ( TSearchPatternBitBlit[iPatternSearchState][2] != KErrNotFound && TSearchPatternBitBlit[iPatternSearchState][2] == aCommand ))
    		&& TSearchPatternBitBlit[iPatternSearchState][1] == iPatternSearchState )
    	{
    	TInt tmp = iPatternSearchState;
    	iPatternSearchState = TSearchPatternBitBlit[iPatternSearchState+1][1];
#ifdef _ALF_PRINT_WS_COMMANDS_
    	TPatternCommand command( aCommand, aSize );
    	iPatternCommands.Append( command );
#endif    	
    	if ( tmp == 0 && iPatterSearchSequentialBlits == 0 )
    		{
    		// This is the beginning of the sequence. Save position (offset minus size of the current command) 
    		// the for later use.
    		iPatternCacheBeginPosition = iOffset;
#ifdef _OLD_STREAM    		
    		iPatternCacheBeginPosition = TStreamPos( iPatternCacheBeginPosition - sizeof(TUint8) ); // the command
#else
    		iPatternCacheBeginPosition = iPatternCacheBeginPosition - sizeof(TUint8) ; // the command
#endif
    		iSearchPatternClipRegion.Clear();
#ifdef _ALF_PRINT_WS_COMMANDS_
    		iPatternCommands.Reset();
#endif    		
    		}
    	else if ( tmp == sizeOfPattern - 1)
    		{
    		// Start from the beginning the next sequential pattern
    		iPatternSearchState = TSearchPatternBitBlit[0][1]; 
    		iPatterSearchSequentialBlits++;
    		}
    	}
    else
    	{
    	TRAP_IGNORE(FinalizePatternL( aCommand ));
    	}
    }

// ---------------------------------------------------------------------------
// FinalizePatternL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::FinalizePatternL( const TUint8& aCommand )
    {
    // We are looking 8 or more connected chaff pieces 
    if ( iPatterSearchSequentialBlits >= 9 )
        {
        iSearchPatternClipRegion.Tidy(); // will return only one region, if pieces are connected 
        iSearchPatternBlitRect.Tidy();
        
        
        if ( iSearchPatternClipRegion.Count() == 1 && iSearchPatternBlitRect.Count() == 1)
            {
#ifdef _OLD_STREAM
            SeekL( iPatternCacheBeginPosition.Offset() );
#else
            SeekL( iPatternCacheBeginPosition );
#endif
            WriteInt8L( EAlfCombinedBitBlitMasked );
#ifdef _ALF_PRINT_WS_COMMANDS_
            TInt size = iPatterSearchSequentialBlits * sizeof(TBlitStruct) + 2 * sizeof(TRect);
            iCommandDebugger->SetDescriptionAndSize( EAlfCombinedBitBlitMasked, size, R_ALF_COMMAND_DESCRIPTION_ARRAY );
            iCommandDebugger->Print();
            while ( iPatternCommands.Count())
                {
                iCommandDebugger->AdjustCommand( iPatternCommands[0].iCommand,  (-1) * iPatternCommands[0].iSize );
                iPatternCommands.Remove(0);
                }
#endif
            // item count
            WriteInt8L( iPatterSearchSequentialBlits );
            // Clipping region for all the pieces. 
            // This contains the clippingregion (set by EAlfSetClippingRegion) and rect for all the pieces 
            // (from EAlfBitBlit, EAlfBitBltRect, EAlfBitBlitMasked )
            TRect clipRect = iSearchPatternClipRegion.BoundingRect();
            TRect clipBlitRect = iSearchPatternBlitRect.BoundingRect();

            WriteL( (TUint8*)&clipRect,     sizeof(TRect));
            WriteL( (TUint8*)&clipBlitRect, sizeof(TRect));
                        
            // Items
            while( iPatterSearchSequentialBlits-- )
                {
                TPoint point = iPatternHandleCache[iPatterSearchSequentialBlits].iTl;
                WriteL( (TUint8*)&iPatternHandleCache[iPatterSearchSequentialBlits], sizeof(TBlitStruct)  );
                }
#ifndef _OPTIMIZE_WS_COMMANDS_ADVANCED_
            WriteInt8L( EAlfSetPenStyle );
            WriteInt32L( iPenStyle );
#endif                        
            // Write again the current command (after the pattern sequence), because we just overwrote it
            WriteInt8L( aCommand );
            }
        }
    ResetPatternSearch();
    }

// ---------------------------------------------------------------------------
// ResetPatternSearch
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::ResetPatternSearch()
    {
    iPatternSearchState = ESeekSetClippingRegion;
    iPatterSearchSequentialBlits = 0;
    iSearchPatternClipRegion.Clear();
    iSearchPatternBlitRect.Clear();
    iPatternHandleCache.Reset();
#ifdef _ALF_PRINT_WS_COMMANDS_
    iPatternCommands.Reset();
#endif
    }

// ---------------------------------------------------------------------------
// AppendPatternSearchCache
// saves masked bitblits to the cache
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::AppendPatternSearchCache( const CFbsBitmap& aSourceBitmap, const CFbsBitmap* aMaskBitmap, const TRect& aSourceRect, const TPoint& aDestPos, TBool aInvertMask )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    TSize size1 = aSourceRect.Size();
    TSize size2 = aSourceBitmap.SizeInPixels();
    if ( aInvertMask == 1 )
        {
        TInt handle = aMaskBitmap ?  aMaskBitmap->Handle() : 0;
        iPatternHandleCache.Append( TBlitStruct( aSourceBitmap.Handle(),handle, aDestPos ) ); 
        TRect rect( TPoint(0,0), aSourceRect.Size() );
        rect.Move( aDestPos );
        iSearchPatternBlitRect.AddRect( rect );
        }
    else
        {
        ResetPatternSearch();
        }
    }    


// ---------------------------------------------------------------------------
// InsertPaddingL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::InsertPaddingL()
    {
    // padding is required for the 1st part of the package, because the 2nd package 
    // is attached straight to it and it MUST start at offset divisible by 4. Otherwise 
    // possible strings in the second package will not be correctly alligned.
    TInt offset = iOffset; 
    TInt padding = offset % KDivisibleByX;
    if ( padding > 0)
        {
        padding = KDivisibleByX - ( offset + sizeof(TUint8) * 2 ) % KDivisibleByX; // 2 = sizeof( EAlfPacketPadding + padding )
        WriteInt8L( EAlfPacketPadding );
		WriteInt8L( padding );
		while( padding--)
            {
            WriteInt8L( 0 );
			}
        if ( !iReceivingDrawingCommands )
            {
            WriteInt8L( EAlfCommandEndMarker );
			}
        }
#ifdef _DEBUG
    ASSERT( ( offset = iOffset ) % KDivisibleByX == 0);
#endif    
    }

// ---------------------------------------------------------------------------
// SetFlag
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::SetFlag( TAlfSendBufferFrameFlags aFlag )
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    iFlags |= aFlag;
    }

// ---------------------------------------------------------------------------
// EndFrameL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::EndFrameL()
    {
    if (iDisabled) // return if this send buffer is not in use
        {
        return;
        }
    
    TUint8 command(EAlfPacketReady);
    if ( iFlags & EAlfTransparentContent && !(iFlags & EAlfTransparentContentFlush ) )
        {
        InsertPaddingL();
        }

    if ( WriteFollowingFrameOffsetL() )
        {
        WriteCommandL( command );  
        }
#ifdef ALF_DEBUG_TRACK_DRAWING
    TInt trackThisNode = 0;
    if ( trackThisNode )
        {
        WriteIntsL( EAlfDebugTrackNode, 2, (TInt32)&iWindowId, 1 );
        }
#endif
    }

// ---------------------------------------------------------------------------
// SeekL
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::SeekL( const TInt aOffset )
    {
#ifdef _OLD_STREAM    
    iBufStream->Sink()->SeekL( MStreamBuf::EWrite, TStreamPos(aOffset));
#endif    
    iOffset = aOffset;
    }

// ---------------------------------------------------------------------------
// SetOrientation
// ---------------------------------------------------------------------------
//
void CAlfRsSendBuffer::SetOrientation(TInt aOrientation)
    {
    iOrientation = aOrientation;
    }
      
