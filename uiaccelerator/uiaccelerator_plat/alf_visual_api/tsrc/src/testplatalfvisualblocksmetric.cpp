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
* Description:  test functions for alfmetric.h
*
*/



// [INCLUDE FILES]
#include <alf/alfmetric.h>

#include "testplatalfvisual.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestMetricMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestMetricMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestMetricMethods, "TestMetricMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestMetricMethods );
    // Print to log file
    iLog->Log( KTestMetricMethods );
    
    TAlfMetric metric;
    
    TAlfMetric metric2( 1 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestXYMetricMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestXYMetricMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestXYMetricMethods, "TestXYMetricMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestXYMetricMethods );
    // Print to log file
    iLog->Log( KTestXYMetricMethods );
    
    TAlfXYMetric xyMetric;
    
    TAlfMetric metricX;
    TAlfXYMetric xyMetric2( metricX );
    
    TAlfMetric metricY;
    TAlfXYMetric xyMetric3( metricX, metricY );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBoxMetricMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestBoxMetricMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBoxMetricMethods, "TestBoxMetricMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBoxMetricMethods );
    // Print to log file
    iLog->Log( KTestBoxMetricMethods );
    
    TAlfBoxMetric boxMetric;
    
    TAlfMetric metric;
    TAlfBoxMetric boxMetric2( metric );
    
    TAlfXYMetric xyMetric;
    TAlfBoxMetric boxMetric3( xyMetric );
    
    TAlfXYMetric topLeft, bottomRight;
    TAlfBoxMetric boxMetric4( topLeft, bottomRight ); 
    
    TAlfMetric left, right, top, bottom;
    TAlfBoxMetric boxMetric5( left, right, top, bottom );
    
    return KErrNone;
    }


//  [End of File]
