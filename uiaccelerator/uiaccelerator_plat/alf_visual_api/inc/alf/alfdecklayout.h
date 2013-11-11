/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Deck layout definition.
*
*/



#ifndef C_ALFDECKLAYOUT_H
#define C_ALFDECKLAYOUT_H

#include <alf/alflayout.h>

/**
 * Deck layout
 * Deck layout is a very simple layout that overlays all its children to 
 * match the layout's own rectangle. A suitable analogy would be a deck 
 * of cards. 
 *  Usage:
 *  @code
 *  // Create a deck layout to overlay two text visuals
 *   CAlfDeckLayout* deckLayout = CAlfDeckLayout::AddNewL( control, NULL );
 * 
 * // The text visual that is aligned to the top edge of the deck layout.
 *  TextVisual* topText = CAlfTextVisual::AddNewL( control, deck );
 *  
 *  topText->SetTextL( _L"Top Aligned" );
 *  topText->SetAlign( EAlfAlignHLeft, EAlfAlignVTop );
 * 
 * // The text visual that is aligned to the bottom edge of the deck layout.
 *  TextVisual* bottomText = CAlfTextVisual::AddNewL( control, deck );
 *  topText->SetTextL( _L"Bottom Aligned" );
 *  bottomText->SetAlign( EAlfAlignHCenter, EAlfAlignVBottom );
 * 
 *  @endcode
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
class CAlfDeckLayout : public CAlfLayout
    {

public:

    /**
     * Two-phased constructor, which attach new object into the control.
     * @param aOwnerControl Control that owns this new obejct.
     * @param aParentLayout If given, the new object is set as child.
     */
    IMPORT_C static CAlfDeckLayout* AddNewL(CAlfControl& aOwnerControl,
                                            CAlfLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    IMPORT_C CAlfDeckLayout();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner);

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAlfDeckLayout();

    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void RemoveAndDestroyAllD();
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0);
    IMPORT_C CAlfVisual* FindTag(const TDesC8& aTag);
    /** 
     * From CAlfLayout
     * @see CAlfLayout
     */
    IMPORT_C TAlfXYMetric BaseUnit() const; 


protected:
    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void DoRemoveAndDestroyAllD();
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

                                
private:

    // Private data structure. Owned.
    struct TDeckLayoutPrivateData;
    TDeckLayoutPrivateData* iDeckLayoutData;

    };



#endif // C_ALFDECKLAYOUT_H
