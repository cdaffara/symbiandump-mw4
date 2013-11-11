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
* Description:  SVG Implementation source file
 *
*/



#include "SVGSchemaData.h"

//ELEMENT LITERALS
_LIT(KPATH, "path");
_LIT(KSTOP, "stop");
_LIT(KLINEARGRADIENT, "linearGradient");
_LIT(KG, "g");
_LIT(KRECT, "rect");
_LIT(KPOLYGON, "polygon");
_LIT(KSVG, "svg");
_LIT(KRADIALGRADIENT, "radialGradient");
_LIT(KCIRCLE, "circle");
_LIT(KLINE, "line");
_LIT(KPOLYLINE, "polyline");
_LIT(KELLIPSE, "ellipse");
_LIT(KDEFS, "defs");
_LIT(KFOREIGNOBJECT, "foreignObject");
_LIT(KALTGLYPH, "altGlyph");
_LIT(KALTGLYPHDEF, "altGlyphDef");
_LIT(KDESC, "desc");
_LIT(KSCRIPT, "script");
_LIT(KSWITCH, "switch");
_LIT(KMETADATA, "metadata");
_LIT(KTITLE, "title");
_LIT(KFONTFACENAME, "font-face-name");
_LIT(KFONTFACESRC, "font-face-src");
_LIT(KFONTFACEURI, "font-face-uri");
_LIT(KGLYPHREF, "glyphRef");
_LIT(KVKERN, "vkern");
_LIT(KVIEW, "view");
_LIT(KHKERN, "hkern");
_LIT(KA, "a");
_LIT(KFONT, "font");
_LIT(KFONTFACE, "font-face");
_LIT(KGLYPH, "glyph");
_LIT(KIMAGE, "image");
_LIT(KMISSINGGLYPH, "missing-glyph");
_LIT(KSTYLE, "style");
_LIT(KTEXT, "text");
_LIT(KUSE, "use");
_LIT(KANIMATE, "animate");
_LIT(KANIMATECOLOR, "animateColor");
_LIT(KANIMATEMOTION, "animateMotion");
_LIT(KANIMATETRANSFORM, "animateTransform");
_LIT(KSET, "set");
_LIT(KMPATH, "mpath");
_LIT(KTEXTAREA, "textArea");
_LIT(KDISCARD, "discard");
_LIT(KSOLIDCOLOR, "solidColor");
_LIT(KANIMATION, "animation");
_LIT(KAUDIO, "audio");
_LIT(KNOTFOUND, "NOT FOUND");

//ATTRIBUTE LITERALS
_LIT(KFILL, "fill");
_LIT(KSOLIDCOLORATT, "solid-color");
_LIT(KSTROKE, "stroke");
_LIT(KFILLOPACITY, "fill-opacity");
_LIT(KSOLIDOPACITY, "solid-opacity");
_LIT(KSTROKEOPACITY, "stroke-opacity");
_LIT(KOPACITY, "opacity");
_LIT(KSTROKEWIDTH, "stroke-width");
_LIT(KVISIBILITY, "visibility");
_LIT(KFONTFAMILY, "font-family");
_LIT(KFONTSIZE, "font-size");
_LIT(KFONTSTYLE, "font-style");
_LIT(KFONTWEIGHT, "font-weight");
_LIT(KSTROKEDASHARRAY, "stroke-dasharray");
_LIT(KDISPLAY, "display");
_LIT(KFILLRULE, "fill-rule");
_LIT(KSTROKELINECAP, "stroke-linecap");
_LIT(KSTROKELINEJOIN, "stroke-linejoin");
_LIT(KSTROKEDASHOFFSET, "stroke-dashoffset");
_LIT(KSTROKEMITERLIMIT, "stroke-miterlimit");
_LIT(KCOLOR, "color");
_LIT(KTEXTANCHOR, "text-anchor");
_LIT(KTEXTDECORATION, "text-decoration");
_LIT(KLETTERSPACING, "letter-spacing");
_LIT(KWORDSPACING, "word-spacing");
_LIT(KCOLORINTERPOLATION, "color-interpolation");
_LIT(KCOLORRENDERING, "color-rendering");
_LIT(KVOLUME, "volume");
_LIT(KAUDIOLEVEL, "audio-level");

_LIT(KPAD, "pad");
_LIT(KREFLECT, "reflect");
_LIT(KREPEAT, "repeat");


_LIT(KNORMAL, "normal");
_LIT(KITALIC, "italic");
_LIT(KOBLIQUE, "oblique");
_LIT(KINHERIT, "inherit");

_LIT(KBOLD, "bold");
_LIT(KBOLDER, "bolder");
_LIT(KLIGHTER, "lighter");
_LIT(K100, "100");
_LIT(K200, "200");
_LIT(K300, "300");
_LIT(K400, "400");
_LIT(K500, "500");
_LIT(K600, "600");
_LIT(K700, "700");
_LIT(K800, "800");
_LIT(K900, "900");

_LIT(KSTART, "start");
_LIT(KMIDDLE, "middle");
_LIT(KEND, "end");
_LIT(KNONE, "none");
_LIT(KUNDERLINE, "underline");
_LIT(KOVERLINE, "overline");
_LIT(KLINETHROUGH, "line-through");

_LIT(KD, "d");
_LIT(KOFFSET, "offset");
_LIT(KGRADIENTUNITS, "gradientUnits");
_LIT(KX1, "x1");
_LIT(KY1, "y1");
_LIT(KX2, "x2");
_LIT(KY2, "y2");
_LIT(KWIDTH, "width");
_LIT(KHEIGHT, "height");
_LIT(KGRADIENTTRANSFORM, "gradientTransform");
_LIT(KX, "x");
_LIT(KY, "y");
_LIT(KPOINTS, "points");
_LIT(KVIEWBOX, "viewBox");
_LIT(KRX, "rx");
_LIT(KRY, "ry");
_LIT(KCX, "cx");
_LIT(KCY, "cy");
_LIT(KR, "r");
_LIT(KFX, "fx");
_LIT(KFY, "fy");
_LIT(KPATHLENGTH, "pathLength");
_LIT(KSPREADMETHOD, "spreadMethod");
_LIT(KSTOPCOLOR, "stop-color");
_LIT(KSTOPOPACITY, "stop-opacity");
_LIT(KTEXTLENGTH, "textLength");
_LIT(KHORIZADVX, "horiz-adv-x");
_LIT(KHORIZORIGINX, "horiz-origin-x");
_LIT(KHORIZORIGINY, "horiz-origin-y");
_LIT(KASCENT, "ascent");
_LIT(KDESCENT, "descent");
_LIT(KALPHABETIC, "alphabetic");
_LIT(KUNDERLINEPOSITION, "underline-position");
_LIT(KUNDERLINETHICKNESS, "underline-thickness");
_LIT(KOVERLINEPOSITION, "overline-position");
_LIT(KOVERLINETHICKNESS, "overline-thickness");
_LIT(KSTRIKETHROUGHPOSITION, "strikethrough-position");
_LIT(KSTRIKETHROUGHTHICKNESS, "strikethrough-thickness");
_LIT(KUNITSPEREM, "units-per-em");
_LIT(KK, "k");
_LIT(KG1, "g1");
_LIT(KG2, "g2");
_LIT(KU1, "u1");
_LIT(KU2, "u2");
_LIT(KROTATE, "rotate");
_LIT(KCDATA, "cdata");
_LIT(KUNICODE, "unicode");
_LIT(KGLYPHNAME, "glyph-name");
_LIT(KLANG, "lang");
_LIT(KVERSION, "version");
_LIT(KATTRIBUTENAME, "attributeName");
_LIT(KTYPE, "type");
_LIT(KTRANSFORM, "transform");
_LIT(KFROM, "from");
_LIT(KTO, "to");
_LIT(KBY, "by");
_LIT(KVALUES, "values");
_LIT(KTARGET, "target");
_LIT(KKEYSPLINES, "keySplines");
_LIT(KKEYTIMES, "keyTimes");
// Runtime Sync Attributes
_LIT( KSYNCBEHAVIORDEFAULT, "syncBehaviorDefault" );
_LIT( KSYNCTOLERANCEDEFAULT,"syncToleranceDefault" );
_LIT( KSYNCBEHAVIOR, "syncBehavior" );
_LIT( KSYNCTOLERANCE, "syncTolerance" );
_LIT( KSYNCMASTER, "syncMaster" );
_LIT( KINITIALVISIBILITY, "initialVisibility" );

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSchemaData::ConstructL()
    {


    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgSchemaData* CSvgSchemaData::NewL()
    {
    CSvgSchemaData* self = new ( ELeave ) CSvgSchemaData();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgSchemaData* CSvgSchemaData::NewLC()
    {
    CSvgSchemaData* self = new ( ELeave ) CSvgSchemaData();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgSchemaData::~CSvgSchemaData()
    {

    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgSchemaData::CSvgSchemaData()
    {
    }


/********************* Methods to generate SchemaData Literals ******************/

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSchemaData::GetSVGElementId(const TDesC& aElement)
{
	if (aElement == KPATH )
    	{
       	return KSvgPathElement;
     	}
    if (aElement == KSTOP )
        {
        return KSvgStopElement;
        }
    if (aElement == KLINEARGRADIENT )
        {
        return KSvgLinearGradientElement;
        }
    if (aElement == KG)
        {
        return KSvgGElement;
        }
    if (aElement == KRECT )
        {
        return KSvgRectElement;
        }
    if (aElement == KPOLYGON )
        {
        return KSvgPolygonElement;
        }
    if (aElement == KSVG)
        {
        return KSvgSvgElement;
        }
    if (aElement == KRADIALGRADIENT )
        {
        return KSvgRadialGradientElement;
        }
    if (aElement == KCIRCLE )
        {
        return KSvgCircleElement;
        }
    if (aElement == KLINE )
        {
        return KSvgLineElement;
        }
    if (aElement == KPOLYLINE )
        {
        return KSvgPolylineElement;
        }
    if (aElement == KELLIPSE )
        {
        return KSvgEllipseElement;
        }
    if (aElement == KDEFS)
        {
        return KSvgDefsElement;
        }
    if (aElement == KFOREIGNOBJECT)
        {
        return KSvgForeignObjectElement;
        }
    if (aElement == KALTGLYPH)
        {
        return KSvgAltglyphElement;
        }
    if (aElement == KALTGLYPHDEF )
        {
        return KSvgAltglyphdefElement;
        }
    if (aElement == KDESC )
        {
        return KSvgDescElement;
        }
    if (aElement == KSCRIPT )
        {
        return KSvgScriptElement ;
        }
    if (aElement == KSWITCH )
        {
        return KSvgSwitchElement;
        }
    if (aElement == KMETADATA)
        {
        return KSvgMetadataElement;
        }
    if (aElement == KTITLE)
        {
        return KSvgTitleElement;
        }
    if (aElement == KFONTFACENAME)
        {
        return KSvgFontfacenameElement;
        }
    if (aElement == KFONTFACESRC)
        {
        return KSvgFontfacesrcElement;
        }
    if (aElement == KFONTFACEURI)
        {
        return KSvgFontfaceuriElement;
        }
    if (aElement == KGLYPHREF)
        {
        return KSvgGlyphrefElement;
        }
    if (aElement == KVKERN)
        {
        return KSvgVkernElement;
        }
    if (aElement == KVIEW )
        {
        return KSvgViewElement;
        }
    if (aElement == KHKERN )
        {
        return KSvgHkernElement;
        }
    if (aElement == KA )
        {
        return KSvgAElement;
        }
    if (aElement == KFONT )
        {
        return KSvgFontElement;
        }
    if (aElement == KFONTFACE )
        {
        return KSvgFontfaceElement;
        }
    if (aElement == KGLYPH )
        {
        return KSvgGlyphElement;
        }
    if (aElement == KIMAGE )
        {
        return KSvgImageElement;
        }
    if (aElement == KMISSINGGLYPH )
        {
        return KSvgMissingglyphElement;
        }
    if (aElement == KSTYLE )
        {
        return KSvgStyleElement;
        }
    if (aElement == KTEXT )
        {
        return KSvgTextElement;
        }
    if (aElement == KUSE )
        {
        return KSvgUseElement;
        }
    if (aElement == KANIMATE )
        {
        return KSvgAnimateElement;
        }
    if (aElement == KANIMATECOLOR )
        {
        return KSvgAnimateColorElement;
        }
    if (aElement == KANIMATEMOTION )
        {
        return KSvgAnimateMotionElement;
        }
    if (aElement == KANIMATETRANSFORM )
        {
        return KSvgAnimateTransformElement;
        }
    if (aElement == KSET )
        {
        return KSvgSetElement;
        }
    if (aElement == KMPATH )
        {
        return KSvgMpathElement;
        }
    if (aElement == KTEXTAREA )
        {
        return KSvgTextAreaElement;
        }
    if (aElement == KDISCARD )
        {
        return KSvgDiscardElement;
        }
     if (aElement == KSOLIDCOLOR )
        {
        return KSvgSolidColorElement;
        }
   /* if (aElement == KANIMATION )
        {
        return KSvgAnimationElement;
        }
        */
    if (aElement == KAUDIO )
        {
        return KSvgAudioElement;
        }
//#ifdef RD_SVGT_MEDIAANIMATION_SUPPORT
    if (aElement == KANIMATION )
        {
        return KSvgMediaAnimationElement;
        }
//#endif
    else
        {
        return KErrNotFound;
        }

}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSchemaData::GetPresentationAttributeId(const TDesC& aElement)
{
    if ( (aElement == KFILL ) || (aElement == KSOLIDCOLORATT ) )
        {
        return KCSS_ATTR_FILL;
        }
    if (aElement == KSTROKE )
        {
        return KCSS_ATTR_STROKE;
        }
    if ( (aElement == KFILLOPACITY) || (aElement == KSOLIDOPACITY) )
        {
        return KCSS_ATTR_FILL_OPACITY;
        }
    if (aElement == KSTROKEOPACITY )
        {
        return KCSS_ATTR_STROKE_OPACITY;
        }
    if (aElement == KOPACITY )
        {
        return KCSS_ATTR_GROUP_OPACITY;
        }
    if (aElement == KSTROKEWIDTH )
        {
        return KCSS_ATTR_STROKEWIDTH;
        }
    if (aElement == KVISIBILITY )
        {
        return KCSS_ATTR_VISIBILITY;
        }
    if (aElement == KFONTFAMILY )
        {
        return KCSS_ATTR_FONTFAMILY;
        }
    if (aElement == KFONTSIZE )
        {
        return KCSS_ATTR_FONTSIZE;
        }
    if (aElement == KFONTSTYLE )
        {
        return KCSS_ATTR_FONTSTYLE;
        }
    if (aElement == KFONTWEIGHT )
        {
        return KCSS_ATTR_FONTWEIGHT;
        }
    if (aElement == KSTROKEDASHARRAY )
        {
        return KCSS_ATTR_STROKE_DASHARRAY;
        }
    if (aElement == KDISPLAY )
        {
        return KCSS_ATTR_DISPLAY;
        }
    if (aElement == KFILLRULE )
        {
        return KCSS_ATTR_FILLRULE;
        }
    if (aElement == KSTROKELINECAP )
        {
        return KCSS_ATTR_STROKE_LINECAP;
        }
    if (aElement == KSTROKELINEJOIN )
        {
        return KCSS_ATTR_STROKE_LINEJOIN;
        }
    if (aElement == KSTROKEDASHOFFSET )
        {
        return KCSS_ATTR_STROKE_DASHOFFSET;
        }
    if (aElement == KSTROKEMITERLIMIT )
        {
        return KCSS_ATTR_STROKE_MITERLIMIT;
        }
    if (aElement == KCOLOR )
        {
        return KCSS_ATTR_COLOR;
        }
    if (aElement == KTEXTANCHOR )
        {
        return KCSS_ATTR_TEXTANCHOR;
        }
    if (aElement == KTEXTDECORATION )
        {
        return KCSS_ATTR_TEXTDECORATION;
        }
    if (aElement == KLETTERSPACING )
        {
        return KCSS_ATTR_LETTERSPACING;
        }
    if (aElement == KWORDSPACING )
        {
        return KCSS_ATTR_WORDSPACING;
        }
    if (aElement == KCOLORINTERPOLATION )
        {
        return KCSS_ATTR_COLORINTERPOLATION;
        }
    if (aElement == KCOLORRENDERING )
        {
        return KCSS_ATTR_COLORRENDERING;
        }
    else
        {
        return KErrNotFound;
        }

}

TInt CSvgSchemaData::FindSpreadMethod(const TDesC& aElement)
{
    if(aElement == KPAD )
        {
        return 0;
        }
    if(aElement == KREFLECT )
        {
        return 1;
        }
    if(aElement == KREPEAT )
        {
        return 2;
        }
    else
        {
        return KErrNotFound;
        }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSchemaData::FindFontStyle(const TDesC& aElement)
{
    if(aElement == KNORMAL )
        {
        return 0;
        }
    if(aElement == KITALIC )
        {
        return 1;
        }
    if(aElement == KOBLIQUE )
        {
        return 2;
        }
    if(aElement == KINHERIT )
        {
        return 3;
        }
    else
        {
        return KErrNotFound;
        }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSchemaData::FindFontWeightEnum(const TDesC& aElement)
{
    if(aElement == KNORMAL )
        {
        return 0;
        }
    if (aElement == KBOLD )
        {
        return 1;
        }
    if (aElement == KBOLDER )
        {
        return 2;
        }
    if (aElement == KLIGHTER )
        {
        return 3;
        }
    if (aElement == K100 )
        {
        return 4;
        }
    if (aElement == K200 )
        {
        return 5;
        }
    if (aElement == K300 )
        {
        return 6;
        }
    if (aElement == K400 )
        {
        return 7;
        }
    if (aElement == K500 )
        {
        return 8;
        }
    if (aElement == K600 )
        {
        return 9;
        }
    if (aElement == K700 )
        {
        return 10;
        }
    if (aElement == K800 )
        {
        return 11;
        }
    if (aElement == K900 )
        {
        return  12;
        }
    if (aElement == KINHERIT )
        {
        return 13;
        }
    else
        {
        return KErrNotFound;
        }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSchemaData::FindTextAnchorEnum(const TDesC& aElement)
{
    if(aElement == KSTART )
        {
        return 0;
        }
    if (aElement == KMIDDLE )
        {
        return 1;
        }
    if (aElement == KEND )
        {
        return 2;
        }
    else
        {
        return -1;
        }

}



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSchemaData::FindTextDecorationEnum(const TDesC& aElement)
{
    if (aElement == KNONE )
        {
        return 0;
        }
    if (aElement == KUNDERLINE )
        {
        return 1;
        }
    if (aElement == KOVERLINE )
        {
        return 2;
        }
    if (aElement == KLINETHROUGH )
        {
        return 3;
        }

    //Inherit & Blink is not supported currently
    else
        {
        return -1;
        }

}

TUint16 CSvgSchemaData::GetSVGTAttributeId(const TDesC& aAttrName)
{
    if( aAttrName == KD )
        {
        return KAtrD;
        }
    if(aAttrName == KOFFSET )
        {
         return KAtrOffset;
        }
    if( aAttrName == KSTYLE )
        {
        return KAtrStyle;
        }
    if(aAttrName == KGRADIENTUNITS )
        {
        return KAtrGradientUnits;
        }
    if( aAttrName == KX1 )
        {
        return KAtrX1;
        }
    if( aAttrName == KY1 )
        {
        return KAtrY1;
        }
    if( aAttrName == KX2 )
        {
        return KAtrX2;
        }
    if( aAttrName == KY2 )
        {
        return KAtrY2;
        }
    if( aAttrName == KWIDTH )
        {
        return KAtrWidth;
        }
    if( aAttrName == KHEIGHT )
        {
        return KAtrHeight;
        }
    if( aAttrName == KGRADIENTTRANSFORM )
        {
        return KAtrGradientTransform;
        }
    if( aAttrName == KX )
        {
        return KAtrX;
        }
    if( aAttrName == KY )
        {
        return KAtrY;
        }
    if( aAttrName == KPOINTS )
        {
        return KAtrPoints;
        }
	if (aAttrName == KVIEWBOX )
        {
        return KAtrViewBox;
        }
    if( aAttrName == KRX )
        {
        return KAtrRx;
        }
    if( aAttrName == KRY )
        {
        return KAtrRy;
        }
    if( aAttrName == KCX )
        {
        return KAtrCx;
        }
    if( aAttrName == KCY )
        {
        return KAtrCy;
        }
    if( aAttrName == KR )
        {
        return KAtrR;
        }
    if(aAttrName == KFX )
        {
        return KAtrFx;
        }
    if(aAttrName == KFY )
        {
        return KAtrFy;
        }
    if( aAttrName == KPATHLENGTH )
        {
        return KAtrPathLength;
        }
    if( aAttrName == KSTROKEWIDTH )
        {
        return KAtrStrokeWidth;
        }
    if(aAttrName == KSPREADMETHOD )
        {
        return KAtrSpreadMethods;
        }
    if(aAttrName == KSTOPCOLOR )
        {
        return KAtrStopColor;
        }
    if(aAttrName == KSTOPOPACITY )
        {
        return KAtrStopOpacity;
        }
    if( aAttrName == KTEXTLENGTH )
        {
        return KAtrTextLength;
        }
    if( aAttrName == KHORIZADVX )
        {
        return KAtrHorizAdvX;
        }
    if( aAttrName == KHORIZORIGINX )
        {
        return KAtrHorizOriginX;
        }
    if( aAttrName == KHORIZORIGINY )
        {
        return KAtrHorizOriginY;
        }
    if( aAttrName == KASCENT )
        {
        return KAtrAscent;
        }
    if( aAttrName == KDESCENT )
        {
        return KAtrDescent;
        }
    if( aAttrName == KALPHABETIC )
        {
        return KAtrAlphabetic;
        }
    if( aAttrName == KUNDERLINEPOSITION )
        {
        return KAtrUnderlinePosition;
        }
    if( aAttrName == KUNDERLINETHICKNESS )
        {
        return KAtrUnderlineThickness;
        }
    if( aAttrName == KOVERLINEPOSITION )
        {
        return KAtrOverlinePosition;
        }
    if( aAttrName == KOVERLINETHICKNESS )
        {
        return KAtrOverlineThickness;
        }
    if( aAttrName == KSTRIKETHROUGHPOSITION )
        {
        return KAtrStrikethroughPosition;
        }
    if( aAttrName == KSTRIKETHROUGHTHICKNESS )
        {
        return KAtrStrikethroughThickness;
        }
    if( aAttrName == KUNITSPEREM )
        {
        return KAtrUnitsPerEm;
        }
    if( aAttrName == KWORDSPACING )
        {
        return KAtrWordSpacing;
        }
    if( aAttrName == KLETTERSPACING )
        {
        return KAtrLetterSpacing;
        }
    if( aAttrName == KK )
        {
        return KAtrK;
        }
    if( aAttrName == KG1 )
        {
        return KAtrG1;
        }
    if( aAttrName == KG2 )
        {
        return KAtrG2;
        }
    if( aAttrName == KU1 )
        {
        return KAtrU1;
        }
    if( aAttrName == KU2 )
        {
        return KAtrU2;
        }
    if( aAttrName == KTEXTDECORATION )
        {
        return KAtrTextDecoration;
        }
    if( aAttrName == KROTATE )
        {
        return KAtrRotate;
        }
    if( aAttrName == KCDATA )
        {
        return KAtrCdata;
        }
    if( aAttrName == KUNICODE )
        {
        return KAtrUnicode;
        }
    if( aAttrName == KGLYPHNAME )
        {
        return KAtrGlyphName;
        }
    if( aAttrName == KLANG )
        {
        return KAtrLang;
        }
    if( aAttrName == KVERSION )
        {
        return KAtrVersion;
        }
    if( aAttrName == KATTRIBUTENAME )
        {
        return KAtrAttributeName;
        }
    if( aAttrName == KTYPE )
        {
        return KAtrType;
        }
    if( aAttrName == KTRANSFORM )
        {
        return KAtrTransform;
        }
    if( aAttrName == KFROM )
        {
        return KAtrFrom;
        }
    if( aAttrName == KTO )
        {
        return KAtrTo;
        }
    if( aAttrName == KBY )
        {
        return KAtrBy;
        }
    if (aAttrName == KVALUES )
        {
        return KAtrValues;
        }
    if (aAttrName == KKEYSPLINES)
        {
    	return KAtrKeySplines;
        }
    if (aAttrName == KKEYTIMES)
       {
       return KAtrKeyTimes;	
       }
	if (aAttrName == KTARGET )
	    {
		return KAtrTarget;
	    }
	if (aAttrName == KSOLIDOPACITY )
	    {
		return KAtrSolidOpacity;
	    }
    if ( aAttrName == KSYNCBEHAVIORDEFAULT )
        {
        return KAtrSyncBehaviorDefault;
        }
    if ( aAttrName == KSYNCTOLERANCEDEFAULT )    
        {
        return KAtrSyncToleranceDefault;
        }
    if ( aAttrName == KSYNCBEHAVIOR )
        {
        return KAtrSyncBehavior;
        }
    if ( aAttrName == KSYNCTOLERANCE )
        {
        return KAtrSyncTolerance;
        }        
    if ( aAttrName == KSYNCMASTER )
        {
        return KAtrSyncMaster;
        }
    if ( aAttrName == KINITIALVISIBILITY )
        {
        return KAtrInitialVisibility;
        }    
    if ( aAttrName == KVOLUME )
        {
        return KAtrVolume;
        } 
    if ( aAttrName == KAUDIOLEVEL )
        {
        return KAtrAudioLevel;
        }    
    else
        {
        return (TUint16)KErrNotFound;
        }
}




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgSchemaData::GetSVGElementName(const TUint8 aElement, TDes& aElemName)
{
    if (aElement == KSvgPathElement )
        {
        aElemName.Copy( KPATH );
        return ;

        }
    if (aElement == KSvgStopElement )
        {
        aElemName.Copy( KSTOP );
        return ;

        }
    if (aElement == KSvgLinearGradientElement )
        {
        aElemName.Copy( KLINEARGRADIENT );
        return ;

        }
    if (aElement == KSvgGElement)
        {
        aElemName.Copy( KG );
        return ;

        }
    if (aElement == KSvgRectElement )
        {
        aElemName.Copy( KRECT );
        return ;

        }
    if (aElement == KSvgPolygonElement)
        {
        aElemName.Copy( KPOLYGON );
        return ;

        }
    if (aElement == KSvgSvgElement)
        {
        aElemName.Copy( KSVG );
        return ;
        }
    if (aElement == KSvgRadialGradientElement )
        {
        aElemName.Copy( KRADIALGRADIENT );
        return ;

        }
    if (aElement == KSvgCircleElement )
        {
        aElemName.Copy( KCIRCLE );
        return ;

        }
    if (aElement == KSvgLineElement )
        {
        aElemName.Copy( KLINE );
        return ;

        }
    if (aElement == KSvgPolylineElement )
        {
        aElemName.Copy( KPOLYLINE );
        return ;

        }
    if (aElement == KSvgEllipseElement )
        {
        aElemName.Copy( KELLIPSE );
        return ;

        }
    if (aElement == KSvgDefsElement)
        {
        aElemName.Copy( KDEFS );
        return ;

        }
    if (aElement == KSvgForeignObjectElement)
        {
        aElemName.Copy( KFOREIGNOBJECT );
        return ;

        }
    if (aElement == KSvgScriptElement)
        {
        aElemName.Copy( KSCRIPT );
        return ;

        }
    if (aElement == KSvgSwitchElement )
        {
        aElemName.Copy( KSWITCH );
        return ;

        }
    if (aElement == KSvgDescElement)
        {
        aElemName.Copy( KDESC );
        return ;

        }
    if (aElement == KSvgAltglyphElement)
        {
        aElemName.Copy( KALTGLYPH );
        return ;

        }
    if (aElement == KSvgAltglyphdefElement)
        {
        aElemName.Copy( KALTGLYPHDEF );
        return ;

        }
    if (aElement == KSvgMetadataElement)
        {
        aElemName.Copy( KMETADATA );
        return ;

        }
    if (aElement == KSvgTitleElement)
        {
        aElemName.Copy( KTITLE );
        return ;

        }
    if (aElement == KSvgFontfacenameElement)
        {
        aElemName.Copy( KFONTFACENAME );
        return ;

        }
    if (aElement == KSvgFontfacesrcElement)
        {
        aElemName.Copy( KFONTFACESRC );
        return ;
        }
    if (aElement == KSvgFontfaceuriElement)
        {
        aElemName.Copy( KFONTFACEURI );
        return ;

        }
    if (aElement == KSvgGlyphrefElement)
        {
        aElemName.Copy( KGLYPHREF );
        return ;

        }
    if (aElement == KSvgVkernElement)
        {
        aElemName.Copy( KVKERN );
        return ;

        }
    if (aElement == KSvgViewElement)
        {
        aElemName.Copy( KVIEW );
        return ;

        }
    if (aElement == KSvgHkernElement )
        {
        aElemName.Copy( KHKERN );
        return ;

        }
    if (aElement == KSvgAElement )
        {
        aElemName.Copy( KA );
        return ;

        }
    if (aElement == KSvgFontElement)
        {
        aElemName.Copy( KFONT );
        return ;

        }
    if (aElement == KSvgFontfaceElement)
        {
        aElemName.Copy( KFONTFACE );
        return ;

        }
    if (aElement == KSvgGlyphElement)
        {
        aElemName.Copy( KGLYPH );
        return ;

        }
    if (aElement == KSvgImageElement )
        {
        aElemName.Copy( KIMAGE );
        return ;

        }
    if (aElement == KSvgMissingglyphElement )
        {
        aElemName.Copy( KMISSINGGLYPH );
        return ;

        }
    if (aElement == KSvgStyleElement)
        {
        aElemName.Copy( KSTYLE );
        return ;

        }
    if (aElement == KSvgTextElement)
        {
        aElemName.Copy( KTEXT );
        return ;

        }
    if (aElement == KSvgUseElement)
        {
        aElemName.Copy( KUSE );
        return ;

        }
    if (aElement == KSvgAnimateElement )
        {
        aElemName.Copy( KANIMATE );
        return ;

        }
    if (aElement == KSvgAnimateColorElement )
        {
        aElemName.Copy( KANIMATECOLOR );
        return ;

        }
    if (aElement == KSvgAnimateMotionElement )
        {
        aElemName.Copy( KANIMATEMOTION );
        return ;

        }
    if (aElement == KSvgAnimateTransformElement )
        {
        aElemName.Copy( KANIMATETRANSFORM );
        return ;

        }
    if (aElement == KSvgSetElement )
        {
        aElemName.Copy( KSET );
        return ;

        }
    if (aElement == KSvgMpathElement )
        {
        aElemName.Copy( KMPATH );
        return ;

        }
   if (aElement == KSvgTextAreaElement )
        {
        aElemName.Copy( KTEXTAREA );
        return ;

        }
    if (aElement == KSvgDiscardElement )
        {
        aElemName.Copy( KDISCARD );
        return ;
        }
/*    if (aElement == KSvgAnimationElement )
        {
        aElemName.Copy( KANIMATION );
        return ;
        }*/
     if (aElement == KSvgSolidColorElement )
        {
        aElemName.Copy( KSOLIDCOLOR );
        return ;
        }
     if (aElement == KSvgAudioElement )
        {
        aElemName.Copy( KAUDIO );
        return ;
        }
    else
        {
        aElemName.Copy( KNOTFOUND );
        return ;

        }
}

TAttributeType CSvgSchemaData::GetAttributeType(const TDesC& aAttrName)
    {
    if(aAttrName == KFILL )
        {
        return EString;
        }
    if( aAttrName == KD )
        {
        return EString;
        }
    if(aAttrName == KOFFSET )
        {
         return EFloat;
        }
    if( aAttrName == KSTYLE )
        {
        return EUndefinedType;
        }
    if(aAttrName == KGRADIENTUNITS )
        {
        return EUndefinedType;
        }
    if( aAttrName == KX1 )
        {
        return EFloat;
        }
    if( aAttrName == KY1 )
        {
        return EFloat;
        }
    if( aAttrName == KX2 )
        {
        return EFloat;
        }
    if( aAttrName == KY2 )
        {
        return EFloat;
        }
    if( aAttrName == KWIDTH )
        {
        return EFloat;
        }
    if( aAttrName == KHEIGHT )
        {
        return EFloat;
        }
    if(aAttrName == KFILLOPACITY )
        {
        return EFloat;
        }
    if( aAttrName == KGRADIENTTRANSFORM )
        {
        return EString;
        }
    if( aAttrName == KX )
        {
        return EFloat;
        }
    if( aAttrName == KY )
        {
        return EFloat;
        }
    if( aAttrName == KPOINTS )
        {
        return EString;
        }
    if (aAttrName == KVIEWBOX )
        {
        return EString;
        }
    if( aAttrName == KRX )
        {
        return EFloat;
        }
    if( aAttrName == KRY )
        {
        return EFloat;
        }
    if( aAttrName == KCX )
        {
        return EFloat;
        }
    if( aAttrName == KCY )
        {
        return EFloat;
        }
    if( aAttrName == KR )
        {
        return EFloat;
        }
    if(aAttrName == KFX )
        {
        return EFloat;
        }
    if(aAttrName == KFY )
        {
        return EFloat;
        }
    if( aAttrName == KPATHLENGTH )
        {
        return EFloat;
        }
    if( aAttrName == KSTROKEWIDTH )
        {
        return EFloat;
        }
    if(aAttrName == KSTROKE )
        {
        return EInteger;
        }
    if(aAttrName == KSPREADMETHOD )
        {
        return EUndefinedType;
        }
    if(aAttrName == KSTOPCOLOR )
        {
        return EUndefinedType;
        }
    if(aAttrName == KSTOPOPACITY )
        {
        return EFloat;
        }
    if( aAttrName == KTEXTLENGTH )
        {
        return EFloat;
        }
    if( aAttrName == KHORIZADVX )
        {
        return EFloat;
        }
    if( aAttrName == KHORIZORIGINX )
        {
        return EFloat;
        }
    if( aAttrName == KHORIZORIGINY )
        {
        return EFloat;
        }
    if( aAttrName == KASCENT )
        {
        return EFloat;
        }
    if( aAttrName == KDESCENT )
        {
        return EFloat;
        }
    if( aAttrName == KALPHABETIC )
        {
        return EUndefinedType;
        }
    if( aAttrName == KUNDERLINEPOSITION )
        {
        return EFloat;
        }
    if( aAttrName == KUNDERLINETHICKNESS )
        {
        return EFloat;
        }
    if( aAttrName == KOVERLINEPOSITION )
        {
        return EFloat;
        }
    if( aAttrName == KOVERLINETHICKNESS )
        {
        return EFloat;
        }
    if( aAttrName == KSTRIKETHROUGHPOSITION )
        {
        return EFloat;
        }
    if( aAttrName == KSTRIKETHROUGHTHICKNESS )
        {
        return EFloat;
        }
    if( aAttrName == KUNITSPEREM )
        {
        return EFloat;
        }
    if( aAttrName == KWORDSPACING )
        {
        return EFloat;
        }
    if( aAttrName == KLETTERSPACING )
        {
        return EFloat;
        }
    if( aAttrName == KK )
        {
        return EFloat;
        }
    if( aAttrName == KG1 )
        {
        return EString;
        }
    if( aAttrName == KG2 )
        {
        return EString;
        }
    if( aAttrName == KU1 )
        {
        return EString;
        }
    if( aAttrName == KU2 )
        {
        return EString;
        }
    if( aAttrName == KTEXTDECORATION )
        {
        return EUndefinedType;
        }
    if( aAttrName == KROTATE )
        {
        return EUndefinedType;
        }
    if( aAttrName == KCDATA )
        {
        return EString;
        }
    if( aAttrName == KUNICODE )
        {
        return EString;
        }
    if( aAttrName == KGLYPHNAME )
        {
        return EString;
        }
    if( aAttrName == KLANG )
        {
        return EString;
        }
    if( aAttrName == KVERSION )
        {
        return EString;
        }
    if( aAttrName == KATTRIBUTENAME )
        {
        return EString;
        }
    if( aAttrName == KTYPE )
        {
        return EUndefinedType;
        }
    if( aAttrName == KTRANSFORM )
        {
        return EString;
        }
    if( aAttrName == KFROM )
        {
        return EFloat;
        }
    if( aAttrName == KTO )
        {
        return EFloat;
        }
    if( aAttrName == KBY )
        {
        return EUndefinedType;
        }
    if (aAttrName == KVALUES )
        {
        return EString;
        }
    if (aAttrName == KSTROKEDASHARRAY )
        {
        return EString;
        }
    if (aAttrName == KSTROKEDASHOFFSET )
        {
        return EFloat;
        }
    if (aAttrName == KTARGET )
    	{
    	return EString;
    	}
    if(aAttrName == KSOLIDOPACITY )
        {
        return EFloat;
        }
    if ( aAttrName == KSYNCBEHAVIORDEFAULT )
        {
        return EString;
        }
    if ( aAttrName == KSYNCTOLERANCEDEFAULT )    
        {
        return EString;
        }
    if ( aAttrName == KSYNCBEHAVIOR )
        {
        return EString;
        }
    if ( aAttrName == KSYNCTOLERANCE )
        {
        return EString;
        }        
    if ( aAttrName == KSYNCMASTER )
        {
        return EInteger;
        }
    if ( aAttrName == KINITIALVISIBILITY )
        {
        return EString;
        }    
    else
        {
        return EUndefinedType;
        }
    }
