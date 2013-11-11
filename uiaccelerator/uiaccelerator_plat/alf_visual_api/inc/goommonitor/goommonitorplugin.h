/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main functionality for Graphics Out of Memory Monitor
*
*/


#ifndef GOOMMONITORPLUGIN_H
#define GOOMMONITORPLUGIN_H

#include <e32base.h>

class CMemoryMonitor;
class RFs;
class RWsSession;

// temp support for migration
#define __GOOM_PLUGIN_API_V2

enum TGOomMonitorFlags
	{
	KGOomNone = 0,
	KGOomUseSwRendering
	};

/**
* Panic codes with category "OomMonitorPlugin"
*/
enum TGOomMonitorPluginPanic
    {
    /**
    * The plugin has been constructed by a thread other than the GOOM monitor.
    */
    EGOomMonitorPluginPanic_PluginConstructedOutsideGOomMonitorThread
    };

// Message type used to signal an GOOM plug-in to free some memory.
const TInt KAppGOomMonitor_FreeRam = 0x10282304;

// Message type used to signal an OOM plug-in that there is free memory again.
const TInt KAppGOomMonitor_MemoryGood = 0x10282305;

class CGOomMonitorPluginBase : public CBase
	{
public:
	/**
	* Constructor
	* Plugins should assume that the system memory is good when
	* they are created.
	*/
	IMPORT_C CGOomMonitorPluginBase();
	/**
	* Destructor
	*/
	IMPORT_C ~CGOomMonitorPluginBase();
	/**
	* 2nd stage constructor
	*/
	IMPORT_C void ConstructL();

public:
	/**
	* FreeRam is called when the system RAM level becomes
	* low. This plugin is requested to help free some RAM.
	*/
	//virtual void FreeRam(TInt aBytesToFree);
	virtual void FreeRam(TInt aBytesToFree, TInt aFlags) = 0;
	/**
	* MemoryGood is called when the system RAM level becomes
	* good after being low.The plugin may take this opportunity
	* to start using RAM again.
	* Nb It is assumed that the plugin will not immediately
	* cause a large increase in memory use, but that memory may be
	* used over time, otherwise the plugin may cause oscillation
	* between low and good memory states.
	*/
	//virtual void MemoryGood();
	virtual void MemoryGood(TInt aFlags) = 0;

public:
    /**
    * Get a handle to the file server.
    * @return a connected file server session.
    */
    IMPORT_C RFs& FsSession();
    /**
    * Get a handle to the window server.
    * @return a connected window server session.
    */
    IMPORT_C RWsSession& WsSession();

	IMPORT_C virtual void ExtensionInterface(TUid aInterfaceId, TAny*& aImplementaion);

private:
	CMemoryMonitor* iMemoryMonitor;
	};


class CGOomMonitorPlugin : public CGOomMonitorPluginBase
	{
public:	// new functions

	CGOomMonitorPlugin():CGOomMonitorPluginBase(){}    

public:	// From CGOomMonitorPluginBase
	/**
	* FreeRam is called when the system graphics memory level becomes
	* low. This plugin is requested to help free some RAM.
	*
	* this version of the function should not need to be implemented.
	* The definition of this function should change once the alf plugin is stable and finetuned
    * @param aBytesToFree The minimum number of bytes of free memory that the plugin should try to free.
	*/

    virtual void FreeRam(TInt aBytesToFree, TInt aFlags) = 0;
	
	/**
	* MemoryGood is called when the system RAM level becomes
	* good after being low.The plugin may take this opportunity
	* to start using RAM again.
	* Nb It is assumed that the plugin will not immediately
	* cause a large increase in memory use, but that memory may be
	* used over time, otherwise the plugin may cause oscillation
	* between low and good memory states.
	*/
	virtual void MemoryGood(TInt aFlags) = 0;
	
	};



/**
* CAppOomMonitorPlugin is a specialised OOM monitor plugin
* that sends messages to applications when the memory state
* changes.
* The target application is specified by the appUid parameter.
* the message will be sent to all running instances of this app.
* The messages are:
* KAppOomMonitor_FreeRam when memory should be released.
* KAppOomMonitor_MemoryGood when memory is good again.
*
* These messages can be picked up in the app UI like this:
* void CMyAppUi::HandleApplicationSpecificEventL(TInt aType, const TWsEvent& aEvent)
*     {
*     CAknAppUi::HandleApplicationSpecificEventL(aType, aEvent);
*     if (aType == KAppOomMonitor_FreeRam)
*         {
*         // release memory and disable memory use
*         }
*     else if (aType == KAppOomMonitor_MemoryGood)
*         {
*         // re-enable memory use
*         }
*     }
*
* The plugin can be instantiated in an ECom factory function.
* The application does not need to derive from this class.
* For example:
* TAny* CreateAppPlugin()
*     {
*     return CAppOomMonitorPlugin::NewL(KMyAppUid);
*     }
*/
NONSHARABLE_CLASS(CAppGOomMonitorPlugin) : public CGOomMonitorPluginBase
    {
public:
    IMPORT_C static CAppGOomMonitorPlugin* NewL(TUid aAppUid);

private:
	CAppGOomMonitorPlugin(TUid aAppUid);

	/*void FreeRam(TInt aFreeMemory);
	void MemoryGood();
	*/
	void FreeRam(TInt aBytesToFree, TInt aFlags);
	void MemoryGood(TInt aFlags);

    void SendMessageToApp(TInt aMessage);

private:
    TUid iAppUid;
    };


/**
* Sets the CMemoryMonitor object for this DLL.
* Only for use by CMemoryMonitor.
* @internal
*/
IMPORT_C void SetGMemoryMonitorTls(CMemoryMonitor* aMonitor);


#endif // OOMMONITORPLUGIN_H
