/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Engine header file
 *
*/


 #ifndef _INC_SVGJAVAINTERFACEIMPL_
 #define _INC_SVGJAVAINTERFACEIMPL_

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include <SVGEngineInterfaceImpl.h>
#include <SVGListener.h>

typedef int SvgEngineHandle;
typedef int SvgDocumentHandle;
typedef int SvgElementHandle;
typedef int SvgSurfaceHandle;
typedef int SvgPathHandle;
typedef short SvgAttrType;

typedef unsigned int SvgtBitmapHandle;

class CSvgEngineImpl;
class CSvgDocumentImpl;
class CFbsBitmap;

// ***************************************************/
//Find a way to get these values from SvgEngine and defined here in JavaInterface.
const TInt KSvgUnknownElement       = -1;

const TInt KSvgSvgElement           = 0;

const TInt KSvgAltglyphElement      = 1;
const TInt KSvgAltglyphdefElement   = 2;

const TInt KSvgDefsElement          = 3;
const TInt KSvgDescElement          = 4;

const TInt KSvgMetadataElement      = 6;
const TInt KSvgTitleElement         = 7;

const TInt KSvgFontfacenameElement  = 8;
const TInt KSvgFontfacesrcElement   = 9;
const TInt KSvgFontfaceuriElement   = 10;
const TInt KSvgGElement             = 11;
const TInt KSvgGlyphrefElement      = 12;
const TInt KSvgVkernElement         = 13;
// const TInt KSvgScriptElement        = 14;
const TInt KSvgSwitchElement        = 15;

const TInt KSvgViewElement          = 16;

const TInt KSvgHkernElement         = 17;

const TInt KSvgAElement             = 18;
const TInt KSvgFontElement          = 19;
const TInt KSvgFontfaceElement      = 20;
const TInt KSvgGlyphElement         = 21;
const TInt KSvgImageElement         = 22;
const TInt KSvgMissingglyphElement  = 23;
const TInt KSvgStyleElement         = 24;
const TInt KSvgTextElement          = 25;
const TInt KSvgUseElement           = 26;
const TInt KSvgCircleElement        = 27;
const TInt KSvgEllipseElement       = 28;
const TInt KSvgLineElement          = 29;
const TInt KSvgPathElement          = 30;
const TInt KSvgPolygonElement       = 31;
const TInt KSvgPolylineElement      = 32;
const TInt KSvgRectElement          = 33;
const TInt KSvgAnimateElement       = 34;
const TInt KSvgAnimateColorElement  = 35;
const TInt KSvgAnimateMotionElement = 36;
const TInt KSvgAnimateTransformElement = 37;
const TInt KSvgSetElement           = 38;
const TInt KSvgMpathElement         = 39;
const TInt KSvgLinearGradientElement = 40;
const TInt KSvgRadialGradientElement = 41;
const TInt KSvgStopElement           = 42;
const TInt KSvgScriptElement        = 43;
const TInt KSvgTextAreaElement      = 44;
const TInt KSvgDiscardElement       = 45;
const TInt KSvgSolidColorElement    = 46;
const TInt KSvgAnimationElement     = 47;
const TInt KSvgVideoElement         = 48;
const TInt KSvgForeignObjectElement = 49;


/*************************************************/
/*************************************************/
	const TInt KSvgUnknownAttribute = -1;
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
		const TUint16 KAtrTarget = 100 + KSvgOffset;



#define KSVG_MAX_ATTRIBUTES KAtrTarget
#define KSVG_ATTRIBUTE_NOT_SUPPORTED  KSVG_MAX_ATTRIBUTES+1

    // special Ids for Animation attribute Values

    const TUint16 KAnimFreeze                  = 110 + KSvgOffset;

	// special Id for Embedded image XLink:href attribute . 

	const TUint16 KXlinkhrefImageEmbedded	   = 111 + KSvgOffset;

// special Id for multiple end times.Used for the Binary compatibility of the encoder 

	const TUint16 KMultipleEndTime	   = 112 + KSvgOffset;



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
#define KCSS_ATTR_FONT                      24
/***************************************************/
/***************************************************/
const TInt KSvgTypeTransform     = 30;
const TInt KSvgTypeTranslate     = 31;
const TInt KSvgTypeRotate        = 32;
const TInt KSvgTypeScale         = 33;
const TInt KSvgTypeSkewX         = 34;
const TInt KSvgTypeSkewY         = 35;
const TInt KSvgType2DMatrix      = 36;

/////////////EVENT MASKS////////////////////
const TUint8    KSvgEventMaskNone           = 0x0;
const TUint8    KSvgEventMaskExternalUI     = 0x1;
const TUint8    KSvgEventMaskTimer          = 0x2;
const TUint8    KSvgEventMaskInternal       = 0x4;
const TUint8    KSvgEventMaskExternalSystem = 0x8;
////////////////////////////////////////////////// taken from event.h




////////////////////////////////////////////////// taken from event.h
// these are taken from gfxgeneralPath.h
const TUint32 KSvgSegMoveTo   = 2;
const TUint32 KSvgSegLineTo  = 4;
const TUint32 KSvgSegQuadTo  = 10;
const TUint32 KSvgSegCubicTo = 12;
const TUint32 KSvgSegClose  = 0;
const TUint32 KSvgHorizontal = 5;
const TUint32 KSvgVertical   = 6;
/////////////////////////////////////////////////////

class MJavaError
    {
    public:
         /**
         * Dtor
         * @since 1.0
         */
         virtual ~MJavaError()
         	{
         	}

         /**
         * Determine wether an error is indicated by this object.
         *
         * @since 1.0
         * @return : ETrue, if ErrorCode() != ESvgNoError
         */
        virtual TBool HasError() const = 0;

        /**
         * Determine wether an error is only a warning.
         * This should be a state when the svg may be display, despite
         * a conflict.
         *
         * @since 1.0
         * @return ETrue if HasError() is only a warning.
         */
        virtual TBool IsWarning() const = 0;

        /**
         * Get the error code contained by this object.
         *
         * @since 1.0
         * @return : the TSvgErrorCode value
         */
        virtual TInt ErrorCode() const = 0;

        /**
         * Get the error description string for this object.
         *
         * @since 1.0
         * @return : error description string.
         */
        virtual TDesC8& Description() = 0;
    };


/**
 * This class implements the interface for a MJavaError object, which contains
 * an error code and a description of the error if one exists.
 *
 *  @since 1.0
 */
class CJavaError : public CBase, public MJavaError
    {
    public:

        /**
         * Two-phase constructor.
		 * Creates an integer 'handle' from C++ object for referencing them inside Java.
		 * The shift garauntees a positive integer.
		 * Unhanding the integer requires the destination type to be known and bit shifting.
         *  
	     * @since 1.0
         * @return : CJavaError object
         */
        static TInt NewL( CSvgErrorImpl& aError );

        /**
         * Destructor
         */
         virtual ~CJavaError();

        /**
         * Determine wether an error is indicated by this object.
         *
         * @since 1.0
         * @return : ETrue, if ErrorCode() != ESvgNoError
         */
        virtual TBool HasError() const;

        /**
         * Determine wether an error is only a warning.
         * This should be a state when the svg may be display, despite
         * a conflict.
         *
         * @since 1.0
         * @return ETrue if HasError() is only a warning.
         */
        virtual TBool IsWarning() const;

        /**
         * Get the error code contained by this object.
         *
         * @since 1.0
         * @return : the TSvgErrorCode value
         */
        virtual TInt ErrorCode() const;

        /**
         * Get the error description string for this object.
         *
         * @since 1.0
         * @return : error description string.
         */
        virtual TDesC8& Description();

    protected:
        /**
         * Second phase of constructor
         *
         * @since 1.0
         */
        void ConstructL( CSvgErrorImpl& aError );

        /**
         * Ctor.
		 *
         * @since 1.0
         */
        CJavaError();

    private:
        TInt iErrorCode;
        TBool iIsWarning;
        HBufC8* iDescription;
    };

// ***********************************************************************
// SVG Engine
// ***********************************************************************
/**
 * The SVG Engine is fundamental object for 2D rendering. The rendering can 
 * only be achieved through the render method provided by the SVG Engine object.
 * NOTE: Java MIDlet can uses multiple SVG Engine objects.
 * NOTE: Java side cannot provide the SVG Engine object for:
 * <ul>
 * <li>the SVG Document operations</li>
 * <li>the SVG Element operations</li>
 * </ul>
 */

// *****************************************************************************
// SVG Rendering Surface Functions
// *****************************************************************************
/**
 * The SVG Rendering Surface functions are not used in java side.
 *

// ***********************************************************************
// SVG Document
// ***********************************************************************
/**
 * The SVG Document represents an XML Document.
 * <p>The SVG Document is a subset of the Document interface defined
 * in the <a href="http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/idl-definitions.html">
 * DOM Level 3 Core</a>.</p>
 */

// ***********************************************************************
// SVG Element
// ***********************************************************************
/**
 * Description of an SVG element in the document tree. Element's id can be set only
 * if it does not already have an id. Elements with non-null id can be inserted, but 
 * <b>cannot be removed</b> from the DOM tree.
 *
 * <p>
 * An SVG element's "traits" value can be read and manipulated. Each <i>trait</i> 
 * corresponds to an attribute or property,which is parsed and understood by the element 
 * and in most cases animatable. Unlike attributes, each element has a
 * well-defined set of traits and attempting to access undefined trait is an error. Also unlike
 * attributes traits are typed and their values are normalized; for instance SVG path specification
 * is parsed and all path commands are converted to their absolute variants, it is not possible to
 * say through the value of the trait if a path command was absolute or relative. When getting and
 * setting trait values, accessor of the correct type must be used or exception will be thrown.
 * </p>
 * <p>
 * Initial trait values come from parsing corresponding attributes. If value is not specified, but
 * corresponing attribute (or property for environments where styling is supported) is inherited,
 * inherited value is returned as a result of the trait query method. If it is not inherited, default
 * value is returned. Default values are also returned in the case when there is no parent to inherit
 * from, for ex: when you create a new element, set a trait value to 'inherit', but there is no parent for
 * inheritance. It is important to note that the value which is returned is always a base value (i.e. before animation
 * is applied), and this is true for both static and animated content.
 * </p>
 * <p>
 * Setting a trait value has the same effect as changing a corresponding attribute, but trait
 * setters can operate on typed values. The value which is modified is always a base value.
 * For inheritable traits the trait value can always be set to "inherit"
 * (but querying the value will always return the actual inherited value as explained above).
 * </p>
 *
 * <h3>Traits supported in this specification, SVG Tiny 1.1 DOM</h3>
 *
 * <p>The table below shows the list of attributes and properties that SVG Tiny
 * DOM 1.1 implementations must support. Each light gray section lists one or
 * multiple elements for which the subsequent attributes or properties
 * apply. Each attribute row lists the allowed getter and setter (s). The last
 * column specifies the default values that must be used for each attribute or
 * property.</p>
 * <p><b>Note:</b> For 'REQUIRED' attributes, there are two cases:
 *
 * <ul>
 *  <li>i) The document is in error, if this attribute was not present at the time of loading.</li>
 *  <li>ii) When using uDOM API, the specified default value (in parenthesis) must be used.</li>
 * </ul>
 * </p>
 *
 * <table height="1586" cellpadding="2" cellspacing="2" border="1"
 *  width="825" style="border-collapse: collapse;">
 *   <tbody>
 *     <tr>
 *       <th width="150" valign="top" bgcolor="#999999"><b>Property<br>
 *       </b> </th>
 *       <th valign="top" bgcolor="#999999"><b>Trait Getter <br>[possible return value(s)]<br>
 *       </b> </th>
 *       <th bgcolor="#999999" valign="top"><b>Trait Setter <br>[allowed value(s)]<br>
 *       </b> </th>
 *       <th bgcolor="#999999" valign="top"><b>Default Values<br>
 *       </b> </th>
 *     </tr>
 *     <tr>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td bgcolor="#cccccc" colspan="4" rowspan="1" valign="top">&lt;svg&gt;,
 * 	&lt;rect&gt;, &lt;circle&gt;, &lt;ellipse&gt;, &lt;line&gt;,
 * 	&lt;path&gt;, &lt;g&gt;, &lt;image&gt;, &lt;text&gt;, &lt;a&gt;, and &lt;use&gt;</td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">color <br>
 *       </td>
 *       <td valign="top">getRGBColorTrait [SVGRGBColor]<br>
 *       </td>
 *       <td valign="top">setTrait [inherit]<br>setRGBColorTrait [SVGRGBColor]<br>
 *       </td>
 *       <td valign="top" align="center">rgb(0,0,0)<br>
 *      </td>
 *    </tr>
 *      <tr>
 *       <td width="150" valign="top">display<br>
 *       </td>
 *       <td valign="top">getTrait [inline | none ] <br>
 *       </td>
 *       <td valign="top">setTrait [inline | none | inherit ] </td>
 *       <td valign="top" align="center">"inline"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">fill<br>
 *       </td>
 *       <td valign="top">getRGBColorTrait [null, SVGRGBColor]<br>
 *       </td>
 *       <td valign="top">setRGBColorTrait [SVGRGBColor]<br>setTrait(none | currentColor | inherit)<br>
 *       </td>
 *       <td valign="top" align="center">rgb(0,0,0)<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">fill-rule<br>
 *       </td>
 *       <td valign="top">getTrait [nonzero | evenodd] <br>
 *       </td>
 *       <td valign="top">setTrait [nonzero | evenodd | inherit] </td>
 *       <td valign="top" align="center">"nonzero"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">stroke</td>
 *      <td valign="top">getRGBColorTrait [null, SVGRGBColor]</td>
 *       <td valign="top">setRGBColorTrait [SVGRGBColor]<br>setTrait [none | currentColor | inherit]</td>
 *       <td valign="top" align="center">"none"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">stroke-dashoffset</td>
 *       <td valign="top">getFloatTrait </td>
 *       <td valign="top">setTrait [inherit]<br>setFloatTrait </td>
 *       <td valign="top" align="center">0.0f<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">stroke-linecap</td>
 *       <td valign="top">getTrait [butt | round | square]</td>
 *       <td valign="top">setTrait [butt | round | square | inherit]</td>
 *       <td valign="top" align="center">"butt"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">stroke-linejoin</td>
 *       <td valign="top">getTrait [miter | round | bevel ]</td>
 *       <td valign="top">setTrait [miter | round | bevel | inherit]</td>
 *       <td valign="top" align="center">"miter"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">stroke-miterlimit</td>
 *       <td valign="top">getFloatTrait [ value &gt;= 1]</td>
 *       <td valign="top">setTrait [inherit]<br>setFloatTrait [value &gt;= 1]</td>
 *       <td valign="top" align="center">4.0f<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">stroke-width</td>
 *       <td valign="top">getFloatTrait [value &gt;= 0]</td>
 *       <td valign="top">setTrait [inherit]<br> setFloatTrait [value &gt;= 0]</td>
 *       <td valign="top" align="center">1.0f<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">visibility</td>
 *       <td valign="top">getTrait [visible | hidden]</td>
 *       <td valign="top">setTrait [visible | hidden | inherit]</td>
 *       <td valign="top" align="center">"visible"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td bgcolor="#cccccc" colspan="4" rowspan="1" valign="top">&lt;svg&gt;, &lt;text&gt;, &lt;g&gt;, &lt;a&gt, and &lt;use&gt;;<br>
 *      </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">font-family<br>
 *       </td>
 *       <td valign="top">getTrait [single, computed font-family value]<br>
 *      </td>
 *      <td valign="top">setTrait [same syntax as font-family attribute]<br>
 *     </td>
 *      <td valign="top" align="center"> User-Agent <br>
 *      </td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">font-size<br>
 *       </td>
 *       <td valign="top">getFloatTrait&nbsp; [value &gt;= 0]<br>
 *     </td>
 *       <td valign="top">setFloatTrait [value &gt;= 0]<br>setTrait [inherit]<br>
 *      </td>
 *       <td valign="top" align="center"> User-Agent <br>
 *       </td>     </tr>
 *     <tr>
 *       <td width="150" valign="top">font-style<br>
 *       </td>
 *      <td valign="top">getTrait [normal | italic | oblique ] </td>
 *       <td valign="top">setTrait [normal | italic | oblique | inherit] </td>
 *       <td valign="top" align="center">"normal"<br>
 *       </td>     </tr>
 *     <tr>
 *       <td width="150" valign="top">font-weight<br>
 *       </td>
 *       <td valign="top">getTrait [100 | 200 | 300 <br> | 400 | 500 | 600 | 700 | 800 | 900 ] </td>
 *       <td valign="top">setTrait [normal | bold | bolder | lighter | 100 | 200 | 300 <br />
 *       | 400 | 500 | 600 | 700 | 800 | 900 | inherit] </td>
 *       <td valign="top" align="center">"normal"<br>
 *       </td>     </tr>
 *     <tr>
 *       <td width="150" valign="top">text-anchor<br>
 *       </td>
 *       <td valign="top">getTrait [start | middle | end]<br>
 *       </td>
 *       <td valign="top">setTrait [start | middle | end | inherit ]<br>
 *       </td>
 *       <td valign="top" align="center">"start"<br>
 *       </td>     </tr>
 *     <tr>
 *       <td width="150" colspan="4" rowspan="1" valign="top"><br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td width="150" bgcolor="#999999" valign="top"><b>Attribute<br>
 *       </b></td>
 *       <td bgcolor="#999999" valign="top"><b>Trait Getter<br>
 *       </b></td>
 *       <td bgcolor="#999999" valign="top"><b>Trait Setter<br>
 *       </b></td>
 *       <td bgcolor="#999999" valign="top"><b>Default Values<br>
 *       </b></td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td width="150" bgcolor="#cccccc" colspan="4" rowspan="1" valign="top">
 *       &lt;rect&gt;, &lt;circle&gt;, &lt;ellipse&gt;, &lt;line&gt;, &lt;path&gt;, &lt;g&gt;,
 *       &lt;image&gt;, &lt;text&gt;, &lt;a&gt;, and &lt;use&gt;</td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">transform<br>
 *       </td>
 *       <td valign="top">getMatrixTrait [SVGMatrix]<br>
 *       </td>
 *      <td valign="top">setMatrixTrait [SVGMatrix]<br>
 *       </td>
 *       <td valign="top" align="center">Identity matrix<br>
 *       (1,0,0,1,0,0)<br>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;rect&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">height<br>
 *      </td>
 *      <td valign="top">getFloatTrait [ value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [ value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">width<br>
 *      </td>
 *      <td valign="top">getFloatTrait [ value &gt;= 0]</td>
 *      <td valign="top">setFloatTrait [ value &gt;= 0]</td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">rx<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]</td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]</td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">ry<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;circle&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">cx<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">cy<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">r<br>
 *      </td>
 *      <td valign="top">getFloatTrait [ value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" bgcolor="#cccccc" colspan="4" rowspan="1" valign="top">&lt;ellipse&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">cx<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">cy<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">rx<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">ry<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;line&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x1<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x2<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y1<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y2<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;path&gt; (path-length is not supported)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">d<br>
 *      </td>
 *      <td valign="top">getPathTrait [SVGPath]<br>
 *      </td>
 *      <td valign="top">setPathTrait [SVGPath]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(Empty SVGPath)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;image&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x<br>
 *      </td>
 *      <td valign="top">getFloatTrait <br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">width<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">height<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">xlink:href<br>
 *      </td>
 *      <td valign="top">getTrait NS[absolute URI]<br>
 *      </td>
 *      <td valign="top">setTraitNS [non local-URI value]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>( "" )<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;use&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td valign="top">xlink:href<br>
 *      </td>
 *      <td valign="top">getTraitNS[absolute URI]<br>
 *      </td>
 *      <td valign="top">setTraitNS<br>
 *      </td>
 *      <td valign="top" align="center">""<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;a&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">target<br>
 *      </td>
 *      <td valign="top">getTrait<br>
 *      </td>
 *      <td valign="top">setTrait<br>
 *      </td>
 *      <td valign="top" align="center">""<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td valign="top">xlink:href<br>
 *      </td>
 *      <td valign="top">getTraitNS[absolute URI]<br>
 *      </td>
 *      <td valign="top">setTraitNS<br>
 *      </td>
 *      <td valign="top" align="center">""<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="*" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;text&gt;<br />(Notes:
 * For 'x' and 'y', it is only possible
 * to provide floating point scalar values; an array of x or y values is not supported. <br />
 * 'rotate' attribute is not supported.)<br/>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">#text<br>
 *      </td>
 *      <td valign="top">getTrait [not null]<br>
 *      </td>
 *      <td valign="top">setTrait [not null]<br>
 *      </td>
 *      <td valign="top" align="center">""<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;svg&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">version<br>
 *      </td>
 *      <td valign="top">getTrait<br>
 *      </td>
 *      <td valign="top">Not available (readonly)<br>
 *      </td>
 *      <td valign="top" align="center">"1.1"<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">baseProfile<br>
 *      </td>
 *      <td valign="top">getTrait<br>
 *      </td>
 *      <td valign="top">Not available (readonly)<br>
 *      </td>
 *      <td valign="top" align="center">"tiny"<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">viewBox<br>
 *      </td>
 *      <td valign="top">getRectTrait [null, SVGRect]<br>
 *      </td>
 *      <td valign="top">setRectTrait [SVGRect]<br>
 *      </td>
 *      <td valign="top" align="center">null<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">zoomAndPan<br>
 *      </td>
 *      <td valign="top">getTrait [disable | magnify]<br>
 *      </td>
 *      <td valign="top">setTrait [disable | magnify]<br>
 *      </td>
 *      <td valign="top" align="center">"magnify"<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *     </tr>
 *   </tbody>
 * </table>
 */

// ***********************************************************************
// SVG Path
// ***********************************************************************
/**
 * An SVG Path datatype is used to define the path geometry.
 * Corresponds to SVG path specification or the "d" attribute.
 *
 * <p>The native implementations must support the following simplifications or
 * canonicalization of path segments. Any simplifications should be lossless.
 *
 * <ul>
 * <li>Relative commands (c, h, l, m, q, s, t, and v) must be converted to their absolute 
 * counterparts·</li>
 * <li>Horizontal and Vertical lines (H, h, V, and v) must be converted to general 
 * lines (L and l)·</li>
 * <li>Translate command S to command C·</li>
 * <li>Translate command T to command Q.</li>
 * </ul>
 * </p>
 */

/**
 * The CSvgJavaInterfaceImpl class specializes the CSvgEngineInterfaceImpl class
 */
class CSvgJavaInterfaceImpl : public CSvgEngineInterfaceImpl, public MSvgMouseListener
{
public:
	/**
	 * Construct an instance of CSvgJavaInterfaceImpl
	 */
	IMPORT_C static CSvgJavaInterfaceImpl* NewL( TFontSpec& aFontSpec );
	IMPORT_C static CSvgJavaInterfaceImpl* NewL();

	/**
	 * Destruct an instance of CSvgJavaInterfaceImpl
	 */
    IMPORT_C ~CSvgJavaInterfaceImpl();

    // ***********************************************************************
    // SVG Engine
    // ***********************************************************************
    /**
     * Create an SvgEngine instance.
	 * NOTE: Java side uses engine only during rendering phase. 
	 *
     * @since 1.0
	 * @see javax.microedition.m2g.ScalableGraphics#render() 
	 * @return Pointer to CSvgEngineImpl object if creation is succesful, otherwise 0 is returned.
     */
    IMPORT_C SvgEngineHandle SvgEngineCreate();

    /**
     * Request to set render quality.
	 * Set the quality of rendering. It can take one of the values, 
	 * <code>RENDERING_QUALITY_LOW</code> (=1) or <code>RENDERING_QUALITY_HIGH</code> (=2). 
	 * Default value is <code>RENDERING_QUALITY_HIGH</code>. 
	 * The implementation of these quality levels is implementation dependent and should 
	 * be mapped to definitions in SVG spec (shape, text, image and color rendering)
	 *
     * @since 1.0
	 * @see javax.microedition.m2g.ScalableGraphics#setRenderingQuality() 
	 * @param aEngineHandle Engine handle.
	 * @param aQuality This value indicates the quality of rendering required.
     */
    IMPORT_C void SvgEngineSetRenderQuality( SvgEngineHandle aEngineHandle, TInt aQuality );

    /**
     * Request to render the SVG document.
	 *
     * @since 1.0
	 * @see javax.microedition.m2g.ScalableGraphics#render() 
	 * @param aEngineHandle Svg engine handle
     * @param aDocumentHandle Svg document handle
     * @param aSurfaceHandle Svg surface handle
	 * @param aSurfaceMaskHandle Mask that defines what pixels should be thrown and what not.
	 * @param aCurrentTime Current frame time. NOTE might be obsolete since the 
     */
    IMPORT_C void SvgEngineRenderDocument( 
		SvgEngineHandle aEngineHandle, SvgDocumentHandle aDocumentHandle, 
		TInt aSurfaceHandle, TInt aSurfaceMaskHandle = NULL, TReal32 aCurrentTime = 0.0f ) __SOFTFP;

    IMPORT_C void SvgEngineRenderDocument( 
        SvgEngineHandle aEngineHandle, SvgDocumentHandle aDocumentHandle, 
        SvgtBitmapHandle aSurfaceHandle, SvgtBitmapHandle aSurfaceMaskHandle = NULL, 
        TReal32 aCurrentTime = 0.0f ) __SOFTFP;

    IMPORT_C void SvgEngineRenderDocumentL( 
		SvgEngineHandle aEngineHandle, SvgDocumentHandle aDocumentHandle, 
		TInt aSurfaceHandle, const TPoint& aAnchor, const TRect& aRenderArea, TReal32 aCurrentTime, TReal32 aAlpha ) __SOFTFP;
		
    /**
     * Request to destroy the given engine by handle.
	 * NOTE: Java MIDlet could use multiple engines.
	 *
     * @since 1.0
     * @param aEngineHandle Engine
     */
    IMPORT_C void SvgEngineDestroy( SvgEngineHandle aEngineHandle );

	/*
	 * Sets the animation back to time 0 and then starts the internal engine timer
     * NOTE: Java doesn't use this method. Java uses own timer(s).
	 *
     * @since 1.0
     * @param aEngineHandle Engine
	 */
	IMPORT_C void SvgEngineStart( SvgEngineHandle aEngineHandle );

	/*
	 * Stops the internal SVG engine timer
     * NOTE: Java doesn't use this method. Java uses own timer.
	 *
     * @since 1.0
     * @param aEngineHandle Engine
	 */
	IMPORT_C void SvgEngineStop( SvgEngineHandle aEngineHandle );

    /*
	 * Resumes the internal SVG engine timer
     * NOTE: Java doesn't use this method. Java uses own timer.
	 *
     * @since 1.0
     * @param aEngineHandle Engine
	 */
	IMPORT_C void SvgEngineResume( SvgEngineHandle aEngineHandle );

    // *****************************************************************************
    // SVG Rendering Surface Functions
    // *****************************************************************************
    /**
     * Create a Svg Rendering Surface.
     * NOTE: Java doesn't use this method.
	 *
     * @since 1.0
	 * @param aWidth Width
	 * @param aHeight Height
	 * @return Surface
     */
    IMPORT_C SvgSurfaceHandle SvgRenderingSurfaceCreate( TInt aWidth, TInt aHeight );

    /**
     * Destroy a Svg Rendering Surface.
     * NOTE: Java doesn't use this method.
	 *
     * @since 1.0
	 * @param aSurface Surface
     */
	IMPORT_C void SvgRenderingSurfaceDestroy( SvgSurfaceHandle aSurface );

    /**
     * Get a pointer to the Svg Rendering surface.
     * NOTE: Java doesn't use this method.
	 *
     * @since 1.0
	 * @param aSurface Surface
	 * @return CFbsBitmap object
     */
    IMPORT_C TUint* SvgRenderingSurfaceGetBuffer( SvgSurfaceHandle aSurface );
    
	/**
     * Get the width of the Svg Rendering Surface.
     * NOTE: Java doesn't use this method.
	 *	
     * @since 1.0
	 * @param aSurface Surface
	 * @return Width
     */
    IMPORT_C TInt SvgRenderingSurfaceGetWidth( SvgSurfaceHandle aSurface );
    /**
     * Get the height of the Svg Rendering Surface.
     * NOTE: Java doesn't use this method.
	 *
     * @since 1.0
	 * @param aSurface Surface
	 * @return Height
     */
    IMPORT_C TInt SvgRenderingSurfaceGetHeight( SvgSurfaceHandle aSurface );

    /**
     * Clear the Svg Rendering Surface.
     * NOTE: Java doesn't use this method.
	 *
     * @since 1.0
	 * @param aSurface Surface
     */
    IMPORT_C void SvgRenderingSurfaceClearBuffer( SvgSurfaceHandle aSurface );

    // ***********************************************************************
    // SVG Document
    // ***********************************************************************
    /**
     * Create an empty svg document.
     * NOTE: Java doesn't use this method.
     * Java side uses the <code>SvgDocumentCreateL</code> method to create an empty SVG document
	 * that contains a root &lt;svg&gt; element with default viewport size of 100x100.
	 *
     * @since 1.0
	 * @see javax.microedition.m2g.SVGImage#createEmptyImage()
	 * @return Svg Document.
     */
    IMPORT_C SvgDocumentHandle SvgDocumentCreateEmpty();

    /**
     * Create a svg document by parsing the given string.
	 * The image size is determined by the content specification
     * (eg: width/height attributes on root SVG element). The default viewport size
     * of 100-by-100 pixels is used when the size is unspecified. This method will throw an exception
     * when the document is in error:
     * <ul>
     * <li>the SVG document does not conform to the XML 1.0 specification</li>
     * <li>an element has an attribute or property value which is not permissible according to the SVG specification </li>
     * <li>the required or mandatory attributes according to the SVG specification are missing</li>
     * <li>the document contains circular references on the &lt;use&gt; element</li>
     * <li>the document contains &lt;image&gt; element(s) with local references </li>
     * <li>the document contains elements with duplicate Id's</li>
     * <li>the document contains animation(s) in error</li>
     * </ul>
     *
     * The native engine might be invoked for any external resource referenced in the document
	 * by calling the <code>SvgDocumentRequestCompleted</code>. 
	 * However, please note that data URIs (for ex: base64 encoded images 
	 * like &lt;image xlink:href="data:image/png;base64,/9j/4AAQ..."/&gt;)
     * are required to be decoded by the native engine. 
	 * The engine must only make one call if there exist multiple resources with the same URI. 
	 * 
	 * If a svg document contains external resources and <code>SvgDocumentRequestCompleted</code> is not called 
	 * then the engine's default implementation is used automatically, but it might not be able to load all of 
	 * external resources. For example, if the svg document is loaded from a file in a Jar file, 
	 * the implementation will not be able to load images that have relative 
	 * URIs (ex; &lt;image xlink:href="myImage.png" /&gt;).
	 * 
     * <p>Note that <code>SvgDocumentRequestCompleted</code> is also called when the xlink:href attribute on
     * &lt;image&gt; is set or changed by the application, but the call is made only when the
     * element is hooked into the document tree i.e. when the ancestors go all the way up to
     * the root &lt;svg&gt; element. There are two cases:
     * <ul>
     * <li>When changing the xlink:href attribute of an existing &lt;image&gt; element that is already hooked or part of the tree.</li>
     * <li>When creating a new &lt;image&gt;, setting its xlink:href and hooking it to the document tree.</li>
     * </ul>
     * </p>
	 *
     * @since 1.0
	 * @see javax.microedition.m2g.ScalableImage#createImage()
	 * @param aString The string from which the SVG content should be read
     * @return Svg Document if successful, otherwise the function leaves.
     * @throws MJavaError if an error occurs while loading the content.
	 */
	IMPORT_C SvgDocumentHandle SvgDocumentCreateL( const TPtrC16& aString );

    /**
     * Destroy the given svg document (by handle).
	 * NOTE: Java MIDlet could use multiple documents.
	 * 
     * @since 1.0
	 * @param aDocumentHandle Document handle.
     */
    IMPORT_C void SvgDocumentDestroy( SvgDocumentHandle aDocumentHandle );

    /**
     * Svg Document request complete.
     * Once the requested external resource is available, the application forwards this information
     * (data) to the SVG engine. If this method is called a second time for a same URL (uri) of a
     * same SVG document, the engine will replace the current resource data with the new
     * one. <b>Note:</b> Setting <code>NULL</code> for data (<code>0</code> for data_size) indicates 
	 * that the requested resource could not be fetched by java side, and in this event the SVG engine 
	 * will not make further attempts to load this resource.
     *
     * @since 1.0
	 * @see javax.microedition.m2g.ScalableImage#requestCompleted()
     * @param hDocument SVG document handle
     * @param uri External resource URI
     * @param data External resource data
     * @param data_size External resource data size
	 * @return TODO
	 */
    IMPORT_C TInt SvgDocumentRequestCompleted( SvgDocumentHandle hDocument, const TPtrC16& aUri, const TPtrC8& );

    /**
     * Get an external item indicated in a SVG document.
     * Java side might request an external resouce and once the requested external resource is available, 
	 * java side forwards this information (<code>SvgDocumentGetExternalListItem</code>) to the SVG engine. 
	 * <b>Note:</b> Returning <code>null</code> indicates that the SVG engine tryes to handle the requested resource. 
	 *
	 * @since 1.0
	 * @param aDocumentHandle SVG document handle
	 * @param index External resource item index. Scale:
	 * <li>0 <= <code>index</code> < <code>SvgDocumentGetExternalListSize</code></li>
     * @return External resource URI
	 */
	IMPORT_C TInt SvgDocumentGetExternalListItem(SvgDocumentHandle aDocumentHandle, TInt aIndex, TPtrC16& aItem);

     /**
     * Get number of external list items in a SVG document.
	 * After the SVG document is created (see <code>SvgDocumentCreateL</code>) java side checks if 
	 * the SVG document contains external item(s)
     *
	 * @since 1.0
	 * @param aDocumentHandle SVG document handle
     * @return Number of external resource items
	 */
    IMPORT_C TInt SvgDocumentGetExternalListSize( SvgDocumentHandle aDocumentHandle );

  /**
   * Increments the animation or media timeline for the specified document aDocumentHandle
	 * (in seconds). 
	 * This method is intended to move only forward in the timeline. 
	 * It is important to note that setting large increments of time would result in
   * skipping parts of the animation as per the SVG animation model.
   *
	 * @since 1.0
	 * @see javax.microedition.m2g.SVGImage#incrementTime()
	 * @see org.w3c.dom.svg.SVGSVGElement#setCurrentTime()
   * @param aDocumentHandle SVG document handle.
   * @param seconds the value of time to be set in seconds.
   */
    IMPORT_C void SvgDocumentSetMediaTime( SvgDocumentHandle aDocumentHandle, TReal32 seconds ) __SOFTFP;

    /**
     * Get current animation or media timeline time for the specified document.
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGSVGElement#getCurrentTime()
	 * @param aDocumentHandle SVG document handle
	 * @return Returns current animation timeline time in seconds
     */
    IMPORT_C TReal32 SvgDocumentGetMediaTime( SvgDocumentHandle aDocumentHandle ) __SOFTFP;

   /**
    * Get the viewport width of the given SVG document.
	 * The value returned is always in pixels. If the specified width is defined in percentages, the
   * values are mapped to the default view port size of 100x100. If the viewport width
   * is explicitly changed by the java application, then the percentages are ignored and the content
   * is made to fit to this new viewport width.
   *
	 * @since 1.0
	 * @see javax.microedition.m2g.ScalableImage#getViewportWidth()
	 * @see SvgDocumentSetViewportWidth
	 * @param aDocumentHandle SVG document handle
   * @return the current width of the given SVG document.
   */    
	IMPORT_C TInt SvgDocumentGetViewportWidth( SvgDocumentHandle aDocumentHandle );

  /**
   * Get the viewport height of the given SVG document.
	 * The value returned is always in pixels. If the specified height is defined in percentages, the
   * values are mapped to the default view port size of 100x100. If the viewport height
   * is explicitly changed by the java application, then the percentages are ignored and the content
   * is made to fit to this new viewport height.
   *
	 * @since 1.0
	 * @see javax.microedition.m2g.ScalableImage#getViewportHeight()
	 * @see SvgDocumentSetViewportHeight
	 * @param aDocumentHandle SVG document handle
   * @return the current height of the given SVG document.
   */    
	IMPORT_C TInt SvgDocumentGetViewportHeight( SvgDocumentHandle aDocumentHandle );

    /**
     * Set the new (viewport) width for the given SVG document.
	 *
	 * @since 1.0
	 * @see javax.microedition.m2g.ScalableImage#setViewportWidth()
	 * @see SvgDocumentGetViewportWidth
	 * @param aDocumentHandle SVG document handle
	 * @param aWidth The new width to be set.
     */
    IMPORT_C void SvgDocumentSetViewportWidth( SvgDocumentHandle aDocumentHandle, TInt aWidth );

  /**
   * Set the new (viewport) height for the given SVG document.
	 *
	 * @since 1.0
	 * @see javax.microedition.m2g.ScalableImage#setViewportHeight()
	 * @see SvgDocumentGetViewportHeight
	 * @param aDocumentHandle SVG document handle
	 * @param aHeight The new height to be set.
   */
    IMPORT_C void SvgDocumentSetViewportHeight( SvgDocumentHandle aDocumentHandle, TInt aHeight );

  /**
   * Set the default viewport size for the given SVG document.
	 * The default viewport size of 100-by-100 pixels is used when the size is unspecified.
	 *
	 * @since 1.0
	 * @param aDocumentHandle Document
   */
    IMPORT_C void SvgDocumentViewportInit( SvgDocumentHandle aDocumentHandle );

  /**
   * Get the viewport width units for the given document.
	 * NOTE: Java side is not actually need this method.
	 *
	 * @since 1.0
	 * @param aDocumentHandle Document
	 * @return <code>1</code> if pixels or <code>0</code> if percentages 
     */
    IMPORT_C TInt SvgDocumentGetViewportWidthUnits( SvgDocumentHandle aDocumentHandle );

  /**
   * Get the viewport height units for the given document.
	 * NOTE: Java side is not actually need this method.
	 *
	 * @since 1.0
	 * @param aDocumentHandle Document
	 * @return <code>1</code> if pixels or <code>0</code> if percentages 
   */
    IMPORT_C TInt SvgDocumentGetViewportHeightUnits( SvgDocumentHandle aDocumentHandle  );

  /**
   * Return a child element of the given SVG document Node which corresponds to the top-most
   * tag in XML file. For SVG files it must be <code>SVGSVGElement</code>.
   *
   * @since 1.0
	 * @see org.w3c.dom.Document#getDocumentElement()
   * @param aDocumentHandle SVG document handle
   * @return The root element associated with the given SVG document.
   */
    IMPORT_C SvgElementHandle SvgDocumentGetRootElement( SvgDocumentHandle aDocumentHandle );

    /**
     * Get the svg element in the given SVG document with
     * the given unique ID string.If no such element exists, this returns NULL.
	   *
     * @since 1.0
	   * @see org.w3c.dom.Document#getElementById()
     * @param aDocumentHandle SVG document handle
     * @param aId the ID of the element to be retrieved.
     * @return An element handle that matches with the given ID or
     * <code>NULL</code> if the ID is not present.
     */
    IMPORT_C SvgElementHandle SvgDocumentGetElementById( SvgDocumentHandle aDocumentHandle, const TPtrC16& aID );

	/**
   * Get the number of ids in the current SVG document.
   * NOTE: Java doesn't use this method.
	 *
   * @since 1.0
	 * @param aDocumentHandle SVG document handle
   */
	IMPORT_C TInt SvgDocumentGetNumberOfIds(SvgDocumentHandle aDocumentHandle );

    /**
     * Get an id (at index) from the current SVG document.
     * NOTE: Java doesn't use this method.
	   *
     * @since 1.0
	   * @param aDocumentHandle SVG document handle
	   * @param index ID index
	   * @return ID string
     */
    IMPORT_C TInt SvgDocumentGetId(SvgDocumentHandle aDocumentHandle, TInt index, TPtrC16& aId);

  /**
   * Set the begin animation time for the given element in the given SVG document.
   * The new time is added to the <a href="http://www.w3.org/TR/2001/REC-smil20-20010807/smil20.html#smil-timing-Timing-BeginEnd-InstanceTimesLists">
   * begin instance times list</a>.
   *
   * @since 1.0
	 * @see org.w3c.dom.svg.SVGAnimationElement#beginElementAt()
	 * @param aDocumentHandle SVG document handle
	 * @param aElementHandle Element handle
   * @param aOffsetTime The time in seconds at which to begin the element.
   */
    IMPORT_C void SvgDocumentBeginElementAt( SvgDocumentHandle aDocumentHandle,
                                        SvgElementHandle aElementHandle,
                                        TReal32 aOffsetTime ) __SOFTFP;

  /**
   * Set the end animation time for the given element in the given SVG document.
   * The new time is added to the <a href="http://www.w3.org/TR/2001/REC-smil20-20010807/smil20.html#smil-timing-Timing-BeginEnd-InstanceTimesLists">
   * end instance times list</a>.
	 * NOTE: Native engine checks the element is active before set the new end time.
   *
   * @since 1.0
	 * @see org.w3c.dom.svg.SVGAnimationElement#endElementAt()
	 * @param aDocumentHandle SVG document handle
	 * @param aElementHandle Element handle
   * @param aOffsetTime The time in seconds at which to end the element.
   */
    IMPORT_C void SvgDocumentEndElementAt( SvgDocumentHandle aDocumentHandle,
                                  SvgElementHandle aElementHandle,
                                  TReal32 aOffsetTime ) __SOFTFP;

    /**
     * Set the given element to have the focus in the given SVG document.
     * The initial focus is always <code>null</code> and setting
     * <code>NULL</code> will remove the current focus.
     * NOTE: Java doesn't use this method.
     *
     * @since 1.0
	   * @see javax.microedition.m2g.SVGImage#focusOn()
	   * @param aDocumentHandle SVG document handle
     * @param aElementHandle The element to set the focus on.
     
     * changes made for Focus-in/focus-out bug(AMIA-6T8EEG)
     */
    IMPORT_C void SvgDocumentFocusOn( SvgDocumentHandle aDocumentHandle,
                             SvgElementHandle aElementHandle );

    /**
     * Get the element having the focus.
     * NOTE: Java doesn't use this method.
	   *
     * @since 1.0
	   * @see javax.microedition.m2g.SVGImage#focusOn()
	   * @param aDocumentHandle SVG document handle
     * @return The element focused.
     */
    IMPORT_C SvgElementHandle SvgDocumentGetFocus( SvgDocumentHandle aDocumentHandle );

    /**
     * Activate the element that has the focus.
     * NOTE: Java doesn't use this method.
	   *
     * @since 1.0
	   * @see javax.microedition.m2g.SVGImage#activate()
	   * @param aDocumentHandle SVG document handle
     */
    IMPORT_C void SvgDocumentActivate( SvgDocumentHandle aDocumentHandle );

    /**
     * Request a mouse event at the given coordinate.
     * This method is used to dispatch a mouse "click" event to the given
     * document. The mouse position is given as screen coordinates <code>aMouseX, aMouseY</code>. 
	   * If the aMouseX, aMouseY values are outside the viewport area or no target is available 
	   * for the aMouseX, aMouseY coordinates, the event is not dispatched. 
     *
     * @since 1.0
	   * @see javax.microedition.m2g.SVGImage#dispatchMouseEvent()
	   * @param aDocumentHandle SVG document handle
	   * @param aMouseX The x location of the mouse/pointer in viewport coordinate system.
	   * @param aMouseX The y location of the mouse/pointer in viewport coordinate system.
	   * @return Element handle to where mouse was clicked. Return null if no hit.
     */
    IMPORT_C SvgElementHandle SvgDocumentDispatchMouseEvent( SvgDocumentHandle aDocumentHandle,
                                         TInt aMouseX, TInt aMouseY );

  /**
   * Check if document has animation.
   * NOTE: Java doesn't use this method.
	 *
	 * @since 1.0
	 * @param aDocumentHandle SVG document handle
	 * @param TBool . ETrue if animation present otherwise EFalse.
   */
    IMPORT_C TInt SvgDocumentHasAnimation( SvgDocumentHandle aDocumentHandle );


    // ***********************************************************************
    // SVG Element
    // ***********************************************************************
	/**
     * Create a new svg element based on the specified
     * (<code>aType</code>) SVG tag name. Only the following elements must be supported:
     * &lt;rect&gt;, &lt;circle&gt;, &lt;ellipse&gt;, &lt;line&gt;,
     * &lt;path&gt; &lt;use&gt; &lt;image&gt; &lt;text&gt;,
     * &lt;a&gt; and &lt;g&gt;.
	 *
	 * @since 1.0
	 * @see org.w3c.dom.Document#createElementNS()
	 * @param aType
     * @return handle to the newly created SVG Element.
     */
    IMPORT_C SvgElementHandle SvgElementCreate( SvgAttrType aType );

    /**
     * Destroy svg element of the given handle.
     * NOTE: Java doesn't use this method.
	   *
	   * @since 1.0
	   * @param hElement Element handle
     */
    IMPORT_C void  SvgElementDestroy( SvgElementHandle hElement );

    /**
     * Returns a parent element of the given element.
     *
	   * @since 1.0
	   * @see org.w3c.dom.svg.SVGElement#getParentNode()
	   * @param aElementHandle Element handle
     * @return The parent element or <code>NULL</code> if there is no parent 
	   * (i.e. if a element has just been created and not yet added to the tree, 
	   * or if it has been removed from the tree, this is null).
     */
    IMPORT_C SvgElementHandle SvgElementGetParent( SvgElementHandle aElementHandle );

    /**
     * Returns the first child element node of the given element. 
	   * Return <code>NULL</code> if this element has no child elements.
	   *
	   * @since 1.0
	   * @see org.w3c.dom.svg.SVGElement#getFirstElementChild()
	   * @param aElementHandle Element handle
	   * @return The first child element node of the given element.
     */
    IMPORT_C SvgElementHandle SvgElementGetFirstElementChild( SvgElementHandle aElementHandle );

    /**
     * Returns the next sibling element of the given element. 
	   * Return <code>NULL</code> if the given element has no element sibling
     * nodes that come after this one in the document tree.
	   *
	   * @since 1.0
	   * @see org.w3c.dom.svg.SVGElement#getNextElementSibling()
	   * @param aElementHandle Element handle
     * @return The next sibling element node of this element.
     */
    IMPORT_C SvgElementHandle SvgElementGetNextElementSibling( SvgElementHandle aElementHandle );

    /**
     * Append the given child element to the given svg element.
     *
	 * @since 1.0
	 * @see org.w3c.dom.Node#appendChild()
	 * @param aElementHandle Element handle
	 * @param aChildElementHandle New child element handle
	 * @return Next sibling element
     */
    IMPORT_C void SvgElementAppendChild( SvgElementHandle aElementHandle,
                                 SvgElementHandle aChildElementHandle );

    /**
     * Remove the given child element from the given svg element.
	 * Elements that have ids cannot be removed from the tree.
	 *
	 * @since 1.0
	 * @see org.w3c.dom.Node#removeChild()
	 * @param aElementHandle The parent element.
	 * @param aChildElementHandle The element that is to be removed
	 * TODO @return Handle to the removed element. Return <code>NULL</NULL>
	 * if operation failed.
     */
    IMPORT_C SvgElementHandle SvgElementRemoveChild( SvgElementHandle aElementHandle,
                                 SvgElementHandle aChildElementHandle );

    /**
     * Inserts <code>aChildElementHandle</code> before <code>aReferenceElementHandle</code>
	 * in the child list for the <code>aElementHandle</code>. If <code>aReferenceElementHandle</code>
	 * is <code>NULL</code>, <code>aChildElementHandle</code> is inserted at the end of the list. 
	 * If the <code>aChildElementHandle</code> is already part of the tree, it is first removed.
	 *
	 * @since 1.0
	 * @see org.w3c.dom.Node#insertBefore()
	 * @param aElementHandle The parent element.
	 * @param aChildElementHandle The child to add
	 * @param aReferenceElementHandle The child before which the new child should be added.
	 * @throws TODO Should throw exception if the <code>aChildElementHandle</code> would cause 
	 * the SVG document to go into error, for ex: when the newChild contains a &lt;use&gt; 
	 * element with an invalid xlink:href attribute.
     */
    IMPORT_C void SvgElementInsertBefore( SvgElementHandle aElementHandle,
                                  SvgElementHandle aChildElementHandle,
                                  SvgElementHandle aReferenceElementHandle );

    /**
     * Request element type. The following element type are supported:
     * &lt;a&gt;, &lt;animate&gt;, &lt;animateColor&gt;, &lt;animateMotion&gt;,
     * &lt;animateTransform&gt; &lt;circle&gt; &lt;defs&gt; &lt;ellipse&gt;,
     * &lt;font&gt;, &lt;font-face&gt;, &lt;font-face-name&gt;, &lt;font-face-src&gt;, 
	 * &lt;foreignObject&gt;, &lt;g&gt;, &lt;glyph&gt;, &lt;hkern&gt;, &lt;image&gt;, 
	 * &lt;line&gt;, &lt;metadata&gt;, &lt;missing-glyph&gt;, &lt;metadata&gt;, &lt;mpath&gt;.
	 * &lt;path&gt;, &lt;polygon&gt;, &lt;polyline&gt;, &lt;rect&gt;, &lt;set&gt;,
	 * &lt;svg&gt;, &lt;switch&gt;, &lt;text&gt;, &lt;title&gt;, &lt;use&gt;.
	 *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement
	 * @param aElementHandle The element.
	 * @return Element type. 
     */
    IMPORT_C TInt SvgElementGetType( SvgElementHandle aElementHandle );

	 /**
   * Check if document has animation.
   * NOTE: Java doesn't use this method.
	 *
	 * @since 1.0
	 * @param aElementHandle SVG element handle
	 * @param TBool . ETrue if animation present otherwise EFalse.
   */
    IMPORT_C TInt SvgElementHasAnimation( SvgElementHandle aElementHandle );
    
    /**
     * Get a string attribute from the given element.
	 *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement
	 * @param aElementHandle The element.
	 * @param aAttributeName Attribute's name
	 * @return Attribute value.
     */
    IMPORT_C TInt SvgElementGetStringAttribute( SvgElementHandle aElementHandle,
                                        SvgAttrType aAttributeName, TPtrC16& aStrAttribute );

    /**
     * Set a string attribute in the given element.
	 *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement
	 * @param aElementHandle The SVG element handle.
	 * @param aAttributeName Attribute's name
	 * @param aAttributeValue Atribute's value.
     */
    IMPORT_C void SvgElementSetStringAttribute( SvgElementHandle aElementHandle,
                                        SvgAttrType aAttributeName,
                                        const TPtrC16& aAttributeValue );

    /**
     * Get a color attribute from the given element.
     * The values are copied into the color components given.
	 *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGRGBColor
	 * @param aElementHandle The element handle.
	 * @param aAttributeName Attribute's name
	 * @param aRedValue The red value
	 * @param aGreenValue The green value 
	 * @param aBlueValue The blue value
	 * @return Integer containing the RGB value for the color.
     */
    IMPORT_C TInt SvgElementGetColorAttribute( SvgElementHandle aElementHandle,
                                       SvgAttrType aAttribute,
                                       TInt* aRedValue, TInt* aGreenValue, TInt* aBlueValue );

    /**
   * Set a color attribute for the given element.
	 *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGRGBColor
	 * @param aElementHandle The element.
	 * @param aAttribute Attribute's name
	 * @param aRedValue The red value
	 * @param aGreenValue The green value 
	 * @param aBlueValue The blue value
     */
    IMPORT_C void SvgElementSetColorAttribute( SvgElementHandle aElementHandle,
                                       SvgAttrType aAttribute,
                                       TInt aRedValue, TInt aGreenValue, TInt aBlueValue );


    /**
	 * Return a attribute (trait) value as float.
	 *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement#getFloatTrait()
	 * @param aElementHandle The element.
	 * @param aAttribute Attribute's (or trait's) name
     * @return The attribute (or trait) value as float for the specified name.
     */
    IMPORT_C TReal32 SvgElementGetFloatAttribute( SvgElementHandle aElementHandle,
                                        SvgAttrType aAttribute ) __SOFTFP;

    /**
     * Set the attribute (trait) value as float.
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement#getFloatTrait()
	 * @param aElementHandle The element handle.
     * @param aAttribute The name of the attribute (trait) to be set.
     * @param aFloatValue The value of the attribute (trait) to be set as float.
     */
    IMPORT_C void SvgElementSetFloatAttribute( SvgElementHandle aElementHandle,
                                      SvgAttrType aAttribute,
                                      TReal32 aFloatValue ) __SOFTFP;

    /**
     * Set enum attribute (trait) value.
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement
	 * @param aElementHandle The element handle.
     * @param aAttribute The name of the attribute (trait) to be set.
     * @param aValue The value of the attribute (trait) to be set.
     */
    IMPORT_C void SvgElementSetEnumAttribute( SvgElementHandle aElementHandle,
                                      SvgAttrType aAttribute, short aValue );

    /**
     * Get enum attribute (trait) value.
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement
	 * @param aElementHandle The element handle.
     * @param aAttribute The name of the attribute (trait).
     * @return The value of the attribute (trait).
     */
    IMPORT_C short SvgElementGetEnumAttribute( SvgElementHandle aElementHandle,
                                      SvgAttrType aAttribute );

    /**
     * Return the rectangle attribute (trait) values. 
	 * NOTE: The returned values are copies of the actual attribute (trait) values and 
	 * will not change if the corresponding trait changes.
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement#getRectTrait() 
	 * @param aElementHandle The element handle.
     * @param aRectAttribute The name of the attribute (trait) to retrieve.
	 * @param aX X coordinate.
	 * @param aY Y coordinate.
	 * @param aWidth Width.
	 * @param aHeight Height.
     * @return <code>-1</code> if failed otherwise returns positive integer.
     */
    IMPORT_C TInt SvgElementGetRectAttribute( SvgElementHandle aElementHandle,
                                      SvgAttrType aRectAttribute,
                                      TReal32* aX, TReal32* aY, TReal32* aWidth, TReal32* aHeight ) __SOFTFP;

    /**
     * Set the rectangle attribute (trait) values. 
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement#setRectTrait() 
	 * @param aElementHandle The element handle.
     * @param aRectAttribute The name of the attribute (trait) to be set.
	 * @param aX X coordinate.
	 * @param aY Y coordinate.
	 * @param aWidth Width.
	 * @param aHeight Height.
     */
    IMPORT_C void SvgElementSetRectAttribute( SvgElementHandle aElementHandle,
                                     SvgAttrType aRectAttribute,
                                     TReal32 aX, TReal32 aY, TReal32 aWidth, TReal32 aHeight ) __SOFTFP;

    /**
     * Get the matrix attribute (trait) values.     
     * NOTE: The returned values are copies of the actual attribute (trait) values and 
	 * will not change if the corresponding trait changes.
     * <pre>
     * [aAVal aCVal aEVal]
     * [aBVal aDVal aFVal]
     * </pre>
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement#getMatrixTrait() 
	 * @param aElementHandle The element handle.
     * @param aMatrixAttribute The name of the attribute (trait) to retrieve.
     * @param aAVal The x scaling component
     * @param aBVal The y shearing component
     * @param aCVal The x shearing component
     * @param aDVal The y scaling component
     * @param aEVal The x translation component
     * @param aFVal The y translation component
     * @return TODO
	 */ 
	IMPORT_C TInt SvgElementGetMatrixAttribute( SvgElementHandle aElementHandle,
                                       SvgAttrType aMatrixAttribute,
                                       TReal32* aAVal, TReal32* aBVal, TReal32* aCVal,
                                       TReal32* aDVal, TReal32* aEVal, TReal32* aFVal ) __SOFTFP;

    /**
     * Set the matrix attribute (trait) values.     
     * <pre>
     * [aAVal aCVal aEVal]
     * [aBVal aDVal aFVal]
     * </pre>
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement#setMatrixTrait() 
	 * @param aElementHandle The element handle.
     * @param aMatrixAttribute The name of the attribute (trait) to be set.
     * @param aAVal The x scaling component
     * @param aBVal The y shearing component
     * @param aCVal The x shearing component
     * @param aDVal The y scaling component
     * @param aEVal The x translation component
     * @param aFVal The y translation component
	 */ 
    IMPORT_C void SvgElementSetMatrixAttribute( SvgElementHandle aElementHandle,
                                        SvgAttrType aMatrixAttribute,
                                       TReal32 aAVal, TReal32 aBVal, TReal32 aCVal,
                                       TReal32 aDVal, TReal32 aEVal, TReal32 aFVal ) __SOFTFP;

    /**
     * Get the path attribute (trait)value. 
	 * NOTE: The returned values are copies of the actual attribute values and will not change if
     * the corresponding trait changes.
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement#getPathTrait() 
	 * @see org.w3c.dom.svg.SVGPath
	 * @param aElementHandle The element handle.
     * @param aPathAttribute The name of the attribute (trait) to retrieve.
     * @return The handle to path object. <code>NULL</code> if not successed.
     */    
	IMPORT_C TInt SvgElementGetPathAttribute( SvgElementHandle aElementHandle,
                                      SvgAttrType aPathAttribute );

    /**
     * Set the path attribute (trait) value. 
	 * NOTE: Value is a copy in the trait so subsequent changes to the given
     * <code>aPathHandle</code> have no effect on the value of the attribute (trait).
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGElement#setPathTrait() 
	 * @see org.w3c.dom.svg.SVGPath
	 * @param aElementHandle The element handle.
     * @param aPathAttribute The name of the attribute (trait) to be set.
     * @param aPathHandle The value of the attribute (trait) to be set as.
     */
   	IMPORT_C void SvgElementSetPathAttribute( SvgElementHandle aElementHandle,
                                      SvgAttrType aPathAttribute , SvgPathHandle aPathHandle);

    /**
     * Returns the tight bounding box in current user coordinate space. 
	 * Tight bounding box is the smallest possible rectangle that includes the geometry 
	 * of all contained graphics elements excluding stroke.
     * The calculation is done in the user coordinate space of the element. When bounding box
     * is calculated elements with display property (trait) set to none are ignored. 
	 * Exact rules for the bounding box calculation are given in the 
	 * <a href="http://www.w3.org/TR/SVG/coords.html#ObjectBoundingBox">SVG spec</a>.
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGLocatableElement#getBBox() 
	 * @param aElementHandle The element handle.
     * @param aAttributeType The name of the attribute (trait) to retrieve.
	 * @param aX X coordinate.
	 * @param aY Y coordinate.
	 * @param aWidth Width.
	 * @param aHeight Height.
     */
    IMPORT_C void SvgElementGetBBox( SvgElementHandle aElementHandle,
                            SvgAttrType aAttributeType,
                            TReal32* aX, TReal32* aY, TReal32* aWidth, TReal32* aHeight ) __SOFTFP;

    /**
     * Add event listener to the given element.
     * Note: need to replace void* with specific class
     * NOTE: Java doesn't use this method.
	 *
     * @since 1.0
	 * @param aElementHandle The element handle.
	 * @param aListener Listener
	 * @param aType Type
     */
    IMPORT_C void SvgElementAddEventListener( SvgElementHandle aElementHandle,
                                     TInt aListener, SvgAttrType aType );

    /**
     * Remove event listener from the given element.
     * Note: need to replace void* with specific class
     * NOTE: Java doesn't use this method.
	 *
     * @since 1.0
	 * @param aElementHandle The element handle.
	 * @param aListener Listener
	 * @param aType Type
     */
    IMPORT_C void SvgElementRemoveEventListener( SvgElementHandle aElementHandle,
                                        TInt aListener, SvgAttrType aType );
    /**
     * Check if element is active.
	 * i.e. an animation element is active in these cases:
     * <ul>
	 * <li>
	 * A media time is 1s.
     * <pre>
	 *    <rect id=&lt;element&gt; x=&lt;0&gt; y=&lt;20&gt; width=&lt;20&gt; height=&lt;20&gt;>
          <animate id=&lt;animationElement&gt; attributeName=&lt;x&gt; attributeType=&lt;XML&gt;
                 begin=&lt;1s&gt; dur=&lt;4s&gt; end=&lt;2s&gt; fill=&lt;freeze&gt; 
				 from=&lt;0&gt; to=&lt;80&gt;
		   </animate>
		   </rect>
     * </pre>
	 * </li>
	 * <li>
	 * A media time is 2s.
     * <pre>
	 *    <rect id=&lt;element&gt; x=&lt;0&gt; y=&lt;20&gt; width=&lt;20&gt; height=&lt;20&gt;>
          <animate id=&lt;animationElement&gt; attributeName=&lt;x&gt; attributeType=&lt;XML&gt;
                 begin=&lt;1s&gt; dur=&lt;4s&gt; end=&lt;2s&gt; fill=&lt;freeze&gt; 
				 from=&lt;0&gt; to=&lt;80&gt;
		   </animate>
		   </rect>
     * </pre>
	 * </li>
     * </ul>
     * 
	 * @since 1.0
     * @see org.w3c.dom.svg.SVGAnimationElement#endElementAt()
	 * @param aElementHandle The element handle.
	 * @return Positive integer if element is active.
     */
    IMPORT_C TInt SvgElementIsActive( SvgElementHandle aElementHandle );

    /**
     * Check if the given element is removable (no id or children with ids).
     * 
	 * @since 1.0
     * @see org.w3c.dom.Node#removeChild()
	 * @param aElementHandle The element handle.
	 * @return Positive integer if element is removeable.
     */
    IMPORT_C TInt SvgElementCheckRemoveable( SvgElementHandle aElementHandle );

    // ***********************************************************************
    // SVG Path
    // ***********************************************************************
    /**
     * Creates new path object that is empty. 
	 * This object can be used to modify value of path traits
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGSVGElement#createSVGPath()
	 * @see org.w3c.dom.svg.SVGPath
     * @return The newly created path object handle with empty path commands.
     */
    IMPORT_C SvgPathHandle SvgPathCreate();

    /**
     * Destroy the specified path path
	 *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGPath
     * @param aPathHandle Handle to the path to be deleted.
     */
    IMPORT_C void SvgPathDestroy( SvgPathHandle aPathHandle );

    /**
     * Get the segment count of the given path.
	 *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGPath#getNumberOfSegments()
     * @param aPathHandle The path handle.
	 * @return Segment count
     */
    IMPORT_C TInt SvgPathGetSegmentCount( SvgPathHandle aPathHandle );

    /**
     * Returns segment command by zero-based command index. 
	 * Returns one of <code>MOVE_TO</code>, <code>LINE_TO</code>, 
	 * <code>CURVE_TO</code>, <code>QUAD_TO</code> or <code>CLOSE</code>.
	 *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGPath#getSegment()
     * @param aPathHandle The path handle.
     * @param aSegmentIndex The command index for the segment command to retrieve.
     * @return The segment command for the specified aSegmentIndex.
     */
    IMPORT_C TInt SvgPathGetSegmentType( SvgPathHandle aPathHandle, TInt aSegmentIndex );

    /**
     * Returns segment parameter by zero-based command index and zero-based parametr index.
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGPath#getSegmentParameter()
     * @param aPathHandle The path handle.
     * @param aSegmentIndex The command index for the segment command to retrieve.
     * @param aSegmentParameterIndex The parameter index for the segment parameter to retrieve.
     * @return the segment parameter for the specified aSegmentIndex and aSegmentParameterIndex.
     */
    IMPORT_C TReal32 SvgPathGetSegmentParameter( SvgPathHandle aPathHandle,
                                       TInt aSegmentIndex,
                                       TInt aSegmentParameterIndex ) __SOFTFP;

    /**
     * Appends 'M' (absolute move) segment to the path with the specified coordinates.
     *
	 * @since 1.0
	 * @see org.w3c.dom.svg.SVGPath#moveTo()
     * @param aPathHandle The path handle.
     * @param aX The x-axis coordinate for the specified point.
     * @param aY The y-axis coordinate for the specified point.
     */
    IMPORT_C void SvgPathAddMoveTo( SvgPathHandle aPathHandle, TReal32 aX, TReal32 aY ) __SOFTFP;

    /**
     * Appends 'L' (absolute line) segment to the path with the specified coordinates.
     *
     * @since 1.0
	 * @see org.w3c.dom.svg.SVGPath#moveTo()
     * @param aPathHandle The path handle.
     * @param aX The x-axis coordinate for the specified point.
     * @param aY The y-axis coordinate for the specified point.
     */
    IMPORT_C void SvgPathAddLineTo( SvgPathHandle aPathHandle, TReal32 aX, TReal32 aY ) __SOFTFP;

    /**
     * Appends 'Q' (absolute quadratic curve) segment to the path.
     *
     * @since 1.0
	 * @see org.w3c.dom.svg.SVGPath#quadTo()
     * @param aPathHandle The path handle.
     * @param aX1 the x-axis coordinate of the first control point.
     * @param aY1 the y-axis coordinate of the first control point.
     * @param aX2 the x-axis coordinate of the final end point.
     * @param aY2 the y-axis coordinate of the final end point.
     *
     */
	 IMPORT_C void SvgPathAddQuadTo( SvgPathHandle aPathHandle,
                            TReal32 aX1, TReal32 aY1,
                            TReal32 aX2, TReal32 aY2 ) __SOFTFP;

    /**
     * Appends 'C' (absolute cubic curve) segment to the path.
     *
     * @since 1.0
	 * @see org.w3c.dom.svg.SVGPath#curveTo()
     * @param aPathHandle The path handle.
     * @param aX1 the x-axis coordinate of the first control point.
     * @param aY1 the y-axis coordinate of the first control point.
     * @param aX2 the x-axis coordinate of the second end point.
     * @param aY2 the y-axis coordinate of the second end point.
     * @param aX3 the x-axis coordinate of the final end point.
     * @param aY3 the y-axis coordinate of the final end point.
     *
     */
    IMPORT_C void SvgPathAddCurveTo( SvgPathHandle aPathHandle,
                            TReal32 aX1, TReal32 aY1,
                            TReal32 aX2, TReal32 aY2,
                            TReal32 aX3, TReal32 aY3 ) __SOFTFP;
    /**
     * Appends 'Z' (close path) segment to the path
     *
     * @since 1.0
	 * @see org.w3c.dom.svg.SVGPath#close()
     * @param aPathHandle The path handle.
     */    
	IMPORT_C void SvgPathAddClose( SvgPathHandle aPathHandle );

    /**
     * Update path info.
     * NOTE: Java doesn't use this method.
     *
     * @since 1.0
     * @param aElementHandle The element handle.
     * @param aPathHandle The path handle.
     */
    IMPORT_C void SvgElementUpdatePath( SvgElementHandle aElementHandle, SvgPathHandle aPathHandle );

	/**
	 * Find whether the element is present in a document.
     *
     * @since 1.0
     * @param hDocument The SVG document handle.
     * @param hElement The element handle.
	 * @return Positive integer if the element presents in the given SVG document.
	 */
	IMPORT_C TInt SvgElementElementInDOM ( TInt hDocument, TInt hElement );

	/**
	 * Checks whether this element is child of a use element.
     *
     * @since 1.0
	 * @see javax.microedition.m2g.SVGImage#dispatchMouseEvent()
     * @param hElement The element handle.
	 * @return Postivive integer if the element is child of a use element.
	 */
    IMPORT_C TInt SvgElementIsUsed ( TInt hElement );

	/**
	 * Finds the handle to the actual cloned element.
     *
     * @since 1.0
	 * @see javax.microedition.m2g.SVGImage#dispatchMouseEvent()
     * @param hElement aElement This is handle to the cloned element. this is child of the
     *                          <use> element.
	 * @return Element handle to the actual element, which is cloned.
	 */
    IMPORT_C TInt SvgElementGetUsedFromElement ( TInt hElement );

    /**
	 * Gets a screen bounding box for the given element.
     * Returns the tight bounding box in screen coordinate space. Tight bounding box is the smallest
     * possible rectangle that includes the geometry of all contained graphics elements excluding 
	 * stroke. The box coordinates are in the screen coordinate space, which is connected to the 
	 * current user coordinate space by the matrix returned by <code>SvgElementGetMatrixAttribute</code>
     * (with attribute id=&lt;svg&gt;).
     *
     * @since 1.0
	 * @see org.w3c.dom.svg.SVGLocatableElement#getScreenBBox()
	 * @see org.w3c.dom.svg.SVGLocatableElement#getScreenCTM()
     * @param hElement The element handle.
     */
	IMPORT_C void SvgElementGetScreenBBox( TInt hElement, TReal32* x,TReal32* y, TReal32* w, TReal32* h ) __SOFTFP;

	 /**
     * Set the focus out of the given element.
     * The initial focus is always <code>null</code> and setting
     * <code>NULL</code> will remove the current focus.
     * NOTE: Java doesn't use this method.
     *
     * @since 1.0
	   * @see javax.microedition.m2g.SVGImage#focusOut()
	   * @param aDocumentHandle SVG document handle
     * @param aElementHandle The element for which to set the focus out.
     
     * changes made for Focus-in/focus-out bug(AMIA-6T8EEG)
     */
    IMPORT_C void CSvgJavaInterfaceImpl::SvgDocumentFocusOut( SvgDocumentHandle aDocumentHandle,
                             SvgElementHandle aElementHandle);
                             
	/*
	* Maps the PATH segment type from SVG to JSR.
	*/

    TInt MapSegmentType(TInt aSvgSegmentType);

	/*
	*	LISTENER IMPLEMENTATIONS FROM THE ENGINE
	*/
	
	TBool MouseEntered( RPointerArray<CSvgElementImpl>& aElements, TInt aX, TInt aY );

	TBool MouseExited( RPointerArray<CSvgElementImpl>& aElements, TInt aX, TInt aY );

	TBool MouseMoved( RPointerArray<CSvgElementImpl>& aElements, TInt aX, TInt aY );
	
	TBool MousePressed( RPointerArray<CSvgElementImpl>& aElements, TInt aX, TInt aY );

	TBool MouseReleased( RPointerArray<CSvgElementImpl>& aElements, TInt aX, TInt aY );

public:
	CSvgDocumentImpl* iTempDoc;

private:

		TBool IsAnimationElemId( const TInt aElementId );
    /**
     * Gets The corresponding JSR-to-SVG element id. Ids of the elements differ in the svg
     * engine and the JSR-API's.
     */
    TInt SvgGetElementTypeMappingJSRtoSVG( SvgAttrType aType );
    /**
     * Gets The corresponding SVG-to-JSR element id. Ids of the elements differ in the svg
     * engine and the JSR-API's.
     */
    SvgAttrType  SvgGetElementTypeMappingSVGtoJSR( const TInt aElementId);

	/**
	* Gets the attribute type mapping from SVG to JSR. Attribute t
	*
	*/
	SvgAttrType  SvgGetAttributeTypeMappingSVGtoJSR( const TInt aElementId);
	/**
	* Gets the attribute type mapping from JSR to SVG. Attribute t
	*
	*/
	 TInt SvgGetAttributeTypeMappingJSRtoSVG( SvgAttrType aType );
	/**
	* Gets the Enumeration value mapping from JSR  to SVG. Attribute t
	*
	*/
	 TInt  SvgEnumerationMappingJSRtoSVG(const TInt aAttributeId, TInt aJsrEnumValue);
	/**
	* Gets the Enumeration value to string  mapping from JSR  to SVG. Attribute t
	*
	*/
	 TInt  SvgEnumerationtoStringMappingJSRtoSVG(const TInt aAttributeId, short aJsrEnumValue, TDes& aValue);
	/**
	* Gets the string to enumeration value mapping from SVG to JSR. Attribute t
	*
	*/
	 TInt  SvgStringtoEnumerationMappingSVGtoJSR(const TInt aAttrbuteId , TPtrC16 aValue);
	/**
	* Gets the Enumeration value mapping from JSR  to SVG. Attribute t
	*
	*/
	 TInt  SvgEnumerationMappingSVGtoJSR(const TInt aAttributeId, TInt32 aSvgEnumValue);
	
private:
	    CSvgJavaInterfaceImpl();
	    void ConstructL( TFontSpec& aFontSpec );
	    void ConstructL();

private:

};

#endif