/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of CHuiTextVisual. CHuiTextVisual is a visual
*                that draws text.
*
*/



#include <AknsConstants.h>
#include <AknsUtils.h>
#include <AknPictographInterface.h>
#include <AknPictographDrawerInterface.h>
#include <AknUtils.h>
#include "uiacceltk/HuiTextVisual.h"  // Class definition
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiDrawing.h"
#include "uiacceltk/HuiTextMesh.h"  
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/huitextstyle.h"
#include "uiacceltk/huitextstylemanager.h"
#include "uiacceltk/huidropshadow.h"

#include "HuiRosterImpl.h"

// Internal class to handle Japanese pictograph animations.
NONSHARABLE_CLASS(CHuiPictographAnimator) : public CBase, public MAknPictographAnimatorCallBack
    {
public:
    CHuiPictographAnimator(CHuiTextVisual* aVisual, CHuiTextMesh* aTextMesh);
    ~CHuiPictographAnimator();
    TBool IsAnimating() const;
    void SetAnimating(TBool aAnimating);    
    // From MAknPictographAnimatorCallBack
    void DrawPictographArea();
private:    
    CHuiTextVisual* iVisual;
    CHuiTextMesh* iTextMesh;    
    TBool iAnimating;
    };
    
CHuiPictographAnimator::CHuiPictographAnimator(CHuiTextVisual* aVisual, CHuiTextMesh* aTextMesh)
    {
    iVisual = aVisual;
    iTextMesh = aTextMesh;	
    }

CHuiPictographAnimator::~CHuiPictographAnimator()
    {	
    iVisual = NULL;
    iTextMesh = NULL;
    }

TBool CHuiPictographAnimator::IsAnimating() const
    {
    return iAnimating;	
    }

void CHuiPictographAnimator::SetAnimating(TBool aAnimating)
    {
    iAnimating = aAnimating;	
    }

void CHuiPictographAnimator::DrawPictographArea()
    {
    // This condition might need some further consideration.
    if (iVisual->EffectiveOpacity() > 0.0 && 
        CHuiStatic::Env().RefreshMode() != EHuiRefreshModeManual)
        {
        SetAnimating(ETrue);
        // This BuildPictographsL()-call also keeps the animation ticker 
        // behind CAknPictographInterface alive.
        TRAP_IGNORE(iTextMesh->BuildPictographsL())
        iVisual->SetChanged();            
        }
    else
        {
        // No call made to CAknPictographInterface -> animation timer gets automatically freezed.
        SetAnimating(EFalse);                    
        }    
    }


EXPORT_C CHuiTextVisual* CHuiTextVisual::AddNewL(CHuiControl& aOwnerControl,
                                                 CHuiLayout* aParentLayout)
    {
    CHuiTextVisual* text = STATIC_CAST(CHuiTextVisual*,
        aOwnerControl.AppendVisualL(EHuiVisualTypeText, aParentLayout));
    return text;
    }


CHuiTextVisual::CHuiTextVisual(MHuiVisualOwner& aOwner)
        : CHuiVisual(aOwner),
          iLineWrapping(ELineWrapManual),
          iAlignHorizontal(EHuiAlignHCenter),
          iAlignVertical(EHuiAlignVCenter),
          iStyle(EHuiTextStyleNormal),
          iFontColor(KRgbWhite),
          iFontColorId(KAknsIIDNone)
    {
    }


void CHuiTextVisual::ConstructL()
    {
    CHuiVisual::ConstructL();
    
    iTextMesh = CHuiStatic::Renderer().CreateTextMeshL();
    iTextMesh->SetRelatedVisual( this );
    
    iMeshUpdated = EFalse;
    iExtentsUpdated = EFalse;
    iPictographAnimator = new (ELeave) CHuiPictographAnimator(this, iTextMesh);
    // Ugly missing CCoeControl workaround, but CAknPictographInterface does not seem to use CCoeControl.
    CCoeControl* dummy = NULL;    
    iPictographInterface = CAknPictographInterface::NewL( *dummy, *iPictographAnimator ); // Returns NULL if not supported.
    if (iPictographInterface)
        {
        iTextMesh->InitPictographsL(iPictographInterface);    
        }
    }


CHuiTextVisual::~CHuiTextVisual()
    {
    delete iText;
    delete iTextMesh;
    delete iPictographAnimator;    
    delete iPictographInterface;
    }


EXPORT_C void CHuiTextVisual::SetStyle(THuiPreconfiguredTextStyle aStyle, THuiBackgroundType aBackgroundType)
    {
    SetTextStyle(aStyle);
    iStyle = aStyle;
    THuiTextStyle* textStyle = Env().TextStyleManager().TextStyle(aStyle);
    
    if( aBackgroundType == EHuiBackgroundTypeLight)
        {
        textStyle->SetTextColor(KRgbBlack);
        }
    else
        {
        textStyle->SetTextColor(KRgbWhite);
        }
    iBackgroundType = aBackgroundType;
    }


EXPORT_C THuiPreconfiguredTextStyle CHuiTextVisual::Style() const
    {
    return iStyle;
    }


EXPORT_C void CHuiTextVisual::SetMaxLineCount(TInt aMaxLineCount)
    {
    if(iTextMesh->SetMaxLineCount(aMaxLineCount))
        {
        iMeshUpdated = EFalse;
        iExtentsUpdated = EFalse;        
        SetChanged();
        }
    }


EXPORT_C TInt CHuiTextVisual::MaxLineCount() const
    {
    return iTextMesh->MaxLineCount();
    }


EXPORT_C CHuiTextVisual::TLineWrap CHuiTextVisual::Wrapping() const
    {
    return iLineWrapping;
    }


EXPORT_C void CHuiTextVisual::SetWrapping(TLineWrap aWrap)
    {
    iLineWrapping = aWrap;
    switch(iLineWrapping)
        {
        case ELineWrapManual:
        case ELineWrapTruncate:
            iTextMesh->SetLineMode(CHuiTextMesh::ELineModeTruncate);
            break;

        case ELineWrapBreak:
            iTextMesh->SetLineMode(CHuiTextMesh::ELineModeWrap);
            break;
        }
    SetChanged();
    }


EXPORT_C THuiBackgroundType CHuiTextVisual::BackgroundType() const
    {
    return iBackgroundType;
    }


EXPORT_C void CHuiTextVisual::SetTextL(const TDesC& aText)
    {
    delete iText; iText = 0;
    iText = aText.AllocL();

    iMeshUpdated = EFalse;
    iExtentsUpdated = EFalse;    
    SetChanged();
    }

    
EXPORT_C const TDesC& CHuiTextVisual::Text() const
    {
    if(iText)
        {
        return *iText;
        }
    _LIT(KNoText, "");
    return KNoText;
    }


EXPORT_C void CHuiTextVisual::SetAlign(THuiAlignHorizontal aAlignHorizontal,
                                       THuiAlignVertical aAlignVertical)
    {
    iAlignHorizontal = aAlignHorizontal;
    iAlignVertical = aAlignVertical;

    SetChanged();
    }


EXPORT_C void CHuiTextVisual::SetLineSpacing(TInt aLineSpacing, TLineSpacingUnits aUnits)
    {
    TInt lineSpacingInPixels = 0;
    
    if(aUnits == ETwips)
        {
        CWsScreenDevice* screenDev = CHuiStatic::ScreenDevice();
        lineSpacingInPixels = screenDev->VerticalTwipsToPixels(aLineSpacing);
        }
    else
        {
        lineSpacingInPixels = aLineSpacing;  
        }
        
    iTextMesh->SetLineSpacing(lineSpacingInPixels);     
    }


EXPORT_C TSize CHuiTextVisual::TextExtents() const
    {
    if(!iText)
        {
        return TSize(0, 0);
        }

	if (!iExtentsUpdated)
		{
		TRAPD(err, iTextMesh->SetTextL(*iText, EFalse));
		if (err != KErrNone) 
			{
			HUI_DEBUG1(_L("CHuiTextVisual::TextExtents() - ERROR! Failed to update mesh with leave errorcode %i. Text extents are not currently available. "), err);
			iExtentsUpdated = EFalse; // we have to retry soon..
            return TSize(0, 0);
			}
			
	    iExtentsUpdated = ETrue;
		}
	
    return iTextMesh->Extents();
    }

EXPORT_C TRect CHuiTextVisual::SubstringExtents(TUint aStart, TUint aEnd) const
    {
    if(!iText)
        {
        return TRect(0, 0, 0, 0);
        }

    // Retrieve the text style used when rasterizing this text.
    THuiTextStyle* textStyle = CHuiStatic::Env().TextStyleManager().TextStyle(iTextMesh->TextStyle());
    
    TSize begin(0,0); 
    TSize end(0,0);
    TRAP_IGNORE( 
        {
        begin = textStyle->LineExtentsL(iText->Mid(0, aStart));
        end = textStyle->LineExtentsL(iText->Mid(0, aEnd));
        })

    return TRect(begin.iWidth, 0, end.iWidth, end.iHeight);
    }

const TUint8 color_s_to_lin[256] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
   0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
   0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
   0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04,
   0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05,
   0x05, 0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07,
   0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x0a,
   0x0a, 0x0a, 0x0b, 0x0b, 0x0c, 0x0c, 0x0c, 0x0d,
   0x0d, 0x0e, 0x0e, 0x0e, 0x0f, 0x0f, 0x10, 0x10,
   0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x14,
   0x15, 0x15, 0x16, 0x16, 0x17, 0x17, 0x18, 0x18,
   0x19, 0x1a, 0x1a, 0x1b, 0x1b, 0x1c, 0x1d, 0x1d,
   0x1e, 0x1f, 0x1f, 0x20, 0x21, 0x21, 0x22, 0x23,
   0x23, 0x24, 0x25, 0x26, 0x26, 0x27, 0x28, 0x29,
   0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2d, 0x2e, 0x2f,
   0x30, 0x31, 0x32, 0x33, 0x33, 0x34, 0x35, 0x36,
   0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
   0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
   0x47, 0x48, 0x49, 0x4a, 0x4c, 0x4d, 0x4e, 0x4f,
   0x50, 0x51, 0x52, 0x54, 0x55, 0x56, 0x57, 0x58,
   0x5a, 0x5b, 0x5c, 0x5d, 0x5f, 0x60, 0x61, 0x63,
   0x64, 0x65, 0x67, 0x68, 0x69, 0x6b, 0x6c, 0x6d,
   0x6f, 0x70, 0x72, 0x73, 0x74, 0x76, 0x77, 0x79,
   0x7a, 0x7c, 0x7d, 0x7f, 0x80, 0x82, 0x83, 0x85,
   0x86, 0x88, 0x8a, 0x8b, 0x8d, 0x8e, 0x90, 0x92,
   0x93, 0x95, 0x97, 0x98, 0x9a, 0x9c, 0x9d, 0x9f,
   0xa1, 0xa3, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xad,
   0xaf, 0xb1, 0xb3, 0xb5, 0xb7, 0xb8, 0xba, 0xbc,
   0xbe, 0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc,
   0xce, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc,
   0xde, 0xe0, 0xe2, 0xe5, 0xe7, 0xe9, 0xeb, 0xed,
   0xef, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfd, 0xff };

inline TRgb ConvertToLinear(TRgb aColor)
    {
    // perform sRGB->linear color conversion if the renderer is
    // openvg
    // NOTE: For emulator depending on the OpenVG SW version mapping may
    // be needed or not. Use/unuse ifdefs below if text colors are too dark/light.
//#ifndef __WINSCW__
//    if (CHuiStatic::Env().Renderer() == EHuiRendererVg10)
        {
        TUint32 color = aColor.Internal();
        return
            ((TUint32)color_s_to_lin[(color >> 0) & 0xff] << 16) |
            ((TUint32)color_s_to_lin[(color >> 8) & 0xff] << 8) |
            ((TUint32)color_s_to_lin[(color >> 16) & 0xff] << 0) |
            (color & 0xff000000);
        }
//#endif    
    return aColor;
    }

    
EXPORT_C void CHuiTextVisual::SetColor(const TRgb& aColor)
    {
	iFontColor = ConvertToLinear(aColor);
    iFontColorId = KAknsIIDNone;
    iFontColorIndex = -1;
    iFontColorValid = ETrue;
    SetChanged();
    }
    
    
EXPORT_C void CHuiTextVisual::SetColor(const TAknsItemID& aID,const TInt aIndex)
    {
    iFontColorId = aID;
    iFontColorIndex = aIndex;
    iFontColorValid = EFalse;
    SetChanged();
    }
    

EXPORT_C void CHuiTextVisual::EnableShadow(TBool aDoEnable)
    {
    TRAP_IGNORE( EnableDropShadowL(aDoEnable) );
    }
    

TBool CHuiTextVisual::PrepareDrawL()
	{
    if (Flags() & EHuiVisualFlagDrawOnlyAsExternalContent)
    	{
   	    // This is used only as external content visual. Return now if we are not currently drawing
   	    // external content.
    	if (!Display() || !Display()->RosterImpl().IsDrawingExternalContent())
    		{
			return ETrue;
    		}
    	}

    if(!iText)
        {
        return ETrue;
        }   

    TReal32 effectiveOpacity = EffectiveOpacity();
    if(effectiveOpacity <= 0)
        {
        return ETrue;
        }
        
    if ( DropShadowHandler() )
        {
        if ( DropShadowHandler()->iRadius.Changed() )
            {
            iMeshUpdated = EFalse;
            }
        }

    iTextMesh->EnableRasterizedShadow(TBool(DropShadowHandler()));

    // use directly target rect and thus rasterize the text only once
    // and not for each frame when the visual's size is changing
    THuiRealRect content = DisplayRectTarget();
    content.Shrink(PaddingInPixels());
    
    // In wrapping mode, let the mesh know how much space there is
    // for drawing into.
    TInt maxWidth = KMaxTInt;
    if(iLineWrapping != ELineWrapManual)
        {
        maxWidth = TInt(content.Width()+0.5f);
        }

    // Update the text mesh, if needed. Set max line width first to avoid
    // rendering with wrong max line width.
    if(iTextMesh->SetMaxLineWidth(maxWidth) || !iMeshUpdated) 
        {
        UpdateMeshL();
        }
        
    if (effectiveOpacity > 0 && 
        iPictographInterface &&
        iPictographAnimator && 
        !iPictographAnimator->IsAnimating())
        {
        iPictographAnimator->DrawPictographArea(); // restarts animation timer if needed    
        }
        
	return ETrue;
	}


void CHuiTextVisual::DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const
    {
    if(!iText)
        {
        return;
        }

    TReal32 effectiveOpacity = EffectiveOpacity();
    if(effectiveOpacity <= 0)
        {
        return;
        }        
    aGc.Enable(CHuiGc::EFeatureBlending);
    aGc.SetAlign(iAlignHorizontal, iAlignVertical);
    aGc.SetPenAlpha(TInt(effectiveOpacity * 255));
    
    // Set up the text color. Priority order is: explicit color set, then by skin ID, then by Hui Style
    TBool setDefaultColor = ETrue;
    if ( iFontColorValid )
        {
        setDefaultColor = EFalse;
        aGc.SetPenColor(iFontColor);
        }
    else if (iFontColorId != KAknsIIDNone)
        {
        TRgb color = KRgbGreen;
        setDefaultColor = ( CHuiStatic::GetCachedColor(
                  color, iFontColorId, iFontColorIndex )
                                != KErrNone );
        if ( !setDefaultColor )
            {
            aGc.SetPenColor(color);
            }
        }
    else
        {
        // setDefaultColor equals to ETrue and so it's ok.
        }

    if ( setDefaultColor )
        {
        // Default is to use the color indicated by style
        aGc.SetPenColor(Skin().StyleTextColor(iStyle, iBackgroundType));
        }    

    HUI_DEBUGF3(_L("CHuiTextVisual::DrawSelf() -- iStyle=%i, iBackgroundType=%i, lightness=%f"),
                iStyle, iBackgroundType, HuiUtil::ColorLightness(aGc.PenColor()));

    THuiRealRect content = aDisplayRect;
    content.Shrink(PaddingInPixels());
    content.iTl += LocalPointInPixels(iOffset.Now());

    if ( DropShadowHandler() )
        {
        aGc.DrawText(*iTextMesh, content, DropShadowHandler()->iOpacity.Now());
        }
    else
        {
        aGc.DrawText(*iTextMesh, content, 0.f);
        }


	if(iHighlightEnd - iHighlightStart)
		{
		TPoint textPos(content.iTl.iX, content.iTl.iY);
	        
	     TSize textBounds = TextExtents();
	        
	   	switch(iAlignHorizontal)
	        {
	        case EHuiAlignHRight:
	            textPos.iX = content.iBr.iX - textBounds.iWidth;
	            break;
    
            case EHuiAlignHCenter:
	            textPos.iX = content.Center().iX - textBounds.iWidth / 2; // or 2.f?
	            break;
    
            default:
	            break;
	   	    }
	        
	   	switch(iAlignVertical)
	       	{
	        case EHuiAlignVBottom:
	            textPos.iY = content.iBr.iY - textBounds.iHeight;
	            break;
    
            case EHuiAlignVCenter:
	            textPos.iY = content.Center().iY - textBounds.iHeight / 2; // or 2.f?
	            break;
    
            default:
	            break;
	       	}
	    
		THuiRealRect textRect(textPos, textBounds);

		// hackish, for now
		TBool conversionRequired = ETrue;
		if (iHighlightStart > iText->Length() || iHighlightEnd > iText->Length() )
		    {
		    conversionRequired = ETrue;
		    }
		
        const TBidiText::TDirectionality bidiDirection = TBidiText::TextDirectionality( *iText);
        if ( bidiDirection == TBidiText::ERightToLeft )	        
			{
			textRect.iBr.iX -= conversionRequired?SubstringExtents(0, iHighlightStart).Width():iHighlightStart;	    
		    textRect.iTl.iX = textRect.iBr.iX - conversionRequired?SubstringExtents(iHighlightStart,iHighlightEnd).Width():iHighlightEnd;
			}
		else
			{
			textRect.iTl.iX += conversionRequired?SubstringExtents(0, iHighlightStart).Width():iHighlightStart;	    
		   	textRect.iBr.iX = textRect.iTl.iX + conversionRequired?SubstringExtents(iHighlightStart,iHighlightEnd).Width():iHighlightEnd;
			}
        
	    aGc.SetPenColor(iHighlightColor);
	    aGc.DrawRect(textRect);
	    aGc.SetPenColor(iHighlightTextColor);

		THuiTextStyle* textStyle = CHuiStatic::Env().TextStyleManager().TextStyle(iTextMesh->TextStyle());
		
		textRect.Move(- textPos.iX, - textPos.iY);
		
		textStyle->EnableClipping(textRect);
		
		TRAPD(error,UpdateMeshL());  
				
		if ( DropShadowHandler() )
        	{
        	aGc.DrawText(*iTextMesh, content, DropShadowHandler()->iOpacity.Now());
        	}
    	else
        	{
	    	aGc.DrawText(*iTextMesh, content, 0.f);
        	}

		textStyle->DisableClipping();
	
		TRAP(error,UpdateMeshL());  
		
	    }

    }


TBool CHuiTextVisual::Changed() const
    {
    if(CHuiVisual::Changed())
        {
        return ETrue;
        }
    return (iOffset.Changed() );
    }


void CHuiTextVisual::ClearChanged()
    {
    CHuiVisual::ClearChanged();
    iOffset.ClearChanged();
    }


void CHuiTextVisual::UpdateMeshL() const
    {
    iTextMesh->Reset();
    iTextMesh->SetTextL(*iText, ETrue);
    iMeshUpdated = ETrue;
    iExtentsUpdated = ETrue;    
    }


void CHuiTextVisual::NotifySkinChangedL()
    {
    iMeshUpdated = EFalse;
    iExtentsUpdated = EFalse;    
    SetChanged();
    }


void CHuiTextVisual::ExpandRectWithContent(TRect& aRect) const
    {   
    if(!Clipping() && iText && iText->Length() )
        {
        TSize extents = TextExtents();
        TPoint tl = aRect.Center();
        
        // check top left X
        THuiAlignHorizontal usedHorizontalAlign = iAlignHorizontal;
        switch(iAlignHorizontal)
            {
            case EHuiAlignHLocale:
                if(CHuiStatic::LayoutMirrored())
                    {
                    usedHorizontalAlign = EHuiAlignHRight;
                    }
                else
                    {
                    usedHorizontalAlign = EHuiAlignHLeft;
                    }
                break;

            case EHuiAlignHLocaleMirrored:
                if(CHuiStatic::LayoutMirrored())
                    {
                    usedHorizontalAlign = EHuiAlignHLeft;
                    }
                else
                    {
                    usedHorizontalAlign = EHuiAlignHRight;
                    }
                break;
            case EHuiAlignHBidirectionalText:
                {
                TBool bidiInformationFound = EFalse;
                const TBidiText::TDirectionality bidiDirection = TBidiText::TextDirectionality( *iText, &bidiInformationFound );
                if ( bidiInformationFound )
                    {
                    if ( bidiDirection == TBidiText::ELeftToRight )
                        {
                        usedHorizontalAlign = EHuiAlignHLeft;
                        }
                    else // ERightToLeft
                        {
                        usedHorizontalAlign = EHuiAlignHRight;
                        }
                    }
                else // fall back... use locale
                    {
                    if(CHuiStatic::LayoutMirrored())
                        {
                        usedHorizontalAlign = EHuiAlignHRight;
                        }
                    else
                        {
                        usedHorizontalAlign = EHuiAlignHLeft;
                        }
                    }
                }
                break;
            case EHuiAlignHBidirectionalTextMirrored:
                {
                TBool bidiInformationFound = EFalse;
                const TBidiText::TDirectionality bidiDirection = TBidiText::TextDirectionality( *iText, &bidiInformationFound );
                if ( bidiInformationFound )
                    {
                    if ( bidiDirection == TBidiText::ELeftToRight )
                        {
                        usedHorizontalAlign = EHuiAlignHRight;
                        }
                    else // ERightToLeft
                        {
                        usedHorizontalAlign = EHuiAlignHLeft;
                        }
                    }
                else // fall back... use locale mirrored
                    {
                    if(CHuiStatic::LayoutMirrored())
                        {
                        usedHorizontalAlign = EHuiAlignHLeft;
                        }
                    else
                        {
                        usedHorizontalAlign = EHuiAlignHRight;
                        }
                    }
                }
                break;
            default:
                break;
            }
        
        switch( usedHorizontalAlign )
            {
            case EHuiAlignHLeft:
                tl.iX = aRect.iTl.iX;
                break;
            case EHuiAlignHCenter:
                tl.iX -= extents.iWidth/2;
                break;
            case EHuiAlignHRight:
                tl.iX = aRect.iBr.iX - extents.iWidth;
                break;
            default:
                break;
            }
            
        // check top left Y
        switch( iAlignVertical )
            {
            case EHuiAlignVTop:
                tl.iY = aRect.iTl.iY;
                break;
            case EHuiAlignVCenter:
                tl.iY -= extents.iHeight/2;
                break;
            case EHuiAlignVBottom:
                tl.iY = aRect.iBr.iY - extents.iHeight;
                break;
            default:
                break;
            }
        
        TRect textExtendsRect(tl, extents);
        textExtendsRect.Grow(1,1); // fix rounding errors from Center() and /2 functions.

        textExtendsRect.Move(TPoint(LocalPointInPixels(iOffset.Now()))); 
                
        aRect.BoundingRect(textExtendsRect);
       
        
        // add shadow
        CHuiDropShadow* shadowHandler = DropShadowHandler();
        if ( shadowHandler &&
             !HuiUtil::RealCompare( shadowHandler->iScale.Now(), 0.f ) &&
             !HuiUtil::RealCompare( shadowHandler->iOpacity.Now(), 0.f ) )
            {
            iTextMesh->ExpandRectWithShadow( aRect );
            }
        }

    CHuiVisual::ExpandRectWithContent(aRect);
    }
    

EXPORT_C void CHuiTextVisual::SetTextStyle(TInt aTextStyleId)
    {
    iTextMesh->SetTextStyle(aTextStyleId);
    
    iMeshUpdated = EFalse;
    iExtentsUpdated = EFalse;
    iStyle = STATIC_CAST(THuiPreconfiguredTextStyle, aTextStyleId);
    SetChanged();
    }

EXPORT_C TInt CHuiTextVisual::TextStyle() const
    {
    return iStyle;   
    }


EXPORT_C void CHuiTextVisual::SetHighlightRange(TInt aStart, TInt aEnd, TRgb& aHighlightColor, TRgb& aHighlightTextColor)
	{
	/*
	if((aStart < 0) || (aStart > iText->Length()) || (aEnd < 0))
		return;
	
	if(aStart >= aEnd)
		return;
	
	if(aEnd > iText->Length())
		aEnd = iText->Length();
	*/
    iHighlightStart = aStart;
    iHighlightEnd = aEnd;
      
    iHighlightColor = aHighlightColor;
    iHighlightTextColor = aHighlightTextColor;
    
    SetChanged();
	}

EXPORT_C void CHuiTextVisual::UpdateMeshL(const TDesC8& aBuffer)
    {
    iTextMesh->UpdateMeshL(aBuffer);
    if (!iText)
        {
        SetTextL(KNullDesC);
        }

    SetChanged();
    }
