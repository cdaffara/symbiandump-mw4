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
* Description:   Text visual
*
*/


#include <s32mem.h>
#include "alf/alftextvisual.h"
#include "alf/alfcontrol.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfconstants.h"
#include "alf/alfgencomponent.h"
#include "alflogger.h"
#include "alf/alflayoutmetrics.h"
#include "alf/alftextstyle.h"
#include "alf/alfdropshadow.h"
#include <uiacceltk/HuiFont.h>

#include <AknsConstants.h>
#include <AknBidiTextUtils.h>

#include <utf.h>

#include <aknnotewrappers.h>

#ifdef ALF_RASTER_TEXT

/** Granularity of line wrapping array. */
const TInt KLineArrayGranularity = 4;

NONSHARABLE_CLASS(CAlfRasterizedTextMesh): public CBase
    {
    struct SRasterizedLine
        {
        SRasterizedLine()
            {
            iTexture = NULL;
            iGap = 0;
            }
        /** Texture that holds the rasterized version of a text line. The
            size of the texture determines the extents of the line. */
        CAlfTexture* iTexture;
        
        /** Extra gap to the next line in pixels. Not applied to the last
            line. */
        TInt iGap;
        };
    
    public:
    static CAlfRasterizedTextMesh* NewL(CAlfTextVisual* aVisual)
        {
        CAlfRasterizedTextMesh* self = new (ELeave) CAlfRasterizedTextMesh(aVisual);
        return self;
        }

    CAlfRasterizedTextMesh(CAlfTextVisual* aVisual)
        :iVisual(aVisual){}

    ~CAlfRasterizedTextMesh()
        {
        ResetLines(ETrue);
        iLines.Close();
        ReleaseTextures(ETrue);
        iObsoleteTextures.Close();
        delete iBuf;
        }
    
    void PrepareForRasterize()
        {
        // resolve max width 
        iMaxWidth = KMaxTInt;
        if(iVisual->Wrapping() != CAlfTextVisual::ELineWrapManual)
            {
            TAlfRealRect content = iVisual->DisplayRectTarget();
            content.Shrink(iVisual->PaddingInPixels());
            iMaxWidth = TInt(content.Width()+0.5f);
            }        
        }
    
    TInt Lines()
        {
        return iLines.Count();
        }

    void ResetLines(TBool aDeleteTexture = EFalse)
        {
        for(TInt i = 0; i < iLines.Count(); ++i)
            {
            if ( aDeleteTexture )
                {
                delete iLines[i].iTexture;
                }
            else 
                {
                MarkTextureForRemovalL(iLines[i].iTexture);
                }
            iLines[i].iTexture = NULL;
            }

        iLines.Reset();
        }

    void RasterizeLineL(const TDesC& aTextLine, SRasterizedLine & aLineOut, CAlfTextStyle* aTextStyle)
        {
        // Calculate line extents and assign it to texture size.
        TSize textureSize = aTextStyle->LineExtentsL(aTextLine);
    
        if(textureSize.iWidth == 0)
            {
            // This is an empty string. We will not rasterize it.
            // Just add a gap to rows.
            aLineOut.iTexture = NULL;

            aLineOut.iGap = textureSize.iHeight; // @todo: refacture/rename iGap? iGap is used as a size of an empty line?
            return;
            }

        // Rasterize string using the defined text style.
        aTextStyle->RasterizeLineL(aTextLine, &aLineOut.iTexture);
        }

    void DoBuildL(CAlfTextStyle* aTextStyle)
        {
        ResetLines();
        
        TSize extents(0, 0);
        const TDesC& text = iVisual->Text();
        
        // Retrieve the CFont object used when rasterizing this text mesh.
        CFont* font = aTextStyle->Font()->NearestFontL(1.0);

        // In wrapping mode, let the mesh know how much space there is
        // for drawing into.
        TInt maxWidth = iMaxWidth; //KMaxTInt;
        TInt lineCount = 0;

        // awkward, just to avoid warning
        CArrayFixFlat<TPtrC>* linePtrs = 0;
        HBufC* buf = 0;

        switch(iVisual->Wrapping())
            {
            case CAlfTextVisual::ELineWrapManual:
                {
                lineCount = 1;
                for (TInt i = text.Length()-2;i>=0;i--) // linebreak as last character is ignored
                    {
                    if (text[i]=='\n') // not elegant but keeps the compatibility
                        lineCount++;
                    }
                if (lineCount > 1)
                    {
                    TInt lineStart = 0;
                    TInt breakpos = 0;
                    TInt remaining = text.Length();                    
                    while(lineCount)
                        {
                        for (TInt i = lineStart; i<remaining ;i++)
                            {
                            if (text[i]=='\n') // not elegant but keeps the compatibility
                                {
                                breakpos = i;
                                break;
                                }
                            }
                            if (breakpos < lineStart) // not found
                                {
                                breakpos = remaining-1;
                                }
                                
                            HBufC* buf = text.Mid(lineStart,breakpos-lineStart).AllocLC(); // todo.. is extra space required for bidi
                            lineStart = breakpos+1;
                            TPtr ptr = buf->Des();
                            // truncate line
                            AknBidiTextUtils::ConvertToVisualAndClipL(ptr, *font, maxWidth, maxWidth);
                            // create the line entry if not already existing

                            SRasterizedLine line;
                            line.iTexture = NULL;
                            line.iGap = 0;
                            iLines.AppendL(line);
                        
                            TInt index = iLines.Count()-1;
                            // rasterize a single line (updates texture in iLines[0].iTexture)
                            RasterizeLineL(ptr, iLines[index], aTextStyle);                     

                            // Get extents from the texture we just created
                            CAlfTexture* tex = iLines[index].iTexture;
                            extents.iHeight += iLines[index].iGap;
                            if(tex)
                                {
                                extents.iWidth = Max(extents.iWidth, tex->Size().iWidth);
                                extents.iHeight += tex->Size().iHeight;
                                }                   
                    
                            CleanupStack::PopAndDestroy(buf);
                            lineCount--;
                            }
                        break;
                        }
                    } // fall through with single line
            case CAlfTextVisual::ELineWrapTruncate:
                {
                lineCount = 1; // there's always one line created per logical line
                HBufC* buf = text.AllocLC(); // todo.. is extra space required for bidi
                TPtr ptr = buf->Des();
                // truncate line
                AknBidiTextUtils::ConvertToVisualAndClipL(ptr, *font, maxWidth, maxWidth);
                // create the line entry if not already existing

                SRasterizedLine line;
                line.iTexture = NULL;
                line.iGap = 0;
                iLines.AppendL(line);
                        
 	            // rasterize a single line (updates texture in iLines[0].iTexture)
        	    RasterizeLineL(ptr, iLines[0], aTextStyle);	                    

   	            // Get extents from the texture we just created
                CAlfTexture* tex = iLines[0].iTexture;
                extents.iHeight += iLines[0].iGap;
                if(tex)
                    {
                    extents.iWidth = Max(extents.iWidth, tex->Size().iWidth);
                    extents.iHeight += tex->Size().iHeight;
                    }	                
                	
        	    CleanupStack::PopAndDestroy(buf);
                break;
                }

            case CAlfTextVisual::ELineWrapBreak:
                {
                // wrap lines to array
                linePtrs = new (ELeave) CArrayFixFlat<TPtrC>(KLineArrayGranularity);
                CleanupStack::PushL(linePtrs);
        
                buf = AknBidiTextUtils::ConvertToVisualAndWrapToArrayL(
                    text, maxWidth, *font, *linePtrs);
                CleanupStack::PushL(buf);

              	// Do rasterisation
                for(TInt i = 0; i < linePtrs->Count();i++)
                    {
                    SRasterizedLine line;
                    line.iTexture = NULL;
                    line.iGap = 0;
                    iLines.AppendL(line);
                        
                    // rasterize a single line (updates texture in iLines[i].iTexture)
                    RasterizeLineL(linePtrs->At(i), iLines[i], aTextStyle);
                    CAlfTexture* tex = iLines[i].iTexture;
                    extents.iHeight += iLines[i].iGap;
                            
                    if(tex)
                        {
                        extents.iWidth = Max(extents.iWidth, tex->Size().iWidth);
                        extents.iHeight += tex->Size().iHeight;
                        }    
                            	                                                         
                    if (i == iVisual->MaxLineCount()-1)
          	            {
       	                // Maximum number of lines reached.
       	                break;
       	                }
                    }
                CleanupStack::PopAndDestroy(buf);
                CleanupStack::PopAndDestroy(linePtrs);
                break;
                }
                
            default:
                break;
            }
            
        // Extents needs to be updated in order to make alignment 
        // work properly.
        iExtents = extents;

        UpdateDescriptorL();
        }

    void UpdateDescriptorL()
        {
        delete iBuf;
        iBuf = 0;
        iBuf = HBufC8::NewL((iLines.Count()+1)*8 + 2*4);
        RDesWriteStream stream;
        TPtr8 ptr = iBuf->Des();
        stream.Open(ptr);
        CleanupClosePushL(stream);
        stream.WriteInt32L(iLines.Count());
        for (TInt i = iLines.Count()-1; i >= 0; i--)
            {
            if (iLines[i].iTexture)
                stream.WriteInt32L(iLines[i].iTexture->ServerHandle());
            else
                stream.WriteInt32L(0);                
            stream.WriteInt32L(iLines[i].iGap);
            }
        stream.WriteInt32L(iExtents.iWidth);
        stream.WriteInt32L(iExtents.iHeight);
        stream.CommitL();
        CleanupStack::PopAndDestroy(); // close stream
        }
    
    HBufC8* Buffer()
        {
        return iBuf;    
        }
    
    void MarkTextureForRemovalL(CAlfTexture* aTexture)
        {    
        if (iObsoleteTextures.Find(aTexture)==KErrNotFound)
            {
            iObsoleteTextures.AppendL(aTexture);
            }
        }
        
    void ReleaseTextures(TBool aObsoleteOnly)
        {
        if (!aObsoleteOnly)
            {        
            for (TInt i = iLines.Count()-1;i>=0;i--)
                {
                delete iLines[i].iTexture;
                iLines.Remove(0);
                }
            }        
        iObsoleteTextures.ResetAndDestroy();
        }

    
    CAlfTextVisual* iVisual;
    RArray<SRasterizedLine> iLines;
    RPointerArray<CAlfTexture> iObsoleteTextures;
    HBufC8* iBuf;
    TSize iExtents;
    
    TInt iMaxWidth;
    };

#endif // #ifdef ALF_RASTER_TEXT

struct CAlfTextVisual::TTextVisualPrivateData
    {
    HBufC* iText; // own
    TInt iTextStyleId;
    CAlfTextStyle* iTextStyle;
#ifdef ALF_RASTER_TEXT
    CAlfRasterizedTextMesh* iMesh;
    TInt iWrappingMode;
    TInt iMaxLineCount;
    TInt iLineSpacingUnit;
    TInt iLineSpacing;
    
    TBool iMeshUptoDate;   
#endif // #ifdef ALF_RASTER_TEXT
    };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextVisual::CAlfTextVisual()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextVisual::ConstructL(CAlfControl& aOwner)
    {
    CAlfVisual::ConstructL(aOwner);
    
    iTextVisualData = new (ELeave) TTextVisualPrivateData;
    iTextVisualData->iText = NULL;
    iTextVisualData->iTextStyleId = 0;
#ifdef ALF_RASTER_TEXT
    iTextVisualData->iTextStyle = 0;
    iTextVisualData->iMesh = 0;
    iTextVisualData->iWrappingMode=0;
    iTextVisualData->iMaxLineCount=256;
    iTextVisualData->iLineSpacing=0;
    iTextVisualData->iLineSpacingUnit=0;
    iTextVisualData->iMesh = CAlfRasterizedTextMesh::NewL(this);
    
    iTextVisualData->iMeshUptoDate = EFalse;
    
#endif //#ifdef ALF_RASTER_TEXT
    
    // If alflayoutmetrcis is in automatic mode, mark this visual.
    if (aOwner.Env().LayoutMetricsUtility()->AutoMarking() )
        {
        aOwner.Env().LayoutMetricsUtility()->MarkVisual(NULL, this);    
        }
    }

void CAlfTextVisual::PrepareForUpdateMesh()
    {
#ifdef ALF_RASTER_TEXT
    if(!iTextVisualData->iMeshUptoDate)
        {
        iTextVisualData->iMesh->PrepareForRasterize();
        }
#endif // ALF_RASTER_TEXT    
    }

void CAlfTextVisual::UpdateMesh(TBool aSynch)
    {
 #ifdef ALF_RASTER_TEXT
    // update textures
    CAlfTextStyle* style = iTextVisualData->iTextStyle;
    if (!style)
        { // default
        iTextVisualData->iTextStyle = Env().TextStyleManager().SwitchTextStyle(0,this);//register for updates 
        style = Env().TextStyleManager().TextStyle(0);
        }
    if (aSynch)
        {
        if(!iTextVisualData->iMeshUptoDate)
            {
            TRAPD(err, iTextVisualData->iMesh->DoBuildL(style))
            // Inform server
            if (!err)
                {
                TPtr8 ptr = iTextVisualData->iMesh->Buffer()->Des();
                Comms()->DoCmdNoReply(EAlfTextVisualSetRasterizedMesh, ptr);    
                }    
            iTextVisualData->iMesh->ReleaseTextures(ETrue);
            iTextVisualData->iMeshUptoDate = ETrue;
            }
        }
     else
        {
        iTextVisualData->iMeshUptoDate = EFalse;
        style->ReportChanged();
        }
 #endif
    }
void CAlfTextVisual::ReleaseMesh()
    {
#ifdef ALF_RASTER_TEXT
    iTextVisualData->iMesh->ResetLines(ETrue);
    iTextVisualData->iMeshUptoDate = EFalse;
#endif
    }
	


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextVisual::~CAlfTextVisual()
    {
    if ( iTextVisualData )
        {
        delete iTextVisualData->iText;
#ifdef ALF_RASTER_TEXT
        Env().TextStyleManager().Unregister(this);
        delete iTextVisualData->iMesh;
#endif
        }
    delete iTextVisualData;
    
    Env().LayoutMetricsUtility()->UnmarkVisual(this);
    }


// ---------------------------------------------------------------------------
// Creates new CAlfTextVisual
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextVisual* CAlfTextVisual::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout)
    {
    CAlfTextVisual* text = STATIC_CAST(CAlfTextVisual*,
        aOwnerControl.AppendVisualL(EAlfVisualTypeText, aParentLayout));
    return text;
    }
    
// ---------------------------------------------------------------------------
// Sets text.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextVisual::SetTextL(const TDesC& aText)
    {
#ifdef ALF_RASTER_TEXT
    if (!iTextVisualData->iText || aText.Compare(*iTextVisualData->iText))
        {
		// Store the text into a temp buffer.
        HBufC* newBuffer = aText.AllocL();
        delete iTextVisualData->iText;
        iTextVisualData->iText = newBuffer;
        UpdateMesh();
        }
#else
    // Store the text into a temp buffer.
    HBufC* newBuffer = aText.AllocLC();
    
    TPtrC8 ptr(reinterpret_cast<const TUint8 *>(aText.Ptr()), aText.Length()*2);
    TBuf8<1> dum;
    
    TInt err = Comms()->DoSynchronousCmd(EAlfTextVisualSetText, ptr, dum);
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetTextL leave error %d", err )
        User::Leave( err );    
        }
    
    // Everything's OK. Switch the internal text buffer.
    CleanupStack::Pop( newBuffer );
    delete iTextVisualData->iText;
    iTextVisualData->iText = newBuffer;
#endif
    }
  
// ---------------------------------------------------------------------------
// Gets text.
// ---------------------------------------------------------------------------
//  
EXPORT_C const TDesC& CAlfTextVisual::Text() const
    {
    if ( iTextVisualData && iTextVisualData->iText )
        {
        return *iTextVisualData->iText;
        }
    return KNullDesC();
    }
  
// ---------------------------------------------------------------------------
// Sets style.
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfTextVisual::SetStyle(
        TAlfPreconfiguredTextStyle aStyle, 
        TAlfBackgroundType aBackgroundType )
    {
#ifdef ALF_RASTER_TEXT
    iTextVisualData->iTextStyle = Env().TextStyleManager().SwitchTextStyle(aStyle, this);
    iTextVisualData->iTextStyleId = aBackgroundType; // zero warnings, right
    iTextVisualData->iTextStyleId = aStyle;
    UpdateMesh();
#else
    TInt2 params(aStyle, aBackgroundType);
    TPckgC<TInt2> buf(params);
    
    TInt err = Comms()->DoCmdNoReply(EAlfTextVisualSetStyle, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetStyle ignore error %d", err )
        } 
#endif
    }
    
// ---------------------------------------------------------------------------
// Sets style.
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfTextVisual::SetTextStyle( TInt aTextStyleId )
    {   
#ifdef ALF_RASTER_TEXT
    if(!iTextVisualData->iTextStyle || iTextVisualData->iTextStyleId != aTextStyleId)
        {
        iTextVisualData->iTextStyle = Env().TextStyleManager().SwitchTextStyle(aTextStyleId, this);
        iTextVisualData->iTextStyleId = aTextStyleId;
        UpdateMesh();
        }
#else
    // Convert the text style id from client domain to session domain.
    CAlfTextStyle* textStyle = Env().TextStyleManager().TextStyle(aTextStyleId);
    TPckgC<TInt> buf(textStyle->Comms()->Identifier());
    TBuf8<1> dum;	
	    
    TInt err = Comms()->DoSynchronousCmd(EAlfTextVisualSetTextStyle, buf, dum);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetTextStyle ignore error %d", err )
        }     
    else
        {
        // once we know that it's been set successfully, we can cache the client-side id 
        // on the client-side, for later use
        iTextVisualData->iTextStyleId = aTextStyleId;
        }
#endif // #ifdef ALF_RASTER_TEXT
    }
    
// ---------------------------------------------------------------------------
// Sets alignment.
// ---------------------------------------------------------------------------
//      
EXPORT_C void CAlfTextVisual::SetAlign( TAlfAlignHorizontal aAlignHorizontal,
                                        TAlfAlignVertical aAlignVertical)
    {
    TInt2 params(aAlignHorizontal, aAlignVertical);
    TPckgC<TInt2> buf(params);

    TInt err = Comms()->DoCmdNoReply(EAlfTextVisualSetAlign, buf );

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetAlign ignore error %d", err )
        }     
    }
    
// ---------------------------------------------------------------------------
// Sets the line spacing for multiline text visual when the text wraps around.
// ---------------------------------------------------------------------------
//      
EXPORT_C void CAlfTextVisual::SetLineSpacing(TInt aLineSpacing, 
                                             TLineSpacingUnits aUnits)
    {
#ifdef ALF_RASTER_TEXT
    iTextVisualData->iLineSpacing = aLineSpacing;
    iTextVisualData->iLineSpacingUnit = aUnits;   
    if (iTextVisualData->iMesh->Lines() > 1)
        {
        UpdateMesh();    
        }    
#else
    TInt2 params(aLineSpacing, aUnits);
    TPckgC<TInt2> buf(params);

    TInt err = Comms()->DoCmdNoReply(EAlfTextVisualSetLineSpacing, buf );

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetAlign ignore error %d", err )
        }     
#endif
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C TSize CAlfTextVisual::TextExtents() const
    {
#ifdef ALF_RASTER_TEXT
    TSize size( 0, 0 );    
    if( Text().Length() != 0 )
        {
        TRAP_IGNORE(size = iTextVisualData->iTextStyle->LineExtentsL(Text()))
        }
        
    return size;
#else
    if( Text().Length() == 0 )
        {
        return TSize( 0, 0 );
        }

    TBufC8<1> inDum;
    TSize size;
    TPckg<TSize> outBuf(size);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfTextVisualTextExtents, inDum, outBuf);
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::TextExtents ignore error %d", err )
        } 
        
    return size;
#endif
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C TRect CAlfTextVisual::SubstringExtents(TUint aStart, TUint aEnd) const
    {
#ifdef ALF_RASTER_TEXT
    TSize size( 0, 0 );    
    if( Text().Length() != 0 && aStart < aEnd )
        {
        TRAP_IGNORE(size = iTextVisualData->iTextStyle->LineExtentsL(Text().Mid(aStart, aEnd-aStart)))
        }
        
    return size;
#else
    if( Text().Length() == 0 )
        {
        return TSize( 0, 0 );
        }

    TInt2 params(aStart, aEnd);
    TPckgC<TInt2> inBuf(params);
    TRect size;
    TPckg<TRect> outBuf(size);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfTextVisualSubstringExtents, inBuf, outBuf);
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SubstringExtents ignore error %d", err )
        } 
        
    return size;        
#endif
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C TAlfPreconfiguredTextStyle CAlfTextVisual::Style()
    {
#ifdef ALF_RASTER_TEXT
    return (TAlfPreconfiguredTextStyle)TextStyle();        

#else
    TAlfPreconfiguredTextStyle style = EAlfTextStyleNormal;
    TAlfBackgroundType backgroundType = EAlfBackgroundTypeLight; 
    TInt2 params(style, backgroundType);
    TPckg<TInt2> outBuf(params);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfTextVisualStyle, KNullDesC8(), outBuf);    
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::Style ignore error %d", err )
        } 
        
    return style;        
 #endif
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C TInt CAlfTextVisual::TextStyle() const
    {
    // use the client-side cache, as this id only makes sense on the client side
    return iTextVisualData->iTextStyleId;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C void CAlfTextVisual::SetMaxLineCount(TInt aMaxLineCount)
    {
#ifdef ALF_RASTER_TEXT
    if (aMaxLineCount != iTextVisualData->iMaxLineCount )
        {
        iTextVisualData->iMaxLineCount = aMaxLineCount;
        if (iTextVisualData->iMesh->Lines() > aMaxLineCount)
            {
            UpdateMesh();
            }
        }
 
#else
    TPckgC<TInt> inBuf(aMaxLineCount);    
    TInt err = Comms()->DoCmdNoReply(EAlfTextVisualSetMaxLineCount,  inBuf);     
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetMaxLineCount ignore error %d", err )
        }        
#endif
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C TInt CAlfTextVisual::MaxLineCount() const
    {
#ifdef ALF_RASTER_TEXT
    return iTextVisualData->iMaxLineCount;
#else
    TBufC8<1> inDum;
    TInt value = 0;
    TPckg<TInt> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfTextVisualMaxLineCount, inDum, 
        outBuf);   
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::MaxLineCount ignore error %d", err )
        }     
         
    return value;                
#endif
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      

EXPORT_C CAlfTextVisual::TLineWrap CAlfTextVisual::Wrapping() const
    {
#ifdef ALF_RASTER_TEXT
	return (CAlfTextVisual::TLineWrap)iTextVisualData->iWrappingMode;      
#else
    TBufC8<1> inDum;
    TLineWrap value = ELineWrapTruncate;
    TPckg<TLineWrap> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfTextVisualWrapping, inDum, 
        outBuf);    
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::Wrapping ignore error %d", err )
        }   
        
    return value;                        
#endif
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C void CAlfTextVisual::SetWrapping(CAlfTextVisual::TLineWrap aWrap)
    {
#ifdef ALF_RASTER_TEXT
    if(iTextVisualData->iWrappingMode != (TInt)aWrap)
        {
        iTextVisualData->iWrappingMode = (TInt)aWrap;
        UpdateMesh();
        }
#else
    TPckg<TLineWrap> inBuf(aWrap);    
    TInt err = Comms()->DoCmdNoReply(EAlfTextVisualSetWrapping, inBuf ); 
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetWrapping ignore error %d", err )
        }     
#endif
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C TAlfBackgroundType CAlfTextVisual::BackgroundType()
    {
#ifdef ALF_RASTER_TEXT
    TAlfBackgroundType backgroundType = EAlfBackgroundTypeLight;          
    return backgroundType;   
#else
    TAlfPreconfiguredTextStyle style = EAlfTextStyleNormal;
    TAlfBackgroundType backgroundType = EAlfBackgroundTypeLight; 
    TInt2 params(style, backgroundType);
    TPckg<TInt2> outBuf(params);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfTextVisualStyle, KNullDesC8(), outBuf);    
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::BackgroundType ignore error %d", err )
        }  
         
    return backgroundType;                                
#endif
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C TAlfTimedValue CAlfTextVisual::ShadowOpacity()
    {
    if ( DropShadowHandler() )
        {
        return 1.f; // not the real value...
        }   
        
    return TAlfTimedValue(0);                                        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C void CAlfTextVisual::SetShadowOpacity(const TAlfTimedValue& aShadowOpacity)
    {
    TRAPD( err, EnableDropShadowL() );
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetShadowOpacity ignore error %d", err )
        return;
        }      
        
    DropShadowHandler()->SetOpacity( aShadowOpacity );    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C void CAlfTextVisual::SetColor(TRgb aColor)
    {
    TAlfTextVisualFontColorParams params;
    params.iColor = aColor;
    params.iId = KAknsIIDNone; // This will be ignored in the serverside
    params.iIndex = 0; // This will be ignored in the serverside
    
    TPckg<TAlfTextVisualFontColorParams> inBuf(params);    
    TInt err = Comms()->DoCmdNoReply(EAlfTextVisualSetColor, inBuf);  
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetColor1 ignore error %d", err )
        }            
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C void CAlfTextVisual::SetColor(const TAknsItemID& aId,const TInt aIndex)
    {
    TAlfTextVisualFontColorParams params;
    params.iColor = KRgbBlack; // This will be ignored in the serverside
    params.iId = aId;
    params.iIndex = aIndex;
    
    TPckg<TAlfTextVisualFontColorParams> inBuf(params);    
    TInt err = Comms()->DoCmdNoReply(EAlfTextVisualSetColor, inBuf);  
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetColor1 ignore error %d", err )
        }            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfTextVisual::EnableShadow(TBool aDoEnable)
    {
    TRAPD( err, EnableDropShadowL( aDoEnable ) );
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::EnableShadow ignore error %d", err )
        }      
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfTextVisual::RemoveAndDestroyAllD()
    {
    CAlfVisual::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfTextVisual::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfVisual::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfTextVisual::FindTag(const TDesC8& aTag)
    {
    return CAlfVisual::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfTextVisual::DoRemoveAndDestroyAllD()
    {
    CAlfVisual::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfTextVisual::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfVisual::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
    
// ---------------------------------------------------------------------------
// DEPRECATED! Set Color for Visual using String table and Index
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextVisual::SetColor(const TDesC& /*aTextColorTable*/,const TDesC& /*aColorIndex*/)
    {   	     		   
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfTextVisual::SetOffset(const TAlfTimedPoint& aOffset)
    {
    TPckg<TAlfTimedPoint> inBuf(aOffset);    
    TInt err = Comms()->DoCmdNoReply(EAlfTextVisualSetOffset, inBuf);  
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetOffset ignore error %d", err )
        }                    
    }
     
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TAlfTimedPoint CAlfTextVisual::Offset() const
    {
    TBufC8<1> inDum;
    TAlfTimedPoint value;
    TPckg<TAlfTimedPoint> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfTextVisualOffset, inDum, 
        outBuf);    
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::Offset ignore error %d", err )
        }  
        
    return value;                                                
    }

#ifdef ALF_RASTER_TEXT
// for convenience
void DoLocalHighlightConversionL(TInt aStart, TInt aEnd, HBufC* aText, CAlfTextStyle* aStyle, TSize& aStartPos, TSize& aEndPos)
    {
    aStartPos = aStyle->LineExtentsL(aText->Left(aStart));
    aEndPos = aStyle->LineExtentsL(aText->Left(aEnd));
    }
#endif
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C void CAlfTextVisual::SetHighlightRange(TInt aStart, TInt aEnd, TRgb& aHighlightColor, TRgb& aHighlightTextColor)
    {
#ifdef ALF_RASTER_TEXT
    // ToDO: convert to mesh coordinates    
    CAlfTextStyle* style = iTextVisualData->iTextStyle;
    if (!style || Text().Length() == 0 || // has not been rasterized ever or text lenght is zero
        (aStart < 0) || (aStart > Text().Length()) || (aEnd < 0) || (aStart >= aEnd) || (aEnd > Text().Length()))
                
        {
        return;
        }
    
    TSize startPos, endPos;
    TRAPD(err, DoLocalHighlightConversionL(aStart, aEnd, iTextVisualData->iText, style, startPos, endPos))
    if (!err)
        {    
        TAlfTextVisualSetHighlightRangeParams params;
        params.iStart = startPos.iWidth;
        params.iEnd = endPos.iWidth;
        params.iHighlightColor = aHighlightColor;
        params.iHighlightTextColor = aHighlightTextColor;
            
        TPckg<TAlfTextVisualSetHighlightRangeParams> inBuf(params);    
        err = Comms()->DoCmdNoReply(EAlfTextVisualSetHighlightRange, inBuf);  
            
        if ( err != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfTextVisual::SetHighlightRange ignore error %d", err )
            }            
        }
    
    
#else
    TAlfTextVisualSetHighlightRangeParams params;
    params.iStart = aStart;
    params.iEnd = aEnd;
    params.iHighlightColor = aHighlightColor;
    params.iHighlightTextColor = aHighlightTextColor;
        
    TPckg<TAlfTextVisualSetHighlightRangeParams> inBuf(params);    
    TInt err = Comms()->DoCmdNoReply(EAlfTextVisualSetHighlightRange, inBuf);  
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfTextVisual::SetHighlightRange ignore error %d", err )
        }            
#endif
    }
