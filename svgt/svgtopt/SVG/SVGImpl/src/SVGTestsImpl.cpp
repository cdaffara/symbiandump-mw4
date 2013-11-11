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


#include "SVGTestsImpl.h"


// ---------------------------------------------------------------------------
// Two phase construction:
// - Implements the SVGTests DOM interface.
// - Partially implemented, only those required
//   for passing SVG Test Suite.
// ---------------------------------------------------------------------------
CSvgTestsImpl* CSvgTestsImpl::NewL()
    {
    CSvgTestsImpl* self = new ( ELeave ) CSvgTestsImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTestsImpl* CSvgTestsImpl::NewLC()
    {
    CSvgTestsImpl* self = new ( ELeave ) CSvgTestsImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTestsImpl::~CSvgTestsImpl()
    {
    if ( iRequiredFeatures )
		{
        delete iRequiredFeatures;
		iRequiredFeatures = NULL;
		}
    if ( iRequiredExtensions )
		{
        delete iRequiredExtensions;
		iRequiredExtensions = NULL;
		}
    if ( iSystemLanguage )
		{
        delete iSystemLanguage;
		iSystemLanguage = NULL;
		}
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTestsImpl::ConstructL()
    {
    iRequiredFeatures = new ( ELeave ) CDesCArrayFlat( 1 );
    iRequiredFeatures->Reset();
    iRequiredExtensions = new ( ELeave ) CDesCArrayFlat( 1 );
    iRequiredExtensions->Reset();
    iSystemLanguage = new ( ELeave ) CDesCArrayFlat( 1 );
    iSystemLanguage->Reset();

    iSystemLanguageEmpty = EFalse;
    }


// From MSvgTests

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTestsImpl::GetRequiredFeatures( const CDesCArrayFlat*& aRequiredFeatures )
    {
    aRequiredFeatures = iRequiredFeatures;
    }

// ---------------------------------------------------------------------------
// function for decoding
// ---------------------------------------------------------------------------
void CSvgTestsImpl::SetRequiredFeatures(  CDesCArrayFlat*& aRequiredFeatures )
    {
	if(iRequiredFeatures)
		{
		iRequiredFeatures->Reset();
		delete iRequiredFeatures;
		iRequiredFeatures= NULL;
		}
    iRequiredFeatures= aRequiredFeatures;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTestsImpl::GetRequiredExtensions( const CDesCArrayFlat*& aRequiredExtensions )
    {
    aRequiredExtensions = iRequiredExtensions;
    }

// ---------------------------------------------------------------------------
// function for decoding
// ---------------------------------------------------------------------------
void CSvgTestsImpl::SetRequiredExtensions(  CDesCArrayFlat*& aRequiredExtensions )
    {
	if(iRequiredExtensions)
		{
		iRequiredExtensions->Reset();
		delete iRequiredExtensions;
		iRequiredExtensions= NULL;
		}
    iRequiredExtensions= aRequiredExtensions;
    }

// ---------------------------------------------------------------------------
// returns false if there was an empty string system language true otherwise
// ---------------------------------------------------------------------------
TBool CSvgTestsImpl::GetSystemLanguage( const CDesCArrayFlat*& aSystemLanguage )
    {
    aSystemLanguage = iSystemLanguage;

    if (iSystemLanguageEmpty)
    	{
    		return EFalse;
    	}

    return ETrue;

    }

// ---------------------------------------------------------------------------
// function for decoding
// ---------------------------------------------------------------------------
void CSvgTestsImpl::SetSystemLanguage(  CDesCArrayFlat*& aSystemLanguage )
    {
	if(iSystemLanguage)
		{
		iSystemLanguage->Reset();
		delete iSystemLanguage;
		iSystemLanguage= NULL;
		}
    iSystemLanguage= aSystemLanguage;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgTestsImpl::HasExtension( const TDesC& /* aExtension */ )
    {
    return EFalse;
    }

// For use in requiredFeatures
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgTestsImpl::HasFeature( const TDesC& aFeature )
    {
    // SVG Tiny Feature List
    _LIT( KTiny, "http://www.w3.org/TR/SVGMobile/Tiny/feature#" );
    _LIT( KBase, "base" );
    _LIT( KInteractivity, "interactivity" );
    _LIT( KAll, "all" );

    // SVG 1.1 Feature List
    _LIT( KSVG11, "http://www.w3.org/TR/SVG11/feature#" );
    _LIT( KCoreAttr, "CoreAttribute" );
    _LIT( KBasicStr, "BasicStructure" );
    _LIT( KStyle, "Style" );
    _LIT( KPaintAttr, "PaintAttribute" );
    _LIT( KGraphicsAttr, "BasicGraphicsAttribute" );
    _LIT( KHyperlink, "Hyperlinking" );
    _LIT( KXlinkAttr, "XlinkAttribute" );
    _LIT( KCond, "ConditionalProcessing" );
    _LIT( KShape, "Shape" );
    _LIT( KImage, "Image" );
    _LIT( KBasicTest, "BasicText" );
    _LIT( KBasicFont, "BasicFont" );
    _LIT( KDocAttr,"DocumentEventsAttribute" );
    _LIT( KGraphEvt,"GraphicalEventsAttribute" );
    _LIT( KAnimEvt,"AnimationEventsAttribute" );
    _LIT( KScript, "Scripting" );
    _LIT( KView, "View" );
    _LIT( KAnim, "SVG-animation" );
    _LIT( KExt, "Extensibility" );

    _LIT( KSVG12, "http://www.w3.org/TR/SVG12/feature#" );
    _LIT( KAudio, "Audio");

    // SVG 1.2 Feature List
    // SVG Feature List
    _LIT( KStatic, "org.w3c.svg.static" );
    _LIT( KDynamic, "org.w3c.svg.dynamic" );

    TPtrC tinybase( KTiny );
    if( (aFeature.Length()-tinybase.Length()) > 1 )
        {
        TPtrC rightTiny= aFeature.Right(aFeature.Length()-tinybase.Length() );
        if ( rightTiny.Match(KBase) != KErrNotFound)
        return ETrue;
        if ( rightTiny.Match(KInteractivity) != KErrNotFound)
        return ETrue;
        if ( rightTiny.Match(KAll) != KErrNotFound )
        return ETrue;
        }

    TPtrC svg11base( KSVG11 );
    if( (aFeature.Length()-svg11base.Length()) > 1 )
        {
        TPtrC rightSvg11= aFeature.Right(aFeature.Length()-svg11base.Length() );
    if ( rightSvg11.Match(KAnim) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KCoreAttr) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KBasicStr) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KStyle) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KPaintAttr) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KGraphicsAttr) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KHyperlink) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KXlinkAttr) != KErrNotFound )
        return ETrue;
    if (rightSvg11.Match(KCond) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KShape) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KImage) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KBasicTest) != KErrNotFound )
        return ETrue;
    if (rightSvg11.Match(KBasicFont) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KDocAttr) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KGraphEvt) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KAnimEvt) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KScript)!= KErrNotFound )
        return EFalse;          // Scripting is not supported.
    if ( rightSvg11.Match(KView) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KAnim) != KErrNotFound )
        return ETrue;
    if ( rightSvg11.Match(KExt) != KErrNotFound )
        return ETrue;
        }

    TPtrC svg12base( KSVG12 );
    if( (aFeature.Length()-svg12base.Length()) > 1 )
        {
        TPtrC rightSvg12= aFeature.Right(aFeature.Length()-svg12base.Length() );
        if ( rightSvg12.Match(KAudio) != KErrNotFound )
            {
            return ETrue;
            }
        }

    if ( aFeature == KStatic )
        return ETrue;
    if ( aFeature == KDynamic )
        return EFalse;
    else
        return FALSE;
    }

TBool CSvgTestsImpl::HasAnyTests()
{
	const CDesCArrayFlat* tempTestArray = NULL;

	GetRequiredFeatures(tempTestArray);
	if (tempTestArray && tempTestArray->Count() > 0)
	{
		return ETrue;
	}

	GetSystemLanguage(tempTestArray);
	if (tempTestArray && tempTestArray->Count() > 0)
	{
		return ETrue;
	}

	GetRequiredExtensions(tempTestArray);
	if (tempTestArray && tempTestArray->Count() > 0)
	{
		return ETrue;
	}

	if (iSystemLanguageEmpty)
	{
		return ETrue;
	}

	return EFalse;
}

// Implementation

TBool CSvgTestsImpl::SetRequiredFeatureAttribute( const TDesC& aValue )
{
	TLex input( aValue );

    while ( !( input.Eos() ) )
    {
    	input.SkipSpace();
        input.Mark();
        input.SkipCharacters();
        TPtrC token = input.MarkedToken();    // then extract token
        TRAPD(error,iRequiredFeatures->AppendL( token ));

        if (error != KErrNone)
        {
        	//Error Processing
        	return EFalse;
        }
    }

    return ETrue;
}

TBool CSvgTestsImpl::SetRequiredExtensionAttribute( const TDesC& aValue )
{
	TLex input( aValue );

    if ( aValue.Length() == 0 )
    {
	    TRAPD(error1,iRequiredExtensions->AppendL( aValue ));

	    if (error1 != KErrNone)
	    {
        	//Error Processing
        	return EFalse;
	  	}
    }
    else
    {
    	while ( !( input.Eos() ) )
        {
        	input.SkipSpace();
            input.Mark();
            input.SkipCharacters();
            TPtrC token = input.MarkedToken();    // then extract token
            TRAPD(error,iRequiredExtensions->AppendL( token ));

            if (error != KErrNone)
            {
							//Error Processing
							return EFalse;
            }
        }
    }
    return ETrue;
}

TBool CSvgTestsImpl::SetSystemLanguageAttribute( const TDesC& aValue  )
{
	TLex input( aValue );

	iSystemLanguageEmpty = EFalse;

 	if ( input.Eos() )
    {
		iSystemLanguageEmpty = ETrue;
    }
    else
    {
    	while ( !( input.Eos() ) )
        {
        	input.SkipSpace();
            input.Mark();
            input.SkipCharacters();
            TPtrC token = input.MarkedToken();    // then extract token
            TRAPD(error,iSystemLanguage->AppendL( token ));

            if (error != KErrNone)
            {
							//Error Processing
							return EFalse;
          	}
        }
    }

    return ETrue;

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgTestsImpl::SetTestAttributeL( const TDesC& aPropertyName,
                                        const TDesC& aValue )
{

    _LIT( KRequiredFeatures, "requiredFeatures" );
    _LIT( KRequiredExtensions, "requiredExtensions" );
    _LIT( KSystemLanguage, "systemLanguage" );

    if (aPropertyName == KRequiredFeatures)
    {
    	return SetRequiredFeatureAttribute(aValue);
    }
    else if (aPropertyName == KRequiredExtensions)
    {
    	return SetRequiredExtensionAttribute(aValue);
    }
    else if (aPropertyName == KSystemLanguage)
    {
    	return SetSystemLanguageAttribute(aValue);
    }
	else
	{
		return EFalse;
	}
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTestsImpl* CSvgTestsImpl::CloneL()
    {
    CSvgTestsImpl* retValue = CSvgTestsImpl::NewL();

	CleanupStack::PushL(retValue);
    // no need to call Reset() on any of these arrays because
    // that already happened in the call to NewL above
    int i;

    // *** copy required features
    TInt requiredFeaturesCnt = this->iRequiredFeatures->Count();
    for (i=0; i< requiredFeaturesCnt; i++)
        {
        retValue->iRequiredFeatures->AppendL(this->iRequiredFeatures->operator[](i));
        }

    // *** copy required extensions
    TInt requiredExtensionsCnt = this->iRequiredExtensions->Count();
    for (i=0; i< requiredExtensionsCnt; i++)
        {
        retValue->iRequiredExtensions->AppendL(this->iRequiredExtensions->operator[](i));
        }

    // *** copy system language
    TInt systemLanguageCnt = this->iSystemLanguage->Count();
    for (i=0; i< systemLanguageCnt; i++)
        {
        retValue->iSystemLanguage->AppendL(this->iSystemLanguage->operator[](i));
        }
	CleanupStack::Pop();
    return retValue;
    }
