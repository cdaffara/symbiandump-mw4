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
* Description:   LCT Text visual
*
*/



#ifndef C_ALFLCTTEXTVISUAL_H
#define C_ALFLCTTEXTVISUAL_H

class TAknsItemID;
class TRgb;

#include <alf/alftextvisual.h>

class TAknLayoutHierarchyComponentHandle;

/**
 *  Text visual for text drawing. Supports setting style from LCT layout data.
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
class CAlfLCTTextVisual : public CAlfTextVisual
    {

public:

    /**
     * Constructor, which gives ownership to the control.
     *
     * @param aOwnerControl The control
     * @param aParentLayout If given, the parent layout.
     * @return New instance. Ownership NOT transreffed (owned by control)
     */
    IMPORT_C static CAlfLCTTextVisual* AddNewL(CAlfControl& aOwnerControl,
                                            CAlfLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    IMPORT_C CAlfLCTTextVisual();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner);

    /**
     * Destructor.
     */
    IMPORT_C ~CAlfLCTTextVisual();

    /**
     * Sets the text pane layout of the text visual. Note that the actual
     * text pane position should be called by setting an anchor on the containing layout.
     *
     * @param aComponentHandle reference to the layout data that corresponds to the text visual.
     */
    IMPORT_C void SetTextPane(TAknLayoutHierarchyComponentHandle& aComponentHandle);

    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void RemoveAndDestroyAllD();
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0);
    IMPORT_C CAlfVisual* FindTag(const TDesC8& aTag);

protected:
    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void DoRemoveAndDestroyAllD();
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

                           
private:

    struct TLCTTextVisualPrivateData;
    TLCTTextVisualPrivateData* iLCTTextVisualData;
    };


#endif // C_ALFLCTTEXTVISUAL_H
