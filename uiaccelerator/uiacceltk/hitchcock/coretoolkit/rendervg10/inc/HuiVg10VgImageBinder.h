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
* Description:   Definition of CHuiVg10VGImageBinder
*
*/

#ifndef HUIVG10VGIMAGEBINDER_H_
#define HUIVG10VGIMAGEBINDER_H_

#include <e32base.h>
#include <e32def.h>
#include <VG/openvg.h>
#include <EGL/egl.h>
#include <MVGImageBinder.h>

// Forward declarations
class CHuiVg10RenderPlugin;

/**
 * Implementation class for MVGImageBinder API.
 * 
 * This class is being used for providing the NVGDecoder the necessary
 * implementation for handling NVG-TLV special case "Group opacity"
 */
class CHuiVg10VgImageBinder : public CBase, public MVGImageBinder
    {
public:

    /**
     * Constructor.
     */
    static CHuiVg10VgImageBinder* NewL(CHuiVg10RenderPlugin* aRenderPlugin);

    /**
     * Destructor.
     */
    virtual ~CHuiVg10VgImageBinder();

    /**
     * From MVGImageBinder, creates a new PBufferFromClientBuffer
     * surface from the given parameter (of type VGImage)
     * 
     * @param   aBuffer A Buffer identifier (VGImage handle), used to create the surface
     * @return  Error code
     */
    TInt BindClientBuffer(TUint aBuffer);

    /**
     * From MVGImageBinder, unbinds the PBufFromCliBuf & restores the previous surface
     * 
     * @return  Error code
     */
    TInt UnBindClientBuffer();

    
private:    // Methods
    
    /**
    * Constructor.
    */
   CHuiVg10VgImageBinder(CHuiVg10RenderPlugin* aRenderPlugin);

   /**
    * 2nd phase constructor
    */
   void ConstructL();
    
   
private:    // Data

    /** NOT Owned! **/
    CHuiVg10RenderPlugin* iRenderPlugin;
    
    EGLSurface iEglPBufferSurface_Client;
    EGLSurface iSavedDrawSurface;
    EGLSurface iSavedReadSurface;

    EGLContext iContext;
    EGLContext iSavedContext;

    // Array for maintaining vgImages
    RArray<VGImage> iGroupOpacityImages;
    // Array for maintaining vgImages
    RArray<EGLSurface> iGroupOpacitySurfaces;

    };
    

#endif /* HUIVG10VGIMAGEBINDER_H_ */

