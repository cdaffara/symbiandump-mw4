/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for caiwprintingprovider
*
*/



#ifndef __CAIWPRINTINGPROVIDER_H__
#define  __CAIWPRINTINGPROVIDER_H__

#include <AiwServiceIfMenu.h>
#include <AknLaunchAppService.h>
#include <badesca.h>

#include "caiwimageprintif.h"


/**
* AIW provider plugin that handles print scenario.
*/
class CAiwPrintingProvider :
    public CAiwImagePrintIf,
    public MAknServerAppExitObserver
    {
    public: //Constructors and destructors
        static CAiwPrintingProvider* NewL();
        ~CAiwPrintingProvider();
        
    public: // from MAknServerAppExitObserver
		void HandleServerAppExit( TInt aReason );
		void StartApplicationL();

    
    private: // From CAiwServiceIfMenu
        void InitialiseL(MAiwNotifyCallback& aFrameworkCallback,
                         const RCriteriaArray& aInterest);

        void InitializeMenuPaneL(CAiwMenuPane& aMenuPane,
                                 TInt aIndex,
                                 TInt aCascadeId,
                                 const CAiwGenericParamList& aInParamList);

        void HandleServiceCmdL(const TInt& aCmdId,
                               const CAiwGenericParamList& aInParamList,
                               CAiwGenericParamList& aOutParamList,
                               TUint aCmdOptions = 0,
                               const MAiwNotifyCallback* aCallback = NULL);

        void HandleMenuCmdL(TInt aMenuCmdId,
                            const CAiwGenericParamList& aInParamList,
                            CAiwGenericParamList& aOutParamList,
                            TUint aCmdOptions = 0,
                            const MAiwNotifyCallback* aCallback = NULL);
    private:
        void DoHandleCmdL(TInt aMenuCmdId,
                            const CAiwGenericParamList& aInParamList,
                            CAiwGenericParamList& aOutParamList,
                            TUint aCmdOptions = 0,
                            const MAiwNotifyCallback* aCallback = NULL);
                            
        void LaunchImagePrintApplicationL();  
        TBool IsImagePrintUiRunning();
        void ShowNoteL();
        
        
          
    private: // Data
        TInt iResourceOffset;
        const RCriteriaArray* iInterest;
        TInt iAiwCall;
        MAiwNotifyCallback* iNotifyCallback;
        CAknLaunchAppService* iService;
        
        const CAiwGenericParamList *iConsumerInParamList;
        CAiwGenericParamList *iConsumerOutParamList;
        const MAiwNotifyCallback *iConsumerCallback;
    };

#endif // __CAIWPRINTINGPROVIDER_H__

