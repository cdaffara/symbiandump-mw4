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



#ifndef HUIFXEFFECTPARSER_H_
#define HUIFXEFFECTPARSER_H_

#include <f32file.h>
#include <gmxmlparser.h>
#include "HuiFxEngine.h"
#include "HuiFxFilter.h"

//FORWARD DECLARATIONS
class CMDXMLNode;
class CMDXMLDocument;
class CMDXMLParser;
class CHuiVisual;
class CHuiFxLayer;
class MAlfGfxEffectObserver;

class MHuiFxParsingEndedObserver
    {
public:
    virtual void ParsingEnded(TInt aHandle) =0;
    };


class CHuiFxEffectParser : public CActive, public MMDXMLParserObserver
    {
public: // construction/destruction
    static CHuiFxEffectParser* NewL(CHuiFxEngine& aEngine, MHuiEffectable* aVisual = 0);
    static CHuiFxEffectParser* NewLC(CHuiFxEngine& aEngine, MHuiEffectable* aVisual = 0);
    virtual ~CHuiFxEffectParser();

public: // from MMDXMLParserObserver
    void ParseFileCompleteL();
    
public: // from CActive
    void RunL();
    TInt RunError( TInt aError );
    void DoCancel();
    
public:
    void ParseL( const TDesC& aFile, TRect aExtRect );
    void Effect( CHuiFxEffect*& aEffect ); // effect ownership transferred to caller
    
    void SetEffectEndObserver( MAlfGfxEffectObserver* aEffectEndObserver, TInt aHandle );
    void SetEffectFlags( TInt aFlags );
public:
    void SetParsingEndedObserver( MHuiFxParsingEndedObserver *aObserver, TInt aHandle );
    
protected:
    CHuiFxEffectParser(CHuiFxEngine& aEngine, MHuiEffectable* aVisual);
    
private:
    enum TNodeType
        {
        ENodeTypeUnknown,
        ENodeTypeVisual,
        ENodeTypeGroup,
        ENodeTypeFilter,
        ENodeTypeBlending,
        ENodeTypeParam,
        ENodeTypeKeyFrame,
        ENodeTypeDuration,
        ENodeTypeStyle,
        ENodeTypeMarker,
        ENodeTypeStart,
        ENodeTypeEnd,
        };
        
    void ConstructL();
    void BuildEffectL();
    void ParseNodeL( CMDXMLNode* aNode, CHuiFxLayer* aLayer = NULL );
    CHuiFxEffectParser::TNodeType ResolveNode( CMDXMLNode* aNode );
    void ResolveParamL( CMDXMLNode* aNode, CHuiFxLayer* aLayer );
    THuiFxFilterType GetFilterTypeL( CMDXMLNode* aNode );
    THuiFxReferencePoint GetReferencePointL( CMDXMLNode* aNode, TReal32& aRefValue, TBool& aNeedRefValue );
    THuiFxVisualSrcType GetSrcTypeL( CMDXMLNode* aNode, TPtrC16& aBitmap );
    TBool GetOpaqueHintL( CMDXMLNode *aNode );
    TReal32 ParseFloatValueL(CMDXMLNode* aNode);
    TRgb ParseColorValueL(CMDXMLNode* aNode);
    TReal32 ParseFloatAttributeL(CMDXMLNode* aNode, const TDesC& aName, TBool aMustExist = ETrue);

    template <typename PARAM_TYPE, typename TIMELINE_TYPE, typename VALUE_TYPE>
    void ParseAnimationNodeL(CMDXMLNode* aNode, PARAM_TYPE& aParam, TIMELINE_TYPE& aTimeLine);

    template <typename VALUE_TYPE, typename TIMELINE_TYPE>
    VALUE_TYPE ParseAnimationKeyFrameValueL(CMDXMLNode* aNode, TIMELINE_TYPE& aTimeLine);
    
    void FailL(TInt aCode, TRefByValue<const TDesC> aFormat, ...);
    
private: // members
    CHuiFxEffect*           iEffect;
    CHuiFxEngine&           iEngine;
    CMDXMLParser*           iParser;
    CMDXMLDocument*         iDoc;
    RFs                     iFs;
    MHuiEffectable*             iVisual;
    TRect                   iExtRect;
    MAlfGfxEffectObserver*  iEffectEndObserver;
    TInt                    iHandle; // handle for effect end observer
    TInt                    iFlags; // flags from caller that will go into the effect
    TBool                   iIsCancelled;
    MHuiFxParsingEndedObserver *iParsingEndedObserver;
    TInt iParsingEndedHandle;
public:
    /*void LoadEffect(const TDesC& aFileName, Effect* aTemplateEffect);*/
private:
    };

#endif /*HUIFXEFFECTPARSER_H_*/
