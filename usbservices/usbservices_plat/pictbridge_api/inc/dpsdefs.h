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
* Description:  This class defines the dps definations. 
*
*/


#ifndef DPSDEFS_H
#define DPSDEFS_H

#include <e32base.h>

const TInt KMaxArgLen = 256;
const TInt KDateLen = 32;

// special element which has sub-elements, only startJob and 
// getCapability have this field
enum TDpsElement
    {
    EDpsEleEmpty = 0,
    EDpsCapability,
    EDpsJobConfig,
    EDpsPrintInfo,
    KDpsEleMax
    };
    
enum TDpsEvent
    {
    EDpsEvtEmpty = 0,
    EDpsEvtNotifyJobStatus,
    EDpsEvtNotifyDeviceStatus,
    KDpsEvtMax
    };
	
enum TDpsArgument
    {
    EDpsArgDpsVersions = 0,
    EDpsArgVendorName,
    EDpsArgVendorSpecificVersion,
    EDpsArgProductName,
    EDpsArgSerialNo,
    EDpsArgPrintServiceAvailable,
    EDpsArgQualities,
    EDpsArgPaperSizes,
    EDpsArgPaperTypes,
    EDpsArgFileTypes,
    EDpsArgDatePrints,
    EDpsArgFileNamePrints,
    EDpsArgImageOptimizes,
    EDpsArgLayouts,
    EDpsArgFixedSizes,
    EDpsArgChroppings,
    EDpsArgPrtPID,
    EDpsArgFilePath,
    EDpsArgCopyID,
    EDpsArgProgress,
    EDpsArgImagePrinted,
    EDpsArgDpsPrintServiceStatus,
    EDpsArgJobEndReason,
    EDpsArgErrorStatus,
    EDpsArgErrorReason,
    EDpsArgDisconnectEnable,
    EDpsArgCapabilityChanged,
    EDpsArgNewJobOk,
    EDpsArgQuality,
    EDpsArgPaperSize,
    EDpsArgPaperType,
    EDpsArgFileType,
    EDpsArgDatePrint,
    EDpsArgFileNamePrint,
    EDpsArgImageOptimize,
    EDpsArgLayout,
    EDpsArgFixedSize,
    EDpsArgCropping,
    EDpsArgCroppingArea,
    EDpsArgFileID,
    EDpsArgFileName,
    EDpsArgDate,
    EDpsArgCopies,
    EDpsArgAbortStyle,
    EDpsArgImagesPrinted,
    EDpsArgBasePathID,
    EDpsArgFileSize,
    EDpsArgThumbFormat,
    EDpsArgThumbSize,
    EDpsArgBytesRead,
    EDpsArgOffset,
    EDpsArgMaxSize,
    EDpsArgParentFileID,
    EDpsArgMaxNumIDs,
    EDpsArgFileIDs,
    EDpsArgNumIDs,
    EDpsArgMax
    };

// define the DPS action result
// ref: DPS spec page 52
// high bits
enum TDpsResultMajorCode
    {
    EDpsResultOk = 0x1000,
    EDpsResultNotExecuted = 0x1001,
    EDpsResultNotSupported = 0x1002,
    EDpsResultNotRecognized = 0x1003
    };
	
// define the DPS action result minor code
// ref: DPS spec page 52
// low bits
enum TDpsResultMinorCode
    {
    EDpsResultNone = 0x0000,
    EDpsResultUnrecognizedParam = 0x0001,
    EDpsResultillegalParam = 		0x0002,
    EDpsResultMissingParam = 		0x0003,
    EDpsResultBufferOverflow = 	0x0004	
    };

// define the DPS service availability
// ref: DPS spec page 53
// only high bits are useful
enum TDpsServiceAvailability
    {
    EDpsPrintServiceAvailableFalse = 0x3000,
    EDpsPrintServiceAvailableTrue = 0x3001	
    };

// define printing qualities
// ref: DPS spec page 54
// only high bits are useful
enum TDpsPrintQuality
    {
    EDpsPrintQualityDefault = 0x5000,
    EDpsPrintQualityNormal = 	0x5001,
    EDpsPrintQualityDraft = 	0x5002,
    EDpsPrintQualityFine = 	0x5003	
    };

// define paper sizes
// ref: DPS spec page 54
// only high bits are useful
enum TDpsPaperSizes
    {
    EDpsPaperSizeDefault = 	0x5100,
    EDpsPaperSizeL = 		0x5101,
    EDpsPaperSize2L = 		0x5102,
    EDpsPaperSizePostcard = 0x5103,
    EDpsPaperSizeCard = 	0x5104,
    EDpsPaperSize100x150 = 	0x5105,
    EDpsPaperSize4x6 = 		0x5106,
    EDpsPaperSize8x10 = 	0x5107,
    EDpsPaperSizeLetter = 	0x5108,
    EDpsPaperSize11x17 = 	0x510A,
    EDpsPaperSizeA0 = 		0x5110,
    EDpsPaperSizeA1 = 		0x5111,
    EDpsPaperSizeA2 = 		0x5112,
    EDpsPaperSizeA3 = 		0x5113,
    EDpsPaperSizeA4 = 		0x5114,
    EDpsPaperSizeA5 = 		0x5115,
    EDpsPaperSizeA6 = 		0x5116,
    EDpsPaperSizeA7 = 		0x5117,
    EDpsPaperSizeA8 = 		0x5118,
    EDpsPaperSizeA9 = 		0x5119,
    EDpsPaperSizeB0 = 		0x5120,
    EDpsPaperSizeB1 = 		0x5121,
    EDpsPaperSizeB2 = 		0x5122,
    EDpsPaperSizeB3 = 		0x5123,
    EDpsPaperSizeB4 = 		0x5124,
    EDpsPaperSizeB5 = 		0x5125,
    EDpsPaperSizeB6 = 		0x5126,
    EDpsPaperSizeB7 = 		0x5127,
    EDpsPaperSizeB8 = 		0x5128,
    EDpsPaperSizeB9 = 		0x5129,
    EDpsPaperSize89 = 		0x5181,
    EDpsPaperSize127 = 		0x5182,
    EDpsPaperSize100 = 		0x5186,
    EDpsPaperSize210 = 		0x5194
    };
// define paper types
// ref: DPS spec page 54
// only high bits are useful
enum TDpsPaperTypeMajor
    {
    EDpsPaperTypeDefault = 		0x5200,
    EDpsPaperTypePlainPaper = 	0x5201,
    EDpsPaperTypePhotoPaper = 	0x5202,
    EDpsPaperTypeFastPhotopaper = 	0x5203	
    };

enum TDpsPaperTypeMinor
    {
    EDpsPaperTypeStationery = 1,
    EDpsPaperTypeStationeryCoated,
    EDpsPaperTypeStationeryInkjet,
    EDpsPaperTypeStationeryPreprinted,
    EDpsPaperTypeStationeryLetterhead,
    EDpsPaperTypeStationeryPrepunched,
    EDpsPaperTypeStationeryFine,
    EDpsPaperTypeStationeryHeavyweight,
    EDpsPaperTypeStationeryLightweight,
    EDpsPaperTypeTransparency,
    EDpsPaperTypeEnvelope,
    EDpsPaperTypeEnvelopePlain,
    EDpsPaperTypeEnvelopeWindow,
    EDpsPaperTypeContinuous,
    EDpsPaperTypeContinuousLong,
    EDpsPaperTypeContinuousShort,
    EDpsPaperTypeTabStock,
    EDpsPaperTypePreCutTabs,
    EDpsPaperTypeFullCutTabs,
    EDpsPaperTypeMultiPartForm,
    EDpsPaperTypeLabels,
    EDpsPaperTypeMultiLayer,
    EDpsPaperTypeScreen,
    EDpsPaperTypeScreenPaged,
    EDpsPaperTypePhotographic,
    EDpsPaperTypePhotographicGlossy,
    EDpsPaperTypePhotographicHighGloss,
    EDpsPaperTypePhotographicSemiGloss,
    EDpsPaperTypePhotographicSatin,
    EDpsPaperTypePhotographicMatte,
    EDpsPaperTypePhotographicFilm,
    EDpsPaperTypeBackPrintFilm,
    EDpsPaperTypeCardStock
    };
    
struct TDpsPaperType
    {
    TDpsPaperTypeMajor iMajor;
    TDpsPaperTypeMinor iMinor;
    };

// define file types
// ref: DPS sepc page 55
// only high bits are useful
enum TDpsFileType
    {
    EDpsFileTypeDefault =   0x5300,
    EDpsFileTypeEXIF =      0x5301,
    EDpsFileTypeJPEG =      0x5303	
    };

// define date print
// ref: DPS sepc page 55
// only high bits are useful
enum TDpsDatePrint
    {
    EDpsDatePrintDefault = 0x5400,
    EDpsDatePrintOff = 0x5401,
    EDpsDatePrintOn = 0x5402
    };

// define fle name print
// ref: DPS sepc page 56
// only high bits are useful
enum TDpsFileNamePrint
    {
    EDpsFileNamePrintDefault = 	0x5500,
    EDpsFileNamePrintOff = 		0x5501,
    EDpsFileNamePrintOn = 		0x5503	
    };

// define image optimization
// ref: DPS sepc page 56
// only high bits are useful
enum TDpsImageOptimize
    {
    EDpsImageOptimizeDefault = 	0x5600,
    EDpsImageOptimizeOff = 		0x5601,
    EDpsImageOptimizeOn = 		0x5603	
    };

// define layouts
// ref: DPS sepc page 56
// only high bits are useful
enum TDpsLayout
    {
    EDpsLayoutDefault = 	0x5700,
    EDpsLayout1Up = 		0x5701,
    EDpsLayout2Up = 		0x5702,
    EDpsLayout3Up = 		0x5703,
    EDpsLayout4Up = 		0x5704,
    EDpsLayout5Up = 		0x5705,
    EDpsLayout6Up = 		0x5706,
    EDpsLayout7Up = 		0x5707,
    EDpsLayout8Up = 		0x5708,
    EDpsLayout9Up = 		0x5709,
    EDpsLayoutIndex = 		0x57FE,
    EDpsLayoutBorderless =  0x57FF	
    };

// define fixed sizes
// ref: DPS sepc page 57
// only high bits are useful
enum TDpsFixedSizes
    {
    EDpsFixedSizeDefault =  0x5800,
    EDpsFixedSize4x6 =      0x5803,
    EDpsFixedSize5x7 =      0x5804,
    EDpsFixedSizeA4 =       0x5811,
    EDpsFixedSizeLetter =   0x5812
    };

// define croppings
// ref: DPS sepc page 57
// only high bits are useful
enum TDpsCropping
    {
    EDpsCroppingDefault = 	0x5900,
    EDpsCroppingOff = 		0x5901,
    EDpsCroppingOn = 		0x5902	
    };

// define Device status
// ref: DPS sepc page 61
// only high bits are useful
// 1. Print service status
enum TDpsPrintServiceStatus
    {
    EDpsPrintServiceStatusInit = 0,
    EDpsPrintServiceStatusPrinting = 	0x7000,
    EDpsPrintServiceStatusIdle = 		0x7001,
    EDpsPrintServiceStatusPaused = 		0x7002
    };
    
enum TDpsJobStatus
    {
    // 2. Job end reasons
    EDpsJobStatusNotEnded = 					0x7100,
    EDpsJobStatusEndedOk = 						0x7101,
    EDpsJobStatusEndedAbortImmediately = 		0x7102,
    EDpsJobStatusEndedAbortCompleteCurrent = 	0x7103,
    EDpsJobStatusEndedOther = 					0x7104    
    };	
	
	// 3. Error status
enum TDpsErrorStatus
    {
    EDpsErrorStatusOk =         0x7200,
    EDpsErrorStatusWarning = 	0x7201,
    EDpsErrorStatusFatal = 	    0x7202
    };
         
enum TDpsJobEndReasonMajor
    {
    // 4. Job end reasons
    EDpsJobErrorNone = 		0x7300,
    EDpsJobErrorPaper = 	0x7301,
    EDpsJobErrorInk = 		0x7302,
    EDpsJobErrorHardware = 	0x7303,
    EDpsJobErrorFile = 		0x7304    
    };

enum TDpsJobEndReasonPaper
    {
    EDpsPaperDefault = 0,
    EDpsPaperEmpty = 0x0100,
    EDpsPaperLoad = 0x200,
    EDpsPaperEject = 0x300,
    EDpsPaperMedia = 0x400,
    EDpsPaperJam = 0x500,
    EDpsPaperNearlyEmpty = 0x600,
    EDpsPaperTypeSizeNoMatch = 0x700
    };
 
enum TDpsJobEndReasonInk
    {
    EDpsInkDefault = 0,
    EDpsInkEmpty = 0x100,
    EDpsInkLow = 0x200,
    EDpsInkWaste = 0x300   
    };
   
enum TDpsJobEndReasonHard
    {
    EDpsHardDefault = 0,
    EDpsHardFatal = 0x0100,
    EDpsHardServiceCall = 0x0200,
    EDpsHardNotAvailable = 0x0300,
    EDpsHardBusy = 0x0400,
    EDpsHardLever = 0x0500,
    EDpsHardCoverOpen = 0x0600,
    EDpsHardNoMarkingHead = 0x0700,
    EDpsHardInkCoverOpen = 0x0800,
    EDpsHardNoInkCartridge = 0x0900
    };
    
enum TDpsJobEndReasonFile
    {
    EDpsFileDefault = 0,
    EDpsFilePrintInfo = 0x0100,
    EDpsFileDecode = 0x0200
    };
    
struct TDpsJobEndReason 
    {
    TDpsJobEndReasonMajor iMajor;
    TDpsJobEndReasonPaper iPaperMinor;
    TDpsJobEndReasonInk iInkMinor;
    TDpsJobEndReasonHard iHardMinor;
    TDpsJobEndReasonFile iFileMinor;
    };
    
enum TDpsDisconnectEnable
    {
    // 5. Disconnect Enable
    EDpsDisconnectEnableFalse = 0x7400,
    EDpsDisconnectEnableTrue = 	0x7401    
    };
    
enum TDpsCapabilityChanged
    {
	// 6. Capability changes
    EDpsCapabilityChangedFalse = 	0x7500,
    EDpsCapabilityChangedTrue = 	0x7501
    };
    
enum TDpsNewJobOk
    {
    // 7. New Job Ok
    EDpsNewJobOkFalse = 	0x7600,
    EDpsNewJobOkTrue = 		0x7601	
    };


// define error reason minor codes
// ref: DPS sepc page 62
enum TDpsErrorMinorCode
    {
    EDpsErrorPaperEmpty =       0x0100,
    EDpsErrorPaperJam =         0x0500,
    EDpsErrorPaperUnsupport =   0x0700,
    EDpsErrorInkEmpty =         0x0100
    };

// define About style
// ref: DPS spec page 68
// only high bits are useful
enum TDpsAbortStyle
    {
    EDpsAbortStyleImmediately = 		0x9000,
    EDpsAbortStyleCompleteCurrent = 	0x9001	
    };

typedef TUint TDpsAttribute;

struct TDpsEle
    {
    TDpsElement iElement;
    // number of arguments included in this element
    TInt		iNum;
    };
    
typedef RArray<TDpsEle> TDpsEleArray;

struct TDpsArg
    {
    TDpsArgument 	iElement;
    TBuf8<KMaxArgLen>		iContent;
    };
	 
// used for get DPS respond	
typedef RArray<TDpsArg> TDpsArgArray;

struct TDpsResult
    {
    TDpsResultMajorCode iMajorCode;
    TDpsResultMinorCode iMinorCode;	
    };
	
#endif

