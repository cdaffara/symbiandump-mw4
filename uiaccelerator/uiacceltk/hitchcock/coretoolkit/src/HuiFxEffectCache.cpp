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

#include "HuiFxEffectCache.h"
#include "HuiFxEffectParser.h"
#include "HuiFxEffect.h"
#include "uiacceltk/HuiVisual.h" 
#include "uiacceltk/HuiPanic.h"
#include "HuiFxConstants.h"

CHuiFxEffectCache *CHuiFxEffectCache::NewL()
{
	return new (ELeave)CHuiFxEffectCache;
}

CHuiFxEffectCache::~CHuiFxEffectCache()
   {
   iCachedEffects.ResetAndDestroy();
#if 0
   THashMapIter<TInt, MHuiFxEffectCacheNode*> iter(iDuplicateEffects);
   MHuiFxEffectCacheNode * const *node;
   do
       {
       node =  iter.NextValue();
       if (node)
           {
           delete *node;
           }
       }
   while(node);
#endif
   TInt size = iDuplicateEffects.Count();
   for(TInt i=0;i<size;i++)
       {
       const IDNode &node = iDuplicateEffects[i];
       delete node.iNode;
       }

   iCachedEffects.Close();
   iDuplicateEffects.Close();   
   }

TInt CHuiFxEffectCache::UniqueId()
    {
    iUniqueId++;
    return iUniqueId;
    }

void CHuiFxEffectCache::CreateL(MHuiFxEffectCacheNode *aNode)
    { // registration of fxml file
    MHuiFxEffectCacheNode *cachedNode = Find(aNode);
    MHuiFxEffectCacheNode *dupNode = FindDuplicate(aNode);
    if (cachedNode)
       { // found in cache => duplicate registration => no need to do anything
       cachedNode->Ref(1);
       delete aNode;
       }
    else if (dupNode)
        { // found same file already being loaded
        dupNode->Ref(1);
        delete aNode;
        }
    else
       { // not found in cache, so insert it and start parsing the effect.
       TInt id = UniqueId();
       IDNode n = { id, aNode };
       iDuplicateEffects.Append(n);

       TRAPD(err, aNode->HeavyOperationL(this, id)); // from empty to this instance
       if (err != KErrNone)
           {
           iDuplicateEffects.Remove(iDuplicateEffects.Count()-1);
           User::Leave(err);
           }
       }
    }

EXPORT_C void CHuiFxEffectCache::FindOrCreateL(MHuiFxEffectCacheNode *aNode)
   { // using fxml file
   MHuiFxEffectCacheNode *cachedNode = Find(aNode);

   if (cachedNode)
	  { // found in cache
	  aNode->LightOperationL(cachedNode); // from cache to this instance  
	  delete aNode;
	  }
   else
	  { // not found in cache, so start parsing the effect.

	  // TODO: What happens if we did not find it from cache, but it exists in iDuplicateEffects?
	  //       Current behaviour is that it starts to load it again, causing duplicate entries in the cache.
	  TInt id = UniqueId();
      IDNode n = { id, aNode };
      iDuplicateEffects.Append(n);
      //iDuplicateEffects.InsertL(id, aNode);

	  TRAPD(err, aNode->HeavyOperationL(this, id)); // from empty to this instance
	  if (err != KErrNone)
	      {
	      iDuplicateEffects.Remove(iDuplicateEffects.Count()-1);
	      User::Leave(err);
	      }
	  }
  }

void CHuiFxEffectCache::UnUse(MHuiFxEffectCacheNode *aNode)
   { // unregisteration of fxml file
   MHuiFxEffectCacheNode *cachedNode = Find(aNode);
   if (cachedNode)
       {
       TInt count = cachedNode->Ref(-1);
       if (count == 0)
           {
           Remove(iCachedEffects, cachedNode);
           cachedNode->Delete();
           delete cachedNode;
           }
       }
   if (cachedNode != aNode)
       { // this node was not found in iCachedEffects array, so it needs to be in iDuplicateEffects array
       Remove(iDuplicateEffects, aNode);
       delete aNode;
       }
   }

void CHuiFxEffectCache::ParsingEnded(TInt aHandle)
    {
    //MHuiFxEffectCacheNode **node2 = iDuplicateEffects.Find(aHandle);
    TInt index = FindById(aHandle);
    if (index == -1) return;
    MHuiFxEffectCacheNode *node = iDuplicateEffects[index].iNode;
    node->ParsingEndedBefore();
    iCachedEffects.AppendL(node); // takes ownership
    iDuplicateEffects.Remove(index);
    node->Ref(1);
    node->ParsingEndedAfter(node);
    }

bool CHuiFxEffectCache::Compare(MHuiFxEffectCacheNode *aNode1, MHuiFxEffectCacheNode *aNode2)
   {
   return aNode1->Id().Compare(aNode2->Id()) == 0;
   }

MHuiFxEffectCacheNode *CHuiFxEffectCache::Find(MHuiFxEffectCacheNode *aNode)
   {
   TInt size = iCachedEffects.Count();
   for(TInt i=0;i<size;i++)
	{
	MHuiFxEffectCacheNode *node = iCachedEffects[i];
	if (Compare(node, aNode))
		{
		return node;
		}
	}
   return 0;
   }
MHuiFxEffectCacheNode *CHuiFxEffectCache::FindDuplicate(MHuiFxEffectCacheNode *aNode)
    {
    TInt size = iDuplicateEffects.Count();
    for(TInt i=0;i<size;i++)
        {
        MHuiFxEffectCacheNode *node = iDuplicateEffects[i].iNode;
        if (Compare(node, aNode))
            {
            return node;
            }
        }
    return 0;
    }

TInt CHuiFxEffectCache::FindById(TInt aId)
    {
    TInt size = iDuplicateEffects.Count();
    for(TInt i=0;i<size;i++)
        {
        const IDNode &node = iDuplicateEffects[i];
        if (node.iId == aId)
            {
            return i;
            }
        }
    return -1;
    }


MHuiFxEffectCacheNode *CHuiFxEffectCache::FindDup(const TDesC &aId)
    {
    TInt size = iDuplicateEffects.Count();
    for(TInt i=0;i<size;i++)
        {
        const IDNode &node = iDuplicateEffects[i];
        if (node.iNode->Id() == aId)
            {
            return node.iNode;
            }
        }
    
#if 0
    THashMapIter<TInt, MHuiFxEffectCacheNode*> iter(iDuplicateEffects);
    MHuiFxEffectCacheNode * const * node = 0;
    do
        {
        node = iter.NextValue();
        if (node && (*node)->Id()==aId)
            {
            return *node;
            }
        }
    while(node);
#endif
    
   TInt size2 = iCachedEffects.Count();
   for(TInt i=0;i<size2;i++)
	   {
	   MHuiFxEffectCacheNode *node = iCachedEffects[i];
	   if (node->Id()==aId)
		  {
		  return node;
		  }
	   }
    return 0;
    }
MHuiFxEffectCacheNode *CHuiFxEffectCache::FindCached(const TDesC &aId) const
    {
    TInt size2 = iCachedEffects.Count();
    for(TInt i=0;i<size2;i++)
        {
        MHuiFxEffectCacheNode *node = iCachedEffects[i];
        if (node->Id()==aId)
           {
           return node;
           }
        }
     return 0;    
    }

TBool CHuiFxEffectCache::FxmlUsesInput1(const TDesC &aFileName)
    {
    MHuiFxEffectCacheNode *node = FindCached(aFileName);
    if (node)
        return node->FxmlUsesInput1();
    else
        return EFalse;
    }

TBool CHuiFxEffectCache::FxmlUsesOpaqueHint(const TDesC &aFileName) const
    {
    MHuiFxEffectCacheNode *node = FindCached(aFileName);
    if (node)
        return node->FxmlUsesOpaqueHint();
    else
        return EFalse;
    }



void CHuiFxEffectCache::Remove(RHashMap<TInt, MHuiFxEffectCacheNode*> & /*aMap*/, MHuiFxEffectCacheNode * /*aNode*/)
    {
#if 0
    THashMapIter<TInt, MHuiFxEffectCacheNode*> iter(aMap);
    MHuiFxEffectCacheNode * const *node = 0;
    do
        {
        node = iter.NextValue();
        if (!node)
            break;
	    MHuiFxEffectCacheNode *n = *node;
        if (n == aNode)
            {
            const TInt *key = iter.CurrentKey();
            aMap.Remove(*key);
            return;
            }
        }
    while(node);
#endif
    }

void CHuiFxEffectCache::Remove(RPointerArray<MHuiFxEffectCacheNode> &aEffects, MHuiFxEffectCacheNode *aNode)
    {
    TInt size = aEffects.Count();
    for(TInt i=0;i<size;i++)
        {
        MHuiFxEffectCacheNode *node = aEffects[i];
        if (node == aNode)
            {
            aEffects.Remove(i);
            return;
            }
        }
    }

void CHuiFxEffectCache::Remove(RArray<IDNode> &aMap, MHuiFxEffectCacheNode *aNode)
    {
    TInt size = aMap.Count();
    for(TInt i=0;i<size;i++)
        {
        const IDNode &n = aMap[i];
        if (n.iNode == aNode)
            {
            aMap.Remove(i);
            return;
            }
        }
    }


CHuiFxEffectCacheEffectNode::~CHuiFxEffectCacheEffectNode()
    {
    delete iFileName;
    delete iParser;
    }
void CHuiFxEffectCacheEffectNode::Delete()
    {
    delete iEffect;
    }

TPtrC CHuiFxEffectCacheEffectNode::Id()
   {
   return TPtrC(*iFileName);
   }

TInt CHuiFxEffectCacheEffectNode::Ref(TInt aCount)
    {
    iRefCount += aCount;
    return iRefCount;
    }

MHuiFxEffectCacheNode *CHuiFxEffectCacheEffectNode::CloneL()
   {
   CHuiFxEffect *effect = iEffect->CloneL();
   return new CHuiFxEffectCacheEffectNode(*iFileName, effect);
   }

void CHuiFxEffectCacheEffectNode::HeavyOperationL(MHuiFxParsingEndedObserver *aObserver, TInt aIndex)
	{
	TRect extRect = TRect();
    if ( iExtRect )
       	{
       	extRect = *iExtRect;
       	}
    iParser = CHuiFxEffectParser::NewL( *iEngine, iVisual );
    iParser->SetEffectEndObserver( iEffectEndObserver, iHandle );
    iParser->SetEffectFlags( iFlags );
    iParser->SetParsingEndedObserver(aObserver, aIndex);
    iParser->ParseL( *iFileName, extRect );
    if (iVisual)
        {
        iVisual->SetLoadingEffect(ETrue);
        }
	}

void CHuiFxEffectCacheEffectNode::LightOperationL(MHuiFxEffectCacheNode *aCached)
	{
	CHuiFxEffectCacheEffectNode *cachednode = (CHuiFxEffectCacheEffectNode*)aCached;
	TRect extRect = TRect();
	if ( iExtRect )
		{
		extRect = *iExtRect;
		}
	iEffectCached = cachednode->iEffect;
	iEffect = iEffectCached->CloneL();	
	iEffect->SetEffectEndObserver(iEffectEndObserver, iHandle);
	iEffect->SetEffectFlags(iFlags);
	iEffect->SetEffectGroup(iGroup);
	iEffect->SetExtRect( &extRect );
	iEffect->SetVisual( iVisual );
	iEffect->SetEngine( iEngine );
	if (iVisual)
	    iVisual->EffectSetEffect( iEffect );
	}

void CHuiFxEffectCacheEffectNode::ParsingEndedBefore()
    {
    iParser->Effect( iEffect ); // moves ownership of iEffect to iCachedNodeArray, and will be deleted in UnUse() / Delete().
    }

void CHuiFxEffectCacheEffectNode::ParsingEndedAfter(MHuiFxEffectCacheNode *aCached)
    {
    if (iVisual)
        {
        //iVisual->SetEffect( iEffect );
        iVisual->SetLoadingEffect(EFalse);
        CHuiFxEffectCacheEffectNode *node = new (ELeave) CHuiFxEffectCacheEffectNode(*iFileName, iEffect, iVisual, iExtRect, iEngine);
        CleanupStack::PushL(node);
        node->LightOperationL(aCached);
        CleanupStack::PopAndDestroy(node);
        }
    // no delete here, because the parser will be destroyed immediately after this inside CHuiFxEffectParser::RunL()
    // This moves ownership to CHuiFxEffectParser.
    iParser = NULL;
    }

void CHuiFxEffectCacheEffectNode::SetEffectEndObserver( MAlfGfxEffectObserver* aEffectEndObserver, TInt aHandle )
    {
    iEffectEndObserver = aEffectEndObserver;
    iHandle = aHandle;
    }

void CHuiFxEffectCacheEffectNode::SetEffectFlags( TInt aFlags )
    {
    iFlags = aFlags;
    }
	
void CHuiFxEffectCacheEffectNode::SetEffectGroup( TInt aGroup)
    {
    iGroup = aGroup;
    }

TBool CHuiFxEffectCacheEffectNode::FxmlUsesInput1()
    {
    CHuiFxEffect *effect = iEffect;

    RArray<THuiFxVisualSrcType> array;
    effect->FxmlVisualInputs(array);
 
    TInt c = array.Count();
    for(TInt i = 0; i<c; i++)
        {
        THuiFxVisualSrcType val = array[i];
        if (val == EVisualSrcInput1)
            {
            array.Close();
            return ETrue;
            }
        }
    array.Close();
    return EFalse;
    }

TBool CHuiFxEffectCacheEffectNode::FxmlUsesOpaqueHint() const
    {
    CHuiFxEffect *effect = iEffect;
    return effect->FxmlUsesOpaqueHint();
    }
