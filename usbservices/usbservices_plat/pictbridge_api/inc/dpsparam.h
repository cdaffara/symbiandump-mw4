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
* Description:  These classes define the dps operation parameters. 
*
*/


#ifndef DPSPARAM_H
#define DPSPARAM_H

#include "dpsdefs.h"

enum TDpsSupportedOp
    {
    EDpsEmptyRequest = 0,
    EDpsConfigPrintService,
    EDpsGetCapability,
    EDpsGetJobStatus,
    EDpsGetPrinterStatus,
    EDpsStartJob,
    EDpsAbortJob,
    EDpsContinueJob
    };

struct TDpsArgsInt
    {
    TDpsArgument 	iElement;
    TUint32		iContent;
    };

/**
*   The class defines the print job information which is needed by
*   startJob operation
*/
NONSHARABLE_CLASS(TDpsPrintInfo)
    {
public:
    /**
    *
    */
    inline TDpsPrintInfo();
    
    /**
    *   Resets all parameters
    */    
    IMPORT_C  void Reset();
    
    // the file name        
    TBuf<KMaxArgLen> iFile; 
               
    TBool isDPOF;
    // this is 0 when UI passed it to dps. Dps engine must find the ID
    // for this file by asking ptp server
    TUint32 iFileID; 
    // if don't need to print file name, this is EFalse
    TBool iFileName;
    // if don't need to print date, this is empty
    TBuf<KDateLen> iDate;
    // if only one cope, this is emtpy
    TInt iCopies;
    // if not DPOF, this is emtpy
    TInt iPrtPID;
    // if not DPOF, this is emtpy
    TInt iCopyID;
    };

/**
*   Dps version defination
*/
NONSHARABLE_CLASS(TDpsVersion)
    {
    public:
        TInt iMajor;
        TInt iMinor;
    };

/**
*   AbortJob request
*/	
NONSHARABLE_CLASS(TDpsAbortJobReq)
    {
    public:
        TInt iAbortStyle;
    };
    
/**
*   ConfigurePrintService request
*/
class TDpsConfigPrintReq
    {
    public:
        RArray<TDpsVersion> iDpsVersions;
        TBuf8<KMaxArgLen> iVendorName;
        TDpsVersion iVendorVersion;
        TBuf8<KMaxArgLen> iProductName;
        TBuf8<KMaxArgLen> iSerialNo;
        
        /**
        *   Destructor. Declared as virtual so that it will be called by 
        *   its derived class.
        */
        inline virtual ~TDpsConfigPrintReq();
        
        /**
        *   Resets all class variables.
        */
        inline void Reset();        
    };

/**
*   ConfigurePrintService reply
*/
NONSHARABLE_CLASS(TDpsConfigPrintRep) : public TDpsConfigPrintReq
    {
    public:
        TInt iPrintAvailable;      
    };
    
/**
*	GetCapability request.
*/
NONSHARABLE_CLASS(TDpsCapReq)
    {
    public:
        TDpsArgument iCap;
        // layouts and paperTypes requests attributes
        TDpsAttribute iAttribute;
    };	
	    
/**
*   GetCapability reply
*/
NONSHARABLE_CLASS(TDpsCapRep) : public TDpsCapReq
    {
    public:
        RArray<TUint> iContent;
        RArray<TDpsPaperType> iPaperType;
        /**
        *   Destructor
        */
        inline ~TDpsCapRep();
        
        /**
        *   Resets all class variables
        */ 
        inline void Reset();
    };
                                	
/**
*	StartJob request has printInfo and jobConfig parameters.
*/
NONSHARABLE_CLASS(TDpsStartJobReq)
    {
    public:
        /**
	    *   Resets all member variables
        */
        inline void Reset();
	    	        
        /**
        *   Gets the number of all parameters, including ones under elements
        *   @return the number of parameters
        */
        IMPORT_C TInt GetParamNum();
		
        /**
        *   Destructor
        */    
        inline ~TDpsStartJobReq(); 
        RArray<TDpsArgsInt> iJobConfig;
        //there might be multiple printInfo in case of several pictures are 
        //selected to be printed 
        RArray<TDpsPrintInfo> iPrintInfo;		
    };	

/**
*   This class is for job status reply
*/	
NONSHARABLE_CLASS(TDpsJobStatusRep)
    {
    public:
        
        /**
        *   
        */
        inline TDpsJobStatusRep();
        /**
        *   Resets all memeber variables
        */
        IMPORT_C void Reset();
        
        TInt iProgress;
        TInt iImagesPrinted;
        TFileName iFilePath;
        TInt iPrtPID;
        TInt iCopyID;
    };
    
/**
*   This class is for device status reply
*/    
NONSHARABLE_CLASS(TDpsPrinterStatusRep)
    {
    public:
        TDpsPrintServiceStatus iPrintStatus;
        TDpsJobStatus iJobStatus;
        TDpsErrorStatus iErrorStatus;
        TDpsJobEndReason iJobEndReason;
        TBool iDisconnectEnable;
        TBool iCapabilityChange;
        TBool  iNewJobOk;
    };
#include "dpsparam.inl"    
#endif
