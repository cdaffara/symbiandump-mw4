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
* Description:   Text style manager
*
*/


#include <avkon.hrh> // @todo: include proper header file
#include <AknUtils.h>
#include <aknappui.h>
#include <AknLayoutFont.h>
#include <gdi.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include "alf/alftextstylemanager.h"
#include "uiacceltk/HuiFont.h"
#include "alf/alfbitmapprovider.h"
#include "alf/alfconstants.h"
#include "alf/alftextstyle.h"
#include "alf/alfgencomponent.h"
#include "alf/alftextvisual.h"

#include <uiacceltk/HuiUtil.h>
#ifdef ALF_RASTER_TEXT

NONSHARABLE_CLASS(CAlfS60TextStyle): public CAlfTextStyle 
{
public:
    /* Constructors and destructor. */

    /** @beginAPI */

    /**
     * Default constructor. Create a new S60 specific text style.
     *
     * @param aParent   A handle id set into the THuiFont by the Texture manager or skin
     * @param aFontSpec Symbian OS Font Specification to store
     */
    static CAlfS60TextStyle* NewL(CAlfEnv& aEnv, TInt aId, TInt aImplementationId, const TDesC8& aConstructionParams)
        {
        CAlfS60TextStyle* me = new (ELeave) CAlfS60TextStyle();
        CleanupStack::PushL(me);
        me->ConstructL(aEnv, aId, aImplementationId, aConstructionParams);
        CleanupStack::Pop();
        me->Init();
        return me;
        }

    void Init()
        {
        if (!CCoeEnv::Static())
            {
            return;
            }
		
        const CAknLayoutFont* font = AknLayoutUtils::LayoutFontFromId(FontStyleId(), NULL);
        THuiFont huiFont(FontStyleId(), font->FontSpecInTwips());
    
        // transfer the text pane height between the different font specs
        TAknFontSpecification aknFs = font->FontSpecification();
        huiFont.SetTextPaneHeight(aknFs.TextPaneHeight());
        SetUnderline(EFalse);
        SetStrikeThrough(EFalse);
        SetTextColor(KRgbBlack);
        SetFont(&huiFont);
        }
    };

NONSHARABLE_CLASS(CAlfTextStyleHolder):public CActive
    {
    public:
    CAlfTextStyleHolder(CAlfTextStyle* aTextStyle)
        :CActive(EPriorityNormal), iTextStyle(aTextStyle)
        {
        CActiveScheduler::Add(this);
        }
    
    ~CAlfTextStyleHolder()
        {
        Cancel();
        delete iTextStyle;
        iVisuals.Reset();
        }
    
    void RemoveReference(CAlfTextVisual* aUser)
        {
        for (TInt i = iVisuals.Count() -1; i >= 0; i--)
            {
            if (aUser == iVisuals[i])
                {
                iVisuals.Remove(i);
                }
            }
        }
    
    void RefreshMesh()
        {
        if (!IsActive())
            {
            SetActive();
            TRequestStatus* sptr = &iStatus;
            User::RequestComplete(sptr, KErrNone);
            }
        }

    void DoRefreshMesh()
        {
        if( IsActive() )
            {
            Cancel();
            }
        
        for (TInt p = iVisuals.Count()-1; p >= 0; p--)
            {
            iVisuals[p]->PrepareForUpdateMesh();
            }
    
        for (TInt i = iVisuals.Count()-1; i >= 0; i--)
            {
            iVisuals[i]->UpdateMesh(ETrue);
            }
        }
    
    void ReleaseMesh()
        {
        for (TInt i = iVisuals.Count()-1; i >= 0; i--)
            {
            iVisuals[i]->ReleaseMesh();
            }        

        }
        
    void RunL()
        {
        if( iStatus.Int() != KErrCancel )
            {
            DoRefreshMesh();
            }
        }
    
    void DoCancel()
        {
        }
    
    TInt AddReference(CAlfTextVisual* aUser)
        {
        return iVisuals.InsertInAddressOrder(aUser);
        }
    
    CAlfTextStyle* iTextStyle;
    RPointerArray<CAlfTextVisual> iVisuals;
    };

#else
const TUint KPreconfiguredTextStyleCount = 10;
#endif

// Private structure
struct CAlfTextStyleManager::TPrivateData
    {
	/** Current Alf environment. */
    CAlfEnv* iEnv;  // Not owned.
    /** Array of alf text style objects. */
#ifdef ALF_RASTER_TEXT
    RPointerArray<CAlfTextStyleHolder> iTextStyles; // Owned.
#else	
    RPointerArray<CAlfTextStyle> iTextStyles; // Owned.
#endif
    };

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfTextStyleManager::CAlfTextStyleManager()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfTextStyleManager::~CAlfTextStyleManager()
    {
	// Free the alf text styles array.
	if(iData)
		{
		iData->iTextStyles.ResetAndDestroy();
		}
	
	// Delete the private data.
	delete iData;
	iData = NULL;
    }

// ---------------------------------------------------------------------------
// 2-phased constructor.
// ---------------------------------------------------------------------------
//
CAlfTextStyleManager* CAlfTextStyleManager::NewL(CAlfEnv& aEnv)
    {
    CAlfTextStyleManager* self = CAlfTextStyleManager::NewLC(aEnv);
    CleanupStack::Pop( self );
    return self;    
    }

// ---------------------------------------------------------------------------
// 2-phased constructor. Object stays on the stack.
// ---------------------------------------------------------------------------
//
CAlfTextStyleManager* CAlfTextStyleManager::NewLC(CAlfEnv& aEnv)
    {
    CAlfTextStyleManager* self = new( ELeave ) CAlfTextStyleManager;
    CleanupStack::PushL( self );
    self->ConstructL(aEnv);
    return self;    
    }

// ---------------------------------------------------------------------------
// Creates new platform style
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyleManager::CreatePlatformTextStyleL(
    TInt aFontStyleId, TInt aParentId)
    {	
    return DoCreatePlatformTextStyleL(aFontStyleId, aParentId, EAlfPlatformTextStyleCreate);
    }

// ---------------------------------------------------------------------------
// Creates new platform style
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyleManager::CreatePlatformTextStyleL(
    TInt aParentId)
    {	
    CAlfTextStyle* parentStyle = TextStyle(aParentId);
    TInt parentStyleFontId = parentStyle->FontStyleId();
    return DoCreatePlatformTextStyleL(parentStyleFontId, aParentId, EAlfPlatformTextStyleCreate);
    }

// ---------------------------------------------------------------------------
// Return text style for given ID
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTextStyle* CAlfTextStyleManager::TextStyle(TInt aId)
    {
#ifdef ALF_RASTER_TEXT
    if(aId >= 0 && aId < iData->iTextStyles.Count())
	    {
	    __ASSERT_DEBUG( iData->iTextStyles[aId]->iTextStyle, USER_INVARIANT() );
    	return iData->iTextStyles[aId]->iTextStyle;
	    }

    return iData->iTextStyles[EAlfTextStyleNormal]->iTextStyle;	

#else
    if(aId >= 0 && aId < iData->iTextStyles.Count())
	    {
	    __ASSERT_DEBUG( iData->iTextStyles[aId], USER_INVARIANT() );
    	return iData->iTextStyles[aId];
	    }

    return iData->iTextStyles[EAlfTextStyleNormal];	
#endif
    }

CAlfTextStyle* CAlfTextStyleManager::SwitchTextStyle(TInt aStyle, CAlfTextVisual* aVisual)
    {
#ifdef ALF_RASTER_TEXT
    CAlfTextStyle* newStyle = iData->iTextStyles[EAlfTextStyleNormal]->iTextStyle;
    
    if(aStyle >= 0 && aStyle < iData->iTextStyles.Count())
        {
        newStyle = iData->iTextStyles[aStyle]->iTextStyle;
        }
     if (iData->iTextStyles[aStyle]->AddReference(aVisual) == KErrNone)
         {
         if(newStyle != iData->iTextStyles[aVisual->TextStyle()]->iTextStyle )
             {
             iData->iTextStyles[aVisual->TextStyle()]->RemoveReference(aVisual);                       
             }
         }
    return newStyle;
#else
	return 0;
#endif
    }

void CAlfTextStyleManager::Unregister(CAlfTextVisual* aVisual)
    {
#ifdef ALF_RASTER_TEXT
    TInt style = aVisual->TextStyle();
    if(style >= 0 && style < iData->iTextStyles.Count())
        {
        iData->iTextStyles[style]->RemoveReference(aVisual);                       
        }

#else
    return;
#endif
    }

void CAlfTextStyleManager::RefreshVisuals(TInt aStyle)
    { // called by text style it self so queue request as such
#ifdef ALF_RASTER_TEXT
    if (aStyle >= iData->iTextStyles.Count())
        return;
    iData->iTextStyles[aStyle]->RefreshMesh();
#else
    return;
#endif    
    }


void CAlfTextStyleManager::ReleaseAllVisuals()
    { // called by CAlfEnv::Release to get all the text visuals to destroy their textures
      // Those textures can not recreate themselves because missing bitmapprovider but need 
      // to be rebuild by AlfTextVisual when client calls CAlfEnv::RestoreL
#ifdef ALF_RASTER_TEXT
    for(TInt i = 0 ; i < iData->iTextStyles.Count() ; i++ )
        {
        iData->iTextStyles[i]->ReleaseMesh();
        }
#else
    return;
#endif    
    }

void CAlfTextStyleManager::RefreshAllVisuals()
    { // called by CAlfEnv::RestoreL to get all the text visuals to rebuild themselves
#ifdef ALF_RASTER_TEXT
    for(TInt i = 0 ; i < iData->iTextStyles.Count() ; i++ )
        {
        iData->iTextStyles[i]->DoRefreshMesh();
        }
#else
    return;
#endif
    }



// ---------------------------------------------------------------------------
// Creates a new text style object that is identical with the given source
// text style.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyleManager::CopyTextStyleL(TInt aSourceId)
    {
#ifdef ALF_RASTER_TEXT
    CAlfTextStyle* sourceStyle = TextStyle(aSourceId);
    const TInt freeSlot = FindFreeSlotIndex();
        
    // Create alf text style object.
    CAlfTextStyle* textStyle = 
        CAlfTextStyle::NewL(
            *iData->iEnv, 
            (freeSlot != KErrNotFound ) ? freeSlot : iData->iTextStyles.Count(), 
            sourceStyle->FontStyleId(), 
            KNullDesC8);
    
    // Set the parent id of the text style.
    textStyle->SetParentId(sourceStyle->ParentId());

    // Store the font style id of the text style
    textStyle->SetFontStyleId(sourceStyle->FontStyleId());
    
    CleanupStack::PushL(textStyle);
    CAlfTextStyleHolder* holder = new (ELeave) CAlfTextStyleHolder(textStyle);
    CleanupStack::PushL(holder);

    if ( freeSlot != KErrNotFound )
        {
        iData->iTextStyles[freeSlot] = holder;
        }
    else
        {
        // Append the specified text style into the array of styles.
        iData->iTextStyles.AppendL(holder);  
        }

    CleanupStack::Pop(2);

#else
    CAlfTextStyle* sourceStyle = TextStyle(aSourceId);
    TInt parentId = sourceStyle->ParentId();

	// Create construction parameters for alf text style object.
    TInt params(sourceStyle->Comms()->Identifier());
    TPckgC<TInt> paramBuf(params);
    
    const TInt freeSlot = FindFreeSlotIndex();
        
    // Create alf text style object.
    CAlfTextStyle* textStyle = 
        CAlfTextStyle::NewL(
            *iData->iEnv, 
            (freeSlot != KErrNotFound ) ? freeSlot : iData->iTextStyles.Count(), 
            EAlfPlatformTextStyleCopy, 
            paramBuf);
    
    // Set the parent id of the text style.
    textStyle->SetParentId(parentId);

    if ( freeSlot != KErrNotFound )
        {
        iData->iTextStyles[freeSlot] = textStyle;
        }
    else
        {
        // Append the specified text style into the array of styles.
        iData->iTextStyles.AppendL(textStyle);	
        }	

#endif
    // Return the id of the created text style.
    return textStyle->Id();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTextStyleManager::DeleteTextStyle(TInt aId)
    {
    if ( aId >= iData->iTextStyles.Count() )
        {
        return KErrArgument;
        }
    
    // Cannot delete platform styles from an application.
#ifdef ALF_RASTER_TEXT
    CAlfTextStyle* deletedStyle = iData->iTextStyles[aId]->iTextStyle;
    
    if (dynamic_cast<CAlfS60TextStyle*>(deletedStyle)) // is platform style
#else
    
    CAlfTextStyle* deletedStyle = iData->iTextStyles[aId];

    if ( aId >= 0 && aId < KPreconfiguredTextStyleCount )
#endif
        {
        return KErrAccessDenied;
        }
    
    if ( deletedStyle )
        {
        delete deletedStyle;
        iData->iTextStyles[aId] = NULL; // NULL the slot for later usage.
        }
    else
        {
        return KErrArgument;
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
void CAlfTextStyleManager::ConstructL(CAlfEnv& aEnv)
    {
    iData = new (ELeave) TPrivateData;
    
    // Fill data
    iData->iEnv = &aEnv;
    
    // Construct CAlfTextStyle representations from preconfigured text styles
    ConstructPreconfiguredStylesL();
    }

// ---------------------------------------------------------------------------
// Constructs CAlfTextStyle representations from build in preconfigured
// text styles
// ---------------------------------------------------------------------------
//
void CAlfTextStyleManager::ConstructPreconfiguredStylesL()
	{
#ifdef ALF_RASTER_TEXT
    // create default parent for all styles for all
    CreatePlatformTextStyleL( EAknLogicalFontPrimaryFont, -1 );    
    
    // initialize platform styles
    CAlfTextStyle* style = 0;

 	// ENormalItalicFont / no EAlfTextStyleXxx def
    TInt normalItalicStyleId = CreatePlatformTextStyleL( EAknLogicalFontPrimaryFont, EAlfTextStyleNormal );
    style = TextStyle(normalItalicStyleId);
	style->SetItalic(ETrue);
	
 	// EAlfTextStyleMenuItem
    CreatePlatformTextStyleL( EAknLogicalFontPrimaryFont, EAlfTextStyleNormal );
	
 	// ESmallFont / EHuiTextStyleSmall
    TInt smallStyleId = CreatePlatformTextStyleL( EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
	
 	// ESmallFont EHuiTextStyleMenuSmall
    CreatePlatformTextStyleL( EAknLogicalFontSecondaryFont, smallStyleId );
	
 	// ENormalBoldFont / EAlfTextStyleMenuTitle 
    TInt normalBoldStyleId = CreatePlatformTextStyleL( EAknLogicalFontPrimaryFont, EAlfTextStyleNormal );
    style = TextStyle(normalBoldStyleId);
	style->SetBold(ETrue);
	
 	// ELargeFont / EAlfTextStyleLarge
    CreatePlatformTextStyleL( EAknLogicalFontTitleFont, EAlfTextStyleNormal );
	
    // ESoftkeyFont / EAlfTextStyleSoftkey
    /*TInt softkeyStyleId =*/ CreatePlatformTextStyleL( EAknLogicalFontTitleFont, EAlfTextStyleNormal );
	
    // EAlfTextStyleSupplement
    CreatePlatformTextStyleL( EAknLogicalFontPrimaryFont, EAlfTextStyleNormal );
	
    // EAlfTextStyleTitle
    /*iLastBuiltInStyleId =*/ CreatePlatformTextStyleL( EAknLogicalFontTitleFont, EAlfTextStyleNormal );

#else
	for(TUint i = 0; i < KPreconfiguredTextStyleCount; ++i)
		{
		// Create construction parameters for alf text style object.
	    TPckgC<TInt> paramBuf(iData->iTextStyles.Count());    
	    
		// Create the new text style.
	    CAlfTextStyle* textStyle = 
	        CAlfTextStyle::NewL(
	            *iData->iEnv, 
	            iData->iTextStyles.Count(), 
	            EAlfPreconfiguredTextStyleCreate, 
	            paramBuf);
	    	
	    // Append the specified text style into the array of styles.
	    iData->iTextStyles.AppendL(textStyle);		
		}
#endif
	}

// ---------------------------------------------------------------------------
// Finds a free slot
// ---------------------------------------------------------------------------
//
TInt CAlfTextStyleManager::FindFreeSlotIndex() const
    {
    TInt index = KErrNotFound;
    for ( TInt i = iData->iTextStyles.Count() -1 ; i >= 0; i-- )
        {
        if ( !iData->iTextStyles[i] )
            {
            index = i;
            break;
            }
        }
    return index;
    }


// ---------------------------------------------------------------------------
// Internal implementation to create a platform style.
// ---------------------------------------------------------------------------
//
TInt CAlfTextStyleManager::DoCreatePlatformTextStyleL(TInt aFontStyleId, TInt aParentId, TInt aImplementationId)
    {
#ifdef ALF_RASTER_TEXT
    // Create construction parameters for alf text style object.
    CAlfTextStyle* parentStyle = 0;
    if (iData->iTextStyles.Count())
        {
        parentStyle = TextStyle(aParentId);
        }
    
    const TInt freeSlot = FindFreeSlotIndex();
        
    // Create alf text style object.
    CAlfS60TextStyle* textStyle = 
        CAlfS60TextStyle::NewL(
            *iData->iEnv, 
            (freeSlot != KErrNotFound ) ? freeSlot : iData->iTextStyles.Count(), 
            aFontStyleId, 
            KNullDesC8);
    
    if ( parentStyle )
        {// Set the parent id of the text style.
        textStyle->SetParentId(parentStyle->Id());
        }
    
    CleanupStack::PushL(textStyle);
    CAlfTextStyleHolder* holder = new (ELeave) CAlfTextStyleHolder(textStyle);
    CleanupStack::PushL(holder);

    if ( freeSlot != KErrNotFound )
        {
        iData->iTextStyles[freeSlot] = holder;
        }
    else
        {
        // Append the specified text style into the array of styles.
        iData->iTextStyles.AppendL(holder);	
        }

    CleanupStack::Pop(2);

    textStyle->SetManager(this);

#else
    // Create construction parameters for alf text style object.
    CAlfTextStyle* parentStyle = TextStyle(aParentId);
    TInt2 params(aFontStyleId, parentStyle->Comms()->Identifier());
    TPckgC<TInt2> paramBuf(params);
    
    const TInt freeSlot = FindFreeSlotIndex();
        
    // Create alf text style object.
    CAlfTextStyle* textStyle = 
        CAlfTextStyle::NewL(
            *iData->iEnv, 
            (freeSlot != KErrNotFound ) ? freeSlot : iData->iTextStyles.Count(), 
            aImplementationId, 
            paramBuf);
    
    // Set the parent id of the text style.
    textStyle->SetParentId(parentStyle->Id());
    
    // Store the font style id of the text style
    textStyle->SetFontStyleId(aFontStyleId);
    
    if ( freeSlot != KErrNotFound )
        {
        iData->iTextStyles[freeSlot] = textStyle;
        }
    else
        {
        // Append the specified text style into the array of styles.
        iData->iTextStyles.AppendL(textStyle);	
        }

#endif
    // Return the id of the created text style.
    return textStyle->Id();            
    }
