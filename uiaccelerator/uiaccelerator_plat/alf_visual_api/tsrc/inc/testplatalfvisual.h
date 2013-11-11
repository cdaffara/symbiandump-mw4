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
* Description:  test functions in alf_visual_api
*
*/



#ifndef C_TESTPLATALFVISUAL_H
#define C_TESTPLATALFVISUAL_H

//  INCLUDES
#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>
#include <testclassassert.h>
#include <eikenv.h>
#include <coemain.h>

#include <alf/alfenv.h>
#include <alf/alfcontrol.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alfroster.h>
#include <alf/alfvisual.h>
#include <alf/alfcurvepath.h>
#include <alf/alfcurvepathlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfdropshadow.h>
#include <alf/alftextvisual.h>
#include <alf/alfdropshadowbrush.h>
#include <alf/alfevent.h>
#include <alf/alfscrollerevent.h>
#include <alf/alfflowlayout.h>
#include <alf/alfframebrush.h>
#include <alf/alfgencomponent.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfgridlayout.h>
#include <alf/alfimage.h>
#include <alf/alfimagebrush.h>
#include <alf/alfimageloaderutil.h>
#include <alf/alfimagevisual.h>
#include <alf/alflayout.h>
#include <alf/alflinevisual.h>
#include <alf/alfownership.h>
#include <alf/alfmeshvisual.h>
#include <alf/alfownership.h>
#include <alf/alfmeshvisual.h>
#include <alf/alfmaterial.h>
#include <alf/alfmappingfunctions.h>
#include <alf/alfm3gmesh.h>
#include <alf/alftexture.h>
#include <alf/alfbrush.h>




// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KtestplatalfvisualLogPath, "\\logs\\testframework\\testplatalfvisual\\" ); 
// Log file
_LIT( KtestplatalfvisualLogFile, "testplatalfvisual.txt" ); 
_LIT( KtestplatalfvisualLogFileWithTitle, "testplatalfvisual_[%S].txt" );

/**
*  CTestPlatAlfVisual test class for STIF Test Framework TestScripter.
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTestPlatAlfVisual ) : public CScriptBase
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CTestPlatAlfVisual* NewL( CTestModuleIf& aTestModuleIf );

    /**
    * Destructor.
    */
    virtual ~CTestPlatAlfVisual();

public: // Functions from base classes

    /**
    * From CScriptBase Runs a script line.
    * @since S60 5.0
    * @param aItem Script line containing method name and parameters
    * @return Symbian OS error code
    */
    virtual TInt RunMethodL( CStifItemParser& aItem );

private:

    /**
    * C++ default constructor.
    */
    CTestPlatAlfVisual( CTestModuleIf& aTestModuleIf );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
     * Method used to log version of test class
     */
    void SendTestClassVersion();
    
    /**
     * Turn off ScreenSaver
     * @since S60 5.0
     * @return Symbian OS error code.
     */
    void TurnOffScreenSaver();

    /**
     * Restore ScreenSaver
     * @since S60 5.0
     * @return Symbian OS error code.
     */
    void RestoreScreenSaver();

private: // [TestMethods]
//FOR TEST alfanchorlayout.h
    /**
    * TestAnchorLayoutAddNewL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutAddNewL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutConstructorL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutConstructorL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutConstructL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutConstructL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutResetL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutResetL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutSetAnchorL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutSetAnchorL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutSetRelativeAnchorRectL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutSetRelativeAnchorRectL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutRemoveAnchorL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutRemoveAnchorL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutAttachEdgeL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutAttachEdgeL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutAttachCornerL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutAttachCornerL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutAttachBoxL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutAttachBoxL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutDetachL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutDetachL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutDetachTypeL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutDetachTypeL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutRemoveAndDestroyAllDL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutRemoveAndDestroyAllDL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutUpdateChildrenLayoutL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutUpdateChildrenLayoutL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutFindTagL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutFindTagL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutBaseUnitL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutBaseUnitL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutDoRemoveAndDestroyAllDL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutDoRemoveAndDestroyAllDL( CStifItemParser& aItem );
    /**
    * TestAnchorLayoutPropertyOwnerExtensionL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAnchorLayoutPropertyOwnerExtensionL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfvisual.h
    /**
    * TestCAlfVCAlfVisualL tests Constructor of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVCAlfVisualL( CStifItemParser& aItem );
    /**
    * TestCAlfVConstructL tests ConstructL of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVConstructL( CStifItemParser& aItem );
    /**
    * TestCAlfVRemoveAndDestroyAllDL tests RemoveAndDestroyAllD of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVRemoveAndDestroyAllDL( CStifItemParser& aItem );
    /**
    * TestCAlfVIdentifierL tests Identifier of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVIdentifierL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetFlagL tests SetFlag of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetFlagL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetFlagsL tests SetFlags of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetFlagsL( CStifItemParser& aItem );
    /**
    * TestCAlfVClearFlagL tests ClearFlag of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVClearFlagL( CStifItemParser& aItem );
    /**
    * TestCAlfVClearFlagsL tests ClearFlags of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVClearFlagsL( CStifItemParser& aItem );
    /**
    * TestCAlfVFlagsL tests Flags of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVFlagsL( CStifItemParser& aItem );
    /**
    * TestCAlfVLayoutL tests Layout of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVLayoutL( CStifItemParser& aItem );
    /**
    * TestCAlfVPosL tests Pos of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVPosL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetPosL tests SetPos of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetPosL( CStifItemParser& aItem );
    /**
    * TestCAlfVSizeL tests Size of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSizeL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetSizeL tests SetSize of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetSizeL( CStifItemParser& aItem );
    /**
    * TestCAlfVOpacityL tests Opacity of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVOpacityL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetOpacityL tests SetOpacity of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetOpacityL( CStifItemParser& aItem );
    /**
    * TestCAlfVEnvL tests Env of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVEnvL( CStifItemParser& aItem );
    /**
    * TestCAlfVDisplayL tests Display of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVDisplayL( CStifItemParser& aItem );
    /**
    * TestCAlfVTagL tests Tag of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVTagL( CStifItemParser& aItem );
    /**
    * TestCAlfVUpdateChildrenLayoutL tests UpdateChildrenLayout of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVUpdateChildrenLayoutL( CStifItemParser& aItem );
    /**
    * TestCAlfVDisplayRectTargetL tests DisplayRectTarget of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVDisplayRectTargetL( CStifItemParser& aItem );
    /**
    * TestCAlfVLocalToDisplayL tests LocalToDisplay of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVLocalToDisplayL( CStifItemParser& aItem );
    /**
    * TestCAlfVLocalToDisplayWithRealPointL tests LocalToDisplay of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVLocalToDisplayWithRealPointL( CStifItemParser& aItem );
    /**
    * TestCAlfVDisplayToLocalL tests DisplayToLocal of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVDisplayToLocalL( CStifItemParser& aItem );
    /**
    * TestCAlfVDisplayToLocalWithRealPointL tests DisplayToLocal of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVDisplayToLocalWithRealPointL( CStifItemParser& aItem );
    /**
    * TestCAlfVEnableBrushesL tests EnableBrushesL of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVEnableBrushesL( CStifItemParser& aItem );
    /**
    * TestCAlfVBrushesL tests Brushes of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVBrushesL( CStifItemParser& aItem );
    /**
    * TestCAlfVMoveToFrontL tests MoveToFront of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVMoveToFrontL( CStifItemParser& aItem );
    /**
    * TestCAlfVEnableTransformationL tests EnableTransformationL of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVEnableTransformationL( CStifItemParser& aItem );
    /**
    * TestCAlfVTransformationL tests Transformation of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVTransformationL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetPaddingL tests SetPadding of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetPaddingL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetPaddingWithPointL tests SetPadding of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetPaddingWithPointL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetPaddingWithMetricL tests SetPadding of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetPaddingWithMetricL( CStifItemParser& aItem );
    /**
    * TestCAlfVPaddingInPixelsL tests PaddingInPixels of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVPaddingInPixelsL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetTagL tests SetTagL of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetTagL( CStifItemParser& aItem );
    /**
    * TestCAlfVIsTaggedL tests IsTagged of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVIsTaggedL( CStifItemParser& aItem );
    /**
    * TestCAlfVFindTagL tests FindTag of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVFindTagL( CStifItemParser& aItem );
    /**
    * TestCAlfVConstructWithMoreParamsL tests ConstructL of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVConstructWithMoreParamsL( CStifItemParser& aItem );
    /**
    * TestCAlfVCommsL tests Comms of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVCommsL( CStifItemParser& aItem );
    /**
    * TestCAlfVDepthOffsetL tests DepthOffset of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVDepthOffsetL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetDepthOffsetL tests SetDepthOffset of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetDepthOffsetL( CStifItemParser& aItem );
    /**
    * TestCAlfVOwnerL tests Owner of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVOwnerL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetOwnerL tests SetOwner of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetOwnerL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetOriginL tests SetOrigin of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetOriginL( CStifItemParser& aItem );
    /**
    * TestCAlfVHorizontalOriginL tests HorizontalOrigin of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVHorizontalOriginL( CStifItemParser& aItem );
    /**
    * TestCAlfVVerticalOriginL tests VerticalOrigin of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVVerticalOriginL( CStifItemParser& aItem );
    /**
    * TestCAlfVAlignByOriginL tests AlignByOrigin of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVAlignByOriginL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetMinSizeL tests SetMinSize of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetMinSizeL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetMinSizeWithRealSizeL tests SetMinSize of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetMinSizeWithRealSizeL( CStifItemParser& aItem );
    /**
    * TestCAlfVMinSizeL tests MinSize of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVMinSizeL( CStifItemParser& aItem );
    /**
    * TestCAlfVMinSizeInBaseUnitsL tests MinSizeInBaseUnits of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVMinSizeInBaseUnitsL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetMaxSizeL tests SetMaxSize of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetMaxSizeL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetMaxSizeWithRealSizeL tests SetMaxSize of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetMaxSizeWithRealSizeL( CStifItemParser& aItem );
    /**
    * TestCAlfVMaxSizeL tests MaxSize of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVMaxSizeL( CStifItemParser& aItem );
    /**
    * TestCAlfVMaxSizeInBaseUnitsL tests MaxSizeInBaseUnits of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVMaxSizeInBaseUnitsL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetRectL tests SetRect of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetRectL( CStifItemParser& aItem );
    /**
    * TestCAlfVMoveL tests Move of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVMoveL( CStifItemParser& aItem );
    /**
    * TestCAlfVMoveWithRealPointL tests Move of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVMoveWithRealPointL( CStifItemParser& aItem );
    /**
    * TestCAlfVMimicL tests Mimic of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVMimicL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetCenteredPosAndSizeL tests SetCenteredPosAndSize of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetCenteredPosAndSizeL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetCenteredPosAndSizeWithRealL tests SetCenteredPosAndSize of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetCenteredPosAndSizeWithRealL( CStifItemParser& aItem );
    /**
    * TestCAlfVDisplayRectL tests DisplayRect of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVDisplayRectL( CStifItemParser& aItem );
    /**
    * TestCAlfVEffectiveOpacityL tests EffectiveOpacity of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVEffectiveOpacityL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetClippingL tests SetClipping of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetClippingL( CStifItemParser& aItem );
    /**
    * TestCAlfVClippingL tests Clipping of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVClippingL( CStifItemParser& aItem );
    /**
    * TestCAlfVPaddingL tests Padding of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVPaddingL( CStifItemParser& aItem );
    /**
    * TestCAlfVPaddingAsMetricL tests PaddingAsMetric of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVPaddingAsMetricL( CStifItemParser& aItem );
    /**
    * TestCAlfVHorizontalPaddingL tests HorizontalPadding of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVHorizontalPaddingL( CStifItemParser& aItem );
    /**
    * TestCAlfVVerticalPaddingL tests VerticalPadding of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVVerticalPaddingL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetPosWithTimedPointL tests SetPos of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetPosWithTimedPointL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetSizeWithTimedSizeL tests SetSize of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetSizeWithTimedPointL( CStifItemParser& aItem );
    /**
    * TestCAlfVCopyValuesL tests CopyValues of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVCopyValuesL( CStifItemParser& aItem );
    /**
    * TestCAlfVPauseL tests Pause of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVPauseL( CStifItemParser& aItem );
    /**
    * TestCAlfVResumeL tests Resume of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVResumeL( CStifItemParser& aItem );
    /**
    * TestCAlfVDoRemoveAndDestroyAllDL tests DoRemoveAndDestroyAllD of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVDoRemoveAndDestroyAllDL( CStifItemParser& aItem );
    /**
    * TestCAlfVSetTactileFeedbackL tests SetTactileFeedbackL of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVSetTactileFeedbackL( CStifItemParser& aItem );
    /**
    * TestCAlfVHasTactileFeedbackL tests HasTactileFeedback of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVHasTactileFeedbackL( CStifItemParser& aItem );
    /**
    * TestCAlfVRemoveTactileFeedbackL tests RemoveTactileFeedback of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVRemoveTactileFeedbackL( CStifItemParser& aItem );
    /**
    * TestCAlfVEnableDropShadowL tests EnableDropShadowL of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVEnableDropShadowL( CStifItemParser& aItem );
    /**
    * TestCAlfVDropShadowHandlerL tests DropShadowHandler of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVDropShadowHandlerL( CStifItemParser& aItem );
    /**
    * TestCAlfVPropertyOwnerExtensionL tests PropertyOwnerExtension of CAlfVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCAlfVPropertyOwnerExtensionL( CStifItemParser& aItem );

    
private: //FOR TEST alfvisualfactory.h
    /**
    * TestAlfVFNewLayoutL tests NewLayoutL of AlfVisualFactory.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfVFNewLayoutL( CStifItemParser& aItem );
    /**
    * TestAlfVFNewLayoutLCL tests NewLayoutLC of AlfVisualFactory.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfVFNewLayoutLCL( CStifItemParser& aItem );
    /**
    * TestAlfVFNewVisualL tests NewVisualL of AlfVisualFactory.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfVFNewVisualL( CStifItemParser& aItem );
    /**
    * TestAlfVFNewVisualLCL tests NewVisualLC of AlfVisualFactory.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfVFNewVisualLCL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfcontrol.h
    /**
    * TestAlfCtrlConstructorL tests constructor of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlConstructorL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlConstructorL tests ConstructL of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlConstructL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlIdL tests SetId and Id of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlIdL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlGetCtrlMember tests Env, ControlGroup and Display of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlGetCtrlMember( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlProcessVisualL tests Append, Remove, AppendVisualL, Visual,
    * FindTag, VisualCount, VisualDestroyed, VisualLayoutUpdated and 
    * VisualPrepareDrawFailed of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlProcessVisualL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlProcessConnectL tests AddConnectionL, RemoveConnection, 
    * FindConnection, ConnectionCount,Connection, ConnectionByOrdinal,
    * ConnectionRole and ConnectionOrdinal of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlProcessConnectL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlProcessRole tests SetRole and Role of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlProcessRole( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlProcessHostL tests Host, HostId, SetHostId and SetHost
    * of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlProcessHostL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlHostDisplayL tests HostToDisplay and DisplayToHost
    * of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlHostDisplayL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlDisplayPos tests DisplayArea and DisplayCenter
    * of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlDisplayPos( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlEventL tests AcceptInput, OfferEventL, CancelAllCommands and
    * HitTest of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlEventL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlProcessFocus tests AcquireFocus, RelinquishFocus, Focus,
    * FocusedConnection, IsFocusing and FocusChanged of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlProcessFocus( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlLayoutL tests AppendLayoutL and ContainerLayout
    * of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlLayoutL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlIdentifier tests Identifier of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlIdentifier( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlBounds tests Bounds of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlBounds( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlShowL tests ShowL of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlShowL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlAddAndRemoveVisualL tests VisualAddedL and VisualRemoved
    * of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlAddAndRemoveVisualL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlNotifyControlVisibilityL tests NotifyControlVisibility
    * of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlNotifyControlVisibilityL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlAddAndRemoveConnectionL tests ConnectionAddedL and 
    * ConnectionRemoved of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlAddAndRemoveConnectionL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlHostChangingL tests HostChangingL of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlHostChangingL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlPropertyOwnerExtensionL tests PropertyOwnerExtension
    * of CAlfControl.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlPropertyOwnerExtensionL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfcontrolgroup.h
    /**
    * TestAlfCtrlGroupResourceId tests ResourceId
    * of CAlfControlGroup.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlGroupResourceId( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlGroupProcessCtrlL tests AppendL, Remove, FindControl, Count
    * and Control of CAlfControlGroup.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlGroupProcessCtrlL( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlGroupProcessInput tests AcceptInput and SetAcceptInput
    * of CAlfControlGroup.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlGroupProcessInput( CStifItemParser& aItem );
    
    /**
    * TestAlfCtrlGroupProcessTransformL tests EnableTransformationL, 
    * IsTransformed and Transformation of CAlfControlGroup.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCtrlGroupProcessTransformL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfcurvepath.h
    /**
    * TestAlfCurvePathNewL tests NewL and NewLC of CAlfCurvePath.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfCurvePathLoopL tests EnableLoop and Loop of CAlfCurvePath.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathLoopL( CStifItemParser& aItem );
    
    /**
    * TestAlfCurvePathOffsetL tests SetOffset and Offset of CAlfCurvePath.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathOffsetL( CStifItemParser& aItem );
    
    /**
    * TestAlfCurvePathProcessPointsL tests MapValue, SetOrigin, two AppendLineL,
    * two AppendArcL, Length and Evaluate of CAlfCurvePath.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathProcessPointsL( CStifItemParser& aItem );
    
    /**
    * TestAlfCurvePathResetL tests Reset of CAlfCurvePath.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathResetL( CStifItemParser& aItem );
    
    /**
    * TestAlfCurvePathMappingFunctionIdentifierL tests MappingFunctionIdentifier
    * of CAlfCurvePath.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathMappingFunctionIdentifierL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfcurvepathlayout.h
    /**
    * TestAlfCurvePathLayoutAddNewL tests AddNewL
    * of CAlfCurvePathLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathLayoutAddNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfCurvePathLayoutFullConstructL tests constructor and ConstructL
    * of CAlfCurvePathLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathLayoutFullConstructL( CStifItemParser& aItem );
    
    /**
    * TestAlfCurvePathLayoutForLayoutL tests CurvePath and BaseUnit
    * of CAlfCurvePathLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathLayoutForLayoutL( CStifItemParser& aItem );
    
    /**
    * TestAlfCurvePathLayoutForVisualL tests FindTag, UpdateChildrenLayout and
    * RemoveAndDestroyAllD of CAlfCurvePathLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathLayoutForVisualL( CStifItemParser& aItem );
    
    /**
    * TestAlfCurvePathLayoutForMapL tests MappingFunctionIdentifier and
    * MapValue of CAlfCurvePathLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathLayoutForMapL( CStifItemParser& aItem );
    
    /**
    * TestAlfCurvePathLayoutProtectedFuncL tests DoRemoveAndDestroyAllD and
    * PropertyOwnerExtension of CAlfCurvePathLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCurvePathLayoutProtectedFuncL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfdecklayout.h
    /**
    * TestAlfDeckLayoutAddNewL tests AddNewL of CAlfDeckLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDeckLayoutAddNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfDeckLayoutFullConstructL tests constructor and ConstructL
    * of CAlfDeckLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDeckLayoutFullConstructL( CStifItemParser& aItem );
    
    /**
    * TestAlfDeckLayoutForLayoutL tests BaseUnit
    * of CAlfDeckLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDeckLayoutForLayoutL( CStifItemParser& aItem );
    
    /**
    * TestAlfDeckLayoutForVisualL tests FindTag, UpdateChildrenLayout and
    * RemoveAndDestroyAllD of CAlfDeckLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDeckLayoutForVisualL( CStifItemParser& aItem );
    
    /**
    * TestAlfDeckLayoutProtectedFuncL tests DoRemoveAndDestroyAllD and
    * PropertyOwnerExtension of CAlfDeckLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDeckLayoutProtectedFuncL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfdisplay.h
    /**
    * TestAlfDisplayForBackgroundL tests SetClearBackgroundL and
    * SetBackgroundItemsL of CAlfDisplay.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDisplayForBackgroundL( CStifItemParser& aItem );
    
    /**
    * TestAlfDisplayForRoster tests two Roster of CAlfDisplay.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDisplayForRoster( CStifItemParser& aItem );
    
    /**
    * TestAlfDisplayForVisibleArea tests SetVisibleArea, VisibleArea and
    * ForceSetVisibleArea of CAlfDisplay.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDisplayForVisibleArea( CStifItemParser& aItem );
    
    /**
    * TestAlfDisplayForSetAttributesL tests SetFocusedEditor, SetQuality, 
    * SetUseDepth, SetUsageL and SetDirty of CAlfDisplay.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDisplayForSetAttributesL( CStifItemParser& aItem );
    
    /**
    * TestAlfDisplayGetAttributes tests Env, Quality and ObjectProvider
    * of CAlfDisplay.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDisplayGetAttributes( CStifItemParser& aItem );
    
    /**
    * TestAlfDisplayFading tests SuppressAutomaticFading of CAlfDisplay.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDisplayFading( CStifItemParser& aItem );
    
    /**
    * TestAlfDisplayHandlePointerEventL tests HandlePointerEventL of CAlfDisplay.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDisplayHandlePointerEventL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfdisplaybackgrounditem.h
    /**
    * TestAlfDisplayBgItemConstructor tests SetClearBackgroundL and
    * SetBackgroundItemsL of CAlfDisplay.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDisplayBgItemConstructor( CStifItemParser& aItem );
    
    /**
    * TestAlfDisplayBgItemSetAttributes tests SetRect, SetColor, SetColor and
    * SetSkinBackground of CAlfDisplayBackGroundItem.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDisplayBgItemSetAttributes( CStifItemParser& aItem );
    
    
private: //FOR TEST alfdropshadow.h
    /**
    * TestAlfDropShadowSetAttributesL tests two SetOffset, two SetColor, 
    * SetOpacity, SetRadius and SetScale of CAlfDropShadow.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDropShadowSetAttributesL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfdropshadowbrush.h
    /**
    * TestAlfDropShadowBrushNewFuncsL tests two NewL and two NewLC
    * of CAlfDropShadowBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDropShadowBrushNewFuncsL( CStifItemParser& aItem );
    
    /**
    * TestAlfDropShadowBrushSoftnessL tests two SetSoftness and Softness
    * of CAlfDropShadowBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDropShadowBrushSoftnessL( CStifItemParser& aItem );
    
    /**
    * TestAlfDropShadowBrushDepthL tests two SetDepth and Depth
    * of CAlfDropShadowBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDropShadowBrushDepthL( CStifItemParser& aItem );
    
    /**
    * TestAlfDropShadowBrushSetColorL tests  SetColor of CAlfDropShadowBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfDropShadowBrushSetColorL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfenv.h
    /**
    * TestAlfEnvNewFuncsL tests  NewL and NewLC of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvNewFuncsL( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvClient tests  Client and TokenClient of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvClient( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvNewDisplayL tests  two NewDisplayL, PrimaryDisplay and 
    * DisplayCount of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvNewDisplayL( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvCtrlGroupL tests  NewControlGroupL, DeleteControlGroup, 
    * ControlGroup and FindControlGroup of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvCtrlGroupL( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvProcessFresh tests  SetRefreshMode, RefreshMode, SetMaxFrameRate
    * ContinueRefresh, PauseRefresh and RefreshCallBack of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvProcessFresh( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvProcessTextureManagerL tests  CreateSharedTextureManagerL, 
    * SharedTextureManager, TextureManager, TextStyleManager and
    * DeleteSharedTextureManager of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvProcessTextureManagerL( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvObserverL tests  AddActionObserverL and 
    * RemoveActionObserver of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvObserverL( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvProcessCmd tests  Send, two CancelCommands, 
    * two CancelCustomCommands, three MilliSecondsUntilCommand  and two 
    * MilliSecondsUntilCustomCommand of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvProcessCmd( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvKeyAndPointerEventL tests  HandleKeyEventL and HandlePointerEventL 
    * of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvKeyAndPointerEventL( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvNotifyL tests  NotifySkinChangedL and NotifyLayoutChangedL 
    * of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvNotifyL( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvProcessWidgetL tests  WidgetFactory, AppendWidgetL, 
    * DestroyWidget and FindWidget of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvProcessWidgetL( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvGetAttributes tests  LayoutMetricsUtility, Renderer, 
    * LastReceivedPtrEvent, Version, BatchBufferHandler, Extension and
    * Display of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvGetAttributes( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvFindDisplayIndex tests  FindDisplayIndex of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvFindDisplayIndex( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvFindControl tests  TestAlfEnvFindControl of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvFindControl( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvAddExtension tests  AddExtension of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvAddExtension( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvSetAndReSetL tests  SetIdleThreshold, Release, RestoreL and 
    * SetFullScreenDrawing of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvSetAndReSetL( CStifItemParser& aItem );
    
    /**
    * TestAlfEnvStatic tests  Static of CAlfEnv.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEnvStatic( CStifItemParser& aItem );
    
    
private: //FOR TEST alfevent.h
    /**
    * TestAlfEventConstructor tests five TAlfEvent constructor of TAlfEvent.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEventConstructor( CStifItemParser& aItem );
    
    /**
    * TestAlfEventParameters tests PointerDown, PointerUp, PointerLongTap, 
    * IsPointerEvent, Type, IsCustomEvent, PointerEvent, IsScrollerEvent,
    * ScrollerEvent, Code, KeyEvent and IsKeyEvent of TAlfEvent.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEventParameters( CStifItemParser& aItem );
    
    /**
    * TestAlfEventCustomData tests CustomParameter and CustomEventData
    * of TAlfEvent.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEventCustomData( CStifItemParser& aItem );
    
    /**
    * TestAlfEventDisplayAndVisual tests Display, SetVisual and Visual
    * of TAlfEvent.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfEventDisplayAndVisual( CStifItemParser& aItem );
    
    
private: //FOR TEST alfflowlayout.h
    /**
    * TestAlfFlowLayoutAddNewL tests AddNewL of CAlfFlowLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfFlowLayoutAddNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfFlowLayoutFullConstructL tests constructor and ConstructL
    * of CAlfFlowLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfFlowLayoutFullConstructL( CStifItemParser& aItem );
    
    /**
    * TestAlfFlowLayoutForLayoutL tests BaseUnit
    * of CAlfFlowLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfFlowLayoutForLayoutL( CStifItemParser& aItem );
    
    /**
    * TestAlfFlowLayoutForVisualL tests FindTag, UpdateChildrenLayout and
    * RemoveAndDestroyAllD of CAlfFlowLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfFlowLayoutForVisualL( CStifItemParser& aItem );
    
    /**
    * TestAlfFlowLayoutProtectedFuncL tests DoRemoveAndDestroyAllD and
    * PropertyOwnerExtension of CAlfFlowLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfFlowLayoutProtectedFuncL( CStifItemParser& aItem );
    
    /**
    * TestAlfFlowLayoutSetParametersL tests SetFlowDirection, SetCentering,
    * SetMode and Mode of CAlfFlowLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfFlowLayoutSetParametersL( CStifItemParser& aItem );
    
private: //FOR TEST alfframebrush.h
    /**
    * TestAlfFrameBrushNewL tests three NewL and three NewLC of CAlfFrameBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfFrameBrushNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfFrameBrushSetParaL tests SetFrameRectsL, SetEdgeOffset and two
    * SetImage of CAlfFrameBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfFrameBrushSetParaL( CStifItemParser& aItem );
    
private: //FOR TEST alfgencomponent.h
    /**
    * TestAlfGenComponentNewL tests NewL of CAlfGenComponent.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGenComponentNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfGenComponentProcessL tests DoCmdNoReply, DoSynchronousCmd, 
    * DoAsynchronousCmdL, CancelAsynchCmd and Identifier of CAlfGenComponent.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGenComponentProcessL( CStifItemParser& aItem );
    
private: //FOR TEST alfgradientbrush.h
    /**
    * TestAlfGradientBrushNewL tests NewL and NewLC of CAlfGradientBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGradientBrushNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfGradientBrushSetL tests SetDirection, SetColor, SetImage and 
    * AppendColorL of CAlfGradientBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGradientBrushSetL( CStifItemParser& aItem );
    
    /**
    * TestAlfGradientBrushGetL tests Color, Opacity and Image of CAlfGradientBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGradientBrushGetL( CStifItemParser& aItem );
    
private: //FOR TEST alfgridlayout.h
    /**
    * TestAlfGridLayoutAddNewL tests AddNewL of CAlfGridLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGridLayoutAddNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfGridLayoutFullConstructL tests constructor and ConstructL
    * of CAlfGridLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGridLayoutFullConstructL( CStifItemParser& aItem );
    
    /**
    * TestAlfGridLayoutForLayoutL tests BaseUnit of CAlfGridLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGridLayoutForLayoutL( CStifItemParser& aItem );
    
    /**
    * TestAlfGridLayoutForVisualL tests FindTag, UpdateChildrenLayout and 
    * RemoveAndDestroyAllD of CAlfGridLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGridLayoutForVisualL( CStifItemParser& aItem );
    
    /**
    * TestAlfGridLayoutProtectedFuncL tests PropertyOwnerExtension and 
    * DoRemoveAndDestroyAllD of CAlfGridLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGridLayoutProtectedFuncL( CStifItemParser& aItem );
    
    /**
    * TestAlfGridLayoutSetParametersL tests SetColumnsL, SetRowsL, SetColumnsL,
    * SetRowsL, FillWeightsL, AppendWeightL, InsertWeightL,ReplaceWeightL,
    * RemoveWeightL, DimensionCount, SetExpanding, OrdinalToBlock, 
    * SetLayoutModeFlags and ClearLayoutModeFlags of CAlfGridLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGridLayoutSetParametersL( CStifItemParser& aItem );
    
    /**
    * TestAlfGridLayoutGetParaL tests Weight, LayoutModeFlags, RowCount and
    * ColumnCount of CAlfGridLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfGridLayoutGetParaL( CStifItemParser& aItem );
    
private: //FOR TEST alfimage.h
    /**
    * TestAlfImageConstructorL tests five constructor of TAlfImage.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImageConstructorL( CStifItemParser& aItem );
    
    /**
    * TestAlfImageSetAttributesL tests SetTexture, Translate, Scale and
    * SetTexCoords of TAlfImage.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImageSetAttributesL( CStifItemParser& aItem );
    
    /**
    * TestAlfImageGetAttributesL tests Texture, TopLeft, BottomRight and
    * HasTexture of TAlfImage.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImageGetAttributesL( CStifItemParser& aItem );
    
private: //FOR TEST alfimagebrush.h
    /**
    * TestAlfImageBrushNewL tests NewL and NewLC of CAlfImageBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImageBrushNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfImageBrushSetAndGetParaL tests SetScaleMode, SetImage, SetBorders,
    * SetOffset, SetScale, Image, BorderDrawingEnabled, Offset, 
    * CenterDrawingEnabled and Scale of CAlfImageBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImageBrushSetAndGetParaL( CStifItemParser& aItem );
    
private: //FOR TEST alfimageloaderutil.h
    /**
    * TestAlfImgLoadUtilConstructorL tests constructor of CAlfImageLoaderUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImgLoadUtilConstructorL( CStifItemParser& aItem );
    
    /**
    * TestAlfImgLoadUtilCreateImageLoaderL tests three CreateImageLoaderL and 
    * CreateSVGImageLoaderL of CAlfImageLoaderUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImgLoadUtilCreateImageLoaderL( CStifItemParser& aItem );
    
    /**
    * TestAlfImgLoadUtilSetL tests SetSize and SetSkinInstance 
    * of CAlfImageLoaderUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImgLoadUtilSetL( CStifItemParser& aItem );
    
    /**
    * TestAlfAutoSizeImgLoadUtilSetL tests functions 
    * of CAlfAutoSizeImageLoaderUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfAutoSizeImgLoadUtilSetL( CStifItemParser& aItem );
    
private: //FOR TEST alfimagevisual.h
    /**
    * TestAlfImgVisualFullConstructorL tests constructor and ConstructL
    * of CAlfImageVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImgVisualFullConstructorL( CStifItemParser& aItem );
    
    /**
    * TestAlfImgVisualAddNewL tests AddNewL of CAlfImageVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImgVisualAddNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfImgVisualSetAndGetL tests SetColor, SetColorMode, SetStretch,
    * SetStretchMode, three SetDropShadow, SetTurnAngle, TurnAngle, SetScale,
    * Scale, SetOffset, Offset, SetScaleMode, ScaleMode, SetImage,
    * SetSecondaryImage, SetSecondaryAlpha, SecondaryAlpha, Image and
    * SecondaryImage of CAlfImageVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImgVisualSetAndGetL( CStifItemParser& aItem );
    
    /**
    * TestAlfImgVisualForVisualL tests FindTag, UpdateChildrenLayout and 
    * RemoveAndDestroyAllD of CAlfImageVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImgVisualForVisualL( CStifItemParser& aItem );
    
    /**
    * TestAlfImgVisualProtectedFuncL tests PropertyOwnerExtension and 
    * DoRemoveAndDestroyAllD of CAlfImageVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfImgVisualProtectedFuncL( CStifItemParser& aItem );
    
private: //FOR TEST alflayout.h
    /**
    * TestAlfLayoutFullConstructorL tests constructor and ConstructL
    * of CAlfLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLayoutFullConstructorL( CStifItemParser& aItem );
    
    /**
    * TestAlfLayoutAddNewL tests AddNewL of CAlfLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLayoutAddNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfLayoutSetAndGetL tests EnableScrollingL, Scrolling, ScrollOffset, 
    * SetScrollOffset, ChildOrdinal, ChildPos, ChildSize, SetTransitionTime, 
    * TransitionTime, SetInnerPadding, SetInnerPadding, InnerPadding, 
    * InnerPaddingAsMetric, HorizontalInnerPadding, VerticalInnerPadding,
    * InnerPaddingInBaseUnits EffectiveLayoutOrdinal of CAlfLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLayoutSetAndGetL( CStifItemParser& aItem );
    
    /**
    * TestAlfLayoutForVisualL tests Append, Remove, Reorder, MoveVisualToFront, 
    * MoveVisualToBack, VirtualSize, FindVisual, Insert, Count, Visual, FindTag,
    * UpdateChildrenLayout and RemoveAndDestroyAllD of CAlfLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLayoutForVisualL( CStifItemParser& aItem );

    /**
    * TestAlfLayoutBaseUnitL tests two SetBaseUnit and BaseUnit of CAlfLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLayoutBaseUnitL( CStifItemParser& aItem );
    
    /**
    * TestAlfLayoutProtectedFuncL tests PropertyOwnerExtension and 
    * DoRemoveAndDestroyAllD of CAlfLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLayoutProtectedFuncL( CStifItemParser& aItem );
    
private: //FOR TEST alflinevisual.h
    /**
    * TestAlfLineVisualFullConstructorL tests constructor and ConstructL
    * of CAlfLineVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLineVisualFullConstructorL( CStifItemParser& aItem ); 
    
    /**
    * TestAlfLineVisualAddNewL tests AddNewL of CAlfLineVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLineVisualAddNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfLineVisualSetAndGetL tests SetPath,, Path, SetImage, Image, 
    * SetAlphaFunction, SetWidthFunction, Thickness, SetThickness, 
    * ShadowThickness, SetShadowThickness, StartPos, SetStartPos, EndPos(), 
    * SetEndPos and SetColor of CAlfLineVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLineVisualSetAndGetL( CStifItemParser& aItem );
    
    /**
    * TestAlfLineVisualForVisualL tests FindTag, UpdateChildrenLayout and 
    * RemoveAndDestroyAllD of CAlfLineVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLineVisualForVisualL( CStifItemParser& aItem );
    
    /**
    * TestAlfLineVisualProtectedFuncL tests PropertyOwnerExtension and 
    * DoRemoveAndDestroyAllD of CAlfLineVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLineVisualProtectedFuncL( CStifItemParser& aItem );
    
private: //FOR TEST alfm3gmesh.h
    /**
    * TestAlfM3GMeshLoadSceneL tests LoadSceneL of CAlfM3GMesh.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfM3GMeshLoadSceneL( CStifItemParser& aItem ); 
    
private: //FOR TEST alfmappingfunctions.h
    /**
    * TestAlfConstantMapFuncL tests NewL, SetValue and Value 
    * of CAlfConstantMappingFunction.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfConstantMapFuncL( CStifItemParser& aItem ); 
    
    /**
    * TestAlfLinearMapFuncL tests NewL, SetOffset, Offset, SetFactor and Factor 
    * of CAlfLinearMappingFunction.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfLinearMapFuncL( CStifItemParser& aItem ); 
    
    /**
    * TestAlfSineMapFuncL tests NewL, SetOffset, Offset, SetFactor and Factor 
    * of CAlfSineMappingFunction.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfSineMapFuncL( CStifItemParser& aItem ); 
    
    /**
    * TestAlfCosineMapFuncL tests NewL, SetOffset, Offset, SetFactor and Factor 
    * of CAlfCosineMappingFunction.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfCosineMapFuncL( CStifItemParser& aItem ); 
    
    /**
    * TestAlfAverageMapFuncL tests NewL, SetMappingFunction1, 
    * SetMappingFunction2, SetMappingFunctions and SetWeight 
    * of CAlfAverageMappingFunction.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfAverageMapFuncL( CStifItemParser& aItem ); 
    
    /**
    * TestAlfTableMapFuncL tests NewL and SetMappingTableValues 
    * of CAlfTableMappingFunction.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfTableMapFuncL( CStifItemParser& aItem ); 
    
private: //FOR TEST alfmaterial.h
    /**
    * TestAlfMaterialConstructor tests constructor of CAlfMaterial.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfMaterialConstructor( CStifItemParser& aItem ); 
    
private: //FOR TEST alfmesh.h
    /**
    * TestAlfMeshSetAndGetL tests MeshType, Reset, StretchUniformly, 
    * ScaleUniformly, AnimationControllerCount, SetAnimationPosition, 
    * StartAnimationController and StopAnimationController of CAlfMesh.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfMeshSetAndGetL( CStifItemParser& aItem ); 
    
private: //FOR TEST alfmeshvisual.h
    /**
    * TestAlfMeshVisualFullConstructorL tests constructor and ConstructL
    * of CAlfMeshVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfMeshVisualFullConstructorL( CStifItemParser& aItem ); 
    
    /**
    * TestAlfMeshVisualAddNewL tests AddNewL of CAlfMeshVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfMeshVisualAddNewL( CStifItemParser& aItem );
    
    /**
    * TestAlfMeshVisualSetAndGetL tests CreateMeshL, SetImage, Image, 
    * SetSecondaryImage, SecondaryImage, SetSpecularImage,SpecularImage, Mesh, 
    * ProceduralMesh, M3GMesh, SecondaryAlpha, SetSecondaryAlpha,YawAngle, 
    * SetYawAngle, PitchAngle, SetPitchAngle, Scale and 
    * SetScale of CAlfMeshVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfMeshVisualSetAndGetL( CStifItemParser& aItem );
    
    /**
    * TestAlfMeshVisualForVisualL tests FindTag, UpdateChildrenLayout and 
    * RemoveAndDestroyAllD of CAlfMeshVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfMeshVisualForVisualL( CStifItemParser& aItem );
    
    /**
    * TestAlfMeshVisualProtectedFuncL tests PropertyOwnerExtension and 
    * DoRemoveAndDestroyAllD of CAlfMeshVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestAlfMeshVisualProtectedFuncL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfbatchbuffer.h
    /**
    * TestBBSetAutoFlushModeL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBBSetAutoFlushMode( CStifItemParser& aItem ); 
    /**
    * TestBBAutoFlushModeL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBBAutoFlushMode( CStifItemParser& aItem ); 
    /**
    * TestBBFlushBatchBufferL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBBFlushBatchBufferL( CStifItemParser& aItem ); 
    /**
    * TestBBSetMaxBatchBufferSize test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBBSetMaxBatchBufferSize( CStifItemParser& aItem ); 
    /**
    * TestBBGetBatchBufferInfoL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBBGetBatchBufferInfoL( CStifItemParser& aItem ); 
    
    
private: //FOR TEST alfborderbrush.h
    /**
    * TestBorderBrushNewL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushNewL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushNewLCL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushNewLCL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushOffsetNewL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushOffsetNewL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushOffsetNewLCL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushOffsetNewLCL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushSetImageL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushSetImageL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushImageL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushImageL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushSetColorL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushSetColorL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushThicknessL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushThicknessL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushSetThicknessL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushSetThicknessL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushSetThicknessMetricL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushSetThicknessMetricL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushEdgeOffsetL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushEdgeOffsetL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushSetEdgeOffsetL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushSetEdgeOffsetL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushSetEdgeOffsetMetricL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushSetEdgeOffsetMetricL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushImageOffsetL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushImageOffsetL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushSetImageOffsetL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushSetImageOffsetL( CStifItemParser& aItem ); 
    /**
    * TestBorderBrushSetImageOffsetMetricL test method.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBorderBrushSetImageOffsetMetricL( CStifItemParser& aItem ); 
    
    
private: //FOR TEST alfviewportlayout.h
    /**
    * TestVpLayoutAddNewL test AddNewL of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutAddNewL( CStifItemParser& aItem ); 
    /**
    * TestVpLayoutConstructorL test constructor of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutConstructorL( CStifItemParser& aItem ); 
    /**
    * TestVpLayoutConstructL test ConstructL of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutConstructL( CStifItemParser& aItem ); 
    /**
    * TestVpLayoutSetVirtualSizeL test SetVirtualSize of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutSetVirtualSizeL( CStifItemParser& aItem ); 
    /**
    * TestVpLayoutSetViewportSizeL test SetViewportSize of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutSetViewportSizeL( CStifItemParser& aItem ); 
    /**
    * TestVpLayoutSetViewportPosL test SetViewportPos of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutSetViewportPosL( CStifItemParser& aItem ); 
    /**
    * TestVpLayoutRemoveAndDestroyAllDL test RemoveAndDestroyAllD of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutRemoveAndDestroyAllDL( CStifItemParser& aItem ); 
    /**
    * TestVpLayoutUpdateChildrenLayoutL test UpdateChildrenLayout of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutUpdateChildrenLayoutL( CStifItemParser& aItem ); 
    /**
    * TestVpLayoutFindTagL test FindTag of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutFindTagL( CStifItemParser& aItem ); 
    /**
    * TestVpLayoutBaseUnitL test BaseUnit of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutBaseUnitL( CStifItemParser& aItem ); 
    /**
    * TestVpLayoutDoRemoveAndDestroyAllDL test DoRemoveAndDestroyAllD of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutDoRemoveAndDestroyAllDL( CStifItemParser& aItem ); 
    /**
    * TestVpLayoutPropertyOwnerExtensionL test PropertyOwnerExtension of CAlfViewportLayout.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVpLayoutPropertyOwnerExtensionL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfutil.h
    /**
    * TestUtilPower2 test Power2 of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilPower2( CStifItemParser& aItem );
    /**
    * TestUtilPower2RoundDown test Power2RoundDown of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilPower2RoundDown( CStifItemParser& aItem );
    /**
    * TestUtilInterpolate test Interpolate of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilInterpolate( CStifItemParser& aItem );
    /**
    * TestUtilWrapValue test WrapValue of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilWrapValue( CStifItemParser& aItem );
    /**
    * TestUtilRandomInt test RandomInt of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilRandomInt( CStifItemParser& aItem );
    /**
    * TestUtilRandomReal test RandomReal of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilRandomReal( CStifItemParser& aItem );
    /**
    * TestUtilQuickLengthWithPoint test QuickLength of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilQuickLengthWithPoint( CStifItemParser& aItem );
    /**
    * TestUtilQuickLength test QuickLength of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilQuickLength( CStifItemParser& aItem );
    /**
    * TestUtilQuickNormalize test QuickNormalize of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilQuickNormalize( CStifItemParser& aItem );
    /**
    * TestUtilQuickNormalizeWithVector test QuickNormalize of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilQuickNormalizeWithVector( CStifItemParser& aItem );
    /**
    * TestUtilCrossProduct test CrossProduct of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilCrossProduct( CStifItemParser& aItem );
    /**
    * TestUtilNormalFromPoints test NormalFromPoints of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilNormalFromPoints( CStifItemParser& aItem );
    /**
    * TestUtilShadowMatrix test ShadowMatrix of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilShadowMatrix( CStifItemParser& aItem );
    /**
    * TestUtilFreeMemory test FreeMemory of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilFreeMemory( CStifItemParser& aItem );
    /**
    * TestUtilScreenSize test ScreenSize of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilScreenSize( CStifItemParser& aItem );
    /**
    * TestUtilNearestFontInTwipsL test NearestFontInTwipsL of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilNearestFontInTwipsL( CStifItemParser& aItem );
    /**
    * TestUtilReleaseFont test ReleaseFont of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilReleaseFont( CStifItemParser& aItem );
    /**
    * TestUtilLengthUnit test LengthUnit of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilLengthUnit( CStifItemParser& aItem );
    /**
    * TestUtilColorLightness test ColorLightness of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilColorLightness( CStifItemParser& aItem );
    /**
    * TestUtilScaleFbsBitmapL test ScaleFbsBitmapL of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilScaleFbsBitmapL( CStifItemParser& aItem );
    /**
    * TestUtilCombineMaskFbsBitmapL test CombineMaskFbsBitmapL of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilCombineMaskFbsBitmapL( CStifItemParser& aItem );
    /**
    * TestUtilCropFbsBitmapL test CropFbsBitmapL of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilCropFbsBitmapL( CStifItemParser& aItem );
    /**
    * TestUtilScaleImageL test ScaleImage of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilScaleImageL( CStifItemParser& aItem );
    /**
    * TestUtilCropImageL test CropImage of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilCropImageL( CStifItemParser& aItem );
    /**
    * TestUtilTagMatches test TagMatches of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilTagMatches( CStifItemParser& aItem );
    /**
    * TestUtilConvertBitmapToDisplayModeLCL test ConvertBitmapToDisplayModeLC of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilConvertBitmapToDisplayModeLCL( CStifItemParser& aItem );
    /**
    * TestUtilThemeItemIdL test ConvertBitmapToDisplayModeLC of AlfUtil.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestUtilThemeItemIdL( CStifItemParser& aItem );
    
    
private: //FOR TEST alftransformation.h
    /**
    * TestTransNewL test NewL of CAlfTransformation.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTransNewL( CStifItemParser& aItem );
    /**
    * TestTransNewLCL test NewLC of CAlfTransformation.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTransNewLCL( CStifItemParser& aItem );
    /**
    * TestTransCountL test Count of CAlfTransformation.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTransCountL( CStifItemParser& aItem );
    /**
    * TestTransNonIdentityCountL test NonIdentityCount of CAlfTransformation.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTransNonIdentityCountL( CStifItemParser& aItem );
    /**
    * TestTransStepL test Step of CAlfTransformation.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTransStepL( CStifItemParser& aItem );
    /**
    * TestTransReplaceStepL test ReplaceStep of CAlfTransformation.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTransReplaceStepL( CStifItemParser& aItem );
    /**
    * TestTransLoadIdentityL test LoadIdentity of CAlfTransformation.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTransLoadIdentityL( CStifItemParser& aItem );
    /**
    * TestTransTranslateL test Translate of CAlfTransformation.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTransTranslateL( CStifItemParser& aItem );
    /**
    * TestTransScaleL test Scale of CAlfTransformation.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTransScaleL( CStifItemParser& aItem );
    /**
    * TestTransRotateL test Rotate of CAlfTransformation.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTransRotateL( CStifItemParser& aItem );
    
    
private: //FOR TEST alftimedvalue.h
    /**
    * TestTimedValueConstructor test Constructor of TAlfTimedValue.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTimedValueConstructor( CStifItemParser& aItem );
    /**
    * TestTimedValueValueNow test Methods of TAlfTimedValue.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTimedValueValueNow( CStifItemParser& aItem );
    /**
    * TestTimedValueTargetAndSpeed test Methods of TAlfTimedValue.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTimedValueTargetAndSpeed( CStifItemParser& aItem );
    /**
    * TestTimedValueStyle test Methods of TAlfTimedValue.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTimedValueStyle( CStifItemParser& aItem );
    /**
    * TestTimedValueMappingFunctionIdentifier test Methods of TAlfTimedValue.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTimedValueMappingFunctionIdentifier( CStifItemParser& aItem );
    /**
    * TestTimedValueFlags test Flags of TAlfTimedValue.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTimedValueFlags( CStifItemParser& aItem );
    /**
    * TestRealPointMethods test Methods of TAlfRealPoint.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestRealPointMethods( CStifItemParser& aItem );
    /**
    * TestRealSizeMethods test Methods of TAlfRealSize.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestRealSizeMethods( CStifItemParser& aItem );
    /**
    * TestRealRectMethods test Methods of TAlfRealRect.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestRealRectMethods( CStifItemParser& aItem );
    /**
    * TestTimedPointConstructor test Constructor of TAlfTimedPoint.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTimedPointConstructor( CStifItemParser& aItem );
    /**
    * TestTimedPointSetStyle test SetStyle of TAlfTimedPoint.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTimedPointSetStyle( CStifItemParser& aItem );
    /**
    * TestTimedPointSetMapFuncIdentifier test SetMappingFunctionIdentifier of TAlfTimedPoint.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTimedPointSetMapFuncIdentifier( CStifItemParser& aItem );
    /**
    * TestTimedPointTargetMethods test Target Methods of TAlfTimedPoint.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTimedPointTargetMethods( CStifItemParser& aItem );
    /**
    * TestTimedPointValueNowMethods test ValueNow Methods of TAlfTimedPoint.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTimedPointValueNowMethods( CStifItemParser& aItem );
    
    
private: //FOR TEST alftextvisual.h
    /**
    * TestTxtVisualAddNewL test AddNewL of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualAddNewL( CStifItemParser& aItem );
    /**
    * TestTxtVisualConstructor test Constructor of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualConstructorL( CStifItemParser& aItem );
    /**
    * TestTxtVisualConstructL test ConstructL of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualConstructL( CStifItemParser& aItem );
    /**
    * TestTxtVisualTxtMethodsL test Text Methods of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualTxtMethodsL( CStifItemParser& aItem );
    /**
    * TestTxtVisualStyleMethodsL test Style Methods of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualStyleMethodsL( CStifItemParser& aItem );
    /**
    * TestTxtVisualSetAlignL test SetAlign of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualSetAlignL( CStifItemParser& aItem );
    /**
    * TestTxtVisualSetLineSpacingL test SetLineSpacing of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualSetLineSpacingL( CStifItemParser& aItem );
    /**
    * TestTxtVisualExtentsMethodsL test TextExtents Methods of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualExtentsMethodsL( CStifItemParser& aItem );
    /**
    * TestTxtVisualMaxLineCountMethodsL test MaxLineCount Methods of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualMaxLineCountMethodsL( CStifItemParser& aItem );
    /**
    * TestTxtVisualWrappingMethodsL test Wrapping Methods of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualWrappingMethodsL( CStifItemParser& aItem );
    /**
    * TestTxtVisualBackgroundTypeL test BackgroundType of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualBackgroundTypeL( CStifItemParser& aItem );
    /**
    * TestTxtVisualShadowOpacityMethodsL test ShadowOpacity Methods of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualShadowOpacityMethodsL( CStifItemParser& aItem );
    /**
    * TestTxtVisualColorMethodsL test Color Methods of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualColorMethodsL( CStifItemParser& aItem );
    /**
    * TestTxtVisualEnableShadowL test EnableShadow of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualEnableShadowL( CStifItemParser& aItem );
    /**
    * TestTxtVisualOffsetMethodsL test Offset Methods of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualOffsetMethodsL( CStifItemParser& aItem );
    /**
    * TestTxtVisualRemoveAndDestroyAllDL test RemoveAndDestroyAllD of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualRemoveAndDestroyAllDL( CStifItemParser& aItem );
    /**
    * TestTxtVisualUpdateChildrenLayoutL test UpdateChildrenLayout of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualUpdateChildrenLayoutL( CStifItemParser& aItem );
    /**
    * TestTxtVisualFindTagL test FindTag of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualFindTagL( CStifItemParser& aItem );
    /**
    * TestTxtVisualSetHighlightRangeL test SetHighlightRange of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualSetHighlightRangeL( CStifItemParser& aItem );
    /**
    * TestTxtVisualProtectedMethodsL test protected methods of CAlfTextVisual.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtVisualProtectedMethodsL( CStifItemParser& aItem );
    
    
private: //FOR TEST alftextureprocessor.h
    /**
    * TestTxtProcessorBlurL test BlurL methods of CAlfTextureProcessor.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtProcessorBlurL( CStifItemParser& aItem );
    
    
private: // FOR TEST alftexturegroup.h
    /**
    * TestTxtGroupNewL test NewL of CAlfTextureGroup.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtGroupNewL( CStifItemParser& aItem );
    /**
    * TestTxtGroupNewLCL test NewLC of CAlfTextureGroup.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtGroupNewLCL( CStifItemParser& aItem );
    /**
    * TestTxtGroupTextureMethodsL test Texture Methods of CAlfTextureGroup.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtGroupTextureMethodsL( CStifItemParser& aItem );
    /**
    * TestTxtGroupIsLoadingCompletedL test IsLoadingCompleted of CAlfTextureGroup.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtGroupIsLoadingCompletedL( CStifItemParser& aItem );
    /**
    * TestTxtGroupObserverMethodsL test Observer Methods of CAlfTextureGroup.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtGroupObserverMethodsL( CStifItemParser& aItem );
    
    
private: //FOR TEST alftexturemanager.h
    /**
    * TestTxtMgrEnv test Env of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrEnv( CStifItemParser& aItem );
    /**
    * TestTxtMgrTextureL test Texture of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrTextureL( CStifItemParser& aItem );
    /**
    * TestTxtMgrImagePathL test ImagePath Methods of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrImagePathL( CStifItemParser& aItem );
    /**
    * TestTxtMgrBlankTexture test BlankTexture of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrBlankTexture( CStifItemParser& aItem );
    /**
    * TestTxtMgrLoadTextureL test LoadTextureL of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrLoadTextureL( CStifItemParser& aItem );
    /**
    * TestTxtMgrCreateTextureL test CreateTextureL of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrCreateTextureL( CStifItemParser& aItem );
    /**
    * TestTxtMgrUnloadTextureL test UnloadTexture of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrUnloadTextureL( CStifItemParser& aItem );
    /**
    * TestTxtMgrUpdateTextureL test UpdateTexture of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrUpdateTextureL( CStifItemParser& aItem );
    /**
    * TestTxtMgrDefineFileNameL test DefineFileNameL of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrDefineFileNameL( CStifItemParser& aItem );
    /**
    * TestTxtMgrPrependImagePath test PrependImagePath of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrPrependImagePath( CStifItemParser& aItem );
    /**
    * TestTxtMgrObserverMethodsL test Observer Methods of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrObserverMethodsL( CStifItemParser& aItem );
    /**
    * TestTxtMgrProcessor test Processor of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrProcessor( CStifItemParser& aItem );
    /**
    * TestTxtMgrSetAutomaticTextureIdRange test SetAutomaticTextureIdRange of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrSetAutomaticTextureIdRange( CStifItemParser& aItem );
    /**
    * TestTxtMgrIsLoadedL test IsLoaded of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrIsLoadedL( CStifItemParser& aItem );
    /**
    * TestTxtMgrTextureIdL test TextureId of CAlfTextureManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtMgrTextureIdL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfstatic.h
    /**
    * TestStaticEnv test Env of CAlfStatic.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStaticEnv( CStifItemParser& aItem );
    
    
private: //FOR TEST alfsymbiansbdrawer.h
    /**
    * TestBufDrawerDrawBufferL test DrawBuffer of CAlfSymbianBufferDrawer.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBufDrawerDrawBufferL( CStifItemParser& aItem );
    /**
    * TestBufDrawerGetBufferBitmapsL test GetBufferBitmaps of CAlfSymbianBufferDrawer.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBufDrawerGetBufferBitmapsL( CStifItemParser& aItem );
    /**
    * TestBufDrawerReleaseDrawerL test ReleaseDrawer of CAlfSymbianBufferDrawer.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBufDrawerReleaseDrawerL( CStifItemParser& aItem );
    
    
private: //FOR TEST alftextstylemanager.h
    /**
    * TestStyleMgrTextStyleMethodsL test TextStyle Methods of CAlfTextStyleManager.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStyleMgrTextStyleMethodsL( CStifItemParser& aItem );
    
    
private: //FOR TEST alftextstyle.h
    /**
    * TestStyleParentIdL test ParentId of CAlfTextStyle.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStyleParentIdL( CStifItemParser& aItem );
    /**
    * TestStyleIdL test Id of CAlfTextStyle.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStyleIdL( CStifItemParser& aItem );
    /**
    * TestStyleTextColorL test TextColor of CAlfTextStyle.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStyleTextColorL( CStifItemParser& aItem );
    /**
    * TestStyleTextSizeL test TextSize Methods of CAlfTextStyle.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStyleTextSizeL( CStifItemParser& aItem );
    /**
    * TestStyleBoldL test Bold Methods of CAlfTextStyle.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStyleBoldL( CStifItemParser& aItem );
    /**
    * TestStyleItalicL test Italic Methods of CAlfTextStyle.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStyleItalicL( CStifItemParser& aItem );
    /**
    * TestStyleUnderlineL test Underline Methods of CAlfTextStyle.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStyleUnderlineL( CStifItemParser& aItem );
    /**
    * TestStyleStrikeThroughL test StrikeThrough Methods of CAlfTextStyle.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStyleStrikeThroughL( CStifItemParser& aItem );
    /**
    * TestStyleTypefaceNameL test TypefaceNameL of CAlfTextStyle.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStyleTypefaceNameL( CStifItemParser& aItem );
    /**
    * TestStyleSetTextPaneHeightInPixelsL test SetTextPaneHeightInPixels of CAlfTextStyle.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestStyleSetTextPaneHeightInPixelsL( CStifItemParser& aItem );
    
    
private: //FOR TEST alftexture.h
    /**
    * TestTextureGetMethods test Get Methods of CAlfTexture.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTextureGetMethods( CStifItemParser& aItem );
    /**
    * TestTextureAnimation test Animation Methods of CAlfTexture.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTextureAnimation( CStifItemParser& aItem );
    /**
    * TestTextureSetMethodsL test Set Methods of CAlfTexture.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTextureSetMethodsL( CStifItemParser& aItem );
    
    
private: //FOR TEST alftextureautosizeparams.h
    /**
    * TestTxtAutoSizeParamsConstructor test Constructor of CAlfTexture.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtAutoSizeParamsConstructor( CStifItemParser& aItem );
    /**
    * TestTxtAutoSizeParamsGetMethods test Get Methods of TAlfTextureAutoSizeParams.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtAutoSizeParamsGetMethods( CStifItemParser& aItem );
    /**
    * TestTxtAutoSizeParamsSetMethods test Set Methods of CAlfTexture.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTxtAutoSizeParamsSetMethods( CStifItemParser& aItem );
    
    
private: //FOR TEST alfshadowborderbrush.h
    /**
    * TestShadowBBNewL test NewL Methods of CAlfShadowBorderBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestShadowBBNewL( CStifItemParser& aItem );
    /**
    * TestShadowBBNewLCL test NewLCL Methods of CAlfShadowBorderBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestShadowBBNewLCL( CStifItemParser& aItem );
    /**
    * TestShadowBBWidthMethodsL test Width Methods of CAlfShadowBorderBrush.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestShadowBBWidthMethodsL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfpropertyowner.h
    /**
    * TestPropOwnerConstructorL test Constructor of CAlfPropertyOwner.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestPropOwnerConstructorL( CStifItemParser& aItem );
    /**
    * TestPropOwnerPropertyRemoveL test PropertyRemove of CAlfPropertyOwner.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestPropOwnerPropertyRemoveL( CStifItemParser& aItem );
    /**
    * TestPropOwnerPropertyClearL test PropertyClear of CAlfPropertyOwner.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestPropOwnerPropertyClearL( CStifItemParser& aItem );
    /**
    * TestPropOwnerPropertyIntegerL test PropertyInteger Methods of CAlfPropertyOwner.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestPropOwnerPropertyIntegerL( CStifItemParser& aItem );
    /**
    * TestPropOwnerPropertyStringL test PropertyString Methods of CAlfPropertyOwner.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestPropOwnerPropertyStringL( CStifItemParser& aItem );
    /**
    * TestPropOwnerCopyPropertyL test CopyProperty of CAlfPropertyOwner.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestPropOwnerCopyPropertyL( CStifItemParser& aItem );
    /**
    * TestPropOwnerPropertyOwnerExtensionL test PropertyOwnerExtension of CAlfPropertyOwner.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestPropOwnerPropertyOwnerExtensionL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfroster.h
    /**
    * TestRosterCtrlGroupMethodsL test ControlGroup Methods of CAlfRoster.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestRosterCtrlGroupMethodsL( CStifItemParser& aItem );
    /**
    * TestRosterCtrlMethodsL test Control Methods of CAlfRoster.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestRosterCtrlMethodsL( CStifItemParser& aItem );
    /**
    * TestRosterVisualMethodsL test Visual Methods of CAlfRoster.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestRosterVisualMethodsL( CStifItemParser& aItem );
    /**
    * TestRosterPointerEventObserverMethodsL test PointerEventObserver Methods of CAlfRoster.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestRosterPointerEventObserverMethodsL( CStifItemParser& aItem );
    /**
    * TestRosterSetPointerDragThresholdL test SetPointerDragThreshold of CAlfRoster.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestRosterSetPointerDragThresholdL( CStifItemParser& aItem );
    /**
    * TestRosterDisableLongTapEventsWhenDraggingL test DisableLongTapEventsWhenDragging of CAlfRoster.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestRosterDisableLongTapEventsWhenDraggingL( CStifItemParser& aItem );
    /**
    * TestRosterCtrlGroupOrderChangedObserverL test ControlGroupOrderChangedObserver Methods of CAlfRoster.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestRosterCtrlGroupOrderChangedObserverL( CStifItemParser& aItem );

    
private: //FOR TEST alfscreenbuffer.h
    /**
    * TestScrBufNewLMethodsL test NewL Methods of CAlfScreenBuffer.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestScrBufNewLMethodsL( CStifItemParser& aItem );
    /**
    * TestScrBufGetMethodsL test Get Methods of CAlfScreenBuffer.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestScrBufGetMethodsL( CStifItemParser& aItem );
    /**
    * TestScrBufRequestMethodsL test Request Methods of CAlfScreenBuffer.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestScrBufRequestMethodsL( CStifItemParser& aItem );
    /**
    * TestScrBufObserverMethodsL test Observer Methods of CAlfScreenBuffer.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestScrBufObserverMethodsL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfscrollerevent.h
    /**
    * TestScrollEventMethods test Methods of TAlfScrollerEvent.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestScrollEventMethods( CStifItemParser& aItem );
    
    
private: //FOR TEST alfmetric.h
    /**
    * TestMetricMethods test Methods of TAlfMetric.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestMetricMethods( CStifItemParser& aItem );
    /**
    * TestXYMetricMethods test Methods of TAlfXYMetric.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestXYMetricMethods( CStifItemParser& aItem );
    /**
    * TestBoxMetricMethods test Methods of TAlfBoxMetric.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestBoxMetricMethods( CStifItemParser& aItem );
    
    
private: //FOR TEST alfproceduralmesh.h
    /**
    * TestProMeshMakeMethodsL test Make Methods of CAlfProceduralMesh.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestProMeshMakeMethodsL( CStifItemParser& aItem );
    /**
    * TestProMeshMaterialMethodsL test Material Methods of CAlfProceduralMesh.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestProMeshMaterialMethodsL( CStifItemParser& aItem );
    
    
private: //FOR TEST alfcommand.h
    /**
    * TestCmdMethods test Methods of TAlfCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCmdMethods( CStifItemParser& aItem );
    /**
    * TestActCmdMethodsL test Methods of TAlfActionCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestActCmdMethodsL( CStifItemParser& aItem );
    /**
    * TestObjCmdMethodsL test Methods of TAlfObjectCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestObjCmdMethodsL( CStifItemParser& aItem );
    /**
    * TestGroupCmdMethodsL test Methods of TAlfGroupCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestGroupCmdMethodsL( CStifItemParser& aItem );
    /**
    * TestCtrlCmdMethodsL test Methods of TAlfControlCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCtrlCmdMethodsL( CStifItemParser& aItem );
    /**
    * TestVisualCmdMethodsL test Methods of TAlfVisualCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestVisualCmdMethodsL( CStifItemParser& aItem );
    /**
    * TestImageCmdMethodsL test Methods of TAlfImageCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestImageCmdMethodsL( CStifItemParser& aItem );
    /**
    * TestTextCmdMethodsL test Methods of TAlfTextCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTextCmdMethodsL( CStifItemParser& aItem );
    /**
    * TestValueCmdMethodsL test Methods of TAlfValueCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestValueCmdMethodsL( CStifItemParser& aItem );
    /**
    * TestPointCmdMethodsL test Methods of TAlfPointCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestPointCmdMethodsL( CStifItemParser& aItem );
    /**
    * TestTransCmdMethodsL test Methods of TAlfTransformationCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestTransCmdMethodsL( CStifItemParser& aItem );
    /**
    * TestMarkerCmdMethodsL test Methods of TAlfMarkerCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestMarkerCmdMethodsL( CStifItemParser& aItem );
    /**
    * TestCustomEventCmdMethodsL test Methods of TAlfCustomEventCommand.
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    virtual TInt TestCustomEventCmdMethodsL( CStifItemParser& aItem );
    
private: //FOR TEST alfbrush.h
    /**
     * TestAlfBrushOpacityL test Opacity method of CAlfBrush.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestAlfBrushOpacityL( CStifItemParser& aItem );
    /**
     * TestAlfBrushSetOpacityL test SetOpacity method of CAlfBrush.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestAlfBrushSetOpacityL( CStifItemParser& aItem );
    /**
     * TestAlfBrushSetLayerL test SetLayer method of CAlfBrush.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestAlfBrushSetLayerL( CStifItemParser& aItem );
    /**
     * TestAlfBrushSetClipToVisualL test SetClipToVisual method of CAlfBrush.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestAlfBrushSetClipToVisualL( CStifItemParser& aItem );
    /**
     * TestAlfBrushClipToVisualL test ClipToVisual method of CAlfBrush.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestAlfBrushClipToVisualL( CStifItemParser& aItem );
    /**
     * TestAlfBrushIdentifierL test Identifier method of CAlfBrush.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestAlfBrushIdentifierL( CStifItemParser& aItem );
    /**
     * TestAlfBrushCommsL test Comms method of CAlfBrush.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestAlfBrushCommsL( CStifItemParser& aItem );
    /**
     * TestAlfBrushConstructLWithParamL test ConstructL method with param of CAlfBrush.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestAlfBrushConstructLWithParamL( CStifItemParser& aItem );
    /**
     * TestAlfBrushConstructL test ConstructL method of CAlfBrush.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestAlfBrushConstructL( CStifItemParser& aItem );
    /**
     * TestAlfBrushPropertyOwnerExtL test PropertyOwnerExtension method of CAlfBrush.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestAlfBrushPropertyOwnerExtL( CStifItemParser& aItem );
    
private: //FOR TEST alfbrusharray.h
    /**
     * TestBrushArrayResetL test Reset of CAlfBrushArray.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestBrushArrayResetL( CStifItemParser& aItem );
    /**
     * TestBrushArrayAppendL test AppendL of CAlfBrushArray.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestBrushArrayAppendL( CStifItemParser& aItem );
    /**
     * TestBrushArrayInsertL test InsertL of CAlfBrushArray.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestBrushArrayInsertL( CStifItemParser& aItem );
    /**
     * TestBrushArrayRemoveL test Remove of CAlfBrushArray.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestBrushArrayRemoveL( CStifItemParser& aItem );
    /**
     * TestBrushArrayCountL test Count of CAlfBrushArray.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestBrushArrayCountL( CStifItemParser& aItem );
    /**
     * TestBrushArrayOperatorL test operator [] of CAlfBrushArray.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestBrushArrayOperatorL( CStifItemParser& aItem );
    /**
     * TestBrushArrayAtL test At of CAlfBrushArray.
     * @since S60 5.0
     * @param aItem is not used.
     * @return Symbian OS error code.
     */
    virtual TInt TestBrushArrayAtL( CStifItemParser& aItem );
    
private:    // Data

    /**
     * ScreenSaver Property
     */
    TInt iOldScreenSaverProperty;
    
    /**
     * Own: pointer to CAlfEnv object
     */
    CAlfEnv* iAlfEnv;
    
    /**
     * Own by iAlfEnv for binding to display
     */
    CAlfControl* iAlfCtl;
    
    /**
     * Own by iAlfEnv
     */
    CAlfDisplay* iAlfDisplay;
    
    /**
     * Own by iAlfEnv
     */
    CAlfControlGroup* iAlfCtrlGroup;

    };

#endif      // C_TESTPLATALFVISUAL_H

// End of File




