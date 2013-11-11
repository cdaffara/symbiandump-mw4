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




#include "alf/alftexturegroup.h"
#include "alf/alfenv.h"
#include "alf/alftexture.h"


NONSHARABLE_CLASS( CAlfInternalTextureLoadObserver ) : public CBase, public MAlfTextureLoadingCompletedObserver
    {
public:

    /**
     * Static factory method. 
     * @param aEnv Environment
     */
    static CAlfInternalTextureLoadObserver* NewL(CAlfTextureGroup& aTextureGroup);

    /**
     * Static factory method. 
     * @param aEnv Environment
     */
    static CAlfInternalTextureLoadObserver* NewLC(CAlfTextureGroup& aTextureGroup);

   /**
     * Destructor
     */
    virtual ~CAlfInternalTextureLoadObserver();

   /**
     * From MAlfTextureLoadingCompletedObserver
     */
    void TextureLoadingCompleted(CAlfTexture& aTexture,
                                TInt aTextureId,
                                TInt aErrorCode);

private:

   /**
     * Constructor.
     */
    CAlfInternalTextureLoadObserver();
    
   /**
     * Constructor.
     */
    void ConstructL(CAlfTextureGroup& aTextureGroup);

private:
        
    CAlfTextureGroup* iTextureGroup; // Not owned   
    };

// Private structure
struct CAlfTextureGroup::TPrivateData
    {
    /** Environment */
    CAlfEnv* iEnv;                	

    /** Queue of texture group loading observers. */
    RPointerArray<MAlfTextureGroupLoadingCompletedObserver> iLoadObserverQueue;

    /** Queue of textures of this group. Textures are not owned by this class */
    RPointerArray<CAlfTexture> iTextures;
    
    /** Error */ 
    TInt iError;
    
    /** Flag to enable/disable observing notifications */
    TBool iObservingNotificationsEnabled;
    
    /** Observer for texture laodings */
    CAlfInternalTextureLoadObserver* iTextureLoadObserver;
    };




// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextureGroup::CAlfTextureGroup() 
	{
		
	}
	
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextureGroup::~CAlfTextureGroup()
    {
    if ( iData )
        {
        while (Count())
            {
            RemoveTexture(Texture(Count()-1));
            }
    
        iData->iLoadObserverQueue.Close();	
	iData->iTextures.Close();		    
	    
	delete iData->iTextureLoadObserver;
	iData->iTextureLoadObserver = NULL;
	}	

    delete iData;
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextureGroup* CAlfTextureGroup::NewL(CAlfEnv& aEnv)
	{
	CAlfTextureGroup* self = CAlfTextureGroup::NewLC(aEnv);        
    CleanupStack::Pop( self );
    return self;		
	}
	

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextureGroup* CAlfTextureGroup::NewLC(CAlfEnv& aEnv)
	{
    CAlfTextureGroup* self = new( ELeave ) CAlfTextureGroup();
    CleanupStack::PushL( self );
    self->ConstructL(aEnv);
    return self;	
	}

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextureGroup::ConstructL(CAlfEnv& aEnv)
	{
    iData = new (ELeave) TPrivateData;    
    // Zero all data
    iData->iEnv = &aEnv;
    iData->iError = KErrNone;
    iData->iObservingNotificationsEnabled = ETrue;
    iData->iTextureLoadObserver = NULL;
    
    iData->iTextureLoadObserver = CAlfInternalTextureLoadObserver::NewL(*this);    
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextureGroup::AddTextureL(CAlfTexture& aTexture)
    {
    if (!IsTextureInGroup(aTexture))
        {            
        TBool addLoadObserver = ETrue;
        for(TInt i = 0; i < Count(); i++)
          	{
        	if (iData->iTextures[i]->TextureManager() == aTexture.TextureManager())
        		{
                addLoadObserver = EFalse;
        		break;
        		}								                       
            }

        iData->iTextures.Append(&aTexture);

        if (addLoadObserver)
            {
            RegisterAsTextureLoadObserverL(aTexture.TextureManager());                        
            }
        }            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextureGroup::RemoveTexture(CAlfTexture& aTexture)
    {
    if (IsTextureInGroup(aTexture))
        {            
        for(TInt i = 0; i < Count(); i++)
          	{
        	if (iData->iTextures[i] == &aTexture)
        		{
        		iData->iTextures.Remove(i);
        		iData->iTextures.Compress();
        		break;
        		}								                       
            }                

        TBool removeLoadObserver = ETrue;
        for(TInt i = 0; i < Count(); i++)
          	{
        	if (iData->iTextures[i]->TextureManager() == aTexture.TextureManager())
        		{
                removeLoadObserver = EFalse;
        		break;
        		}								                       
            }
        
        if (removeLoadObserver)
            {
            UnRegisterAsTextureLoadObserver(aTexture.TextureManager());    
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTextureGroup::IsLoadingCompleted() const
    {
    for (TInt i=0; i<Count();i++)
        {
        if ((Texture(i).Size() == TSize(0,0)) || !Texture(i).HasContent())
            {
            return EFalse;    
            }
        }
    
    if (Count() == 0)
        {
        return EFalse;            
        }
        
    return ETrue;   
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextureGroup::Count() const
    {
    return iData->iTextures.Count();    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTexture& CAlfTextureGroup::Texture( TInt aIndex ) const
    {
    return *iData->iTextures[aIndex];    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextureGroup::AddLoadObserverL(MAlfTextureGroupLoadingCompletedObserver& aObserver)
    {
    iData->iLoadObserverQueue.Append(&aObserver);            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextureGroup::RemoveLoadObserver(MAlfTextureGroupLoadingCompletedObserver& aObserver)
    {
    TInt count = iData->iLoadObserverQueue.Count();
    for(TInt i = 0; i < count; i++)
      	{
    	if (iData->iLoadObserverQueue[i] == &aObserver)
    		{
    		iData->iLoadObserverQueue.Remove(i);
    		iData->iLoadObserverQueue.Compress();
    		break;
    		}								                       
        }                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextureGroup::EnableLoadObservers(TBool aEnable)
    {
    iData->iObservingNotificationsEnabled = aEnable;    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfTextureGroup::NotifyTextureAvailability(CAlfTexture& aTexture,
                                TInt aErrorCode)
    {
    if (IsTextureInGroup(aTexture))
        {
        if (aErrorCode != KErrNone)
            {
            iData->iError = aErrorCode;    
            }
        
        if (IsLoadingCompleted() && iData->iObservingNotificationsEnabled)
            {
            NotifyObservers();    
            }            
        }            
    }
      
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CAlfTextureGroup::IsTextureInGroup(CAlfTexture& aTexture) const
    {
    TInt count = iData->iTextures.Count();
    for(TInt i = 0; i < Count(); i++)
      	{
    	if (iData->iTextures[i] == &aTexture)
    		{
    		return ETrue;
    		}								                       
        }                
    return EFalse;    
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfTextureGroup::NotifyObservers()
    {
    TInt count = iData->iLoadObserverQueue.Count();
    for(TInt i = 0; i < count; i++)
      	{
    	iData->iLoadObserverQueue[i]->TextureGroupLoadingCompleted(*this,iData->iError);
        }                        
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfTextureGroup::RegisterAsTextureLoadObserverL(CAlfTextureManager* aManager)
    {
    if (aManager)
        {
        aManager->AddLoadObserverL(iData->iTextureLoadObserver);    
        }        
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfTextureGroup::UnRegisterAsTextureLoadObserver(CAlfTextureManager* aManager)
    {
    if (aManager)
        {
        aManager->RemoveLoadObserver(iData->iTextureLoadObserver);    
        }                
    }
    


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAlfInternalTextureLoadObserver* CAlfInternalTextureLoadObserver::NewL(CAlfTextureGroup& aTextureGroup)
    {
	CAlfInternalTextureLoadObserver* self = CAlfInternalTextureLoadObserver::NewLC(aTextureGroup);        
    CleanupStack::Pop( self );
    return self;		        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAlfInternalTextureLoadObserver* CAlfInternalTextureLoadObserver::NewLC(CAlfTextureGroup& aTextureGroup)
    {
    CAlfInternalTextureLoadObserver* self = new( ELeave ) CAlfInternalTextureLoadObserver();
    CleanupStack::PushL( self );
    self->ConstructL(aTextureGroup);
    return self;	            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAlfInternalTextureLoadObserver::~CAlfInternalTextureLoadObserver()
    {        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAlfInternalTextureLoadObserver::CAlfInternalTextureLoadObserver()
    {        
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfInternalTextureLoadObserver::ConstructL(CAlfTextureGroup& aTextureGroup)
    {
    iTextureGroup = &aTextureGroup;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfInternalTextureLoadObserver::TextureLoadingCompleted(CAlfTexture& aTexture,
                                TInt /*aTextureId*/,
                                TInt aErrorCode)
    {
    iTextureGroup->NotifyTextureAvailability(aTexture, aErrorCode);        
    }

