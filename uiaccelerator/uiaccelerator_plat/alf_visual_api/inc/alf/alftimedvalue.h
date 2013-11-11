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
* Description:   Timed values
*
*/



#ifndef __ALF_TIMED_VALUE__
#define __ALF_TIMED_VALUE__

#include <e32std.h>

// Todo: is there need to expose these
enum TAlfTimedValueFlags
    {
    EAlfValueFlagsNone = 0,
    EAlfValueNowChanged = 1,
    EAlfValueTargetChanged = 2,
    EAlfInterpolationStyleChanged =4,
    EAlfMappingFunctionChanged = 8,
    EAlfSpeedChanged = 16,    
    EAlfValueFlagsAll = 0xffffffff
    };

/**
 * Timed value interpolation styles.
 *
 * @see To implement custom interpolation styles, you can use
 * mapping functions.
 *
 * @see MAlfMappingFunction
 * @see TAlfTimedValue::SetFunction()
 */
enum TAlfInterpolationStyle
    {
    /** No interpolation is done. The target value becomes effective
        in a single step. */
    EAlfTimedValueStyleUseSystemDefault,

    EAlfTimedValueStyleNone,

    /** Use linear interpolation between the current value and the
        target value. */
    EAlfTimedValueStyleLinear,

    /** Use a sine wave for approximating preservation of momentum. */
    EAlfTimedValueStyleSineWave,

    /** Quarter of a sine wave for decelerating movement. */
    EAlfTimedValueStyleDecelerate,

    /** Quarter of a sine wave for accelerating movement. */
    EAlfTimedValueStyleAccelerate,

    /** Sine wave or decelerating style based on when the target value
        is defined. */
    EAlfTimedValueStyleAdaptive,

    /** Combination of linear and deceleration. */
    EAlfTimedValueStyleImpulse,

    EAlfTimedValueStyleLast
    };

/**
 * 
 * Timed value class.
 *
 * Timed values are able to interpolate their value independently based on
 * time. The interpolation can be configured to follow a curve instead of
 * being linear. Timed value does not restrict the value units but interpolates
 * on any given real value. Mapping function can be used that maps the interpolated
 * values to some other real value space. If no mapping function is specified an
 * identity mapping is used. Current values and target values of timed value can be
 * set through class methods. Delays for timed value to achieve the target value can
 * be given in the Set - method. Timed value supports number of different interpolation
 * styles.
 *
 * @code
 *   // Here are some examples how the timed value can be used.
 *   // In the examples, the opacity of the visual is used as
 *   // an example, but any of the timed values work the same.
 * 
 *   // #1 New variable - getters are not really relevant
 *   TAlfTimedValue opacity;
 *   opacity->SetTarget( 1, 500 );
 *   visual->SetOpacity( opacity ); // changes only the target
 *   
 *   // 2# Data fetching
 *   // a) preferred
 *   const TAlfTimedValue& opacity = visual->Opacity();
 *   TReal32 valueNow = opacity->ValueNow();
 *   TReal32 target = opacity->Target();
 *   // b) 
 *   TReal32 valueNow = visual->Opacity().ValueNow();
 *   TReal32 target = visual->Opacity().Target();
 *   
 *   // 3# Data fetching and value update
 *   TAlfTimedValue opacity( visual->Opacity() );
 *   TReal32 target = opacity->Target();
 *   opacity->SetTarget( target*2, 500 );
 *   visual->SetOpacity( opacity );
 *   
 *   // 4# Use the same timed value for other places:
 *   // a)
 *   const TAlfTimedValue& visualOpacity = visual->Opacity();
 *   layout->SetOpacity( visualOpacity );
 *   // b)
 *   TAlfTimedValue visualOpacity( visual->Opacity() );
 *   visualOpacity->SetTarget( 1, 500 );
 *   layout->SetOpacity( visualOpacity );
 * @endcode
 *
 * @lib alfclient.lib
 * @since S60 v3.2
 **/
class TAlfTimedValue
    {
    public:

    /**
     * Constructor. Enables creation of timed value with no explicitly set initial value.
     * The timed value is set to zero.
     *
     */
    IMPORT_C TAlfTimedValue();
    
    /**
     * Constructor. Enables initial value setting.
     *
     * @param aInitialValue  Initial value.
     *
     */
    IMPORT_C TAlfTimedValue(TReal32 aInitialValue) __SOFTFP; 

    /**
     * Constructor. Enables target value setting.
     *
     * @param aTargetValue  Target value.
     * @param aTime When the target value will be in effect. Transition duration in milliseconds.
     *
     */
    IMPORT_C TAlfTimedValue(TReal32 aTargetValue, TInt aTime) __SOFTFP; 
    
    /**
     * Set a new current value Change becomes effective immediately.
     *
     * @param aValueNow  New currentvalue.
     */
    IMPORT_C void SetValueNow(const TReal32& aValueNow) __SOFTFP;

    /**
     * Gets current value.
     *
     * @return Current value.
     */
    IMPORT_C TReal32 ValueNow() const __SOFTFP;
    
    /**
     * Set a new target value that becomes effective after a delay.
     * @see SetTargetWithSpeed()
     *
     * @param aValue            New target value.
     * @param aTime   Time duration after the target value will be in effect. Transition duration in milliseconds.
     */
    IMPORT_C void SetTarget(const TReal32& aTarget, TInt aTime) __SOFTFP; 

    /**
     * Sets a new target value. The transition time depends on the speed.
     * @see SetTarget()
     *
     * @param aTarget          New target value.
     * @param aUnitsPerSecond  Speed of change.
     */
    IMPORT_C void SetTargetWithSpeed(TReal32 aTarget, TReal32 aUnitsPerSecond) __SOFTFP;

    /**
     * Gets target value.
     *
     * @return Target value.
     */
    IMPORT_C TReal32 Target() const __SOFTFP;

    /**
     * Gets duration of the transition.
     *
     * @return Duration time in milliseconds..
     */
    IMPORT_C TInt TimeToTargetinMilliSeconds() const;

    /**
     * Gets speed of the transition in units per second if it has been set
     * with SetTargetWithSpeed.
     *
     * @return Speed of the transition in units per second.
     */
    IMPORT_C TReal32 Speed() const __SOFTFP;
        
    /**
     * Sets the interpolation style of the timed value.
     *
     * @param aStyle  Interpolation style used to reach target value.
     */
    IMPORT_C void SetStyle(TAlfInterpolationStyle aStyle);
    
    /**
     * Sets identifier of a mapping function that affects the timed value. 
     *
     * @param aIdentifier Identifier of a function that will map the current value to required
     * number space.
     */
    IMPORT_C void SetMappingFunctionIdentifier(TInt aIdentifier);    

    /**
     * Gets identifier of a mapping function that affects the timed value. 
     *
     * @return aIdentifier Identifier of a function that will map the current value to required
     * number space.
     */
    IMPORT_C TInt MappingFunctionIdentifier() const;
            
    /**
     * Gets flags.
     *
     * @return Flags.
     */
    IMPORT_C TInt& Flags();
        
    /**
     * Gets the interpolation style of the timed value.
     *
     * @return Interpolation style used to reach target value.
     */
    IMPORT_C TAlfInterpolationStyle Style();
        
private:
    TReal32 iValueNow;
    TReal32 iValueTarget;
    TInt iTimeToTarget;
    TInt iInterpolationStyle;
    TInt iMappingFunctionIdentifier;
    TReal32 iSpeed;
    TInt iFlags;

private:
    TInt iSpare1;
    TInt iSpare2;
    }; 

struct TAlfRealPoint
    {
    TAlfRealPoint():iX(0),iY(0){}
    TAlfRealPoint(const TReal32& aX, const TReal32& aY):iX(aX),iY(aY){}
    TAlfRealPoint(const TPoint& aPoint):iX(aPoint.iX),iY(aPoint.iY){}
        
    inline operator TPoint() const
        {
        return TPoint((TInt)iX, (TInt)iY);
        }

    inline TSize AsSize() const
        {
        return TSize((TInt)iX, (TInt)iY);
        }

    inline TAlfRealPoint operator-(const TAlfRealPoint& aPoint) const
        {
        return TAlfRealPoint(iX - aPoint.iX, iY - aPoint.iY);
        }

    inline TAlfRealPoint operator+(const TAlfRealPoint& aPoint) const
        {
        return TAlfRealPoint(iX + aPoint.iX, iY + aPoint.iY);
        }

    inline TAlfRealPoint& operator+=(const TAlfRealPoint& aPoint)
        {
        iX += aPoint.iX;
        iY += aPoint.iY;
        return *this;
        }

    inline TAlfRealPoint& operator-=(const TAlfRealPoint& aPoint)
        {
        iX -= aPoint.iX;
        iY -= aPoint.iY;
        return *this;
        }
    
    inline TBool operator==(const TAlfRealPoint& aPoint) const 
        {
        if ( Abs(iX-aPoint.iX) < 0.0001 && Abs(iY-aPoint.iY) < 0.0001 )
            {
            return ETrue;
            }
        
        return EFalse;
        }

    TReal32 iX;
    TReal32 iY;    

private:
    TInt iSpare1;
    TInt iSpare2;
    };

struct TAlfRealSize
    {
    TAlfRealSize()
        : iWidth(0.f), iHeight(0.f) {}
        
    inline TAlfRealSize(TReal32 aWidth, TReal32 aHeight)
            : iWidth(aWidth), iHeight(aHeight)
        {
        }
        
    inline TAlfRealSize(const TSize& aSize)
            : iWidth((TReal32)aSize.iWidth), iHeight((TReal32)aSize.iHeight)
        {
        }
        
    inline TAlfRealSize(const TAlfRealPoint& aPoint)
            : iWidth( aPoint.iX ), iHeight( aPoint.iY )
        {
        }
        
    inline operator TSize() const
        {
        return TSize((TInt)iWidth, (TInt)iHeight);
        }
        
public:    

    /** Width. */    
    TReal32 iWidth;

    /** Height. */
    TReal32 iHeight;

private:
    TInt iSpare1;
    TInt iSpare2;
    };


struct TAlfRealRect
    {
    TAlfRealRect(){}
    TAlfRealRect(const TRect& aRect):iTl(aRect.iTl),iBr(aRect.iBr){}
    TAlfRealRect(const TAlfRealPoint& aTl, const TAlfRealPoint& aBr):iTl(aTl),iBr(aBr){}
    inline operator TRect() const
        {
        return TRect((TPoint)iTl, (TPoint)iBr);
        }

    inline TReal32 Width() const
        {
        return iBr.iX - iTl.iX;
        }
        
    inline TReal32 Height() const
        {
        return iBr.iY - iTl.iY;
        }
        
    inline TAlfRealPoint TopRight() const
        {
        return TAlfRealPoint(iBr.iX, iTl.iY);
        }

    inline TAlfRealPoint BottomLeft() const
        {
        return TAlfRealPoint(iTl.iX, iBr.iY);
        }

    inline TAlfRealSize Size() const
        {
        return TAlfRealSize(Width(), Height());
        }


    inline TAlfRealPoint Center() const
        {
        return iTl + TAlfRealPoint(Width()/2, Height()/2);
        }


    inline void Grow(TReal32 aX, TReal32 aY)
        {
        iTl.iX -= aX;
        iTl.iY -= aY;
        iBr.iX += aX;
        iBr.iY += aY;
        }
        
    inline void Shrink(TReal32 aX, TReal32 aY)
        {
        iTl.iX += aX;
        iTl.iY += aY;
        iBr.iX -= aX;
        iBr.iY -= aY;
        }

    inline void Shrink(const TPoint& aPoint)
        {
        iTl.iX += aPoint.iX;
        iTl.iY += aPoint.iY;
        iBr.iX -= aPoint.iX;
        iBr.iY -= aPoint.iY;
        }
        
    inline void Shrink(const TAlfRealPoint& aPoint)
        {
        iTl.iX += aPoint.iX;
        iTl.iY += aPoint.iY;
        iBr.iX -= aPoint.iX;
        iBr.iY -= aPoint.iY;
        }
        
    inline void Shrink(const TAlfRealRect& aRect)
        {
        iTl.iX += aRect.iTl.iX;
        iTl.iY += aRect.iTl.iY;
        iBr.iX -= aRect.iBr.iX;
        iBr.iY -= aRect.iBr.iY;
        }
        
    inline void Move(TReal32 aDx, TReal32 aDy)
        {
        iTl.iX += aDx;
        iTl.iY += aDy;
        iBr.iX += aDx;
        iBr.iY += aDy;
        }

    TAlfRealPoint iTl;
    TAlfRealPoint iBr;    

private:
    TInt iSpare1;
    TInt iSpare2;
    };

class TAlfTimedPoint
    {
public:    
    /**
     * Default constructor.
     */
    IMPORT_C TAlfTimedPoint();

    /**
     * Constructor that sets default values.
     */
    IMPORT_C TAlfTimedPoint(TReal32 aX, TReal32 aY) __SOFTFP;

    /**
     * Constructor. Enables target value setting.
     *
     * @param aXTarget  Target X value.
     * @param aYTarget  Target Y value.
     * @param aTransitionTime When the target value will be in effect. Transition duration in milliseconds.
     *
     */
    IMPORT_C TAlfTimedPoint(TReal32 aXTarget, TReal32 aYTarget, TInt aTransitionTime) __SOFTFP;

    /**
     * Sets the interpolation style of both components of the point.
     *
     * @param aStyle  Timed value interpolation style.
     */
    IMPORT_C void SetStyle(TAlfInterpolationStyle aStyle);

    /**
     * Sets identifier of a mapping function that affects the timed value for both 
     * components of the point. 
     *
     * @param aIdentifier Identifier of a function that will map the current value to required
     * number space.
     */
    IMPORT_C void SetMappingFunctionIdentifier(TInt aIdentifier);    
 
     /**
     * Sets the target for the timed point.
     *
     * @param aPoint           Target point.
     * @param aTransitionTime  Duration for reaching the target.
     */
    IMPORT_C void SetTarget(const TAlfRealPoint& aPoint, TInt aTransitionTime = 0);

    /**
     * Sets the target value of both components of the timed point.
     *
     * @param aValue           New target value.
     * @param aTransitionTime  Duration for reaching the target.
     */
    IMPORT_C void SetTarget(TReal32 aValue, TInt aTransitionTime = 0) __SOFTFP;

    /**
     * Sets the target value for the timed point, with a transition that is
     * done using a specific speed.
     *
     * @param aPoint  Target point.
     * @param aUnitsPerSecond  Units per second during the transition.
     */
    IMPORT_C void SetTargetWithSpeed(const TAlfRealPoint& aPoint, TReal32 aUnitsPerSecond) __SOFTFP;

    /**
     * Sets the target value of both components of the timed point.
     *
     * @param aValue  New target value.
     * @param aUnitsPerSecond  Speed of change.
     */
    IMPORT_C void SetTargetWithSpeed(TReal32 aValue, TReal32 aUnitsPerSecond) __SOFTFP;
    
    /**
     * Returns the current value of the point.
     *
     * @return  Current values as a regular TAlfRealPoint.
     */
    inline TAlfRealPoint ValueNow() const
        {
        return TAlfRealPoint(iX.ValueNow(), iY.ValueNow());
        }
        
    /**
     * Returns the target value of the point.
     *
     * @return  Target as a TAlfRealPoint.
     */
    inline TAlfRealPoint Target()  const
        {
        return TAlfRealPoint(iX.Target(), iY.Target());
        }
    
    /**
     * Returns the transition time value of the point.
     *
     * @return  Transition times as a TAlfRealPoint.
     */
    inline TAlfRealPoint TimeToTargetinMilliSeconds()  const
        {
        return TAlfRealPoint(iX.TimeToTargetinMilliSeconds(), iY.TimeToTargetinMilliSeconds());
        }

    /**
     * Returns the current interpolatad value of the point rounded to nearest
     * integer values. Note that the TAlfTimedPoint values are
     * floating-point.
     *
     * @see ValueNow() For returning the floating-point current value.
     *
     * @return  Current values as a regular TPoint.
     */
    IMPORT_C TPoint IntValueNow() const;

    /**
     * Returns the target value of the point rounded to nearest
     * integer values. Note that the TAlfTimedPoint values are
     * floating-point.
     *
     * @see Target() For returning the floating-point values.
     *
     * @return Target values as a normal TPoint - target value rounded to
     * nearest integer.
     */
    IMPORT_C TPoint IntTarget() const;

public:
    
    TAlfTimedValue iX;
    TAlfTimedValue iY;

private:
    TInt iSpare1;
    TInt iSpare2;
    };




#endif