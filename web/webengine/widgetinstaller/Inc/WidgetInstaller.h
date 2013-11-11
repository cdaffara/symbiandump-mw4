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
* Description:  This file contains the header file of the WidgetInstaller class.
*
*
*
*/


#ifndef WIDGETINSTALLER_H
#define WIDGETINSTALLER_H

//  INCLUDES
#include <e32base.h>
#include <WidgetRegistryClient.h>
#ifdef _DEBUG
#include <flogger.h>
#endif

// FORWARD DECLARATIONS
class CWidgetConfigHandler;
class CWidgetRegistrationManager;

// CLASS DECLARATION
/**
* This class is provided for callback to notify icon conversion completion.
* @lib WidgetInstallerUI.lib
* @since 3.1
*/
class MConverterController
    {
public:
    /**
     * notify icon conversion completion
     */
    virtual void NotifyCompletionL( TInt aErr ) = 0;
    };

/**
* This class handles the install.
*
* @lib WidgetBackupRestore.exe
* @since 3.1
*/
NONSHARABLE_CLASS(CWidgetInstaller) : public CBase, MConverterController
    {
public:  // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CWidgetInstaller* NewL();

    /**
     * Destructor.
     */
    virtual ~CWidgetInstaller();

public: // New functions
    /**
     * Handle install request.
     * @since 3.1
     */
    IMPORT_C void InstallL( TDesC16& aRestoreDir );

    /**
     * Called when installation or update is finished,
     * do the final steps of registration, file clean up (remove
     * temporary files, display complete message.
     * Or handle error.
     */
    IMPORT_C void FinishInstallL( TInt aErr );

    /**
     * @since 3.2
     */
    IMPORT_C TInt RunError( TInt aError );

    /**
     * Rebuild registry properties (except UID) from installed widget dir.
     *
     * @param aFs: file session to use
     * @param aWidgetPath: path to main widget dir (icon path)
     * @param aLproj: language project name for localization (from registry)
     * @param aUid: the UID to assign to the widget
     * @since 5.0
     */
    IMPORT_C RPointerArray<CWidgetPropertyValue>* WidgetPropertiesFromInstalledWidgetL(
        RFs &aFs,
        const TDesC& aWidgetPath, /* should end with backslash */
        const TDesC& aLproj,
        TUid aUid );

    /**
     * If widget registry entry is assigned a new UID, then the
     * processed icon file (mbm file), if any, needs to be renamed
     * with the new UID.
     *
     * @param aFs: file session to use
     * @param aEntry: widget entry as stored in the registry
     * @since 5.0
     */
    IMPORT_C TInt RenameIconFile( RFs &aFs,
                                  RPointerArray<CWidgetPropertyValue>* aEntry );

    /**
     * @since 5.0
     */
    IMPORT_C void RegisterWidgetL( const TDesC& aMainHTML,
                                   const TDesC& aBundleDisplayName,
                                   const TDesC& aIconPath,
                                   const TDesC& aDriveName,
                                   const TUid& aUid );

    /**
     * @since 5.0
     */
    IMPORT_C void DeregisterWidgetL( const TUid& aUid );


private:
    /**
     * C++ default constructor.
     * @param aRequestStatus - Status of the request.
     */
    CWidgetInstaller();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Create widget entry by parsing the info.plist, insert the entry into the entry list
     * if the entry is not already existing.
     * @return the existed widget's index if already present
     * @since 3.1
     */
    TInt PreprocessWidgetBundleL( TDesC& aRestoreDir );

    /** 
	* Fix the widget properties need to restore from the backup 
	* @return void
    * @since 5.0 
	*/
    void FixWidgetPropsL(); 

    void ProcessRestoreDirL( TDesC& aRestoreDir );

    /**
     * Set drive to C: to install/update the widget
     * @return true if no error, Leaves if drive space is not sufficient
     * @since 3.2
     */
    TBool SetDriveAndCheckSpaceL();

    /**
     * update basepath of widget
     * @since 3.2
     */
    void UpdateWidgetBasePathL( TDriveUnit& aDrive );

    /**
     * When overwriting an installed widget, backup so can restore on error
     * @since 3.2
     */
    void BackupL();

    /**
     * When overwriting an installed widget, restore on error
     * @since 3.2
     */
    void RestoreL();

    TInt DriveInfo( TInt aDrive, TInt64& aDiskSpace );

    /**
     * Return the name of file/dir found given path of dir to search in and search critiria.
     * @Since 3.2

     * @param aDir: in param, bundle ID, e.g. com.yahoo.local.widget.traffic
     * @param aFoundName: out param,
     */
    TInt SearchByExtL( const TDesC& aDir, TUint aSortKey, const TDesC& aExt, TFileName& aFoundName );

    TInt GetDirSizeL( const TDesC& aDir );
    
    /**
     * Return the name of dir which contains info.plist file.
     * 

     * @param aDir: in param, bundle ID, e.g. com.yahoo.local.widget.traffic
     * @param aFoundName: out param,
     */
    TInt SearchWidgetRootL( const TDesC& aDir, TFileName& aFoundName );

    /**
    * ConvertIconL
    * Specific to WidgetPreInstaller
    * converts Icon.png to mbm
    * @Since 5.0
    */
    void ConvertIconL( TUid& aUid, const TDesC& aIconPath );


public: // From MConverterController
    /**
     * Used for notify icon convertion complete
     * @since 5.0
     */
    void NotifyCompletionL( TInt aErr ) ;

private: // Data

    TBool iOverwriting;

    CFileMan* iFileMgr; // owned
    RFs iRfs; // owned

    RWidgetRegistryClientSession iRegistry;
    TBool iServerConnected; // to Widget Registry server

    CWidgetConfigHandler* iWidgetConfigHandler; // owned
    CWidgetRegistrationManager* iAppManager; // owned

    TFileName iOriginalDir; // for overwrite backup/restore
    TFileName iBackupDir; // for overwrite backup/restore

    TFileName iBundleRootName; // e.g. yahooTraffic.wdgt

    // these values mainly come from parsing bundle metadata file
    // (info.plist) and are passed to WidgetRegistry for registration.
    RPointerArray<CWidgetPropertyValue> iPropertyValues;

#ifdef _DEBUG
    RFileLogger             iFileLogger;
    TBool                   iCanLog;
#endif
    };

#endif      // WIDGETINSTALLER_H

// End of File
