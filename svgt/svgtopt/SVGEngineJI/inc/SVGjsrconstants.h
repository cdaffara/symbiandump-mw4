/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 #ifndef _INC_SVGJSRCONSTANTS_
 #define _INC_SVGJSRCONSTANTS_
// The error flag
    #define TEXT_NOT_SHORT  -3
    #define DEFAULT_INHERIT  -2  //  null on native engine
    #define SVG_ERROR  -1
    const TInt KJavaIOException = -10;

    // A constant for each element in SVGT
    #define EL_A  0
    #define EL_ANIMATE  1
    #define EL_ANIMATECOLOR  2
    #define EL_ANIMATEMOTION  3
    #define EL_ANIMATETRANSFORM  4
    #define EL_CIRCLE  5
    #define EL_DEFS  6
    #define EL_DESC  7
    #define EL_ELLIPSE  8
    #define EL_FONT  9
    #define EL_FONTFACE  10
    #define EL_FONTFACENAME  11
    #define EL_FONTFACESRC  12
    #define EL_FOREIGNOBJECT  13
    #define EL_G  14
    #define EL_GLYPH  15
    #define EL_HKERN  16
    #define EL_IMAGE  17
    #define EL_LINE  18
    #define EL_METADATA  19
    #define EL_MISSINGGLYPH  20
    #define EL_MPATH  21
    #define EL_PATH  22
    #define EL_POLYGON  23
    #define EL_POLYLINE  24
    #define EL_RECT  25
    #define EL_SET  26
    #define EL_SVG  27
    #define EL_SWITCH  28
    #define EL_TEXT  29
    #define EL_TITLE  30
    #define EL_USE  31

    // A constant for each attribute in SVGT
    #define AT_ACCENTHEIGHT  50		// Not supported by SVGT Engine
    #define AT_ACCUMULATE  51		// SVGT - Int and JSR Enum
    #define AT_ADDITIVE  52			// SVGT - Int and JSR Enum
    #define AT_ALPHABETIC  53		// SVGT - Float and JSR  
    #define AT_ARABICFORM  54       // Not supported by SVGT Engine  
    #define AT_ASCENT  55			// SVGT - Float and JSR  
    #define AT_ATTRIBUTENAME  56	// SVGT - Int and JSR  
    #define AT_ATTRIBUTETYPE  57	// Not supported by SVGT Engine
    #define AT_BASEPROFILE  58		// SVGT - String and JSR  
    #define AT_BBOX  59				// SVGT doesn't support as such as a attribute but GetBBox can be implemented
    #define AT_BEGIN  60			// SVGT - Int and JSR 
    #define AT_BY  61				// SVGT- different data types JSR  
    #define AT_CALCMODE  62			// SVGT - Int and JSR - Enum
    #define AT_CAPHEIGHT  63		// Not supported by SVGT Engine
    #define AT_COLOR  64			// SVGT - RGB and and JSR - RGB
    #define AT_COLORRENDERING  65	// Not supported by SVGT Engine
    // #define AT_CONTENT					 66
    #define AT_CX  67				// SVGT - Flaot and JSR  
    #define AT_CY  68				// SVGT - Float and JSR 
    #define AT_D  69				// SVGT - Path and JSR 
    #define AT_DESCENT  70			// SVGT - Float and JSR 
    #define AT_DISPLAY  71			// SVGT - Int and JSR Enum
    #define AT_DUR  72				// SVGT - Int and JSR  				
    #define AT_END  73				// SVGT - Int and JSR  
    #define AT_FILL  74				// SVGT - Int and JSR Enum and also SVGT - RGB and JSr - RGB
    #define AT_FILLRULE  75			// SVGT - String and JSR Enum
    #define AT_FONTFAMILY  76		// SVGT - String and JSR String
    #define AT_FONTSIZE  77			// SVGT - Float and JSR - How to convert the JSR Enums
    #define AT_FONTSTRETCH  78		//	Not supported by SVGT Engine	
    #define AT_FONTSTYLE  79		// SVGT -  Enum and JSR - Enum
    #define AT_FONTVARIANT  80		// Not supported by SVGT Engine	
    #define AT_FONTWEIGHT  81		// SVGT -  Enum and JSR - Enum
    #define AT_FROM  82				// SVGT- different data types JSR  
    #define AT_G1  83				// SVGT - String and JSR  
    #define AT_G2  84				// SVGT - String and JSR  
    #define AT_GLYPHNAME  85		// SVGT - String and JSR  
    #define AT_HANGING  86			//	Not supported by SVGT Engine			
    #define AT_HEIGHT  87			// SVGT - Float and JSR 		
    #define AT_HORIZADVX  88		// SVGT - Float and JSR 
    #define AT_HORIZORIGINX  89		// SVGT - Float and JSR 
    #define AT_ID  90				// SVGT - String and JSR  
    #define AT_IDEOGRAPHIC  91		//	Not supported by SVGT Engine
    #define AT_K  92				// SVGT - String and JSR  
    #define AT_KEYPOINTS  93		//	Not supported by SVGT Engine
    #define AT_KEYSPLINES  94		// Not clear how the data will be sent by JSR 
    #define AT_KEYTIMES  95			// Not clear how the data will be sent by JSR 
    #define AT_LANG  96				// SVGT - String and JSR  
    #define AT_MATHEMATICAL  97		//	Not supported by SVGT Engine
    #define AT_MAX  98				//	Not supported by SVGT Engine
    #define AT_MIN  99				//	Not supported by SVGT Engine
    #define AT_NAME  100			//	Not supported by SVGT Engine
    #define AT_ORIGIN  101			//	Not supported by SVGT Engine
    #define AT_OVERLINEPOSITION  102	// SVGT - Float and JSR 
    #define AT_OVERLINETHICKNESS  103	// SVGT - Float and JSR 
    #define AT_PANOSE1  104				//	Not supported by SVGT Engine
    #define AT_PATH  105				// SVGT - Path  and JSR 
    #define AT_PATHLENGTH  106			// Currently on implemented in SVGT
    #define AT_POINTS  107				// SVGT - Path  and JSR 	
    #define AT_PRESERVEASPECTRATIO  108 // SVGT - String  and  JSR - Enum
    #define AT_R  109					// SVGT - Float and JSR 
    #define AT_REPEATCOUNT  110			// SVGT - Int and JSR 
    #define AT_REPEATDUR  111			// SVGT - Int and JSR 
    #define AT_REQUIREDEXTENSIONS  112  // SVGT - String  and  JSR 
    #define AT_REQUIREDFEATURES  113	// SVGT - String  and  JSR 
    #define AT_RESTART  114				// SVGT - Int and JSR 	
    #define AT_ROTATE  115				// JSR 
    #define AT_RX  116					// SVGT - Float and JSR 
    #define AT_RY  117					// SVGT - Float and JSR 
    #define AT_SLOPE  118				//	Not supported by SVGT Engine
    #define AT_STEMH  119				//	Not supported by SVGT Engine
    #define AT_STEMV  120				//	Not supported by SVGT Engine
    #define AT_STRIKETHROUGHPOSITION  121	// SVGT - Float and JSR 	
    #define AT_STRIKETHROUGHTHICKNESS  122	// SVGT - Float and JSR 
    #define AT_STROKE  123					// SVGT - color	and JSR  
    #define AT_STROKEDASHARRAY  124			// SVGT - Vector and JSr 
    #define AT_STROKEDASHOFFSET  125		// SVGT - Float and JSR 
    #define AT_STROKELINECAP  126			// SVGT - String and JSR - Enum
    #define AT_STROKELINEJOIN  127			// SVGT - String and JSR - Enum
    #define AT_STROKEMITERLIMIT  128		// SVGT - Float and JSR 
    #define AT_STROKEWIDTH  129				// SVGT - Float and JSR 
    #define AT_STYLE  130					// JSR ?
    #define AT_SYSTEMLANGUAGE  131			// SVGT - String and JSR 
    #define AT_TARGET  132					//	Not supported by SVGT Engine
    #define AT_TEXTANCHOR  133				// SVGT - Enum and JSR - Enum
    #define AT_TO  134						// SVGT- different data types JSR  
    #define AT_TRANSFORM  135				// SVGT - Matrix	and JSR - matrix			
    #define AT_TYPE  136					// SVGT - one case Matrix and another String JSR 
    #define AT_U1  137						// SVGT - String and  JSr 
    #define AT_U2  138						// SVGT - String and  JSr 
    #define AT_UNDERLINEPOSITION  139		// SVGT - Float and JSR 
    #define AT_UNDERLINETHICKNESS  140		// SVGT - Float and JSR 
    #define AT_UNICODE  141					// SVGT - String and JSR 	
    #define AT_UNICODERANGE  142			// SVGT - String and JSR 	
    #define AT_UNITSPEREM  143				// SVGT - Float and JSR 
    #define AT_VALUES  144					// SVGT - matrix and JSR  
    #define AT_VERSION  145					// SVGT - Float and JSR 
    #define AT_VIEWBOX  146					// SVGT - Rect and JSR 
    #define AT_VISIBILITY  147				// SVGT - Enum and JSR - Enum
    #define AT_WIDTH  148					// SVGT - Float and JSR 
    #define AT_WIDTHS  149					// unknown attribute
    #define AT_X  150						// SVGT - Float and JSR 
    #define AT_XHEIGHT  151					// unknown attribute	
    #define AT_X1  152						// SVGT - Float and JSR 
    #define AT_X2  153						// SVGT - Float and JSR 
    #define AT_XLINKACTUATE  154			// SVGT - String and JSR 	
    #define AT_XLINKARCROLE  155			// SVGT - String and JSR 	
    #define AT_XLINKHREF  156				// SVGT - String and JSR 	
    #define AT_XLINKROLE  157				// SVGT - String and JSR 	
    #define AT_XLINKSHOW  158				// SVGT - String and JSR 	
    #define AT_XLINKTITLE  159				// SVGT - String and JSR 	
    #define AT_XLINKTYPE  160				// SVGT - String and JSR 
    #define AT_XMLBASE  161					// SVGT - String and JSR 
    #define AT_XMLLANG  162					// SVGT - String and JSR 
    #define AT_XMLSPACE  163				// SVGT - String and JSR 
    #define AT_Y  164						// SVGT - Float and JSR 
    #define AT_Y1  165						// SVGT - Float and JSR 
    #define AT_Y2  166						// SVGT - Float and JSR 
    #define AT_ZOOMANDPAN  167				// SVGT - Enum and JSR -Enum

    // not in the spec but used to correctly implement animateMotion
    #define AT_MOTIONTRANSFORM  168			/// No corresponding SVGT Attribute

    // not actually an attribute, but needed for desc, title, and text
    #define AT_STRING  169					// No corresponding SVGT Attribute
    #define AT_TEXTDECORATION  170			// SVGT - Enum and JSR -Enum

    #define AT_HORIZORIGINY  171			// No corresponding SVGT Attribute
    #define AT_MOUSEEVENT  172				// No corresponding SVGT Attribute
    #define AT_USERBBOX  173				// No corresponding SVGT Attribute

    // A constant for each type of value
    #define VAL_EVENT  200
    #define VAL_INTEGER  201
    #define VAL_PAINT  202
    #define VAL_POINT  203
    #define VAL_REAL  204
    #define VAL_STRING  205
    #define VAL_TIME  206
    #define VAL_TRANSFORM  207
    #define VAL_VECTOR  208

    // These are used for animation purposes
    #define VAL_VECTOR_POINT  215
    #define VAL_VECTOR_REAL  216
    #define VAL_VECTOR_STRING  217
    #define VAL_VECTOR_PATH  218

    // VAL_VECTOR_X + VECTOR_TO_VAL  VAL_X
    #define VECTOR_TO_VAL  -12

    //
    // Constants for "choices"
    //

    // Preserve aspect ratio constants
    #define PAR_NONE  310
    #define PAR_XMIDYMID  311

    // Zoom and pan constants
    #define ZPN_MAGNIFY  320
    #define ZPN_DISABLE  321

    // Paint
    #define PAINT_NONE  325
    #define PAINT_CURRENT  326
    #define PAINT_COLOR  327
    #define PAINT_INHERIT  328

    // Fonts
    #define FONT_ALL  330
    #define FONT_NORMAL  331

    #define FONT_STYLE_ITALIC  332
    #define FONT_STYLE_OBLIQUE  333

    #define FONT_VARIANT_SMALLCAPS  334

    #define FONT_WEIGHT_BOLD  335
    #define FONT_WEIGHT_BOLDER  336
    #define FONT_WEIGHT_LIGHTER  337
    #define FONT_WEIGHT_100  338
    #define FONT_WEIGHT_200  339
    #define FONT_WEIGHT_300  340
    #define FONT_WEIGHT_400  341
    #define FONT_WEIGHT_500  342
    #define FONT_WEIGHT_600  343
    #define FONT_WEIGHT_700  344
    #define FONT_WEIGHT_800  345
    #define FONT_WEIGHT_900  346

    #define FONT_STRETCH_WIDER  347
    #define FONT_STRETCH_NARROWER  348
    #define FONT_STRETCH_ULTRA_COND  349
    #define FONT_STRETCH_EXTRA_COND  350
    #define FONT_STRETCH_COND  351
    #define FONT_STRETCH_SEMI_COND  352
    #define FONT_STRETCH_SEMI_EXPD  353
    #define FONT_STRETCH_EXPD  354
    #define FONT_STRETCH_EXTRA_EXPD  355
    #define FONT_STRETCH_ULTRA_EXPD  356

    // Text
    #define TEXT_ANCHOR_START  360
    #define TEXT_ANCHOR_MIDDLE  361
    #define TEXT_ANCHOR_END  362
    #define TEXT_UNDER_LINE  363
    #define TEXT_OVER_LINE  364
    #define TEXT_LINE_THROUGH  365

    // These are actual sizes in 8:8 fixed point, not "choices"
    #define FONT_SIZE_XXSMALL  0x20000
    #define FONT_SIZE_XSMALL  0x40000
    #define FONT_SIZE_SMALL  0x60000
    #define FONT_SIZE_MEDIUM  0xa0000
    #define FONT_SIZE_LARGE  0x100000
    #define FONT_SIZE_XLARGE  0x140000
    #define FONT_SIZE_XXLARGE  0x180000

    //
    // Styles
    //

    // Fill
    #define FILL_RULE_EVENODD  375
    #define FILL_RULE_NONZERO  376

    // Display
    #define DISPLAY_NONE  380
    #define DISPLAY_OTHER  381

    // Visibility
    #define VISIBILITY_VISIBLE  385
    #define VISIBILITY_OTHER  386

    // Color-rendering
    #define COLOR_RENDERING_AUTO  390
    #define COLOR_RENDERING_SPEED  391
    #define COLOR_RENDERING_QUALITY  392

    // Strokes
    // MUST PRESERVE ORDER!!
    #define STROKE_LINECAP_BUTT  395
    #define STROKE_LINECAP_ROUND  396
    #define STROKE_LINECAP_SQUARE  397

    #define STROKE_LINEJOIN_MITER  400
    #define STROKE_LINEJOIN_ROUND  401
    #define STROKE_LINEJOIN_BEVEL  402

    //
    // Animation
    //

    #define ANIM_INDEFINITE  445

    #define ACCUMULATE_NONE  450
    #define ACCUMULATE_SUM  451

    #define ADDITIVE_REPLACE  455
    #define ADDITIVE_SUM  456

    #define CALC_MODE_DISCRETE  460
    #define CALC_MODE_LINEAR  461
    #define CALC_MODE_PACED  462
    #define CALC_MODE_SPLINE  463

    #define FILL_REMOVE  465
    #define FILL_FREEZE  466

    #define RESTART_ALWAYS  470
    #define RESTART_NEVER  471
    #define RESTART_WHENNOTACTIVE  472

    #define TYPE_TRANSLATE  475
    #define TYPE_SCALE  476
    #define TYPE_ROTATE  477
    #define TYPE_SKEWX  478
    #define TYPE_SKEWY  479

    #define ATTR_TYPE_CSS  485
    #define ATTR_TYPE_XML  486
    #define ATTR_TYPE_AUTO  487

    #define ROTATE_AUTO  490
    #define ROTATE_AUTOREVERSE  491

    #define ANIM_FROM_TO  500
    #define ANIM_FROM_BY  501
    #define ANIM_BY  502
    #define ANIM_TO  503
    #define ANIM_VALUES  504
    #define ANIM_PATH  505

    //
    // Path Commands
    //

    #define PATH_COMMAND_M  600
    #define PATH_COMMAND_m  601

    #define PATH_COMMAND_Z  602

    #define PATH_COMMAND_L  603
    #define PATH_COMMAND_l  604

    #define PATH_COMMAND_H  605
    #define PATH_COMMAND_h  606

    #define PATH_COMMAND_V  607
    #define PATH_COMMAND_v  608

    #define PATH_COMMAND_C  609
    #define PATH_COMMAND_c  610

    #define PATH_COMMAND_S  611
    #define PATH_COMMAND_s  612

    #define PATH_COMMAND_Q  613
    #define PATH_COMMAND_q  614

    #define PATH_COMMAND_T  615
    #define PATH_COMMAND_t  616

    //
    // Events
    //

    #define EVENT_BEGIN  650  // the animation has started
    #define EVENT_END  651  // the animation has finished
    #define EVENT_REPEAT  652  // the animation has repeated
    #define EVENT_BEGIN_EL  653
    #define EVENT_END_EL  654

#endif