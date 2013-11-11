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


#include "SVGColor.h"
#include "SVGSchemaData.h"
#include "SVGDocumentImpl.h"


//CONSTANTS
_LIT(KCOLOR_WHITE, "white");
_LIT(KCOLOR_ANTIQUEWHITE, "antiquewhite");
_LIT(KCOLOR_BLACK, "black");
_LIT(KCOLOR_SKYBLUE, "skyblue");
_LIT(KCOLOR_BLUE, "blue");
_LIT(KCOLOR_PURPLE, "purple");
_LIT(KCOLOR_RED, "red");
_LIT(KCOLOR_WHITESMOKE, "whitesmoke");
_LIT(KCOLOR_SNOW, "snow");
_LIT(KCOLOR_YELLOW, "yellow");
_LIT(KCOLOR_GRAY, "gray");
_LIT(KCOLOR_GREEN, "green");
_LIT(KCOLOR_DARKRED, "darkred");
_LIT(KCOLOR_ORANGE, "orange");
_LIT(KCOLOR_BROWN, "brown");
_LIT(KCOLOR_MAGENTA, "magenta");
_LIT(KCOLOR_MAROON, "maroon");
_LIT(KCOLOR_PINK, "pink");
_LIT(KCOLOR_ROYALBLUE, "royalblue");
_LIT(KCOLOR_ALICEBLUE, "aliceblue");
_LIT(KCOLOR_AQUA, "aqua");
_LIT(KCOLOR_LIGHTGREEN, "lightgreen");
_LIT(KCOLOR_LIGHTGRAY, "lightgray");
_LIT(KCOLOR_LIGHTGREY, "lightgrey");
_LIT(KCOLOR_LIGHTPINK, "lightpink");
_LIT(KCOLOR_AQUAMARINE, "aquamarine");
_LIT(KCOLOR_AZURE, "azure");
_LIT(KCOLOR_BEIGE, "beige");
_LIT(KCOLOR_CYAN, "cyan");
_LIT(KCOLOR_DARKBLUE, "darkblue");
_LIT(KCOLOR_DARKCYAN, "darkcyan");
_LIT(KCOLOR_DARKGOLDENROD, "darkgoldenrod");
_LIT(KCOLOR_DARKGRAY, "darkgray");
_LIT(KCOLOR_DARKGREY, "darkgrey");
_LIT(KCOLOR_DARKGREEN, "darkgreen");
_LIT(KCOLOR_DARKKHAKI, "darkkhaki");
_LIT(KCOLOR_DARKMAGENTA, "darkmagenta");
_LIT(KCOLOR_DARKOLIVEGREEN, "darkolivegreen");
_LIT(KCOLOR_DARKORANGE, "darkorange");
_LIT(KCOLOR_DARKORCHID, "darkorchid");
_LIT(KCOLOR_INDIGO, "indigo");
_LIT(KCOLOR_IVORY, "ivory");
_LIT(KCOLOR_KHAKI, "khaki");
_LIT(KCOLOR_LAVENDER, "lavender");
_LIT(KCOLOR_BISQUE, "bisque");
_LIT(KCOLOR_BLANCHEDALMOND, "blanchedalmond");
_LIT(KCOLOR_BLUEVIOLET, "blueviolet");
_LIT(KCOLOR_BURLYWOOD, "burlywood");
_LIT(KCOLOR_CADETBLUE, "cadetblue");
_LIT(KCOLOR_CHARTREUSE, "chartreuse");
_LIT(KCOLOR_CHOCOLATE, "chocolate");
_LIT(KCOLOR_CORAL, "coral");
_LIT(KCOLOR_CORNFLOWERBLUE, "cornflowerblue");
_LIT(KCOLOR_CORNSILK, "cornsilk");
_LIT(KCOLOR_CRIMSON, "crimson");
_LIT(KCOLOR_DARKSALMON, "darksalmon");
_LIT(KCOLOR_DARKSEAGREEN, "darkseagreen");
_LIT(KCOLOR_DARKSLATEBLUE, "darkslateblue");
_LIT(KCOLOR_DARKSLATEGRAY, "darkslategray");
_LIT(KCOLOR_DARKTURQUOISE, "darkturquoise");
_LIT(KCOLOR_DARKVIOLET, "darkviolet");
_LIT(KCOLOR_DEEPPINK, "deeppink");
_LIT(KCOLOR_DEEPSKYBLUE, "deepskyblue");
_LIT(KCOLOR_DIMGRAY, "dimgray");
_LIT(KCOLOR_DODGERBLUE, "dodgerblue");
_LIT(KCOLOR_FIREBRICK, "firebrick");
_LIT(KCOLOR_FLORALWHITE, "floralwhite");
_LIT(KCOLOR_FORESTGREEN, "forestgreen");
_LIT(KCOLOR_FUCHSIA, "fuchsia");
_LIT(KCOLOR_GAINSBORO, "gainsboro");
_LIT(KCOLOR_GHOSTWHITE, "ghostwhite");
_LIT(KCOLOR_GOLD, "gold");
_LIT(KCOLOR_GOLDENROD, "goldenrod");
_LIT(KCOLOR_GREENYELLOW, "greenyellow");
_LIT(KCOLOR_HONEYDEW, "honeydew");
_LIT(KCOLOR_HOTPINK, "hotpink");
_LIT(KCOLOR_INDIANRED, "indianred");
_LIT(KCOLOR_LAVENDERBLUSH, "lavenderblush");
_LIT(KCOLOR_LAWNGREEN, "lawngreen");
_LIT(KCOLOR_LEMONCHIFFON, "lemonchiffon");
_LIT(KCOLOR_LIGHTBLUE, "lightblue");
_LIT(KCOLOR_LIGHTCORAL, "lightcoral");
_LIT(KCOLOR_LIGHTCYAN, "lightcyan");
_LIT(KCOLOR_LIGHTGOLDENRODYELLOW, "lightgoldenrodyellow");
_LIT(KCOLOR_LIGHTSALMON, "lightsalmon");
_LIT(KCOLOR_LIGHTSEAGREEN, "lightseagreen");
_LIT(KCOLOR_LIGHTSKYBLUE, "lightskyblue");
_LIT(KCOLOR_LIGHTSLATEGRAY, "lightslategray");
_LIT(KCOLOR_LIGHTSTEELBLUE, "lightsteelblue");
_LIT(KCOLOR_LIGHTYELLOW, "lightyellow");
_LIT(KCOLOR_LIME, "lime");
_LIT(KCOLOR_LIMEGREEN, "limegreen");
_LIT(KCOLOR_LINEN, "linen");
_LIT(KCOLOR_MEDIUMAQUAMARINE, "mediumaquamarine");
_LIT(KCOLOR_MEDIUMBLUE, "mediumblue");
_LIT(KCOLOR_MEDIUMORCHID, "mediumorchid");
_LIT(KCOLOR_MEDIUMPURPLE, "mediumpurple");
_LIT(KCOLOR_MEDIUMSEAGREEN, "mediumseagreen");
_LIT(KCOLOR_MEDIUMSLATEBLUE, "mediumslateblue");
_LIT(KCOLOR_MEDIUMSPRINGGREEN, "mediumspringgreen");
_LIT(KCOLOR_MEDIUMTURQUOISE, "mediumturquoise");
_LIT(KCOLOR_MEDIUMVIOLETRED, "mediumvioletred");
_LIT(KCOLOR_MIDNIGHTBLUE, "midnightblue");
_LIT(KCOLOR_MINTCREAM, "mintcream");
_LIT(KCOLOR_MISTYROSE, "mistyrose");
_LIT(KCOLOR_MOCCASIN, "moccasin");
_LIT(KCOLOR_NAVAJOWHITE, "navajowhite");
_LIT(KCOLOR_NAVY, "navy");
_LIT(KCOLOR_OLDLACE, "oldlace");
_LIT(KCOLOR_OLIVE, "olive");
_LIT(KCOLOR_OLIVEDRAB, "olivedrab");
_LIT(KCOLOR_ORANGERED, "orangered");
_LIT(KCOLOR_ORCHID, "orchid");
_LIT(KCOLOR_PALEGOLDENROD, "palegoldenrod");
_LIT(KCOLOR_PALEGREEN, "palegreen");
_LIT(KCOLOR_PALETURQUOISE, "paleturquoise");
_LIT(KCOLOR_PALEVIOLETRED, "palevioletred");
_LIT(KCOLOR_PAPAYAWHIP, "papayawhip");
_LIT(KCOLOR_PEACHPUFF, "peachpuff");
_LIT(KCOLOR_PERU, "peru");
_LIT(KCOLOR_PLUM, "plum");
_LIT(KCOLOR_POWDERBLUE, "powderblue");
_LIT(KCOLOR_ROSYBROWN, "rosybrown");
_LIT(KCOLOR_SADDLEBROWN, "saddlebrown");
_LIT(KCOLOR_SALMON, "salmon");
_LIT(KCOLOR_SANDYBROWN, "sandybrown");
_LIT(KCOLOR_SEAGREEN, "seagreen");
_LIT(KCOLOR_SEASHELL, "seashell");
_LIT(KCOLOR_SIENNA, "sienna");
_LIT(KCOLOR_SILVER, "silver");
_LIT(KCOLOR_SLATEBLUE, "slateblue");
_LIT(KCOLOR_SLATEGRAY, "slategray");
_LIT(KCOLOR_SPRINGGREEN, "springgreen");
_LIT(KCOLOR_STEELBLUE, "steelblue");
_LIT(KCOLOR_TAN, "tan");
_LIT(KCOLOR_TEAL, "teal");
_LIT(KCOLOR_THISTLE, "thistle");
_LIT(KCOLOR_TOMATO, "tomato");
_LIT(KCOLOR_TURQUOISE, "turquoise");
_LIT(KCOLOR_VIOLET, "violet");
_LIT(KCOLOR_WHEAT, "wheat");
_LIT(KCOLOR_YELLOWGREEN, "yellowgreen");
_LIT(KCOLOR_NONE, "none");

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TSvgColor::TSvgColor( TUint32 aValue ) : TGfxColor( aValue )
    {
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TSvgColor::TSvgColor( TInt aRed, TInt aGreen, TInt aBlue ) : TGfxColor( aRed,
                                                                    aGreen,
                                                                    aBlue )
    {
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TBool TSvgColor::GetStringL( const TDesC& aColorValue, TUint32 &aColor)

    {

    TUint32 color = 0; // default value

    const TUint16* lPtr = aColorValue.Ptr();
    TInt offset = KErrNotFound;

    _LIT16( Krgb, "rgb" );

    // If color is given as a '#...' value
    if ( lPtr[0] == '#' )
        {
        TPtrC tPtrC = aColorValue.Right( aColorValue.Length() - 1 );

        if ( tPtrC.Length() > 6 )
            {
          	aColor = color;
            return EFalse;            
            }
            
        for ( TInt i = 0; i < tPtrC.Length(); i++ )
            {
            if ( !TChar( tPtrC[i] ).IsHexDigit() )
                {
            	aColor = color;
                return EFalse;
                }
            }

        if ( tPtrC.Length() == 3 ) // as in "#f00"
        {
            TBuf<6> lCol( tPtrC );

            // Make it #rrggbb
            lCol.Insert( 0, TPtrC( &lCol[0], 1 ) );
            lCol.Insert( 2, TPtrC( &lCol[2], 1 ) );
            lCol.Insert( 4, TPtrC( &lCol[4], 1 ) );


            TLex tLex( lCol );
            if(tLex.Val( color, EHex ) != KErrNone)
            	{
             	return EFalse;
             	}
            }
        else
            {
            TLex tLex( tPtrC );
            if(tLex.Val( color, EHex ) != KErrNone)
            	{
	            return EFalse;
        	    }
            }
        }
    // If color is given as a RGB
    else if ( ( offset = aColorValue.FindF( Krgb ) ) != KErrNotFound )
        {
        if ( offset > 0 || aColorValue.Length() > 36 )
            {
            return ETrue;
            }

        TBuf<36> tBuf; // Usually not more than 24 chars long
        const TUint16* cPtr = aColorValue.Ptr();
        TInt colorValueLength = aColorValue.Length();
        for ( TInt idx = 0; idx < colorValueLength; idx++ )
            {
            if ( cPtr[idx] != ' ' )// remove spaces
				{
                tBuf.Append( cPtr[idx] );
                }
            }

        color = ProcessRGBL( tBuf );
        }
    // If color is given as a text string (eg. "red")
    else
        {
        color= MapColorToInt( aColorValue );
		if(color==KGfxColorNull)
			{
			return EFalse;
			}
        }


	aColor = color;

    return ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TUint32 TSvgColor::MapColorToInt( const TDesC& aColorName )
    {

	TLex convert(aColorName);
	convert.SkipSpace();

	if ( !convert.Remainder().CompareF( KCOLOR_WHITE ))
	{
		return 0xFFFFFF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_BLACK ))
	{
		return 0x000000;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_BLUE ))
	{
		return 0x0000FF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SKYBLUE ))
	{
		return 0x87CEEB;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_RED ))
	{
		return 0xFF0000;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_YELLOW ))
	{
		return 0xFFFF00;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_GRAY ))
	{
		return 0x808080;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_GREEN ))
	{
		return 0x008000;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_ALICEBLUE ))
	{
		return 0xF0F8FF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_ANTIQUEWHITE ))
	{
		return 0xFAEBD7;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_AQUA ))
	{
		return 0x00FFFF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_AQUAMARINE ))
	{
		return 0x7FFFD4;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_AZURE ))
	{
		return 0xF0FFFF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_ORANGE ))
	{
		return 0xFFA500;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_BEIGE ))
	{
		return 0xF5F5DC;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MAGENTA ))
	{
		return 0xFF00FF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MAROON ))
	{
		return 0x800000;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKGRAY ))
	{
		return 0xA9A9A9;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKGREY ))
	{
		return 0xA9A9A9;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKGREEN ))
	{
		return 0x006400;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKKHAKI ))
	{
		return 0xBDB76B;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKMAGENTA ))
	{
		return 0x8B008B;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_CYAN ))
	{
		return 0x00FFFF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKBLUE ))
	{
		return 0x00008B;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKCYAN ))
	{
		return 0x008B8B;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_BISQUE ))
	{
		return 0xFFE4C4;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_BLANCHEDALMOND ))
	{
		return 0xFFEBCD;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_BLUEVIOLET ))
	{
		return 0x8A2BE2;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_BROWN ))
	{
		return 0xA52A2A;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_BURLYWOOD ))
	{
		return 0xDEB887;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_CADETBLUE ))
	{
		return 0x5F9EA0;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_CHARTREUSE ))
	{
		return 0x7FFF00;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_CHOCOLATE ))
	{
		return 0xD2691E;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_CORAL ))
	{
		return 0xFF7F50;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_CORNFLOWERBLUE ))
	{
		return 0x6495ED;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_CORNSILK ))
	{
		return 0xFFF8DC;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_CRIMSON ))
	{
		return 0xDC143C;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKGOLDENROD ))
	{
		return 0xB8860B;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKOLIVEGREEN ))
	{
		return 0x556B2F;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKORANGE ))
	{
		return 0xFF8C00;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKORCHID ))
	{
		return 0x9932CC;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKRED ))
	{
		return 0x8B0000;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKSALMON ))
	{
		return 0xE9967A;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKSEAGREEN ))
	{
		return 0x8FBC8F;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKSLATEBLUE ))
	{
		return 0x483D8B;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKSLATEGRAY ))
	{
		return 0x2F4F4F;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKTURQUOISE ))
	{
		return 0x00CED1;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DARKVIOLET ))
	{
		return 0x9400D3;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DEEPPINK ))
	{
		return 0xFF1493;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DEEPSKYBLUE ))
	{
		return 0x00BFFF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DIMGRAY ))
	{
		return 0x696969;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_DODGERBLUE ))
	{
		return 0x1E90FF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_FIREBRICK ))
	{
		return 0xB22222;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_FLORALWHITE ))
	{
		return 0xFFFAF0;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_FORESTGREEN ))
	{
		return 0x228B22;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_FUCHSIA ))
	{
		return 0xFF00FF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_GAINSBORO ))
	{
		return 0xDCDCDC;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_GHOSTWHITE ))
	{
		return 0xF8F8FF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_GOLD ))
	{
		return 0xFFD700;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_GOLDENROD ))
	{
		return 0xDAA520;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_GREENYELLOW ))
	{
		return 0xADFF2F;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_HONEYDEW ))
	{
		return 0xF0FFF0;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_HOTPINK ))
	{
		return 0xFF69B4;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_INDIANRED ))
	{
		return 0xCD5C5C;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_INDIGO ))
	{
		return 0x4B0082;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_IVORY ))
	{
		return 0xFFFFF0;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_KHAKI ))
	{
		return 0xF0E68C;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LAVENDER ))
	{
		return 0xE6E6FA;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LAVENDERBLUSH ))
	{
		return 0xFFF0F5;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LAWNGREEN ))
	{
		return 0x7CFC00;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LEMONCHIFFON ))
	{
		return 0xFFFACD;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTBLUE ))
	{
		return 0xADD8E6;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTCORAL ))
	{
		return 0xF08080;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTCYAN ))
	{
		return 0xE0FFFF;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTGOLDENRODYELLOW ))
	{
		return 0xFAFAD2;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTGREEN ))
	{
		return 0x90EE90;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTGRAY ))
	{
		return 0xD3D3D3;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTGREY ))
	{
		return 0xD3D3D3;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTPINK ))
	{
		return 0xFFB6C1;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTSALMON ))
	{
		return 0xFFA07A;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTSEAGREEN ))
	{
		return 0x20B2AA;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTSKYBLUE ))
	{
		return 0x87CEFA;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTSLATEGRAY ))
	{
		return 0x778899;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTSTEELBLUE ))
	{
		return 0xB0C4DE;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIGHTYELLOW ))
	{
		return 0xFFFFE0;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIME ))
	{
		return 0x00FF00;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LIMEGREEN ))
	{
		return 0x32CD32;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_LINEN ))
	{
		return 0xFAF0E6;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MEDIUMAQUAMARINE ))
	{
		return 0x66CDAA;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MEDIUMBLUE ))
	{
		return 0x0000CD;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MEDIUMORCHID ))
	{
		return 0xBA55D3;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MEDIUMPURPLE ))
	{
		return 0x9370DB;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MEDIUMSEAGREEN ))
	{
		return 0x3CB371;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MEDIUMSLATEBLUE ))
	{
		return 0x7B68EE;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MEDIUMSPRINGGREEN ))
	{
		return 0x00FA9A;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MEDIUMTURQUOISE ))
	{
		return 0x48D1CC;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MEDIUMVIOLETRED ))
	{
		return 0xC71585;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MIDNIGHTBLUE ))
	{
		return 0x191970;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MINTCREAM ))
	{
		return 0xF5FFFA;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MISTYROSE ))
	{
		return 0xFFE4E1;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_MOCCASIN ))
	{
		return 0xFFE4B5;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_NAVAJOWHITE ))
	{
		return 0xFFDEAD;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_NAVY ))
	{
		return 0x000080;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_OLDLACE ))
	{
		return 0xFDF5E6;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_OLIVE ))
	{
		return 0x808000;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_OLIVEDRAB ))
	{
		return 0x6B8E23;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_ORANGERED ))
	{
		return 0xFF4500;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_ORCHID ))
	{
		return 0xDA70D6;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_PALEGOLDENROD ))
	{
		return 0xEEE8AA;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_PALEGREEN ))
	{
		return 0x98FB98;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_PALETURQUOISE ))
	{
		return 0xAFEEEE;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_PALEVIOLETRED ))
	{
		return 0xDB7093;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_PAPAYAWHIP ))
	{
		return 0xFFEFD5;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_PEACHPUFF ))
	{
		return 0xFFDAB9;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_PERU ))
	{
		return 0xCD853F;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_PINK ))
	{
		return 0xFFC0CB;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_PLUM ))
	{
		return 0xDDA0DD;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_POWDERBLUE ))
	{
		return 0xB0E0E6;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_PURPLE ))
	{
		return 0x800080;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_ROSYBROWN ))
	{
		return 0xBC8F8F;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_ROYALBLUE ))
	{
		return 0x4169E1;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SADDLEBROWN ))
	{
		return 0x8B4513;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SALMON ))
	{
		return 0xFA8072;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SANDYBROWN ))
	{
		return 0xF4A460;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SEAGREEN ))
	{
		return 0x2E8B57;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SEASHELL ))
	{
		return 0xFFF5EE;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SIENNA ))
	{
		return 0xA0522D;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SILVER ))
	{
		return 0xC0C0C0;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SLATEBLUE ))
	{
		return 0x6A5ACD;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SLATEGRAY ))
	{
		return 0x708090;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SNOW ))
	{
		return 0xFFFAFA;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_SPRINGGREEN ))
	{
		return 0x00FF7F;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_STEELBLUE ))
	{
		return 0x4682B4;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_TAN ))
	{
		return 0xD2B48C;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_TEAL ))
	{
		return 0x008080;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_THISTLE ))
	{
		return 0xD8BFD8;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_TOMATO ))
	{
		return 0xFF6347;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_TURQUOISE ))
	{
		return 0x40E0D0;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_VIOLET ))
	{
		return 0xEE82EE;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_WHEAT ))
	{
		return 0xF5DEB3;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_WHITESMOKE ))
	{
		return 0xF5F5F5;
	}
	else if ( !convert.Remainder().CompareF( KCOLOR_YELLOWGREEN ))
	{
		return 0x9ACD32;
	}
    else if ( !convert.Remainder().CompareF( KCOLOR_NONE ) )
        {
        return 0x1ffffff ;
        }
    return KGfxColorNull;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TUint32 TSvgColor::ProcessRGBL( const TDesC& aColorValue )
    {
    //this function is used to process the rgb(255,255,255)
    //style color attribute

    TUint32 color = 0x1ffffff; // KGfxColorNull
    TInt red = 0;
    TInt green = 0;
    TInt blue = 0;

    // To be completed
    TInt pos1 = aColorValue.Locate( ',' );
    TInt pos2 = aColorValue.LocateReverse( ',' );
    if (pos1 < 0 )
    	{
    	pos1 = 0;
    	}
    if (pos2 < 0 )
    	{
    	pos2 = 0;
    	}

    if ( pos1 == pos2 ) // there is only one comma
    {
        return color;
        }

    TPtrC tStr = aColorValue.Right( aColorValue.Length() - 4 ); // Remove "Rgb(" leave "R,G,B)"
    TPtrC tRed = tStr.Left( pos1 ); // Get Red
    TPtrC tStr2 = aColorValue.Left( pos2 );
    TInt pos3 = tStr2.Locate( ',' );
    if (pos3 < 0 )
    	{
    	pos3 = 0;
    	}
    TPtrC tGreen = tStr2.Right( tStr2.Length() - pos3 - 1 ); // Get Green
    TPtrC tStr3 = aColorValue.Right( aColorValue.Length() - pos2 - 1 );
    TInt pos4 = tStr3.Locate(')');
    if (pos4 < 0 )
    	{
    	pos4 = 0;
    	}
    	
    TPtrC tBlue = tStr3.Left( pos4 ); // Get Blue

    if ( !ProcessRGBPercentageL( tRed, red ) )
        {
        TLex rLex( tRed );
        if (rLex.Val( red ) != KErrNone)
        	{
			 red = 0;
		    }
        }

    if ( !ProcessRGBPercentageL( tGreen, green ) )
        {
        TLex gLex( tGreen );
        if (gLex.Val( green ) != KErrNone)
        	{
			 green = 0;
			}
        }

    if ( !ProcessRGBPercentageL( tBlue, blue ) )
        {
        TLex bLex( tBlue );
        if (bLex.Val( blue ) != KErrNone)
        	{
			 blue = 0;
			}
        }

	if (blue > 255)
	{
		blue = 255;
	}

	if (green > 255)
	{
		green = 255;
	}

	if (red > 255)
	{
		red = 255;
	}

	//clipping to zero in case of -ve values.ESMA-7QFHZC
	if (blue < 0)
        {
	    blue = 0;
        }
	if (green < 0)
	    {
	    green = 0;
	    }
	if (red < 0)
	    {
	    red = 0;
	    }
    return (red<<16)|(green<<8)|(blue);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool TSvgColor::ProcessRGBPercentageL( const TDesC& aColorValue, TInt& color )
    {
    TInt pos;

    pos = aColorValue.Locate( '%' );

    if ( pos == KErrNotFound )
        {
        return EFalse;
        }

    HBufC* tBufC = HBufC::NewLC( aColorValue.Length() );
    TPtr tPtr = tBufC->Des();
    tPtr.Copy( aColorValue );

    tPtr.Delete( pos, 1 ); // Remove '%'

    TInt pcn = 0;
    TLex lex( tPtr );

	if (lex.Val( pcn ) != KErrNone)
		{
		 pcn = 0;
		}
	//ESMA-7QGBUN
    color = ( int ) ( (255 * pcn)/100 ); //  (2.55 * pcn)

    CleanupStack::PopAndDestroy( 1 ); //tBufC

    return ETrue;
    }
