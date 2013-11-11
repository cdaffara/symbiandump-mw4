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
* Description:   Definition of THuiTimedValue. THuiTimedValue is value that can 
*                change based on time.
*
*/



#ifndef __HUITIMEDVALUE_H__
#define __HUITIMEDVALUE_H__


#include <e32base.h>
#include <e32std.h>


/**
 * Mapping function interface. Timed values use this interface for mapping
 * the return value of a timed value evaluation. Can be used to define
 * custom interpolation functions for example timed values.
 *
 * @see THuiTimedValue::SetFunction()
 */
class MHuiMappingFunction
    {
public:

    /**
     * Virtual destructor.
     */
    virtual ~MHuiMappingFunction()
        {
        }

    /**
     * Perform the mapping. Defines a function y = f(x, mode).
     *
     * Implement this method that calculates a output value for
     * a timed value based on the input value and a component/mode
     * parameter.
     *
     * For example the aValue input could be an angle, and the aMode
     * could be a X or Y component enumeration, then the MapValue could
     * calculate return a x or y position on a curve path.
     *
     * @param aValue      Unmapped value.
     * @param aMode       Mapping mode used for mapping this value.
     *
     * @return  Mapped value.
     */
    virtual TReal32 MapValue(TReal32 aValue, TInt aMode) const __SOFTFP = 0;
    
    /**
     * Determines whether the mapping function has changed since the last
     * call to MHuiMappingFunction::MappingFunctionClearChanged().
     *
     * By default, mapping functions themselves are assumed to be constant
     * (not changing over time).
     *
     * @return  <code>ETrue</code>, if the mapping function has changed 
     *          so that the same input value will return a different result.
     *          Otherwise, <code>EFalse</code>.
     */
    IMPORT_C virtual TBool MappingFunctionChanged() const;
    
    /**
     * Clears the change flags of the mapping function. This is called 
     * by the refresh loop after a frame has been fully drawn.
     */
    IMPORT_C virtual void MappingFunctionClearChanged();

    };


/**
 * Timed value interpolation styles.
 *
 * @see To implement custom interpolation styles, you can use
 * mapping functions.
 *
 * @see MHuiMappingFunction
 * @see THuiTimedValue::SetFunction()
 */
enum THuiInterpolationStyle
    {
    /** No interpolation is done. The target value becomes effective
        in a single step. */
    EHuiTimedValueStyleNone,

    /** Use linear interpolation between the current value and the
        target value. */
    EHuiTimedValueStyleLinear,

    /** Use a sine wave for approximating preservation of momentum. */
    EHuiTimedValueStyleSineWave,

    /** Quarter of a sine wave for decelerating movement. */
    EHuiTimedValueStyleDecelerate,

    /** Quarter of a sine wave for accelerating movement. */
    EHuiTimedValueStyleAccelerate,

    /** Sine wave or decelerating style based on when the target value
        is defined. */
    EHuiTimedValueStyleAdaptive,

    /** Combination of linear and deceleration. */
    EHuiTimedValueStyleImpulse,

    EHuiTimedValueStyleLast
    };


/**
 * Timed values are able to interpolate their value independently based on
 * time. The interpolation can be configured to follow a curve instead of
 * being linear. Timed value does not restrict the value units but interpolates
 * on any given real value. Mapping function can be used that maps the interpolated
 * values to some other real value space. If no mapping function is specified an
 * identity mapping is used. Current values and target values of timed value can be
 * set and current value according to the current CHuiStatic provided time can be retrieved
 * through class methods. Delays for timed value to achieve the target value can
 * be given in the Set - method. Timed value supports number of different interpolation
 * styles.
 *
 * @todo  The size of a timed value should be minimized. Also possible:
 *        centralize timed values to a static array, and make THuiTimedValue
 *        a handle to an element there. Since timed values are sometimes
 *        used as public member variables, their size cannot be changed
 *        or binary compatibility will be broken.
 */
class THuiTimedValue
    {
public:

    /* Constructors. */

    /** @beginAPI */

    /**
     * Constructor. Enables creation of timed value with no explicitly set initial value.
     * The timed value is set to zero.
     *
     * @param aStyle  Interpolation style used to reach target value.
     */
    IMPORT_C THuiTimedValue(THuiInterpolationStyle aStyle = EHuiTimedValueStyleAdaptive);

    /**
     * Constructor. Enables initial value setting.
     *
     * @param aInitialValue  Initial value.
     *
     * @param aStyle  Interpolation style used to reach target value.
     */
    IMPORT_C THuiTimedValue(TReal32 aInitialValue,
                            THuiInterpolationStyle aStyle = EHuiTimedValueStyleAdaptive) __SOFTFP;


    /* Methods. */

    /**
     * Returns the interpolation style of the timed value.
     */
    IMPORT_C THuiInterpolationStyle Style() const;

    /**
     * Sets the interpolation style of the timed value.
     *
     * @param aStyle  Interpolation style used to reach target value.
     */
    IMPORT_C void SetStyle(THuiInterpolationStyle aStyle);

    /**
     * Sets a mapping function that affects the value returned by
     * Now(). The timed value does not receive ownership
     * of the mapping function object.
     *
     * @param aFunction  Function that will map the current value to required
     * number space.
     */
    IMPORT_C void SetFunction(MHuiMappingFunction* aFunction);

    /**
     * Sets the mapping mode that is communicated to the current mapping function
     * (@see SetFunction()). The mapping function can map values differently
     * according to the value mode. The mapping mode is represented as one
     * bit, thus it can vary between 0 and 1. This can be ignored
     * unless required by the used mapping function.
     * @note Mode range may be extended in the future.
     * @todo Add MappingMode - method.
     *
     * @param aMode  One bit integer that defines the mode of this timed value.
     */
    IMPORT_C void SetMappingMode(TInt aMode);

    /**
     * Returns the current value of the timed value. This is affected by the
     * set mapping function. @see SetFunction()
     *
     * @return  Mapped current value.
     */
    IMPORT_C TReal32 Now() const __SOFTFP;

    /**
     * Returns the current value of the timed value, with no value mapping.
     *
     * @return  Unmapped value at the moment.
     */
    IMPORT_C TReal32 UnmappedNow() const __SOFTFP;

    /**
     * Returns the target value.
     *
     * @return  Target value.
     */
    IMPORT_C TReal32 Target() const __SOFTFP;

    /**
     * Set a new current value and target value. Change becomes effective immediately.
     * This stops the current interpolation.
     *
     * @param aValue  New current and target value.
     */
    IMPORT_C void Set(TReal32 aValue) __SOFTFP;

    /**
     * Set a new target value that becomes effective after a delay.
     * @see SetWithSpeed()
     *
     * @param aValue            New target value.
     * @param aTransitionTime   When the target value will be in effect. Transition duration in milliseconds.
     */
    IMPORT_C void Set(TReal32 aValue, TInt aTransitionTime) __SOFTFP;

    /**
     * Sets a new target value. The transition time depends on the speed.
     * @see Set()
     *
     * @param aValue           New target value.
     * @param aUnitsPerSecond  Speed of change.
     */
    IMPORT_C void SetWithSpeed(TReal32 aValue, TReal32 aUnitsPerSecond) __SOFTFP;


    /**
     * Determines how much time remains until the transition is complete.
     *
     * @return  Time in milliseconds.
     */
    IMPORT_C TInt RemainingTime() const;

    /**
     * Finishes the current interpolation immediately. Sets the value to target value.
     * If no interpolation is under progress nothing is done.
     */
    IMPORT_C void Finish();

    /**
     * Determines whether the value has changed since last call to
     * ClearChanged().
     */
    inline TBool Changed() const
        {
        if(iMapFunction && iMapFunction->MappingFunctionChanged())
            {
            return ETrue;
            }
        return (iInternalState & EFlagChanged) != 0;
        }

    /**
     * Determines whether the value is currently being interpolated towards
     * a target value.
     */
    inline TBool Interpolating() const
        {
        return (iInternalState & EFlagInterpolating) != 0;
        }

    /**
     * Flag the value as not changed. This is called by owning controls and visuals
     * when they clear their change status. See CHuiControl and CHuiVisual.
     */
    IMPORT_C void ClearChanged();

    /**
     * Enables or disables timed value clock mode. In clock mode, the timed
     * value's evaluated value is affected by the number of seconds since the
     * initialization of CHuiStatic. The timed value will still operate like
     * normally. In clock mode, a timed value can be used for perpetually
     * continuing animations. Mapping functions can be used as normally.
     * The clock mode is disabled by default.
     *
     * When a timed value's clock mode is activated, its evaluted value will
     * be <i>MappingFunction((unmapped value) + (seconds since start))</i>.
     * If you wish to make the evaluated value begin from zero at the time
     * when the clock mode is enabled, use the following code:
     * @code
     * timedValue.EnableClock();
     * timedValue.Set(-CHuiStatic::SecondsSinceStart());
     * @endcode
     *
     * @see CHuiStatic::SecondsSinceStart()
     *
     * @param aEnable  <code>ETrue</code> to enable clock mode,
     *                 <code>EFalse</code> to disable clock mode.
     */
    IMPORT_C void EnableClock(TBool aEnable = ETrue);

    /**
     * Suspends or restores the functionality of the timed value. When suspended,
     * the timed value's internal state is not updated. This allows waiting
     * for an indetermine period of time before the state accessed.
     * @note The suspend state is not copied when a copy of the timed value
     * is made with the assignment operator.
     *
     * @param aDoSuspend  <code>ETrue</code>, if the timed value is to be suspended.
     *                    <code>EFalse</code>, if restored.
     *
     * @see THuiTimedValue::TFlags::EFlagFrozen
     */
    IMPORT_C void Suspend(TBool aDoSuspend = ETrue);

    /**
     * Assigns another timed value to this value. If the other value is suspended,
     * it is automatically restored.
     *
     * @param aOtherValue  Other timed value.
     *
     * @return  Reference to this timed value.
     */
    IMPORT_C THuiTimedValue& operator = (const THuiTimedValue& aOtherValue);

    /** @endAPI */

    /**
     * Helper method to calcuate seconds between start and end times.
     * Can handle wrapping over of counter.
     *
     * @param aStart Starting time in milliseconds.
     * @param aEnd Ending time in milliseconds.
     * @return Seconds between the times.
     */
    TReal32 SecondsFrom(TUint32 aStart, TUint32 aEnd) const;

protected:

    /**
     * Map a value using the mapping function.
     *
     * @param aValue Value to be mapped.
     * @return Result of the mapping operation.
     */
    TReal32 MapValue(TReal32 aValue) const;


private:

    /**
     * Flags that are used to store state and mode values
     * to one internal state variable iInternalState.
     * These are collected to one variable to save memory on
     * used THuiTimedValue objects.
     */
    enum TFlags
        {
        /** True, if the value has changed since the last call to
            ClearChanged(). */
        EFlagChanged = 0x0001,

        /** True, if the value is currently being interpolated between the
            last value and the target. */
        EFlagInterpolating = 0x0002,

        /** Adapt interpolation style depending on the kind of change that
            is occuring when a new target value is specified. */
        EFlagAdaptive = 0x0004,

        /** The number of seconds since initialization of the toolkit is
            added to the value returned by UnmappedNow(). This can be used
            for making perpetually ongoing animations.
            @see CHuiStatic::SecondsSinceStart(). */
        EFlagAddSecondsSinceStart = 0x0008,

        /** The timed value is frozen so that if evaluated, it will remain
            at iInterpolationStartValue and none of the time counters are updated. Frozen
            values can be used for storing a timed value for an indeterminate
            time and applying at a later time. This is used by
            THuiValueCommand, for example, to store the new target state of
            a value in a command that is executed at a later point in time.
            @note This is an internal flag that will not be copied when a copy of
            the timed value is made with the assignment operator. */
        EFlagFrozen = 0x0010,

        /** Mapping mode mask. The masked bit is used to store mapping mode. */
        EFlagMappingModeMask = 0x0100,

        /**
          * Mapping mode bitwise shift. This shift value will shift the mapping
          * mode value to bit 0.
          */
        EFlagMappingModeShift = 8
        };

private:

    /** Interpolation style used to reach target value. */
    THuiInterpolationStyle iStyle;

    /** Internal state of the timed value. Collection of values masked using TFlags. */
    mutable TInt16 iInternalState;

    /**
     * Time when the current interpolation was started (milliseconds).
     * Relative to first call to CHuiStatic::UpdateTime().
     * @see CHuiStatic::MilliSecondsSinceStart()
     */
    mutable TUint32 iInterpolationStartTime;

    /**
     * Time when the target value will be in effect in milliseconds.
     * Relative to first CHuiStatic::UpdateTime().
     * @see CHuiStatic::MilliSecondsSinceStart()
     */
    mutable TUint32 iInterpolationEndTime;

    /**
     * Start value of current interpolation.
     */
    mutable TReal32 iInterpolationStartValue;

    /** Target value for the interpolation. */
    TReal32 iTargetValue;

    /** Pointer to a mapping function object. If <code>NULL</code>, an identity
        mapping is used. */
    MHuiMappingFunction* iMapFunction;

private:
    TInt iTimeRemainingWhenSuspended;
    TInt iSpare2;
    friend class CHuiVisual;
    };


#endif  // __HUITIMEDVALUE_H__
