/**
 * @note Contents of this file are provided only if module test hooks are set on.
 */

#include "alfmoduletestconf.h"
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF

#ifndef ALFMODULETESTTYPE_H
#define ALFMODULETESTTYPE_H

/**
 * TAlfModuleTestType specifies recognized test types.
 */
enum TAlfModuleTestType
    {
    // Do not use this value when creating item.
    EAlfModuleTestTypeNone,
    
    // Render stage component specific tests
    EAlfModuleTestTypeRenderStageCreateWindow,
    EAlfModuleTestTypeRenderStageReleaseWindow,    
    EAlfModuleTestTypeRenderStageCreateWindowGroup,
    EAlfModuleTestTypeRenderStageReleaseWindowGroup,
    EAlfModuleTestTypeRenderStageActiveWindow,
    EAlfModuleTestTypeRenderStageChangeWindowSize,
    EAlfModuleTestTypeRenderStageChangeWindowPosition,
    EAlfModuleTestTypeRenderStageChangeWindowVisibility,
    EAlfModuleTestTypeRenderStageChangeTextCursorType,
    EAlfModuleTestTypeRenderStageChangeTextCursorClipRect,
    EAlfModuleTestTypeRenderStageChangeTextCursorFlag,
    EAlfModuleTestTypeRenderStageChangeTextCursorColor,
    
    // Streamer hierarchy model component specific tests
    EAlfModuleTestTypeHierarchyModelCreateLayer,
    EAlfModuleTestTypeHierarchyModelReleaseLayer,
    EAlfModuleTestTypeHierarchyModelCreateWindow,
    EAlfModuleTestTypeHierarchyModelReleaseWindow,    
    EAlfModuleTestTypeHierarchyModelCreateWindowGroup,
    EAlfModuleTestTypeHierarchyModelReleaseWindowGroup,
    EAlfModuleTestTypeHierarchyModelActiveWindow,
    EAlfModuleTestTypeHierarchyModelChangeWindowSize,
    EAlfModuleTestTypeHierarchyModelChangeWindowPosition,
    EAlfModuleTestTypeHierarchyModelChangeWindowVisibility,
    EAlfModuleTestTypeHierarchyModelChangeTextCursorType,
    EAlfModuleTestTypeHierarchyModelChangeTextCursorClipRect,
    EAlfModuleTestTypeHierarchyModelChangeTextCursorFlag,
    EAlfModuleTestTypeHierarchyModelChangeTextCursorColor,
    
    // Server bridge component specific tests
    EAlfModuleTestTypeBridgeCreateLayer,
    EAlfModuleTestTypeBridgeReleaseLayer,
    EAlfModuleTestTypeBridgeCreateWindow,
    EAlfModuleTestTypeBridgeReleaseWindow,    
    EAlfModuleTestTypeBridgeCreateWindowGroup,
    EAlfModuleTestTypeBridgeReleaseWindowGroup,
    EAlfModuleTestTypeBridgeActiveWindow,
    EAlfModuleTestTypeBridgeChangeWindowSize,
    EAlfModuleTestTypeBridgeChangeWindowPosition,
    EAlfModuleTestTypeBridgeChangeWindowVisibility,
    EAlfModuleTestTypeBridgeChangeTextCursorType,
    EAlfModuleTestTypeBridgeChangeTextCursorClipRect,
    EAlfModuleTestTypeBridgeChangeTextCursorFlag,
    EAlfModuleTestTypeBridgeChangeTextCursorColor,
    EAlfModuleTestTypeBridgeVisualVisibility,
    EAlfModuleTestTypeBridgeGoom,
    
    // Core toolkit component specific tests
    EAlfModuleTestTypeCoreToolkitDrawWindow,
    EAlfModuleTestTypeCoreToolkitDrawFromRenderBuffer,
    EAlfModuleTestTypeCoreToolkitGoom,
    
    // Layer tests
    EAlfModuleTestTypeCreateLayer,
    EAlfModuleTestTypeReleaseLayer,
    EAlfModuleTestTypeLayerOrdinalPosition,
    EAlfModuleTestTypeLayerNodeOrdinalPosition,
    EAlfModuleTestTypeGceCreateLayer,
    EAlfModuleTestTypeGceReleaseLayer,
    EAlfModuleTestTypeGceLayerPosition,
    EAlfModuleTestTypeGceLayerExtent,
    EAlfModuleTestTypeGceLayerOpacity,
    
    // Do not use this value when creating item.
    // This is just meant for Find operations when all tests are accepted.
    EAlfModuleTestTypeAll
    };

#endif // ALFMODULETESTTYPE_H

#endif // USE_MODULE_TEST_HOOKS_FOR_ALF

// End of File
