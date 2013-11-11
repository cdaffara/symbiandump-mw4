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
* Description:  This classes define dps operations requests and replys. 
*
*/


#ifndef DPSOPERATION_H
#define DPSOPERATION_H

#include <s32file.h>
#include "dpsparam.h"

class CDpsTransaction;
/**
*   This is the base class for all dps operations. 
*/
class TMDpsOperation
    {
    friend class CDpsTransaction;
    public:
        inline TMDpsOperation();
        /**
        *   Fills in dps operation request parameters. Dps engine must
        *   call this function to fill the dps operation request parameters.
        *   @param aArgs the dps operation request arguments
        *   @param aElems the dps operation elements
        *   @param aAttrib the dps operation attributes
        *   @param aTrader the pointer to CDpsTransaction object for filling 
        *   the request arguments for Dps Engine
        *   @return TInt KErrNone if successful or other system error if failed
        */
    
    protected:
        inline virtual TInt FillReqArgs(TDpsArgArray& aArgs, 
                                        TDpsEleArray& aElems,
                                        TDpsAttribute& aAttrib, 
                                        CDpsTransaction* aTrader);
            
       
        /**
        *   Fills in the dps operation reply parameters. Dps engine uses this
        *   function after the dps operation gets responsed. After this call,
        *   dps engine will call RequestComplete() to inform the client the
        *   completion of the dps operation.
        *   @param aArgs dps operation reply arguments.
        *   @param aParam the pointer to CDpsTransacton object for filling 
        *   the reply arguments for the client (print UI engine)
        *   @return TInt KErrNone if successful or other system error if failed
        */    
        inline virtual TInt FillRepArgs(const TDpsArgArray& aArguments, 
                                        CDpsTransaction* aTrader);
                                        
           
        /**
        *   Creates the Dps request script
        *   @param aArgs the arguments of the Dps request
        *   @param aElements the elements of the Dps request
        *   @param aAttribute the attribute of the Dps request
        *   @param aScript the buffer of the script
        *   @param aTrader the pointer to the CDpsTransaction object for 
        *   creating the Dps script
        */
        IMPORT_C virtual void CreateReqScriptL(const TDpsArgArray& aArguments, 
                                              const TDpsEleArray& aElements,
                                              TDpsAttribute aAttribute, 
                                              RWriteStream& aScript, 
                                              CDpsTransaction* aTrader); 
        
        
    public:
        // the dps operation result
        TDpsResult iResult;
        // the dps operaton enumeration
        TDpsSupportedOp iOperation;
    };

/**
*   The class for dps startJob operation
*/
class TDpsStartJob : public TMDpsOperation
    {
    friend class CDpsTransaction;
    public:
        /**
        *   Default constructor
        */
        inline TDpsStartJob();
            
    private:
        /**
        *   @see TMDpsOperation 
        */
        IMPORT_C TInt FillReqArgs(TDpsArgArray& aArgs, TDpsEleArray& aElems,
                                  TDpsAttribute& aAttrib, 
                                  CDpsTransaction* aTrader);
        
        /**
        *   @see TMDpsOperation 
        */                          
        IMPORT_C void CreateReqScriptL(const TDpsArgArray& aArgs, 
                                      const TDpsEleArray& aElements, 
                                      TDpsAttribute aAttribute, 
                                      RWriteStream& aScript, 
                                      CDpsTransaction* aTrader); 
        
    public:    
        // the request operation parameter which needed to be filled by 
        // the client    
        TDpsStartJobReq iReqParam;
        
    };

/**
*   The class for dps abortJob operation
*/    
class TDpsAbortJob : public TMDpsOperation
    {
    friend class CDpsTransaction;
    public:
        /**
        *   Default constructor
        */
        inline TDpsAbortJob();
        
    private:
        /**
        *   @see TMDpsOperation 
        */
        IMPORT_C TInt FillReqArgs(TDpsArgArray& aArgs, TDpsEleArray& aElems,
                                 TDpsAttribute& aAttrib, 
                                 CDpsTransaction* aTrader);
        
    public:    
        // the request operation parameter which needed to be filled by 
        // the client        
        TDpsAbortJobReq iReqParam;
        
    };

/**
*   The class for dps continueJob operation
*/        
class TDpsContinueJob : public TMDpsOperation
    {
    friend class CDpsTransaction;
    public:
        /**
        *   Default constructor
        */
        inline TDpsContinueJob();           
    };

/**
*   The class for dps continueJob operation
*/    
class TDpsGetJobStatus : public TMDpsOperation
    {
    friend class CDpsTransaction;
    public:    
        /**
        *   Default constructor
        */
        inline TDpsGetJobStatus();
        
    private:        
        /**
        *   @see TMDpsOperation 
        */    
        IMPORT_C TInt FillRepArgs(const TDpsArgArray& aArgs, 
                                  CDpsTransaction* aTrader);
        
    public:    
        // the request operation parameter which needed to be filled by 
        // the client
        TDpsJobStatusRep iRepParam;
        
    };
 
/**
*   The class for dps continueJob operation
*/    
class TDpsGetPrinterStatus : public TMDpsOperation
    {
    friend class CDpsTransaction;
    public:    
        /**
        *   Default constructor
        */
        inline TDpsGetPrinterStatus();
        
    private:        
        /**
        *   @see TMDpsOperation 
        */    
        IMPORT_C TInt FillRepArgs(const TDpsArgArray& aArgs, 
                                  CDpsTransaction* aTrader);
        
    public:    
        // the request operation parameter which needed to be filled by 
        // the client
        TDpsPrinterStatusRep iRepParam;
        
    };
    
/**
*   The class for dps continueJob operation
*/
class TDpsGetCapability : public TMDpsOperation
    {
    friend class CDpsTransaction;
    public:
        /**
        *   Default constructor
        */
        inline TDpsGetCapability();
        
    private:        
        /**
        *   @see TMDpsOperation 
        */
        IMPORT_C TInt FillReqArgs(TDpsArgArray& aArgs, TDpsEleArray& aElems,
                                 TDpsAttribute& aAttrib, 
                                 CDpsTransaction* aTrader);
        /**
        *   @see TMDpsOperation 
        */                                 
        IMPORT_C TInt FillRepArgs(const TDpsArgArray& aArgs, 
                                  CDpsTransaction* aParam);
        
        /**
        *   @see TMDpsOperation 
        */                          
        IMPORT_C void CreateReqScriptL(const TDpsArgArray& aArgs, 
                                       const TDpsEleArray& aElements,
                                       TDpsAttribute aAttribute, 
                                       RWriteStream& aScript, 
                                       CDpsTransaction* aTrader);
        
    public:    
        // the request operation parameter which needed to be filled by 
        // the client
        TDpsCapReq iReqParam;
        // the request operation parameter which needed to be filled by 
        // the client
        TDpsCapRep iRepParam;
        
    };

/**
*   The class for dps continueJob operation
*/    
class TDpsConfigPrintService : public TMDpsOperation
    {
    friend class CDpsTransaction;
    public:
        /**
        *   Default constructor
        */
        inline TDpsConfigPrintService();
        
    private:        
        /**
        *   @see TMDpsOperation 
        */
        IMPORT_C TInt FillReqArgs(TDpsArgArray& aArgs, TDpsEleArray& aElems,
                                 TDpsAttribute& aAttrib, 
                                 CDpsTransaction* aTrader);
        /**
        *   @see TMDpsOperation 
        */                                 
        IMPORT_C TInt FillRepArgs(const TDpsArgArray& aArgs, 
                                  CDpsTransaction* aTrader);
                                                                           
    public:    
        // the request operation parameter which needed to be filled by 
        // the client
        TDpsConfigPrintReq iReqParam;
        
        // the request operation parameter which needed to be filled by 
        // the client
        TDpsConfigPrintRep iRepParam;

    };

/**
*   The class defines dps events
*/    
NONSHARABLE_CLASS(TDpsEvents) 
	{
	public:
	    // the event enumeration
		TDpsEvent iEvent;
		// jobStatus event
		TDpsGetJobStatus iJobEvent;
		// printerStatus event
		TDpsGetPrinterStatus iPrinterEvent;
	};    
	
#include "dpsoperation.inl"	
#endif
