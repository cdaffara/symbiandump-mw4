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



#include "HuiFxEffectParser.h"

#include <stdarg.h>
#include <gmxmldocument.h>
#include <gmxmlelement.h>
#include <gmxmlnode.h>
#include <gmxmltext.h>
#include <gmxmlprocessinginstruction.h>
#include <gmxmlcdatasection.h>
#include "HuiFxConstants.h"
#include "HuiFxFilter.h"
#include "HuiFxGroupLayer.h"
#include "HuiFxFilterLayer.h"
#include "HuiFxVisualLayer.h"
#include "HuiFxEffect.h"
#include "HuiFxParameter.h"
#include "HuiFxScalarParameter.h"
#include "HuiFxColorParameter.h"
#include "uiacceltk/HuiVisual.h"  // Class definition
#include "alflogger.h"
#include "huieffectable.h"
#include <stdlib.h>

//#define _HUI_FX_PARSER_LOGGING // Enable this, if you need to debug the xml parsing. otherwise leave this out, because the logging breaks the timing of effects
// Debugging macros
#if !defined(NDEBUG)
#define FAIL(CODE, MSG)         FailL(CODE, MSG)
#define FAIL1(CODE, MSG, P1)    FailL(CODE, MSG, P1)
#else
#define FAIL(CODE, MSG)
#define FAIL1(CODE, MSG, P1)
#endif

CHuiFxEffectParser* CHuiFxEffectParser::NewL(CHuiFxEngine& aEngine, MHuiEffectable* aVisual)
    {
    CHuiFxEffectParser* self = NewLC(aEngine, aVisual);
    CleanupStack::Pop(self);
    return self;
    }

CHuiFxEffectParser* CHuiFxEffectParser::NewLC(CHuiFxEngine& aEngine, MHuiEffectable* aVisual)
    {
    CHuiFxEffectParser* self = new (ELeave) CHuiFxEffectParser( aEngine, aVisual );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
void CHuiFxEffectParser::ConstructL()
    {
    iParser = CMDXMLParser::NewL( this ) ;
    User::LeaveIfError( iFs.Connect() );
    CActiveScheduler::Add( this );
    }
    
CHuiFxEffectParser::CHuiFxEffectParser( CHuiFxEngine& aEngine, MHuiEffectable* aVisual ) :
    CActive( EPriorityHigh ),
    iEngine( aEngine ),
    iVisual( aVisual ),
    iParsingEndedObserver( NULL ), iParsingEndedHandle(0)
    {
    __ALFFXLOGSTRING2("CHuiFxEffectParser::CHuiFxEffectParser - this: 0x%x aVisual 0x%x", this, aVisual);
    }
    
CHuiFxEffectParser::~CHuiFxEffectParser()
    {
    Cancel();
    
    delete iParser;
    iParser = NULL;
    
    delete iDoc;
    iDoc = NULL;
    
    delete iEffect;
    iEffect = NULL;
    
    iFs.Close();
    __ALFFXLOGSTRING1("CHuiFxEffectParser::~CHuiFxEffectParser - 0x%x ", this);
}
    
void CHuiFxEffectParser::ParseFileCompleteL()
    {
    TRequestStatus* status = &iStatus;
    TInt err = KErrNone; // We don't get any error from the caller.
    if ( iIsCancelled )
       {
       err = KErrCancel;
       }
    User::RequestComplete( status, err );
    __ALFFXLOGSTRING1("CHuiFxEffectParser::ParseFileCompleteL - 0x%x",this);
    }

void CHuiFxEffectParser::RunL()
    {
    __ALFFXLOGSTRING2("CHuiFxEffectParser::RunL - this 0x%x, status: %d", this, iStatus.Int());
    // RunL will only be called once after the parsing completes
    if ( iStatus.Int() == KErrNone )
        {
        BuildEffectL();
        iEffect->SetEffectFlags( iFlags );
        iFlags = 0;
        // As soon as the visual gets the effect, it kills this effect parser.
        // Therefore the effect must be stored into a temporary variable 
        // and the member of the parser set to null to prevent the fresly released visual
        // from being destroyed immediately.
        // CHuiVisual::SetEffect does dot leave, it just stores the pointer to its own data area,
        // so this is a safe trick, and the boat does not leak.
        if (iParsingEndedObserver)
            {
            iParsingEndedObserver->ParsingEnded(iParsingEndedHandle);
            }

#ifndef HUIFX_EFFECTCACHE_ENABLED
        CHuiFxEffect* tempEffect = iEffect;
#endif
        iEffect = NULL;
        if (iVisual)
            {
            iVisual->EffectSetOpacity(1.0f);
#ifndef HUIFX_EFFECTCACHE_ENABLED
            iVisual->SetEffect( tempEffect );
#endif
            }
        }
    else if ( iStatus.Int() != KErrCancel )
        {
        // some error during parsing - delete possible incomplete effect
        // and set the effect in the visual to NULL as we did not succeed in creating
        // a decent effect
        if (iParsingEndedObserver)
            {
            iParsingEndedObserver->ParsingEnded(iParsingEndedHandle);
            }
        delete iEffect;
        iEffect = NULL;
        if (iVisual)
            {
            iVisual->EffectSetOpacity(1.0f);
            iVisual->EffectSetEffect( NULL );
            }
        }
    else // We got cancelled
        {
        delete iEffect;
        iEffect = NULL;
        }    
    delete this;
    // MUST NOT DO ANYTHING AFTER THE delete this -line!
    }

TInt CHuiFxEffectParser::RunError( TInt /*aError*/ )
    {
    // we signal that we handled the error 
    delete iEffect;
    iEffect = NULL;
    if ( iVisual )
        {
        iVisual->EffectSetEffect( NULL );
        }
    return KErrNone;
    }

void CHuiFxEffectParser::DoCancel()
    {
    // cancel the parsing.
    // 
    iIsCancelled = ETrue;
    iParser->Cancel();
    // parser should now call our ParseFileCompleteL()
    // Unfortunately it will not tell us any error code.
    // Therefore we must remember that we were cancelled.
    // We can only be cancelled when CHuiFxEffectParser is deleted.
    // We do not call iVisual->SetEffect any more in that case 
    // because in that case the parser gets deleted anyway.
    // We don't want to be deleted twice - though setting the parser to NULL after
    // deleting should take care of that anyway.
    
    // Actually it seems that parser will not necessarily call us,
    // so we must ensure ourselves that we are completed (but only once!)
    // Otherwise then whole system will hang waiting for an event
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrCancel );
    __ALFFXLOGSTRING1("CHuiFxEffectParser::DoCancel - 0x%x",this);
    }

void CHuiFxEffectParser::ParseL( const TDesC& aFile, TRect aExtRect )
    {
    __ALFFXLOGSTRING2("CHuiFxEffectParser::ParseL - 0x%x %S",this, &aFile);
    iIsCancelled = EFalse;
    iExtRect = aExtRect;
    TInt err = iParser->ParseFile( iFs, aFile );
    if ( err != KErrNone )
        {
        // If parsing fails, give up immediately
        iVisual->EffectSetEffect( NULL );
        User::Leave( err );
        }
    iStatus = KRequestPending;
    SetActive();
    // BuildEffectL is called in RunL when parser calls our callback which completes us.
    __ALFFXLOGSTRING("CHuiFxEffectParser::ParseL - activated ");
    }

void CHuiFxEffectParser::Effect( CHuiFxEffect*& aEffect )
    {
    aEffect = iEffect;
    iEffect = NULL;
    }

void CHuiFxEffectParser::SetEffectEndObserver( MAlfGfxEffectObserver* aEffectEndObserver, TInt aHandle )
    {
    iEffectEndObserver = aEffectEndObserver;
    iHandle = aHandle;
    }

void CHuiFxEffectParser::SetParsingEndedObserver( MHuiFxParsingEndedObserver *aObserver, TInt aHandle )
    {
    iParsingEndedObserver = aObserver;
    iParsingEndedHandle = aHandle;
    }


void CHuiFxEffectParser::SetEffectFlags( TInt aFlags )
    {
    iFlags = aFlags;
    }
    
void CHuiFxEffectParser::BuildEffectL()
    {
    __ALFFXLOGSTRING1("CHuiFxEffectParser::BuildEffectL - 0x%x",this);    
    iEffect = CHuiFxEffect::NewL( iEngine );
    iEffect->SetEffectEndObserver( iEffectEndObserver, iHandle );
    iEffectEndObserver = NULL;
    iHandle = 0;
    iDoc = iParser->DetachXMLDoc();
    User::LeaveIfNull( iDoc );
    CMDXMLElement* root = iDoc->DocumentElement();
    User::LeaveIfNull( root );
    CMDXMLNode* node = root->FirstChild();
    if( node->NodeName().Compare( KLitLayers ) != 0 )
        {
        FAIL(KErrGeneral, _L("Layer list not found"));
        }
    for (node = node->FirstChild(); node; node = node->NextSibling())
        {
        ParseNodeL( node, NULL );
        }
    __ALFFXLOGSTRING("CHuiFxEffectParser::BuildEffectL - complete ");
    }
    
void CHuiFxEffectParser::ParseNodeL( CMDXMLNode* aNode, CHuiFxLayer* aLayer)
    {
    switch( ResolveNode( aNode ) )
        {
        case ENodeTypeUnknown:
            {
#ifdef _HUI_FX_PARSER_LOGGING
            __ALFFXLOGSTRING("CHuiFxEffectParser::ParseNodeL - ENodeTypeUnknown");
#endif
            __ALFFXLOGSTRING1("CHuiFxEffectParser::ParseNodeL : WARNING - Node tag <%S> is not supported.", &(aNode->NodeName()));
            break;
            }
            
        case ENodeTypeGroup:
            {
#ifdef _HUI_FX_PARSER_LOGGING
            __ALFFXLOGSTRING("CHuiFxEffectParser::ParseNodeL - ENodeTypeGroup");
#endif
            // can't have group layers inside group layer
            if (aLayer && aLayer->Type() == ELayerTypeGroup)
                {
                FAIL(KErrGeneral, _L("Nested group layers not supported"));
                }
            
            CHuiFxGroupLayer* group = CHuiFxGroupLayer::NewL();
            CleanupStack::PushL( group );
            for (CMDXMLNode* node = aNode->FirstChild(); node; node = node->NextSibling())
                {
                ParseNodeL( node, group );
                }
            iEffect->AddLayerL( group ); // ownership transferred
            CleanupStack::Pop( group );
            break;
            }
            
        case ENodeTypeFilter:
            {
#ifdef _HUI_FX_PARSER_LOGGING
            __ALFFXLOGSTRING("CHuiFxEffectParser::ParseNodeL - ENodeTypeFilter");
#endif
            THuiFxFilterType filterType = GetFilterTypeL( aNode );
            CHuiFxFilter* filter = iEngine.CreateFilterL( filterType );
            CleanupStack::PushL( filter );
            CHuiFxFilterLayer* filterLayer = CHuiFxFilterLayer::NewL( filter );
            CleanupStack::Pop( filter );
            CleanupStack::PushL( filterLayer );
            if (aLayer && aLayer->Type() == ELayerTypeGroup)
                {
                CHuiFxGroupLayer* group = reinterpret_cast<CHuiFxGroupLayer*>(aLayer);
                group->AddLayerL( filterLayer ); // ownership transferred
                }
            else
                {
                iEffect->AddLayerL( filterLayer ); // ownership transferred
                }
            CleanupStack::Pop( filterLayer );
            
            if (filterType == EFilterTypeTransform)
                {
                filterLayer->SetTransformed(ETrue);
                }
            
            for (CMDXMLNode* node = aNode->FirstChild(); node; node = node->NextSibling())
                {
                ParseNodeL( node, filterLayer );
                }
            break;
            }
            
        case ENodeTypeVisual:
            {
#ifdef _HUI_FX_PARSER_LOGGING
            __ALFFXLOGSTRING("CHuiFxEffectParser::ParseNodeL - ENodeTypeVisual");
#endif
            TPtrC16 extBitmap;
            THuiFxVisualSrcType srcType = GetSrcTypeL( aNode, extBitmap );
            TBool opaqueHint = GetOpaqueHintL(aNode);
            CHuiFxVisualLayer* visual = CHuiFxVisualLayer::NewL( iVisual );
            CleanupStack::PushL( visual );
            visual->SetSourceType( srcType );
            visual->SetFxmlUsesOpaqueHint( opaqueHint );
            if ( srcType == EVisualSrcBitmap && extBitmap.Length() > 0 )
                {
                visual->SetExtBitmapFileL( extBitmap );
                }
            if (aLayer && aLayer->Type() == ELayerTypeGroup)
                {
                CHuiFxGroupLayer* group = reinterpret_cast<CHuiFxGroupLayer*>(aLayer);
                group->AddLayerL( visual ); // ownership transferred
                }
            else
                {
                iEffect->AddLayerL( visual ); // ownership transferred
                }
            for (CMDXMLNode* node = aNode->FirstChild(); node; node = node->NextSibling())
                {
                ParseNodeL( node, visual );
                }
            CleanupStack::Pop( visual );
            break;
            }
         
         case ENodeTypeBlending:
            {
#ifdef _HUI_FX_PARSER_LOGGING
            __ALFFXLOGSTRING("CHuiFxEffectParser::ParseNodeL - ENodeTypeBlending");
#endif
            CMDXMLNode* blendingNode = aNode->FirstChild();
            if( blendingNode->NodeType() != CMDXMLNode::ETextNode )
                {
                FAIL(KErrGeneral, _L("Bad blending mode"));
                }
            TPtrC modePtr( ((CMDXMLText *)blendingNode)->Data() );
            
            // set replace as default blending mode
            THuiFxBlendingMode blendingMode = EBlendingModeReplace;
            if( modePtr.Compare( KLitReplace ) == 0 )
                {
                blendingMode = EBlendingModeReplace;
                }
            else if( modePtr.Compare( KLitOver ) == 0 )
                {
                blendingMode = EBlendingModeOver;
                }
            else if( modePtr.Compare( KLitMultiply ) == 0 )
                {
                blendingMode = EBlendingModeMultiply;
                }
            else if( modePtr.Compare( KLitAdditive ) == 0 )
                {
                blendingMode = EBlendingModeAdditive;
                }
            else if( modePtr.Compare( KLitDarken ) == 0 )
                {
                blendingMode = EBlendingModeDarken;
                }
            else if( modePtr.Compare( KLitLighten ) == 0 )
                {
                blendingMode = EBlendingModeLighten;
                }
            else
                {
                FAIL(KErrGeneral, _L("Bad blending mode"));
                }
                
            if (aLayer && aLayer->Type() == ELayerTypeGroup)
                {
                aLayer->SetBlendingMode( blendingMode );
                }
            else
                {
                // TODO: should other layers have a blending mode?
                FAIL(KErrGeneral, _L("Blending mode can only be set for a group layer"));
                }            
            break;
            }
            
         case  ENodeTypeParam:
            {
#ifdef _HUI_FX_PARSER_LOGGING
            __ALFFXLOGSTRING("CHuiFxEffectParser::ParseNodeL - ENodeTypeParam");
#endif
            ResolveParamL(aNode, aLayer);
            break;
            }
            
        default:
            __ALFFXLOGSTRING("CHuiFxEffectParser::ParseNodeL - default??");
            break;
        }
    
    }
    
CHuiFxEffectParser::TNodeType CHuiFxEffectParser::ResolveNode( CMDXMLNode* aNode )
    {    
    TPtrC nodeName( aNode->NodeName() );
    if( nodeName.Compare( KLitLayerGroup ) == 0 )
        {
        return ENodeTypeGroup;
        }
    else if( nodeName.Compare( KLitFilter ) == 0 )
        {
        return ENodeTypeFilter;
        }
    else if( nodeName.Compare( KLitVisual ) == 0 )
        {
        return ENodeTypeVisual;
        }
    else if( nodeName.Compare( KLitBlending ) == 0 )
        {
        return ENodeTypeBlending;
        }
    else if( nodeName.Compare( KLitParam ) == 0 )
        {
        return ENodeTypeParam;
        }
    else if( nodeName.Compare( KLitStyle ) == 0 )
        {
        return ENodeTypeStyle;
        }
    else if( nodeName.Compare( KLitDuration ) == 0 )
        {
        return ENodeTypeDuration;
        }
    else if( nodeName.Compare( KLitKeyFrame ) == 0 )
        {
        return ENodeTypeKeyFrame;
        }
    else if( nodeName.Compare( KLitMarker ) == 0 )
        {
        return ENodeTypeMarker;
        }
    else if( nodeName.Compare( KLitStart ) == 0 )
        {
        return ENodeTypeStart;
        }
    else if( nodeName.Compare( KLitEnd ) == 0 )
        {
        return ENodeTypeEnd;
        }
    else
        {
        return ENodeTypeUnknown;
        }
    }

// not locale dependent float parser
static TReal32 ParseFloatVal(TPtrC valuePtr)
    {
    HBufC8 *buf = HBufC8::New(valuePtr.Length()+1);
    if (!buf) return 0.0;
    CleanupStack::PushL(buf);
    buf->Des().Copy(valuePtr);
    
    TUint8 *buf2 = new TUint8[valuePtr.Length()+1];
    if (!buf2) return 0.0;
    int s = valuePtr.Length();
    int i = 0;
    for(;i<s;i++)
        buf2[i] = buf->operator[](i);
    buf2[i] = 0;
    
    TReal32 v = atof((const char*)buf2);
    delete [] buf2;
    CleanupStack::PopAndDestroy(buf);
    return v;
    }

TReal32 CHuiFxEffectParser::ParseFloatAttributeL(CMDXMLNode* aNode, const TDesC& aName, TBool aMustExist)
    {
    TInt index = ((CMDXMLElement*)aNode)->FindIndex(aName);
    if( index == KErrNotFound )
        {
        if (aMustExist)
            {
            FAIL1(KErrGeneral, _L("Attribute not found: %S"), &aName); // leaves 
            return 0.0f; // never reached. For completeness
            }
        else
            {
            return 0.0f;
            }
        }

    TPtrC attributeValue;
    TPtrC attributeName;
    TReal32 value = 0.0f;
    User::LeaveIfError(((CMDXMLElement*)aNode)->AttributeDetails( index, attributeName, attributeValue ));

    value = ParseFloatVal(attributeValue);
    return value;
    }

TReal32 CHuiFxEffectParser::ParseFloatValueL(CMDXMLNode* aNode)
    {
    if ( !aNode || aNode->NodeType() != CMDXMLNode::ETextNode )
        {
        FAIL(KErrGeneral, _L("Text node expected while parsing a floating point value"));
        }
    
    TPtrC valuePtr( ((CMDXMLText *)aNode)->Data() );
    TReal32 v = ParseFloatVal(valuePtr);
    return v;
    }

TRgb CHuiFxEffectParser::ParseColorValueL(CMDXMLNode* aNode)
    {
    // Parse color definition in HTML notation (#aarrggbb)
    if ( !aNode || aNode->NodeType() != CMDXMLNode::ETextNode )
        {
        FAIL(KErrGeneral, _L("Text node expected while parsing a color value")); // leaves
        return KRgbBlack; // never reached. For completeness
        }
    
    TPtrC valuePtr( ((CMDXMLText *)aNode)->Data() );
    TLex parser(valuePtr);

    if (parser.Get() != '#')
            {
            FAIL(KErrGeneral, _L("Invalid color specification")); // leaves
            return KRgbBlack; // never reached. For completeness
            }

    TUint32 color;
    parser.Val(color, EHex);
    // Opaque alpha by default
    if (valuePtr.Length() <= 7)
        {
        color |= 0xff000000;
        }
    
    TUint32 r = (color & 0x00ff0000) >> 16;
    TUint32 g = (color & 0x0000ff00) >> 8;
    TUint32 b = (color & 0x000000ff);
    TUint32 a = (color & 0xff000000) >> 24;
    
    return TRgb(r, g, b, a);
    }

template <>
TReal32 CHuiFxEffectParser::ParseAnimationKeyFrameValueL(CMDXMLNode* aNode, RHuiFxScalarTimeLine& /*aTimeLine*/)
    {
    return ParseFloatValueL(aNode);
    }

template <>
TRgb CHuiFxEffectParser::ParseAnimationKeyFrameValueL(CMDXMLNode* aNode, RHuiFxColorTimeLine& /*aTimeLine*/)
    {
    return ParseColorValueL(aNode);
    }

template <typename PARAM_TYPE, typename TIMELINE_TYPE, typename VALUE_TYPE>
void CHuiFxEffectParser::ParseAnimationNodeL(CMDXMLNode* aNode, PARAM_TYPE& aParam, TIMELINE_TYPE& aTimeLine)
    {
#ifdef _HUI_FX_PARSER_LOGGING
    __ALFFXLOGSTRING1("CHuiFxEffectParser::ParseAnimationNodeL - 0x%x", this);
#endif
    switch( ResolveNode( aNode ) )
        {
        case ENodeTypeDuration:
            {
            aTimeLine.SetDuration(ParseFloatValueL(aNode->FirstChild()));
            }
            break;
        case ENodeTypeStyle:
            {
            CMDXMLNode* styleNode = aNode->FirstChild();
            if (styleNode->NodeType() != CMDXMLNode::ETextNode)
                {
                FAIL(KErrGeneral, _L("Text node expected while parsing an interpolation mode"));
                }
            TPtrC modePtr( ((CMDXMLText *)styleNode)->Data() );
            
            if( modePtr.Compare( KLitHold ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeHold);
                }
            else if( modePtr.Compare( KLitLinear ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeLinear);
                }
            else if( modePtr.Compare( KLitInQuad ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeInQuad);
                }
            else if( modePtr.Compare( KLitOutQuad ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeOutQuad);
                }
            else if( modePtr.Compare( KLitInOutQuad ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeInOutQuad);
                }
            else if( modePtr.Compare( KLitOutInQuad ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeOutInQuad);
                }
            else if( modePtr.Compare( KLitInBack ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeInBack);
                }
            else if( modePtr.Compare( KLitOutBack ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeOutBack);
                }
            else if( modePtr.Compare( KLitInOutBack ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeInOutBack);
                }
            else if( modePtr.Compare( KLitOutInBack ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeOutInBack);
                }
            else if( modePtr.Compare( KLitQuadraticBezier ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeQuadraticBezier);
                }
            else if( modePtr.Compare( KLitCubicBezier ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeCubicBezier);
                }
/*                
            else if( modePtr.Compare( KLitDeclerate ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeDecelerate);
                }
            else if( modePtr.Compare( KLitAccelerate ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeAccelerate);
                }
            else if( modePtr.Compare( KLitImpulse ) == 0 )
                {
                aTimeLine.SetInterpolationMode(EInterpolationModeImpulse);
                }
*/                
            else
                {
                // unknown (unsupported) interpolation modes default to linear
                // (at least during testing when we add modes)
                aTimeLine.SetInterpolationMode(EInterpolationModeLinear);
//                FAIL(KErrGeneral, _L("Unknown interpolation mode"));
                }
            break;
            }
        case ENodeTypeMarker:
            {
            TReal32 time = ParseFloatAttributeL(aNode, KLitAt);
            TInt typeIndex = ((CMDXMLElement*)aNode)->FindIndex( KLitType );
            
            if (typeIndex == KErrNotFound)
                {
                FAIL(KErrGeneral, _L("Marker name not found"));
                }

            TPtrC attributeValue;
            TPtrC attributeName;
            User::LeaveIfError(((CMDXMLElement*)aNode)->AttributeDetails( typeIndex, attributeName, attributeValue ));

            // TODO: Should this be controllable?
            aTimeLine.SetLoopingMode(ELoopingModeRepeat);

            if (attributeValue.Compare(KLitLoopStart) == 0)
                {
                aTimeLine.SetLoopStart(time);
                }
            else if (attributeValue.Compare(KLitLoopEnd) == 0)
                {
                aTimeLine.SetLoopEnd(time);
                }
            else
                {
                FAIL(KErrGeneral, _L("Unsupported marker type"));
                }
            break;
            }
        case ENodeTypeKeyFrame:
            {
            TReal32 time = ParseFloatAttributeL(aNode, KLitAt);
            TReal32 aux1 = ParseFloatAttributeL(aNode, KLitAux1, EFalse);
            TReal32 aux2 = ParseFloatAttributeL(aNode, KLitAux2, EFalse);
            VALUE_TYPE value = ParseAnimationKeyFrameValueL<PARAM_TYPE::ValueType, TIMELINE_TYPE>(aNode->FirstChild(), aTimeLine);
            TRAPD(err, aTimeLine.AppendKeyFrameL(time, value, aux1, aux2));
            if (err != KErrNone)
                {
                FAIL(KErrGeneral, _L("Unable to append new key frame to timeline"));
                }
            }
            break;
        case ENodeTypeStart:
            {
            TInt refIndex  = ((CMDXMLElement*)aNode)->FindIndex( KLitRef );
            TReal32 refValue = 0;
            TBool needRefValue = EFalse;
            THuiFxReferencePoint ref = EReferencePointUndefined;
            if ( refIndex != KErrNotFound )
                {
                ref = GetReferencePointL( aNode, refValue, needRefValue );
                }
            aParam.SetStartReference( ref );
            if ( needRefValue )
                {
                aParam.SetStartValue( refValue );
                }
            if ( aParam.StartReference() != EReferencePointUndefined &&
               aParam.EndReference() != EReferencePointUndefined )
                {
                aParam.SetReferencePoint( EReferencePointUndefined );
                }
	     float value = ParseFloatValueL(aNode->FirstChild());
	     aParam.SetStartMultiplier( value );

            }
            break;
        case ENodeTypeEnd:
            {
            TInt refIndex  = ((CMDXMLElement*)aNode)->FindIndex( KLitRef );
            TReal32 refValue = 0;
            TBool needRefValue = EFalse;
            THuiFxReferencePoint ref = EReferencePointUndefined;
            if ( refIndex != KErrNotFound )
                {
                ref = GetReferencePointL( aNode, refValue, needRefValue );
                }
            aParam.SetEndReference( ref );
            if ( needRefValue )
                {
                aParam.SetEndValue( refValue );
                }
            if ( aParam.StartReference() != EReferencePointUndefined &&
               aParam.EndReference() != EReferencePointUndefined )
                {
                aParam.SetReferencePoint( EReferencePointUndefined );
                }
            float value = ParseFloatValueL(aNode->FirstChild());
            aParam.SetEndMultiplier( value );

            }
            break;
        }
#ifdef _HUI_FX_PARSER_LOGGING
    __ALFFXLOGSTRING("CHuiFxEffectParser::ParseAnimationNodeL - out");
#endif
    }


void CHuiFxEffectParser::ResolveParamL( CMDXMLNode* aNode, CHuiFxLayer* aLayer )
    {
#ifdef _HUI_FX_PARSER_LOGGING
    __ALFFXLOGSTRING1("CHuiFxEffectParser::ResolveParamL - 0x%x",this);
#endif
    ASSERT(aNode->NodeType() == CMDXMLNode::EElementNode);

    TInt nameIndex = ((CMDXMLElement*)aNode)->FindIndex( KLitName );
    TInt typeIndex = ((CMDXMLElement*)aNode)->FindIndex( KLitType );
    TInt refIndex  = ((CMDXMLElement*)aNode)->FindIndex( KLitRef );
    if( nameIndex == KErrNotFound )
        {
        FAIL(KErrGeneral, _L("Parameter name not found"));
        }
    
    THuiFxReferencePoint ref = EReferencePointIdentity;
    TBool isAnimated = EFalse;
    TPtrC paramName;
    TPtrC attributeName;
    User::LeaveIfError(((CMDXMLElement*)aNode)->AttributeDetails(nameIndex, attributeName, paramName));

    if (typeIndex != KErrNotFound)
        {
        TPtrC paramType;
        User::LeaveIfError(((CMDXMLElement*)aNode)->AttributeDetails(typeIndex, attributeName, paramType));

        if( paramType.Compare( KLitAnim ) == 0 )
            {
            isAnimated = ETrue;
            aLayer->SetAnimated(ETrue);
            }
        else
            {
            FAIL(KErrGeneral, _L("Unsupported parameter type"));
            }
        }
    
    TReal32 refValue;
    TBool needRefValue = EFalse;
    
    
    if ( refIndex != KErrNotFound )
        {
        ref = GetReferencePointL( aNode, refValue, needRefValue );
        if ( ref == EReferencePointUndefined )
            {
            // For the parameters an undefined reference point means no value for the change
            ref = EReferencePointIdentity;
            }
        }
    
    MHuiFxParameter* param = NULL;
    if (aLayer && aLayer->Type() == ELayerTypeFilter)
        {
        CHuiFxFilterLayer* filterLayer = reinterpret_cast<CHuiFxFilterLayer*>(aLayer);
        // We must check if we really have a filter.
        // If the filter type was incorrect, a filter was not created
        // We should not crash or panic even if the effect file is not syntactically correct.
        // An incorrect file should simply produce no effect.
        CHuiFxFilter& filter = filterLayer->Filter();
        
        if ( &filter )
            {
            param = filter.Parameter( paramName );
            }
        }
    else if (aLayer && aLayer->Type() == ELayerTypeVisual)
        {
        CHuiFxVisualLayer* visualLayer = reinterpret_cast<CHuiFxVisualLayer*>(aLayer); 
        param = visualLayer->Parameter(paramName);
        }
    
    if (!param)
        {
        FAIL(KErrNotFound, _L("Parameter name not recognized")); // leaves
        return; // never reached. For completeness
        }

    param->SetReferencePoint(ref);
    if ( needRefValue )
        {
        param->SetReferenceValue( refValue );
        }
    
    if (isAnimated)
        {
        // Collect animation keyframes and other parameters
        switch (param->Type())
            {
            case EParameterTypeScalar:
                {
                RHuiFxScalarTimeLine* t = new (ELeave) RHuiFxScalarTimeLine();
                CHuiFxScalarParameter* p = static_cast<CHuiFxScalarParameter*>(param); 
                CleanupStack::PushL(t);
                for (CMDXMLNode* child = aNode->FirstChild(); child; child = child->NextSibling())
                    {
                    ParseAnimationNodeL<CHuiFxScalarParameter, RHuiFxScalarTimeLine, CHuiFxScalarParameter::ValueType>(child, *p, *t);
                    }
                CleanupStack::Pop(t);
                p->SetTimeLine(t);
                if ( p->ReferencePoint() == EReferencePointUndefined )
                    {
                    if ( paramName.Compare( KLitScaleX ) == 0 )
                        {
                        p->SetReferencePoint( EReferencePointVisualWidth );
                        }
                    else if ( paramName.Compare( KLitScaleY ) == 0 )
                        {
                        p->SetReferencePoint( EReferencePointVisualHeight );
                        }
                    else
                        {
                        p->SetReferencePoint( EReferencePointIdentity );
                        }
                    }
                break;
                }
        case EParameterTypeColor:
                {
                RHuiFxColorTimeLine* t = new (ELeave) RHuiFxColorTimeLine();
                CHuiFxColorParameter* p = static_cast<CHuiFxColorParameter*>(param); 
                CleanupStack::PushL(t);
                for (CMDXMLNode* child = aNode->FirstChild(); child; child = child->NextSibling())
                    {
                    ParseAnimationNodeL<CHuiFxColorParameter, RHuiFxColorTimeLine, CHuiFxColorParameter::ValueType>(child, *p, *t);
                    }
                CleanupStack::Pop(t);
                p->SetTimeLine(t);
                if ( p->ReferencePoint() == EReferencePointUndefined )
                    {
                    p->SetReferencePoint( EReferencePointIdentity );
                    }
                break;
                }
            }
        }
    else
        {
        switch (param->Type())
            {
            case EParameterTypeScalar:
                {
                CHuiFxScalarParameter* p = static_cast<CHuiFxScalarParameter*>(param); 
                p->SetValue(ParseFloatValueL(aNode->FirstChild()));
                break;
                }
        case EParameterTypeColor:
                {
                CHuiFxColorParameter* p = static_cast<CHuiFxColorParameter*>(param); 
                p->SetValue(ParseColorValueL(aNode->FirstChild()));
                break;
                }
            }
        }
#ifdef _HUI_FX_PARSER_LOGGING
    __ALFFXLOGSTRING("CHuiFxEffectParser::ResolveParamL - out ");
#endif
    }

THuiFxFilterType CHuiFxEffectParser::GetFilterTypeL( CMDXMLNode* aNode )
    {
#ifdef _HUI_FX_PARSER_LOGGING
    __ALFFXLOGSTRING1("CHuiFxEffectParser::GetFilterTypeL - 0x%x",this);
#endif
    if (aNode->NodeType() != CMDXMLNode::EElementNode)
        {
        FAIL(KErrGeneral, _L("Text node expected while reading filter type"));
        }
    
    TInt attributeIndex = ((CMDXMLElement*)aNode)->FindIndex( KLitType );

    if (attributeIndex == KErrNotFound)
        {
        FAIL(KErrGeneral, _L("Filter type not found"));
        }
    
    TPtrC attributeValue;
    TPtrC attributeName;
    User::LeaveIfError(((CMDXMLElement*)aNode)->AttributeDetails( attributeIndex, attributeName, attributeValue));

    if( attributeValue.Compare( KLitDesaturate ) == 0 )
        {
        return EFilterTypeDesaturate;
        }
    else if( attributeValue.Compare( KLitBlur ) == 0 )
        {
        return EFilterTypeBlur;
        }
    else if( attributeValue.Compare( KLitGlow ) == 0 )
        {
        return EFilterTypeGlow;
        }
    else if( attributeValue.Compare( KLitBrightnessContrast ) == 0 )
        {
        return EFilterTypeBrightnessContrast;
        }
    else if( attributeValue.Compare( KlitHSL ) == 0 )
        {
        return EFilterTypeHSL;
        }
    else if( attributeValue.Compare( KlitColorize ) == 0 )
        {
        return EFilterTypeColorize;
        }
    else if( attributeValue.Compare( KlitOutline ) == 0 )
        {
        return EFilterTypeOutline;
        }
    else if( attributeValue.Compare( KLitDropShadow ) == 0 )
        {
        // drop shadow is generated using outline filter
        return EFilterTypeOutline;
        }
    else if( attributeValue.Compare( KlitBevel ) == 0 )
        {
        return EFilterTypeBevel;
        }
    else if ( attributeValue.Compare ( KlitTransform ) == 0 )
        {
        return EFilterTypeTransform;
        }
    else
        {
        return EFilterTypeUnknown;
        }
    }
    
THuiFxReferencePoint CHuiFxEffectParser::GetReferencePointL( CMDXMLNode* aNode, TReal32& aRefValue, TBool& aNeedRefValue )
    {
    __ALFFXLOGSTRING1("CHuiFxEffectParser::GetReferencePointL - 0x%x",this);
    if (aNode->NodeType() != CMDXMLNode::EElementNode)
        {
        FAIL(KErrGeneral, _L("Text node expected while reading ref type"));
        }
    
    TInt attributeIndex = ((CMDXMLElement*)aNode)->FindIndex( KLitRef );

    THuiFxReferencePoint ref = EReferencePointIdentity;
    if (attributeIndex == KErrNotFound)
        {
        ref = EReferencePointUndefined;
        return ref;
        }
    aRefValue = 0;
        
    TPtrC attributeName;
    TPtrC paramRef;
    User::LeaveIfError(((CMDXMLElement*)aNode)->AttributeDetails( attributeIndex, attributeName, paramRef));
    TReal32 refValue = 0;
    TBool needRefValue = EFalse;

    if ( paramRef.Compare( KLitVisualWidth ) == 0 )
        {
        ref = EReferencePointVisualWidth;
        }
    else if ( paramRef.Compare( KLitVisualHeight ) == 0 )
        {
        ref = EReferencePointVisualHeight;
        }
    else if ( paramRef.Compare( KLitVisualTop ) == 0 )
        {
        ref = EReferencePointVisualTop;
        }
    else if ( paramRef.Compare( KLitVisualBottom ) == 0 )
        {
        ref = EReferencePointVisualBottom;
        }
    else if ( paramRef.Compare( KLitVisualLeft ) == 0 )
        {
        ref = EReferencePointVisualLeft;
        }
    else if ( paramRef.Compare( KLitVisualRight ) == 0 )
        {
        ref = EReferencePointVisualRight;
        }
    else if ( paramRef.Compare( KLitDisplayWidth ) == 0 )
        {
        ref = EReferencePointDisplayWidth;
        }
    else if ( paramRef.Compare( KLitDisplayHeight ) == 0 )
        {
        ref = EReferencePointDisplayHeight;
        }
    else if ( paramRef.Compare( KLitDisplayHeightMinusVisualTop ) == 0 )
        {
        ref = EReferencePointDisplayHeightMinusVisualTop;
        }
    else if ( paramRef.Compare( KLitDisplayTop ) == 0 )
        {
        ref = EReferencePointDisplayTop;
        }
    else if ( paramRef.Compare( KLitDisplayBottom ) == 0 )
        {
        ref = EReferencePointDisplayBottom;
        }
    else if ( paramRef.Compare( KLitDisplayLeft ) == 0 )
        {
        ref = EReferencePointDisplayLeft;
        }
    else if ( paramRef.Compare( KLitDisplayRight ) == 0 )
        {
        ref = EReferencePointDisplayRight;
        }
    else if ( paramRef.Compare( KLitExtRectWidth ) == 0 )
        {
        needRefValue = ETrue;
        ref = EReferencePointExtRectWidth;
        refValue = iExtRect.Width();
        }
    else if ( paramRef.Compare( KLitExtRectHeight ) == 0 )
        {
        needRefValue = ETrue;
        ref = EReferencePointExtRectHeight;
        refValue = iExtRect.Height();
        }
    else if ( paramRef.Compare( KLitExtRectTop ) == 0 )
        {
        needRefValue = ETrue;
        ref = EReferencePointExtRectTop;
        refValue = iExtRect.iTl.iY;
        }
    else if ( paramRef.Compare( KLitExtRectBottom ) == 0 )
        {
        needRefValue = ETrue;
        ref = EReferencePointExtRectBottom;
        refValue = iExtRect.iBr.iY;
        }
    else if ( paramRef.Compare( KLitExtRectLeft ) == 0 )
        {
        needRefValue = ETrue;
        ref = EReferencePointExtRectLeft;
        refValue = iExtRect.iTl.iX;
        }
    else if ( paramRef.Compare( KLitExtRectRight ) == 0 )
        {
        needRefValue = ETrue;
        ref = EReferencePointExtRectRight;
        refValue = iExtRect.iBr.iX;
        }
    // these are deprecated
    else if (paramRef.Compare(KLitVisualWidth1) == 0)
        {
        ref = EReferencePointVisualWidth; 
        }
    else if (paramRef.Compare(KLitVisualHeight1) == 0)
        {
        ref = EReferencePointVisualHeight; 
        }
    else if (paramRef.Compare(KLitDisplayWidth1) == 0)
        {
        ref = EReferencePointDisplayWidth; 
        }
    else if (paramRef.Compare(KLitDisplayHeight1) == 0)
        {
        ref = EReferencePointDisplayHeight; 
        }
    else
        {
        FAIL(KErrGeneral, _L("Unsupported parameter reference point"));
        }
    aRefValue = refValue;
    aNeedRefValue = needRefValue;
#ifdef _HUI_FX_PARSER_LOGGING
    __ALFFXLOGSTRING1("CHuiFxEffectParser::GetReferencePointL - return %d",ref);
#endif
    return ref;
    }
TBool CHuiFxEffectParser::GetOpaqueHintL( CMDXMLNode *aNode )
    {
    if (aNode->NodeType() != CMDXMLNode::EElementNode)
        {
        FAIL(KErrGeneral, _L("Text node expected while reading visual source type"));
        }
    TInt attributeIndex = ((CMDXMLElement*)aNode)->FindIndex( KLitOpaque );
    if (attributeIndex == KErrNotFound)
        {
        // If src not found, the source defaults to visual itself
        return EFalse;
        }
      
    TPtrC attributeValue;
    TPtrC attributeName;
    User::LeaveIfError(((CMDXMLElement*)aNode)->AttributeDetails( attributeIndex, attributeName, attributeValue ));
  
    if( attributeValue.Compare( KLitTrue ) == 0 )
        {
        return ETrue;
        }
    
    if( attributeValue.Compare( KLitFalse ) == 0 )
        {
        return EFalse;
        }

    return EFalse;
    }

THuiFxVisualSrcType CHuiFxEffectParser::GetSrcTypeL( CMDXMLNode* aNode, TPtrC16& aBitmap )
    {
#ifdef _HUI_FX_PARSER_LOGGING
    __ALFFXLOGSTRING1("CHuiFxEffectParser::GetSrcTypeL - 0x%x ",this);
#endif
    if (aNode->NodeType() != CMDXMLNode::EElementNode)
        {
        FAIL(KErrGeneral, _L("Text node expected while reading visual source type"));
        }
    TInt attributeIndex = ((CMDXMLElement*)aNode)->FindIndex( KLitSrc );
    
    if (attributeIndex == KErrNotFound)
        {
        // If src not found, the source defaults to visual itself
        return EVisualSrcVisual;
        }
        
    TPtrC attributeValue;
    TPtrC attributeName;
    User::LeaveIfError(((CMDXMLElement*)aNode)->AttributeDetails( attributeIndex, attributeName, attributeValue ));
  
    if( attributeValue.Compare( KLitItem ) == 0 )
        {
        return EVisualSrcVisual;
        }
    else if ( attributeValue.Compare( KLitInput1 ) == 0 )
        {
        // This means that the source is given as a parameter to the effect
        return EVisualSrcInput1;
        }
    else if ( attributeValue.Compare( KLitInput2 ) == 0 )
        {
        // This means that the source is given as a parameter to the effect
        return EVisualSrcInput2;
        }
    else
        {
        // if nothing else is defined, the src represents an external file (bitmap)
        aBitmap.Set( attributeValue );
        return EVisualSrcBitmap;
        }
    
    }


void CHuiFxEffectParser::FailL(TInt aCode, TRefByValue<const TDesC> aFormat, ...)
    {
    TBuf<512> message;
    VA_LIST args;
    VA_START(args, aFormat);
    message.Append(_L("CHuiFxEffectParser::Fail(): "));
    message.AppendFormatList(aFormat, args);
    VA_END(args);
    __ALFFXLOGSTRING1("CHuiFxEffectParser::FailL %S", &message);
    User::InfoPrint( message );
    User::Leave(aCode);
    }
