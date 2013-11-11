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
* Description:   Text style
*
*/



#include <AknFontProvider.h>
#include "alf/alftextstyle.h"
#include "alf/alfgencomponent.h"
#include "alf/alftextstylemanager.h"
#include "alf/alftexturemanager.h"
#include "alf/alfbitmapprovider.h"
#include "alf/alfenv.h"
#include "alflogger.h"
#include "alf/alfconstants.h"
#include "uiacceltk/HuiFont.h"

/**
 * Constants that are uset to set/reset local definition flags.
 * Local definition flags describe which text style attributes are set/defined
 * locally in this particular text style, and which cascade from the parent.
 */
const TInt  KTextColorDefined =         0x00000001;
//const TInt  KBackgroundColorDefined =   0x00000002;
const TInt  KTextSizeDefined =          0x00000004;
const TInt  KUnderlineDefined =         0x00000008;
const TInt  KStrikeThroughDefined =     0x00000010;
const TInt  KStrokeWeightDefined =      0x00000020;
const TInt  KPostureDefined =           0x00000040;
const TInt  KFontDefined =              0x00000080;

#ifndef ALF_RASTER_TEXT


// Private data
struct CAlfTextStyle::TPrivateData
    {
    CAlfTextStyleManager* iManager; // Not owned.
    CAlfGenComponent* iComms;       // Owned.
    TInt iId; // Owned.
    TInt iParentId; // Owned.
    TInt iFontStyleId; // Owned
    TInt iLocalDefinitionFlags; // Owned.
    TRgb iTextColor; // Owned.
    TBool iIsUnderline; // Owned.
    TBool iIsStrikeThrough; // Owned.
    TBool iIsPreconfigured; // Owned.
    TInt iServerHandle; // Serverside id
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfTextStyle::CAlfTextStyle()
	: iData(NULL)
	{
	}
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfTextStyle::~CAlfTextStyle()
	{
    if(iData)
	    {
    	delete iData->iComms;
    	iData->iComms = NULL;
	    }
	delete iData;
	iData = NULL;
	}

// ---------------------------------------------------------------------------
// Constructs a new CAlfTextStyle object
// ---------------------------------------------------------------------------
//
CAlfTextStyle* CAlfTextStyle::NewL(CAlfEnv& aEnv, TInt aId, TInt aImplementationId, const TDesC8& aConstructionParams)
	{
	CAlfTextStyle* self = CAlfTextStyle::NewLC(aEnv, aId, aImplementationId, aConstructionParams);        
    CleanupStack::Pop(self);
    return self;			
	}

// ---------------------------------------------------------------------------
// Constructs a new CAlfTextStyle object
// ---------------------------------------------------------------------------
//
CAlfTextStyle* CAlfTextStyle::NewLC(CAlfEnv& aEnv, TInt aId, TInt aImplementationId, const TDesC8& aConstructionParams)
	{
	CAlfTextStyle* self = new( ELeave ) CAlfTextStyle();
    CleanupStack::PushL(self);
    self->ConstructL(aEnv, aId, aImplementationId, aConstructionParams);
    return self;			
	}

// ---------------------------------------------------------------------------
// Return font style id
// ---------------------------------------------------------------------------
//
TInt CAlfTextStyle::FontStyleId() const
    {
	return iData->iFontStyleId;
    }
    
// ---------------------------------------------------------------------------
// Set font style id
// ---------------------------------------------------------------------------
//
void CAlfTextStyle::SetFontStyleId(TInt aFontStyleId)
    {
	iData->iFontStyleId = aFontStyleId;
    
    // The changes are not reflected in Hitchcockcore because Hitchcockcore
    // text style object does not store this info        
    }
    
// ---------------------------------------------------------------------------
// Return parent id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyle::ParentId() const
    {
	return iData->iParentId;
    }
    
// ---------------------------------------------------------------------------
// Set parent id
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetParentId(TInt aParentId)
    {
    if(!iData->iIsPreconfigured)
        {
        iData->iParentId = aParentId;
        
        // Reflect the changes also in Hitchcockcore        
        // Convert the parent id from client domain to session domain.
    	CAlfTextStyle* parentStyle = iData->iManager->TextStyle(aParentId);    	        
	    TPckgC<TInt> buf(parentStyle->Comms()->Identifier());
	    TBuf8<1> dum;
	    
	    TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleSetParent, buf, dum);
	
	    if ( err != KErrNone )
	        {
	        __ALFLOGSTRING1( "CAlfTextStyle::SetParentId panic error %d", err )
	        User::Invariant();
	        }        
        }    
    }

// ---------------------------------------------------------------------------
// Return Id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyle::Id() const
    {
	return iData->iId;
    }
    
// ---------------------------------------------------------------------------
// Return text color
// ---------------------------------------------------------------------------
//
EXPORT_C TRgb CAlfTextStyle::TextColor() const
    {
	if(iData->iIsPreconfigured)
		{
	    TBufC8<1> inDum;
	    TRgb textColor;
	    TPckg<TRgb> outBuf(textColor);
	    
	    TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleTextColor, inDum, outBuf);
	    
	    if ( err != KErrNone )
	        {
	        __ALFLOGSTRING1( "CAlfTextStyle::TextColor ignore error %d", err )
	        } 
	        
	    return textColor;	
		}
    if(iData->iLocalDefinitionFlags & KTextColorDefined)
        {
        return iData->iTextColor;
        }
    return iData->iManager->TextStyle(iData->iParentId)->TextColor();
    }

// ---------------------------------------------------------------------------
// Sets the color of the text rasterized with this style.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetTextColor(const TRgb& aTextColor)
    {
    if(!iData->iIsPreconfigured)
        {
        iData->iTextColor = aTextColor;
        iData->iLocalDefinitionFlags |= KTextColorDefined;
        
        TPckgC<TRgb> buf(aTextColor);
        TBuf8<1> dum;
        
        TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleSetTextColor, buf, dum);

        if ( err != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfTextStyle::SetTextColor panic error %d", err )
            User::Invariant();
            }     
        }
    }
    
// ---------------------------------------------------------------------------
// Returns the text size of this style in screen size independent units (twips).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyle::TextSizeInTwips(TBool aIsDecoratedSize) const
    {
    TInt textSizeInTwips = 0;
    
    TPckgC<TInt> buf(aIsDecoratedSize);
    TPckg<TInt> outBuf(textSizeInTwips);
    
    TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleSizeInTwips, buf, outBuf);
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextStyle::TextSizeInTwips ignore error %d", err )
        } 
        
    return textSizeInTwips;
    }
    
// ---------------------------------------------------------------------------
// Set text size
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetTextSizeInTwips(TInt aTextSizeInTwips, TBool aIsDecoratedSize)
    {
    if(!iData->iIsPreconfigured)
        {
        TInt2 params(aTextSizeInTwips, aIsDecoratedSize);
        TPckgC<TInt2> buf(params);       
        TBuf8<1> dum;
        
        TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleSetTextSizeInTwips, buf, dum);

        if ( err != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfTextStyle::SetTextSizeInTwips panic error %d", err )
            User::Invariant();
            }     
        }
    }
    
// ---------------------------------------------------------------------------
// Get text size
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyle::TextSizeInPixels(TBool aIsDecoratedSize) const
	{
	TInt textSizeInPixels = 0;

    TPckgC<TInt> buf(aIsDecoratedSize);
	TPckg<TInt> outBuf(textSizeInPixels);
	
	TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleSizeInPixels, buf, outBuf);
	
	if ( err != KErrNone )
		{
	    __ALFLOGSTRING1( "CAlfTextStyle::TextSizeInPixels ignore error %d", err )
		}
	        
	return textSizeInPixels;
	}
    
 // ---------------------------------------------------------------------------
 // Set text size
 // ---------------------------------------------------------------------------
 //
EXPORT_C void CAlfTextStyle::SetTextSizeInPixels(TInt aTextSizeInPixels, TBool aIsDecoratedSize)
    {
    if(!iData->iIsPreconfigured)
        {
        TInt2 params(aTextSizeInPixels, aIsDecoratedSize);
        TPckgC<TInt2> buf(params);   
        TBuf8<1> dum;
        
        TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleSetTextSizeInPixels, buf, dum);

        if ( err != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfTextStyle::SetTextSizeInPixels panic error %d", err )
            User::Invariant();
            }
        }
    }
    
// ---------------------------------------------------------------------------
// Is bold
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTextStyle::IsBold() const
    {
	TBufC8<1> inDum;
	TBool isBold = EFalse;
	TPckg<TBool> outBuf(isBold);
	
	TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleIsBold, inDum, outBuf);
	
	if ( err != KErrNone )
		{
	    __ALFLOGSTRING1( "CAlfTextStyle::IsBold ignore error %d", err )
		}
	        
	return isBold;
    }
    
// ---------------------------------------------------------------------------
// Set bold
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetBold(TBool aIsBold)
    {
    if(!iData->iIsPreconfigured)
        {
        TPckgC<TInt> buf(aIsBold);
        TBuf8<1> dum;
        
        TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleSetBold, buf, dum);

        if ( err != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfTextStyle::SetBold panic error %d", err )
            User::Invariant();
            }
        }    
    }
    
// ---------------------------------------------------------------------------
// Is italic
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTextStyle::IsItalic() const
    {
	TBufC8<1> inDum;
	TBool isItalic = EFalse;
	TPckg<TBool> outBuf(isItalic);
	
	TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleIsItalic, inDum, outBuf);
	
	if ( err != KErrNone )
		{
	    __ALFLOGSTRING1( "CAlfTextStyle::IsItalic ignore error %d", err )
		}
	        
	return isItalic;
    }
    
// ---------------------------------------------------------------------------
// Set italic
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetItalic(TBool aIsItalic)
    {
    if(!iData->iIsPreconfigured)
	    {
	    TPckgC<TInt> buf(aIsItalic);
	    TBuf8<1> dum;
	    
	    TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleSetItalic, buf, dum);
	
	    if ( err != KErrNone )
	        {
	        __ALFLOGSTRING1( "CAlfTextStyle::SetItalic panic error %d", err )
	        User::Invariant();
	        }
	    }    
    }
    
// ---------------------------------------------------------------------------
// Is underlined
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTextStyle::IsUnderline() const
    {
	if(iData->iIsPreconfigured)
		{
	    TBufC8<1> inDum;
	    TBool isUnderline = EFalse;
	    TPckg<TBool> outBuf(isUnderline);
	    
	    TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleIsUnderline, inDum, outBuf);
	    
	    if ( err != KErrNone )
	        {
	        __ALFLOGSTRING1( "CAlfTextStyle::IsUnderline ignore error %d", err )
	        } 
	        
	    return isUnderline;
		}	
    if(iData->iLocalDefinitionFlags & KUnderlineDefined)
        {
        return iData->iIsUnderline;
        }
    return iData->iManager->TextStyle(iData->iParentId)->IsUnderline();
    }
    
// ---------------------------------------------------------------------------
// Set underlined
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetUnderline(TBool aIsUnderline)
    {
    if(!iData->iIsPreconfigured)
        {
        iData->iIsUnderline = aIsUnderline;
        iData->iLocalDefinitionFlags |= KUnderlineDefined;
        
	    TPckgC<TInt> buf(aIsUnderline);
	    TBuf8<1> dum;
	    
	    TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleSetUnderline, buf, dum);
	
	    if ( err != KErrNone )
	        {
	        __ALFLOGSTRING1( "CAlfTextStyle::SetUnderline panic error %d", err )
	        User::Invariant();
	        }
        }
    }
    
// ---------------------------------------------------------------------------
// Is strike through
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTextStyle::IsStrikeThrough() const
    {
	if(iData->iIsPreconfigured)
		{
	    TBufC8<1> inDum;
	    TBool isStrikeThrough = EFalse;
	    TPckg<TBool> outBuf(isStrikeThrough);
	    
	    TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleIsStrikeThrough, inDum, outBuf);
	    
	    if ( err != KErrNone )
	        {
	        __ALFLOGSTRING1( "CAlfTextStyle::IsStrikeThrough ignore error %d", err )
	        } 
	        
	    return isStrikeThrough;
		}	
    if(iData->iLocalDefinitionFlags & KStrikeThroughDefined)
        {
        return iData->iIsStrikeThrough;
        }
    return iData->iManager->TextStyle(iData->iParentId)->IsStrikeThrough();
    }
    
// ---------------------------------------------------------------------------
// Set strike through
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetStrikeThrough(TBool aIsStrikeThrough)
    {
    if(!iData->iIsPreconfigured)
        {
        iData->iIsStrikeThrough = aIsStrikeThrough;
        iData->iLocalDefinitionFlags |= KStrikeThroughDefined;
        
	    TPckgC<TInt> buf(aIsStrikeThrough);
	    TBuf8<1> dum;
	    
	    TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleSetStrikeThrough, buf, dum);
	
	    if ( err != KErrNone )
	        {
	        __ALFLOGSTRING1( "CAlfTextStyle::SetStrikeThrough panic error %d", err )
	        User::Invariant();
	        }
        }
    }

// ---------------------------------------------------------------------------
// Get the typeface
// ---------------------------------------------------------------------------
//
void CAlfTextStyle::GetTypeface( TTypeface& aTypeface ) const
	{
    TBufC8<1> inDum;
	TPckg<TTypeface> outBuf( aTypeface );
	
	TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleGetTypeface, inDum, outBuf);
	
	if ( err != KErrNone )
		{
	    __ALFLOGSTRING1( "CAlfTextStyle::GetTypeface ignore error %d", err )
		}
	        
	return;
	}
	
// ---------------------------------------------------------------------------
// Get the typeface
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CAlfTextStyle::TypefaceNameL( ) const
	{
       TBufC8<1> inDum;
       TTypeface aTypeface;
	TPckg<TTypeface> outBuf( aTypeface );
	
	TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleGetTypeface, inDum, outBuf);
	
	if ( err != KErrNone )
		{
	    __ALFLOGSTRING1( "CAlfTextStyle::TypefaceName ignore error %d", err )
		}
        return aTypeface.iName.AllocL();	        
	}
	
 // ---------------------------------------------------------------------------
 // Set Text Pane Height In Pixels
 // ---------------------------------------------------------------------------
 //
EXPORT_C void CAlfTextStyle::SetTextPaneHeightInPixels(TInt aTextPaneHeight, TBool aIsDecoratedSize)
    {
    if(!iData->iIsPreconfigured)
        {
        TInt2 params(aTextPaneHeight, aIsDecoratedSize);
        TPckgC<TInt2> buf(params);   
        TBuf8<1> dum;
        
        TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleSetTextPaneHeightInPixels, buf, dum);

        if ( err != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfTextStyle::SetTextPaneHeightInPixels panic error %d", err )
            User::Invariant();
            }
        }
    }
	
	
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfTextStyle::ConstructL(CAlfEnv& aEnv, TInt aId, TInt aImplementationId, const TDesC8& aConstructionParams)
    {
	if(!iData)
		{
	    iData = new (ELeave) TPrivateData;
	    iData->iComms = NULL;
	    iData->iId = 0;
	    iData->iLocalDefinitionFlags = 0;
	    iData->iManager = 0;
	    iData->iParentId = 0;
	    iData->iFontStyleId = 0;
	    iData->iIsUnderline = EFalse;
	    iData->iIsStrikeThrough = EFalse;
	    iData->iIsPreconfigured = EFalse;
		}
	
    if (!iData->iComms)
        {
        iData->iComms = CAlfGenComponent::NewL(
            aEnv,
            aImplementationId, 
            0,
            aConstructionParams);  
        }    
    iData->iId = aId;
    iData->iManager = &aEnv.TextStyleManager();
    iData->iIsPreconfigured = (aImplementationId == EAlfPreconfiguredTextStyleCreate) ? ETrue : EFalse;
    iData->iServerHandle = KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Return Alf general component instance
// ---------------------------------------------------------------------------
//
CAlfGenComponent* CAlfTextStyle::Comms() const
	{
	return iData->iComms;
	}
	
// ---------------------------------------------------------------------------
// Return serverside id
// ---------------------------------------------------------------------------
//
TInt CAlfTextStyle::ServerHandle() const
    {
    if (iData->iServerHandle != KErrNotFound)
        {
        return iData->iServerHandle;    
        }
    else
        {
        TPckg<TInt> buf(iData->iServerHandle);   
        TBuf8<1> dum;        
        TInt err = iData->iComms->DoSynchronousCmd(EAlfTextStyleServerHandle, dum, buf);        
        if ( err != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfTextStyle::ServerHandle panic error %d", err )
            User::Invariant();
            }
        return iData->iServerHandle;
        }            
    }
#else
NONSHARABLE_CLASS(CTextBitmapProvider):public CBase, public MAlfBitmapProvider
    {
    public: 
    virtual void ProvideBitmapL(TInt /*aId*/, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap)
        {
        aBitmap = iColorBitmap;
        iColorBitmap = 0;
        aMaskBitmap = iAlphaBitmap;
        iAlphaBitmap= 0;
        }
        
    ~CTextBitmapProvider()
        {
        delete iColorBitmap;
        delete iAlphaBitmap;
        }
        
    CFbsBitmap* iColorBitmap;
    CFbsBitmap* iAlphaBitmap;
    };
    
// Private data
struct CAlfTextStyle::TPrivateData
    {
    CAlfTextStyleManager* iManager; // Not owned.
    TInt iId; // Owned.
    TInt iParentId; // Owned.
    TInt iFontStyleId; // Owned
    TInt iLocalDefinitionFlags; // Owned.
    TRgb iTextColor; // Owned.
    TBool iIsUnderline; // Owned.
    TBool iIsStrikeThrough; // Owned.
    THuiFont iFont; // cool
    CTextBitmapProvider* iBitmapProvider;
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfTextStyle::CAlfTextStyle()
	: iData(NULL)
	{
	}
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfTextStyle::~CAlfTextStyle()
	{
    if(iData)
	    {
    	//delete iData->iComms;
    	//iData->iComms = NULL;
	    delete iData->iBitmapProvider;
	    }
	delete iData;
	iData = NULL;
	}

// ---------------------------------------------------------------------------
// Constructs a new CAlfTextStyle object
// ---------------------------------------------------------------------------
//
CAlfTextStyle* CAlfTextStyle::NewL(CAlfEnv& aEnv, TInt aId, TInt aImplementationId, const TDesC8& aConstructionParams)
	{
	CAlfTextStyle* self = CAlfTextStyle::NewLC(aEnv, aId, aImplementationId, aConstructionParams);        
    CleanupStack::Pop(self);
    return self;			
	}

// ---------------------------------------------------------------------------
// Constructs a new CAlfTextStyle object
// ---------------------------------------------------------------------------
//
CAlfTextStyle* CAlfTextStyle::NewLC(CAlfEnv& aEnv, TInt aId, TInt aImplementationId, const TDesC8& aConstructionParams)
	{
	CAlfTextStyle* self = new( ELeave ) CAlfTextStyle();
    CleanupStack::PushL(self);
    self->ConstructL(aEnv, aId, aImplementationId, aConstructionParams);
    return self;			
	}

// ---------------------------------------------------------------------------
// Return font style id
// ---------------------------------------------------------------------------
//
TInt CAlfTextStyle::FontStyleId() const
    {
	return iData->iFontStyleId;
    }
    
// ---------------------------------------------------------------------------
// Set font style id
// ---------------------------------------------------------------------------
//
void CAlfTextStyle::SetFontStyleId(TInt aFontStyleId)
    {
	iData->iFontStyleId = aFontStyleId;
    ReportChanged();
    }
    
// ---------------------------------------------------------------------------
// Return parent id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyle::ParentId() const
    {
	return iData->iParentId;
    }
    
// ---------------------------------------------------------------------------
// Set parent id
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetParentId(TInt aParentId)
    {
    iData->iParentId = aParentId;
    ReportChanged();
    }

// ---------------------------------------------------------------------------
// Return Id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyle::Id() const
    {
	return iData->iId;
    }
    
// ---------------------------------------------------------------------------
// Return text color
// ---------------------------------------------------------------------------
//
EXPORT_C TRgb CAlfTextStyle::TextColor() const
    {
    if(iData->iLocalDefinitionFlags & KTextColorDefined)
        {
        return iData->iTextColor;
        }
    return iData->iManager->TextStyle(iData->iParentId)->TextColor();
    }

// ---------------------------------------------------------------------------
// Sets the color of the text rasterized with this style.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetTextColor(const TRgb& aTextColor)
    {
    iData->iTextColor = aTextColor;
    iData->iLocalDefinitionFlags |= KTextColorDefined;
    ReportChanged();
    }
    
// ---------------------------------------------------------------------------
// Returns the text size of this style in screen size independent units (twips).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyle::TextSizeInTwips(TBool /*aIsDecoratedSize*/) const
    {
    TFontSpec spec = Font()->FontSpec();
    TInt size = spec.iHeight;
            
    return size;
    }
    
// ---------------------------------------------------------------------------
// Set text size
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetTextSizeInTwips(TInt aTextSizeInTwips, TBool /*aIsDecoratedSize*/)
    {
	TFontSpec spec = OwnFont()->FontSpec();
    
    spec.iHeight = aTextSizeInTwips;
    Font()->SetFontSpec(spec);
    iData->iLocalDefinitionFlags |= KTextSizeDefined;
    ReportChanged();
    }
    
// ---------------------------------------------------------------------------
// Get text size
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyle::TextSizeInPixels(TBool /*aIsDecoratedSize*/) const
	{
    TFontSpec spec = Font()->FontSpec();
    CWsScreenDevice* screenDevice = CCoeEnv::Static()->ScreenDevice();
    TInt textsizeInPix = screenDevice->VerticalTwipsToPixels(spec.iHeight);
            
    return textsizeInPix;
 	}
    
 // ---------------------------------------------------------------------------
 // Set text size
 // ---------------------------------------------------------------------------
 //
EXPORT_C void CAlfTextStyle::SetTextSizeInPixels(TInt aTextSizeInPixels, TBool /*aIsDecoratedSize*/)
    {
    TFontSpec spec = OwnFont()->FontSpec(); 
    CWsScreenDevice* screenDev = CCoeEnv::Static()->ScreenDevice();
    
    TInt textsizeInTwips = screenDev->VerticalPixelsToTwips(aTextSizeInPixels);
    spec.iHeight = textsizeInTwips;
    Font()->SetFontSpec(spec);
    iData->iLocalDefinitionFlags |= KTextSizeDefined;
    ReportChanged();
	}
    
// ---------------------------------------------------------------------------
// Is bold
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTextStyle::IsBold() const
    {
    TFontSpec spec = Font()->FontSpec();
    return spec.iFontStyle.StrokeWeight();
    }
    
// ---------------------------------------------------------------------------
// Set bold
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetBold(TBool aIsBold)
    {
    TFontSpec spec = OwnFont()->FontSpec();
	spec.iFontStyle.SetStrokeWeight((aIsBold ? EStrokeWeightBold : EStrokeWeightNormal));
    Font()->SetFontSpec(spec);
    iData->iLocalDefinitionFlags |= KStrokeWeightDefined;    
    ReportChanged();
    }
    
// ---------------------------------------------------------------------------
// Is italic
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTextStyle::IsItalic() const
    {
    TFontSpec spec = Font()->FontSpec();
    return spec.iFontStyle.Posture();
    }
    
// ---------------------------------------------------------------------------
// Set italic
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetItalic(TBool aIsItalic)
    {
    TFontSpec spec = OwnFont()->FontSpec();
	spec.iFontStyle.SetPosture((aIsItalic ? EPostureItalic : EPostureUpright));
    Font()->SetFontSpec(spec);
    iData->iLocalDefinitionFlags |= KPostureDefined;
    ReportChanged();
    }
    
// ---------------------------------------------------------------------------
// Is underlined
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTextStyle::IsUnderline() const
    {
    if(iData->iLocalDefinitionFlags & KUnderlineDefined)
        {
        return iData->iIsUnderline;
        }
    return iData->iManager->TextStyle(iData->iParentId)->IsUnderline();
    }
    
// ---------------------------------------------------------------------------
// Set underlined
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetUnderline(TBool aIsUnderline)
    {
    iData->iIsUnderline = aIsUnderline;
    iData->iLocalDefinitionFlags |= KUnderlineDefined;
    ReportChanged();
    }
    
// ---------------------------------------------------------------------------
// Is strike through
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTextStyle::IsStrikeThrough() const
    {
    if(iData->iLocalDefinitionFlags & KStrikeThroughDefined)
        {
        return iData->iIsStrikeThrough;
        }
    return iData->iManager->TextStyle(iData->iParentId)->IsStrikeThrough();
    }
    
// ---------------------------------------------------------------------------
// Set strike through
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextStyle::SetStrikeThrough(TBool aIsStrikeThrough)
    {
    iData->iIsStrikeThrough = aIsStrikeThrough;
    iData->iLocalDefinitionFlags |= KStrikeThroughDefined;
    ReportChanged();
    }

// ---------------------------------------------------------------------------
// Get the typeface
// ---------------------------------------------------------------------------
//
void CAlfTextStyle::GetTypeface( TTypeface& aTypeface ) const
	{
    TFontSpec spec = Font()->FontSpec();
    aTypeface = spec.iTypeface; // Structure copy
	}
	
// ---------------------------------------------------------------------------
// Get the typeface
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CAlfTextStyle::TypefaceNameL( ) const
	{
	return Font()->FontSpec().iTypeface.iName.AllocL();
	}
	
 // ---------------------------------------------------------------------------
 // Set Text Pane Height In Pixels
 // ---------------------------------------------------------------------------
 //
EXPORT_C void CAlfTextStyle::SetTextPaneHeightInPixels(TInt aTextPaneHeight, TBool /*aIsDecoratedSize*/)
    {
    TFontSpec oldFontSpec = Font()->FontSpec(); 
    
    // now generate a font with the new text pane height 
    CWsScreenDevice* screenDev = CCoeEnv::Static()->ScreenDevice();
    TAknFontSpecification aknFs(Font()->Category(), oldFontSpec, screenDev);
    aknFs.SetTextPaneHeight(aTextPaneHeight);

    // and get its corresponding spec
    TFontSpec newFontSpec;
    TInt foundIndex(KErrNotFound);
    AknFontProvider::GetFontSpecFromMetrics(*screenDev, 0, aknFs, newFontSpec, foundIndex);

    if(foundIndex != KErrNotFound)
        {
        TInt textsizeInTwips = screenDev->VerticalPixelsToTwips(newFontSpec.iHeight);
        newFontSpec.iHeight = textsizeInTwips;
        OwnFont()->SetFontSpec(newFontSpec);
        iData->iLocalDefinitionFlags |= KTextSizeDefined;
        }
    }

// ---------------------------------------------------------------------------
// CAlfTextStyle::RasterizeLineL
// Rasterizes the given string on to the target texture using this text style.
// ---------------------------------------------------------------------------
//
void CAlfTextStyle::RasterizeLineL(const TDesC& aTextLine, CAlfTexture** aTargetTexture)
	{
	// Calculate the pixel extents of the text line.
	TSize textureSize = LineExtentsL(aTextLine);
	
	//TSize maxTextureSize = (*aTargetTexture)->MaxTextureSize();

    if (!iData->iBitmapProvider)
        {
        iData->iBitmapProvider = new (ELeave) CTextBitmapProvider();
        }
    
    if ( !iData->iBitmapProvider->iColorBitmap || textureSize != iData->iBitmapProvider->iColorBitmap->SizeInPixels())  
        {
        // could resize existing instead..    
        delete iData->iBitmapProvider->iColorBitmap;
        iData->iBitmapProvider->iColorBitmap = 0;
        // Create target alpha bitmap to rasterize the text onto.
	    iData->iBitmapProvider->iColorBitmap = new (ELeave) CFbsBitmap();    
	    User::LeaveIfError(iData->iBitmapProvider->iColorBitmap->Create(textureSize, EGray256));
        }
	
	// Create the bitmap device to be used in rasterization.
    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(iData->iBitmapProvider->iColorBitmap);
	CleanupStack::PushL(device);
	
	// Create drawing context.
	CFbsBitGc* gc = 0;
	User::LeaveIfError(device->CreateContext(gc));
	CleanupStack::PushL(gc);
	
	// Prepare the bitmap for drawing.
	gc->SetBrushColor(KRgbBlack);
	gc->Clear();
	
	// Draw the text onto the bitmap.    
	gc->SetPenColor(KRgbWhite);
		
	// Do underlining
	gc->SetUnderlineStyle(IsUnderline() ? EUnderlineOn : EUnderlineOff);
	// Do strikethrough
	gc->SetStrikethroughStyle(IsStrikeThrough() ? EStrikethroughOn : EStrikethroughOff);
	
	//if(iClippingEnabled)
	//{
    //	gc->SetClippingRect(iClipRect);
	//}
    
	// Rasterize text string using the configured font.
	Font()->RasterizeLineL(aTextLine, *gc);
	
	// Destroy graphics context and drawing device.
	CleanupStack::PopAndDestroy(gc);
	CleanupStack::PopAndDestroy(device);
    

	delete iData->iBitmapProvider->iAlphaBitmap;
	iData->iBitmapProvider->iAlphaBitmap = NULL;
	
    // todo: proper flagging, e.g. use directly        
    *aTargetTexture = &CAlfEnv::Static()->TextureManager().CreateTextureL(KAlfAutoGeneratedTextureId, 
                                                                        iData->iBitmapProvider,
                                                                        EAlfTextureFlagDefault); 
	}

// ---------------------------------------------------------------------------
// THuiTextStyle::LineExtentsL
// Provides dimensions of the rasterization of the given string. 
// ---------------------------------------------------------------------------
//
TSize CAlfTextStyle::LineExtentsL(const TDesC& aTextLine)
	{    
	// Retrieve the non-decorated line extents from the used font.
	return Font()->LineExtentsL(aTextLine);
	}
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfTextStyle::ConstructL(CAlfEnv& aEnv, TInt aId, TInt aImplementationId, const TDesC8& /*aConstructionParams*/)
    {
	if(!iData)
		{
	    iData = new (ELeave) TPrivateData;
	    iData->iId = 0;
	    iData->iLocalDefinitionFlags = 0;
	    iData->iManager = 0;
	    iData->iParentId = 0;
	    iData->iFontStyleId = aImplementationId;
	    iData->iIsUnderline = EFalse;
	    iData->iIsStrikeThrough = EFalse;
		iData->iBitmapProvider = 0;
		}
	
/*    if (!iData->iComms)
        {
        iData->iComms = CAlfGenComponent::NewL(
            aEnv,
            aImplementationId, 
            0,
            aConstructionParams);  
        }*/
           
    iData->iId = aId;
    //iData->iManager = &aEnv.TextStyleManager();
    //iData->iIsPreconfigured = (aImplementationId == EAlfPreconfiguredTextStyleCreate) ? ETrue : EFalse;
    //iData->iServerHandle = KErrNotFound;
    iData->iBitmapProvider = new (ELeave) CTextBitmapProvider();
    }

CAlfGenComponent* CAlfTextStyle::Comms() const
	{
    return 0;
	}
	
TInt CAlfTextStyle::ServerHandle() const
    {
    return KErrNotFound;
    }
	
THuiFont* CAlfTextStyle::Font() const
    {
    if(iData->iLocalDefinitionFlags & KFontDefined)
        {
        return &iData->iFont;
        }
        
    return iData->iManager->TextStyle(iData->iParentId)->Font();
    }
    
THuiFont* CAlfTextStyle::OwnFont()
    {
    if(!(iData->iLocalDefinitionFlags & KFontDefined))
        {
        SetFont(iData->iManager->TextStyle(iData->iParentId)->Font());
        }
    
    return Font();
    }

void CAlfTextStyle::SetFont(THuiFont* aFont)
    {
    if (aFont)
        {
        iData->iFont = *aFont;
        iData->iLocalDefinitionFlags |= KFontDefined;
        }
    else    
        {
        iData->iLocalDefinitionFlags &= ~KFontDefined;
        }
        
    ReportChanged();
    }

void CAlfTextStyle::ReportChanged()
    {
    if (iData->iManager)
        { // when creating platform style, manager is not ready yet
        iData->iManager->RefreshVisuals(iData->iId);    
        }
    }

void CAlfTextStyle::SetManager(CAlfTextStyleManager* aManager)
    {
    iData->iManager = aManager;
    }
#endif	
