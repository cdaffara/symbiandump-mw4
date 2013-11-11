/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpIconConversionActive class implementation.
*
*/

#include "upnpiconconversionactive.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnpiconconversionactiveTraces.h"
#endif

// Literal 
_LIT8(KBmpMimeType, "image/bmp");
 

// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpIconConversionActive::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpIconConversionActive* CUpnpIconConversionActive::NewL( RFile& aBitmapFile )
    {
    OstTraceFunctionEntry0( CUPNPICONCONVERSIONACTIVE_NEWL_ENTRY );
    CUpnpIconConversionActive* self = new (ELeave) CUpnpIconConversionActive( ); 
    CleanupStack::PushL(self);
    self->ConstructL( aBitmapFile );
    CleanupStack::Pop(self);
    return self;  
    }

// ---------------------------------------------------------------------------------
// CUpnpIconConversionActive::CUpnpIconConversionActive
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------------
//
CUpnpIconConversionActive::CUpnpIconConversionActive( )
    :CActive(EPriorityStandard)
    {
    OstTraceFunctionEntry0( CUPNPICONCONVERSIONACTIVE_CUPNPICONCONVERSIONACTIVE_ENTRY );
    CActiveScheduler::Add(this);
    OstTraceFunctionExit0( CUPNPICONCONVERSIONACTIVE_CUPNPICONCONVERSIONACTIVE_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpIconConversionActive::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------------
//
void CUpnpIconConversionActive::ConstructL( RFile& aBitmapFile )
    {
    OstTraceFunctionEntry0( CUPNPICONCONVERSIONACTIVE_CONSTRUCTL_ENTRY );
    // Provides access to the ICL (image conversion library) encoders.
    iEncoder = CImageEncoder::FileNewL( aBitmapFile,KBmpMimeType()); 
    OstTraceFunctionExit0( CUPNPICONCONVERSIONACTIVE_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpIconConversionActive::~CUpnpIconConversionActive
// Destructor
// ---------------------------------------------------------------------------------
// 
CUpnpIconConversionActive::~CUpnpIconConversionActive()
    {   
    OstTraceFunctionEntry0( DUP1_CUPNPICONCONVERSIONACTIVE_CUPNPICONCONVERSIONACTIVE_ENTRY );
    Cancel();
    delete iEncoder;
    OstTraceFunctionExit0( DUP1_CUPNPICONCONVERSIONACTIVE_CUPNPICONCONVERSIONACTIVE_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpIconConversionActive::Convert 
// Place where actual image conversion is invoked.
// Its an asynchronous conversion.
// @param aBitmap  Reference to Bitmap object
// ---------------------------------------------------------------------------------
// 
void CUpnpIconConversionActive::Convert( CFbsBitmap& aBitmap )
    {   
    OstTraceFunctionEntry0( CUPNPICONCONVERSIONACTIVE_CONVERT_ENTRY );
    TRequestStatus* status = &iStatus; 
    // Converts image data held in CFbsBitmap object
    iEncoder->Convert( status,aBitmap);   //Actual conversion API
    SetActive();
    OstTraceFunctionExit0( CUPNPICONCONVERSIONACTIVE_CONVERT_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpIconConversionActive::RunL
// Event handler. Stops the active scheduler.
// ---------------------------------------------------------------------------------
// 
void CUpnpIconConversionActive::RunL()
    {
    OstTraceFunctionEntry0( CUPNPICONCONVERSIONACTIVE_RUNL_ENTRY );
    CActiveScheduler::Stop();     
    OstTraceFunctionExit0( CUPNPICONCONVERSIONACTIVE_RUNL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpIconConversionActive::DoCancel
// ---------------------------------------------------------------------------------
//
void CUpnpIconConversionActive::DoCancel()
    {
    
    }

// ---------------------------------------------------------------------------------
// CUpnpIconConversionActive::FetchError
// Returns the completion status.
// @return Returns completion code.
// ---------------------------------------------------------------------------------
//
TInt CUpnpIconConversionActive::FetchError()
    {
    OstTraceFunctionEntry0( CUPNPICONCONVERSIONACTIVE_FETCHERROR_ENTRY );
    return ( iStatus.Int() );
    }
