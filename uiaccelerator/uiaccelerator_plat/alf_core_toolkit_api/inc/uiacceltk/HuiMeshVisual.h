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
* Description:   Definition of CHuiMeshVisual. CHuiMeshVisual is a visual
*                that is able to display a 3D triangle mesh.
*
*/



#ifndef __HUIMESHVISUAL_H__
#define __HUIMESHVISUAL_H__


#include <e32base.h>
#include <uiacceltk/HuiOwnedPointer.h>
#include <uiacceltk/HuiVisual.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiMesh.h>
#include <uiacceltk/HuiImage.h>


/* Forward declarations. */
class CHuiControl;
class CHuiProceduralMesh;
class CHuiM3GMesh;

/**
 * Visual that displays a 3D triangle mesh. 
 * This implementation only supports OpenGL ES as the renderer.
 */
NONSHARABLE_CLASS(CHuiMeshVisual) : public CHuiVisual
    {
public:

    /* Constructors and destructor. */

    /**
     * Constructs and appends a new mesh visual to the owner control.
     *
     * Creates a concrete CHuiMesh - derived implementation into this mesh visual
     * depending on the given mesh type. This method can leave with any leave codes
     * defined in the NewL - method of the concrete mesh class that is
     * constructed. This method allows creation of a specific mesh type upon the 
     * construction of the visual. When visual is created through CHuiControl::AppendVisualL()
     * or CHuiControl::AppendVisualLC() EHuiMeshTypeProcedural is used as the mesh
     * type. The mesh type can be changed by calling CreateMeshL().
     * 
     * @param aOwnerControl  Control that will own the new visual.    
     * 
     * @param aMeshType The enumerator describing the type of concrete mesh
     * to be created.     
     *
     * @return  The new visual.
     */
    IMPORT_C static CHuiMeshVisual* AddNewL(CHuiControl& aOwnerControl,
    										THuiMeshType aMeshType,
                                            CHuiLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    CHuiMeshVisual(MHuiVisualOwner& aOwner);

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    ~CHuiMeshVisual();


    /* Methods. */
	/**
	 * Creates CHuiMesh - derived object and attaches it to this CHuiMeshVisual.
	 * This method will destroy the mesh object that was attached to this
	 * mesh visual before. The created mesh object can be queried through
	 * Mesh().
	 *
	 * @param aMeshType Mesh type to be created.
	 */
	IMPORT_C void CreateMeshL(THuiMeshType aMeshType);    
    
    /**
     * Sets the texture image used with this CHuiMeshVisual.
     */    
    IMPORT_C void SetImage(const THuiImage& aImage);

    /**
     * Secondary image is used for crossfading.
     */    
    IMPORT_C void SetSecondaryImage(const THuiImage& aImage);

    IMPORT_C void SetSpecularImage(const THuiImage& aImage);

	
    /**
     * Returns the mesh object of the mesh visual.
     *
     * @return  Mesh.
     */
    IMPORT_C CHuiMesh& Mesh();

	/**
	 * Returns pointer to the procedural mesh of the mesh visual. If the 
	 * contained mesh isn't procedural mesh, function returns NULL.
	 * 
	 * @return Pointer to the procedural mesh.
	 */
	IMPORT_C CHuiProceduralMesh* ProceduralMesh() const; 

	/**
	 * Returns pointer to the M3G mesh of the mesh visual. If the contained 
	 * mesh isn't M3G mesh, function returns NULL.
	 * 
	 * @return Pointer to the M3G mesh.
	 */
	IMPORT_C CHuiM3GMesh* M3GMesh() const; 

    void GetClassName(TDes& aName) const
        {
        aName = _L("CHuiMeshVisual");
        }

public: // From CHuiVisual

	void SetSize(const THuiRealSize& aSize, TInt aTransitionTime = 0);
	
   	void SetPos(const THuiRealPoint& aPos, TInt aTransitionTime = 0);

	void SetRect(const THuiRealRect& aRect, TInt aTransitionTime = 0);
 	
 	/**
     * Draw the visual.
     */
    void DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const;

    TBool Changed() const;

    void ClearChanged();

private:

    /** Triangle mesh. */
    RHuiOwnedPointer<CHuiMesh> iMesh;

    /** Surface image for the mesh. */
    THuiImage iImage;
    
    /** Secondary surface image for the mesh. */
    THuiImage iSecondaryImage;


public:

    /* Public properties. */
    
    /** Alpha level of the secondary image. */
    THuiTimedValue iSecondaryAlpha;
    
    THuiTimedValue iYawAngle;
    
    THuiTimedValue iPitchAngle;
    
    THuiTimedValue iScale;

    };

#endif  // __HUIMESHVISUAL_H__
