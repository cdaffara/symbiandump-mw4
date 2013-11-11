/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*
*/

#ifndef WIDGETREGISTRYCLIENT_H
#define WIDGETREGISTRYCLIENT_H

#include <WidgetRegistryData.h>
#include <WidgetPropertyValue.h>

// Forward declaration
class RWriteStream;

class RWidgetRegistryClientSession : public RSessionBase
    {
public: // API
    /**
    * Constructor
    */
    IMPORT_C RWidgetRegistryClientSession();

    /**
    * Connect to widgetregistry server
    */
    IMPORT_C TInt Connect();

    /**
    * Returns the version
    */
    IMPORT_C TVersion Version() const;

    /**
    * Disconnect from widgetregistry server
    */
    IMPORT_C TInt Disconnect();

    /**
    * Returns true if the Uid falls within the range specified for widgets.
    */
    IMPORT_C TBool IsWidget( const TUid& aUid );

    /**
    * Returns true if the widget is installed
    */
    IMPORT_C TBool WidgetExistsL( const TDesC& aBundleId );

    /**
    * Returns true if the widget is running.
    */
    IMPORT_C TBool IsWidgetRunning( const TUid& aUid );

    /**
    * Returns count of widgets installed.
    */
    IMPORT_C TInt WidgetCount( TInt& aCount );

    /**
    * Returns path of the widget with a particular UId
    */
    IMPORT_C void GetWidgetPath( const TUid& aUid, TDes& aWidgetPath );

    /**
    * Returns uid of the widget with a particular bundle identifier
    */
    IMPORT_C TInt GetWidgetUidL( const TDesC& aBundleId );

    /**
    * Returns UId of the widget with a particular Url
    */
    IMPORT_C TInt GetWidgetUidForUrl( const TDesC& aUrl );

    /**
    * Get next availble uid
    */
    IMPORT_C TUid GetAvailableUidL( TUint aDriveLetter );

    /**
    * Returns bundle Id of the widget with a particular UId
    */
    IMPORT_C void GetWidgetBundleId( const TUid& aUid, TDes& aWidgetBundleId );

    /**
    * Returns bundle name of the widget with a particular UId
    */
    IMPORT_C void GetWidgetBundleName( const TUid& aUid, TDes& aWidgetBundleName );

    /**
    * Gets property value for a widget by UId, leaves on error
    */
    IMPORT_C CWidgetPropertyValue* GetWidgetPropertyValueL(
        const TUid& aUid,
        TWidgetPropertyId aPropertyId );

    /**
    * Returns widget info for all the installed widgets.
    */
    IMPORT_C TInt InstalledWidgetsL( RWidgetInfoArray& aWidgetInfoArr );

    /**
    * Returns widget info for all the running widgets.
    */
    IMPORT_C TInt RunningWidgetsL( RWidgetInfoArray& aWidgetInfoArr );

    /**
    * Registers the widget
    */
    IMPORT_C void RegisterWidgetL(
        const RPointerArray<CWidgetPropertyValue>&  aPropertyValues );

    /**
    * Deregister the widget
    */
    IMPORT_C void DeRegisterWidgetL( const TUid& aUid );

    /**
    * Set/Reset active status of the widget
    */
    IMPORT_C void SetActive( const TUid& aUid, TInt aStatus );

    /**
    * Get the language project name which is the directory to
    * hold localized resources
    */
    IMPORT_C void GetLprojName( TDes& aLprojName );

    /**
    * Returns security policyId.
    */
    IMPORT_C TInt SecurityPolicyId();
	
    /**
    * Returns if widget sapi access is prompted, promptless or denied.
    */
    IMPORT_C TInt WidgetSapiAccessState( const TUid& aUid );

    /**
    * Returns true if the widget is in MiniView.
    */
    IMPORT_C TBool IsWidgetInMiniView( const TUid& aUid );

    /**
    * Returns true if the widget is in FullView.
    */
    IMPORT_C TBool IsWidgetInFullView( const TUid& aUid );

    /**
    * Returns true if the blanket permission is granted
    */
    IMPORT_C TBool IsBlanketPermGranted( const TUid& aUid );
	
    /**
    * Set/Reset Mini view launched status of widget
    */
    IMPORT_C void SetMiniViewL(const TUid& aUid, TInt aStatus );

    /**
    * Set/Reset Full view  status of widget
    */
    IMPORT_C void SetFullViewL(const TUid& aUid, TInt aStatus );

    /**
    * Set/Reset blaket permission for widgets
    */
    IMPORT_C void SetBlanketPermissionL(const TUid& aUid, TInt aStatus );

private:
    /**
    * Creates and returns heap descriptor which holds contents of
    * RWidgetDataArray or CWidgetInfo based on the datatype passed
    */
    CBufFlat* MarshalPropertyValuesL(
        const RPointerArray<CWidgetPropertyValue>& aPropertyValues ) const;

    /**
    * Read data from aStream
    */
    void DeserializeWidgetInfoL(
        RReadStream& aStream, RWidgetInfoArray& aWidgetInfoArr, TInt aCount );


private:
    TIpcArgs iMesgArgs;
    };

#endif
