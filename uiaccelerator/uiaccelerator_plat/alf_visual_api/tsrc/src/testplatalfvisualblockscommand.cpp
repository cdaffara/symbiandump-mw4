/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  test functions for alfcommand.h
*
*/



// [INCLUDE FILES]
#include <alf/alfcommand.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TUid KTestUid = { 0x00000001 };


// FORWARD DECLARATION

// class TTestActionCmd for testing TAlfActionCommand
class TTestActionCmd : public TAlfActionCommand
    {
public:
    /**
     * Constructor
     */
    TTestActionCmd( TInt aId ) : TAlfActionCommand( aId ) {}
    
public:
    /**
     * From TAlfActionCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfActionCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };

// class TTestObjCmd for testing TAlfObjectCommand
class TTestObjCmd : public TAlfObjectCommand
    {
public:
    /**
     * Constructor
     */
    TTestObjCmd( TAlfCommandType aType, TAny* aDataOwnerObject, TAlfOp aOperation )
    : TAlfObjectCommand( aType, aDataOwnerObject, aOperation ) {}
    
public:
    /**
     * From TAlfObjectCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };

// class TTestGroupCmd for testing TAlfGroupCommand
class TTestGroupCmd : public TAlfGroupCommand
    {
public:
    /**
     * Constructor
     */
    TTestGroupCmd( CAlfControlGroup& aGroup, TAlfOp aOperation )
    : TAlfGroupCommand( aGroup, aOperation ) {}
    
public:
    /**
     * From TAlfGroupCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfGroupCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };

// class TTestCtrlCmd for testing TAlfControlCommand
class TTestCtrlCmd : public TAlfControlCommand
    {
public:
    /**
     * Constructor
     */
    TTestCtrlCmd( CAlfControl& aControl, TAlfOp aOperation )
    : TAlfControlCommand( aControl, aOperation ) {}
    
public:
    /**
     * From TAlfControlCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfControlCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };

// class TTestVisualCmd for testing TAlfVisualCommand
class TTestVisualCmd : public TAlfVisualCommand
    {
public:
    /**
     * Constructor
     */
    TTestVisualCmd( CAlfVisual& aVisual, TAlfOp aOperation )
    : TAlfVisualCommand( aVisual, aOperation ) {}
    
public:
    /**
     * From TAlfVisualCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfVisualCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };

// class TTestImageCmd for testing TAlfImageCommand
class TTestImageCmd : public TAlfImageCommand
    {
public:
    /**
     * Constructor
     */
    TTestImageCmd( CAlfImageVisual& aImageVisual, const TAlfImage& aImage )
    : TAlfImageCommand( aImageVisual, aImage ) {}
    
public:
    /**
     * From TAlfImageCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfImageCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };

// class TTestTextCmd for testing TAlfTextCommand
class TTestTextCmd : public TAlfTextCommand
    {
public:
    /**
     * Constructor
     */
    TTestTextCmd( CAlfTextVisual& aTextVisual, const TDesC& aText )
    : TAlfTextCommand( aTextVisual, aText ) {}
    
public:
    /**
     * From TAlfTextCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfTextCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };

// class TTestValueCmd for testing TAlfValueCommand
class TTestValueCmd : public TAlfValueCommand
    {
public:
    /**
     * Constructor
     */
    TTestValueCmd( TAny* aObject, TAlfValueCommandTarget aObjectType )
    : TAlfValueCommand( aObject, aObjectType ) {}
    
public:
    /**
     * From TAlfValueCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfValueCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };

// class TTestPointCmd for testing TAlfPointCommand
class TTestPointCmd : public TAlfPointCommand
    {
public:
    /**
     * Constructor
     */
    TTestPointCmd( TAny* aObject, TAlfPointCommandTarget aObjectType, TAlfOp aOperation )
    : TAlfPointCommand( aObject, aObjectType, aOperation ) {}
    
public:
    /**
     * From TAlfPointCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfPointCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };

// class TTestTransCmd for testing TAlfTransformationCommand
class TTestTransCmd : public TAlfTransformationCommand
    {
public:
    /**
     * Constructor
     */
    TTestTransCmd( TAny* aOwner, CAlfTransformation& aTransformation )
    : TAlfTransformationCommand( aOwner, aTransformation ) {}
    
public:
    /**
     * From TAlfTransformationCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfTransformationCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };

// class TTestMarkerCmd for testing TAlfMarkerCommand
class TTestMarkerCmd : public TAlfMarkerCommand
    {
public:
    /**
     * Constructor
     */
    TTestMarkerCmd( TInt aId ) : TAlfMarkerCommand( aId ) {}
    
public:
    /**
     * From TAlfMarkerCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfMarkerCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };

// class TTestCustomEventCmd for testing TAlfCustomEventCommand
class TTestCustomEventCmd : public TAlfCustomEventCommand
    {
public:
    /**
     * Constructor
     */
    TTestCustomEventCmd( TInt aEventParam, MAlfEventHandler* aRecipient = NULL )
    : TAlfCustomEventCommand( aEventParam, aRecipient ) {}
    
public:
    /**
     * From TAlfCustomEventCommand
     */
    void CommandExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        TAlfCustomEventCommand::CommandExtension( aExtensionUid, aExtensionParams );
        }
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCmdMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCmdMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCmdMethods, "TestCmdMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCmdMethods );
    // Print to log file
    iLog->Log( KTestCmdMethods );
    
    // TAlfActionCommand::TAlfActionCommand will call TAlfCommand::TAlfCommand
    // TAlfActionCommand::~TAlfActionCommand will call TAlfCommand::~TAlfCommand
    TAlfActionCommand cmd( 1 );
    cmd.Type();
    cmd.ObjectCommand();
    
    // TTestActionCmd::CommandExtension will call TAlfCommand::CommandExtension
    TTestActionCmd testCmd( 1 );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestActCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestActCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestActCmdMethodsL, "TestActCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestActCmdMethodsL );
    // Print to log file
    iLog->Log( KTestActCmdMethodsL );
    
    TAlfActionCommand actCmd( 1 );
    actCmd.ExecuteL( *iAlfEnv );
    actCmd.Size();
    actCmd.Id();
    
    TTestActionCmd testCmd( 1 );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestObjCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestObjCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestObjCmdMethodsL, "TestObjCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestObjCmdMethodsL );
    // Print to log file
    iLog->Log( KTestObjCmdMethodsL );
    
    TTestObjCmd objCmd( EAlfCommandTypeObject, NULL, EAlfOpNone );
    objCmd.ObjectCommand();
    objCmd.Object();
    objCmd.Operation();
    objCmd.ExecuteL( *iAlfEnv );
    objCmd.Size();
    
    TAny* extensionParams = NULL;
    objCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestGroupCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestGroupCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestGroupCmdMethodsL, "TestGroupCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestGroupCmdMethodsL );
    // Print to log file
    iLog->Log( KTestGroupCmdMethodsL );
    
    TAlfGroupCommand groupCmd( *iAlfCtrlGroup, EAlfOpHide, iAlfDisplay );
    groupCmd.ExecuteL( *iAlfEnv );
    groupCmd.Size();
    groupCmd.ControlGroup();
    groupCmd.Display();
    
    TTestGroupCmd testCmd( *iAlfCtrlGroup, EAlfOpHide );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCtrlCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCtrlCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCtrlCmdMethodsL, "TestCtrlCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCtrlCmdMethodsL );
    // Print to log file
    iLog->Log( KTestCtrlCmdMethodsL );
    
    TAlfControlCommand ctrlCmd( *iAlfCtl, EAlfOpNone );
    ctrlCmd.ExecuteL( *iAlfEnv );
    ctrlCmd.Size();
    ctrlCmd.Control();
    
    TTestCtrlCmd testCmd( *iAlfCtl, EAlfOpNone );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVisualCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVisualCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVisualCmdMethodsL, "TestVisualCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVisualCmdMethodsL );
    // Print to log file
    iLog->Log( KTestVisualCmdMethodsL );
    
    CAlfVisual* visual = iAlfCtl->AppendVisualL( EAlfVisualTypeVisual );
    TAlfVisualCommand visualCmd( *visual, EAlfOpNone );
    visualCmd.ExecuteL( *iAlfEnv );
    visualCmd.Size();
    visualCmd.Visual();
    visualCmd.FlagsToSet();
    visualCmd.FlagsToClear();
    
    TTestVisualCmd testCmd( *visual, EAlfOpNone );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestImageCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestImageCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestImageCmdMethodsL, "TestImageCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestImageCmdMethodsL );
    // Print to log file
    iLog->Log( KTestImageCmdMethodsL );
    
    TAlfImage image;
    CAlfImageVisual* imageVisual =
        STATIC_CAST( CAlfImageVisual*, iAlfCtl->AppendVisualL( EAlfVisualTypeImage ) );
    TAlfImageCommand imageCmd( *imageVisual, image );
    imageCmd.ExecuteL( *iAlfEnv );
    imageCmd.Size();
    imageCmd.ImageVisual();
    imageCmd.Image();
    
    TTestImageCmd testCmd( *imageVisual, image );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTextCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTextCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTextCmdMethodsL, "TestTextCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTextCmdMethodsL );
    // Print to log file
    iLog->Log( KTestTextCmdMethodsL );
    
    _LIT( KTestText, "Test" );
    CAlfTextVisual* txtVisual =
        STATIC_CAST( CAlfTextVisual*, iAlfCtl->AppendVisualL( EAlfVisualTypeText ) );
    TAlfTextCommand txtCmd( *txtVisual, KTestText );
    txtCmd.ExecuteL( *iAlfEnv );
    txtCmd.Size();
    txtCmd.TextVisual();
    txtCmd.Text();
    
    TTestTextCmd testCmd( *txtVisual, KTestText );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestValueCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestValueCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestValueCmdMethodsL, "TestValueCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestValueCmdMethodsL );
    // Print to log file
    iLog->Log( KTestValueCmdMethodsL );
    
    CAlfTextVisual* txtVisual =
        STATIC_CAST( CAlfTextVisual*, iAlfCtl->AppendVisualL( EAlfVisualTypeText ) );
    TAlfValueCommand valueCmd( txtVisual, EAlfValueCommandVisualSetOpacity, 1.0 );
    TAlfValueCommand valueCmd2( txtVisual, EAlfValueCommandVisualSetOpacity );
    valueCmd.ExecuteL( *iAlfEnv );
    valueCmd.Size();
    valueCmd.TimedValue();
    valueCmd.Target();
    valueCmd.TransitionTime();
    valueCmd.Speed();
    
    TTestValueCmd testCmd( txtVisual, EAlfValueCommandVisualSetOpacity );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestPointCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestPointCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestPointCmdMethodsL, "TestPointCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestPointCmdMethodsL );
    // Print to log file
    iLog->Log( KTestPointCmdMethodsL );
    
    CAlfTextVisual* txtVisual =
        STATIC_CAST( CAlfTextVisual*, iAlfCtl->AppendVisualL( EAlfVisualTypeText ) );
    TAlfPointCommand pointCmd( txtVisual, EAlfPointCommandVisualSetPos, EAlfOpSet );
    TAlfRealPoint realPoint;
    TAlfPointCommand pointCmd2( txtVisual, EAlfPointCommandVisualSetPos, realPoint );
    TAlfPointCommand pointCmd3( txtVisual, EAlfPointCommandVisualSetPos, EAlfOpSet, realPoint );
    pointCmd.ExecuteL( *iAlfEnv );
    pointCmd.Size();
    pointCmd.TimedPoint();
    pointCmd.Target();
    pointCmd.TransitionTime();
    pointCmd.Speed();
    
    TTestPointCmd testCmd( txtVisual, EAlfPointCommandVisualSetPos, EAlfOpSet );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTransCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTransCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTransCmdMethodsL, "TestTransCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTransCmdMethodsL );
    // Print to log file
    iLog->Log( KTestTransCmdMethodsL );
    
    iAlfCtrlGroup->EnableTransformationL();
    CAlfTransformation& alfTrans = iAlfCtrlGroup->Transformation();
    TAlfTransformationCommand transCmd( iAlfCtrlGroup, alfTrans );
    transCmd.ExecuteL( *iAlfEnv );
    transCmd.Size();
    transCmd.Transformation();
    transCmd.Step();
    TAlfTimedValue timedValue;
    transCmd.SetX( timedValue );
    transCmd.SetY( timedValue );
    transCmd.SetZ( timedValue );
    transCmd.SetAngle( timedValue );
    
    TTestTransCmd testCmd( iAlfCtrlGroup, alfTrans );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestMarkerCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestMarkerCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestMarkerCmdMethodsL, "TestMarkerCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestMarkerCmdMethodsL );
    // Print to log file
    iLog->Log( KTestMarkerCmdMethodsL );
    
    TAlfMarkerCommand markerCmd( 1 );
    markerCmd.ExecuteL( *iAlfEnv );
    markerCmd.Size();
    markerCmd.Id();
    
    TTestMarkerCmd testCmd( 1 );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCustomEventCmdMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCustomEventCmdMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCustomEventCmdMethodsL, "TestCustomEventCmdMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCustomEventCmdMethodsL );
    // Print to log file
    iLog->Log( KTestCustomEventCmdMethodsL );
    
    TAlfCustomEventCommand customEventCmd( 1 );
    customEventCmd.ExecuteL( *iAlfEnv );
    customEventCmd.Size();
    customEventCmd.Recipient();
    customEventCmd.Param();
    customEventCmd.EventData();
    
    TTestCustomEventCmd testCmd( 1 );
    TAny* extensionParams = NULL;
    testCmd.CommandExtension( KTestUid, &extensionParams );
    
    return KErrNone;
    }


//  [End of File]
