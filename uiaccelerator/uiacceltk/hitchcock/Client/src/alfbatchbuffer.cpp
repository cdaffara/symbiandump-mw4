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



#include "alf/alfbatchbuffer.h"
#include "alf/alfenv.h"
#include "alfclient.h"

// ---------------------------------------------------------------------------
// TBatchBufferData
//
// private member data
// ---------------------------------------------------------------------------
//
struct CAlfBatchBuffer::TBatchBufferData
    {
    TBatchBufferData() : 
        iClient(0) {}
    
    RAlfClient* iClient;
    };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfBatchBuffer::CAlfBatchBuffer()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CAlfBatchBuffer::ConstructL( CAlfEnv& aEnv )
    {
    iData = new (ELeave) TBatchBufferData;
    iData->iClient = &aEnv.Client();
    }


// ---------------------------------------------------------------------------
// 2-phased constructor
// ---------------------------------------------------------------------------
//
CAlfBatchBuffer* CAlfBatchBuffer::NewL( CAlfEnv& aEnv )
    {
    CAlfBatchBuffer* self = new( ELeave ) CAlfBatchBuffer;
    CleanupStack::PushL( self );
    self->ConstructL(aEnv);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfBatchBuffer::~CAlfBatchBuffer()
    {
    if ( iData )
        {
        }
    delete iData;
    iData = NULL;
    }


// ---------------------------------------------------------------------------
// Sets the flush mode
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfBatchBuffer::SetAutoFlushMode( TAlfAutoFlushMode aAutoFlushMode )
    {
    iData->iClient->SetAutoFlushMode( aAutoFlushMode );
    }

// ---------------------------------------------------------------------------
// Gets the flush mode
// ---------------------------------------------------------------------------
//    
EXPORT_C TAlfAutoFlushMode CAlfBatchBuffer::AutoFlushMode() const
    {
    return iData->iClient->AutoFlushMode();
    }

// ---------------------------------------------------------------------------
// Manual flush
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CAlfBatchBuffer::FlushBatchBuffer()
    {
    return iData->iClient->FlushBatchBuffer();
    }

// ---------------------------------------------------------------------------
// Set the max size
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfBatchBuffer::SetMaxBatchBufferSize( TInt aBufferSize )
    {
    iData->iClient->SetMaxBatchBufferSize( aBufferSize );
    }

// ---------------------------------------------------------------------------
// Get batch buffer info
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CAlfBatchBuffer::GetBatchBufferInfo( 
        TAlfBatchBufferInfoType aBufferInfoType ) const
    {
    return iData->iClient->GetBatchBufferInfo( aBufferInfoType );
    }

