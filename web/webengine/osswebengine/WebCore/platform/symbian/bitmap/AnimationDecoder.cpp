/*
* ==============================================================================
*    Copyright (c) 2006, Nokia Corporation
*    All rights reserved.
*
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions
*   are met:
*
*      * Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*      * Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in
*        the documentation and/or other materials provided with the
*        distribution.
*      * Neither the name of the Nokia Corporation nor the names of its
*        contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
*   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
*   USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
*   DAMAGE.
*
* ==============================================================================
*/

/*
 *  This class is a proxy for CAnimationDecoderWrapped
 *  
 *  We needed to wrap animation decoding in a proxy because sometimes the cache gets cleared when decoding
 *  is in progress; when that happens the animation gets deleted while it's in the middle (which causes all sorts
 *  of crashes and memory stomping).  Now, the cache can delete the proxy while the animation is decoding; the proxy
 *  will pass on the delete request to the decoder which will cleanup (delete itself) when it's safe to do so.
 *  
 */

// INCLUDE FILES
#include "config.h"
#include "AnimationDecoder.h"

using namespace WebCore;

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAnimationDecoder::CAnimationDecoder( CAnimationDecoderWrapped* aDecoder )
    : iWrapped(aDecoder)
{
}

// -----------------------------------------------------------------------------
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAnimationDecoder* CAnimationDecoder::NewL( ImageObserver* aObs )
{
    CAnimationDecoderWrapped* aDecoder = CAnimationDecoderWrapped::NewL( aObs );
    CAnimationDecoder* self = new (ELeave) CAnimationDecoder( aDecoder );
    return self;
}

// -----------------------------------------------------------------------------
//
// Symbian constructor can leave.
// -----------------------------------------------------------------------------
void CAnimationDecoder::ConstructL( )
  {
  }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CAnimationDecoder::~CAnimationDecoder()
{
    if (iWrapped) {
        iWrapped->Invalidate();
        iWrapped = NULL;
    }
}

CMaskedBitmap* CAnimationDecoder::Destination() 
{ 
    if (iWrapped)
        return(iWrapped->Destination());
    return(NULL);
}

// -----------------------------------------------------------------------------
// OpenL
// -----------------------------------------------------------------------------
void CAnimationDecoder::OpenL( const TDesC8& aData, TDesC* aMIMEType, TBool aIsComplete )
{
    if (!iWrapped) {
        return;
    }
    TRAPD(err, iWrapped->OpenL(aData, aMIMEType, aIsComplete));
    if (err != KErrNone) {
        delete iWrapped;
        iWrapped = NULL;
    }
    User::LeaveIfError(err);
    
}

TSize CAnimationDecoder::Size() const
{
    if (iWrapped)
        return(iWrapped->Size());
    TSize t;
    return(t);
}

TInt CAnimationDecoder::AnimationFrameCount() const
{
    if (iWrapped)
        return(iWrapped->AnimationFrameCount());
    return(-1);
}

TInt16 CAnimationDecoder::getLoopCount()
{
    if (iWrapped)
        return(iWrapped->getLoopCount());
    return(-1);
}

TBool CAnimationDecoder::isSizeAvailable() const
{
    if (iWrapped)
        return(iWrapped->isSizeAvailable());
    return(EFalse);
}

TInt CAnimationDecoder::LoadFrame(TInt aFrameIndex)
{
    if (iWrapped)
        return(iWrapped->LoadFrame(aFrameIndex));
    return(-1);
}

TBool CAnimationDecoder::decodeInProgress() const
{
    if (iWrapped)
        return(iWrapped->decodeInProgress());
    return(EFalse);
}

//  End of File
