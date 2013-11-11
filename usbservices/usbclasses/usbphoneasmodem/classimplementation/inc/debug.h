// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// 
// 


#ifndef ATPLUGINDEBUG_H
#define ATPLUGINDEBUG_H

#include <e32base.h>
#include <flogger.h>
#include <e32svr.h>


_LIT(KLogFile,"usbmscfile.txt");
_LIT(KLogDir,"usbmscfile");

_LIT(KTracePrefix16, "[UsbMscFile] ");
_LIT8(KTracePrefix8, "[UsbMscFile] ");
_LIT8(KFuncFormat8, ">< %S");
_LIT8(KFuncThisFormat8, ">< %S, [0x%08X]");
_LIT8(KFuncEntryFormat8, "-> %S");
_LIT8(KFuncEntryThisFormat8, "-> %S, [0x%08X]");
_LIT8(KFuncExitFormat8, "<- %S");
_LIT(KPanicCategory, "UsbMscFile");
_LIT8(KPanicPrefix8, "PANIC code ");
_LIT8(KLeavePrefix8, "LEAVE code ");

#ifdef _DEBUG

const TInt KMaxLogLineLength = 512;

// Trace options
#define KPRINTERROR		0x00000001 // Print error
#define KPRINTINFO	    0x00000002 // Print function trace
#define KPRINTSTATE		0x00000004 // Print state machine infos
#define KPRINTWARNING   0x00000008 // Print warnings

const TInt KTraceMask = KPRINTERROR | KPRINTINFO | KPRINTSTATE | KPRINTWARNING;

NONSHARABLE_CLASS(TOverflowTruncate16) : public TDes16Overflow
	{
public:
	void Overflow(TDes16& /*aDes*/) {}
	};

NONSHARABLE_CLASS(TOverflowTruncate8) : public TDes8Overflow
	{
public:
	void Overflow(TDes8& /*aDes*/) {}
	};

inline void Trace(TRefByValue<const TDesC16> aFmt, ...)
    {
	VA_LIST list;
	VA_START(list,aFmt);
#ifdef __WINS__
#ifdef _DEBUG
    RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, aFmt, list);
#endif
#else
	TBuf16<KMaxLogLineLength> theFinalString;
	theFinalString.Append(KTracePrefix16);
	TOverflowTruncate16 overflow;
	theFinalString.AppendFormatList(aFmt,list,&overflow);
	RDebug::Print(theFinalString);
#endif
    }

inline void Trace(TRefByValue<const TDesC8> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list, aFmt);
#ifdef __WINS__    
#ifdef _DEBUG
    RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, aFmt, list);
#endif
#else
	TOverflowTruncate8 overflow;
    TBuf8<KMaxLogLineLength> buf8;
    buf8.Append(KTracePrefix8);
	buf8.AppendFormatList(aFmt, list, &overflow);
    TBuf16<KMaxLogLineLength> buf16(buf8.Length());
    buf16.Copy(buf8);
    TRefByValue<const TDesC> tmpFmt(_L("%S"));
    RDebug::Print(tmpFmt, &buf16);
#endif
    }

inline void TracePanic(
	char* aFile, 
	TInt aLine,
	TInt aPanicCode,
	const TDesC& aPanicCategory)
    {
	TPtrC8 fullFileName((const TUint8*)aFile);
	TPtrC8 fileName(fullFileName.Ptr()+fullFileName.LocateReverse('\\')+1);
	TBuf8<KMaxLogLineLength> buf;
	buf.Append(KPanicPrefix8);
	buf.AppendFormat(_L8("%d at line %d in file %S"), aPanicCode, aLine, &fileName);
	Trace(buf);
	User::Panic(aPanicCategory, aPanicCode); 
    }

inline void TraceLeave(char* aFile, TInt aLine, TInt aReason)
	{
	TPtrC8 fullFileName((const TUint8*)aFile);
	TPtrC8 fileName(fullFileName.Ptr()+fullFileName.LocateReverse('\\')+1);
	TBuf8<KMaxLogLineLength> buf;
	buf.Append(KLeavePrefix8);
	buf.AppendFormat(_L8("%d at line %d in file %S"), aReason, aLine, &fileName);
	Trace(buf);
	User::LeaveIfError(aReason);
	}

#define TRACE_INFO(p) {if(KTraceMask & KPRINTINFO) Trace p;}

#define TRACE_ERROR(p) {if(KTraceMask & KPRINTERROR) Trace p;}

#define TRACE_STATE(p) {if(KTraceMask & KPRINTSTATE) Trace p;}

#define TRACE_WARNING(p) {if(KTraceMask & KPRINTWARNING) Trace p;}

#define TRACE_INFO_SEG(p) {if(KTraceMask & KPRINTINFO) p;}

#define TRACE_ASSERT(GUARD, CODE) {if (!(GUARD)) TracePanic(__FILE__, __LINE__, CODE, KPanicCategory);}

#define PANIC(CODE) TracePanic(__FILE__, __LINE__, CODE, KPanicCategory)

#define LEAVE_IF_ERROR(REASON) {if (REASON) TraceLeave(__FILE__, __LINE__, REASON);}

#define LEAVE(REASON) TraceLeave(__FILE__, __LINE__, REASON)

#define TRACE_FUNC_ENTRY {if(KTraceMask & KPRINTINFO) { TPtrC8 ptr8((TUint8*)__PRETTY_FUNCTION__); Trace(KFuncEntryFormat8, &ptr8);}}

#define TRACE_FUNC_ENTRY_THIS {if(KTraceMask & KPRINTINFO) { TPtrC8 ptr8((TUint8*)__PRETTY_FUNCTION__); Trace(KFuncEntryThisFormat8, &ptr8, this);}}

#define	 TRACE_FUNC_EXIT {if(KTraceMask & KPRINTINFO) { TPtrC8 ptr8((TUint8*)__PRETTY_FUNCTION__); Trace(KFuncExitFormat8, &ptr8);}}

#define TRACE_FUNC {if(KTraceMask & KPRINTINFO) { TPtrC8 ptr8((TUint8*)__PRETTY_FUNCTION__); Trace(KFuncFormat8, &ptr8);}}

#define TRACE_FUNC_THIS {if(KTraceMask & KPRINTINFO) { TPtrC8 ptr8((TUint8*)__PRETTY_FUNCTION__); Trace(KFuncThisFormat8, &ptr8, this);}}

#define RETURN_IF_ERR(ERR) {if(ERR) {Trace(_L8(" RETURN %d at file %S line %d"), ERR, &(TPtrC8((const TUint8*)__FILE__)), __LINE__); return ERR;}}

#else // ! _DEBUG

#define TRACE_INFO(p)

#define TRACE_ERROR(p)

#define TRACE_STATE(p)

#define TRACE_WARNING(p)

#define TRACE_INFO_SEG(p)

#define TRACE_ASSERT(GUARD, CODE)

#define PANIC(CODE) {User::Panic(KPanicCategory, CODE);}

#define LEAVE_IF_ERROR(REASON) {static_cast<void>(User::LeaveIfError(REASON));}

#define LEAVE(REASON) {static_cast<void>(User::Leave(REASON));}

#define TRACE_FUNC_ENTRY

#define TRACE_FUNC_ENTRY_THIS

#define TRACE_FUNC_EXIT

#define TRACE_FUNC

#define TRACE_FUNC_THIS

#define RETURN_IF_ERR(ERR) {if(ERR) return ERR;}
#endif // _DEBUG

#endif // ATPLUGINDEBUG_H

// End of File
