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
* Description:   Group of textures.
*
*/



#ifndef C_ALFTEXTUREGROUP_H
#define C_ALFTEXTUREGROUP_H

#include <coemain.h>
#include <alf/alftexturemanager.h>

class CAlfEnv;
class CAlfTexture;
class CAlfTextureGroup;

/**
 * Provides callback methods for getting notifications when texture manager
 * finishes asynchronous texture loads for a group of textures.
 *
 * The class willing to receive these events must implement the methods
 * of this interface and register itself as an observer to a texture group.
 *
 */
class MAlfTextureGroupLoadingCompletedObserver
    {
public:
    /**
     * Called to notify the observer that loading of a texture group has
     * been completed.
     * @param aTextureGroup Group of textures that has been loaded.
     * @param aErrorCode KErrNone if the loading of all textures was successful, 
     * otherwise one of the system-wide error codes indicating reason why the first failed 
     * texture loading failed.
     * @note One should not commence loading of a new texture in this callback method. Also
     * one should not make any changes to a texture group e.g. add or remove textures
     * in this method.
     */
    virtual void TextureGroupLoadingCompleted(CAlfTextureGroup& aTextureGroup, TInt aErrorCode) = 0;

    };


/**
 * This class is a utility class that can be used to group textures into logical group and
 * then performing operations for the whole group at once.
 *
 * Example of utilizing texture group load observer service.  
 * @code
 * CMyObserver::InitL()
 *    {
 *    // Precondition is that textures has been created at this point but are still possibly 
 *    // being loaded in the background... 
 *
 *    // Create a texturegroup and add textures into it
 *    iTextureGroup = CAlfTextureGroup::NewL(iEnv);
 *    iTextureGroup.AddTextureL(texture1);
 *    iTextureGroup.AddTextureL(texture2);
 *    iTextureGroup.AddTextureL(texture3);
 *
 *    if (iTextureGroup.IsLoadingCompleted())
 *        {
 *        // Already all textures have content. We are done, no need to observe.
 *        TextureGroupLoadingCompleted(iTextureGroup, KErrNone);
 *        }
 *    else
 *        {
 *        // Not all textures have their content ready, we start observing and get notification later.
 *        iTextureGroup->AddLoadObserverL(*this);
 *        }   
 *     ...
 *     ...
 *    }
 *
 * CMyObserver::TextureGroupLoadingCompleted(CAlfTextureGroup& aTextureGroup, TInt aErrorCode)
 *     {
 *     if (&aTextureGroup = iTextureGroup)
 *         { 
 *         // Do needed things here, e.g. show all visuals that use grouped textures   
 *         iLayout->SetOpacity(TAlfTimedValue(1.f, 300));     
 *         }
 *     }
 *  @endcode
 *  @lib alfclient.lib
 *  @since S60 v3.2
 *
 */
NONSHARABLE_CLASS( CAlfTextureGroup ) : public CBase
    {
public:

    /**
     * Static factory method. 
     * @param aEnv Environment
     */
    IMPORT_C static CAlfTextureGroup* NewL(CAlfEnv& aEnv);

    /**
     * Static factory method. 
     * @param aEnv Environment
     */
    IMPORT_C static CAlfTextureGroup* NewLC(CAlfEnv& aEnv);

   /**
     * Destructor
     */
    IMPORT_C virtual ~CAlfTextureGroup();

   /**
     * Adds a texture into group. Ownership is not transferred.
     * If added texture is deleted, it should be removed from the group 
     * before deletion happens.
     * @param aTexture Texture to be added into group. 
     */
    IMPORT_C void AddTextureL(CAlfTexture& aTexture);

   /**
     * Removes a texture from group.
     * @param aTexture Texture to be removed from group. 
     */
    IMPORT_C void RemoveTexture(CAlfTexture& aTexture);

    /**
     * Checks if all textures in the group have content.
     * @return ETrue if group contains textures, all textures have content
     * and are bigger than zero size.
     * otherwise returns EFalse. 
     *
     */
    IMPORT_C TBool IsLoadingCompleted() const;

    /**
     * Returns the number of textures in the group.
     *
     * @return Texture count.
     */
    IMPORT_C TInt Count() const;
    
    /**
     * Returns a texture.
     *
     * @param aIndex  Index of the texture.
     *
     * @return  Reference to a texture.
     */
     IMPORT_C CAlfTexture& Texture( TInt aIndex ) const;

    /**
     * Adds texture group observer.
     * @param aObserver Observer to be added.
     */
    IMPORT_C void AddLoadObserverL(MAlfTextureGroupLoadingCompletedObserver& aObserver);

    /**
     * Removes texture group observer, this must be done at latest when observer is being deleted.
     * @param aObserver Observer to be removed.
     */
    IMPORT_C void RemoveLoadObserver(MAlfTextureGroupLoadingCompletedObserver& aObserver);

    /**
     * This method can be used to temporarely enable/disable notifications to observers
     * when e.g. texture group content is being modified. By default notifications are
     * enabaled.
     * @param aEnable If ETrue notifications are delivered normally, otherwise notifications
     * are not delivered.
     */
    IMPORT_C void EnableLoadObservers(TBool aEnable = ETrue);

public:  
    
    /**
     * Called by the toolkit when one texture has been loaded
     */
    void NotifyTextureAvailability(CAlfTexture& aTexture, TInt aErrorCode);    
    
private:

   /**
     * Constructor.
     */
    CAlfTextureGroup();
    
   /**
     * Constructor.
     */
    void ConstructL(CAlfEnv& aEnv);
    
   /**
     * Checks if given texture is part of the group. 
     */
    TBool IsTextureInGroup(CAlfTexture& aTexture) const;

   /**
     * Notifies observers.
     */
    void NotifyObservers();

   /**
     * Registers this object as loading observer to the given texturemanager.
     */
    void RegisterAsTextureLoadObserverL(CAlfTextureManager* aManager);

   /**
     * Unregisters this object as loading observer from the given texturemanager.
     */
    void UnRegisterAsTextureLoadObserver(CAlfTextureManager* aManager);

private:

	/* Private data. Owned */
    struct TPrivateData;    
    TPrivateData* iData;
    };

#endif