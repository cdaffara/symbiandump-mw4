/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Page overview control
*
*/
#ifndef __PLUGINCONTENTPLAYER_H__
#define __PLUGINCONTENTPLAYER_H__

// INCLUDES
#include <e32base.h>
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CBrCtl;
class PluginWin;

// CLASS
class PluginPlayer : public CCoeControl
    {
    public:
        // two phase contructor
        static PluginPlayer* NewL( CBrCtl& parent, PluginWin* plugin );

        virtual ~PluginPlayer();

        //from CCoeControl
        TInt CountComponentControls() const;
        CCoeControl *ComponentControl(TInt index) const;
        TKeyResponse OfferKeyEventL(const TKeyEvent keyEvent, TEventCode type);
        void Draw(const TRect& rect) const;

        // playing control
        void start();
        void stop();
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    private:
        void ConstructL();
        PluginPlayer( CBrCtl& parent, PluginWin* plugin );

        CBrCtl&         m_brctl;             // the parent control
        PluginWin*     m_plugin;            // not owned, the plugin
        TRect           m_orgrect;           // plugin rect in original parent
        CCoeControl*    m_orgparent;         // plugin parent
    };

#endif // !__PLUGINCONTENTPLAYER_H__
