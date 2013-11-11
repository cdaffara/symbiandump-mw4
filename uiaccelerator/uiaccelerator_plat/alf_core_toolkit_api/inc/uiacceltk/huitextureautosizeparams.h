/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Declares huitextureautosizeparams texture class.
*
*/



#ifndef C_HUITEXTUREAUTOSIZEPARAMS_H
#define C_HUITEXTUREAUTOSIZEPARAMS_H

#include <e32std.h>

/**
 * Texture auto size parameters. These can be used to control texture 
 * resizing when automatic size calculation is enabled. Actual behaviour
 * of parameters may depend also on the used renderer, e.g. texture
 * resizing algorithm may be more aggressive in BitGdi than in 
 * OpenGLES renderer.
 * 
 * Example of use 
 * @code
 * texture->EnableAutoSizeCalculation();
 * THuiTextureAutoSizeParams newparams = texture->AutoSizeParams();
 * newparams.SetSizeLowerThreshold(THuiTextureAutoSizeParams::EHigh);
 * newparams.SetSizeUpperThreshold(THuiTextureAutoSizeParams::ELow);
 * texture->SetAutoSizeParams(newparams);
 * @endcode
 *
 */
NONSHARABLE_CLASS( THuiTextureAutoSizeParams )
    {

public:
    
    enum
        {
        EVeryLow = -20,
        ELow = -10,
        EMedium = 0,
        EHigh = 10,
        EVeryHigh = 20
        };
    
    /**
     * Constructor.
     */
    IMPORT_C THuiTextureAutoSizeParams();
    
    /** 
     * Gets a size threshold which defines how much required size may
     * decrease before it is considered to be small enough to 
     * cause texture resizing.
     *
     * @return Texture downsize threshold
     */  
    IMPORT_C TInt SizeLowerThreshold() const;

    /** 
     * Sets a size threshold which defines how much required size may
     * decrease before it is considered to be small enough to 
     * cause texture resizing.
     * 
     * Exmple: Setting value to EVeryLow causes texture to be 
     * very easily resized to a smaller size if needed. 
     * 
     * @param aSizeLowerThreshold Texture downscale threshold
     */  
    IMPORT_C void SetSizeLowerThreshold(TInt aSizeLowerThreshold);

    /** 
     * Gets a size threshold which defines how much required size may
     * increase before it is considered to be big enough to 
     * cause texture resizing.
     *
     * @return Texture upscale threshold
     */  
    IMPORT_C TInt SizeUpperThreshold() const;

    /** 
     * Sets a size threshold which defines how much required size may
     * increase before it is considered to be big enough to 
     * cause texture resizing.
     *
     * Exmple: Setting value to EVeryLow causes texture to be 
     * very easily resized to a larger size if needed. 
     *
     * @param aSizeUpperThreshold Texture upscale threshold
     */  
    IMPORT_C void SetSizeUpperThreshold(TInt aSizeUpperThreshold);

    /**
     * Gets the minumum amount of resize. If height or width difference 
     * between current size and required size is smaller than threshold
     * specified by this value, resizing is not done 
     * (except if the current texture size is zero).
     * @return aMinSizeChange 
     */
     IMPORT_C TInt MinSizeChange() const;   

    /**
     * Sets the minumum amount of resize. If height or width difference 
     * between current size and required size is smaller than threshold
     * specified by this value, resizing is not done 
     * (except if the current texture size is zero).
     *
     * Exmple: Setting value to EHigh can be used to prevent unnecessary
     * size changes if the required size varies often but changes are small.
     *
     * @param aMinSizeChange 
     */
     IMPORT_C void SetMinSizeChange(TInt aMinSizeChange);   

    /**
     * Gets size threshold under which relative resizing calculations are
     * settled to be less aggressive than specified. This is used to avoid unnecessary
     * resizing when texture size is very small (small pixel changes may be large
     * relative changes)
     *
     * @return Size threshold.
     */
    IMPORT_C TInt DownsizeSettleThreshold() const;

    /**
     * Sets size threshold under which relative resizing calculations are
     * settled to be less aggressive than specified. This is used to avoid unnecessary
     * resizing when texture size is very small (small pixel changes may be large
     * relative changes)
     *
     * @param aDownsizeSettlethreshold Size threshold.
     */
    IMPORT_C void SetDownsizeSettleThreshold(TInt aDownsizeSettleThreshold);
     

private:
    
    /** Lower size threshold */
    TInt iSizeLowerThreshold;
    /** Higher size threshold */
    TInt iSizeUpperThreshold;
    /** Min resize amount that makes sense to do */
    TInt iMinSizeChange;            
    /** Size when texture is so small that relative lower/upper thresholds are no more followed strictly */
    TInt iDownsizeSettleThreshold;
    /** Reserved for future use */
    TInt iSpare[10];
    };
#endif
