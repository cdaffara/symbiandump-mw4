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
* Description:   Header file for MWKWmlInterface implementation.
*
*/



#ifndef WMLINTERFACEIMPL_H
#define WMLINTERFACEIMPL_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "WmlInterface.h"

#include "BrCtlDialogsProvider.h"

class CBrCtlObjectInfo;
class CWmlContentInterface;

//Array for start up.

const TUint wmlSettingTable[]={ 
                         TBrCtlDefs::ESettingsLaunchAppUid,
                         TBrCtlDefs::ESettingsLaunchViewId, 
                         TBrCtlDefs::ESettingsLaunchCustomMessageId,
                         TBrCtlDefs::ESettingsAutoLoadImages,
                         TBrCtlDefs::ESettingsEmbedded,
                         TBrCtlDefs::ESettingsCookiesEnabled,
                         TBrCtlDefs::ESettingsECMAScriptEnabled,
                         TBrCtlDefs::ESettingsIMEINotifyEnabled,
                         TBrCtlDefs::ESettingsSendRefererHeader,
                         TBrCtlDefs::ESettingsSecurityWarnings,
                         TBrCtlDefs::ESettingsPageOverview,
                         TBrCtlDefs::ESettingsBackList,
                         TBrCtlDefs::ESettingsAutoRefresh,
                         TBrCtlDefs::ESettingsTextWrapEnabled,
                         TBrCtlDefs::ESettingsCharacterset,
                         TBrCtlDefs::ESettingsCharacterset
                        };



/**
*  This observer is notified when the browser requests softkeys changes
*
*  @lib BrowserEngine.lib
*  @since 3.1
*/

NONSHARABLE_CLASS(CWmlInterfaceImpl):public CBase,public MWmlInterface
    {
    public: // New functions
        /**
        * Free all WML service options
    * Function \S60\WebEngine\WEBKIT\BrowserControl\inc\BrCtl.h
    * void CBrCtl::ResetWmlServiceOptions().
        * @since 3.1.
        * @return void
        */

         void resetWmlServiceOptions();

        /**
        * Add a WML service option.
    * Function \S60\WebEngine\WEBKIT\BrowserControl\inc\BrCtl.h
    * void CBrCtl::WmlServiceOptionsAddItemL
        * @since 3.1.
        * @return void
        */

        void setWmlServiceOptionsAddItemL(const TText* aText, TUint32 aElemID, TInt aElType);
 
    
    /**
        * Request the Browser Control to request the host application to update softkeys.
    * Function \S60\WebEngine\WEBKIT\BrowserControl\inc\BrCtl.h
    * void CBrCtl::UpdateSoftkeys()
        * @since 3.1.
        * @return void
        */


        void setUpdateSoftkeys(void);

    /**
        * Browser Control posts the data to the specified url.
    * Function \S60\WebEngine\WEBKIT\BrowserControl\inc\BrCtl.h
    * void CBrCtl::PostUrlL(..)
        * @since 3.1.
        * @return void
        */

      void setPostUrlL(TDesC& aUrl, TDesC8& aPostData, TDesC& aContentType);


    /**
        * Convert a file name to a url.
    * Function \S60\WebEngine\WEBKIT\BrowserControl\inc\BrCtl.h
    * HBufC* CBrCtl::fileNameToUrlLC(..)
        * @since 3.1.
        * @return HBufC*
        */


    HBufC* fileNameToUrlLC(const TDesC& aFileName);

    /**
        * Set the WKScrollingProviderNotifyLayoutChange.
    * Function \S60\WebEngine\WEBKIT\BrowserControl\inc\BrCtlDialogsProvider.h
    * void CBrCtlScrollingProvider::NotifyLayoutChange(..)
        * @since 3.1.
        * @return void
        */

    void scrollingProviderNotifyLayoutChange(TBrCtlLayout aLayout);

    /**
        * Set the SetWKScrollingProviderUpdateVScrollBarL.
    * Function \S60\WebEngine\WEBKIT\BrowserControl\inc\BrCtlDialogsProvider.h
    * void CBrCtlScrollingProvider::UpdateVScrollBarL(..)
        * @since 3.1.
        * @return void
        */


    void setScrollingProviderUpdateVScrollBarL( TInt aDocumentHeight,
                TInt aDisplayHeight,
                TInt aDisplayPosY);
    /**
        * Set the SetWKScrollingProviderUpdateHScrollBarL
    * Function \S60\WebEngine\WEBKIT\BrowserControl\inc\BrCtlDialogsProvider.h
    * void CBrCtlScrollingProvider::UpdateHScrollBarL(..)
        * @since 3.1.
        * @return void
        */

    void setScrollingProviderUpdateHScrollBarL( TInt aDocumentWidth,
                                TInt aDisplayPosX,
                TInt aDisplayPosY);



    void updateScrollbars(int documentHeight, int displayHeight, int displayPosY,
                int documentWidth, int displayWidth, int displayPosX);


    /**
        * Used by and defined in support of CWKDialogProvider
    * Function \S60\WebEngine\WEBKIT\BrowserControl\inc\BrCtlDialogsProvider.h
    * CBrCtlObjectInfo::CWKObjectInfo() in
    * [WEBKIT\BrowserControl\src\WKDialogsProviderUtils.cpp]
        * @since 3.1.
        * @return CBrCtlObjectInfo*
        */

    CBrCtlObjectInfo* objectInfoCreateL();

        /**
        * Loads URL and returns the handle to resource
    * Function \S60\WebEngine\WEBKIT\BrowserView\inc\WebKitLoader.h
    * CWmlResourceLoadListener* CWebKitLoader::LoadWmlResourceL(..)
        * @since 3.1.
        * @return CWmlResourceLoadListener*
        */
  

    //CWmlResourceLoadListener* WKWmlResourceLoadListenerLoadWmlResourceL(CWmlContentInterface& aWmlContentInterface,const TDesC& aURL);
    void resourceLoadListenerLoadWmlResourceL(const TDesC& aURL);

        /**
        * Set the URL
    * Function \S60\WebEngine\WEBKIT\BrowserView\inc\LoadListeners.h
    * void CWmlResourceLoadListener::SetUrlL(..)
        * @since 3.1.
        * @return void
        */         

    void resourceLoadListenerSetUrlL(/*CWmlResourceLoadListener* aListener,*/ const TDesC& aUrl);

        /**
        * Find if a specific url is in cache
    * Function \S60\WebEngine\WEBKIT\BrowserControl\inc\BrCtl.h
    * TBool BrCtl::IsUrlInCache(TDesC& aUrl)
        * @since 3.1.
        * @return TBool
        */       
    TBool wmlIsUrlInCache(TDesC& aUrl);


        /**
        * Find if a specific url is in cache
    * Function Function WebKitLoader.h
    * TBool void LoadPageL( const TDesC& aURL, const TDesC& aRereferrer, TBool aReload,
        *             TBool aOnLoad, const TDesC& aTarget, TEventCode aTriggeringEvent );
        * @since 3.1.
        * @return TBool
        */       

        void wmlLoadUrlL( const TDesC& aURL, TDesC& aRereferrer, TInt aApId,
                               TBrCtlDefs::TBrCtlCacheMode aBrCtlCacheMode );

    //Destructor
    virtual ~CWmlInterfaceImpl();


    CWmlInterfaceImpl(CBrCtl& aBrCtl);
    void loadResource(TDesC8& url,  CWmlContentInterface* content);


   

    private:
      //Browser control reference
      CBrCtl&   m_brCtl; //Not owned  
    };

#endif      // CBRCTLWMLINTERFACEIMPL_H  
            
// End of File
