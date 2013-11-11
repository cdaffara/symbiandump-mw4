/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Server declaration, internal for Nokia
*
*/



#ifndef __ALF_SCREEN_H__
#define __ALF_SCREEN_H__

#include <e32std.h>
#include <e32base.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiTextVisual.h>
#include "alfshareddisplaycoecontrol.h"

NONSHARABLE_CLASS( TAlfControlGroupEntry )  
    {
public:
    TAlfControlGroupEntry() :    
        iControlGroup(NULL),
        iWindowGroupNodeId(0),
        iClientWindowGroupId(0),
        iScreenNumber(0),
        iSecureId(0)
            {
            }

    /** Control group which represents a window group */
    CHuiControlGroup* iControlGroup;
    
    /** Window group node id, internal */
    TInt iWindowGroupNodeId; 
    
    /** Window group id, external id seen e.g. by the application using wserv */
    TInt iClientWindowGroupId;
    
    TUint8 iScreenNumber;
    
    TInt32 iSecureId;
    };


NONSHARABLE_CLASS(CAlfScreen) : public CBase
	    {
	public:
	        
        /**
         * Constructor
         */
	    CAlfScreen(); 

        /**
         * 2nd phase construction
         */
	    void ConstructL(TInt aScreenNum, MHuiDisplayRefreshObserver& aObserver, CHuiEnv& aHuiEnv, CAlfSharedDisplayCoeControl* aSharedCoeControl);
        void ConstructL(TInt aScreenNum, MHuiDisplayRefreshObserver& aObserver, CHuiEnv& aHuiEnv, RWindow* aWindow);
        void CommonConstructL( MHuiDisplayRefreshObserver& aObserver, CHuiEnv& aHuiEnv);
        /**
         * Destructor
         */
	    ~CAlfScreen(); 

        /**
         * Returns the size of the screen in pixels
         * @return Screesize in pixels
         */
        TSize Size() const;
        
        /**
         * Sets a flag which causes full visual tree visibility scan to happen later.
         */
        void SetVisualTreeVisibilityChanged(TBool aChanged);

        /**
         * Gets a flag which causes full visual tree visibility scan to happen later.
         */
        TBool IsVisualTreeVisibilityChanged();

        /**
         * Returns amount of fixed control groups.
         */
        TInt FixedControlGroupCount() const;
        
	public:
	        	   
	    RArray<TAlfControlGroupEntry> iControlGroups;
	  	CAlfSharedDisplayCoeControl* iCoeControl; // owned
	   
	   	CHuiDisplay* iDisplay; 
	    CHuiControlGroup* iFloatingSpriteControlGroup; 
	    CHuiControlGroup* iFullscreenEffectControlGroup; 
	    CHuiControlGroup* iFpsControlGroup;
	    
	    TBool iVisualTreeVisibilityChanged;
	    TInt iScreenNum;
	    CFbsBitmap* iSwRenderingTarget;	    
#ifdef SYMBIAN_BUILD_GCE	    
	    CHuiTextVisual* iFPSText;
#endif
	    };
         
#endif // __ALF_SCREEN_H__