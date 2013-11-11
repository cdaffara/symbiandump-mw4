/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   M3G Mesh definition
*
*/



#ifndef C_ALFM3GMESH_H
#define C_ALFM3GMESH_H

#include <alf/alfmesh.h>

// ID for Animating all controllers
const TInt KAlfM3GAnimateAllControllers = 100000;  

/**
 *  M3G Mesh support.
 *
 *  This is a CAlfMesh - derived mesh implementation that supports M3G
 *  scene graph loading and integration into the UI. This mesh can
 *  be used through CAlfMeshVisual and created with CAlfMeshVisual::CreateMeshL()
 *  method with argument EAlfMeshTypeM3G.
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfM3GMesh ): public CAlfMesh
    {

public:

    /**
     * Constructs a new CAlfM3G mesh instance.
     */
    static CAlfM3GMesh* NewL( CAlfGenComponent& aComms, CAlfEnv& aEnv );

    /**
     * Destructor
     */
    ~CAlfM3GMesh();

    /**
     * Loads an M3G scene graph from an M3G file. The M3G mesh scene graph is loaded 
     * from the file given as a parameter. 
     * If the filename string is malformed and does not adhere to
     * file path syntax this method leaves with KErrBadName error code.
     * If the file path is not found this method leaves with KErrPathNotFound error code.
     * Notice that CAlfTextureManager::ImagePath() is prepended to the filename before
     * loading the M3G file.
     * If the specified file is not found this method leaves with KErrNotFound error code.
     * If the M3G file does not contain any M3G objects or is corrupted this method
     * leaves with KErrCorrupt error code. In case the M3G scene graph loading fails
     * due to any of the errors listed above the loaded M3G scene graph and any created
     * resources due to the commenced loading are released. This means that no partially
     * loaded M3G content can be accessed through this object.
     * 
     *
     * @param aFileName Path to the M3G file from which the M3G scene graph is loaded
     *                  into this M3G mesh. The texture image path is prepended to this
     *                  resource file name. @see CHuiTextureManager::ImagePath().
     */	
	IMPORT_C virtual void LoadSceneL(const TDesC &aFileName);    

private:

    /**
     * Constructor
     */
    CAlfM3GMesh();

    /**
     * Second phase constructor
     */
    void ConstructL( CAlfGenComponent& aComms, CAlfEnv& aEnv );

private: // data

    struct TM3GMeshPrivateData;
    TM3GMeshPrivateData* iData;

    };


#endif // C_ALFM3GMESH_H
