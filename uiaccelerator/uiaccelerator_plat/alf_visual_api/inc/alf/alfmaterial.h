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
* Description:   ?Description
*
*/



#ifndef T_ALFMATERIAL_H
#define T_ALFMATERIAL_H

#include <alf/alfimage.h>
#include <alf/alftimedvalue.h>
#include <gdi.h>

/** Surface material. */    
enum TAlfMaterialPreset
    {
    /** Material that uses custom parameters. */
    EAlfMaterialCustom,
    
    EAlfMaterialChrome,
    EAlfMaterialPlastic,
    /*
     * Speculer Image in material (iSpecularImage) has to be set
     * before setting  EAlfMaterialGlass preset. If iSpecularImage
     * is not set, Toolkit will Panic, while drawing the mesh.
     */
    EAlfMaterialGlass,
    /*
     * Speculer Image in material (iSpecularImage) has to be set
     * before setting  EAlfMaterialBrushedMetal preset. If iSpecularImage
     * is not set, Toolkit will Panic, while drawing the mesh.
     */
    EAlfMaterialBrushedMetal,
    /*
     * Speculer Image in material (iSpecularImage) has to be set
     * before setting  EAlfMaterialTexture preset. If iSpecularImage
     * is not set, Toolkit will Panic, while drawing the mesh.
     */
    EAlfMaterialTexture,
    EAlfMaterialShadow
    };

struct TAlfMaterial
    {
    /** Preset type. */
    TAlfMaterialPreset iPreset;
    
    /** Color. */
    TRgb iColor;
    
    /** Image used as the texture for the surface. */
    TAlfImage iTextureImage;
    
    /** Image used as the specular reflection for the surface. */
    TAlfImage iSpecularImage;

    /** Strength of specular reflections. */
    TAlfTimedValue iSpecular;

    
    /**
     * Default constructor for initializing the struct.
     */
    inline TAlfMaterial()
        : iPreset(EAlfMaterialCustom), iColor(KRgbWhite)
        {
        iSpecular.SetValueNow(1);
        }
        
    };

#endif // T_ALFMATERIAL_H
