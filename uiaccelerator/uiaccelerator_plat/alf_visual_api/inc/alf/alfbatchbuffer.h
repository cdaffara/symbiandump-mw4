/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   API to control the command batch buffer
*
*/




#ifndef C_ALFBATCHBUFFER_H
#define C_ALFBATCHBUFFER_H
 
#include <e32base.h>

class CAlfEnv;

/**
 * Auto flush modes
 * @see SetAutoFlushMode, AutoFlushMode
 */
enum TAlfAutoFlushMode
    {
    // Batch buffer is flushed automatically by scheduler = default
    EAlfAutoFlushDeferred = 0, 
    
    // Every command is flushed instantly i.e. no batching
    EAlfAutoFlushInstant, 
    
    // Batch buffer is used but only flushed with 
    // #1 manual call to FlushBatchBuffer
    // #2 max buffer limit is reached
    // #3 OOM when expanding the batch buffer
    EAlfAutoFlushOnlyForced 
    };
 
/**
 * Size modes
 * @see SetMaxBatchBufferSize
 */
const TInt KAlfBatchBufferSizeLimitToSystemDefault = 0;
const TInt KAlfBatchBufferSizeNoLimit = -1;


/**
 * Info types
 * @see GetBatchBufferInfo
 */
enum TAlfBatchBufferInfoType
    {
    // Default buffer size which is used if not overridden.
    // 100 <= DefaultBufferSize <= 10000 depending on the product.
    EAlfBatchBufferInfoDefaultBufferSize,
    
    // User defined max size. See SetMaxBatchBufferSize for the values.
    // If user defined: MaxSize >= DefaultBufferSize
    EAlfBatchBufferInfoMaxSize, 
     
    // Currently allocated buffer size.
    // If not limited: DefaultBufferSize <= CurrentlyAllocated
    // If limited: DefaultBufferSize <= CurrentlyAllocated <= MaxSize
    EAlfBatchBufferInfoCurrentlyAllocated,
    
    // Size which the commands use from the buffer. 
    // Always: CommandsAllocate <= CurrentlyAllocated 
    EAlfBatchBufferInfoCommandsAllocate
    };

/**
 *  API to control the command batch buffer
 *
 *  @code
 *   CAlfBatchBuffer& batchBuffer = iEnv->BatchBufferHandler();
 *
 *   // Get the batch buffer max size in bytes
 *   const TInt maxSize = batchBuffer.GetBatchBufferInfo( EAlfBatchBufferInfoMaxSize );
 *
 *   // Flush manually
 *   batchBuffer.FlushBatchBuffer();
 *  @endcode
 *
 *  @lib alfclient.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CAlfBatchBuffer ): public CBase
    {
    
public:

    /**
     * Two-phased constructor.
     * @param aEnv Environment class
     * @return New instance. Ownership returned.
     */
    static CAlfBatchBuffer* NewL( CAlfEnv& aEnv );


    /**
    * Destructor.
    */
    virtual ~CAlfBatchBuffer();

    /**
     * Sets the automatic flush mode.
     *
     * @param aAutoFlushMode - see TAutoFlushMode
     */    
    IMPORT_C void SetAutoFlushMode( TAlfAutoFlushMode aAutoFlushMode );
    
    /**
     * Returns the automatic flush mode.
     *
     * @return Auto-flush mode - see TAlfAutoFlushMode.
     */         
    IMPORT_C TAlfAutoFlushMode AutoFlushMode() const;
    
    /**
     * Flushes the batch buffer manually.
     *
     * Even on error case, the buffer is emptied.
     *
     * @return Symbian OS error code.
     */
    IMPORT_C TInt FlushBatchBuffer();
    
    /**
     * Controls the batch buffer size. 
     *
     * When the buffer reaches the max size, it will be automatically flushed
     * if mode not equals EAutoFlushNever.
     *
     * The buffer will be also flushed, if expanding the buffer fails in OOM.
     *
     * @param aBufferSize 
     *        >0 Batch buffer size in bytes. If < default size, 
     *          the default size will be used.
     *        KAlfBatchBufferSizeLimitToSystemDefault uses the default size.
     *        KAlfBatchBufferSizeNoLimit re-allocates a bigger 
     *          buffer when needed until OOM.
     */
    IMPORT_C void SetMaxBatchBufferSize( TInt aBufferSize );
    
    /**
     * Get info about the batch buffer. 
     *
     * @param aBufferInfoType See TAlfBatchBufferInfoType
     *
     * @return Value in bytes.
     */
    IMPORT_C TInt GetBatchBufferInfo( TAlfBatchBufferInfoType aBufferInfoType ) const;
    
private:

    CAlfBatchBuffer();
    void ConstructL( CAlfEnv& aEnv );
private: // data

    struct TBatchBufferData;
    TBatchBufferData* iData;
    };

#endif // C_ALFBATCHBUFFER_H
