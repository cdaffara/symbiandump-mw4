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
* Description:   Abstract Trace interface definition
*
*/

 

#ifndef M_ALFTRACEINTERFACE_H
#define M_ALFTRACEINTERFACE_H

#include <e32base.h>

/**
 * Trace categories
 */
enum TAlfTraceCategory
    {

    /*
    * Report categories those do not belong to any other category
    */
    EAlfTraceCategoryGeneral                = 0x00000001,

    /*
    * Report object life change time events
    */
    EAlfTraceCategoryObjectLifeTime         = 0x00000002,

    /*
    * Events
    */
    EAlfTraceCategoryKeyEvent               = 0x00000004,
    EAlfTraceCategoryAppEvent               = 0x00000008,
    EAlfTraceCategorySystemEvent            = 0x00000010,
    EAlfTraceCategoryGeneralEvent           = 0x00000020,
    EAlfTraceCategoryAllEvents              = EAlfTraceCategoryKeyEvent |
                                              EAlfTraceCategoryAppEvent |
                                              EAlfTraceCategorySystemEvent |
                                              EAlfTraceCategoryGeneralEvent,

    /*
    * Program flow points
    */
    EAlfTraceCategoryProgramFlowSrcRefresh  = 0x00000040,
    EAlfTraceCategoryProgramFlowTextureLoad = 0x00000080,
    EAlfTraceCategoryProgramFlowGeneral     = 0x00000100,
    EAlfTraceCategoryProgramFlowAll         = 
                                              EAlfTraceCategoryProgramFlowSrcRefresh |
                                              EAlfTraceCategoryProgramFlowTextureLoad |
                                              EAlfTraceCategoryProgramFlowGeneral,

    /*
    * Report ipc event handling events
    */
    EAlfTraceCategoryIpcMessages            = 0x00000400,

    /*
    * Report RAM changes
    */
    EAlfTraceCategoryRAM                    = 0x00000800,

    /*
    * Report framerate
    */
    EAlfTraceCategoryFramerate              = 0x00001000,

    /*
    * General informational trace
    */
    EAlfTraceCategoryInfo                   = 0x08000000,

    /*
    * Trace from warnings
    */
    EAlfTraceCategoryWarning                = 0x10000000,    

    /*
    * Trace from errors
    */
    EAlfTraceCategoryError                  = 0x20000000,

    /*
    * Trace from panic situations
    */
    EAlfTraceCategoryPanic                  = 0x40000000,

    /*
    * Trace from trace internal error situations
    */
    EAlfTraceCategoryInternalError          = 0x80000000,

    /*
    * All traces
    */
    EAlfTraceCategoryAll                    = 0xffffffff,
    
    /*
    * No traces
    */    
    EAlfTraceCategoryNone                   = 0x00000000,
    };


/**
 *  Abstract trace interface
 *
 *  Interface for keeping track of test cases, and for receiving trace output requests.
 *
  *  @since S60 v3.2
 */
class MAlfTraceInterface
    {
public:

    /**
     * Starts a new test case. If existing test case is already started, new nested
     * test case will be created.
     *
     * @since S60 v3.2
     * @aTraceCategory Test case category
     * @param aTestCaseName Test case name
     *
     */
    virtual void StartTestCase(TInt32 aTraceCategory, const TDesC& aTestCaseName) = 0;

    /**
     * Closes the current (most-recently created) test case.
     *
     * @since S60 v3.2
     *
     */
    virtual void EndTestCase(void) = 0;

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
    virtual TInt32 SetOutputFilter(TInt32 aCategories) = 0;

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
    virtual void OutputString(TInt32 aTraceCategory, const TDesC& aString) = 0;

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
    virtual void OutputNumber(TInt32 aTraceCategory, const TDesC& aDescription, TUint aNumber) = 0;

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
    virtual void OutputNumber(TInt32 aTraceCategory, const TDesC& aDescription, TInt aNumber) = 0;

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
    virtual void OutputNumber(TInt32 aTraceCategory, const TDesC& aDescription, TReal32 aNumber) __SOFTFP = 0;

    /*
     * Output arbitary string to trace output device or file, with no additional data nor filtering.
     *
     * @since S60 v3.2
     * @param aString Output string
     *
     */
    virtual void RawOutput(const TDesC& aString) = 0;

    /*
     * Flush buffered output data. This is effective only if the trace output device or file has
     * a output buffer.
     *
     * @since S60 v3.2
     *
     */
    virtual void Flush(void) = 0;
    };

#endif // M_ALFTRACEINTERFACE_H

