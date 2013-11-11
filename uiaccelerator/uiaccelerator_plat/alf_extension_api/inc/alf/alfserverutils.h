/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Application server
*
*/



#ifndef C_ALFAPPSERVERUTILS_H
#define C_ALFAPPSERVERUTILS_H

#include <e32def.h>
#include <alf/alftimedvalue.h>

class TAlfTimedValue;
class THuiTimedValue;
class TAlfTimedPoint;
class THuiTimedPoint;
class MAlfInterfaceProvider;
class RFile;

struct TAlfBoxMetric;
struct THuiBoxMetric;
struct TAlfXYMetric;
struct THuiXYMetric;
struct TAlfMetric;
struct THuiMetric;


class AlfTimedValueUtility
    {
public:
    /** 
     *
     * Helper function to copy TAlfTimedValue (including mapping function) into THuiTimedValue. 
     * Can be used only inside the Alfred-server.
     *
     * @param aSrc Source ALF timedvalue.
     * @param aDst Destination HUI timedvalue.
     * @param aResolver Interface to be used for resolving MHuiMappingFunction from TAlfTimedValue 
     *     MappingFunctionIdentifier
     *
     */
    IMPORT_C static void CopyTimedValue(TAlfTimedValue& aSrc, THuiTimedValue& aDst, MAlfInterfaceProvider& aResolver);

    /** 
     *
     * Helper function to copy TAlfTimedValue (NOT including mapping function) into THuiTimedValue. 
     * Can be used only inside the Alfred-server.
     *
     * @param aSrc Source ALF timedvalue.
     * @param aDst Destination HUI timedvalue.
     *
     */
    IMPORT_C static void CopyTimedValue(TAlfTimedValue& aSrc, THuiTimedValue& aDst);

    /** 
     *
     * Helper function to copy THuiTimedValue (NOT including mapping function) into TAlfTimedValue. 
     * Can be used only inside the Alfred-server.
     *
     * @param aSrc Source HUI timedvalue.
     * @param aSrc Destination ALF timedvalue.
     *
     */
    IMPORT_C static void CopyTimedValue(THuiTimedValue& aSrc, TAlfTimedValue& aDst);
    };

class AlfTimedPointUtility
    {
public:
     
    /** 
     *
     * Helper function to copy TAlfTimedPoint (including mapping function) into THuiTimedPoint. 
     * Can be used only inside the Alfred-server.
     *
     * @param aSrc Source ALF timedpoint.
     * @param aDst Destination HUI timedpoint.
     * @param aResolver Interface to be used for resolving MHuiMappingFunction from TAlfTimedPoint 
     *     MappingFunctionIdentifier
     *
     */
    IMPORT_C static void CopyTimedPoint(TAlfTimedPoint& aSrc, THuiTimedPoint& aDst, MAlfInterfaceProvider& aResolver);
    
    /** 
     *
     * Helper function to copy TAlfTimedPoint (NOT including mapping function) into THuiTimedPoint. 
     * Can be used only inside the Alfred-server.
     *
     * @param aSrc Source ALF timedpoint.
     * @param aDst Destination HUI timedpoint.
     *
     */
    IMPORT_C static void CopyTimedPoint(TAlfTimedPoint& aSrc, THuiTimedPoint& aDst);

    /** 
     *
     * Helper function to copy THuiTimedPoint (NOT including mapping functions) into TAlfTimedPoint. 
     * Can be used only inside the Alfred-server.
     *
     * @param aDst Source HUI timedpoint.
     * @param aDst Destination ALF timedpoint.
     *
     */
    IMPORT_C static void CopyTimedPoint(THuiTimedPoint& aSrc, TAlfTimedPoint& aDst);

    };

class AlfFileTokenUtility
    {
    public:
    /** 
     *
     * Helper function to open a file based on token received from client side
     * As token remains valid as long as the client session is open, there's no need to keep file handle
     * open unless file is really used
     *
     * @param aFile Handle to open
     * @param aToken Token received from client
     *
     */
    IMPORT_C static void OpenFileForTokenL(RFile& aFile, const TDesC8& aToken);
    };



class AlfBoxMetricUtility
    {
public:
    static void CopyMetric(TAlfBoxMetric& aSrc, THuiBoxMetric& aDst);

    static void CopyMetric(THuiBoxMetric& aSrc, TAlfBoxMetric& aDst);
    };

class AlfXYMetricUtility
    {
public:
    static void CopyMetric(TAlfXYMetric& aSrc, THuiXYMetric& aDst);

    static void CopyMetric(THuiXYMetric& aSrc, TAlfXYMetric& aDst);
    };

class AlfMetricUtility
    {
public:
    static void CopyMetric(TAlfMetric& aSrc, THuiMetric& aDst);

    static void CopyMetric(THuiMetric& aSrc, TAlfMetric& aDst);
    };


#endif
