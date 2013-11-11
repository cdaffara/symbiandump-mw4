/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     
*     
*
*/


#ifndef __PHOTOOPERATIONWAIT_H
#define __PHOTOOPERATIONWAIT_H

//  INCLUDES
#include <e32base.h>
#include <apparc.h>
#include <AknServerApp.h>

//  FORWARD DECLARATIONS

//  CONSTANTS


/**
*  Helper class for Async2Sync trick.
*  
*  @lib Browser.app
*  @since Series 60 1.2
*/
class CPhotoOperationWait:  public CActive,
                            public MAknServerAppExitObserver

    {
    public:     // New functions
        
        /**
        * Set view activation
        * @since 1.2
        * @param aViewActivate
        */
        void SetViewActivate( TBool aViewActivate );
        
        /**
        * Is view activation started?
        * @since 1.2
        * @return ETrue if view activation is started
        */
        TBool IsStarted();
                                
        /**
        * Two-phased constructor.
        * @param aPriority Execution priority
        */
        static CPhotoOperationWait* NewL( TInt aPriority = EPriorityStandard );
        
        /**
        * Destructor.
        */
        ~CPhotoOperationWait();

    public:     // From MApaEmbeddedDocObserver

        /**        
        * @since 3.0
        * @param aMode Exit mode
        */
        void HandleServerAppExit( TInt aReason );
           
    public:     // From CActive 
        
        void Activate();  
        void Wait();
        void RunL();
        void DoCancel();

    
    public:
        /**
        * Open the active scheduler.
        * @since 1.2
        * @return -
        */
        void OpenPhotoalbum();
    
    
        /**
        * Stop the active scheduler.
        * @since 1.2
        * @return - 
        */
        void ClosePhotoAlbumObserver();
    
    
    protected:    // New functions
        
        /**
        * @since 1.2
        * @param aPriority Execution priority
        */
        CPhotoOperationWait( TInt aPriority );
    
    private:    // Data
        
        CActiveSchedulerWait iSafeWait;
        TBool iViewActivate;
    };

#endif  //__PHOTOOPERATIONWAIT_H
