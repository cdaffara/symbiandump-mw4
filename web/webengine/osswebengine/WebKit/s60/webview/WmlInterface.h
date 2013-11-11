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
* Description:   All function that is required by the wmlengine from webkit.
*
*/



#ifndef WMLINTERFACE_H
#define WMLINTERFACE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "BrCtlDefs.h"
#include "BrCtl.h" 

class CWmlContentInterface;

class MWmlInterface
    {
    public: // New functions

    /**
     * virtual destructor
     */
    inline virtual ~MWmlInterface() {};
        
        /**
        * Free all WML service options
    * Function BrCtl.h
    * void CBrCtl::ResetWmlServiceOptions().
        * @since 3.1.
        * @return void
        */

        virtual void resetWmlServiceOptions() = 0;

        /**
        * Add a WML service option.
    * Function BrCtl.h
    * void CBrCtl::WmlServiceOptionsAddItemL
        * @since 3.1.
        * @return void
        */

        virtual void setWmlServiceOptionsAddItemL(const TText* aText, TUint32 aElemID, TInt aElType) = 0;
 
    /**
        * Request the Browser Control to request the host application to update softkeys.
    * Function BrCtl.h
    * void CBrCtl::UpdateSoftkeys()
        * @since 3.1.
        * @return void
        */

        virtual void setUpdateSoftkeys(void) = 0;

    /**
        * Browser Control posts the data to the specified url.
    * Function BrCtl.h
    * void CBrCtl::PostUrlL(..)
        * @since 3.1.
        * @return void
        */


    virtual void setPostUrlL(TDesC& aUrl, TDesC8& aPostData, TDesC& aContentType) = 0;

    /**
        * Convert a file name to a url.
    * Function BrCtl.h
    * HBufC* CBrCtl::FileNameToUrlLC(..)
        * @since 3.1.
        * @return HBufC*
        */

    virtual HBufC* fileNameToUrlLC(const TDesC& aFileName) = 0;

    /**
        * Set the WKScrollingProviderNotifyLayoutChange.
    * Function BrCtlDialogsProvider.h
    * void CBrCtlScrollingProvider::NotifyLayoutChange(..)
        * @since 3.1.
        * @return void
        */

      virtual void scrollingProviderNotifyLayoutChange(TBrCtlLayout aLayout) = 0;

    /**
        * Set the SetWKScrollingProviderUpdateVScrollBarL.
    * Function BrCtlDialogsProvider.h
    * void CBrCtlScrollingProvider::UpdateVScrollBarL(..)
        * @since 3.1.
        * @return void
        */


    virtual void setScrollingProviderUpdateVScrollBarL( TInt aDocumentHeight,
                TInt aDisplayHeight,
                TInt aDisplayPosY ) = 0;

    /**
        * Set the SetWKScrollingProviderUpdateHScrollBarL
    * Function BrCtlDialogsProvider.h
    * void CBrCtlScrollingProvider::UpdateHScrollBarL(..)
        * @since 3.1.
        * @return void
        */


    virtual void setScrollingProviderUpdateHScrollBarL( TInt aDocumentWidth,
                                TInt aDisplayWidth,
                                TInt aDisplayPosX ) = 0;  

    virtual void updateScrollbars(int documentHeight, int displayHeight, int displayPosY,
                int documentWidth, int displayWidth, int displayPosX) = 0;

     /**
        * Used by and defined in support of CWKDialogProvider
    * Function BrCtlDialogsProvider.h
    * CBrCtlObjectInfo::CWKObjectInfo() in
    * [WEBKIT\BrowserControl\src\WKDialogsProviderUtils.cpp]
        * @since 3.1.
        * @return CBrCtlObjectInfo*
        */

    virtual CBrCtlObjectInfo* objectInfoCreateL() = 0;
    

        /**
        * Loads URL and returns the handle to resource
    * Function WebKitLoader.h
    * CWmlResourceLoadListener* CWebKitLoader::LoadWmlResourceL(..)
        * @since 3.1.
        * @return CWmlResourceLoadListener*
        */


    //virtual CWmlResourceLoadListener* WKWmlResourceLoadListenerLoadWmlResourceL(CWmlContentInterface& aWmlContentInterface, const TDesC& aURL) = 0;
    virtual void resourceLoadListenerLoadWmlResourceL(const TDesC& aURL) = 0;

        /**
        * Set the URL
    * Function LoadListeners.h
    * void CWmlResourceLoadListener::SetUrlL(..)
        * @since 3.1.
        * @return void
        */ 

    virtual void resourceLoadListenerSetUrlL(/*CWmlResourceLoadListener* aListener,*/const TDesC& aUrl) = 0;

    /**
        * Find if a specific url is in cache
    * Function \S60\WebEngine\WEBKIT\BrowserControl\inc\BrCtl.h
    * TBool BrCtl::IsUrlInCache(TDesC& aUrl)
        * @since 3.1.
        * @return TBool
        */       
    virtual TBool wmlIsUrlInCache(TDesC& aUrl) = 0;

       /**
        * Find if a specific url is in cache
    * Function Function WebKitLoader.h
    * TBool void LoadPageL( const TDesC& aURL, const TDesC& aRereferrer, TBool aReload,
        *             TBool aOnLoad, const TDesC& aTarget, TEventCode aTriggeringEvent );
        * @since 3.1.
        * @return TBool
        */       

        virtual void wmlLoadUrlL( const TDesC& aURL, TDesC& aRereferrer, TInt aApId,
                               TBrCtlDefs::TBrCtlCacheMode aBrCtlCacheMode) = 0;

        virtual void loadResource(TDesC8& url, CWmlContentInterface* content) = 0;

    };

#endif      // BRCTLWMLINTERFACE_H
            
// End of File
