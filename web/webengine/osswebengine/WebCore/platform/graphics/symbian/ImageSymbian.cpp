/*
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ImageSymbian.h"
#include "BitmapImage.h"
#include "FloatRect.h"
#include "ImageObserver.h"
#include "IntRect.h"
#include "PlatformString.h"
#include "Timer.h"
#include <wtf/Vector.h>
#include <wtf/HashMap.h>
#include "GraphicsContext.h"
#include "WebCoreGraphicsContext.h"
#include "bitmap/MaskedBitmap.h"
#include "bitmap/AnimationDecoder.h"
#include "string.h"
#include "StringHash.h"
#include "DebugStream.h"
#include "ZoomFactor.h"

#include <data_caging_path_literals.hrh>

// work around bidi.h conflict
#include <../bidi.h>
#include <AknsConstants.h>
#include <AknsUtils.h>
#include <AknsItemID.h>
#include <AknsConstants.h>
#include <eikenv.h>

#ifdef __GCCE__
#include <WebKitIcons_gcce.mbg>
#else
#include <WebKitIcons.mbg>
#endif

namespace WebCore {

CMaskedBitmap* loadAknIconL(int id, float sizeAdjust);

void FrameData::clear()
{
    if (m_frame) {
        // FIXME: should come up with a better mechanism to handle the lifetime
        // of native bitmaps.  Maybe ref-counting?
        // Caching frames anyway is a bad-idea for Symbian OS, because it consumes
        // a lot of memory for long gif animations.

        delete m_frame;
        m_frame = 0;
        m_duration = 0.;
        m_hasAlpha = true;
    }
}

BitmapImage::BitmapImage(ImageObserver* observer)
    : Image(observer)
    , m_currentFrame(0)
    , m_frames(0)
    , m_frameTimer(0)
    , m_repetitionCount(0)
    , m_repetitionsComplete(0)
    , m_maskedBitmap( 0 )
    , m_isSolidColor(false)
    , m_animatingImageType(true)
    , m_animationFinished(false)
    , m_allDataReceived(false)
    , m_haveSize(false)
    , m_sizeAvailable(false)
    , m_decodedSize(0)
{
    initPlatformData();

    m_frames.clear();
    m_source.setDecoderListener( observer );
}

BitmapImage::BitmapImage(CMaskedBitmap* bitmap)
    : m_currentFrame(0) 
    , m_frameTimer(0)
    , m_repetitionCount(0)
    , m_repetitionsComplete(0)
    , m_maskedBitmap( bitmap )
    , m_imageObserver(0)
    , m_isSolidColor(false)
    , m_animatingImageType(true)
    , m_animationFinished(false)
    , m_allDataReceived(true)
    , m_haveSize(false)
    , m_sizeAvailable(false)
    , m_decodedSize(0)
{
    initPlatformData();

    // setup the image frames
    m_frames.resize(1);
    m_frames[0].m_frame = bitmap;
    m_haveSize = true;
    m_sizeAvailable = true;
    m_size = IntSize(bitmap->SizeInPixels());
}

BitmapImage::~BitmapImage()
{
	//find if m_maskedbitmap is in m_frames so it wont get deleted twice
	for(TInt i=0; i<m_frames.size(); i++)
        {
        if(m_maskedBitmap==m_frames[i].m_frame)
        	{
        	//masked bitmap gets deleted when decoded data is destroyed,
        	m_maskedBitmap = NULL;
        	}
        }
    destroyDecodedData();
    stopAnimation();
	delete m_maskedBitmap;
	m_maskedBitmap = NULL;
    invalidatePlatformData();

    for(TInt i=0; i<m_frames.size(); i++)
        {
        m_frames[i].clear();
        }
    m_frames.clear();
}

void BitmapImage::destroyDecodedData(bool)
{
    // Destroy the cached images and release them, 
    // only cache one image in Symbian port.
    if (m_frames.size()) {
        m_frames.last().clear();
        invalidatePlatformData();
    }
}

IntSize BitmapImage::size() const
{
    if (m_sizeAvailable && !m_haveSize) {
        m_size = m_source.size();
        m_haveSize = true;
    }
    return m_size;
}

void BitmapImage::setMimeType(const String& mimeType)
{
    m_source.setMimeType(mimeType);    
}

const String& BitmapImage::getMimeType()
{
    return m_source.getMimeType();    
}

bool BitmapImage::dataChanged(bool allDataReceived)
{
    destroyDecodedData(true);
    
    // Feed all the data we've seen so far to the image decoder.
    m_allDataReceived = allDataReceived;
        TRAPD(oomErr, m_source.setDataL(m_data.get(), allDataReceived));
    if (oomErr == KErrNoMemory)
     {
       //We are OOM, this should be reported as an error and image load
       // must be stopped.
       return false;
     }
    // Image properties will not be available until the first frame of the file
    // reaches kCGImageStatusIncomplete.
    return isSizeAvailable();
}

size_t BitmapImage::frameCount()
{
    if (m_frames.size()==1 && !m_source.initialized())
        return 1;
    return m_source.frameCount();
}

bool BitmapImage::isSizeAvailable()
{
    if (m_sizeAvailable)
        return true;

    m_sizeAvailable = m_source.isSizeAvailable();

    return m_sizeAvailable;

}

// don't cache the frame
NativeImagePtr BitmapImage::frameAtIndex(size_t index)
{
    if (index >= frameCount())
        return 0;

    if (index < m_frames.size())
        return m_frames[index].m_frame;

    return m_source.createFrameAtIndex(index);
}

float BitmapImage::frameDurationAtIndex(size_t index)
{
    if (index >= frameCount())
        return 0;

    // in millseconds
    return m_source.frameDurationAtIndex(index);
}

bool BitmapImage::frameHasAlphaAtIndex(size_t index)
{
    if (index >= frameCount())
        return 0;

    return m_source.frameHasAlphaAtIndex(index);
}

bool BitmapImage::shouldAnimate()
{
    return (m_animatingImageType && frameCount() > 1 && !m_animationFinished && m_imageObserver);
}

void BitmapImage::startAnimation()
{
    if (m_frameTimer || !shouldAnimate() || frameCount() <= 1)
        return;

    m_frameTimer = new Timer<BitmapImage>(this, &BitmapImage::advanceAnimation);
    m_frameTimer->startOneShot(frameDurationAtIndex(m_currentFrame));
}

void BitmapImage::stopAnimation()
{
    // This timer is used to animate all occurrences of this BitmapImage.  Don't invalidate
    // the timer unless all renderers have stopped drawing.
    delete m_frameTimer;
    m_frameTimer = 0;
}

void BitmapImage::resetAnimation()
{
    stopAnimation();
    m_currentFrame = 0;
    m_repetitionsComplete = 0;
    m_animationFinished = false;
}

void BitmapImage::advanceAnimation(Timer<BitmapImage>* timer)
{
    // Stop the animation.
    stopAnimation();

    // See if anyone is still paying attention to this animation.  If not, we don't
    // advance and will simply pause the animation.
    if (imageObserver() && imageObserver()->shouldPauseAnimation(this))
        return;

    m_currentFrame++;
	m_repetitionCount =  m_source.decoder()->getLoopCount(); 
    if(m_repetitionCount == -1) m_repetitionCount++;
    if (m_currentFrame >= frameCount()) {
        m_repetitionsComplete += 1;
        if (m_repetitionCount && m_repetitionsComplete >= m_repetitionCount) {
            m_animationFinished = false;
            m_currentFrame--;
            return;
        }
        m_currentFrame = 0;
    }

    m_source.decoder()->LoadFrame( m_currentFrame );
}

void BitmapImage::initPlatformData()
{
}

void BitmapImage::invalidatePlatformData()
{
    // tot fixme: we need to flash the image data here.
}

struct IconData {
    int m_id;
    float m_sizeAdjust;
};

const IconData missingImageData = { EMbmWebkiticonsQgn_indi_wml_image_not_shown, 1.2 };
const IconData missingObjectData = { EMbmWebkiticonsQgn_prop_wml_object, 1.2 };
const IconData selectArrowData = { EMbmWebkiticonsQgn_prop_wml_selectarrow, 0.9 };
const IconData selectFileData = { EMbmWebkiticonsQgn_prop_wml_selectfile, 0.9 };
const IconData radioButtonOnData = { EMbmWebkiticonsQgn_prop_wml_radiobutt_on, 0.9 };
const IconData radioButtonOffData = { EMbmWebkiticonsQgn_prop_wml_radiobutt_off, 0.9 };
const IconData checkBoxOnData = { EMbmWebkiticonsQgn_prop_wml_checkbox_on, 0.9 };
const IconData checkBoxOffData = { EMbmWebkiticonsQgn_prop_wml_checkbox_off, 0.9 };

Image* Image::loadPlatformResource(const char *name)
{
    static HashMap<String,const IconData*> resourceIds;
    if (resourceIds.isEmpty()) {
        resourceIds.set("missingImage", &missingImageData);
        resourceIds.set("missingObject", &missingObjectData );
        resourceIds.set("selectArrow", &selectArrowData);
        resourceIds.set("selectFile", &selectFileData);
        resourceIds.set("radioButtonOn", &radioButtonOnData);
        resourceIds.set("radioButtonOff", &radioButtonOffData);
        resourceIds.set("checkBoxOn", &checkBoxOnData);
        resourceIds.set("checkBoxOff", &checkBoxOffData);
    }
    if (const IconData* data = resourceIds.get(name)) {
	CMaskedBitmap* bm = NULL;
        TRAP_IGNORE(bm = loadAknIconL(data->m_id,data->m_sizeAdjust));
        if (bm) {
            BitmapImage* im = new BitmapImage(bm);
            return im;
        }
    }
    return 0;
}

void BitmapImage::draw(GraphicsContext* ctxt, const FloatRect& dst, const FloatRect& src, CompositeOperator op)
{
    WebCoreGraphicsContext* context = ctxt->platformContext();

	if (!context) {
          return;
	}

    CMaskedBitmap* bm = frameAtIndex(m_currentFrame);
    if (!bm) // If it's too early we won't have an BitmapImage yet.
        return;

    TRect target(enclosingIntRect(dst));
    TRect source(enclosingIntRect(src));

    bm->DrawBitmap(context->gc(), xForm(target), source);

    startAnimation();

}

#ifdef __GCCE__
_LIT( KBrowserSvgFile, "Webkiticons_gcce.mif" );
#else
_LIT( KBrowserSvgFile, "webkiticons.mif" );
#endif

static HBufC* iconFileNameBuf = NULL;

void cleanupIconFileName() {
        delete iconFileNameBuf;
        iconFileNameBuf = NULL;
}

TPtrC iconFileName()
{
    if (!iconFileNameBuf) {
        TFileName mbmDrive;
        TParse parse;
        Dll::FileName( mbmDrive );
        Dll::FileName( mbmDrive );
        parse.Set( mbmDrive, NULL, NULL );
        mbmDrive = parse.Drive();
        iconFileNameBuf = HBufC::New( KMaxFileName );
        iconFileNameBuf->Des().Append( mbmDrive );
        iconFileNameBuf->Des().Append( KDC_APP_BITMAP_DIR );
        iconFileNameBuf->Des().Append( KBrowserSvgFile );
    }
    return *iconFileNameBuf;
}

CMaskedBitmap* loadAknIconL(int id, float sizeAdjust)
{
    CEikonEnv* eikEnv = CEikonEnv::Static();
    CWsScreenDevice& screenDev = *eikEnv->ScreenDevice();
    CMaskedBitmap* result = NULL;

    int dpi = screenDev.VerticalTwipsToPixels(KTwipsPerInch);
    int px = (int)10*sizeAdjust;
    // dpi adjustments
    if ((200 > dpi) && (dpi >= 120))
        px *= 1.7;
    else if ((210 > dpi) && (dpi >= 200))
        px *= 2.1;
    else if ((400 > dpi) && (dpi >= 210))
        px *= 2.2;
    else if (dpi >= 400)
        px *= 4.2;

    CFbsBitmap* bitmap=0;
    CFbsBitmap* mask=0;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    AknsUtils::CreateIconLC( skin,
                                   KAknsIIDDefault,
                                   bitmap,
                                   mask,
                                   iconFileName(),
                                   id,
                                   id+1);

    AknIconUtils::SetSize(bitmap,TSize(px,px)); //rect.Rect().Size());
    AknIconUtils::SetSize(mask,TSize(px,px)); //rect.Rect().Size());
	result = new (ELeave) CMaskedBitmap(bitmap,mask);
	CleanupStack::Pop( 2 );//bitmap, mask
    return result;
}

CMaskedBitmap* BitmapImage::getMaskedBitmap()
{
    if (m_maskedBitmap)
        return m_maskedBitmap;

    return  m_source.createFrameAtIndex(0);
}

void BitmapImage::drawPattern(GraphicsContext* ctxt, const FloatRect& srcRect, const AffineTransform& patternTransform,
                             const FloatPoint& phase, CompositeOperator, const FloatRect& dstRect)
{
    WebCoreGraphicsContext* context = ctxt->platformContext();
    
    CMaskedBitmap* bm = frameAtIndex(m_currentFrame);
    if(!bm || bm->IsFullyTransparent()) 
        return;

    IntSize intrinsicImageSize = size();
    TSize scaledSize( xForm(TSize(srcRect.width(), srcRect.height())) );
    
    scaledSize.iWidth = scaledSize.iWidth < 1 ? 1 : scaledSize.iWidth;
    scaledSize.iHeight = scaledSize.iHeight < 1 ? 1 : scaledSize.iHeight;

    if( scaledSize != TSize(intrinsicImageSize) ) {
        bm = bm->ScaleImageToSize(scaledSize);
    }

    if ( bm->SizeInPixels() == TSize(1,1) && !bm->HasMask() ) {
        checkForSolidColor();
    }
        
    CFbsBitmap* dstBmp = context->contentBuffer();

    if (dstBmp != NULL) {
        TSize sz = dstBmp->SizeInPixels();

        // calculate target rect
        TRect trgRect( xForm(dstRect) );
        TRect clip(trgRect);
        if (context->hasClipping())
            {
            clip.Intersection(context->clippingRect());
            }

        // start point in source image
        TPoint point( xForm(TPoint(phase.x(), phase.y())) );
        TPoint off( point.iX%scaledSize.iWidth, point.iY%scaledSize.iHeight );
        off = TPoint( off.iX < 0 ? off.iX + scaledSize.iWidth : off.iX, 
                off.iY < 0 ? off.iY + scaledSize.iHeight : off.iY );
    
        // transform coordinates to bitmap space
        TRect bmpRect( clip );
        TPoint orig( context->origin() );

        TWebCoreSavedContext savedContext = context->save();
        context->setClippingRect(bmpRect);
        bm->TileInBitmapRect(context->gc(), bmpRect, point);
        context->restore(savedContext);

        /*bmpRect.Move( orig );
        if( bmpRect.Intersects( TRect( TPoint(0,0), sz ) ) ) {
            // offscreen bitmap space to aRect space
            TPoint so( bmpRect.iTl );
            so -= (trgRect.iTl + orig);
            so += off;

            // tiling the image
            //bm->TileInBitmapRect( dstBmp, bmpRect, so );
            bm->TileInBitmapRect(context->gc(), bmpRect, point);
        }*/
    }
}

void Image::drawPattern(GraphicsContext* ctxt, const FloatRect& tileRect, const AffineTransform& patternTransform,
                        const FloatPoint& phase, CompositeOperator op, const FloatRect& destRect)
{
    // this function is not really needed for Symbian
    notImplemented();
}

void BitmapImage::checkForSolidColor()
{
    if ( m_source.decoder()->decodeInProgress() || m_isSolidColor ) return;
    
    CMaskedBitmap* bm = frameAtIndex(m_currentFrame);
    
    if(!bm) 
        return;
    
    if ( frameCount() == 1 ) {
        TRgb bgColor;
        bm->Bitmap().GetPixel(bgColor, bm->SizeInPixels().AsPoint() );
        m_isSolidColor = true;
        m_solidColor = Color(bgColor.Red(), bgColor.Green(), bgColor.Blue(), bgColor.Alpha());
    }
}

}
