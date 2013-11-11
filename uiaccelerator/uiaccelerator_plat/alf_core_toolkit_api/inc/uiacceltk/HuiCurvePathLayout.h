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
* Description:   Definition for CHuiCurvePathLayout. Combines a layout visual 
*                and a curve path. The path is rescaled as the layout's size changes.
*
*/



#ifndef __HUICURVEPATHLAYOUT_H__
#define __HUICURVEPATHLAYOUT_H__


#include <e32base.h>
#include <uiacceltk/HuiCurvePath.h>
#include <uiacceltk/HuiLayout.h>
#include <uiacceltk/HuiTimedValue.h>


/* Forward declarations */
class CHuiEnv;
class CHuiLayout;


/**
 * CHuiCurvePathLayout combines a layout visual and a curve path instance. 
 * Using a curve path layout it is easy to create a curve path that scales
 * to the size of the layout.
 *
 * The curve path layout itself can be used as a mapping function, because it 
 * implements the MHuiMappingFunction interface. 
 
 * To support legacy applications, if the metrics unit type is EHuiUnitPixel, 
 * the curve path owned by the layout is scaled to the size of the layout visual.
 * The path itself should be defined using relative coordinates in the range 0...1000.
 *
 * For example, to make the positions of child visuals (or any other visuals)
 * follow the layout's curve path, call the THuiTimedValue::SetFunction() method
 * for the position of the visual(s):
 *
 * <pre>iChild->Pos().SetFunction(iCurvePathLayout);</pre>
 */
NONSHARABLE_CLASS(CHuiCurvePathLayout): public CHuiLayout, public MHuiMappingFunction
    {
public:

    /* Constructors and destructor. */

    /**
     * Construct a new curve path layout and give its ownership to a control.
     *
     * @param aOwnerControl  Owner control.
     * @param aParentLayout  Parent layout for the new curve path layout.
     */
    IMPORT_C static CHuiCurvePathLayout* AddNewL(CHuiControl& aOwnerControl, 
                                                 CHuiLayout* aParentLayout = 0);

    /**
     * Constructor.
     *
     * @param aOwner  Owner control.
     */
    CHuiCurvePathLayout(MHuiVisualOwner& aOwner);

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    ~CHuiCurvePathLayout();


    /* Methods. */

    /**
     * Returns the curve path instance of the layout.
     *
     * @return Curve path.
     */
    IMPORT_C CHuiCurvePath& CurvePath();
    
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
        aName = _L("CHuiCurvePathLayout");
        }
    
    
    /* Mapping function impl. */
    
    TReal32 MapValue(TReal32 aValue, TInt aMode) const __SOFTFP;
    
    TBool MappingFunctionChanged() const;
    
    void MappingFunctionClearChanged();
    
private:    

    /* Private types. */
    

private:

    CHuiCurvePath* iPath;
    
    };


#endif  // __HUICURVEPATHLAYOUT_H__
