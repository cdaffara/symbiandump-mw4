#ifndef ALFMODULETEST_H
#define ALFMODULETEST_H


#include <e32base.h>
#include <graphics/surface.h>
#include <e32debug.h>

// Informs if module test hooks have been set on.
#include "alfmoduletestconf.h"

// Provides module test hooks defines.
#include "alfmoduletestdefines.h"


#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF

// *** Test map classes
#include "alfmoduletesttype.h"
#include "alfmoduletestitem.h"
#include "alfmoduletestmap.h"


// *** Global object names
_LIT(KAlfModuleTestChunkName, "ALF_MODULE_TEST_CHUNK");
_LIT(KAlfModuleTestMutexName, "ALF_MODULE_TEST_MUTEX");

const TInt KMaxSimultMeasurementCount = 10;
/*
 *  Class CAlfModuleTestData
 */

NONSHARABLE_CLASS(CAlfModuleTestData) : public CBase
    {
public:
    void PrintState() const
        {
        RDebug::Print(_L("*** ALF INTERNAL STATE ***"));
        RDebug::Print(_L("iTotalLayerCount[0]=%d"), iTotalLayerCount[0]);
        RDebug::Print(_L("iTotalLayerCount[1]=%d"), iTotalLayerCount[1]);
        RDebug::Print(_L("iCloneLayerCount=%d"), iCloneLayerCount);
        RDebug::Print(_L("iRsTotalNodeCount=%d"), iRsTotalNodeCount);
        RDebug::Print(_L("iRsWindowGroupNodeCount=%d"), iRsWindowGroupNodeCount);
        RDebug::Print(_L("iRsWindowNodeCount=%d"), iRsWindowNodeCount);
        RDebug::Print(_L("iRsAnimNodeCount=%d"), iRsAnimNodeCount);
        RDebug::Print(_L("iRsWindowNodeActivatedCount=%d"), iRsWindowNodeActivatedCount);
        RDebug::Print(_L("iRsNodeExtentChangedCount=%d"), iRsNodeExtentChangedCount);
        RDebug::Print(_L("iRsLatestNodeExtentRect= x:%d, y:%d, width=%d, height=%d"), 
                         iRsLatestNodeExtentRect.iTl.iX, iRsLatestNodeExtentRect.iTl.iY, 
                         iRsLatestNodeExtentRect.Width(), iRsLatestNodeExtentRect.Height());
        RDebug::Print(_L("iRsTotalNodeFlagChangedCount=%d"), iRsTotalNodeFlagChangedCount);
        RDebug::Print(_L("iRsTotalNodeAttributeChangedCount=%d"), iRsTotalNodeAttributeChangedCount);
        RDebug::Print(_L("iTotalNodeCount=%d"), iTotalNodeCount);
        RDebug::Print(_L("iWindowGroupNodeCount=%d"), iWindowGroupNodeCount);
        RDebug::Print(_L("iWindowNodeCount=%d"), iWindowNodeCount);
        RDebug::Print(_L("iAnimNodeCount=%d"), iAnimNodeCount);
        RDebug::Print(_L("iWindowNodeActivatedCount=%d"), iWindowNodeActivatedCount);
        RDebug::Print(_L("iNodeExtentChangedCount=%d"), iNodeExtentChangedCount);
        RDebug::Print(_L("iLatestNodeExtentRect= x:%d, y:%d, width=%d, height=%d"), 
                         iLatestNodeExtentRect.iTl.iX, iLatestNodeExtentRect.iTl.iY, 
                         iLatestNodeExtentRect.Width(), iLatestNodeExtentRect.Height());
        RDebug::Print(_L("iTotalNodeFlagChangedCount=%d"), iTotalNodeFlagChangedCount);
        RDebug::Print(_L("iTotalNodeAttributeChangedCount=%d"), iTotalNodeAttributeChangedCount);        
        RDebug::Print(_L("iTotalVisualCount=%d"), iTotalVisualCount);
        RDebug::Print(_L("iVisibleVisualCount=%d"), iVisibleVisualCount);
        RDebug::Print(_L("iActiveVisualCount=%d"), iActiveVisualCount);
        RDebug::Print(_L("iPassiveVisualCount=%d"), iPassiveVisualCount);
        RDebug::Print(_L("iTextureCount=%d"), iTextureCount);
        RDebug::Print(_L("iRenderBufferCount=%d"), iRenderBufferCount);
        RDebug::Print(_L("iTotalControlGroupCount=%d"), iTotalControlGroupCount);        
        RDebug::Print(_L("iVisualSizeChangedCount=%d"), iVisualSizeChangedCount);        
        RDebug::Print(_L("iVisualPositionChangedCount=%d"), iVisualPositionChangedCount);
        RDebug::Print(_L("iLatestVisualExtentRect= x:%d, y:%d, width=%d, height=%d"), 
                         iLatestVisualExtentRect.iTl.iX, iLatestVisualExtentRect.iTl.iY, 
                         iLatestVisualExtentRect.Width(), iLatestVisualExtentRect.Height());
        RDebug::Print(_L("iTotalVisualFlagChangedCount=%d"), iTotalVisualFlagChangedCount);
        RDebug::Print(_L("iTotalVisualAttributeChangedCount=%d"), iTotalVisualAttributeChangedCount);
        RDebug::Print(_L("iBoolMap =>"));
        PrintBoolMapState( iBoolMap );        
        RDebug::Print(_L("iIntMap =>"));
        PrintIntMapState( iIntMap );
        RDebug::Print(_L("iSizeMap =>"));
        PrintSizeMapState( iSizeMap );
        RDebug::Print(_L("iPositionMap =>"));
        PrintPositionMapState( iPositionMap );
        RDebug::Print(_L("iSurfaceMap =>"));
        PrintSurfaceMapState( iSurfaceMap );        
        RDebug::Print(_L("*** ALF INTERNAL STATE ***"));
        }


    void PrintBoolMapState( const TAlfModuleTestMap< TBool >& aMap ) const
        {
        RDebug::Print(_L("*** ALF INTERNAL BOOL MAP STATE -->"));
        RDebug::Print(_L("Map item count=%d"), aMap.ItemCount());
        for ( TInt i = 0; i < aMap.ItemCount(); ++i )
            {
            const TAlfModuleTestItem< TBool >& item( aMap.Item( i ) );
            RDebug::Print(_L("Map item %d, iTestType=%d, iKey=%d, iValue=%d, iValueSetCount=%d, iLinkTargetKey=%d"),
                             i, item.TestType(), item.Key(), item.Value(), item.ValueSetCount(), item.LinkTargetKey());
            }
        RDebug::Print(_L("<-- ALF INTERNAL BOOL MAP STATE ***"));
        }
    
    
    void PrintIntMapState( const TAlfModuleTestMap< TInt >& aMap ) const
        {
        RDebug::Print(_L("*** ALF INTERNAL INT MAP STATE -->"));
        RDebug::Print(_L("Map item count=%d"), aMap.ItemCount());
        for ( TInt i = 0; i < aMap.ItemCount(); ++i )
            {
            const TAlfModuleTestItem< TInt >& item( aMap.Item( i ) );
            RDebug::Print(_L("Map item %d, iTestType=%d, iKey=%d, iValue=%d, iValueSetCount=%d, iLinkTargetKey=%d"),
                             i, item.TestType(), item.Key(), item.Value(), item.ValueSetCount(), item.LinkTargetKey());
            }
        RDebug::Print(_L("<-- ALF INTERNAL INT MAP STATE ***"));
        }
    
    
    void PrintSizeMapState( const TAlfModuleTestMap< TSize >& aMap ) const
        {
        RDebug::Print(_L("*** ALF INTERNAL SIZE MAP STATE -->"));
        RDebug::Print(_L("Map item count=%d"), aMap.ItemCount());
        for ( TInt i = 0; i < aMap.ItemCount(); ++i )
            {
            const TAlfModuleTestItem< TSize >& item( aMap.Item( i ) );
            RDebug::Print(_L("Map item %d, iTestType=%d, iKey=%d, iValueSetCount=%d, iLinkTargetKey=%d"),
                             i, item.TestType(), item.Key(), item.ValueSetCount(), item.LinkTargetKey());
            RDebug::Print(_L("Map item index=%d, width=%d, height=%d"),
                             i, item.Value().iWidth, item.Value().iHeight);
            }
        RDebug::Print(_L("<-- ALF INTERNAL SIZE MAP STATE ***"));
        }

    
    void PrintPositionMapState( const TAlfModuleTestMap< TPoint >& aMap ) const
        {
        RDebug::Print(_L("*** ALF INTERNAL POSITION MAP STATE -->"));
        RDebug::Print(_L("Map item count=%d"), aMap.ItemCount() );
        for ( TInt i = 0; i < aMap.ItemCount(); ++i )
            {
            const TAlfModuleTestItem< TPoint >& item( aMap.Item( i ) );
            RDebug::Print(_L("Map item %d, iKey=%d, iTestType=%d, iValueSetCount=%d, iLinkTargetKey=%d"),
                             i, item.Key(), item.TestType(), item.ValueSetCount(), item.LinkTargetKey());
            RDebug::Print(_L("Map item index=%d, x=%d, y=%d"),
                             i, item.Value().iX, item.Value().iY);
            }
        RDebug::Print(_L("<-- ALF INTERNAL POSITION MAP STATE ***"));
        }

    
    void PrintSurfaceMapState( const TAlfModuleTestMap< TSurfaceId >& aMap ) const
        {
        RDebug::Print(_L("*** ALF INTERNAL SURFACE MAP STATE -->"));
        RDebug::Print(_L("Map item count=%d"), aMap.ItemCount() );
        for ( TInt i = 0; i < aMap.ItemCount(); ++i )
            {
            const TAlfModuleTestItem< TSurfaceId >& item( aMap.Item( i ) );
            RDebug::Print(_L("Map item %d, iKey=%d, iTestType=%d, iValueSetCount=%d, iLinkTargetKey=%d"),
                             i, item.Key(), item.TestType(), item.ValueSetCount(), item.LinkTargetKey());
            RDebug::Print(_L("Map item index=%d, internal0=%d, internal1=%d, internal2=%d, internal3=%d"),
                             i, item.Value().iInternal[ 0 ], item.Value().iInternal[ 1 ],
                             item.Value().iInternal[ 2 ], item.Value().iInternal[ 3 ] );
            }
        RDebug::Print(_L("<-- ALF INTERNAL SURFACE MAP STATE ***"));
        }


public:
    TBool iIsEnabled;           // *** not yet implemented. For run-time enabling/disabling of the test system.  
    
    // Alf Render Stage
    TInt iScreenCount;          // *** not yet implemented
    TInt iTotalLayerCount[10];  // For each screen
    TInt iCloneLayerCount;      // ** not yet implemented
    TInt iRsTotalNodeCount;
    TInt iRsWindowGroupNodeCount;
    TInt iRsWindowNodeCount;
    TInt iRsAnimNodeCount;
    TInt iRsWindowNodeActivatedCount;    
    TInt iRsNodeExtentChangedCount;
    TRect iRsLatestNodeExtentRect;
    TInt iRsTotalNodeFlagChangedCount;
    TInt iRsTotalNodeAttributeChangedCount;
    // These are temporary variables for Alf Render Stage thread internal use only!
    TInt iARS_Temp1;
    TInt iARS_Temp2;
    TInt iARS_Temp3;
    TInt iARS_Temp4;
   
    // Alf Streamer
    TInt iTotalNodeCount;
    TInt iWindowGroupNodeCount;
    TInt iWindowNodeCount;
    TInt iAnimNodeCount;
    TInt iWindowNodeActivatedCount;
    TInt iNodeExtentChangedCount;
    TRect iLatestNodeExtentRect;
    TInt iTotalNodeFlagChangedCount;
    TInt iTotalNodeAttributeChangedCount;
    // These are temporary variables for Alf Streamer thread internal use only!
    TInt iAST_Temp1;
    TInt iAST_Temp2;
    TInt iAST_Temp3;
    TInt iAST_Temp4;
   
    // Alf Server
    TInt iTotalVisualCount;
    TInt iVisibleVisualCount;
    TInt iActiveVisualCount;
    TInt iPassiveVisualCount;
    TInt iTextureCount;         // *** not yet implemented
    TInt iRenderBufferCount;    // *** not yet implemented
    TInt iTotalControlGroupCount;
    TInt iVisualSizeChangedCount;
    TInt iVisualPositionChangedCount;
    TRect iLatestVisualExtentRect;
    TInt iTotalVisualFlagChangedCount;
    TInt iTotalVisualAttributeChangedCount;
    TInt iOrdinalChange;
    // These are temporary variables for Alf Server thread internal use only!
    TInt iASE_Temp1;
    TInt iASE_Temp2;
    TInt iASE_Temp3;
    TInt iASE_Temp4;

    class TSpeedTest
    {
    public:
        TInt64 iHandle;
        TInt iCounter;
        TBool iEffects;
        TInt64 iTimeStamp[5];
    };
    
    TSpeedTest iTimeMap[10];
    TInt iNextFreeMap;
        
    // Map that contains boolean items for certain test cases.
    TAlfModuleTestMap< TBool > iBoolMap;        
    // Map that contains integer items for certain test cases.
    TAlfModuleTestMap< TInt > iIntMap;    
    // Map that contains size items for certain test cases.
    TAlfModuleTestMap< TSize > iSizeMap;
    // Map that contains position items for certain test cases.
    TAlfModuleTestMap< TPoint > iPositionMap;
    // Map that contains rect items for certain test cases.
    TAlfModuleTestMap< TRect > iRectMap;
    // Map that contains surface id items for layer/surface test cases.
    TAlfModuleTestMap< TSurfaceId > iSurfaceMap;
    
    };


/*
 *  Class CAlfModuleTestDataControl
 */
NONSHARABLE_CLASS(CAlfModuleTestDataControl) : public CBase
    {
public:
    /*
     * Destructor
     */
    ~CAlfModuleTestDataControl()
        {
        iModuleTestMutex.Close();
        iModuleTestChunk.Close();
        }

    /*
     * Open global chunk and mutex created elsewhere
     */
    TInt OpenGlobalObjects()
        {
        RDebug::Print(_L("CAlfModuleTestDataControl::OpenGlobalObjects()."));
        // Open global module testing chunk
        TBool isReadOnly = EFalse;
        TInt err = iModuleTestChunk.OpenGlobal(KAlfModuleTestChunkName, isReadOnly);
        if (!err)
            {
            // Create global module testing mutex
            err = iModuleTestMutex.OpenGlobal(KAlfModuleTestMutexName);
            if (!err)
                {
                iModuleTestData = reinterpret_cast<CAlfModuleTestData*>(iModuleTestChunk.Base());
                }
            }
        RDebug::Print(_L("CAlfModuleTestDataControl::OpenGlobalObjects. ret=%d"), err);
        return err;
        }

    /*
     * Lock access to global memory
     */
    void Lock() {iModuleTestMutex.Wait();}
 
    /*
     * Unlock access to global memory
     */
    void Unlock() {iModuleTestMutex.Signal();}
    
public:
    RChunk iModuleTestChunk;
    RMutex iModuleTestMutex;
    CAlfModuleTestData* iModuleTestData; // Not owned
    };

#endif // USE_MODULE_TEST_HOOKS_FOR_ALF


#endif // ALFMODULETEST_H

// End of File
