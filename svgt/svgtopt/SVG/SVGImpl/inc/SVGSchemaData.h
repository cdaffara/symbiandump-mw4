/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation header file
 *
*/


#include <e32std.h>
#include <bautils.h>
#include <badesca.h>

#include <fbs.h>
#include <f32file.h>
#include <s32file.h>



#ifndef _INC_CSVGSCHEMADATA_
#define _INC_CSVGSCHEMADATA_

    /*Added for binay Bytedata identification*/
    const TUint32 KBinaryFile          = 66737868;
	const TUint32 KBinaryFile2         = 66737869;
	const TUint32 KBinaryFile3         = 66737870;

	// Added for RGB & Float
	const TUint32 KBinaryFile4		   = 66737871;

    const  TUint16 KSvgOffset          =25;

    //AA: Length or number type: range is 0 to +max & set using SetAttributeFloatL
    const TUint16 KAtrTextLength        = 0 + KSvgOffset;
    const TUint16 KAtrWidth             = 1 + KSvgOffset;
    const TUint16 KAtrHeight            = 2 + KSvgOffset;
    const TUint16 KAtrR                 = 3 + KSvgOffset ;



    const TUint8  KSvgCoordAttrStartIndex = 4 + KSvgOffset;
    const TUint16 KAtrRx                = 4 + KSvgOffset;
    const TUint16 KAtrRy                = 5 + KSvgOffset;
    const TUint16 KAtrHorizAdvX         = 6 + KSvgOffset;
    const TUint16 KAtrHorizOriginX      = 7 + KSvgOffset;
    const TUint16 KAtrHorizOriginY      = 8 + KSvgOffset;
    const TUint16 KAtrAscent                = 9 + KSvgOffset;
    const TUint16 KAtrDescent               = 10 + KSvgOffset;

    const TUint16 KAtrAlphabetic            = 11 + KSvgOffset;
    const TUint16 KAtrUnderlinePosition     = 12 + KSvgOffset;
    const TUint16 KAtrUnderlineThickness    = 13 + KSvgOffset;
    const TUint16 KAtrOverlinePosition      = 14 + KSvgOffset;
    const TUint16 KAtrOverlineThickness     = 15 + KSvgOffset;
    const TUint16 KAtrStrikethroughPosition = 16 + KSvgOffset;
    const TUint16 KAtrStrikethroughThickness = 17 + KSvgOffset;
    const TUint16 KAtrUnitsPerEm            = 18 + KSvgOffset;

    const TUint16 KAtrWordSpacing           = 19 + KSvgOffset;
    const TUint16 KAtrLetterSpacing         = 20 + KSvgOffset;



    //AA: Coordinate datatype: range is -max to +max & set using SetAttributeFloatL
    const TUint16 KAtrCx                    = 21 + KSvgOffset;
    const TUint16 KAtrCy                    = 22 + KSvgOffset;
    const TUint16 KAtrY                     = 23 + KSvgOffset;
    const TUint16 KAtrX                     = 24 + KSvgOffset;
    const TUint16 KAtrY1                    = 25 + KSvgOffset;
    const TUint16 KAtrY2                    = 26 + KSvgOffset;
    const TUint16 KAtrX1                    = 27 + KSvgOffset;
    const TUint16 KAtrX2                    = 28 + KSvgOffset;

    const TUint8  KSvgCoordAttrEndIndex     = 28 + KSvgOffset;
    const TUint8  KSvgDesAttrsIndex         = 29 + KSvgOffset;


    //AA: Descriptor type  set using SetAttributeDesL
    const TUint16 KAtrK                 = 29 + KSvgOffset;
    const TUint8  KSvgFloatAttrEndIndex     = 29 + KSvgOffset;
    const TUint16 KAtrG1                = 30 + KSvgOffset;
    const TUint16 KAtrG2                = 31 + KSvgOffset;
    const TUint16 KAtrU1                = 32 + KSvgOffset;
    const TUint16 KAtrU2                = 33 + KSvgOffset;
    const TUint16 KAtrUnicode           = 34 + KSvgOffset;
    const TUint16 KAtrGlyphName         = 35 + KSvgOffset;
    const TUint16 KAtrLang              = 36 + KSvgOffset;
    const TUint8  KSvgDesAttrEndIndex       = 36 + KSvgOffset;

    const TUint16 KAtrTextDecoration    = 37 + KSvgOffset;
    const TUint16 KAtrTextAnchor        = 38 + KSvgOffset;
    const TUint16 KAtrRotate            = 39 + KSvgOffset;
    const TUint16 KAtrCdata             = 40 + KSvgOffset;




    const TUint16 KAtrTransform         = 41 + KSvgOffset;
    const TUint16 KAtrStyle             = 42 + KSvgOffset;
    const TUint16 KAtrFill              = 43 + KSvgOffset;
    const TUint16 KAtrStroke            = 44 + KSvgOffset;
    const TUint16 KAtrColor             = 45 + KSvgOffset;
    const TUint16 KAtrFrom              = 46 + KSvgOffset;
    const TUint16 KAtrTo                = 47 + KSvgOffset;
    const TUint16 KAtrBy                = 48 + KSvgOffset;
    const TUint16 KAtrAttributeName     = 49 + KSvgOffset;



    const TUint8  KSvgNotHandledAttrsIndex = 50 + KSvgOffset;

    const TUint16 KAtrPathLength        = 50 + KSvgOffset;
    const TUint16 KAtrVersion           = 51 + KSvgOffset;
    const TUint16 KAtrStrokeWidth       = 52 + KSvgOffset;
    const TUint16 KAtrPoints            = 53 + KSvgOffset;
    const TUint16 KAtrD                 = 54 + KSvgOffset;
    const TUint16 KAtrType              = 55 + KSvgOffset;
    const TUint16 KAtrStopColor         = 56 + KSvgOffset;
    const TUint16 KAtrFx                = 57 + KSvgOffset;
    const TUint16 KAtrFy                = 58 + KSvgOffset;

    const TUint16 KAtrOffset            = 59+ KSvgOffset;
    const TUint16 KAtrSpreadMethods     = 60 + KSvgOffset;
    const TUint16 KAtrGradientUnits     = 61 + KSvgOffset;
    const TUint16 KAtrStopOpacity       = 62 + KSvgOffset;




// special attributes for encoding
    const TUint16 KAtrViewBox              = 63 + KSvgOffset;
    const TUint16 KAtrBaseProfile          = 64 + KSvgOffset;
    const TUint16 KAtrZoomAndPan           = 65 + KSvgOffset;
    const TUint16 KAtrPreserveAspectRatio  = 66 + KSvgOffset;

    const TUint8  KSvgSVGAttrEndIndex   = KAtrPreserveAspectRatio;

    const TUint16 KAtrId                   = 67 + KSvgOffset;
    const TUint16 KAtrXmlBase              = 68 + KSvgOffset;

    const TUint8  KSvgIdAndXmlAttrEndIndex  = 68 + KSvgOffset;

    const TUint16 KAtrXmlLang              = 69 + KSvgOffset;
    const TUint16 KAtrXmlSpace             = 70 + KSvgOffset;

    const TUint8  KSvgLangAttrEndIndex  = 70 + KSvgOffset;

    const TUint16 KAtrRequiredExtensions   = 71 + KSvgOffset;
    const TUint16 KAtrRequiredFeatures     = 72 + KSvgOffset;
    const TUint16 KAtrSystemLanguage       = 73 + KSvgOffset;

    const TUint8  KSvgTestAttrEndIndex  = 73 + KSvgOffset;

    const TUint16 KAtrDx                   = 74 + KSvgOffset;
    const TUint16 KAtrDy                   = 75 + KSvgOffset;
    const TUint16 KAtrMedia                = 76 + KSvgOffset;
    const TUint16 KAtrTitle                = 77 + KSvgOffset;

    const TUint16 KAtrXlinkactuate         = 78 + KSvgOffset;
    const TUint16 KAtrXlinkarcrole         = 79 + KSvgOffset;

    const TUint16 KAtrXlinkrole            = 80 + KSvgOffset;
    const TUint16 KAtrXlinkshow            = 81 + KSvgOffset;
    const TUint16 KAtrXlinktitle           = 82 + KSvgOffset;
    const TUint16 KAtrXlinktype            = 83 + KSvgOffset;
    const TUint16 KAtrXlinkhref            = 84 + KSvgOffset;

   const TUint8  KSvgUriAttrEndIndex    = 84 + KSvgOffset;

    const TUint16 KAtrBegin                = 85 + KSvgOffset;
    const TUint16 KAtrDur                  = 86 + KSvgOffset;
    const TUint16 KAtrRepeatCount          = 87 + KSvgOffset;
    const TUint16 KAtrRepeatDur            = 88 + KSvgOffset;
    const TUint16 KAtrEnd                  = 89 + KSvgOffset;
    const TUint16 KAtrRestart              = 90 + KSvgOffset;
    const TUint16 KAtrAccumulate           = 91 + KSvgOffset;
    const TUint16 KAtrAdditive             = 92 + KSvgOffset;
    const TUint16 KAtrKeySplines               = 93 + KSvgOffset;
    const TUint16 KAtrKeyTimes              = 94 + KSvgOffset;
    const TUint16 KAtrCalcMode             = 95 + KSvgOffset;
    const TUint16 KAtrPath                 = 96 + KSvgOffset;
    const TUint16 KAtrAnimateMotion        = 97 + KSvgOffset;
    const TUint16 KAtrGradientTransform    = 98 + KSvgOffset;
    const TUint16 KAtrAnimateTransformAttrId = 99 + KSvgOffset;
    const TUint16 KAtrTarget                 = 100 + KSvgOffset;
    const TUint16 KAtrSolidOpacity           = 101 + KSvgOffset;
    const TUint16 KAtrSyncBehaviorDefault    = 102 + KSvgOffset;
    const TUint16 KAtrSyncToleranceDefault   = 103 + KSvgOffset;
    const TUint16 KAtrSyncBehavior           = 104 + KSvgOffset;
    const TUint16 KAtrSyncTolerance          = 105 + KSvgOffset;
    const TUint16 KAtrSyncMaster             = 106 + KSvgOffset;
    const TUint16 KAtrInitialVisibility      = 107 + KSvgOffset;
    const TUint16 KAtrVolume                 = 108 + KSvgOffset;
    const TUint16 KAtrAudioLevel             = 109 + KSvgOffset;


#define KSVG_MAX_ATTRIBUTES KAtrAudioLevel
#define KSVG_ATTRIBUTE_NOT_SUPPORTED  KSVG_MAX_ATTRIBUTES+1

    // special Ids for Animation attribute Values

    const TUint16 KAnimFreeze                  = 110 + KSvgOffset;

	// special Id for Embedded image XLink:href attribute .

	const TUint16 KXlinkhrefImageEmbedded	   = 111 + KSvgOffset;

// special Id for multiple end times.Used for the Binary compatibility of the encoder

	const TUint16 KMultipleEndTime	   = 112 + KSvgOffset;
    const TUint16 KAtrMediaAnimationId = 113 + KSvgOffset;  

    const TUint16 KAtrAudioId           = 114 + KSvgOffset;


    #define     KSVG_ANIMATE_ELEMFLAG           KAtrAttributeName
    #define     KSVG_SET_ELEMFLAG               KAtrAttributeName
    #define     KSVG_ANIMATETRANSFORM_ELEMFLAG  KAtrSVGTrf
    #define     KSVG_CIRCLE_ELEMFLAG            KAtrR
    #define     KSVG_ELLIPSE_ELEMFLAG           KAtrSVGElp
    #define     KSVG_RECT_ELEMFLAG              KAtrSVGRec
    #define     KSVG_IMAGE_ELEMFLAG             KAtrSVGRec
    #define     KSVG_HKERN_ELEMFLAG             KAtrK
    #define     KSVG_PATH_ELEMFLAG              KAtrD
    #define     KSVG_POLYLINE_ELEMFLAG          KAtrPoints
    #define     KSVG_AMINATEMO_ELEMFLAG         KAtrSVGAmo

    const TUint16 KAtrSVGRec                = 120 + KSvgOffset;
    const TUint16 KAtrSVGElp                = 121 + KSvgOffset;
    const TUint16 KAtrSVGTrf                = 122 + KSvgOffset;
    const TUint16 KAtrSVGAmo                = 123 + KSvgOffset;
    const TUint16 KAtrToBy                  = 124 + KSvgOffset;
	const TUint16 KAtrAdditiveSet			= 125 + KSvgOffset;



const TUint16 KTiny = 0;

// Display Enumeration Constants. take care
const TUint KDisplayEnumNone = 16;
const TUint KPresentationAttrDisplay = 10;

const TUint16 KAtrUnicodeRange = 135 + KSvgOffset;
const TUint16 KAtrValues = 138 + KSvgOffset;


// SPECIAL (APPLICATION DEFINED) ATTRIBUTE CONSTANTS
const TUint16 KAtrData = KAtrD; // So KAtrData can still be used
const TUint16 KAtrRadius = KAtrR; // So KAtrRadius can still be used

const TUint16 KAtrRefX = 1001;
const TUint16 KAtrRefY = 1002;
const TUint16 KAtrAnimMotionMatrixIndex = 5000;
const TUint16 KAtrAnimTransformMatrixIndex = 5001;

//error values added for forwward reference support in animation and use elements
const TInt KErrReferencedElementNotFound = -1000;
const TInt KErrAnimateReferenceElementNotFound = -1001;

// start of new element in encoding.
const TUint16 KStartNewElem = 1000;

// SVG TINY DEFINED ENUMERATIONS
const TUint16 KEnumPercent = 0; // '%' sign
const TUint16 KEnum100 = 1;
const TUint16 KEnum200 = 2;
const TUint16 KEnum300 = 3;
const TUint16 KEnum400 = 4;
const TUint16 KEnum500 = 5;
const TUint16 KEnum600 = 6;
const TUint16 KEnum700 = 7;
const TUint16 KEnum800 = 8;
const TUint16 KEnum900 = 9;
const TUint16 KEnumAlways = 10;
const TUint16 KEnumAuto = 11;
const TUint16 KEnumBevel = 12;
const TUint16 KEnumBlink = 13;
const TUint16 KEnumBlock = 14;
const TUint16 KEnumBold = 15;
const TUint16 KEnumBolder = 16;
const TUint16 KEnumButt = 17;
const TUint16 KEnumCdata = 18;
const TUint16 KEnumCm = 19;
const TUint16 KEnumCompact = 20;
const TUint16 KEnumCondensed = 21;
const TUint16 KEnumCss = 22;
const TUint16 KEnumDasharray = 23;
const TUint16 KEnumDisable = 24;
const TUint16 KEnumDiscrete = 25;
const TUint16 KEnumEnd = 26;
const TUint16 KEnumEvenodd = 27;
const TUint16 KEnumExpanded = 28;
const TUint16 KEnumExtraCondensed = 29;
const TUint16 KEnumExtraExpanded = 30;
const TUint16 KEnumFreeze = 31;
const TUint16 KEnumHidden = 32;
const TUint16 KEnumIn = 33;
const TUint16 KEnumInherit = 34;
const TUint16 KEnumInline = 35;
const TUint16 KEnumInlineTable = 36;
const TUint16 KEnumInterger = 37;
const TUint16 KEnumItalic = 38;
const TUint16 KEnumLenght = 39;
const TUint16 KEnumLighter = 40;
const TUint16 KEnumLinear = 41;
const TUint16 KEnumLineThrough = 42;
const TUint16 KEnumListItem = 43;
const TUint16 KEnumMagnify = 44;
const TUint16 KEnumMarker = 45;
const TUint16 KEnumMiddle = 46;
const TUint16 KEnumMiter = 47;
const TUint16 KEnumMm = 48;
const TUint16 KEnumNarrower = 49;
const TUint16 KEnumNever = 50;
const TUint16 KEnumNone = 51;
const TUint16 KEnumNonzero = 52;
const TUint16 KEnumNormal = 53;
const TUint16 KEnumOblique = 54;
const TUint16 KEnumOverline = 55;
const TUint16 KEnumPaced = 56;
const TUint16 KEnumPc = 57;
const TUint16 KEnumPt = 58;
const TUint16 KEnumPx = 59;
const TUint16 KEnumRemove = 60;
const TUint16 KEnumReplace = 61;
const TUint16 KEnumRotate = 62;
const TUint16 KEnumRound = 63;
const TUint16 KEnumRunIn = 64;
const TUint16 KEnumScale = 65;
const TUint16 KEnumSemiCondensed = 66;
const TUint16 KEnumSemiExpanded = 67;
const TUint16 KEnumSkewX = 68;
const TUint16 KEnumSkewY = 69;
const TUint16 KEnumSpline = 70;
const TUint16 KEnumSquare = 71;
const TUint16 KEnumStart = 72;
const TUint16 KEnumSum = 73;
const TUint16 KEnumTable = 74;
const TUint16 KEnumTableCaption = 75;
const TUint16 KEnumTableCell = 76;
const TUint16 KEnumTableColumn = 77;
const TUint16 KEnumTableColumnGroup = 78;
const TUint16 KEnumTableFooterGroup = 79;
const TUint16 KEnumTableHeaderGroup = 80;
const TUint16 KEnumTableRow = 81;
const TUint16 KEnumTableRowGroup = 82;
const TUint16 KEnumTranslate = 83;
const TUint16 KEnumUltraCondensed = 84;
const TUint16 KEnumUltraExpanded = 85;
const TUint16 KEnumUnderline = 86;
const TUint16 KEnumVisible = 87;
const TUint16 KEnumWhenNotActive = 88;
const TUint16 KEnumWider = 89;
const TUint16 KEnumXMaxYMax = 90;
const TUint16 KEnumXMaxYMid = 91;
const TUint16 KEnumXMaxYMin = 92;
const TUint16 KEnumXMidYMax = 93;
const TUint16 KEnumXMidYMid = 94;
const TUint16 KEnumXMidYMin = 95;
const TUint16 KEnumXMinYMax = 96;
const TUint16 KEnumXMinYMid = 97;
const TUint16 KEnumXMinYMin = 98;
const TUint16 KEnumXml = 99;

// Temporary
const TInt KSvgTinyArraySize = 210;

/* List of Elements */


//const TUint8 KSvgAnimElemsStartIndex    = 34;
//const TUint8 KSvgAnimElemsEndIndex    = 38;

const TUint8 KSvgNoBoundBoxElemsStartIndex    = 0;
const TUint8 KSvgNoBoundBoxElemsEndIndex    = 18;

const TUint8 KSvgAllGElemsStartIndex    = 3;
const TUint8 KSvgAllGElemsSubStartIndex    = 4;
const TUint8 KSvgAllGElemsEndIndex    = 7;

/* Media Elements bound indexes */
const TUint8 KSvgMediaElemsStartIndex    = 47;
const TUint8 KSvgMediaElemsEndIndex      = 48;

const TUint8 KSvgSvgElement           = 0;

const TUint8 KSvgAltglyphElement      = 1;
const TUint8 KSvgAltglyphdefElement   = 2;

const TUint8 KSvgDefsElement          = 3;
const TUint8 KSvgDescElement          = 4;

const TUint8 KSvgMetadataElement      = 6;
const TUint8 KSvgTitleElement         = 7;

const TUint8 KSvgFontfacenameElement  = 8;
const TUint8 KSvgFontfacesrcElement   = 9;
const TUint8 KSvgFontfaceuriElement   = 10;
const TUint8 KSvgGElement             = 11;
const TUint8 KSvgGlyphrefElement      = 12;
const TUint8 KSvgVkernElement         = 13;
// Moved to end of element type list to use its own class
// Types 1-16 uses CSvgGElementImpl as container
// const TUint8 KSvgScriptElement        = 14;
const TUint8 KSvgSwitchElement        = 15;

const TUint8 KSvgViewElement          = 16;

const TUint8 KSvgHkernElement         = 17;

const TUint8 KSvgAElement             = 18;
const TUint8 KSvgFontElement          = 19;
const TUint8 KSvgFontfaceElement      = 20;
const TUint8 KSvgGlyphElement         = 21;
const TUint8 KSvgImageElement         = 22;
const TUint8 KSvgMissingglyphElement  = 23;
const TUint8 KSvgStyleElement         = 24;
const TUint8 KSvgTextElement          = 25;
const TUint8 KSvgUseElement           = 26;
const TUint8 KSvgCircleElement        = 27;
const TUint8 KSvgEllipseElement       = 28;
const TUint8 KSvgLineElement          = 29;
const TUint8 KSvgPathElement          = 30;
const TUint8 KSvgPolygonElement       = 31;
const TUint8 KSvgPolylineElement      = 32;
const TUint8 KSvgRectElement          = 33;
const TUint8 KSvgAnimateElement       = 34;
const TUint8 KSvgAnimateColorElement  = 35;
const TUint8 KSvgAnimateMotionElement = 36;
const TUint8 KSvgAnimateTransformElement = 37;
const TUint8 KSvgSetElement           = 38;
const TUint8 KSvgMpathElement         = 39;
const TUint8 KSvgLinearGradientElement = 40;
const TUint8 KSvgRadialGradientElement = 41;
const TUint8 KSvgStopElement          = 42;
const TUint8 KSvgScriptElement        = 43;
const TUint8 KSvgTextAreaElement      = 44;
const TUint8 KSvgDiscardElement          = 45;
const TUint8 KSvgSolidColorElement    = 46;
//const TUint8 KSvgAnimationElement          = 47;
const TUint8 KSvgMediaAnimationElement          = 47;
const TUint8 KSvgAudioElement          = 48;
const TUint8 KSvgForeignObjectElement = 49;

// signs the present of Cdata
const TUint8 KCDataPresent         = 253;
// end of element index
const TUint8 KEndElemIndex         = 254;
const TUint8 KEndSvgFile           = 255;




/* List of Data Types */
const TInt KSvgTypeInteger      = 0;
const TInt KSvgTypeNumber       = 1;
const TInt KSvgTypeColor        = 2;
const TInt KSvgTypeLength       = 3;
//const TInt KSvgTypeStr          = 4;
//const TInt KSvgTypeTime         = 5;
//const TInt KSvgTypeCor          = 6;

const TInt KSvgTypePath          = 7;
const TInt KSvgTypeId            = 8;
const TInt KSvgTypeUriId         = 9;
const TInt KSvgTypeValue         = 10;
const TInt KSvgTypeRDur          = 11;
const TInt KSvgTypeRCount        = 12;
const TInt KSvgTypeList          = 13;
const TInt KSvgTypeStyle         = 14;
const TInt KSvgTypeViewBox       = 15;
const TInt KSvgTypePAR           = 16;
const TInt KSvgTypeCorl          = 17;


/* Transform Types */

const TInt KSvgTypeTransform     = 30;
const TInt KSvgTypeTranslate     = 31;
const TInt KSvgTypeRotate        = 32;
const TInt KSvgTypeScale         = 33;
const TInt KSvgTypeSkewX         = 34;
const TInt KSvgTypeSkewY         = 35;
const TInt KSvgType2DMatrix      = 36;

/* Only Enumarated SVGT ATTRIBUTES */
const TInt KSvgTypeEnum          = 49;
const TInt KSvgTypeAccumulate    = 50;
const TInt KSvgTypeAdditive      = 51;
const TInt KSvgTypeCalcMode      = 52;
const TInt KSvgTypeDisplay       = 53;
const TInt KSvgTypeFill          = 54;
const TInt KSvgTypeFillRule      = 55;
const TInt KSvgTypeFontFamily    = 56;
const TInt KSvgTypeFontSize      = 57;// Break in series due to redefinition originally
const TInt KSvgTypeFontStyle     = 59;
const TInt KSvgTypeFontWeight    = 60;
const TInt KSvgTypeRestart       = 61;
const TInt KSvgTypeStrokeDasharray  = 62;
const TInt KSvgTypeStrokeDashoffset = 63;

const TInt KSvgTypeStrokeLinecap = 64;
const TInt KSvgTypeStrokeLinejoin= 65;
const TInt KSvgTypeStrokeMiterlimit = 66;
const TInt KSvgTypeStrokeWidth   = 67;
const TInt KSvgTypeType          = 68;
const TInt KSvgTypeVisibility    = 69;
const TInt KSvgTypeZoomAndPan    = 70;
const TInt KSvgTypeAttributeType = 71;
const TInt KSvgTypeTo            = 72;
const TInt KSvgTypeTextAnchor    = 73;
const TInt KSvgTypeTextDecoration= 74;
const TInt KSvgTypeAlignmentBaseline = 75;
const TInt KSvgTypeBaselineShift = 76;
const TInt KSvgTypeClipRule      = 77;
const TInt KSvgTypeColorInterpolation = 78;
const TInt KSvgTypeCursor        = 79;
const TInt KSvgTypeDirection     = 80;
const TInt KSvgTypeDominantBaseline   = 81;
const TInt KSvgTypeFontStretch   = 82;
const TInt KSvgTypeFontVariant   = 83;
const TInt KSvgTypeImageRendering= 84;
const TInt KSvgTypeOverflow      = 85;
const TInt KSvgTypePointerEvents = 86;
const TInt KSvgTypeShapeRendering= 87;
const TInt KSvgTypeTextRendering = 88;
const TInt KSvgTypeUnicodeBidi   = 89;
const TInt KSvgTypeWritingMode   = 90;
const TInt KSvgTypeMaskUnits     = 91;
const TInt KSvgTypeMode          = 92;
const TInt KSvgTypeOperator      = 93;
const TInt KSvgTypeMethod        = 94;
const TInt KSvgTypeSpacing       = 95;
const TInt KSvgTypeRenderingIntent= 96;
const TInt KSvgTypeSpreadMethod  = 97;
const TInt KSvgTypeOpacity  = 98;
const TInt KSvgTypeGradientUnits = 99;




#define KCSS_ATTR_FILL                       0
#define KCSS_ATTR_STROKE                     1
#define KCSS_ATTR_STROKEWIDTH                2
#define KCSS_ATTR_VISIBILITY                 3
#define KCSS_ATTR_FONTFAMILY                 4
#define KCSS_ATTR_FONTSIZE                   5
#define KCSS_ATTR_FONTSTYLE                  6
#define KCSS_ATTR_FONTWEIGHT                 7
#define KCSS_ATTR_STROKE_DASHARRAY           8
#define KCSS_ATTR_DISPLAY                   9
#define KCSS_ATTR_FILLRULE                  10
#define KCSS_ATTR_STROKE_LINECAP            11
#define KCSS_ATTR_STROKE_LINEJOIN           12
#define KCSS_ATTR_STROKE_DASHOFFSET         13
#define KCSS_ATTR_STROKE_MITERLIMIT         14
#define KCSS_ATTR_COLOR                     15
#define KCSS_ATTR_TEXTANCHOR                16
#define KCSS_ATTR_TEXTDECORATION            17
#define KCSS_ATTR_COLORINTERPOLATION        18
#define KCSS_ATTR_COLORRENDERING            19
#define KCSS_ATTR_LETTERSPACING             20
#define KCSS_ATTR_WORDSPACING               21

#define KCSS_ATTR_FILL_OPACITY              22
#define KCSS_ATTR_STROKE_OPACITY            23
#define KCSS_ATTR_GROUP_OPACITY             24

#define KCSS_ATTR_FONT                      25
#define KCSS_MAX_ATTR                       25

const TInt	KInherit = 5;
// number of color names supported by SVGT
const TInt KMaxColors = 17;

enum TAttributeType
    {
    EInteger = 0,
    EFloat,
    EString,
    EList,
    EUndefinedType
    };


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgSchemaData : public CBase
    {
    private:


       /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                                ConstructL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                            CSvgSchemaData();
    public:

         /**
         * Need method description - GetSVGTAttributeId
         *
         * @since 1.0
         * @param aAttrName
         * @return
         */
        static TUint16 GetSVGTAttributeId(const TDesC& aAttrName);

         /**
         * Need method description - GetSVGElementId
         *
         * @since 1.0
         * @param aElement
         * @return
         */
        static TInt GetSVGElementId(const TDesC& aElement);

         /**
         * Need method description - GetPresentationAttributeId
         *
         * @since 1.0
         * @param aElement
         * @return
         */
        static TInt GetPresentationAttributeId(const TDesC& aElement);

         /**
         * Get the type of the given attribute string
         *
         * @since 1.0
         * @param aAttrName
         * @return type for attribute-name
         */
        static TAttributeType GetAttributeType( const TDesC& aAttrName );

         /**
         * Need method description - FindSpreadMethod
         * returns the integer enum value for the spread method type
         * @since 1.0
         * @param aElement
         * @return
         */
        TInt FindSpreadMethod(const TDesC& aElement);
        /**
         * Need method description - FindFontStyle
         *
         * @since 1.0
         * @param aElement
         * @return
         */
        TInt FindFontStyle(const TDesC& aElement);

         /**
         * Need method description - FindFontWeightEnum
         *
         * @since 1.0
         * @param aElement
         * @return
         */
        TInt FindFontWeightEnum(const TDesC& aElement);
         /**
         * Need method description - FindTextAnchorEnum
         *
         * @since 1.0
         * @param aElement
         * @return
         */
        TInt FindTextAnchorEnum(const TDesC& aElement);

         /**
         * Need method description - FindTextDecorationEnum
         *
         * @since 1.0
         * @param aElement
         * @return
         */
        TInt FindTextDecorationEnum(const TDesC& aElement);


        // Accessors

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgSchemaData*              NewL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgSchemaData*              NewLC();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                            ~CSvgSchemaData();

        void                  GetSVGElementName(const TUint8 aElement, TDes& aElemName);



};
#endif



