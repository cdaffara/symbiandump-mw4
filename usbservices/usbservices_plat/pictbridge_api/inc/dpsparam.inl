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
* Description:  inline functions of dps parameter. 
*
*/


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDpsPrintInfo::TDpsPrintInfo()
    {
    Reset();
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void TDpsConfigPrintReq::Reset()
    {
    iDpsVersions.Reset();
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TDpsConfigPrintReq::~TDpsConfigPrintReq()
    {
    iDpsVersions.Close();
    }       

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TDpsCapRep::~TDpsCapRep()    
    {
    Reset();
    }    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void TDpsStartJobReq::Reset()
    {
    iJobConfig.Reset();
    iPrintInfo.Reset();
    }   

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
TDpsStartJobReq::~TDpsStartJobReq() 
    {
	iJobConfig.Close();
	iPrintInfo.Close();
	}
	
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//			    
void TDpsCapRep::Reset()
    {
    iContent.Reset();
    iPaperType.Close();
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//			        
TDpsJobStatusRep::TDpsJobStatusRep()    
    {
    Reset();
    }