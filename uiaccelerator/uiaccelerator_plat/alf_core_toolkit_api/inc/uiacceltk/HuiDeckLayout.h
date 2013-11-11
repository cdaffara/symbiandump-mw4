/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition for CHuiDeckLayout. Deck layout is a very simple 
*                layout that overlays all its children to match the layout's 
*                own rectangle. Think of it as a deck of cards.
*
*/



#ifndef __HUIDECKLAYOUT_H__
#define __HUIDECKLAYOUT_H__


#include <e32base.h>
#include <uiacceltk/HuiLayout.h>
#include <uiacceltk/HuiTimedValue.h>


/* Forward declarations */
class CHuiEnv;


/**
 * CHuiDeckLayout is a very simple layout that overlays all its children to 
 * match the layout's own rectangle. A suitable analogy would be a deck of 
 * cards.
 */
NONSHARABLE_CLASS(CHuiDeckLayout) : public CHuiLayout
    {
public:

    /* Constructors and destructor. */

    /**
     * Construct a new deck layout and give its ownership to a control.
     *
     * @param aOwnerControl  Owner control.
     * @param aParentLayout  Parent layout for the new deck layout.
     */
    IMPORT_C static CHuiDeckLayout* AddNewL(CHuiControl& aOwnerControl, 
                                            CHuiLayout* aParentLayout = 0);

    /**
     * Constructor.
     *
     * @param aOwner  Owner control.
     */
    CHuiDeckLayout(MHuiVisualOwner& aOwner);

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    IMPORT_C ~CHuiDeckLayout();


    /* Methods. */

    /**
     * Overridden version of size changing updates the layout of child visuals.
     *
     * @param aSize  New size for the layout.
     * @param aTransitionTime  Time for the transition to new size to take 
     *               place.
     */
    void SetSize(const THuiRealSize& aSize, TInt aTransitionTime=0);
    
    TBool ChildSize(TInt aOrdinal, TSize& aSize);
    
    TBool ChildPos(TInt aOrdinal, TPoint& aPos);

    TInt ChildRect(TInt aOrdinal, THuiRealRect& aRect);        

    void GetClassName(TDes& aName) const
        {
        aName = _L("CHuiDeckLayout");
        }    
    };


#endif  // __HUIDECKLAYOUT_H__
