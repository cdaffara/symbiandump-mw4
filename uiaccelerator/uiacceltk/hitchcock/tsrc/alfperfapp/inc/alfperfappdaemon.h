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
* Description:  alfperfappdaemon application header.
*
*/


#ifndef C_ALFPERFAPPDAEMON_H
#define C_ALFPERFAPPDAEMON_H

#include <e32std.h>
#include <e32base.h>
#include <aknapp.h>
#include <akndoc.h>
#include <aknappui.h>
#include <aknserverapp.h>
#include "alfperfapp.hrh"

class CEikAppUi;
class CAlfDebugExtension;
class CAlfDebugFactory;
class CAlfDebugExtension;

/**
 * UID of this application.
 */
const TUid KAlfPerfAppDaemonUid = { KAlfPerfAppDaemonUidValue };

/**
 * CAlfPerfAppDaemonApplication application class. 
 */
class CAlfPerfAppDaemonApplication : public CAknApplication 
    {
private: // from CApaApplication

    // From base class CAknApplication 

    virtual CApaDocument* CreateDocumentL();
    virtual TUid AppDllUid() const;
    virtual void NewAppServerL( CApaAppServer*& aAppServer );
    };

/**
 *  CAlfPerfAppDaemonDocument document class.
 */
class CAlfPerfAppDaemonDocument : public CAknDocument
    {
public:
    CAlfPerfAppDaemonDocument( CEikApplication& aApp );

    // From base class CAknDocument
    virtual CEikAppUi* CreateAppUiL();
    virtual void UpdateTaskNameL(CApaWindowGroupName* aWgName);
    };

/**
 *  CAlfPerfAppDaemonAppUi appui class.
 */
class CAlfPerfAppDaemonAppUi : public CAknAppUi
    {
public:
    CAlfPerfAppDaemonAppUi();
    ~CAlfPerfAppDaemonAppUi();

    CAlfDebugExtension* Debug();

    // From base class CAknViewAppUi

    virtual void ConstructL();
    virtual void HandleCommandL( TInt aCommand );

private:
    RLibrary iLibrary;
    CAlfDebugFactory* iFactory;
    CAlfDebugExtension* iDebug;
    };

/**
 *  CAlfPerfAppDaemonServer server class.
 */
class CAlfPerfAppDaemonServer : public CAknAppServer
    {
public:
    CAlfPerfAppDaemonServer();

    // From base class CAknAppServer

    virtual CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;
    };

/**
 * CAlfPerfAppDaemonService 
 */
class CAlfPerfAppDaemonService : public CAknAppServiceBase
    {
public:
    CAlfPerfAppDaemonService();
    ~CAlfPerfAppDaemonService();

    // From base class CAknAppServiceBase

    virtual void ServiceL( const RMessage2& aMessage );

private:

    /**
     * Call back function for activity polling.
     * @param aAny pointer to instance of this class.
     * @return always KErrNone.
     */
    static TInt ActivityPollCallBack( TAny* aAny );

private:
    CPeriodic* iActivityPoll;
    };

#endif // C_ALFPERFAPPDAEMON_H

// End of File
