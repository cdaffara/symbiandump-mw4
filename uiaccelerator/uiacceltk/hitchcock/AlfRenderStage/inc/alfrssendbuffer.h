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

#ifndef CALFSENDBUFFER_H_
#define CALFSENDBUFFER_H_

//#define RD_SUBWINDOW_EFFECTS 

#include <e32cmn.h>
#include <e32std.h> 
#include <s32mem.h>
#include <gdi.h>
#include <alfwindowstructs.h>
#include <huiwscanvascommands.h>
#include "alfrenderstageutils.h"

#define _OLD_STREAM

const TInt KAlfWsCommandCount = 90; 
const TInt KAlfWsMaxParameterCount = 15;
const TInt KAlfRsSearchRectMaxCount = 9;

class RAlfBridgerClient;
class CAlfCommandDebug;
class TRgb;
class CAlfRenderStage;
class TResourceReader;
class CAlfCompositionCntrlClient;
/**
 *  The class handles data serialization and syncronous sending to server. Multiple renderstages will share the same chunk.
 *
 *  @code
 *   iAlfSendBuffer = CAlfRsSendBuffer::NewL();
 *  @endcode
 *
 *  @lib alfrenderstage.lib
 *  @since S60 v5.2
 */
NONSHARABLE_CLASS(CAlfRsSendBuffer) : public CActive, public MAlfBridge
	{
    public: // New methods
      
	    /**
	     * NewL
	     * 
	     * Constructor
	     * 
	     * @param aParent   
	     * @param aScreenNumber   
	     * @return  New instance of the class
	     */
		static CAlfRsSendBuffer* NewL( CAlfRenderStage& aParent, TInt aScreenNumber );

	    /**
	     * C++ destructor.
	     */
		virtual ~CAlfRsSendBuffer();

		/**
	     * ConnectL
	     *
	     * Establish connection to AlfStreamerServer.
	     *
	     * @return ETrue, if connection was made or connection was already established. Otherwise returns EFalse
	     */
		TBool ConnectL();

        /**
         * Connected
         *
         * Check connection state
         *
         * @return Connection state
         */
		TBool Connected(){ return iChunk.Handle(); }
		
        /**
         * WriteIntsL
         * 
         * Convenience function for writing TInt values to the stream
         *
         * @param aCommand  CommandId
         * @param aCount    Amount of TInt values to be streamed
         * @param aFirst    The first value
         * @param ...       variable aCount-1 amount of TInt values
         */
	    void WriteIntsL( const TUint8& aCommand, TInt aCount, TRefByValue<const TInt> aFirst, ...);

	    /**
         * WriteIntL
         * 
         * Convenience function for writing possible unsupported command (depending on its parameters) to the stream 
         *
         * @param aCommand   CommandId
         * @param aValue    parameters value
         */
	    void WriteIntL( const TUint8& aCommand, TInt aValue );
	    
        /**
         * WriteRegionL
         * 
         * Convenience function for writing region to the stream 
         *
         * @param aCommand   CommandId
         * @param aRegion    Region
         */
	    void WriteRegionL( const TUint8& aCommand, const TRegion& aRegion );
	    
	    /**
	     * Writes ints followed by region.
         * 
         * Convenience function for writing region to the stream 
         *
         * @param aCommand   CommandId
         * @param aRegion    Region
         * @param aCount    Amount of TInt values to be streamed
         * @param aFirst    The first value
         * @param ...       variable aCount-1 amount of TInt values
	     */
	    void WriteRegionIntsL( const TUint8& aCommand, const TRegion& aRegion, TInt aCount, TRefByValue<const TInt> aFirst, ...);
	    
	    /**
         * WriteDescriptorAndIntsL
         * 
         * Convenience function for writing TDesC reference and TInt values to the stream.
         * Inserts padding to ensure that text starts on divisible by 4 offset.
         *
         * @param aCommand  CommandId
         * @param aText     Descriptor to be streamed
         * @param aCount    Amount of TInt values to be streamed
         * @param aFirst    The first value
         * @param ...       variable aCount-1 amount of TInt values
         */
        void WriteDescriptorAndIntsL( const TUint8& aCommand, const TDesC& aText, TInt aCount, TRefByValue<const TInt> aFirst,... );

        /**
         * WriteDescriptorL
         * 
         * Convenience function for writing TDesC reference to the stream. 
         * Inserts padding to ensure that text starts on divisible by 4 offset.
         *
         * @param aCommand  CommandId
         * @param aText  Descriptor to be streamed
         * @param aTextParameter    Text parameters
         * @param aCount Amount of Ints to be streamed
         * @param aFirst    The first value
         * @param ...       variable aCount-1 amount of TInt values
         */
        void WriteDescriptorAndIntsL(const TUint8& aCommand, const TDesC& aText, const CGraphicsContext::TTextParameters* aTextParameter, TInt aCount, TRefByValue<const TInt> aFirst, ... );

	    /**
         * WriteCommandL
         * 
         * Convenience function for writing 1 value to the stream
         *
         * @param aCommand  Command
         * @param aSize  Size
         */
        void WriteCommandL( const TUint8& aCommand, TInt aSize = 0);

        /**
         * WritePointArrayL
         * 
         * Convenience function for writing pointer array to the stream
         *
         * @param aCommand  CommandId
         * @param aPoWriteIntList  Point array to be streamed
         */
        void WritePointArrayL( const TUint8& aCommand, const CArrayFix<TPoint>* aPoWriteIntList );

        /**
         * WritePointArrayL
         * 
         * Convenience function for writing pointer array to the stream
         *
         * @param aCommand  CommandId
         * @param aPoWriteIntList  Point array to be streamed
         * @param aNumPoints  Point array to be streamed
         */
        void WritePointArrayL( const TUint8& aCommand, const TPoint* aPoWriteIntList, TInt aNumPoints );

        /**
         * WritePointArrayL
         * 
         * Convenience function for writing pointer array to the stream
         *
         * @param aCommand  CommandId
         * @param aPoWriteIntList  Point array to be streamed
         */
        void WritePointArrayL( const TUint8& aCommand, const TArray<TPoint>* aPoWriteIntList );

        /**
         * SetOrientation
         * 
         * Set orientation. All drawing buffers will be tagged with the current orientation.
         *
         * @param aOrientatation 	New orientation
         */        
        void SetOrientation(TInt aOrientation);
        
	//
	// ARRAY IMPLEMENTATION
	//	
	struct TOffsetElem
	{
	TInt32 iOffset; // offset from beginning of buffer
	TInt32 iSize;   // size of the data.
	TInt32 iLayerId;
    TRect iBoundingRectangle;
    };

#ifdef RD_SUBWINDOW_EFFECTS
	void WriteArrayHeaderTemplateL();
	void InitTOffsetElemArray(RArray<TOffsetElem> &aOffset);
	void WriteArrayHeaderL(const TOffsetElem &aIndexArrayOffset);
	TOffsetElem WriteIndexArrayL(const RArray<TOffsetElem> &aOffset);
	void InitMarker(TInt &aMarker);
	void StartMarkerL(TInt &aMarker, TRect &aRectangle, TInt &aLayer, TRect aBoundingRectangle, TInt aLayerId);
	void EndMarkerL(RArray<TOffsetElem> &aOffset, TInt &aMarker, const TRect &aRectangle, TInt aLayerId);
    void StartMarkerL(TRect aBoundingRectangle, TInt aLayerId);
    void EndMarkerL();
#endif // RD_SUBWINDOW_EFFECTS
	
       /**
        * SendL
        *
        * Sends commands to via AlfBridger to AlfStreamerServer. Flushes the buffer.
        * iBufStream is ready for writing after this call.
        *
        * @param aStatus  Request status
        */
        void SendL( TRequestStatus* aStatus );

        /**
         * Commit
         * 
         * Report the last written offset. Reader side may read until
		 * offset.
         * 
         */
        void Commit();

        /**
         * CommitL
         * 
         * Starts the timer for FlushBuffer call. The idea is to deliver bigger 
         * patch of commands to alf in one go and avoid unnecessary IPC calls.
         * 
         */
        void CommitL( );

        /**
         * SetFlag
         * 
         * Set flags
         * 
         * @param aFlag  Flags
         */
        void SetFlag( TAlfSendBufferFrameFlags aFlag );

        /**
         * EndFrameL
         * 
         * Prepares frame for sending
         */
        void EndFrameL();

        /**
         * FrameContainsDataL
         * 
         * @return  ETrue, if data has been serialized to the frame after previous call to PrepareBufferL. this is used for filtering out empty
         *          frames. Wserv might be fixed in the future and this could become obsolete.
         */
        TBool FrameContainsDataL();

        /**
         * SendAsyncCmd
         * 
         * Called by CRsAlfNativeWindowListener::RequestEvent. Used when requesting native window handle from 
         * Alf.
         * 
         * @param aFlag  aCmd
         * @param aFlag  aBuf
         * @param aFlag  aStatus
         */
		void SendAsyncCmd(TInt aCmd, TDes8& aBuf, TRequestStatus& aStatus);

        /**
         * OpenPrimaryChunkForWritingL
         * 
         * Open the stream 
         */
		void OpenPrimaryChunkForWritingL();
		
        /**
         * PrepareBufferL
         * 
         * Resets buffer. Must be called before Write*L and SendSyncL.
         */
        void PrepareBufferL();

		/**
         * WriteWindowDataL
         *
         * Writes data to the stream
         *
         * @param aWindowUid
         * @param aRegionSize
         * @param aShapeRegionSize
         */
        void WriteWindowDataL(TUint32 aWindowUid, TInt aRegionSize, TInt aShapeRegionSize );

        /**
         * WriteFrameSizeTemplateL
         * 
         * Writes a temporary value to the beginning of a frame. This value is later updated with WriteNextFrameBeginL.
         */
        void WriteFollowingFrameOffsetTemplateL();
        
        /**
         * WriteFollowingFrameOffsetL
         * 
         * Goes back to header of the drawing commands and updates the offset to the following frame.
         * This is called, when drawing into a single window in a frame is complete.
         * 
         * @return  ETrue, If this frame contains any commands. Otherwise EFalse.
         */
        TBool WriteFollowingFrameOffsetL(TBool aWriteArray= ETrue);
	 	
        /**
		 * WriteWindowIdentifierL
		 *
		 * Convience function for writing window id and group id to the stream
		 */
		void WriteWindowIdentifierL();

		/**
		 * FlushBuffer
		 * 
		 * Commands are not passed immediately after they are serialized. There
		 * is a timer, which regularly flushes the buffer. See doFlushBuffer. 
		 *
		 * Buffer is also flushed (or in fact the flushtimer is canceled, and buffer
		 * is flushed) when SendL is called from CAlfRsRenderStage
		 * 
		 */
		void FlushBuffer();

        /**
         * SetSupportedCommand
         * 
         * Sets this command as supported
         * 
         * @param aIndex
         * @param aSupport
         */
		void SetSupportedCommand( TInt aIndex, TInt8 aSupport );
  
		/**
		 * AppendPatternSearchCache
		 * 
		 * Caches data for the pattern search. This is call from CAlfRsGc. Cache is released by
		 * ResetPatternSearch.
		 * 
         * @param aSourceBitmap
         * @param aMaskBitmap
         * @param aSourceRect
         * @param aDestPos
         * @param aInvertMask
		 */
	    void AppendPatternSearchCache( const CFbsBitmap& aSourceBitmap, const CFbsBitmap* aMaskBitmap,const TRect& aSourceRect, const TPoint& aDestPos, TBool aInvertMask );

        /**
         * PreviousCommand
         * 
         * This is used for filtering out sequential reset commands in window command stream
         * 
         * @return Previous command
         */
        inline TInt PreviousCommand(){ return iPreviousCommand; } 
      
        /**
         * Synchronize.
         * @param aId id to pass forward.
         */
        void Synchronize(TInt aId);
        
        void SetCompositionModified()
            {
            iCompositionModified = ETrue;    
            }
        
    public: // From CActive

	    void RunL();
	    void DoCancel();
        TInt RunError(TInt aError)
            {
            // will likely lead to inconsistent state and thus reset anyway
            RDebug::Print(_L("CAlfRsSendBuffer::RunError( %d )"),aError);
            aError -= (aError);
            return aError; //KErrNone;
            }
            
	private:
	    
       /**
        * CAlfRsSendBuffer
        * 
        * C++ constructor
        * 
        * @param aParent
        * @param aScreenNumber
        */
	    CAlfRsSendBuffer( CAlfRenderStage& aParent, TInt aScreenNumber ) : iParent( aParent), iScreenNumber( aScreenNumber) , CActive( EPriorityStandard ){CActiveScheduler::Add( this ); };

       /**
        * ConstructL
        * 
        * Constructor
        */
        void ConstructL();

        /**
         * ReadNonSupportedCommandsL
         * 
         * It is known that some commands are not/cannot be supported by certain
         * renderers. Each frame is inspected in case of such command and frame 
         * is marked if it has unsupported commands for certain renderer. This
         * information can be in the future used for selecting suitable renderer
         * during runtime.
         * 
         * @todo This table could be shared with multiple renderstages. 
         *       Currently each sendbuffer has own table.
         */
        void ReadNonSupportedCommandsL();
        
        // only in debug mode
#ifdef _ALF_PRINT_WS_COMMANDS_
        void ReadCommandDescriptionsL();
#endif

		/**
		 * SeekL
		 * 
		 */
	    void SeekL( const TInt aOffset );
	    
	    /**
	     * ConvertToBitsL
	     * 
	     * Converts aCount bytes of data into one byte.
	     * 
	     * @param  aCount  Must be under 7.
	     * @param  aReader
	     * @return Converted result
	     */
	    TUint8 ConvertToBitsL( TInt aCount, TResourceReader* aReader );
           
        /**
         * JumpToAnotherChunkL
         */
	    void JumpToAnotherChunkL( TInt32 aChunkId, TInt aChunkSize );

        /**
         * WriteInt8L
         */
	    inline void WriteInt8L( const TInt8& aValue )
	        {
#ifdef _OLD_STREAM
	        iBufStream->WriteInt8L( aValue );
#else
	        //memcpy( ( TUint8*)(iStreamPtr->Ptr()+iOffset), (const TUint8*)&aValue, sizeof(TInt8));
	        *(( TUint8*)(iStreamPtr->Ptr()+iOffset)) = aValue;
#endif
            iOffset += sizeof(TInt8);
            }
		
        /**
         * WriteInt32L
         */
	    inline void WriteInt32L( const TInt32& aValue )
	        {
#ifdef _OLD_STREAM
	        iBufStream->WriteInt32L( aValue );
#else
	        memcpy( ( TUint8*)(iStreamPtr->Ptr()+iOffset), (const TUint8*)&aValue, sizeof(TInt32));
#endif
            iOffset += sizeof(TInt32);
	        }
	    
        /**
         * WriteL
         */
	    inline void WriteL( const TUint8* aPtr, TInt aSize )
	        {
#ifdef _OLD_STREAM	        
	        iBufStream->WriteL( aPtr, aSize );
#else
	        memcpy( ( TUint8*)(iStreamPtr->Ptr()+iOffset), (const TUint8*)aPtr, aSize);
#endif
	        iOffset += aSize;
	        }

        /**
         * DoCreateTemporaryChunkL
         */
		void DoCreateTemporaryChunkL( TInt aMinimumRequiredSize );
	    
	    /**
	     * DoWrapL
	     * 
	     * Wraps the writing in the chunk. This happens when write head has reached the maximum size of the chunk.
	     * After wrapping, writing continues from the beginning of the chunk. If wrap happens during a frame, then 
	     * the frame is split into two parts. The first part is padded (@see InsertPaddingL) 
	     * 
	     */
	    void DoWrapL( TInt aCommandSize, TBool aCreateTempororaryChunk );
	    
	    /**
	     * ReserveSpaceL
	     * 
	     * Checks if there is enough space to stream the command. Calls DoWrap 
	     * or DoWait, if there is not enough space for serializing the command. 
	     * 
	     * @param  aCommandSize
	     */
	    TBool ReserveSpaceL( TInt aCommandSize );
	    
        /**
         * InitCommandL
         */
	    TBool InitCommandL( const TUint8& aCommand, TInt aSize );

        /**
         * CreateTemporaryChunkL
         */
	    TBool CreateTemporaryChunkL( TInt& aSize );
	    
        /**
         * OpenRewindChunkL
         */
	    void OpenRewindChunkL( TInt aSize );
	    
	    /**
	     * DoPatternSearch
	     * 
	     * Searches for certain bitblit pattern.
	     * 
	     * This functions is most likely to change in the future.
	     * 
	     */
	    void DoPatternSearch( const TUint8& aCommand, TInt aSize );
	    
	    /**
	     * ResetPatternSearch
	     * 
	     * Called when searched pattern was not found and search needs to start
	     * from beginning.
	     */
	    void ResetPatternSearch();
	  
	    /**
	     * FinalizePatternL
	     * 
	     * Full pattern was found. Replace the bitblits with EAlfCombinedBitBlit
	     * that joins the clipping regions, drawrects and partial images into single 
	     * command.
	     */
	    void FinalizePatternL(const TUint8& aCommand);
	    
	    /**
	     * InsertPaddingL
	     * 
	     * Each frame payload need to start on address divisible by 4.
	     * Otherwise operations on the streamed data will fail due to alignment
	     * issues. This function adds padding if necessary.
	     */
	    void InsertPaddingL();

    public:	    
	    RAlfBridgerClient* Client()
	        {
            return iAlfBridgerClient;
            }
        void WriteIntsL(TUint8 aCommand, TInt aCount, TInt* aArray);
	        
	private:    // data
	    
        /**
         * If this is true this class acts as a stub, the does not really do anything.
         * That is used when we handle tv-out drawing commands as they should not go to anywhere. 
         */
	    TBool iDisabled;
	    
        /**
    	 * Window Uid is cached for the use of FlushBuffer
    	 */
    	TUint32 iWindowId;

        TInt iWindowGroupIdentifier;

        /**
         * Stream access for chunk received from server side
         */
#ifdef _OLD_STREAM
        RMemWriteStream* iBufStream;
#else        
        TPtr8* iStreamPtr;
#endif
        
        TInt iNextFrameOffsetPos;
        
        TBool iIsSprite;
        
        /**
    	 *	Bridge to Alf stream server
    	 *
    	 * 	Own.
    	 */
    	RAlfBridgerClient* iAlfBridgerClient;

        /**
         * Our handle to server chunk. InitBufferL must be called before this is valid.
         */
        RChunk iChunk;

        /**
         * Cache chunks are needed for 2 purposes. See DoCreateTemporaryChunkL for more information.
         * 1. During booting, the primary chunk (owned by alf) is not available and data must be serialized into something
         * 2. App (and sequentially Wserv) pushes more data than the primary chunk can chew (write head reachs read head).
         */  
        RArray<RChunk> iCacheChunks;
        
        /**
    	 * Non drawing window updates have no relevance to the drawing. If frame update contains only such
    	 * commands, it can be discarded altogether.
    	 *
    	 * Use Mark() to mark the place of frame start. Use RollBack() to set the buffer to the original place,
    	 * if only non-drawing commands were issued. See CAlfGraphicsContext::iDrawCommandCount for more details.
    	 */
#ifdef _OLD_STREAM
        TStreamPos iFrameBeginOffset;
#else
        TInt iFrameBeginOffset;
#endif
        /**
         * Data transfer is normally asynchronous operation. iParent is used for the callback.
         * Not own.
         */
        CAlfRenderStage& iParent;

        /**
         * The maximum size of the iChunk and iBufStream
         */
        TInt iUsedChunkMaxSize;
        
        TInt iPrimaryChunkMaxSize;
        
        TTime   iBeginTime;
        
        TInt iOffset;
        
        /**
         * iState values
         */
        enum
            {
            EIdle,
            EWaitingAck,
            EWaitingHandle
            };

        TInt iState;
        
        TInt iTemplateOpen;

        TRequestStatus* iQuedStatus;

        TChunkHeader* iChunkHeader;
        
        TBool iReceivingDrawingCommands;

        /**
         * Debug variable for tracking the delivered chunk sizes. Can be used for optimizing the junk size.
         */
        //TInt iMaxSentBatchSize;
        
        TBool iWrappingFrame;
        
        /**
         * Screen connected to this CAlfRsSendBuffer
         */
        TInt iScreenNumber; 

        CPeriodic* iFlushBufferTimer;
        
        TUint8 iNonSupportedCommandsInWindow;
        
        /**
         * See ReadNonSupportedCommandsL for more information
         */
        TUint8 iNonSupportedCommands[KAlfWsCommandCount];

        TUint8 iSupportedCommandParameters[KAlfWsCommandCount][2]; // [index of first parameter in iSupportedParameters][index of last parameter in iSupportedParameters] 
        TInt8 iSupportedParameters[KAlfWsMaxParameterCount][2]; // [parameter value][render support], note the limited count
        
        TInt iPerformanceIssueCommandCount;
        
        TInt iMaxPerformanceIssueCommandCount;
        
        struct TWindowHeaderStruct
            {
            TInt32 iWindowEndOffset;
            TUint8 iCommandEndMarker;
            TUint8 iContainsUnsupportedCommandsCommand;
            TUint8 iUnsupportedCommandsInWindow;
            };
                
        TWindowHeaderStruct iWindowHeaderStruct;
        
        TUint8 iFramePadding;
        
        TInt32 iFlags;

        TUint8 iPreviousCommand;
        
        /**
         * AlfRsSendBuffers searches certain bitblit pattern in the serialized 
         * commands. It tries to combine joined bitblits into one command, which 
         * can be handled more efficiently on HW.
         * 
         * See DoPatternSearch, AppendPatternSearchCache, ResetPatternSearch and FinalizePatternL for more 
         * information.
         * 
         */ 
        TInt iPatternSearchState;
        
        TInt iPatterSearchSequentialBlits;
        
        TInt iPenStyle;
        
        /**
         * Under normal circumtances, a single chunk is used for passing the 
         * commands to alf. However, if Wserv pushes large frames that fill the
         * whole chunk, alfsendbuffer takes a growing cache chunk into use.
         * 
         * The cache chunk is destroyed, when it is no longer needed.
         */
       
        TUint8 iChunkInUse;
        
        /**
         * Joined clipping region is usually different than the following joined blit region (iSearchPatternBlitRect)
         */
        RRegionBuf<KAlfRsSearchRectMaxCount> iSearchPatternClipRegion;
        
        RRegionBuf<KAlfRsSearchRectMaxCount> iSearchPatternBlitRect;
        
        class TBlitStruct
            {
        
        public:
        
        	TBlitStruct( TInt aHandle, TInt aMaskHandle, TPoint aTl ) : iHandle( aHandle ), iMaskHandle( aMaskHandle ), iTl( aTl ){};
            TInt iHandle;
            TInt iMaskHandle;
            TPoint iTl;
            };
        
        RArray<TBlitStruct> iPatternHandleCache;
#ifdef _OLD_STREAM        
        TStreamPos iPatternCacheBeginPosition;
#else
        TInt iPatternCacheBeginPosition;
#endif
        TBool iBooting;
        
#ifdef _ALF_PRINT_WS_COMMANDS_

        /**
         * debug for printing the serializaed commands in human understandable format
         */
        TBool iLog;
        
        CAlfCommandDebug* iCommandDebugger;
        
        class TPatternCommand
            {
        public:
        
            TPatternCommand( TUint8 aCommand, TInt aSize ): iCommand( aCommand ), iSize(aSize){};
            
            TUint8 iCommand;
            
            TInt iSize;
            };
			
        RArray<TPatternCommand> iPatternCommands;
                
#endif  

        // caches text. If same text is used 2nd time inside buffer
        HBufC16* iCachedText;
        CAlfCompositionCntrlClient* iAlfCompositionCntrlClient;
        
        // array implementation
#ifdef RD_SUBWINDOW_EFFECTS		
        RArray<TOffsetElem> iOffsetArray;
        TInt iMarker;
        TRect iBoundingRectangle;
        TInt iLayerId;
        TInt iArrayImplOffset;
        TInt iPreviousBlockOffset;
        TInt iExtraLayerId;
        TBool iLayersEnabled;
#endif        
        TInt iOrientation;
        TBool iCompositionModified;
	};
#endif /*CALFSENDBUFFER_H_*/
