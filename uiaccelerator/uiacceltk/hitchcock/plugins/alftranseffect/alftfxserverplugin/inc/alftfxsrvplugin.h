/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   AlfTfxSrvPlugin Plugin implementation. Implements plugin for 
*                plugin interface under AknSkins.
*
*/




#ifndef ALFTFXSRVPLUGIN_H
#define ALFTFXSRVPLUGIN_H

#include <e32base.h>
#include <mtransitionserver.h>
#include <mtransitionservercontroller.h>
// From TFX Server
#include "alftransitionserver.h"
#include "alftransitionservercontroller.h"

/**
 *  TfxSrvPlugin class
 *
 *  Implements methods that are called by the ECOM to create
 *  instances of MTransitionServer & MTransitionServerController.
 *  This class is not meant to be instantiated.
 *
 *  @lib TfxSrvPlugin.dll
 *  @since 
 */
class TfxSrvPlugin
    {
    public:
    
    static MTransitionServer* CreateTransitionServerL();
    static MTransitionServerController* CreateTransitionServerControllerL( 
                                                                    TAny* aInitParams );

    private:
        // not meant to be instantiated
        TfxSrvPlugin() {};
    };


/**
 *  CSrvImpl, implementation of MTransitionServer
 * 
 *  Implements Tfx Server Plugin API interface, 
 *  MTransitionServer part. Does not contain any other logic
 *  than mediate calls to concrete implementation of
 *  CAlfTransitionServerClient in TFXServer.
 *
 *  @lib TfxSrvPlugin.dll
 *  @since 
 */
class CSrvImpl : public CBase, public MTransitionServer
    {
    
    public: // Constructor
    
        static CSrvImpl* NewL();    
    
    private: // From MTransitionServer
    
         /**
         * For comments on methods, and how to use them, see
         * transitionserver.h header from TFXServer
         */    
    	 TInt Connect();
    	 void Disconnect();
    	 TInt RegisterFullscreenKml(TUint aActionID, const TUid& aUid, const TDesC& aResourceDir, 
    										const TDesC& aFilename);
    	 TInt UnregisterFullscreenKml(TUint aActionID, const TUid& aUid);
         TInt BlockFullScreenUid(const TUid& aUid, TBool aBlockFrom, TBool aBlockTo);
    	 TInt RegisterControlKml(const TUid &aUid, const TDesC& aResourceDir, const TDesC& aFilename);
    	 TInt RegisterControlKml(const TUid &aUid, const TDesC& aResourceDir, const TDesC& aFilename,
    	 						 TInt aWantedTime, TInt aMinTime);
      	 TInt UnregisterControlKml(const TUid &aUid);
         TInt RegisterControlAction(const TUid& aUid, TUint aActionID, const TDesC& aActionString);
    	 TInt UnregisterControlAction(const TUid& aUid, TUint aActionID);
    	 TInt UnregisterAllKml();
    	 TInt SetWantedTime(TInt aTime);
    	 TInt SetMinTime(TInt aTime);
    	 TInt RegisterFullscreenKml(TUint aActionID, const TUid& aUid, const TDesC& aResourceDir, 
    										const TDesC& aFilename, TInt aCachePriority, TInt aWantedTime,
    										TInt aMinTime);
		
		 TInt RegisterListBoxKml(const TUid& aUid, const /*TListBoxType*/ TInt aListBoxType, 
                                     const TDesC& aResourceDir, 
									 const TDesC& aBackgroundFilename, 
									 const TDesC& aListItemFilename);


		TInt UnregisterListBoxKml(const TUid& aUid, const /*TListBoxType*/ TInt aListBoxType);		 		 
		 
		 TInt SetListBoxFrameTime( const TInt aWantedTime );
		 TInt SetListBoxMinFrameTime( const TInt aMinFrameTime );
		 TInt SetControlFrameTime( const TInt aWantedTime );
		 TInt SetControlMinFrameTime( const TInt aMinFrameTime );
          TBool IsConnected();
          
    private: 
    
        // to be deleted via MTransitionServer
        ~CSrvImpl();    
        void ConstructL();
    
    private: // Data
    
        CAlfTransitionServerClient* iServer;
    };


/**
 *  CSrvCtrlImpl, implementation of MTransitionServerController
 * 
 *  Implements Tfx Server Plugin API interface, 
 *  MTransitionServerController part. Does not contain any other logic
 *  than mediate calls to concrete implementation of
 *  CAlfTransitionServerController in TFXServer.
 *
 *  @lib TfxSrvPlugin.dll
 *  @since 
 */
class CSrvCtrlImpl : public CBase, public MTransitionServerController
    {
    public: // Constructor
    
        static CSrvCtrlImpl* NewL( RWsSession& aWsSession );    
        
    private: // From MTransitionServerController
        
        TInt StartTransitionServer();
        TInt StopTransitionServer();
        TBool ServerRunning();
        TInt StartTransitionServer(TInt32 aMaxBytesOngoing, TInt32 aMaxBytesBetween);
        
    private: 
    
        // to be deleted via MTransitionServer
        ~CSrvCtrlImpl();   
        void ConstructL( RWsSession& aWsSession );
        
    private:  // Data
        CAlfTransitionServerController* iController;
    };

#endif // ALFTFXSRVPLUGIN_H
