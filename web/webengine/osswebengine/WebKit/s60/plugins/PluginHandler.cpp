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


// INCLUDE FILES
#include <e32std.h>
#include <e32uid.h>
#include <sysutil.h>
#include "config.h"
#include "../../bidi.h"

// System includes
#include <ecom/ecom.h>


//  INCLUDES
#include "WebCorePluginHandler.h"
#include "NpnImplementation.h"
#include "PluginHandler.h"

// CONSTANTS

const TInt KPluginGranularity = 3;
const TInt KMimeTypeLength = 7;

_LIT( KMimeTypeAudio, "audio/" );
_LIT( KMimeTypeVideo, "video/" );

// LOCAL FUNCTION PROTOTYPES
typedef NPError (*NPP_InitializeFunc)(NPNetscapeFuncs *netscapeFuncs,
                                      NPPluginFuncs *pluginFuncs);
typedef void (*NPP_ShutdownFunc)(void);

#define LOAD_PLUGINS  if(!m_pluginsLoaded) { TRAP_IGNORE(loadPluginsL()); m_pluginsLoaded = ETrue; }


// ============================ MEMBER FUNCTIONS ===============================

void panicHandler(TInt error = KErrNone)
{
    User::Panic(_L("Object Handling Panic"), error);
}


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PluginInfo::PluginInfo
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
PluginInfo::PluginInfo()
    : m_mimeTypes(KPluginGranularity),
      m_mimeFileExtensions(KPluginGranularity),
      m_mimeExtensionToTypeMap(KPluginGranularity),
      m_mimeDescriptions(KPluginGranularity),
      m_handle(-1)
{
}

// -----------------------------------------------------------------------------
// PluginInfo::~PluginInfo
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
PluginInfo::~PluginInfo()
{
    m_mimeDescriptions.ResetAndDestroy();
    m_mimeDescriptions.Close();

    // Don't ResetAndDestroy() the m_mimeExtensionToTypeMap array, this array
    // contains pointers (memory) that is cleaned up in the m_mimeTypes array
    m_mimeExtensionToTypeMap.Close();

    m_mimeFileExtensions.ResetAndDestroy();
    m_mimeFileExtensions.Close();

    m_mimeTypes.ResetAndDestroy();
    m_mimeTypes.Close();

    delete m_name;
    delete m_description;
    delete m_nppFuncs;
    delete m_filename;

}

// -----------------------------------------------------------------------------
// PluginInfo::ShouldLoad
//
// Returns ETrue if the plugin should be loaded.
// -----------------------------------------------------------------------------
//
TBool PluginInfo::shouldLoad()
{
    return m_ref < 1;
}

// -----------------------------------------------------------------------------
// PluginInfo::ShouldUnload
//
// Returns ETrue if the plugin should be unloaded.
// -----------------------------------------------------------------------------
//
TBool PluginInfo::shouldUnload()
{
    return m_ref == 1;
}

// -----------------------------------------------------------------------------
// PluginInfo::LoadInstance
//
// Increments the PluginInfo's reference count
// -----------------------------------------------------------------------------
//
void PluginInfo::loadInstance()
{
    m_ref++;
}

// -----------------------------------------------------------------------------
// PluginInfo::UnloadInstance
//
// Decrements the PluginInfo's reference count.
// -----------------------------------------------------------------------------
//
void PluginInfo::unloadInstance()
{
    m_ref--;
}

// -----------------------------------------------------------------------------
// PluginInfo::CopyMimeDescription
//
// Copy the mimetypes and file extention from one plugin info struct to another,
// Called after a rescan of the file system
// -----------------------------------------------------------------------------
//
void PluginInfo::copyMimeDescription(PluginInfo& pluginInfo)
{
    TUint   i;
    TUint   count;
    HBufC*  entry;
    
    m_mimeTypes.ResetAndDestroy();
    m_mimeExtensionToTypeMap.Reset();
    count = pluginInfo.m_mimeTypes.Count();
    for (i = 0; i < count; i++) {
        entry = pluginInfo.m_mimeTypes[0]->Des().Alloc();
        pluginInfo.m_mimeTypes.Remove(0);
        m_mimeTypes.Append(entry);
    }

    HBufC8*  extEntry;
    m_mimeFileExtensions.ResetAndDestroy();
    count = pluginInfo.m_mimeFileExtensions.Count();
    for (i = 0; i < count; i++) {
        extEntry = pluginInfo.m_mimeFileExtensions[0]->Des().Alloc();
        pluginInfo.m_mimeFileExtensions.Remove(0);
        m_mimeFileExtensions.Append(extEntry);
        m_mimeExtensionToTypeMap.Append(entry);
    }
}

// -----------------------------------------------------------------------------
// PluginInfo::GetMimeExtensionsL
//
// Gets the supported mimeExtensions for the mimeType presented. Returns a HBufC*
// that has allocated memory, which is the responsibility of the caller to
// clean up. The HBufC* will be NULL if no supported mimeExtensions are supported.
// -----------------------------------------------------------------------------
//
HBufC8* PluginInfo::getMimeExtensionsL(const TDesC& mimeType)
{    
    HBufC8* mimeTypeBuf = HBufC8::NewLC(128);
    TPtr8  mimeTypePtr(mimeTypeBuf->Des());
    //
    for (TInt i = 0; i < m_mimeExtensionToTypeMap.Count(); i++) {

        if (*(m_mimeExtensionToTypeMap[i]) == mimeType) {
            // We found a mimeType match, save the mimeExtension
            HBufC8* mimeExtensionBuf = m_mimeFileExtensions[i];
            // Make sure we can fit this mimeFileExtension
            TInt length = mimeExtensionBuf->Length();
            // +1 is for ','
            if (mimeTypePtr.Length() + length + 1 > mimeTypePtr.MaxLength()) {                    
                HBufC8* tmp = HBufC8::NewL(mimeTypePtr.Length() + 2 * length);    
                mimeTypePtr.Set(tmp->Des());
                mimeTypePtr.Copy(*mimeTypeBuf);                
                CleanupStack::PopAndDestroy(); // mimeTypeBuf
                mimeTypeBuf = tmp;
                CleanupStack::PushL(mimeTypeBuf);
            }                
            // append ',' unless this is the first item in the list
            if (mimeTypePtr.Length())
                mimeTypePtr.Append(',');
    
            mimeTypePtr.Append(*mimeExtensionBuf);
        }
    }

    CleanupStack::Pop(); //mimeTypeBuf 
    return mimeTypeBuf;
}

// -----------------------------------------------------------------------------
// PluginInfo::ParseDefaultDataL(const TDesC8& mimeDesc)
//
// Parses the default_data string from resource file to populate the mimeTypes, mimeExtensions,
// and mimeDescriptions arrays.
// -----------------------------------------------------------------------------
//
void PluginInfo::parseDefaultDataL(const TDesC8& mimeDesc)
{
    __ASSERT_DEBUG(mimeDesc.Ptr() != NULL, panicHandler());

    _LIT(KPattern1Ptr, ",;|");
    _LIT(KPattern2Ptr, ";|");

    TUint    end(mimeDesc.Length());
    TUint    i(0);
    TUint    marker(0);
    TUint16  mimeSeparator('|');
    TUint    fieldSeparator(';');
    HBufC*   newMimeTypeEntry = NULL;
    HBufC8*  newMimeFileExtEntry = NULL;
    HBufC*   newMimeDescriptionEntry = NULL;

    // Parse the mimeDesc string to populate the mimeTypes, mimeExtensions,
    // and mimeDescriptions arrays.
    
    for (; i < end;)  { // outer for loop
        // Search until end of buffer or match one of the delimiters ';' or '|'.
        // We are looking for the mimeType, ie "text/html", "application/pdf", etc.
        for (; (i < end) && (KPattern2Ptr().Locate((mimeDesc)[i]) == KErrNotFound); i++) {
            // Walking the mimeDesc string
        }

        if (i > marker) {
            // Create new mimeType entry, the first entry is mimeType
            TPtrC8 mimeType8(mimeDesc.Mid(marker,i-marker));
            newMimeTypeEntry = HBufC::NewLC(i-marker);
            newMimeTypeEntry->Des().Copy(mimeType8);
            User::LeaveIfError(m_mimeTypes.Append(newMimeTypeEntry));
            CleanupStack::Pop(); //pop newMimeTypeEntry
        }

        // Are we at the end of the supported mime string
        if (i == end) {
            // This break with i=end means we leave outer for loop
            break;
        }

        marker = ++i;
        if ((mimeDesc)[i - 1] == mimeSeparator) {
            // Found a mime separator '|', get next supported mime
            continue;
        }

        // We are looking for the mimeFileExtensions, ie "html", "htm", "pdf", "txt", etc.
        // There can be multiple mimeFileExtentions per mimeType
        for (; (i < end);) { // inner for loop
            
            // Search until end of buffer or match one of the delimiters ';' or ',' or '|'.
            for (; (i < end) && (KPattern1Ptr().Locate((mimeDesc)[i]) == KErrNotFound); i++) {
                // Walking the mimeDesc string
            }

            if (i > marker) {
                TPtrC8 mimeFileExt8(mimeDesc.Mid(marker,i-marker));
                newMimeFileExtEntry = HBufC8::NewLC(i-marker);
                newMimeFileExtEntry->Des().Copy(mimeFileExt8);
                User::LeaveIfError(m_mimeFileExtensions.Append(newMimeFileExtEntry));
                CleanupStack::Pop(); //pop newMimeFileExtEntry

                // Add the mimeType to the mapping array. Every mimeFileExtension entry
                // in m_mimeFileExtensions, has a corresponding mimeType entry in
                // m_mimeExtensionToTypeMap.  This allows us to provide a list of
                // extensions for each supported mimeType.
                User::LeaveIfError(m_mimeExtensionToTypeMap.Append(newMimeTypeEntry));
            }

            // Are we at the end of the supported mime string
            if (i == end) {
                // This break means we leave the inner loop, and with i=end means
                // we leave the outer loop
                break;
            }

            marker = ++i;
            if ((mimeDesc)[i - 1] == mimeSeparator) {
                // Found a mime separator '|', get next supported mime
                break;
            }

            if ((mimeDesc)[i - 1] == fieldSeparator) {
            
                // Found a field separator ';', get the mimeDescription.
                // Search until end of buffer or match one of the delimiters ';' or '|'.
                for (; (i < end) && (KPattern2Ptr().Locate((mimeDesc)[i]) == KErrNotFound); i++) {
                    // Walking the mimeDesc string
                }

                if (i > marker) {
                    // Create new mimeDescription entry
                    TPtrC8 mimeDesc8(mimeDesc.Mid(marker,i-marker));
                    newMimeDescriptionEntry = HBufC::NewLC(i-marker);
                    newMimeDescriptionEntry->Des().Copy(mimeDesc8);

                    User::LeaveIfError(m_mimeDescriptions.Append(newMimeDescriptionEntry));
                    CleanupStack::Pop();  //pop newMimeDescriptionEntry
                }

                // Are we at the end of the supported mime string
                if (i == end) {
                    // This break means we leave the inner loop, and with i=end means
                    // we leave the outer loop
                    break;
                }

                marker = ++i;
                // Make sure we start at the next mime, after we found the mimeDescription.
                // We are handling a mime string with an extra semi-colon(s),
                // ie "...mimeDescription1;|mimeType2
                for (; (i < end) && ((mimeDesc)[i-1] != mimeSeparator); i++) {
                    // Walking the mimeDesc string
                }

                // Leave the inner loop and look for the next mime
                break;
            
            }   // end of if fieldSeparator aka get mimeDescription

            // If we get here, we have another mimeFileExtension.  Continue on the
            // inner loop to find additional mimeFileExtensions.

        }   // end of inner for (;i<end;)
    }   // end of outer for (;i<end;)
}


// -----------------------------------------------------------------------------
// PluginInfo::ParseDisplayNameL(const TDesC& nameDesc)
//
// Parses the display_name string from resource file to populate the file name and plugin name
// -----------------------------------------------------------------------------
//
void PluginInfo::parseDisplayNameL(const TDesC& nameDesc)
{
    __ASSERT_DEBUG(nameDesc.Ptr() != NULL, panicHandler());

    _LIT16(KSeparator, ";");

    TUint    end(nameDesc.Length());
    TInt     offset;
    TPtrC fileNamePtr;
    TPtrC namePtr;

    offset = nameDesc.Find(KSeparator);
    if (offset != KErrNotFound) {
        if (offset) {
            fileNamePtr.Set(nameDesc.Left(offset));
            namePtr.Set(nameDesc.Right(end - offset - 1));
            m_filename = fileNamePtr.AllocL();
            m_name = namePtr.AllocL();
        }
    }

}

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// PluginHandler::PluginHandler
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
PluginHandler::PluginHandler(TBool enablePlugins)
    : m_pluginInfoArray( KPluginGranularity )
      ,m_enablePlugins(enablePlugins)
      ,m_asyncLoading(ETrue)
      ,m_pluginToActivate(NULL)
      ,m_activePlugin(NULL)
{
}


// -----------------------------------------------------------------------------
// PluginHandler::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void PluginHandler::ConstructL()
{
    m_idle = CIdle::NewL(CActive::EPriorityLow);
    m_idle->Start(TCallBack(initialize, this));
}


// -----------------------------------------------------------------------------
// PluginHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
PluginHandler* PluginHandler::NewL(TBool enablePlugins)
{
    PluginHandler* self = new(ELeave) PluginHandler(enablePlugins);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// PluginHandler::~PluginHandler
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
PluginHandler::~PluginHandler()
{
    m_pluginInfoArray.ResetAndDestroy();
    m_pluginInfoArray.Close();
    m_pluginObjects.clear();
    if (m_idle) {
       m_idle->Cancel();
    }
    delete m_idle;
}

// -----------------------------------------------------------------------------
// PluginHandler::Initialize
//
// Initializes the PluginHandler, possibly asynchronously
// -----------------------------------------------------------------------------
//
TBool PluginHandler::initialize(TAny* pluginHandler)
{
    TBool doneInit(EFalse);

    PluginHandler* handler = (PluginHandler*)pluginHandler;

    TRAPD(err, doneInit = handler->loadPluginsL());
    if (err) {
        // If we leave from LoadPluginsL, stop CIdle
        doneInit = EFalse;
    }

    return doneInit;
}

//------------------------------------------------------------------------------
// PluginHandler::loadPluginsL
//
// Loads all the plugins and query them for details.
// -----------------------------------------------------------------------------
//
TBool PluginHandler::loadPluginsL()
{
    
    if(m_pluginsLoaded) {
        return EFalse;
    }
        
    // 1) fetch CImplementationInformation instances from the ECOM infrastructure
    // 2) create plugin info for each plugin

    // Create the ECom info array, contains the plugin information
    RImplInfoPtrArray ecomPluginInfoArray;

    // Get list of ECOM plugins that match the KNBrowserPluginInterfaceUid
    REComSession::ListImplementationsL(KBrowserPluginInterfaceUid, ecomPluginInfoArray);

    // Loop through all the Netscape plugins found by ECom and populate the
    // m_pluginInfoArray
    m_nextHandle = 0;
    const TInt pluginCount = ecomPluginInfoArray.Count();
    for (TInt index=0; index<pluginCount; ++index) {
        PluginInfo* pluginInfo = new(ELeave) PluginInfo;
        pluginInfo->m_handle = m_nextHandle++;
        CleanupStack::PushL(pluginInfo);

        // Set the plugin properties
        pluginInfo->m_pluginInfo = ecomPluginInfoArray[index];
        // Read the plugin's name and plugin FileName (display_name) from the IMPLEMENTATION_INFO
        pluginInfo->parseDisplayNameL(pluginInfo->m_pluginInfo->DisplayName());
        pluginInfo->m_description = HBufC::NewL(pluginInfo->m_pluginInfo->OpaqueData().Length());
        pluginInfo->m_description->Des().Copy(pluginInfo->m_pluginInfo->OpaqueData());
        pluginInfo->m_uid = pluginInfo->m_pluginInfo->ImplementationUid();
        // Parses the mimeDesc string to populate the mimeTypes, mimeExtensions,
        // and mimeDescriptions arrays.
        pluginInfo->parseDefaultDataL(pluginInfo->m_pluginInfo->DataType());

        // Add the filter information to the list
        User::LeaveIfError(m_pluginInfoArray.Append(pluginInfo));
        CleanupStack::Pop();    // pluginInfo
    }

    // Clean up the ECom info array
    ecomPluginInfoArray.ResetAndDestroy();
    ecomPluginInfoArray.Close();
    
    m_pluginsLoaded = ETrue;
    return EFalse;
}

// -----------------------------------------------------------------------------
// PluginHandler::findPlugin
//
// Find a plugin by MIME type.
// -----------------------------------------------------------------------------
//
TInt PluginHandler::findPlugin(const TDesC& mimeType) 
   {    
    LOAD_PLUGINS

    TInt bufSize( KMimeTypeLength > mimeType.Length() ? KMimeTypeLength : mimeType.Length() );
    HBufC* newMimeType = HBufC::New(bufSize);
    if( newMimeType == NULL )
        {
        return KErrNoMemory;
        }
    _LIT(KSeparator, "*");
    TInt  pluginIndex;
    TInt  mimeIndex;

    if( mimeType.FindF(KMimeTypeAudio) != KErrNotFound )
        {
        newMimeType->Des().Copy(KMimeTypeAudio);
        newMimeType->Des().Append(KSeparator);
        }
    else if ( mimeType.FindF(KMimeTypeVideo) != KErrNotFound )
        {
        newMimeType->Des().Copy(KMimeTypeVideo);
        newMimeType->Des().Append(KSeparator);
        }
    else
        {
        newMimeType->Des().Copy(mimeType);
        }

    for (pluginIndex = 0; pluginIndex < m_pluginInfoArray.Count(); pluginIndex++) 
        {
        for (mimeIndex = 0;
             mimeIndex < m_pluginInfoArray[pluginIndex]->m_mimeTypes.Count();
             mimeIndex++) 
            {
            if (!m_pluginInfoArray[pluginIndex]->m_mimeTypes[mimeIndex]->CompareF(*newMimeType))
                {
                delete newMimeType;
                return m_pluginInfoArray[pluginIndex]->m_handle;
                }
            }
        }
    delete newMimeType;
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// PluginHandler::findPluginByExtension
//
// Find a plugin by file extension.
// -----------------------------------------------------------------------------
//
TInt PluginHandler::findPluginByExtension(const TDesC8& url)
{
    TInt idx = -1;
    PluginInfo* pluginInfo = pluginInfoByExtention(url, &idx);
    TInt ret = pluginInfo ? pluginInfo->m_handle : KErrNotFound;
    return ret;
}


// -----------------------------------------------------------------------------
//  PluginHandler::pluginInfoByExtention
//  aUrl - url that should be handle by plugin
//  aExtIdx - return value of the index in the m_mimeFileExtensions for the extention
//  returns a pointer to the PluginInfo object of the plugin that should handle aUrl
// -----------------------------------------------------------------------------
PluginInfo* PluginHandler::pluginInfoByExtention(const TPtrC8& aUrl, TInt* aExtIdx)
{
    LOAD_PLUGINS
    
    TInt  pluginIndex;
    TInt  extIndex;

    TPtrC8 extPtr(aUrl.Mid(aUrl.LocateReverse('.') + 1));
    for (pluginIndex = 0; pluginIndex < m_pluginInfoArray.Count(); pluginIndex++) {
        
        for (extIndex = 0; 
            extIndex < m_pluginInfoArray[pluginIndex]->
            m_mimeFileExtensions.Count(); extIndex++) {
            
            if (!m_pluginInfoArray[pluginIndex]->m_mimeFileExtensions[extIndex]->CompareF(extPtr)) {
                *aExtIdx = extIndex;
                return m_pluginInfoArray[pluginIndex];
            }                
        }
    }

    return NULL;
}


// -----------------------------------------------------------------------------
// PluginHandler::pluginMimeByExtention
// aUrl - url that should be handle by plugin
// returns pointer to mime type in m_mimeExtensionToTypeMap array
// -----------------------------------------------------------------------------
HBufC* PluginHandler::pluginMimeByExtention(const TPtrC8& url)
{
    TInt idx = -1;
    PluginInfo* pluginInfo = pluginInfoByExtention(url, &idx);
    HBufC* mimeType = NULL;
    if (pluginInfo && idx < pluginInfo->m_mimeExtensionToTypeMap.Count()) {
        mimeType = pluginInfo->m_mimeExtensionToTypeMap[idx];
    }
    return mimeType;
}


// -----------------------------------------------------------------------------
// PluginHandler::loadPluginL
//
// Load a plugin if not yet loaded and return its function table. The handle was
// gotten from the methods FindPlugin and FindPluginFromExtension, so the
// handle should be valid.
// -----------------------------------------------------------------------------
//
void PluginHandler::loadPluginL(TInt handle, NPPluginFuncs** retPluginFuncs)
{
    LOAD_PLUGINS
    
    TUint index(getIndexFromHandle(handle));
    __ASSERT_DEBUG(index < (TUint)m_pluginInfoArray.Count(), panicHandler());

    PluginInfo* plugin = m_pluginInfoArray[index];

    if (plugin->shouldLoad()) {
    
        NPPluginFuncs* pluginFuncs = (NPPluginFuncs*) User::AllocLC(sizeof (NPPluginFuncs));
        Mem::FillZ((void*) pluginFuncs, sizeof (NPPluginFuncs));
        // create CEcomBrowserPluginInterface, given the UID from the PLuginInfo, when ref count == 0
        plugin->m_pluginInterface = CEcomBrowserPluginInterface::CreatePluginL(plugin->m_uid,
          (NPNetscapeFuncs *) (&NpnImplementationFuncs), pluginFuncs);

        CleanupStack::Pop(pluginFuncs);
        plugin->m_nppFuncs = pluginFuncs;
        pluginFuncs = NULL;
    }
    
    plugin->loadInstance();
    __ASSERT_DEBUG(plugin->m_nppFuncs != NULL, panicHandler());
    *retPluginFuncs = plugin->m_nppFuncs;

}


// -----------------------------------------------------------------------------
// PluginHandler::UnloadPlugin
//
// Unload a plugin. If this is the last instance, the library is unloaded.
// The handle was gotten from the methods FindPlugin and FindPluginFromExtension,
// so the handle should be valid.
// -----------------------------------------------------------------------------
//
void PluginHandler::unloadPlugin(TInt handle)
{
    TUint index(getIndexFromHandle(handle));
    __ASSERT_DEBUG(index < (TUint) m_pluginInfoArray.Count(), panicHandler());

    PluginInfo* plugin = m_pluginInfoArray[index];

    if (plugin->shouldUnload()) {
        User::Free(plugin->m_nppFuncs);
        plugin->m_nppFuncs = NULL;
        // delete the CEcomBrowserPluginInterface instance when ref count  == 0
        delete plugin->m_pluginInterface;
    }

    plugin->unloadInstance();
}

// -----------------------------------------------------------------------------
// PluginHandler::GetIndexFromHandle
//
// Get the index of plugin info from its handle. This method should always
// return a valid handle, since the calling program got the handle from the
// methods FindPlugin and FindPluginFromExtension.
// -----------------------------------------------------------------------------
//
TUint PluginHandler::getIndexFromHandle(TInt handle)
{
    LOAD_PLUGINS
    TUint  index;
    TUint  count(m_pluginInfoArray.Count());

    __ASSERT_DEBUG(handle >= 0, panicHandler());

    for (index = 0; index < count; index++) {
        if (m_pluginInfoArray[index]->m_handle == handle) {
            return index;
        }
    }

    // We should never get here. We should always return a valid handle. Return
    // the count (handle + 1), this should assert debug in calling methods, as
    // well as the forced assert debug .
    __ASSERT_DEBUG(EFalse, panicHandler());
    return count;
}

// -----------------------------------------------------------------------------
// PluginHandler::GetPluginCount
//
// Returns the number of plugins supported by the platform
// -----------------------------------------------------------------------------
TInt PluginHandler::getPluginCount()
{
    LOAD_PLUGINS
    return m_pluginInfoArray.Count();
}
    
TBool PluginHandler::pluginSupportsMIMEType(const TDesC& mimeType)
{
    return findPlugin(mimeType) != KErrNotFound;
}

TBool PluginHandler::objectAtIndex(TInt index, TWebCorePluginInfo& pluginInfo )
{
    if (index<0 || index>m_pluginInfoArray.Count()-1)
        return EFalse;

    PluginInfo* pinfo = m_pluginInfoArray[index];
    if (!pinfo)
        return EFalse;

    pluginInfo.iName.Set(KNullDesC());
    pluginInfo.iFilename.Set(KNullDesC());
    pluginInfo.iPluginDescription.Set(KNullDesC());
    //
    if (pinfo->name())        
        pluginInfo.iName.Set(*(pinfo->name()));
    if (pinfo->fileName())
        pluginInfo.iFilename.Set(*(pinfo->fileName()));
    if (pinfo->description())
        pluginInfo.iPluginDescription.Set(*(pinfo->description()));

    pluginInfo.iMimeTypeArray = pinfo->mimeTypes();

    return ETrue;        
}

void PluginHandler::refreshPlugins(TBool reload)
{
    TRAP_IGNORE(reloadPluginsL(reload));
}

HBufC8* PluginHandler::extensionsForMIMEType(TInt index, TDesC& mimeType )
{
    PluginInfo *pluginInfo = m_pluginInfoArray[index];
    HBufC8 *extn = NULL;
    TRAP_IGNORE(extn = pluginInfo->getMimeExtensionsL(mimeType));
    return extn;
}

HBufC* PluginHandler::descriptionForMIMEType(TInt index, TDesC& mimeType )
{
    // there is no way to tell mimeType-description pairs. return the whole string    
    PluginInfo *pluginInfo = m_pluginInfoArray[index];
    RPointerArray<HBufC> items = pluginInfo->mimeDescriptions();
    HBufC* descBuf = HBufC::New(128);
    if (!descBuf)
        return descBuf;
        
    TPtr descPtr(descBuf->Des());
    for (TInt i=0; i<items.Count(); i++ ) {
        
        // Make sure we can fit this mimeFileExtension
        HBufC* desc = items[i];
        TInt length = desc->Length();
		// +1 is for ','
        if (descPtr.Length() + length + 1 > descPtr.MaxLength()) {                    
            HBufC* tmp = HBufC::New(descPtr.Length() + 2 * length);    
            if (!tmp) {
                // return whatever we have    
                return descBuf;
            }
            descPtr.Set(tmp->Des());
            descPtr.Copy(*descBuf);                
            descBuf = tmp;
        }                
        // append ',' unless this is the first item in the list
        if (descPtr.Length())
            descPtr.Append(',');
        descPtr.Append(*desc);
    }
    
    return descBuf;
}

// -----------------------------------------------------------------------------
// PluginHandler::ReloadPluginsL
//
// Rescan for new plugins.
// -----------------------------------------------------------------------------
//
void PluginHandler::reloadPluginsL(TBool reload)
{
    // This function stores the plugins that are currently loaded in a temp array
    // Then it rescans the drives for all plugins
    // Currently loaded plugins must exist in the new list because the dll is loaded
    // and the plugin could not be deleted from the file system while the dll is loaded.
    // After rescanning, each plugin from the temp array is matched with a plugin
    // in the new array. The matching is done by file name.
    // Each new entry is replaced with the existing entry from the temp array.
    // This is done because the CPluginInst object keeps the handle and
    // the function table of the existing plugin.

    RPointerArray<PluginInfo>  tmpPluginInfoArray;
    PluginInfo*                 pluginInfo = NULL;
    TInt                        count(m_pluginInfoArray.Count());
    TInt                        index;

    // Copy all the loaded plugins to a temp array
    for (index = 0; index < count; index++) {
        pluginInfo = m_pluginInfoArray[0];
        if (pluginInfo->shouldLoad()) { // Is it currently loaded?
            m_pluginInfoArray.Remove(0);
            delete pluginInfo;
            pluginInfo = NULL;
        }
        else {
            m_pluginInfoArray.Remove(0);
            tmpPluginInfoArray.Append(pluginInfo);
        }
    }
        
    // reload the plugins by scanning the drives
    if (m_idle) {
        m_idle->Cancel();
    }

    //Load the plugins synchronously
    m_asyncLoading = EFalse;
    m_pluginsLoaded =EFalse;
    loadPluginsL();
    count = m_pluginInfoArray.Count();

    TInt tmpCount(tmpPluginInfoArray.Count());
    TInt i;
    PluginInfo* delPluginInfo = NULL;
    for (index = 0; index < tmpCount; index++) {
        pluginInfo = tmpPluginInfoArray[0];
        tmpPluginInfoArray.Remove(0);
        // Search through the array for a filename match
        // use ECOM Uid rather then file names
        for (i = 0; i < count && (pluginInfo->m_uid != m_pluginInfoArray[i]->m_uid); i++) {
        }

        if (i == count) {
            i = -1;
        }
        
        if (i >= 0) {
            // i < 0 only if rescanning failed
            delPluginInfo = m_pluginInfoArray[i];
            m_pluginInfoArray.Remove(i);
            // Save the new Mimetype and extensions. They might be different than before
            pluginInfo->copyMimeDescription(*delPluginInfo);

            delete (delPluginInfo);
            delPluginInfo = NULL;
        }
    
        m_pluginInfoArray.Append(pluginInfo);    
    }

    tmpPluginInfoArray.Close();
    if (reload) {
        //m_view.mainFrame().WebKitBridge().reload();
    }

}

// -----------------------------------------------------------------------------
// PluginHandler::IsSupported
//
// Determines whether a given content-type of dot-extension has a cooresponding
// plugin or external application.  aContentType and aUrl can be NULL, but at least
// one must be non-NULL (otherwise it will always return EFalse).  If handlerType
// is EPlugin then it returns ETrue only if their is a supported plugin.  If handlerType
// is EExternalApp then it returns ETrue only if their is a supported external
// application.  If handlerType is EBoth then it returns ETrue only if their is a
// supported plugin or external application.
// -----------------------------------------------------------------------------
TBool PluginHandler::isSupported(const TDesC& contentType, const TDesC8& url)
{
    TBool isSupported(EFalse);

    // Do we have a plugin that supports this content type
	if (contentType.FindF(KMimeTypeAudio) != KErrNotFound || contentType.FindF(KMimeTypeVideo) != KErrNotFound 
			|| findPlugin(contentType) != KErrNotFound || findPluginByExtension(url) != KErrNotFound) {
		isSupported = ETrue;
	}
    // We don't have a plugin to support this content
    return isSupported;
}

void PluginHandler::storePluginObject(PluginSkin* pluginObj)
{
    m_pluginObjects.add(pluginObj);    
}
void  PluginHandler::removePluginObject(PluginSkin* pluginObj)
{
    m_pluginObjects.remove(pluginObj);    
}
//  End of File
