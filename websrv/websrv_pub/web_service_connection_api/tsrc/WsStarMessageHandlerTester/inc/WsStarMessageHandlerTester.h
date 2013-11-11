/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/









#ifndef WSSTARMESSAGEHANDLERTESTER_H
#define WSSTARMESSAGEHANDLERTESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

//  EXTERNAL INCLUDES
#include "wsstarmessagecontext.h"
#include "wsstarhandlercontext.h"
#include "wsstartrustclient.h"
#include <SenXmlReader.h>
#include "msencontext.h"
#include "senmessagehandler.h"
#include "wsstaraddressinghandler.h"
#include "wsstarenveloperhandler.h"
#include "wsstarcredentialcollectorhandler.h"
#include "wsstarpassporthandler.h"
#include "wsstarsecurityhandler.h"
#include <e32def.h>
//  INTERNAL INCLUDES

_LIT8(KFaultMessage12,"<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:wsa=\"http://www.w3.org/2005/08/addressing\"><S:Body><S:Fault><S:Code><S:Value>S:Receiver</S:Value><S:Subcode><S:Value>wsa:EndpointUnavailable</S:Value></S:Subcode></S:Code><S:Detail><wsa:RetryAfter>500</wsa:RetryAfter></S:Detail></S:Fault></S:Body></S:Envelope>");
_LIT8(KFaultMessage11,"<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\"><S:Header><wsa:ReplyTo>replyTo</wsa:ReplyTo></S:Header><S:Body><S:Fault><faultcode>wsa:MessageInformationHeaderRequired</faultcode></S:Fault></S:Body></S:Envelope>");
_LIT8(KFaultMessage11_1,"<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\"><S:Header><wsa:FaultTo>faultTo</wsa:FaultTo></S:Header><S:Body><S:Fault><faultcode>wsa:InvalidMessageInformationHeader</faultcode></S:Fault></S:Body></S:Envelope>");
_LIT8(KFaultMessage11_2,"<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\"><S:Body><S:Fault><faultcode>wsa:DestinationUnreachable</faultcode></S:Fault></S:Body></S:Envelope>");
_LIT8(KFaultMessage11_3,"<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\"><S:Body><S:Fault><faultcode>wsa:ActionNotSupported</faultcode></S:Fault></S:Body></S:Envelope>");
// Logging path
_LIT( KWsStarMessageHandlerTesterLogPath, "\\logs\\testframework\\WsStarMessageHandlerTester\\" ); 
// Log file
_LIT( KWsStarMessageHandlerTesterLogFile, "WsStarMessageHandlerTester.txt" ); 
_LIT( KWsStarMessageHandlerTesterLogFileWithTitle, "WsStarMessageHandlerTester_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CWsStarMessageHandlerTester;

// DATA TYPES
typedef TInt (CWsStarMessageHandlerTester::* TestFunction)(CStifItemParser&);

// CLASS DECLARATION

/**
*  CWsStarMessageHandlerTester test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CWsStarMessageHandlerTester) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWsStarMessageHandlerTester* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CWsStarMessageHandlerTester();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CWsStarMessageHandlerTester( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */


         void SetupL();

         void SetupAddressingL();

         void SetupEnveloperL();

         void SetupCredentialCollectorL();

         void SetupPassportL();

         void SetupSecurityL();
                        
         void Teardown();
        
//addressing
         TInt MT_CWSStarAddressingHandler_NewLL();
        
         TInt MT_CWSStarAddressingHandler_InvokeL1L();

         TInt MT_CWSStarAddressingHandler_InvokeL2L();

         TInt MT_CWSStarAddressingHandler_InvokeL3L();

         TInt MT_CWSStarAddressingHandler_InvokeL4L();

         TInt MT_CWSStarAddressingHandler_InvokeL5L();

         TInt MT_CWSStarAddressingHandler_InvokeL6L();
        
         TInt MT_CWSStarAddressingHandler_DirectionL();
        
         TInt MT_CWSStarAddressingHandler_PhaseL();
        
         TInt MT_CWSStarAddressingHandler_InitLL();
        
//enveloper
         TInt MT_CWSStarEnveloperHandler_NewLL();
        
         TInt MT_CWSStarEnveloperHandler_InvokeL1L();

         TInt MT_CWSStarEnveloperHandler_InvokeL2L();

         TInt MT_CWSStarEnveloperHandler_DirectionL();
        
         TInt MT_CWSStarEnveloperHandler_PhaseL();
        
         TInt MT_CWSStarEnveloperHandler_InitLL();

//credential collector
         TInt MT_CWSStarCredentialCollectorHandler_NewLL();
        
         TInt MT_CWSStarCredentialCollectorHandler_InvokeL1L();

         TInt MT_CWSStarCredentialCollectorHandler_InvokeL2L();

         TInt MT_CWSStarCredentialCollectorHandler_InvokeL3L();

         TInt MT_CWSStarCredentialCollectorHandler_DirectionL();
        
         TInt MT_CWSStarCredentialCollectorHandler_PhaseL();
        
         TInt MT_CWSStarCredentialCollectorHandler_InitLL();

//passport        
         TInt MT_CWSStarPassportHandler_NewLL();
        
         TInt MT_CWSStarPassportHandler_InvokeL1L();

         TInt MT_CWSStarPassportHandler_InvokeL2L();

         TInt MT_CWSStarPassportHandler_DirectionL();
        
         TInt MT_CWSStarPassportHandler_PhaseL();
        
         TInt MT_CWSStarPassportHandler_InitLL();

//security        
         TInt MT_CWSStarSecurityHandler_NewLL();
        
         TInt MT_CWSStarSecurityHandler_InvokeL1L();

         TInt MT_CWSStarSecurityHandler_InvokeL2L();

         TInt MT_CWSStarSecurityHandler_DirectionL();
        
         TInt MT_CWSStarSecurityHandler_PhaseL();
        
         TInt MT_CWSStarSecurityHandler_InitLL();
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        

    private:    // Data
        CWSStarMessageContext* iMessageContextOut;
        CWSStarMessageContext* iMessageContextIn;
        CSenXmlReader* iParser;
        RFileLogger* iLogger;
        CSenMessageHandler* iHandler; 
        CWSStarHandlerContext* iHandlerCtx;
        RPointerArray<HBufC8> iSignedParts;
        



    };

#endif      // WSSTARMESSAGEHANDLERTESTER_H

// End of File
