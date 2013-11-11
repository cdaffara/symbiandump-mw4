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



#ifndef __HUIPROCEDURALMESH_H__
#define __HUIPROCEDURALMESH_H__


#include <e32base.h>
#include <gdi.h>

#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/HuiMaterial.h>
#include <uiacceltk/HuiSessionObject.h>
#include <uiacceltk/HuiMesh.h>


/* Forward declarations. */
class CHuiGc;


/**
 * CHuiProceduralMesh is the base class for renderer-specific implementations  
 * of 3D triangle meshes. It also defines the public interface that 
 * applications can use to manipulate meshes. It is possible to instantiate 
 * CHuiProceduralMesh itself, but the resulting mesh instance will have no 
 * functionality. 
 */
class CHuiProceduralMesh : public CHuiMesh
    {
public:    

    /* Constructors and destructors. */

    /**
     * Default constructor.
     */
    IMPORT_C CHuiProceduralMesh();

    /**
     * Destructor.
     */
    IMPORT_C ~CHuiProceduralMesh();           

    
    /* Methods. */

    /**
     * Resets the mesh. All contents are destroyed.
     */
    IMPORT_C virtual void Reset();

    /**
     * Creates a cube mesh.
     *
     * @param aSizeX
     * @param aSizeY
     * @param aSizeZ
     * @param aEdgeRadius
     */
    IMPORT_C virtual void MakeCubeL(TReal32 aSizeX, TReal32 aSizeY, TReal32 aSizeZ, 
                                    TReal32 aEdgeRadius) __SOFTFP;

    /**
     * Creates a sphere mesh.
     *
     * @param aRadius
     * @param aColumns
     * @param aRows
     */
    IMPORT_C virtual void MakeSphereL(TReal32 aRadius, TInt aColumns, TInt aRows) __SOFTFP;

    /** 
     * Creates a torus mesh.
     *
     * @param aMainRadius
     * @param aOuterRadius
     * @param aMainSegments
     * @param aOuterSegments
     * @param aSegmentAngleOffset
     */
    IMPORT_C virtual void MakeTorusL(TReal32 aMainRadius, TReal32 aOuterRadius, 
                             TInt aMainSegments, TInt aOuterSegments,
                             TReal32 aSegmentAngleOffset = 0) __SOFTFP;
    
    /**
     * Stretch the mesh uniformly along the X, Y, and Z axes. Negative side
     * of each axis is offseted negatively. Positive side of each side is 
     * offseted positively. Normals are not affected.
     *
     * @param aXLimit
     * @param aXOffset
     * @param aYLimit
     * @param aYOffset
     * @param aZLimit
     * @param aZOffset
     */
    IMPORT_C virtual void StretchUniformly(TReal32 aXLimit, TReal32 aXOffset, 
                                           TReal32 aYLimit, TReal32 aYOffset,
                                           TReal32 aZLimit, TReal32 aZOffset) __SOFTFP;
                                
    /**
     * Scale the mesh uniformly along each axis. Normals are also scaled and
     * renormalized.
     *
     * @param aX  X scaling factor.
     * @param aY  Y scaling factor.
     * @param aZ  Z scaling factor.
     */                                
    IMPORT_C virtual void ScaleUniformly(TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP;

    /**
     * Updates the surface material of the mesh.
     * Generate new texture coordinates based on the angles.
     *
     * @param aYaw    Angle around object Y axis.
     * @param aPitch  Angle around object X axis.
     */
    IMPORT_C virtual void UpdateSurface(TReal32 aYaw, TReal32 aPitch) __SOFTFP;

    /**
     * Draw the mesh.
     */     
    IMPORT_C virtual void Draw(CHuiGc& aGc, const THuiImage* aImage = 0, 
                      const THuiImage* aSecondaryImage = 0,
                      TReal32 aSecondaryAlpha = 0.0) const __SOFTFP;

    
protected:  // New methods
    
    /**
     * Provides expandability, helps keeping the binary compatibility. Since virtual
     * table is now exported and this class is dll derivable and the implementation
     * is more difficult to change, hence this method, which can provide additional
     * extension APIs.
     * 
     * @param aExtensionUid     UID, which is being used for recognizing the extension
     * @param aExtensionParams  Return pointer to the extension API, once recognized from the extension uid
     */
    IMPORT_C virtual void ProceduralMeshExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
    
public:

    /** Appearance properties. */
    THuiMaterial iMaterial;

private:

    /** Spare member to help keeping binary compatibility, since
        HuiProceduralMesh is now dll derivable **/
    TInt iSpare;
    
    };

#endif // __HUIPROCEDURALMESH_H__
