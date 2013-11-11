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
* Description:   Server side texture manager
*
*/



#ifndef C_ALFSRVTEXTUREMANAGER_H
#define C_ALFSRVTEXTUREMANAGER_H

#include <e32base.h>
#include <uiacceltk/HuiTextureManager.h>

class CHuiEnv;
class CAlfAppSrvSession;
class MAlfSrvTextureManagerClient;

/**
 *  Server side texture manager.
 *
 *  This texture manager is responsible for managing texture ids and
 *  handling auto size textures.
 *  
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CAlfSrvTextureManager ) 
    : public CBase, 
      private MHuiTextureAutoSizeObserver
    {
public:

    /**
     * Two-phased constructor.
     * @return new instance of CAlfSrvTextureManager.
     */
    static CAlfSrvTextureManager* NewL();
   
    /**
     * Destructor.
     */
    virtual ~CAlfSrvTextureManager();

    /**
     * This will be called when environment has been created.
     * @param aEnv reference to environment.
     */
    void HandleEnvCreateL( CHuiEnv& aEnv );

    /**
     * This will be called just before environment is deleted.
     */
    void HandleEnvToBeDeleted();

    //
    // Texture Id management
    //

    /**
     * Creates new texture id.
     * @param aClientOwnerId client owner id.
     * @param aClientSideId client side id.
     * @param aClientSideManagerId client side manager id.
     * @param aReUseAnyDeletedId ETrue if reuse is allowed.
     * @return texture id.
     */
    TInt CreateTextureId(
        const TProcessId& aClientOwnerId, 
        TInt aClientSideId, 
        TInt aClientSideManagerId, 
        TBool aReUseAnyDeletedId  );

    /**
     * Finds server side id based in client side id.
     * @param aClientOwnerId client owner id.
     * @param aClientSideId client side id.
     * @param aClientSideManagerId client side manager id.
     * @param aIgnoreOwnerForSharedTextures ignore owner.
     * @return texture id.
     */
    TInt ExistingTextureId(
        const TProcessId& aClientOwnerId, 
        TInt aClientSideId, 
        TInt aClientSideManagerId, 
        TBool aIgnoreOwnerForSharedTextures = EFalse );

    /**
     * Releases texture id.
     * @param aClientOwnerId client owner id.
     * @param aClientSideId client side id.
     * @param aClientSideManagerId client side manager id.
     * @return reference count.
     */
    TInt ReleaseTextureId(
        const TProcessId& aClientOwnerId,
        TInt aClientSideId, 
        TInt aClientSideManagerId );

    /**
     * Deletes texture id.
     * @param aClientOwnerId client owner id.
     * @param aClientSideId client side id.
     * @param aClientSideManagerId client side manager id.
     * @return reference count.
     */
    TInt DeleteTextureId(
        const TProcessId& aClientOwnerId,
        TInt aClientSideId, 
        TInt aClientSideManagerId );

    /**
     * Validates skin for texture id.
     * @param aClientOwnerId client owner id.
     * @param aClientSideId client side id.
     * @param aClientSideManagerId client side manager id.
     * @return 1 if skin change coordinator already set, 0 otherwise.
     */
    TInt ValidateSkinForTextureId(
        const TProcessId& aClientOwnerId,
        TInt aClientSideId, 
        TInt aClientSideManagerId );
    
    /**
     * Destroyes deleted texture ids.
     * @param aClientOwnerId client owner id.
     * @return 0.
     */
    TInt DestroyDeletedTextureIds(const TProcessId& aClientOwnerId); 

    //
    // Auto size texture
    //

    /**
     * Adds client for auto size textures handling.
     * Note: Clients must remove itself upon destruction using 
     *       @c RemoveClient method.
     * @param aClientOwnerId client owner id.
     * @param aClient reference to client interface.
     */
    void AddClientL( 
        const TProcessId& aClientOwnerId, 
        MAlfSrvTextureManagerClient& aClient );

    /**
     * Removes client.
     * @param aClient client.
     */    
    void RemoveClient( MAlfSrvTextureManagerClient& aClient );
        
private:

    /**
     * Constructor.
     */
    CAlfSrvTextureManager();

    /**
     * Finds client side texture id based on server side id.
     * Note that shared textures are ignored.
     * 
     * @param aServerSideId server side id.
     * @param aClientSideId this is updated to contain client side id.
     * @param aOwnerUid this is updated to contain owner uid.
     * @return ETrue if found, EFalse otherwise.
     */
    TBool FindByTextureId( 
        TInt aServerSideId,
        TInt& aClientSideId,
        TProcessId& aOwnerId ) const;

    /**
     * Finds index of client.
     * @param aClient client to look for.
     * @return index or KErrNotFound if not found.
     */
    TInt FindClient( MAlfSrvTextureManagerClient& aClient ) const;
    
    /**
     * Finds client interface based on owner id. This one also
     * marks corresponding client so that completion of auto size 
     * changes is informed.
     * 
     * @param aOwnerId owner id.
     * @return client interface, NULL if not found.
     */
    MAlfSrvTextureManagerClient* FindClientAndMark( 
        const TProcessId& aOwnerId );

    /**
     * Notifies marked clients of completion of auto size events.
     * This also clears marks.
     */
    void NotifyTextureSizeChangesCompleted();

    /**
     * Calculates index from value.
     * @param aValue value from THuiTextureAutoSizeParams.
     * @return index.
     */
    static TInt IndexFromParameter( TInt aValue );

    /**
     * Checks if resize is needed.
     * @param aCurrentValue current height/width.
     * @param aNewValue new height/width.
     * @param aTexture reference to texture.
     * @return ETrue if resize is needed, EFalse otherwise.
     */
    static TBool CheckIfShouldResize( 
        TInt aCurrentValue, TInt aNewValue, const CHuiTexture& aTexture );

    /**
     * Checks if resize is needed.
     * @param aCurrentSize current size.
     * @param aNewSize new size.
     * @param aTexture reference to texture.
     * @param aDeliveryPriority this will contain delivery priority.
     * @return ETrue if resize is needed, EFalse otherwise.
     */
    static TBool CheckIfResizeNeeded(
        const TSize& aCurrentSize, const TSize& aNewSize,
        const CHuiTexture& aTexture, TInt& aDeliveryPriority );

    /**
     * Rounds real value to integer.
     * @param aValue value to be rounded.
     * @return result value.
     */
    inline TInt RoundToInteger( const TReal32& aValue ) const;
    
    /**
     * Calculates new size if resize is needed.
     * @param aTexture reference to texture.
     * @param aPreferredSize preferred size.
     * @param aCurrentSize current size.
     * @param aNewSize this will contain new size.
     * @param aDeliveryPriority this will contain delivery priority.
     * @return ETrue upon success, EFalse otherwise.
     */
    TBool CalculateNewSizeIfResizeNeeded( 
        const CHuiTexture& aTexture, 
        const THuiRealSize& aPreferredSize,
        const TSize& aCurrentSize,
        TSize& aNewSize,
        TInt& aDeliveryPriority ) const;

    /**
     * This one decides new size for current texture.
     * @param aClient client interface.
     * @param aClientSideTextureId client side texture id.
     * @param aTexture texture.
     * @param aPreferredSize preferred size.
     */
    void HandlePreferredSizeChanged( 
        MAlfSrvTextureManagerClient& aClient, 
        TInt aClientSideTextureId, 
        const CHuiTexture& aTexture, 
        const THuiRealSize& aPreferredSize );

// from base class MHuiTextureAutoSizeObserver

    /**
     * Handles preferred texture properties changes.
     * @param aTexture texture.
     * @param aServerSideTextureId server side texture id.
     * @param aPreferredSize preferred size.
     */
    virtual TBool PreferredSizeChanged(const CHuiTexture& aTexture, 
        TInt aServerSideTextureId, 
        const THuiRealSize& aPreferredSize );

    /**
     * Handles completion of preferred size changes (for now).
     */
    virtual void PreferredSizeReportCompleted();

private: // data

    /**
     * Pointer to environment.
     * Not own.
     */
    CHuiEnv* iEnv;

    /**
     * Global value which is used to assign unique texture ids 
     * in the serverside.
     */
    TInt iNextTextureId;

    /**
     * Structure containing information about a texture.
     */
    struct TUsedTexture 
        {
        TUsedTexture()
            :iClientSideId(0), 
             iClientSideManagerId(0), 
             iServerSideId(0), 
             iOwnerId(), 
             iReleased(EFalse), 
             iDeleted(EFalse),
             iSkinChangeCoordinator(EFalse){}
        
        /** The client side texture id */
        TInt   iClientSideId;
        /** client side texture manager of the texture */
        TInt32 iClientSideManagerId;                
        /** The server side texture id */
        TInt   iServerSideId;
        /** Owner process of the texture */
        TProcessId iOwnerId;
        /** Released */
        TBool iReleased;
        /** Deleted */
        TBool iDeleted;
        /** Skin change coordinator for shared textures, only one entry with same id has this set to ETrue */
        TBool iSkinChangeCoordinator;
        };
        
    /** 
     * Array of used textures. Items are not deleted from this array when 
     * application exists because hitchcock texture manager does not delete
     * textures either (only unloads).
     * This array is used when texture is re-created again.
     * Own.
     */
    RArray<TUsedTexture> iUsedTextures;

    /**
     * Structure defining mapping from owner id to
     * client interface used for auto size textures.
     */
    struct TClientItem
        {
        TProcessId iOwnerId;
        MAlfSrvTextureManagerClient* iClient;
        TBool iIsMarked;
        };
        
    /**
     * Mapping from texture owner id to client interface.
     * Own.
     */
    RArray< TClientItem > iClients;

    /**
     * Boolean flag indicating if renderer is OpenGL.
     */
    TBool iIsRendererHWA;
    };

#endif // C_CLASSNAME_H
