/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   
*
*/



#ifndef HUIFXCONSTANTS_H_
#define HUIFXCONSTANTS_H_

#include <e32cmn.h>

enum THuiFxVisualSrcType
    {
    EVisualSrcVisual, // default
    EVisualSrcBitmap, // external bitmap
    // the external inputs are not supported yet.
    EVisualSrcInput1, // first participant in an effect between two visuals
    EVisualSrcInput2, // second participant in an effect between two visuals
    };

enum THuiFxBlendingMode
    {
    EBlendingModeReplace,
    EBlendingModeOver,
    EBlendingModeMultiply,
    EBlendingModeAdditive,
    EBlendingModeDarken,
    EBlendingModeLighten,
    };

enum THuiFxLayerType 
    {
    ELayerTypeUnknown,
    ELayerTypeGroup,
    ELayerTypeFilter,
    ELayerTypeVisual,
    };

enum THuiFxFilterType
    {
    EFilterTypeUnknown,
    EFilterTypeDesaturate,
    EFilterTypeBlur,
    EFilterTypeGlow,
    EFilterTypeBrightnessContrast,
    EFilterTypeHSL, 
    EFilterTypeColorize,
    EFilterTypeOutline,
    EFilterTypeBevel,
    EFilterTypeTransform,
    // etc.
    };

enum TRenderBufferType
    {
    EBufferTypeOnscreen = 0,
    EBufferTypeOffscreen
    };

enum THuiFxRenderbufferUsage
    {
    ERenderbufferUsageReadWrite,
    ERenderbufferUsageReadOnly,
    ERenderbufferUsageWriteOnly,
    };

enum THuiFxEngineType
    {
    EHuiFxEngineVg10 = 0,
    EHuiFxEngineGles20
    };

// All literals needed in the parsing are collecter here because it is difficult
// to find them in several different files.
// Some literals were even defined twice in different .cpp files

_LIT(KLitLayers, "layers" );
_LIT(KLitLayerGroup, "layergroup" );
_LIT(KLitFilter, "filter" );
_LIT(KLitVisual, "visual" );

// parameter for visual
_LIT(KLitSrc, "src" ); // source for the effect
_LIT(KLitOpaque, "opaque");
_LIT(KLitBlending, "blending" );
_LIT(KLitParam, "param" );
_LIT(KLitReplace, "replace" );
_LIT(KLitOver, "over" );
_LIT(KLitMultiply, "multiply" );
_LIT(KLitAdditive, "additive" );
_LIT(KLitDarken, "darken" );
_LIT(KLitLighten, "lighten" );

_LIT(KLitType, "type" );
_LIT(KLitName, "name" );
_LIT(KLitRef, "ref" );
_LIT(KLitAnim, "anim" );
_LIT(KLitDuration, "duration" );

_LIT(KLitStyle, "style" );
_LIT(KLitKeyFrame, "keyframe" );
_LIT(KLitHold, "hold" );
_LIT(KLitLinear, "linear" );
_LIT(KLitQuadraticBezier, "quadratic_bezier" );
_LIT(KLitCubicBezier, "cubic_bezier" );
_LIT(KLitInQuad, "inquad" );
_LIT(KLitOutQuad, "outquad" );
_LIT(KLitInOutQuad, "inoutquad" );
_LIT(KLitOutInQuad, "outinquad" );
_LIT(KLitInBack, "inback" );
_LIT(KLitOutBack, "outback" );
_LIT(KLitInOutBack, "inoutback" );
_LIT(KLitOutInBack, "outinback" );
/*
_LIT(KLitDeclerate, "declerate");
_LIT(KLitAccelerate, "accelerate");
_LIT(KLitImpulse, "impulse");
*/
_LIT(KLitTrue,"true");
_LIT(KLitFalse,"false");
_LIT(KLitAt, "at" );
_LIT(KLitMarker, "marker" );
_LIT(KLitLoopStart, "loop_start" );
_LIT(KLitLoopEnd, "loop_end" );
_LIT(KLitAux1, "aux1" );
_LIT(KLitAux2, "aux2" );
_LIT(KLitVisualWidth1, "visual_width" ); // deprecated
_LIT(KLitVisualHeight1, "visual_height" ); // deprecated
_LIT(KLitDisplayWidth1, "display_width" ); // deprecated
_LIT(KLitDisplayHeight1, "display_height" ); // deprecated

_LIT(KLitVisualWidth, "visual.width" );
_LIT(KLitVisualHeight, "visual.height" );
_LIT(KLitVisualTop, "visual.top" ); // top y coordinate
_LIT(KLitVisualBottom, "visual.bottom" ); // bottom y coordinate
_LIT(KLitVisualLeft, "visual.left" ); // left x coordinate
_LIT(KLitVisualRight, "visual.right" ); // right x coordinate
_LIT(KLitDisplayHeightMinusVisualTop, "visual.top.reversed");

_LIT(KLitDisplayWidth, "screen.width" );
_LIT(KLitDisplayHeight, "screen.height" );
_LIT(KLitDisplayTop, "screen.top" );
_LIT(KLitDisplayBottom, "screen.bottom" );
_LIT(KLitDisplayLeft, "screen.left" );
_LIT(KLitDisplayRight, "screen.right" );

_LIT(KLitExtRectWidth, "extrect.width" );
_LIT(KLitExtRectHeight, "extrect.height" );
_LIT(KLitExtRectTop, "extrect.top" );
_LIT(KLitExtRectBottom, "extrect.bottom" );
_LIT(KLitExtRectLeft, "extrect.left" );
_LIT(KLitExtRectRight, "extrect.right" );


_LIT(KLitDesaturate, "desaturate" );
_LIT(KLitBlur, "blur" );
_LIT(KLitGlow, "glow" );
_LIT(KLitBrightnessContrast, "brightness_contrast" );
_LIT(KlitHSL, "hsl");
_LIT(KlitColorize, "colorize");
_LIT(KlitOutline, "outline");
_LIT(KlitBevel, "bevel");
_LIT(KlitTransform, "transform");
_LIT(KLitDropShadow, "drop_shadow");

_LIT(KLitItem, "item" ); // the visual itself, default if src has not been defined
_LIT(KLitInput1, "input1" );
_LIT(KLitInput2, "input2" );

_LIT(KLitBlurX, "blur_x" );
_LIT(KLitBlurY, "blur_y" );

_LIT(KLitHue, "hue" );
_LIT(KLitSaturation, "saturation" );
_LIT(KLitLightness, "lightness" );

_LIT(KLitOutlineX, "outline_x" );
_LIT(KLitOutlineY, "outline_y" );
_LIT(KLitSteepness, "steepness" );
/*_LIT(KLitRed, "red" );
_LIT(KLitGreen, "green" );
_LIT(KLitBlue, "blue" );
*/

_LIT(KLitBrightness, "brightness" );
_LIT(KLitContrast, "contrast" );
 
_LIT(KLitColor, "color" );

_LIT(KLitOpacityParameter, "opacity" );

_LIT(KLitStart, "start" );
_LIT(KLitEnd, "end" );

// Definitions common to visual layer and transform filter  
_LIT(KLitTranslationX, "translation_x");
_LIT(KLitTranslationY, "translation_y");
_LIT(KLitTranslationZ, "translation_z");

_LIT(KLitScaleX, "scale_x");
_LIT(KLitScaleY, "scale_y");
_LIT(KLitScaleZ, "scale_z");
_LIT(KLitScaleOriginX, "scale_origin_x");
_LIT(KLitScaleOriginY, "scale_origin_y");
_LIT(KLitScaleOriginZ, "scale_origin_z");

_LIT(KLitRotationOriginX, "rotation_origin_x");
_LIT(KLitRotationOriginY, "rotation_origin_y");
_LIT(KLitRotationOriginZ, "rotation_origin_z");
_LIT(KLitRotationAngle, "rotation_angle");
_LIT(KLitRotationAxisX, "rotation_axis_x");
_LIT(KLitRotationAxisY, "rotation_axis_y");
_LIT(KLitRotationAxisZ, "rotation_axis_z");

_LIT(KLitSkewAngleX, "skew_angle_x");
_LIT(KLitSkewAngleY, "skew_angle_y");
_LIT(KLitSkewAngleZ, "skew_angle_z");
_LIT(KLitSkewOriginX, "skew_origin_x");
_LIT(KLitSkewOriginY, "skew_origin_y");
_LIT(KLitSkewOriginZ, "skew_origin_z");


    
#endif /*HUIFXCONSTANTS_H_*/
