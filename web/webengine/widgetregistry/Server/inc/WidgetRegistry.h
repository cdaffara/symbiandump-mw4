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
* Description:  This file contains the header file of the CWidgetRegistry class.
*
*                This class implements the CWidgetRegistry class.
*
*/

#ifndef WIDGETREGISTRY_H
#define WIDGETREGISTRY_H

// INCLUDES
#include <e32hashtab.h>
#include <apgcli.h>
#include <Browser_platform_variant.hrh>
#include <apgnotif.h>
#include "WidgetRegistryData.h"
#include "WidgetRegistryConstants.h"
#include "WidgetPropertyValue.h"
#include "WidgetRegistryLog.h"

// CONSTANTS
#define KWidgetRegistryMaxPathName 64

// MACROS

// FORWARD DECLARATIONS
class RFs;
class CWidgetEntry;
class CWidgetInstaller;
class CWidgetRegistryXml;

#if defined( BRDO_WRT_SECURITY_MGR_FF )
class CRTSecManager;
#endif

typedef RPointerArray<CWidgetEntry> RWidgetArray;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CWidgetRegistry
*  @since 3.1
*/

class CWidgetRegistry : public CBase, public MApaAppListServObserver
    {
public:
    /**
     * Returns singleton of factory.
     */
    static CWidgetRegistry* NewL( RFs& aFs );

    /**
     * Destructor.
     */
    virtual ~CWidgetRegistry();

    /**
     * Returns true if the Uid falls within the range specified for widgets.
     */
    TBool IsWidget( const TUid& aUid ) const;

    /**
     * Returns true if the widget is installed.
     */
    TBool WidgetExists( const TDesC& aBundleId ) const;

    /**
     * Returns true if the widget is running.
     */
    TBool IsWidgetRunning( const TUid& aUid ) const;

    /**
     * Returns if widget sapi access is prompted, promptless or denied.
     */
    TInt WidgetSapiAccessState( const TUid& aUid ) const;

    /**
     * Returns true if the widget is in miniview
     */
    TBool IsWidgetInMiniView( const TUid& aUid ) const;

    /**
     * Returns true if the widget is started in fullview
     */
    TBool IsWidgetInFullView( const TUid& aUid ) const;

    /**
     * Returns true if blanket permission is granted
     */
    TBool IsBlanketPermGranted( const TUid& aUid ) const;

    /**
     * Returns count of widgets installed.
     */
    TInt WidgetCount() const { return iEntries.Count();}

    /**
     * Returns path of the widget with a particular UId.
     */
    void GetWidgetPath( const TUid& aUid, TDes& aPath );

    /**
     * Returns the lproj name
     */
    void GetLprojNameL( TDes& aLprojName );

    /**
     * Returns UId of the widget with a particular bundle identifier.
     */
    TUid GetWidgetUid( const TDesC& aBundleId ) const;

    /**
     * Returns uid of the widget with a particular html path.
     */
    TUid GetWidgetUidForUrl( const TDesC& aUrl ) const;

    /**
     * Get next available UId
     */
    TUid GetAvailableUidL( TInt aDriveLetter );

    /**
     * Returns bundle Id of the widget with a particular UId.
     */
    void GetWidgetBundleId( const TUid& aUid, TDes& aBundleId );

    /**
     * Returns bundle display name of the widget with a particular UId.
     */
    void GetWidgetBundleName( const TUid& aUid, TDes& aBundleName );

    /**
     * return serialized value of property aId for the widget aUid or leave
     */
    CBufFlat* CWidgetRegistry::GetWidgetPropertyValueL(
        const TUid& aUid,
        TWidgetPropertyId aId,
        TInt aMaxLength );

    /**
     * Returns widget info for all the installed widgets.
     */
    void InstalledWidgetsL( RWidgetInfoArray& );

    /**
     * Returns widget info for all the running widgets.
     */
    void RunningWidgetsL( RWidgetInfoArray& );

    /**
     * Registers the widget and creates a widget entry.
     */
    void RegisterWidgetL( RReadStream& aStream );

    /**
     * Deregisters the widget and removes the widget entry.
     */
    void DeRegisterWidgetL( const TUid& aUid );

    /**
     * Set/Reset active status of the widget with a particular UId
     */
    void SetActive( TUid aUid, TInt aStatus );

    /**
     * Set/Reset Widget status for launched in MiniView
     */
    void SetMiniView( TUid aUid, TInt aStatus );

    /**
     * Set/Reset Widget status for widget  in FullView
     */
    void SetFullView( TUid aUid, TInt aStatus );

    /**
    * Set/Reset Widget Blanket permission for platform services
    */
    void SetBlanketPermissionL( TUid aUid, TInt aStatus );

    /**
     * Load widget information from a file, detect consistency
     * errors and correct them.  Leave only if unrecoverable issue,
     * dirty flag is true only in non leave case and some change
     * needs to be written out
     */
    void InternalizeL( TBool aDoConsistency,
                       TBool aIgnoreParseError,
                       TBool& aDirtyFlag,
                       TInt& aParseError );

    /**
     * Store widget information to a file
     */
    void ExternalizeL();

    /**
     * Returns security policyId.
     */
     TInt SecurityPolicyId() { return FetchSecurityPolicyIdL(); }

		//from MApaAppListServObserver
		void HandleAppListEvent(TInt aEvent);

private:

    /**
     * Insert a widget entry
     */
    TInt InsertL( CWidgetEntry* aEntry );

    /**
     * Get widget entry with a particular UId
     */
    CWidgetEntry& GetWidgetEntryL( const TUid& aUid ) const;

    /**
     * Get widget entry with a particular UId
     */
    TInt GetWidgetEntry( const TUid& aUid, CWidgetEntry*& aEntry ) const;

    /**
     * Get widget entry with a particular bundle Id
     */
    TInt GetWidgetEntry(
        const TDesC& aBundleId, CWidgetEntry*& aEntry ) const;

    /**
     * Get widget entry at a particular position in the array
     */
    const CWidgetEntry* GetWidgetEntry( const TInt& aPos );

    /**
     * Remove widget entry with a particular bundle Id
     */
    void Remove( const TDesC& aBundleId );

    /**
     * Remove widget entry with a particular bundle UId
     */
    void Remove( const TUid& aUid );


    /**
     * Returns array of used UIds.
     */
    void UsedUidsL( RUidArray& aUsedUids );

    /**
     * Reset widget entry array
     */
    void ResetWidgetEntry();

    /**
     * Get policy Id from security manager
     */
     TInt FetchSecurityPolicyIdL();

protected:
    /**
     * 2-phase constructor
     */
    void ConstructL();

    /**
     * Constructor
     */
    CWidgetRegistry( RFs& aFs );

    /**
     * create the mapping table for language and lproj dir
     */
    void CreateLangDirListL();

    void ExternalizeBinaryL( const TDesC& aFilename,
                             const CArrayFixFlat<TInt>* aIndices );

    void ExternalizeXmlL( const TDesC& aFilename,
                          const CArrayFixFlat<TInt>* aIndices );

    void InternalizeBinaryL( const TDesC& aFileName,
                             const TDriveUnit& aDriveUnit,
                             TBool aDoConsistency,
                             RArray<TUid>& aAppArchList,
                             RArray<TInt>& aAppArchListFlags,
                             const CDir* aInstalledListForDrive,
                             RArray<TInt>& aInstalledListForDriveFlags,
                             TBool& aDirtyFlag );

    void InternalizeXmlL( const TDesC& aFileName,
                          const TDriveUnit& aDriveUnit,
                          TBool aDoConsistency,
                          RArray<TUid>& aAppArchList,
                          RArray<TInt>& aAppArchListFlags,
                          const CDir* aInstalledListForDrive,
                          RArray<TInt>& aInstalledListForDriveFlags,
                          TBool& aDirtyFlag );

    CWidgetEntry* EntryConsistency(
        CWidgetEntry* entry,
        const RArray<TUid>& aAppArchList,
        RArray<TInt>& aAppArchListFlags,
        const CDir* aInstalledListForDrive,
        RArray<TInt>& aInstalledListForDriveFlags,
        const TDriveUnit& aDriveUnit,
        TBool& aDirtyFlag );

    void InstallDirConsistency( const CDir* aInstalledListForDrive,
                                RArray<TInt>& aInstalledListForDriveFlags,
                                TBool& aDirtyFlag );

    void AppArchListConsistency( const RArray<TUid>& aAppArchList,
                                 RArray<TInt>& aAppArchListFlags );

    TBool IsEntryInstalled( CWidgetEntry* entry,
                            const CDir* aInstalledListForDrive,
                            RArray<TInt>& aInstalledListForDriveFlags );

    TInt IsEntryInAppList( CWidgetEntry* entry,
                           const RArray<TUid>& aAppArchList,
                           RArray<TInt>& aAppArchListFlags,
                           TBool& aInAppList,
                           TUid& uid );

    TInt AppArchWidgetUids( RArray< TUid >& aUids );

    TBool AppArchWidgets( RArray<TUid>& aAppArchList,
                          RArray<TInt>& aAppArchListFlags );

    TBool InstallDirWidgets( const TDriveUnit& aDriveUnit,
                             CDir*& aInstalledListForDrive,
                             RArray<TInt>& aInstalledListForDriveFlags );

    TInt InternalizeEntryNewUidIfNeeded( CWidgetEntry* aEntry,
                                         const TDriveUnit& aDriveUnit );

private:

    RWidgetArray                iEntries;
    RUidArray                   iUsedUids;
    RFs                         iFs; // not owned

    // added for validation purpose
    RApaLsSession               iAppArch;
    CWidgetInstaller*           iInstaller;
    TBuf<KWidgetRegistryMaxPathName> iWidgetInstallPath;
    TBuf<KWidgetRegistryMaxPathName> iRegistryBinaryFileName;
    TBuf<KWidgetRegistryMaxPathName> iRegistryXmlFileName;
    TBuf<KWidgetRegistryMaxPathName> iRegistryXmlTempFileName;
    TBuf<KWidgetRegistryMaxPathName> iLprojName;

    // map of language code to lproj dir
    RPtrHashMap<TInt,HBufC8>    iLangDirList;
    TInt                        iPolicyId;
    CWidgetRegistryXml*         iXmlProcessor;
    CApaAppListNotifier* iApaAppListNotifier;
public:
    LOG_MEMBER_VARS
    };

#endif  //WIDGETREGISTRY_H
