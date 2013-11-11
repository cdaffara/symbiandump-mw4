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

#ifndef HUIFXEFFECTCACHE_H_
#define HUIFXEFFECTCACHE_H_

#include <e32cmn.h>
#include <e32base.h>
#include <e32hashtab.h>
#include "HuiFxEffectParser.h"
#include "huieffectable.h"

class MHuiFxParsingEndedObserver;
class MAlfGfxEffectObserver;


class MHuiFxEffectCacheNode
{
public:
    virtual ~MHuiFxEffectCacheNode() { }
    virtual TPtrC Id()=0; // how to regognize this instance
    virtual MHuiFxEffectCacheNode *CloneL()=0; // from this instance to cache
    virtual void LightOperationL(MHuiFxEffectCacheNode *aCached)=0; // from cache to this instance
    virtual void HeavyOperationL(MHuiFxParsingEndedObserver *aObserver, TInt aIndex)=0;    // from nothing to this instance
    virtual TInt Ref(TInt aCount)=0; // increase count by aCount and return count.
    virtual void ParsingEndedBefore()=0;
    virtual void ParsingEndedAfter(MHuiFxEffectCacheNode *aCached)=0;
    virtual TBool FxmlUsesInput1()=0;
    virtual TBool FxmlUsesOpaqueHint() const=0;

    virtual void Delete()=0;
};

class CHuiFxEffectCache : public CBase, public MHuiFxParsingEndedObserver
{
public:
    IMPORT_C static CHuiFxEffectCache* NewL();
    void CreateL(MHuiFxEffectCacheNode *aNode); // moves ownership
    IMPORT_C void FindOrCreateL(MHuiFxEffectCacheNode *aNode); // moves ownership
    MHuiFxEffectCacheNode *FindDup(const TDesC &aId);
    IMPORT_C void UnUse(MHuiFxEffectCacheNode *aNode);
    IMPORT_C ~CHuiFxEffectCache();
    void ParsingEnded(TInt aHandle);
    MHuiFxEffectCacheNode *FindCached(const TDesC &aId) const;
    TBool FxmlUsesInput1(const TDesC &aFileName);
    TBool FxmlUsesOpaqueHint(const TDesC &aFileName) const;
private:
    struct IDNode
        {
        TInt iId;
        MHuiFxEffectCacheNode *iNode;
        };
    
    MHuiFxEffectCacheNode *Find(MHuiFxEffectCacheNode *aNode);
    MHuiFxEffectCacheNode *FindDuplicate(MHuiFxEffectCacheNode *aNode);
    static bool Compare(MHuiFxEffectCacheNode *aNode1, MHuiFxEffectCacheNode *aNode2);
    void Remove(RPointerArray<MHuiFxEffectCacheNode> &aEffects, MHuiFxEffectCacheNode *aNode);
    void Remove(RHashMap<TInt, MHuiFxEffectCacheNode*> &aMap, MHuiFxEffectCacheNode *aNode);
    void Remove(RArray<IDNode> &aMap, MHuiFxEffectCacheNode *aNode);
    TInt FindById(TInt aId);
    TInt UniqueId();
private:
   RPointerArray<MHuiFxEffectCacheNode> iCachedEffects;
   //RPointerArray<MHuiFxEffectCacheNode> iDuplicateEffects; // array can have null pointers in it
   //RHashMap<TInt, MHuiFxEffectCacheNode*> iDuplicateEffects;
   RArray<IDNode> iDuplicateEffects;
   TInt iUniqueId;
};

class CHuiFxEffect;
class TRect;
class CHuiVisual;
class CHuiFxEngine;

// This class has all dependencies to outside objects.
class CHuiFxEffectCacheEffectNode : public CBase, public MHuiFxEffectCacheNode
{
public:
     CHuiFxEffectCacheEffectNode(const TDesC &aFileName, CHuiFxEffect *&aEffect, MHuiEffectable *aVisual, TRect *extRect, CHuiFxEngine *aEngine) 
	: iFileName(aFileName.AllocL()), iEffect(aEffect), iVisual(aVisual), iExtRect(extRect), iEffectCached(0), iEngine(aEngine), iRefCount(0), iParser(0), iGroup(KErrNotFound) { }
     IMPORT_C ~CHuiFxEffectCacheEffectNode();
     void SetEffectEndObserver( MAlfGfxEffectObserver* aEffectEndObserver, TInt aHandle );
     void SetEffectFlags( TInt aFlags );
     void SetEffectGroup( TInt aGroup);
private:
     CHuiFxEffectCacheEffectNode(const TDesC &aFileName, CHuiFxEffect *aEffect) : iFileName(aFileName.AllocL()), iEffect(iEffectCached), iVisual(0), iExtRect(0), iEffectCached(aEffect), iRefCount(0), iParser(0), iEffectEndObserver(0), iHandle(0) { }
public: // from MHuiFxEffectCacheNode
     TPtrC Id();
     MHuiFxEffectCacheNode *CloneL();
     void LightOperationL(MHuiFxEffectCacheNode *aCached);
     void HeavyOperationL(MHuiFxParsingEndedObserver *aObserver, TInt aIndex);
     TInt Ref(TInt aCount);
     void ParsingEndedBefore();
     void ParsingEndedAfter(MHuiFxEffectCacheNode *aCached);
     TBool FxmlUsesInput1();
     TBool FxmlUsesOpaqueHint() const;
     void Delete();

private:
     const TDesC *iFileName;
     CHuiFxEffect *iEffect;
     MHuiEffectable *iVisual;
     TRect *iExtRect;
     CHuiFxEffect *iEffectCached;
     CHuiFxEngine *iEngine;
     TInt iRefCount;
     CHuiFxEffectParser *iParser;
     MAlfGfxEffectObserver* iEffectEndObserver;
     TInt iHandle;
     TInt iFlags;
     TInt iGroup;
};


#endif
