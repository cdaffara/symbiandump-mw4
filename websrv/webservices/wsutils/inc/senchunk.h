/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header declaration
*
*/











#ifndef SEN_CHUNK_H
#define SEN_CHUNK_H

// INCLUDES
#include <e32base.h>
#include <flogger.h>
#include <f32file.h>

#include <MSenMessage.h>
#include <MSenProperties.h>

// Min size for new RChunk allocations:
const TInt KSenMinRChunkSize = 0x2750;   // 10064 bytes
// Max size for new RChunk allocations:
const TInt KSenMaxRChunkSize = 0x400000; // 4194304 bytes 


// FORWARD DECLARE
class CSenChunk;

// CLASS DECLARATION
class TChunkHeader
  /**
  * This class is set in the beginning of the RChunk. Class is used
  * to store header information.
  *
  * Note: All data in here must be 4-byte aligned (using dummy values
  * if needed)
  */
  {
  public:
    /**
    * Getter for message class enumeration
    * @return message type enumeration
    * @see MSenMessage.h
    */
    inline MSenMessage::TClass MessageType() const
        {
        return (MSenMessage::TClass)iMessageType;
        }

    /**
    * Setter for message class enumeration
    * @param aValue defines the message class type enumeration.
    * @see MSenMessage.h
    */
    inline void SetMessageType(MSenMessage::TClass aType)
        {
        iMessageType = (TInt)aType;
        }

    /**
    * Getter for message direction enumeration
    * @return message type enumeration
    * @see MSenMessage.h
    */
    inline MSenMessage::TDirection MessageDirection() const
        {
        return (MSenMessage::TDirection)iMessageDirection;
        }

    /**
    * Setter for message direction enumeration
    * @param aValue defines the message class type enumeration.
    * @see MSenMessage.h
    */
    inline void SetMessageDirection(MSenMessage::TDirection aDirection)
        {
        iMessageDirection = (TInt)aDirection;
        }

    /**
    * Getter to determine should init be done before sending the message.
    * @return ETrue if yes, otherwise EFalse.
    */
    inline TBool DoInitBeforeSending() const
        {
        if ( iDoInitBeforeSending )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }            
        }

    /**
    * Sets the flag indicating that init should be done
    * before sending the message
    * @param aValue if ETrue, init will be done before sending
     *              if EFalse, init will not be done before sending
    */
    inline void SetDoInitBeforeSending(TBool aValue)
        {
        if ( aValue )
            {
            iDoInitBeforeSending = 1;
            }
        else
            {
            iDoInitBeforeSending = 0;
            }
//        iDoInitBeforeSending = aValue;
        }
        
    /**
    * Getter for (transport) properties type.
    * @return transport properties type
    * enumeration. Note:
    *   ENotInUse means that transport
    *   properties are not in use.
    * @see MSenProperties
    */
    inline MSenProperties::TSenPropertiesClassType PropertiesType() const
        {
        return (MSenProperties::TSenPropertiesClassType)iPropertiesType;
        }

    /**
    * Sets the (transport) properties type enumeration
    * @param aType where ENotInUse indicates that
    * there are no associated properties, and all
    * the others point to a concrete properties 
    * class.
    * @see MSenProperties
    */
    inline void SetPropertiesType(MSenProperties::TSenPropertiesClassType aType)
        {
        iPropertiesType = (TInt)aType;
        }

    /**
    * Getter for descriptor count.
    * @return the descriptor count
    */
    inline TInt DescriptorCount() const
        {
        return iDescriptorCount;
        }

    /**
    * Setter for descriptor count.
    * @param aCount is the descriptor count
    */
    inline void SetDescriptorCount(TInt aCount)
        {
        iDescriptorCount = aCount;
        }

    /**
    * Getter for transaction id.
    * @return the transaction id
    */
    inline TInt TransactionId() const
        {
        return iTransactionId;
        }

    /**
    * Setter for transaction id.
    * @param aID is the transaction id
    */
    inline void SetTransactionId(TInt aId)
        {
        iTransactionId = aId;
        }

    /**
    * Getter for ctx id.
    * @return the ctx id
    */
    inline TInt ContextId() const
        {
        return iContextId;
        }

    /**
    * Setter for ctx id.
    * @param aID is the ctx id
    */
    inline void SetContextId(TInt aId)
        {
        iContextId = aId;
        }

    /**
    * Getter for pending transaction id.
    * @return the pending transaction id
    */
    inline TInt PendingTransactionId() const
        {
        return iPendingTransactionId;
        }

    /**
    * Setter for pending transaction id.
    * @param aCount is the pending transaction id
    */
    inline void SetPendingTransactionId(TInt aId)
        {
        iPendingTransactionId = aId;
        }

  private:
        /*MSenMessage::TClass*/                     TInt iMessageType;
        /*MSenMessage::TDirection*/                 TInt iMessageDirection;
        /*TBool*/                                   TInt iDoInitBeforeSending;
        /*MSenProperties::TSenPropertiesClassType*/ TInt iPropertiesType; 
        TInt                                             iDescriptorCount;
        TInt                                             iTransactionId;
        TInt                                             iPendingTransactionId;
        TInt                                             iContextId;
  };

/**
* Offset to the beginning of header
*/
const TInt KHeaderOffset    = 0;

/**
* Offset to the beginning of data
*/
const TInt KDataOffset      = KHeaderOffset + sizeof(TChunkHeader);

// CLASS DECLARATION
class CSenChunk : public CBase
    {
  /**
  * Helper class for storing RChunks
  */
    public:
        /**
        * Two-phased constructor.
        * @since Series60 4.0
        * @param aChunkName is the chunk name.
        * @return a new CSenChunk
        */
        IMPORT_C static CSenChunk* NewL(const TDesC& aChunkName);
        
        /**
        * Two-phased constructor.
        * @since Series60 4.0
        * @param aChunkName is the chunk name.
        * @return a new CSenChunk
        */        
        IMPORT_C static CSenChunk* NewLC(const TDesC& aChunkName);
        
        /**
        * Destructor.
        */
        IMPORT_C ~CSenChunk();

      /**
    * @since Series60 4.0
        * @return   a reference to internal chunk
      */
      IMPORT_C RChunk& Chunk();
      
      /**
    * @since Series60 4.0
        * @return   a pointer to ChunkName
      */
        IMPORT_C TPtrC ChunkName() const;

        /**
        * Creates global protected RChunk inside CSenChunk
        * @since Series60 4.0
        * @return   KErrNone or some system-wide Symbian error codes.
        */
        IMPORT_C TInt CreateChunk();
        
        /**
        * Opens global protected RChunk inside CSenChunk
        * @since Series60 4.0
        * @return   KErrNone or some system-wide Symbian error codes.
        */
        IMPORT_C TInt OpenChunk();
        
        /**
        * Closes RChunk inside CSenChunk
        * @since Series60 4.0
        */
        IMPORT_C void CloseChunk();

        /**
        * Changes the number of bytes committed to the internal chunk.
        * This value is always rounded up to the next nearest processor
        * page boundary.
        * This value is rounded up to KSenMinRChunkSize if value is
        * less than KSenMinRChunkSize.
        * @since Series60 4.0
        * @param    aNewSize The number of bytes to be committed to this chunk.
        * @return   KErrNone or some system-wide Symbian error codes.
        */
        IMPORT_C TInt AdjustChunk(TInt aNewSize);

      /**
    * @since Series60 4.0
        * @return   a header of the internal chunk
      */
        IMPORT_C TChunkHeader& ChunkHeader();

        /**
        * Stores given descriptor into internal chunk.
        * Internal chunk is tried to adjust so that given descriptor
        * fits into chunk.
        * @since Series60 4.0
        * @param aDesc is the descriptor to store        
        * @return   KErrNone or some system-wide Symbian error codes.
        */
        IMPORT_C TInt DescToChunk(const TDesC8& aDesc);

        /**
        * Stores given descriptor into internal chunk.
        * Internal chunk is tried to adjust so that given descriptor
        * fits into chunk.
        * @since Series60 4.0
        * @param aDesc is the descriptor to store        
        * @return   KErrNone or some system-wide Symbian error codes.
        */
        IMPORT_C TInt DescToChunk(const TDesC8& aDesc, TInt aIndex);
        
        /**
        * Changes the number of bytes committed to the internal chunk
        * and places a pointer to chunk to apAllocated.
        * Size is always rounded up to the next nearest processor
        * page boundary.
        * Size is rounded up to KSenMinRChunkSize if given size is
        * less than KSenMinRChunkSize.
        * @since Series60 4.0
        * @param    aSize The number of bytes to be committed to this chunk.
        * @param    apAllocated 
        * @return   KErrNone or some system-wide Symbian error codes.
        */
        IMPORT_C TInt AllocDescToChunk(TInt aSize, TPtr8*& apAllocated);
        
        /**
        * Stores given descriptors into internal chunk.
        * Internal chunk is tried to adjust so that both of the 
        * given descriptors fit into chunk.
        * @since Series60 4.0
        * @param aDesc is the first descriptor to store        
        * @param aDesc2 is the second descriptor to store        
        * @return   KErrNone or some system-wide Symbian error codes.
        */
        IMPORT_C TInt DescsToChunk(const TDesC8& aDesc,
                             const TDesC8& aDesc2);
                             
        /**
        * Gets stored descriptor in given index from internal chunk.
        * @since Series60 4.0
        * @param aDesc will hold the pointer to descriptor in internal chunk        
        * @param aIndex is the index of descriptor to get
        * @return   KErrNone or some system-wide Symbian error codes.
        */
        IMPORT_C TInt DescFromChunk(TPtrC8& aDesc, TInt aIndex = 0);
        
        /**
        * Gets stored descriptors from internal chunk.
        * @since Series60 4.0
        * @param aDesc1 will hold the pointer to the first descriptor in internal chunk        
        * @param aDesc2 will hold the pointer to the second descriptor in internal chunk        
        * @return   KErrNone or some system-wide Symbian error codes.
        */
        IMPORT_C TInt DescsFromChunk(TPtrC8& aDesc1, TPtrC8& aDesc2);

        /**
        * Sets an internal chunk into TIpcArgs into given po.
        * @since Series60 4.0
        * @param aArgs  The IPC message arguments
        * @param aIndex An index value that identifies the slot in the array 
        *               of arguments into which the chunk is to be 
        *               placed. This must be a value in the range 0 to 3. 
        */
        IMPORT_C void ChunkToArgs(TIpcArgs& aArgs, TInt aIndex);
        
        /**
        * Opens internal chunk using a handle number sent by a client to a server.
        * This function is called by the server.
        * @since Series60 4.0
        * @param aMessage Represents the contents of the client message
        * @param aIndex An index specifying which of the four message arguments 
        *               contains the handle number. 
        * @return   KErrNone or some system-wide Symbian error codes.
        */
        IMPORT_C TInt OpenChunkFromRMsgL(const RMessage2& aMessage, TInt aIndex);
        
        /**
        * Opens internal chunk using a handle number returned by a server to a client.
        * This function is called by the client.
        * @since Series60 4.0
        * @param aHandleOrError A handle-number, if zero or positive;
        *                       an error value, if negative.
        * @return   KErrNone, if aHandle is a handle-number;
        *           the value of aHandleOrError, otherwise..
        */
        IMPORT_C TInt OpenChunkFromHandleNumberL(TInt aHandleOrError);
        
        /**
        * Tests whether two CSenChunks are equal.
        * @since Series60 4.0
        * @param aChunk The SenChunk to be compared with this SenChunk
        * @return   True if the two SenChunks are equal. False if not.
        */
        IMPORT_C TBool operator==(const CSenChunk& aChunk);
        
        /**
        * Sets the file logger used for debug messages.
        * @since Series60 4.0
        * @param aLog is the file logger
        */
        IMPORT_C void SetLogger(RFileLogger* aLog);
        
        /**
        * Gets the Rfile Pointer to CHUNK to send content by HTTP:POST
        * @since Series60
        */
        IMPORT_C RFile& RequestFileHandle();
        
        /**
        * Sets the Rfile Pointer to CHUNK to send content by HTTP:POST
        * @since Series60
        */
        IMPORT_C void RequestFileHandle(RFile& aFile);        

    protected: // Helper functions
        /**
        * @since Series60 4.0
        * @return   Pointer to the file logger
        */
        IMPORT_C RFileLogger* Log() const;

    /**
     *  Constructor.
     */
     IMPORT_C CSenChunk();

    private:
        /**
        * This private helper ensures that cast address is dividable by four (MANDATORY in ARMv5 environment, for example)
        */
        TUint8* RoundToNextDividableByFourAddress(TUint8* aValue);


        /**
        * Basic ConstructL function
        * @since Series60 4.0
            * @param aChunkName is the chunk name.
        */
        void ConstructL(const TDesC& aChunkName);
        

    protected: // Data
        HBufC*          ipChunkName;

    private: // Data
        RChunk          iChunk;
        RFileLogger*    iLog;
        RFile           iFile;
    };

#endif //R_SEN_SERVICE_CONNECTION_H
