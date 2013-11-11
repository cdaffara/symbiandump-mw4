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
* Description:   Server side texture manager implementation
*
*/



#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/HuiUtil.h>
#include "alfsrvtexturemanager.h"
#include "alfappsrvsession.h"

/**
 * Granularity of CAlfSrvTextureManager::iClients array.
 */
const TInt KAlfSrvClientsGranularity = 4;

/**
 * Values for auto size min change.
 */
const TInt KAlfTextureManagerAutoSizeMinSizeChange[] =
    {
    0,
    4,
    8,
    64,
    128
    };

/**
 * Values for auto size lower threshold (256 = 1.0f).
 */
const TInt KAlfTetureManagerAutoSizeRelativeThresholdLower[] =
    {
    13,     // 0.05f
    26,     // .10f
    64,     // .25f
    128,    // .50f
    256     // 1.0f
    };

/**
 * Values for auto size upper threshold (256 = 1.0f).
 */
const TInt KAlfTetureManagerAutoSizeRelativeThresholdUpper[] =
    {
    13,     // 0.05f
    26,     // .10f
    64,     // .25f
    256,    // 1.0f
    25600   // 100.f
    };

/**
 * Values for auto size downsize settle threshold.
 */
const TInt KAlfTextureManagerAutoSizeDownsizeSettleThreshold[] =
    {
    0,
    8,
    64,
    128,
    256
    };

/**
 * If width/height is below this low boundary, then one pixel precision
 * is used.
 */
const TInt KAlfSrvTextureManagerAutoSizeLowBoundary = 48;

/**
 * If width/height is above low boundary, but below this high boundary,
 * then KAlfSrvTextureManagerAutoSizeMiddleRoundTo precision is used.
 */
const TInt KAlfSrvTextureManagerAutoSizeHighBoundary = 128;

/**
 * Precision used when width/heigth is above low, but below high 
 * boundary. This must be power of two.
 */
const TInt KAlfSrvTextureManagerAutoSizeMiddleRoundTo = 2;

/**
 * Precision used when width/height is above high boundary.
 * This must be power of two.
 */
const TInt KAlfSrvTextureManagerAutoSizeHighRoundTo = 4;

// ======== MEMBER FUNCTIONS ========

CAlfSrvTextureManager* CAlfSrvTextureManager::NewL()
    {
    CAlfSrvTextureManager* self = new (ELeave) CAlfSrvTextureManager;
    return self;
    }
   
CAlfSrvTextureManager::~CAlfSrvTextureManager()
    {
    iClients.Close();
    iUsedTextures.Close();
    }

// ---------------------------------------------------------------------------
// Starts using CHuiEnv.
// ---------------------------------------------------------------------------
// 
void CAlfSrvTextureManager::HandleEnvCreateL( CHuiEnv& aEnv )
    {
    iEnv = &aEnv;
    
    iEnv->TextureManager().
        iTextureAutoSizeObservers.AppendL( *this );
    iIsRendererHWA = ( iEnv->Renderer() != EHuiRendererBitgdi );
    }

// ---------------------------------------------------------------------------
// Stops using CHuiEnv.
// ---------------------------------------------------------------------------
// 
void CAlfSrvTextureManager::HandleEnvToBeDeleted()
    {
    if ( iEnv )
        {
        iEnv->TextureManager().
            iTextureAutoSizeObservers.Remove( *this );
        iEnv = NULL;
        }
    }

// ---------------------------------------------------------------------------
// This functions converts texture ID used in the client side into id that
// can be used in the serverside HuiTextureManager without the textures getting
// accidentally shared between apps.
// ---------------------------------------------------------------------------
// 
TInt CAlfSrvTextureManager::CreateTextureId(
        const TProcessId& aClientOwnerId,
        TInt aClientSideId, 
        TInt aClientSideManagerId, 
        TBool aReUseAnyDeletedId )
    {
    TInt exisitingSharedTextureId = 0;

    // Look for an existing entry for the id.
    for(TInt i = 0; i < iUsedTextures.Count(); i++)
        {
        if (aClientSideManagerId == 0)
            {
            // Normal texture, "private" to client
            if(iUsedTextures[i].iClientSideId == aClientSideId &&
               iUsedTextures[i].iOwnerId == aClientOwnerId)
                {
                // Re-use old id
                iUsedTextures[i].iReleased = EFalse;
                return iUsedTextures[i].iServerSideId;
                }                
            }
        else
            {
            // Shared texture
            if(iUsedTextures[i].iClientSideId == aClientSideId &&
               iUsedTextures[i].iClientSideManagerId == aClientSideManagerId)
                {
                // Re-use old id                
                if (iUsedTextures[i].iOwnerId == aClientOwnerId)
                    {
                    iUsedTextures[i].iReleased = EFalse;
                    return iUsedTextures[i].iServerSideId;
                    }
                else
                    {
                    exisitingSharedTextureId = iUsedTextures[i].iServerSideId;
                    break;    
                    }    
                }                                
            }    
        }


    // Look for an existing deleted entry for the id i.e. recycle deleted texture id.
    // Only non-shared textures are recycled to keep things more simple and safe.
    if (aReUseAnyDeletedId && aClientSideManagerId == 0)
        {            
        for(TInt i = 0; i < iUsedTextures.Count(); i++)
            {
            if (iUsedTextures[i].iOwnerId == aClientOwnerId &&
                iUsedTextures[i].iDeleted &&
                iUsedTextures[i].iClientSideManagerId == aClientSideManagerId)
                {
                // Re-use old deleted id even if it had different id
                iUsedTextures[i].iClientSideId = aClientSideId;
                iUsedTextures[i].iReleased = EFalse;
                iUsedTextures[i].iDeleted = EFalse;
                iUsedTextures[i].iSkinChangeCoordinator = EFalse;
                return iUsedTextures[i].iServerSideId;                    
                }
            }
        }
        

    // No existing entry found, create new one.
    TUsedTexture usedTexture;
    usedTexture.iClientSideId = aClientSideId;
    usedTexture.iClientSideManagerId = aClientSideManagerId;
    usedTexture.iOwnerId = aClientOwnerId;
        
    if (aClientSideManagerId == 0 || !exisitingSharedTextureId)
        {
        // Select new id and make sure it is not used
        ++iNextTextureId; 
        TBool alreadyExists = ETrue;
        while(alreadyExists)
            {
            alreadyExists = EFalse;
            for(TInt i = 0; i < iUsedTextures.Count(); i++)
                {
                if (iUsedTextures[i].iServerSideId == iNextTextureId || iNextTextureId == 0)
                    {
                    iNextTextureId++;
                    alreadyExists = ETrue;
                    break;    
                    }
                }            
            }                            
        usedTexture.iServerSideId = iNextTextureId; 
        }
    else
        {
        usedTexture.iServerSideId = exisitingSharedTextureId;                    
        }        
    
    TBool entryAdded = EFalse;
    for(TInt i = 0; i < iUsedTextures.Count(); i++)
        {
        if (iUsedTextures[i].iOwnerId == aClientOwnerId &&
            iUsedTextures[i].iClientSideId == 0 &&
            usedTexture.iClientSideManagerId == 0 &&
            usedTexture.iServerSideId == 0)
            {
            entryAdded = ETrue;
            iUsedTextures[i] = usedTexture;
            }                                          
        }

    if (!entryAdded)
        {
        iUsedTextures.Append(usedTexture);        
        }
    
    return usedTexture.iServerSideId;    
    }

// ---------------------------------------------------------------------------
// 
// 
// 
// ---------------------------------------------------------------------------
// 
TInt CAlfSrvTextureManager::ExistingTextureId(
        const TProcessId& aClientOwnerId,
        TInt aClientSideId, 
        TInt aClientSideManagerId,
        TBool aIgnoreOwnerForSharedTextures )
    {
    // Look for an existing entry for the id.
    for(TInt i = 0; i < iUsedTextures.Count(); i++)
        {
        if (aClientSideManagerId == 0)
            {
            // Normal texture, "private" to client
            if(iUsedTextures[i].iClientSideId == aClientSideId &&
               iUsedTextures[i].iOwnerId == aClientOwnerId &&
               iUsedTextures[i].iReleased == EFalse)
                {
                return iUsedTextures[i].iServerSideId;
                }                
            }
        else
            {
            // Shared texture
            if(iUsedTextures[i].iClientSideId == aClientSideId &&
               iUsedTextures[i].iClientSideManagerId == aClientSideManagerId &&
               iUsedTextures[i].iReleased == EFalse)
                {
                if (iUsedTextures[i].iOwnerId == aClientOwnerId || 
                    aIgnoreOwnerForSharedTextures)
                    {
                    return iUsedTextures[i].iServerSideId;
                    }
                }                                
            }    
        }

    // not found
    return 0;
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
// 
TInt CAlfSrvTextureManager::ReleaseTextureId(
        const TProcessId& aClientOwnerId,
        TInt aClientSideId, 
        TInt aClientSideManagerId )
    {    
    TInt sharedTextureReferenceCount = 0;
    TInt toBeRemovedIndex = KErrNotFound;

    // Look for an existing entry for the id.
    for(TInt i = 0; i < iUsedTextures.Count(); i++)
        {
        if (aClientSideManagerId == 0)
            {
            // Normal texture, "private" to client
            if(iUsedTextures[i].iClientSideId == aClientSideId &&
               iUsedTextures[i].iOwnerId == aClientOwnerId &&
               iUsedTextures[i].iReleased == EFalse)
                {
                toBeRemovedIndex = i;
                break;
                }
            }
        else
            {
            // Shared texture
            if(iUsedTextures[i].iClientSideId == aClientSideId &&
               iUsedTextures[i].iClientSideManagerId == aClientSideManagerId  &&
               iUsedTextures[i].iReleased == EFalse)               
                {
                if (iUsedTextures[i].iOwnerId == aClientOwnerId)
                    {
                    toBeRemovedIndex = i;
                    }
                sharedTextureReferenceCount++;    
                }                                                
            }    
        }

    if (toBeRemovedIndex != KErrNotFound)
        {
        iUsedTextures[toBeRemovedIndex].iReleased = ETrue;
        iUsedTextures[toBeRemovedIndex].iSkinChangeCoordinator = EFalse;
        if (sharedTextureReferenceCount > 0)
            {
            sharedTextureReferenceCount--;    
            }
        }

    return sharedTextureReferenceCount;    
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
// 
TInt CAlfSrvTextureManager::DeleteTextureId(
        const TProcessId& aClientOwnerId,
        TInt aClientSideId, 
        TInt aClientSideManagerId )
    {    
    TInt sharedTextureReferenceCount = 0;
    TInt toBeRemovedIndex = KErrNotFound;

    // Look for an existing entry for the id, it has
    // to be released. 
    for(TInt i = 0; i < iUsedTextures.Count(); i++)
        {
        if (aClientSideManagerId == 0)
            {
            // Normal texture, "private" to client
            if(iUsedTextures[i].iClientSideId == aClientSideId &&
               iUsedTextures[i].iOwnerId == aClientOwnerId &&
               iUsedTextures[i].iReleased)
                {
                toBeRemovedIndex = i;
                break;
                }
            }
        else
            {
            // Shared texture
            if(iUsedTextures[i].iClientSideId == aClientSideId &&
               iUsedTextures[i].iClientSideManagerId == aClientSideManagerId  &&
               iUsedTextures[i].iReleased)               
                {
                if (iUsedTextures[i].iOwnerId == aClientOwnerId)
                    {
                    toBeRemovedIndex = i;
                    }
                sharedTextureReferenceCount++;
                }                                                
            }    
        }

    if (toBeRemovedIndex != KErrNotFound)
        {
        iUsedTextures[toBeRemovedIndex].iDeleted = ETrue;
        iUsedTextures[toBeRemovedIndex].iSkinChangeCoordinator = EFalse;
        if (sharedTextureReferenceCount > 0)
            {
            sharedTextureReferenceCount--;    
            }
        }
    return sharedTextureReferenceCount;
    }

// ---------------------------------------------------------------------------
// After skin change event this function determines if given texture content
// is valid or not.
//
// Return value 0 indicates invalid, other valid.
// ---------------------------------------------------------------------------
// 
TInt CAlfSrvTextureManager::ValidateSkinForTextureId(
        const TProcessId& aClientOwnerId,
        TInt aClientSideId, 
        TInt aClientSideManagerId )
    {
    TInt ownEntryIndex = -1;
    TInt retVal = 0;

    // Look for an existing entry for the id.
    for(TInt i = 0; i < iUsedTextures.Count(); i++)
        {
        if (aClientSideManagerId == 0)
            {
            // Normal texture, "private" to client
            if(iUsedTextures[i].iClientSideId == aClientSideId &&
               iUsedTextures[i].iOwnerId == aClientOwnerId &&
               iUsedTextures[i].iReleased == EFalse)
                {                
                retVal = 0; // Not shared texture, always invalid.
                break;
                }                
            }
        else
            {
            // Shared texture
            if(iUsedTextures[i].iClientSideId == aClientSideId &&
               iUsedTextures[i].iClientSideManagerId == aClientSideManagerId &&
               iUsedTextures[i].iReleased == EFalse)
                {
                if (iUsedTextures[i].iOwnerId == aClientOwnerId)
                    {
                    ownEntryIndex = i;
                    }
                else
                    {
                    if (iUsedTextures[i].iSkinChangeCoordinator)
                        {
                        retVal = 1; // For shared textures, only one entry is invalid to avoid multiple reloads.   
                        break;
                        }                        
                    }    
                }                                
            }    
        }

    if (retVal == 0 && ownEntryIndex != -1)
        {
        // This index becomes the one that handles skin change
        iUsedTextures[ownEntryIndex].iSkinChangeCoordinator = ETrue;    
        }

    return retVal;        
    }

// ---------------------------------------------------------------------------
// Destroyes deleted texture ids.
// ---------------------------------------------------------------------------
// 
TInt CAlfSrvTextureManager::DestroyDeletedTextureIds(const TProcessId& aOwnerId)
    {    
    // Look for an existing entry for the id.
    TInt i = 0;
    for(i = 0; i < iUsedTextures.Count(); i++)
        {
        if (iUsedTextures[i].iDeleted &&
            iUsedTextures[i].iClientSideManagerId == 0 &&
            iUsedTextures[i].iOwnerId == aOwnerId)
            {
            iUsedTextures.Remove(i);
            i = 0;                
            }
        }
        
    return 0;    
    }

// ---------------------------------------------------------------------------
// Adds client item to iClients array.
// ---------------------------------------------------------------------------
// 
void CAlfSrvTextureManager::AddClientL( 
        const TProcessId& aClientOwnerId, 
        MAlfSrvTextureManagerClient& aClient )
    {
    if ( FindClient( aClient ) == KErrNotFound )
        {
        TClientItem item;
        item.iOwnerId = aClientOwnerId;
        item.iClient = &aClient;
        item.iIsMarked = EFalse;
        
        iClients.AppendL( item );
        }
    }

// ---------------------------------------------------------------------------
// Removes client item from iClients array
// ---------------------------------------------------------------------------
// 
void CAlfSrvTextureManager::RemoveClient( MAlfSrvTextureManagerClient& aClient )
    {
    TInt pos = FindClient( aClient );
    if ( pos != KErrNotFound )
        {
        iClients.Remove( pos );
        }
    }
        
CAlfSrvTextureManager::CAlfSrvTextureManager()
    : iNextTextureId( 1 ),
      iUsedTextures( 4 ),
      iClients( KAlfSrvClientsGranularity )
    {
    }

// ---------------------------------------------------------------------------
// Finds client side texture id based on server side id. Returns also
// owner id so that it's possible to find corresponding session instance.
// ---------------------------------------------------------------------------
// 
TBool CAlfSrvTextureManager::FindByTextureId( 
        TInt aServerSideId,
        TInt& aClientSideId,
        TProcessId& aOwnerId ) const
    {
    TBool found = EFalse;
    
    const TInt count = iUsedTextures.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        const TUsedTexture& current = iUsedTextures[ i ];
        if ( !current.iClientSideManagerId && 
             current.iServerSideId == aServerSideId )
            {
            found = ETrue;
            aClientSideId = current.iClientSideId;
            aOwnerId = current.iOwnerId;
            break;
            }
        }   
    
    return found;
    }

// ---------------------------------------------------------------------------
// Finds index of aClient from iClients array
// ---------------------------------------------------------------------------
//
TInt CAlfSrvTextureManager::FindClient( 
        MAlfSrvTextureManagerClient& aClient ) const
    {
    TInt pos = KErrNotFound;
    const TInt count = iClients.Count();
    
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iClients[ i ].iClient == &aClient )
            {
            pos = i;
            break;
            }
        }
        
    return pos;
    }
    
// ---------------------------------------------------------------------------
// Finds client interface corresponding to aOwnerId from iClients array
// ---------------------------------------------------------------------------
//
MAlfSrvTextureManagerClient* CAlfSrvTextureManager::FindClientAndMark( 
        const TProcessId& aOwnerId )
    {
    MAlfSrvTextureManagerClient* result = NULL;
    const TInt count = iClients.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iClients[ i ].iOwnerId == aOwnerId )
            {
            result = iClients[ i ].iClient;
            iClients[ i ].iIsMarked = ETrue;
            break;
            }
        }
    
    return result;
    }

// ---------------------------------------------------------------------------
// Inform marked clients of completion of texture size changes. Clear marks.
// ---------------------------------------------------------------------------
//
void CAlfSrvTextureManager::NotifyTextureSizeChangesCompleted()
    {
    const TInt count = iClients.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iClients[ i ].iIsMarked )
            {
            iClients[ i ].iIsMarked = EFalse;
            iClients[ i ].iClient->TextureSizeChangesCompleted();
            }
        }
    }

// ---------------------------------------------------------------------------
// Calculates index (currently 0 .. 4) from very low .. very high.
// ---------------------------------------------------------------------------
//
TInt CAlfSrvTextureManager::IndexFromParameter( TInt aValue )
    {
    // Note: this method makes assumptions about 
    //       THuiTextureAutoSizeParams.

    // Now make range positive (0 .. 40).
    TInt result = aValue - THuiTextureAutoSizeParams::EVeryLow;
    
    // Transfer to index (0 .. 4).
    const TInt KAlfTextureStepSize = 10;
    result /= KAlfTextureStepSize;
    
    // Now make sure that value is in specified range (0 .. 4).
    return Max( 0, Min( result, 4 ) );
    }

// ---------------------------------------------------------------------------
// Checks if value (=width/height) should be resized.
// ---------------------------------------------------------------------------
//
TBool CAlfSrvTextureManager::CheckIfShouldResize( 
        TInt aCurrentValue, TInt aNewValue, const CHuiTexture& aTexture )
    {
    const THuiTextureAutoSizeParams params = aTexture.AutoSizeParams();

    const TInt delta = Abs( aNewValue - aCurrentValue ); 
    TInt minChangeInPixels = 
        KAlfTextureManagerAutoSizeMinSizeChange[ 
            IndexFromParameter( params.MinSizeChange() ) ];
    
    if ( delta < minChangeInPixels )
        {
        // No need to perform complex calculation - no need to resize.
        return EFalse;
        }
    
    TInt sizeRelativeThreshold = 0;
    if ( aNewValue < aCurrentValue )
        {
        sizeRelativeThreshold = 
            KAlfTetureManagerAutoSizeRelativeThresholdLower[ 
                IndexFromParameter( params.SizeLowerThreshold() ) ];
        }
    else
        {
        sizeRelativeThreshold = 
            KAlfTetureManagerAutoSizeRelativeThresholdUpper[ 
                IndexFromParameter( params.SizeUpperThreshold() ) ];
        }

    TInt downsizeSettleThresholdInPixels = 
            KAlfTextureManagerAutoSizeDownsizeSettleThreshold[
                IndexFromParameter( params.DownsizeSettleThreshold() ) ];

    // So if size is becoming smaller and icon is already small,
    // then resize less aggressively.
    TInt64 threshold = 0;
    
    // Calculate 1 * change
    threshold = aCurrentValue;
    threshold *= sizeRelativeThreshold;
    
    if ( aCurrentValue <= downsizeSettleThresholdInPixels && 
         aNewValue < aCurrentValue )
        {
        // 2 * specified change
        threshold *= 2;
        }
    
    // 256 = 1.f, so normalize back to integers.
    threshold /= 256;
    
    // Handle overflow
    if ( threshold > KMaxTInt )
        {
        threshold = KMaxTInt;
        }
        
    const TInt requiredChange = I64INT( threshold );
    const TBool result = delta >= requiredChange;
    return result;
    }

// ---------------------------------------------------------------------------
// Checks if resizing is needed.
// ---------------------------------------------------------------------------
//
TBool CAlfSrvTextureManager::CheckIfResizeNeeded(
        const TSize& aCurrentSize,
        const TSize& aNewSize,
        const CHuiTexture& aTexture,
        TInt& aDeliveryPriority )
    {
    // If new size would become zero size, then don't resize.
    if ( !aNewSize.iWidth || !aNewSize.iHeight )
        {
        return EFalse;
        }

    // Otherwise check if width or height has changed sufficiently.
    // (also, one possibility would be to check area)
    const TBool result = 
        CheckIfShouldResize( aCurrentSize.iWidth, aNewSize.iWidth, 
            aTexture ) ||
        CheckIfShouldResize( aCurrentSize.iHeight, aNewSize.iHeight, 
            aTexture );

    if ( result )
        {
        if ( aNewSize.iWidth > aCurrentSize.iWidth ||
             aNewSize.iHeight > aCurrentSize.iHeight )
            {
            // If it's becoming larger, let's change immediately.
            aDeliveryPriority = 0;
            }
        else
            {
            // If it's becoming smaller, then it's not so urgent.
            aDeliveryPriority = 1;
            }
        }
        
    return result;
    }

// ---------------------------------------------------------------------------
// Rounds real value to integer. Also HWA can be taken into account.
// ---------------------------------------------------------------------------
//
inline TInt CAlfSrvTextureManager::RoundToInteger( const TReal32& aValue ) const
    {
    TInt value = HUI_ROUND_FLOAT_TO_INT( aValue );
    if ( value < 0 )
        {
        value = 0;
        }
        
    if ( iIsRendererHWA && 
         ( value > KAlfSrvTextureManagerAutoSizeLowBoundary ) )
        {
        // Select appropriate rounding precision.
        TInt roundTo = KAlfSrvTextureManagerAutoSizeMiddleRoundTo;
        if ( value > KAlfSrvTextureManagerAutoSizeHighBoundary )
            {
            roundTo = KAlfSrvTextureManagerAutoSizeHighRoundTo;
            }
            
        const TInt remainder = value & ( roundTo - 1 );
        if ( remainder )
            {
            // Round to next multiple of 'roundTo'.
            value -= remainder;
            value += roundTo;
            }
        }
    
    return value;
    }

// ---------------------------------------------------------------------------
// Calculates new size if resizing is needed.
// ---------------------------------------------------------------------------
//
TBool CAlfSrvTextureManager::CalculateNewSizeIfResizeNeeded( 
        const CHuiTexture& aTexture, 
        const THuiRealSize& aPreferredSize,
        const TSize& aCurrentSize,
        TSize& aNewSize,
        TInt& aDeliveryPriority ) const
    {
    // Calculate preferred size.
    const TSize preferredSize( 
        RoundToInteger( aPreferredSize.iWidth ),
        RoundToInteger( aPreferredSize.iHeight ) );

    // First, if texture does not have content, then we must resize and
    // let's then use highest priority.
    TInt deliveryPriority = -1;
    TBool result = !aTexture.HasContent();
        
    if ( !result )
        {
        result = 
            CheckIfResizeNeeded( aCurrentSize, preferredSize, aTexture,
                deliveryPriority );
        }
    
    if ( result )
        {
        aNewSize = preferredSize;
        aDeliveryPriority = deliveryPriority;
        }
        
    return result;
    }

// ---------------------------------------------------------------------------
// Handles a set of texture preferred properties.
// ---------------------------------------------------------------------------
//
void CAlfSrvTextureManager::HandlePreferredSizeChanged( 
        MAlfSrvTextureManagerClient& aClient, 
        TInt aClientSideTextureId,
        const CHuiTexture& aTexture, 
        const THuiRealSize& aPreferredSize )
    {    
    // Get information about current size from client.
    TSize currentSize;
    TBool hasDelivered;
    TBool ok = aClient.GetTextureSize( 
        aClientSideTextureId, currentSize, hasDelivered );
    
    // Determine new size.
    TSize newSize;
    
    TBool currentSizeIsTextureSize = EFalse;
    if ( !( ok && hasDelivered ) )
        {
        currentSizeIsTextureSize = ETrue;
        currentSize = aTexture.Size();
        }

    TInt deliveryPriority = 0;
    
    if ( CalculateNewSizeIfResizeNeeded( 
            aTexture, 
            aPreferredSize, 
            currentSize, 
            newSize,
            deliveryPriority ) )
        {
        // Update size to client
        aClient.SetTextureSize( 
            aClientSideTextureId, newSize, deliveryPriority );
        }
    else        
        {
        // If current texture size is acceptable, then 
        // no need to report new size.
        if ( currentSizeIsTextureSize )
            {
            // Instead, now we can remove the whole texture size.
            aClient.RemoveTextureSize( aClientSideTextureId );
            }
        }
    }

// ---------------------------------------------------------------------------
// Handles one by one texture preferred properties.
// ---------------------------------------------------------------------------
//
TBool CAlfSrvTextureManager::PreferredSizeChanged(
        const CHuiTexture& aTexture, 
        TInt aServerSideTextureId, 
        const THuiRealSize& aPreferredSize )
    {
    TInt clientSideId = 0;
    TProcessId ownerId;
    TBool result = EFalse;

    // Find corresponding server side texture id
    if ( FindByTextureId( aServerSideTextureId, clientSideId, ownerId ) )
        {
        // Find client interface
        MAlfSrvTextureManagerClient* client = FindClientAndMark( ownerId );
        if ( client )
            {
            // Decide size
            HandlePreferredSizeChanged( *client, clientSideId, aTexture, 
                aPreferredSize );
            result = ETrue; 
            }
        }
    return result;
    }

// ---------------------------------------------------------------------------
// Inform all clients of completion. This one also clears marks.
// ---------------------------------------------------------------------------
//
void CAlfSrvTextureManager::PreferredSizeReportCompleted()
    {
    NotifyTextureSizeChangesCompleted();
    }
