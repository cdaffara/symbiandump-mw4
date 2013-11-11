/*
* Copyright (c) 2006, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains the header file of the
*                CWidgetRegistryServerSession class.
*
*                This class implements the CWidgetRegistryServerSession class.
*
*/

#ifndef WIDGETREGISTRYSESSION_H
#define WIDGETREGISTRYSESSION_H

// INCLUDES
#include "WidgetRegistryData.h"

// FORWARD DECLARATIONS

class CWidgetRegistryServer;
class CWidgetData;
class CWidgetRegistry;
class RWriteStream;

class CWidgetRegistryServerSession : public CSession2
    {
public:
    /**
    * Returns singleton of factory.
    * @since 3.1
    * @param aServer WidgetRegistry server
    * @return CWidgetRegistryServerSession object
    */
    static CWidgetRegistryServerSession* NewL(
        CWidgetRegistryServer& aServer,
        const RMessage2& aMessage );

    /**
    * Destructor.
    * @since 3.1
    */
    ~CWidgetRegistryServerSession();

    /**
    * Called by the CServer framework.
    * @since 3.1
    * @return none
    */
    void CreateL();
    
    static void ResetAndDestroy(TAny *aPtr)
        {
            static_cast<RWidgetInfoArray *>(aPtr)->ResetAndDestroy();
        }
    

private:
    /**
    * C++ constructor.
    * @since 3.1
    */
    CWidgetRegistryServerSession();

    /**
    * 2-phased constructor.
    * @since 3.1
    * @param aServer WidgetRegistry server
    * @param aMessage message from client
    * @return none
    */
    void ConstructL( CWidgetRegistryServer& aServer,
        const RMessage2& aMessage );

private:
    /**
    * From CSession2, handles messages from client
    * @since 3.1
    * @param aMessage message from client
    * @return none
    */
    void ServiceL( const RMessage2& aMessage );

    /**
    * From CSession2, handles leaves from ServiceL()
    * @since 3.2
    * @param aMessage message from client
    * @param aError
    * @return none
    */
    void ServiceError(const RMessage2& aMessage,TInt aError);

private: // Op Codes
    /**
    * Register widget
    * @since 3.1
    * @param aMessage message from client
    * @return Error code
    */
    TInt OpRegisterWidgetL( const RMessage2& aMessage );

    /**
    * Whether widget exists or not for a given UId
    * @since 3.1
    * @param aMessage message from client
    * @return boolean value whether widget exists or not
    */
    TBool OpWidgetExists( const RMessage2& aMessage );

    /**
    * Checks if the UId is in the range allocated for widgets
    * @since 3.1
    * @param aMessage message from client
    * @return true if the given UId falls within the range allocated
    * for widgets otherwise false
    */
    TBool OpIsWidget( const RMessage2& aMessage );

    /**
    * Checks if widget is running
    * @since 3.1
    * @param aMessage message from client
    * @return true if widget is running otherwise false
    */
    TBool OpIsWidgetRunning( const RMessage2& aMessage );

    /**
    * Checks if widget sapi access is prompted, promptless or denied
    * @since 5.0
    * @param aMessage message from client
    * @return TInt as state
    */
    TInt OpWidgetSapiAccessState(const RMessage2& aMessage );

    /**
    * Checks if widget is in mini view
    * @since 
    * @param aMessage message from client
    * @return true if widget is miniview otherwise false
    */
    TBool OpIsWidgetInMiniView(const RMessage2& aMessage );

     /**
    * Checks if widget is started  in full view
    * @since 
    * @param aMessage message from client
    * @return true if widget is started in fullview otherwise false
    */
    TBool OpIsWidgetInFullView(const RMessage2& aMessage );

    /**
    * Checks if mini view will be prompted for platform access
    * @since 5.0
    * @param aMessage message from client
    * @return true if prompt is needed
    */
    TBool OpIsWidgetPromptNeeded(const RMessage2& aMessage );

    /**
    * Count of the widgets installed
    * @since 3.1
    * @param aMessage message from client
    * @return number of widgets installed
    */
    TInt OpWidgetCount( const RMessage2& aMessage );

    /**
    * Get array of installed widgets.
    * @since 3.1
    * @param aMessage message from client
    * @return count of installed widgets
    */
    TInt OpInstalledWidgetsPhase1L( const RMessage2& aMessage );

    /**
    * Get array of installed widgets.
    * @since 3.1
    * @param aMessage message from client
    * @return count of installed widgets
    */
    TInt OpInstalledWidgetsPhase2L( const RMessage2& aMessage );

    /**
    * Get array of running widgets.
    * @since 3.1
    * @param aMessage message from client
    * @return count of running widgets
    */
    TInt OpRunningWidgetsPhase1L( const RMessage2& aMessage );

    /**
    * Get array of running widgets.
    * @since 3.1
    * @param aMessage message from client
    * @return count of running widgets
    */
    TInt OpRunningWidgetsPhase2L( const RMessage2& aMessage );

    /**
    * Get widget bundle Id.
    * @since 3.1
    * @param aMessage message from client
    * @return length of widget bundle Id
    */
    TInt OpGetWidgetBundleId( const RMessage2& aMessage );

    /**
    * Get widget info.plist key string value.
    * @since 3.1
    * @param aMessage message from client
    * @return error code
    */
    TInt OpGetWidgetPropertyValueL( const RMessage2& aMessage );

    /**
    * Get widget bundle display name.
    * @since 3.1
    * @param aMessage message from client
    * @return length of widget bundle display name
    */
    TInt OpGetWidgetBundleName( const RMessage2& aMessage );

    /**
    * Get widget path.
    * @since 3.1
    * @param aMessage message from client
    * @return length of widget path
    */
    TInt OpGetWidgetPath( const RMessage2& aMessage );

    /**
    * Get widget UId of the widget with the given bundle Id.
    * @since 3.1
    * @param aMessage message from client
    * @return UId
    */
    TInt OpGetWidgetUid( const RMessage2& aMessage );

    /**
    * widget UId of the widget with the given url.
    * @since 3.1
    * @param aMessage message from client
    * @return UId
    */
    TInt OpGetWidgetUidForUrl( const RMessage2& aMessage );

    /**
    * Register the widget.
    * @since 3.1
    * @param aMessage message from client
    * @return Error code
    */
    TInt OpDeRegisterWidgetL( const RMessage2& aMessage );

    /**
    * next available UId
    * @since 3.1
    * @param aMessage message from client
    * @return next available UId
    */
    TInt OpGetAvailableUidL( const RMessage2& aMessage );

    /**
    * Set/Reset active status of the widget
    * @since 3.1
    * @param aMessage message from client
    * @return none
    */
    void OpSetActive( const RMessage2& aMessage );

    /**
    * Set/Reset active status of the widget
    * @since 3.1
    * @param aMessage message from client
    * @return none
    */
    void OpSetMiniView( const RMessage2& aMessage );

    /**
    * Set/Reset active status of the widget
    * @since 3.1
    * @param aMessage message from client
    * @return none
    */
    void OpSetFullView( const RMessage2& aMessage );

    /**
    * Set/Reset prompt required for miniview platform access
    * @since 5.0
    * @param aMessage message from client
    * @return none
    */
    void OpSetPromptNeeded( const RMessage2& aMessage );

    /**
    * Disconnect from client
    * @since 3.1
    * @param aMessage message from client
    * @return Error code
    */
    TInt OpDisconnect( const RMessage2& aMessage );

    /**
    * Get lproj name.
    * @since 3.1
    * @param aMessage message from client
    * @return length of lproj name
    */
    TInt OpGetLprojNameL( const RMessage2& aMessage );

    /**
    * Get Security PolicyId
    * @since 5.0
    * @param aMessage message from client
    * @return Security PolicyId
    */
    TInt OpSecurityPolicyId( const RMessage2& aMessage );

    /**
    * Get WidgetRegistry server object as reference
    * @since 3.1
    * @return WidgetRegistry server object
    */
    CWidgetRegistryServer& WidgetRegistryServer();

    /**
    * Write data to aStream
    * @since 3.1
    * @return none
    */
    void ExternalizeL(
        RWriteStream& aStream, RWidgetInfoArray& aWidgetInfoArr );

    /**
    * Creates heap descriptor which holds contents of RWidgetInfoArray
    * @since 3.1
    * @return heap descriptor
    */
    HBufC8* MarshalDataL( RWidgetInfoArray& aWidgetInfoArr );

    /**
    * delete each item in RWidgetInfoArray
    * @since 3.1
    * @return none
    */
    void ResetWidgetInfo( RWidgetInfoArray& aWidgetInfoArr );

private:
    RFs                     iRfs;// file session, owned
    static CWidgetRegistry* iWidgetRegistry;// widget registry object, owned
    };

#endif
