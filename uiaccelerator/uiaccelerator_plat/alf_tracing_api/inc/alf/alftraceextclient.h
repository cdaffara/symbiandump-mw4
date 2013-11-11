/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Trace Extension definition
*
*/

 

#ifndef R_ALFTRACEEXTCLIENT_H
#define R_ALFTRACEEXTCLIENT_H

#include <alf/alfclientbase.h>
#include <alf/alftraceinterface.h>

/**
 *  Client-side session object for tracing
 *
 *  The tracing session ensures that the server has tracing extension available.
 *  The trace session attaches to the main session, and forwards tracing requests via
 *  main session IPC to tracing extension
 *
 *  @lib alftraceextclient.lib
 *  @since S60 v3.2
 *
 */
class RAlfTraceExtClient : public RAlfClientBase, public MAlfTraceInterface
  	{
public:
    /**
     * Constructor.
     *
     * @since S60 v3.2
     *
     */
    IMPORT_C RAlfTraceExtClient();
	
    /**
     * Attach to host client. Tracing actions are targetted to host client.
     *
     * @since S60 v3.2
     * @param aHostClient Host client
     *
     */
	IMPORT_C void AttachToHostClient(const RAlfClientBase& aHostClient);

    /**
     * Detach from host client.
     *
     * @since S60 v3.2
     *
     */
	IMPORT_C void DetachFromHostClient();
	
    /**
     * Starts a new test case. If existing test case is already started, new nested
     * test case will be created.
     *
     * @since S60 v3.2
     * @aTraceCategory Test case category
     * @param aTestCaseName Test case name
     *
     */
    IMPORT_C void StartTestCase(TInt32 aTraceCategory, const TDesC& aTestCaseName);

    /**
     * Closes the current (most-recently created) test case.
     *
     * @since S60 v3.2
     *
     */
    IMPORT_C void EndTestCase(void);

    /**
     * Sets an output filter.
     *
     * Output filter controls which trace output events are actually written to trace file or device.    
     * Parameter aCategories can be one or more flags from EAlfTraceCategory enumeration.
     * Join multiple flags with OR (|) operator.
     *
     * @since S60 v3.2
     * @param aCategories One or more flags from EAlfTraceCategory enumaration
     * @return Previous output filter
     *
     */
    IMPORT_C TInt32 SetOutputFilter(TInt32 aCategories);

    /*
     * Output single string to trace output device or file. Time stamp and current test case
     * information will be concatenated with the string. Trace is written only if its
     * category matches with current output filter. If the trace belongs to more than one
     * category, categories can be joined with OR (|) operator.
     *
     * @since S60 v3.2
     * @param aTraceCategory Flag in EAlfTraceCategory enumaration
     * @param aString Output string
     *
     */
    IMPORT_C void OutputString(TInt32 aTraceCategory, const TDesC& aString);

    /*
     * Output single unsigned integer number with description to trace output device or file.
     * Time stamp and current test case information will be concatenated with the string. 
     * Trace is written only if its category matches with current output filter. If the
     * trace belongs to more than one category, categories can be joined with OR (|) operator.
     *
     * @since S60 v3.2
     * @param aTraceCategory Flag in EAlfTraceCategory enumaration
     * @param aDescription Description for number
     * @param aNumber Number value
     *
     */
    IMPORT_C void OutputNumber(TInt32 aTraceCategory, const TDesC& aDescription, TUint aNumber);

    /*
     * Output single signed integer number with description to trace output device or file.
     * Time stamp and current test case information will be concatenated with the string. 
     * Trace is written only if its category matches with current output filter. If the
     * trace belongs to more than one category, categories can be joined with OR (|) operator.
     *
     * @since S60 v3.2
     * @param aTraceCategory Flag in EAlfTraceCategory enumaration
     * @param aDescription Description for number
     * @param aNumber Number value
     *
     */
    IMPORT_C void OutputNumber(TInt32 aTraceCategory, const TDesC& aDescription, TInt aNumber);

    /*
     * Output single floating point number with description to trace output device or file.
     * Time stamp and current test case information will be concatenated with the string. 
     * Trace is written only if its category matches with current output filter. If the
     * trace belongs to more than one category, categories can be joined with OR (|) operator.
     *
     * @since S60 v3.2
     * @param aTraceCategory Flag in EAlfTraceCategory enumaration
     * @param aDescription Description for number
     * @param aNumber Number value
     *
     */
    IMPORT_C void OutputNumber(TInt32 aTraceCategory, const TDesC& aDescription, TReal32 aNumber) __SOFTFP;

    /*
     * Output arbitary string to trace output device or file, with no additional data nor filtering.
     *
     * @since S60 v3.2
     * @param aString Output string
     *
     */
    IMPORT_C void RawOutput(const TDesC& aString);

    /*
     * Flush buffered output data. This is effective only if the trace output device or file has
     *
     * @since S60 v3.2
     *
     */
    IMPORT_C void Flush(void);

private:
    class RSessionPlugin : public RSessionBase
        {
    public:
        void Attach(const RSessionBase& aHost);
        void Detach();
        TBool IsAttached() const;
        TInt SendPluginMessage(TInt aFunction, const TIpcArgs &aArgs) const;
        TInt SendReceivePluginMessage(TInt aFunction, const TIpcArgs &aArgs) const;
        };
	
private: // data
    
    /**
     * Forwarder session to the main session.
     */
    RSessionPlugin iHostClient;

    TInt iSpare1;
    TInt iSpare2;
	};

#endif // R_ALFTRACEEXTCLIENT_H
