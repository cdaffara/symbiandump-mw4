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
* Description:   ?Description
*
*/



#ifndef __HUIMATERIAL_H__
#define __HUIMATERIAL_H__


#include <gdi.h>
#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiTimedValue.h>

    
/** Surface material. */    
enum THuiMaterialPreset
    {
    /** Material that uses custom parameters. */
    EHuiMaterialCustom,
    
    EHuiMaterialChrome,
    EHuiMaterialPlastic,
    EHuiMaterialGlass,
    EHuiMaterialBrushedMetal,
    EHuiMaterialTexture,
    EHuiMaterialShadow
    };



/**
 * THuiMaterial describes the appearance properties of a surface material.
 * Used with 3D objects.
 */
struct THuiMaterial
    {

    /**
     * Default constructor for initializing the struct.
     */
    IMPORT_C THuiMaterial();

    /** Preset type. */
    THuiMaterialPreset iPreset;
    
    /** Color. */
    TRgb iColor;
    
    /** Image used as the texture for the surface. */
    THuiImage iTextureImage;
    
    /** Image used as the specular reflection for the surface. */
    THuiImage iSpecularImage;

    /** Strength of specular reflections. */
    THuiTimedValue iSpecular;
    
private:

    TInt iSpare1;
    TInt iSpare2;
    
    };


#endif // __HUIMATERIAL_H__
