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
* Description:  Handle scrollbar and tab events
*
*/


#ifndef BRCTLLAYOUTOBSERVERIMPL_H
#define BRCTLLAYOUTOBSERVERIMPL_H

//  INCLUDES
#include <BrCtlLayoutObserver.h>

/**
*  This observer is notified of scrolling and tabbing events
*
*  @lib BrowserEngine.lib
*  @since 2.8
*/
class CBrCtlLayoutObserver : public CBase, public MBrCtlLayoutObserver
    {
    public: // From MBrCtlLayoutObserverImpl
        
        /**
        * Update the position of vertical scrollbar.
        * @since 2.8
        * @param aDocumentHeight The total height of the markup page
        * @param aDisplayHeight The height of the display
        * @param aDisplayPosY The current Y position
        * @return void
        */
        virtual void UpdateBrowserVScrollBarL(TInt /*aDocumentHeight*/, 
                                              TInt /*aDisplayHeight*/,
                                              TInt /*aDisplayPosY*/ ) {}

        /**
        * Update the position of horizontal scrollbar.
        * @since 2.8
        * @param aDocumentWidth The total width of the markup page
        * @param aDisplayWidth The width of the display
        * @param aDisplayPosX The current X position
        * @return void
        */
        virtual void UpdateBrowserHScrollBarL(TInt /*aDocumentWidth*/, 
                                              TInt /*aDisplayWidth*/,
                                              TInt /*aDisplayPosX*/ ) {}

        /**
        * Inform the layout of the page: right to left or left to right. Useful when the application draws the scrollbar itself.
        * @since 2.8
        * @param aNewLayout RTL or LTR
        * @return void
        */
        virtual void NotifyLayoutChange( TBrCtlLayout /*aNewLayout*/ ) {}
        
        /**
        * Update the title of the page in history view
        * @since 3.0
        * @param aTitle Title of the page
        * @return void
        */
        virtual void UpdateTitleL( const TDesC& /*aTitle*/ ) {}        
    };

#endif      // BRCTLLAYOUTOBSERVERIMPL_H
            
// End of File
