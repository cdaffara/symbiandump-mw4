/*
* Copyright (c) 2006, 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  inline functions of TMDpsOperation
*
*/


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TMDpsOperation::TMDpsOperation()
    {
    iOperation = EDpsEmptyRequest;
    iResult.iMajorCode = EDpsResultOk;
    iResult.iMinorCode = EDpsResultNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt TMDpsOperation::FillReqArgs(TDpsArgArray&, TDpsEleArray&,
                                 TDpsAttribute&, CDpsTransaction*)
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt TMDpsOperation::FillRepArgs(const TDpsArgArray&, CDpsTransaction*)
     {
     return KErrNone;
     } 
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDpsStartJob::TDpsStartJob() : TMDpsOperation()
    {
    iOperation = EDpsStartJob;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDpsAbortJob::TDpsAbortJob() : TMDpsOperation()
    {
    iOperation = EDpsAbortJob;
    iReqParam.iAbortStyle = EDpsAbortStyleImmediately;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TDpsContinueJob::TDpsContinueJob() : TMDpsOperation()
    {
    iOperation = EDpsContinueJob;
    }
  
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TDpsGetJobStatus::TDpsGetJobStatus() : TMDpsOperation()
    {
    iOperation = EDpsGetJobStatus;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TDpsGetPrinterStatus::TDpsGetPrinterStatus() : TMDpsOperation()
    {
    iOperation = EDpsGetPrinterStatus;
    iRepParam = TDpsPrinterStatusRep();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TDpsGetCapability::TDpsGetCapability() : TMDpsOperation()
    {
    iOperation = EDpsGetCapability;
    iReqParam = TDpsCapReq();
    }
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TDpsConfigPrintService::TDpsConfigPrintService() : TMDpsOperation()
    {
    iOperation = EDpsConfigPrintService;         
    }
