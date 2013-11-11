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
* Description:  Plugin handling, manages the loading and unloading of the
*                plugin. Initializes the CInfoArray that contains the plugin
*                MIME-types supported.
*
*/

#ifndef PluginHandler_H
#define PluginHandler_H

//  INCLUDES
#include <e32std.h>
#include <npupp.h>
#include <f32file.h>
#include <badesca.h>
#include <wtf/HashSet.h>
#include <ecom/implementationinformation.h>    // ecom
#include "EcomBrowserPluginInterface.h"
#include "WebCorePluginHandler.h"

// FORWARD DECLARATIONS
class PluginInfo;
class WebView;
class PluginSkin;

// CLASS DECLARATION

/**
* Primarily deals with the loading and unloading of plugins as well as
* other plugin utility functions.
*
*  @lib Browser Engine
*  @since 3.1
*/
class PluginHandler : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aEnablePlugin flag to check for plugin support enabled by Browser Setting
        * @param aWebKitView Pointer to the browser view
        */
        static PluginHandler* NewL(TBool enablePlugin);

        /**
        * Destructor.
        */
        virtual ~PluginHandler();


    public: // New functions


        /**
        * Find a plugin by MIME type
        *
        * @since 3.1
        * @param aMIMEType The MIME type of the <object> element.
        * @return The handle of the plugin supporting the MIME type or KErrNotFound.
        */
        TInt findPlugin(const TDesC& mimeType);

        /**
        * Find a plugin by file extension
        *
        * @since 3.1
        * @param aFileName The url of the <object> element data.
        * @return The handle of the plugin supporting the file name or KErrNotFound.
        */
        TInt findPluginByExtension(const TDesC8& url);

        /**
        * Load a plugin if not yet loaded
        *
        * @since 3.1
        * @param aHandle The handle of the plugin, returned from FindPlugin.
        * @param aPluginFuncs Upon success it contains the associated
        *        plugin's function table.
        * @return void
        */

        void loadPluginL(TInt handle, NPPluginFuncs** retFuncs);

        /**
        * Unload a plugin. If this is the last instance, the library is unloaded.
        *
        * @since 3.1
        * @param aHandle The handle of the plugin, returned from FindPlugin.
        * @return void
        */
        void unloadPlugin(TInt handle);

        /**
        * Get the index of plugin info from its handle.
        *
        * @since 3.1
        * @param aHandle The handle of the plugin, returned from FindPlugin.
        * @return the index of the plugin info.
        */
        TUint getIndexFromHandle(TInt handle);

    public: //From MWebCorePluginHandler

        TInt getPluginCount();
        TBool pluginSupportsMIMEType(const TDesC& mimeType);
        TBool objectAtIndex(TInt index, TWebCorePluginInfo& aPluginInfo );
        void refreshPlugins(TBool reload);
        HBufC8* extensionsForMIMEType(TInt index, TDesC& mimeType);
        HBufC* descriptionForMIMEType(TInt index, TDesC& mimeType);

        /**
        * Determines whether a given content-type of dot-extension has a cooresponding
        * plugin or external application.  aContentType and aUrl can be NULL, but at least
        * one must be non-NULL (otherwise it will always return EFalse).  If aHandlerType
        * is EPlugin then it returns ETrue only if their is a supported plugin.  If aHandlerType
        * is EExternalApp then it returns ETrue only if their is a supported external
        * application.  If aHandlerType is EBoth then it returns ETrue only if their is a
        * supported plugin or external application.
        *
        * @since 3.1
        * @param aContentType the content type to compare against, may be NULL.
        * @param aUrl the url to extract the dot-extension and to compare against, may be NULL.
        *
        * @return ETrue or EFalse.
        */
        TBool isSupported(const TDesC& contType, const TDesC8& url);
        void  storePluginObject(PluginSkin* pluginObj);
        void  removePluginObject(PluginSkin* pluginObj);
        WTF::HashSet<PluginSkin*>& pluginObjects() { return m_pluginObjects; };
        PluginSkin*  pluginToActivate() { return m_pluginToActivate; };
        void  setPluginToActivate(PluginSkin* pluginObj) {m_pluginToActivate = pluginObj; };
        PluginSkin*  activePlugin() { return m_activePlugin; };
        void setActivePlugin(PluginSkin* pluginObj) { m_activePlugin = pluginObj; };
        HBufC* pluginMimeByExtention(const TPtrC8& url);
        PluginInfo* pluginInfoByExtention(const TPtrC8& aUrl, TInt* aExtIdx);

    private: // New functions

        /**
        * Scans for new plugins.
        *
        * @since 3.1
        * @param aReloadPages if ETrue the current page is reloaded.
        * @return void.
        */
        void reloadPluginsL(TBool reload);

        /**
        * C++ default constructor.
        * @param aEnablePlugin flag to check if plugin support is enabled by Browser Settings
        * @param aWebKitView pointer to CWebKitView
        */
        PluginHandler(TBool enablePlugin);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Initializes the PluginHandler, possibly asynchronously.
        *
        * @since 3.1
        * @return void.
        */
        static TBool initialize(TAny* pluginHandler);

        /**
        * Loads all the plugins and query them for details. Used during
        * initialization.
        *
        * @since 3.1
        * @return void.
        */
        TBool loadPluginsL();

    private:    // Data

        // Array of information about existing plugins
        RPointerArray<PluginInfo>   m_pluginInfoArray;
        WTF::HashSet<PluginSkin*>   m_pluginObjects;
        TInt                        m_nextHandle;
        TBool                       m_pluginsLoaded;
        TBool                       m_enablePlugins;
        TBool                       m_asyncLoading;      //ETrue for Async scanning of drives
                                                        //EFalse for sync scanning and loading

        CIdle*                      m_idle;              //Active Object to initailise
                                                        //the plugins from drives
        PluginSkin*                 m_pluginToActivate;
        PluginSkin*                 m_activePlugin;
    };


/**
* A private helper class used by PluginHandler which is used
* to interact with individual plugins.
*
* @lib Browser Engine
* @since 3.1
*/
class PluginInfo : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static PluginInfo* NewL(const TDesC& mimeDescription, const TDesC& name,
                const TDesC& description, const TDesC& fileName, const TDesC& pathName);

        /**
        * Destructor.
        */
        virtual ~PluginInfo();


    public:  // New functions

        /**
        * Returns ETrue if the plugin should be loaded.
        *
        * @since 3.1
        * @return ETrue or EFalse
        */
        TBool shouldLoad();

        /**
        * Returns ETrue if the plugin should be unloaded.
        *
        * @since 3.1
        * @return ETrue or EFalse
        */
        TBool shouldUnload();

        /**
        * Increments the PluginInfo's reference count.
        *
        * @since 3.1
        * @return void
        */
        void loadInstance();

        /**
        * Decrements the PluginInfo's reference count.
        *
        * @since 3.1
        * @return void
        */
        void unloadInstance();

        /**
        * Copy the mimetypes and file extention from one PluginInfo to another.
        * Called after a rescan of the file system.
        *
        * @since 3.1
        * @param aPluginInfo the instance to move mimetypes and file extentions from.
        * @return void.
        */
        void copyMimeDescription(PluginInfo& pluginInfo);

        /**
        * Getter function.
        *
        * @since 3.1
        * @return The mime types supported by this plugin
        */

        const RPointerArray <HBufC>&  mimeTypes() {return m_mimeTypes;}

        /**
        * Getter function.
        *
        * @since 3.1
        * @return The mime extensions supported by this plugin
        */

        const RPointerArray <HBufC8>&  mimeFileExtensions() {return m_mimeFileExtensions;}

        /**
        * Getter function.
        *
        * @since 3.1
        * @return The mime descriptions supported by this plugin
        */

        const RPointerArray <HBufC>&  mimeDescriptions() {return m_mimeDescriptions;}

        /**
        * Getter function.
        *
        * @since 3.1
        * @return The list of mimeExtensions given the mimeType supported by this plugin
        * The list of mimeExtensions is a comma-delimited string. For example, given
        * the mimeType "text/html", you will receive "html,htm".
        */

        HBufC8* getMimeExtensionsL(const TDesC& mimeType);

        /**
        * Getter function.
        *
        * @since 3.1
        * @return The name of this plugin
        */
        HBufC* name() {return m_name;}

        /**
        * Getter function.
        *
        * @since 3.1
        * @return The description of this plugin
        */
        HBufC* description() {return m_description;}

        /**
        * Getter function.
        *
        * @since 3.1
        * @return The file name of this plugin
        */
        HBufC* fileName() {return m_filename;}

        void parseDefaultDataL(const TDesC8& mimeDescription);
        void parseDisplayNameL(const TDesC& pluginDescription);

    private:  // Private Methods

        /**
        * C++ default constructor.
        */
        PluginInfo();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
    public:   //Data
        CImplementationInformation* m_pluginInfo;  // ECom plugin implementation info

    private:  // Data

        RPointerArray <HBufC>  m_mimeTypes;
        RPointerArray <HBufC8> m_mimeFileExtensions;
        RPointerArray <HBufC>  m_mimeExtensionToTypeMap;
        RPointerArray <HBufC>  m_mimeDescriptions;


        HBufC*                 m_name;
        HBufC*                 m_description;
        TInt                   m_ref;
        TInt                   m_handle;
        NPPluginFuncs*         m_nppFuncs;
        HBufC*                 m_filename;

        TUid                   m_uid;
        CEcomBrowserPluginInterface* m_pluginInterface;

    private:  // Friend classes

        friend class PluginHandler;
    };

#endif      // PluginHandler_H

// End of File

