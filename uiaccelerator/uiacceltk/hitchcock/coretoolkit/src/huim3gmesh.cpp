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
* Description:   Definition of CHuiM3GMesh, an interface enabling usage of M3G
 * 				 scene graphs within Hitchcock UI Toolkit user interfaces.
 *
*/



#include <uiacceltk/huim3gmesh.h>
#include <uiacceltk/HuiStatic.h>
#include <uiacceltk/HuiEnv.h>

// Force softfp linkage until included header has its own definitions
#if defined(__ARMCC_VERSION)
#pragma push
#pragma softfp_linkage
#endif
#include <M3G/m3g_core.h>
#if defined(__ARMCC_VERSION)
#pragma pop
#endif

#include <s32file.h>
#include <e32math.h>
#include <cstack.h>

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// A static function to destroy an M3G object if associated clean up item
// is popped from the cleanup stack.
// ---------------------------------------------------------------------------
//
void DestroyM3GObject(TAny* aObject)
    {
    m3gDeleteObject( (M3GObject)aObject );
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CHuiM3GMesh::CHuiM3GMesh()
: CHuiMesh(EHuiMeshTypeM3G)
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C void CHuiM3GMesh::ConstructL()
    {
    iCamRotation[0] = 0;
    iCamRotation[1] = 0;
    iCamRotation[2] = 0;
    iCamRotation[3] = 0;
    iCamTranslation[0] = 0;
    iCamTranslation[1] = 0;
    iCamTranslation[2] = 0;
    iCamTranslation[3] = 0;
    }

// ---------------------------------------------------------------------------
// Load scene from file.
// ---------------------------------------------------------------------------
//
EXPORT_C void CHuiM3GMesh::LoadSceneL(const TDesC& /*aFileName*/)
    {
    }

EXPORT_C void CHuiM3GMesh::LoadSceneL(const TDesC& aFileName, M3GInterface aInterface)
    {
    // Release the previously loaded M3G scene graph
    ReleaseScene();

    // Load the new scene graph
    TRAPD( err, DoLoadSceneL( aFileName, aInterface ) );

    // If an error occured during the scene loading release the scene and
    // re-leave.
    if( err != KErrNone )
        {
        ReleaseScene();
        User::Leave( err );
        }
    }

void CHuiM3GMesh::DoLoadSceneL(const TDesC& aFileName, M3GInterface aInterface)
    {
    // Load M3G content from same path as images
    TFileName fileName;
    fileName = CHuiStatic::Env().TextureManager().ImagePath();
    fileName += aFileName;

    // Create M3G loader to load the scene from memory buffer.
    M3GLoader loader = m3gCreateLoader(aInterface);
    if (loader == NULL)
        {
        User::Leave(KErrNoMemory);
        }
    TCleanupItem loaderCleaner( &DestroyM3GObject, loader);
    CleanupStack::PushL(loaderCleaner);

    // Read the file contents to the memory buffer    
    RFs fs;
    CleanupClosePushL(fs);
    RFileReadStream reader;
    CleanupClosePushL(reader);
    User::LeaveIfError(fs.Connect() );
    TInt errorCode = reader.Open(fs, fileName, EFileRead);
    if (errorCode != KErrNone)
        {
        User::Leave(errorCode);
        }

    // Read the contents of the file to a buffer.
    TInt sceneSize = reader.Source()->SizeL();
    TUint chunkSize = 1024;
    TUint8 *buf = new (ELeave) TUint8[chunkSize];
    CleanupStack::PushL(buf);
    TInt i = 0;
    M3Gsizei reqBytes = 1;
    while (i < sceneSize && reqBytes > 0 ){
    TInt j = 0;
    for( j = 0; j < chunkSize && i < sceneSize; j++ )
        {
        buf[j] = reader.ReadUint8L();
        i++;
        }
    reqBytes = m3gDecodeData( loader, j, buf );
    }

// Get number of loaded objects
iNumObjects = m3gGetLoadedObjects( loader, NULL );

// The input buffer from the file is not needed anymore
CleanupStack::PopAndDestroy( buf );
CleanupStack::PopAndDestroy( &reader );
CleanupStack::PopAndDestroy( &fs );

// Leave with KErrCorrupt if there were no M3G objects within the file (corrupted file).
if( iNumObjects == 0 )
    {
    User::Leave( KErrCorrupt );
    }

// Add loaded objects to an object array.
iObjects = new (ELeave) M3GObject[iNumObjects];
m3gGetLoadedObjects( loader, iObjects );

// Add references and find the first world object
for( TInt ii = 0; ii < iNumObjects; ii++ )
    {
    m3gAddRef( iObjects[ii] );
    if( m3gGetClass( iObjects[ii] ) == M3G_CLASS_WORLD && !iWorld )
        {
        iWorld = (M3GWorld)iObjects[ii];
        break;
        }
    }

// Fetch animation controllers from world
PopulateAnimationControllerArrayL( (M3GObject)iWorld );

iUpdateCamera = EFalse;
iCamera = m3gGetActiveCamera(iWorld);
if ( !iCamera )
    {
    // Create camera if it doesn't exist
    // The camera is updated according to hitchcock projection and model view matrices
    iUpdateCamera = ETrue;
    iCamera = m3gCreateCamera(aInterface);
    if( iCamera == NULL )
        {
        User::Leave( KErrNoMemory );
        }
    m3gAddChild( (M3GGroup)iWorld, (M3GNode)iCamera );
    m3gSetActiveCamera( iWorld, iCamera );
    M3GMatrix identity;
    m3gIdentityMatrix( &identity );
    m3gSetTransform( (M3GTransformable)iCamera, &identity );
    m3gTranslate( (M3GTransformable)iCamera, 0.f, 0.f, 1.f );
    m3gSetPerspective( iCamera,90.f, 1.f, 0.1f, 100.f );
    }

// Init background
M3GBackground bg = m3gGetBackground( iWorld );
if (!bg)
    {
    bg = m3gCreateBackground( aInterface );
    if( bg == NULL )
        {
        User::Leave( KErrNoMemory );
        }
    m3gSetBackground( iWorld, bg );
    }
m3gSetBgEnable( bg, M3G_SETGET_COLORCLEAR, M3G_FALSE );
m3gSetBgEnable( bg, M3G_SETGET_DEPTHCLEAR, M3G_FALSE );

// Release the loader
CleanupStack::PopAndDestroy( loader );
}

// ---------------------------------------------------------------------------
// Object destructor.
// ---------------------------------------------------------------------------
//    
EXPORT_C CHuiM3GMesh::~CHuiM3GMesh()
    {
    ReleaseScene();
    iControllers.Close();
    }

// ---------------------------------------------------------------------------
// Set viewport rect
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHuiM3GMesh::SetViewportRect( const THuiRealRect& aRect )
    {
    iViewportRect = aRect;
    }

// ---------------------------------------------------------------------------
// Draw scene
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHuiM3GMesh::Draw(CHuiGc& /*aGc*/, const THuiImage* /*aImage*/,
        const THuiImage* /*aSecondaryImage*/,
        TReal32 /*aSecondaryAlpha*/) const __SOFTFP
    {
    }

// ---------------------------------------------------------------------------
// Release scene
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHuiM3GMesh::ReleaseScene()
    {
    if (iObjects)
        {
        for (TInt i = 0; i < iNumObjects; i++)
            {
            m3gDeleteRef(iObjects[i]);
            iObjects[i] = NULL;
            }
        delete [] iObjects;
        iNumObjects = 0;
        iObjects = NULL;
        iWorld = NULL;
        }
    iControllers.Reset();
    }

// ---------------------------------------------------------------------------
// Rotate world around Y axis
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHuiM3GMesh::RotateYaw(TReal32 aAngle) __SOFTFP
    {
    RotateObjects(aAngle, 0.0f, 1.0f, 0.0f);
    }

// ---------------------------------------------------------------------------
// Rotate world around X axis
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHuiM3GMesh::RotatePitch(TReal32 aAngle) __SOFTFP
    {
    RotateObjects(aAngle, 1.0f, 0.0f, 0.0f);
    }

// ---------------------------------------------------------------------------
// Find the center point of the group
// ---------------------------------------------------------------------------
//    
void CHuiM3GMesh::FindObjectCenter(M3GGroup aGroup, TReal32 aTrans[4],
        TBool aFirst)
    {
    TReal32 objectTrans[4];
    TInt numChildren = m3gGetChildCount(aGroup);
    for (TInt i = 0; i < numChildren; ++i)
        {
        M3GNode child = m3gGetChild((M3GGroup)aGroup, i);
        M3GClass m3gClass = m3gGetClass((M3GObject)child);
        M3GMesh mesh = (M3GMesh)child;

        if (m3gClass == M3G_CLASS_SKINNED_MESH || m3gClass == M3G_CLASS_MESH
                || m3gClass == M3G_CLASS_MORPHING_MESH)
            {

            // Set blending and culling
            M3GAppearance hApp = m3gGetAppearance(mesh, 0);

            /*
             M3GCompositingMode cm = m3gGetCompositingMode(hApp);
             m3gSetBlending(cm, M3G_REPLACE);
             m3gSetAlphaWriteEnable(cm, 0);
             m3gSetCompositingMode(hApp, cm);
             m3gSetAppearance( mesh, 0, hApp );
             */

            M3GPolygonMode pm = m3gGetPolygonMode(hApp);
            m3gSetCulling(pm, M3G_CULL_NONE);
            m3gSetPolygonMode(hApp, pm);

            m3gGetTranslation((M3GTransformable)mesh, objectTrans);
            if (!aFirst)
                {
                aTrans[0] = (aTrans[0] + objectTrans[0]) / 2;
                aTrans[1] = (aTrans[1] + objectTrans[1]) / 2;
                aTrans[2] = (aTrans[2] + objectTrans[2]) / 2;
                aTrans[3] = (aTrans[3] + objectTrans[3]) / 2;
                }
            else
                {
                aTrans[0] = objectTrans[0];
                aTrans[1] = objectTrans[1];
                aTrans[2] = objectTrans[2];
                aTrans[3] = objectTrans[3];
                aFirst = EFalse;
                }
            }
        else
            if (m3gClass == M3G_CLASS_GROUP || m3gClass == M3G_CLASS_WORLD)
                {
                m3gGetTranslation((M3GTransformable)mesh, objectTrans);
                if (!aFirst)
                    {
                    aTrans[0] = (aTrans[0] + objectTrans[0]) / 2;
                    aTrans[1] = (aTrans[1] + objectTrans[1]) / 2;
                    aTrans[2] = (aTrans[2] + objectTrans[2]) / 2;
                    aTrans[3] = (aTrans[3] + objectTrans[3]) / 2;
                    }
                else
                    {
                    aTrans[0] = objectTrans[0];
                    aTrans[1] = objectTrans[1];
                    aTrans[2] = objectTrans[2];
                    aTrans[3] = objectTrans[3];
                    aFirst = EFalse;
                    }
                FindObjectCenter((M3GGroup)child, aTrans, aFirst);
                }
            else
                {
                // For PC-lint
                }
        }

    }

// ---------------------------------------------------------------------------
// Rotates camera around worlds objects.
// ---------------------------------------------------------------------------
//    
void CHuiM3GMesh::RotateObjects(TReal32 aAngle, TReal32 aAxisX,
        TReal32 aAxisY, TReal32 aAxisZ)
__SOFTFP        {
        if(iWorld != 0)
            {

            // Find the center of the objects
            TReal32 objectTrans[4];
            objectTrans[0] = 0;
            objectTrans[1] = 0;
            objectTrans[2] = 0;
            objectTrans[3] = 0;
            FindObjectCenter((M3GGroup)iWorld,objectTrans);

            // Get camera position
            M3GCamera cam = iCamera;
            TReal32 trans[4];
            m3gGetTranslation((M3GTransformable)cam, trans);

            // Calculate distance of objects center point and camera
            objectTrans[0] = objectTrans[0]-trans[0];
            objectTrans[1] = objectTrans[1]-trans[1];
            objectTrans[2] = objectTrans[2]-trans[2];
            objectTrans[3] = objectTrans[3]-trans[3];
            TReal dist;
            Math().Sqrt(dist,(TReal)objectTrans[0]*objectTrans[0]+objectTrans[1]*objectTrans[1]+objectTrans[2]*objectTrans[2]);

            // Get orientation of camera
            TReal32 rot[4];
            m3gGetOrientation((M3GTransformable)cam, rot);

            // Create new camera vector
            M3GVec4 camZ;
            camZ.x = 0;
            camZ.y = 0;
            camZ.z = -dist;
            camZ.w = 1;

            // Create rotation matrix for camera (old position)
            M3GMatrix rotmtx;
            m3gIdentityMatrix(&rotmtx);
            m3gPostRotateMatrix(&rotmtx, rot[0], rot[1], rot[2], rot[3]);

            // Create rotation matrix for camera (new position)
            M3GMatrix mtx;
            m3gIdentityMatrix(&mtx);
            m3gPostRotateMatrix(&mtx, aAngle, aAxisX, aAxisY, aAxisZ);

            // Translate camera vector
            m3gTransformVec4(&rotmtx,&camZ);

            // Store this position
            M3GVec4 trans2;
            trans2.x = camZ.x;
            trans2.y = camZ.y;
            trans2.z = camZ.z;
            trans2.w = camZ.w;

            // create new camera vector
            camZ.x = 0;
            camZ.y = 0;
            camZ.z = -dist;
            camZ.w = 1;

            // transform this vector with new position matrix
            m3gTransformVec4(&mtx,&camZ);

            // transform with old position matrix
            m3gTransformVec4(&rotmtx,&camZ);

            // move and rotate camerea based on these calculations
            m3gPostRotate((M3GTransformable)cam,aAngle, aAxisX, aAxisY, aAxisZ);
            m3gTranslate((M3GTransformable)cam, -camZ.x+trans2.x, -camZ.y+trans2.y, -camZ.z+trans2.z);

            m3gAlignNode((M3GNode)cam,NULL);
            }
        }

    // ---------------------------------------------------------------------------
    // Store camera transformations
    // ---------------------------------------------------------------------------
    //    
EXPORT_C void CHuiM3GMesh::StoreCamera()
    {
    if(iWorld != 0)
        {
        m3gGetOrientation((M3GTransformable)iCamera, iCamRotation);
        m3gGetTranslation((M3GTransformable)iCamera, iCamTranslation);
        }
    }

// ---------------------------------------------------------------------------
// Restore camera transformations
// ---------------------------------------------------------------------------
//    	
EXPORT_C void CHuiM3GMesh::RestoreCamera()
    {
    if(iWorld != 0)
        {
        m3gSetOrientation((M3GTransformable)iCamera, iCamRotation[0],iCamRotation[1],iCamRotation[2],iCamRotation[3]);
        m3gSetTranslation((M3GTransformable)iCamera, iCamTranslation[0],iCamTranslation[1],iCamTranslation[2]);
        }
    }

// ---------------------------------------------------------------------------
// Fetch animation controllers from any M3G object (recursively).
// ---------------------------------------------------------------------------
//   
void CHuiM3GMesh::PopulateAnimationControllerArrayL(const M3GObject aObject)
    {
    // Array for storing the already visited M3G object pointers.
    RPointerArray<M3GObjectImpl> visited;
    CleanupClosePushL(visited);

    // Stack for iteratively traversing through the object reference net.
    CStack<M3GObjectImpl, EFalse>* stack = new ( ELeave ) CStack<M3GObjectImpl, EFalse>();
    CleanupStack::PushL(stack);

    // Push the initial M3G object to the iteration stack
    stack->PushL(aObject);

    // Iterate through all objects in the iteration stack
    while ( !stack->IsEmpty() )
        {
        // Pop the current M3G object
        M3GObject current = stack->Pop();

        // If animation controller, add to array
        M3GClass m3gClass = m3gGetClass(current);
        if (m3gClass == M3G_CLASS_ANIMATION_CONTROLLER)
            {
            M3GAnimationController controller =
                    (M3GAnimationController)current;

            THuiM3GMeshAnimationController newController;
            newController.iController = controller;
            newController.iPosition = THuiTimedValue( 0,
                    EHuiTimedValueStyleLinear);
            newController.iAnimating = EFalse;
            newController.iTimeOffset = 0;
            newController.iPreviousTime
                    = CHuiStatic::MilliSecondsSinceStart();
            iControllers.AppendL(newController);
            }

        // Traverse graph
        const TInt count = m3gGetReferences(current, NULL, 0);
        if (count)
            {
            visited.ReserveL(visited.Count() + count);
            M3GObject* refs = new M3GObject[count];
            User::LeaveIfNull(refs);
            CleanupArrayDeletePushL(refs);

            m3gGetReferences(current, refs, count);
            for (TInt i = 0; i < count; i++)
                {
                TInt index = 0;
                TInt err = visited.FindInAddressOrder(refs[i], index);
                if (err != KErrNone)
                    {
                    visited.InsertL(refs[i], index);
                    stack->PushL(refs[i]);
                    }
                }

            CleanupStack::PopAndDestroy(refs);
            }
        }

    CleanupStack::PopAndDestroy(stack);
    CleanupStack::PopAndDestroy( &visited);
    }

// ---------------------------------------------------------------------------
// Returns number of (hitchcock) animation controllers
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CHuiM3GMesh::AnimationControllerCount() const
    {
    TInt baseAnimations = CHuiMesh::AnimationControllerCount();
    TInt count = iControllers.Count();
    return baseAnimations + count;
    }

// ---------------------------------------------------------------------------
// Set position of animation in animation controller.		
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHuiM3GMesh::SetAnimationPosition(TInt aControllerId, TReal32 aTarget, TInt aTime) __SOFTFP
    {
    if(iWorld != 0)
        {
        // Animate every controller
        if(aControllerId == KHuiM3GAnimateAllControllers )
            {
            for(int i = 0; i < iControllers.Count(); i++)
                {
                ((iControllers[i]).iPosition).Set(aTarget-((iControllers[i]).iTimeOffset), aTime);
                }
            }
        else // Animate only aControllerId:s controller

            {
            for(int i = 0; i < iControllers.Count(); i++)
                {
                if(aControllerId == m3gGetUserID((M3GObject)(iControllers[i].iController)))
                    {
                    ((iControllers[i]).iPosition).Set(aTarget-((iControllers[i]).iTimeOffset), aTime);
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Go through animation controllers and does animations.		
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHuiM3GMesh::AnimateControllers()
    {
    if(iWorld != 0)
        {
        TInt currentTime = CHuiStatic::MilliSecondsSinceStart();
        for(TInt i = 0; i < iControllers.Count(); i++)
            {
            M3GAnimationController controller = (iControllers[i]).iController;
            if( ( (iControllers[i]).iPosition).Changed())
                {
                ((iControllers[i]).iPosition).ClearChanged();
                }
            else
                {
                if ( (iControllers[i]).iAnimating )
                    {
                    TInt timeChanged = currentTime-((iControllers[i]).iPreviousTime);
                    ((iControllers[i]).iTimeOffset) += timeChanged;
                    }
                }
            ((iControllers[i]).iPreviousTime) = currentTime;
            m3gSetPosition(controller, ((iControllers[i]).iPosition).Now() + ((iControllers[i]).iTimeOffset), currentTime);
            m3gSetActiveInterval(controller, currentTime, currentTime);
            }
        m3gAnimate((M3GObject)iWorld, currentTime);
        }
    }

// ---------------------------------------------------------------------------
// Start animation of the controller
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHuiM3GMesh::StartAnimationController(TInt aControllerId)
    {
    if(iWorld != 0)
        {
        // Start every animation controller
        if(aControllerId == KHuiM3GAnimateAllControllers )
            {
            for(int i = 0; i < iControllers.Count(); i++)
                {
                iControllers[i].iAnimating = ETrue;
                TInt currentTime = CHuiStatic::MilliSecondsSinceStart();
                m3gSetActiveInterval((iControllers[i]).iController, currentTime, currentTime);
                }
            }
        else // Start only aControllerId:s controller

            {
            for(int i = 0; i < iControllers.Count(); i++)
                {
                if(aControllerId == m3gGetUserID((M3GObject)(iControllers[i].iController)))
                    {
                    iControllers[i].iAnimating = ETrue;
                    TInt currentTime = CHuiStatic::MilliSecondsSinceStart();
                    m3gSetActiveInterval((iControllers[i]).iController, currentTime, currentTime);
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Stop animation of the controller
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHuiM3GMesh::StopAnimationController(TInt aControllerId)
    {
    if(iWorld != 0)
        {
        // Stop every animation controller
        if(aControllerId == KHuiM3GAnimateAllControllers )
            {
            for(int i = 0; i < iControllers.Count(); i++)
                {
                iControllers[i].iAnimating = EFalse;
                TInt currentTime = CHuiStatic::MilliSecondsSinceStart();
                m3gSetActiveInterval((iControllers[i]).iController, currentTime-2, currentTime-1);
                }
            }
        else // Stop only aControllerId:s controller

            {
            for(int i = 0; i < iControllers.Count(); i++)
                {
                if(aControllerId == m3gGetUserID((M3GObject)(iControllers[i].iController)))
                    {
                    iControllers[i].iAnimating = EFalse;
                    TInt currentTime = CHuiStatic::MilliSecondsSinceStart();
                    m3gSetActiveInterval((iControllers[i]).iController, currentTime-2, currentTime-1);
                    }
                }
            }
        }
    }

EXPORT_C void CHuiM3GMesh::M3GMeshExtension(const TUid& /*aExtensionUid*/, TAny** aExtensionParams)
    {
    // If no extension with given UID was found, indicate it by returning null
    *aExtensionParams = NULL;
    }

