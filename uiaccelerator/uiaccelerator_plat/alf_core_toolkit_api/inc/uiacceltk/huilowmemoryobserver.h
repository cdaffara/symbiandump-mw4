/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/
/*
 * huilowmemoryobserver.h
 *
 *  Created on: Jun 9, 2009
 *      Author: ranshett
 */

#ifndef HUILOWMEMORYOBSERVER_H_
#define HUILOWMEMORYOBSERVER_H_

enum THuiMemoryLevel
    {
    /** 
     * No memory available at all. Even all rendering 
     * surfaces and buffers should be released.
     *  
     **/
    EHuiMemoryLevelNone = -50,

    /** 
     * Lowest memory level. Use SW rendering if possible, 
     * only rendering surfaces etc. absolutely mandatory 
     * objects are allowed to consume memory.
     * 
     **/
    EHuiMemoryLevelLowest = -40,
    
    /** 
     * To be defined.
     *  
     **/
    EHuiMemoryLevelLower = -30,    
    
    /** 
     * Low memory level, same as EnableLowMemoryState(ETrue). Normal
     * functionality and user experience is not provided. Memory usage
     * is minimized, but HW rendering is to be used. Caches, buffers,
     * effects, etc. are to be disabled.
     *  
     **/
    EHuiMemoryLevelLow = -20, 
    
    /** 
     *  Reduced memory level. Normal functionality and user experience
     *  should be provided, but all non-mandatory memory usage should
     *  be avoided. 
     *  
     **/
    EHuiMemoryLevelReduced = -10,    

    /** 
     * Normal memory level, same as EnableLowMemoryState(EFalse) 
     **/
    EHuiMemoryLevelNormal = 0    
    };

/** 
 * Memory level observer interface is same as MHuiLowMemoryObserver
 * but gives more detailed information about memory levels. 
 **/
class MHuiMemoryLevelObserver
    {
public:
    IMPORT_C virtual void SetMemoryLevel(THuiMemoryLevel aLevel)=0;
    };

/** 
 * @deprecated use MHuiMemoryLevelObserver instead.
 **/
class MHuiLowMemoryObserver
    {
public:
    IMPORT_C virtual void EnableLowMemoryState(TBool aEnable)=0;
    };

#endif /* HUILOWMEMORYOBSERVER_H_ */
