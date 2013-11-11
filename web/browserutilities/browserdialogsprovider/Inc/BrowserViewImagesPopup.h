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
*      Declaration of view images popup listbox.
*
*
*/


#ifndef __VIEWIMAGESPOPUP_H
#define __VIEWIMAGESPOPUP_H

// INCLUDES
#include <e32base.h>
#include <e32def.h>
#include <aknPopup.h>
#include <aknlists.h>
#include <mdaimageconverter.h>

// Browser as a Plugin includes
#include <BrCtlDialogsProvider.h>

// FORWARD DECLARATIONS
class CCoeControl;
class TCoeEvent;
class CEikListBox;
class TListBoxEvent;
class CAknsListBoxBackgroundControlContext;
class TBrCtlImageCarrier;
class CPhotoOperationWait;
class CBrowserViewImagesListBox;


class CBrowserViewImagesPopup : public CAknPopupList
    {
    // Creation and Destruction
    public:

        CBrowserViewImagesPopup();
        virtual ~CBrowserViewImagesPopup();

        void ConstructL( CArrayFixFlat<TBrCtlImageCarrier>& aPageImages );

    public:       

        void HandleListBoxEventL( CEikListBox* aListBox, 
                                                    TListBoxEvent aEventType );
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

        void ProcessCommandL(TInt aCommandId);

        static void PhotoOperationL(const TBrCtlImageCarrier& aImg, CPhotoOperationWait* aPhotoOperationWait = NULL);
        

    protected:

        void ConstructL( CEikListBox *aListBox, TInt aCbaResource, 
                          AknPopupLayouts::TAknPopupLayouts aType = 
                                                AknPopupLayouts::EMenuWindow );
        
        void AttemptExitL(TBool aAccept);

    private:

        void PhotoOperationL();
        void LaunchPhotoAlbumL();
        TBool IsAppShutterActive();

    private: // from MObjectProvider

        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);    

    private:
        
        CPhotoOperationWait* iPhotoOperationWait;
        CAknsListBoxBackgroundControlContext* iSkinContext;

        CBrowserViewImagesListBox* iListBox;
    };

#endif // __VIEWIMAGESPOPUP_H

// End of file
