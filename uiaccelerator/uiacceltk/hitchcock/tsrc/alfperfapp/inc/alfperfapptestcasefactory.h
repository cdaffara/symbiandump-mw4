/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  alfperfapp test case factory class definition.
*
*/


#ifndef C_ALFPERFAPPTESTCASEFACTORY_H
#define C_ALFPERFAPPTESTCASEFACTORY_H

#include <e32base.h>

class CAlfPerfAppTestCase;

/**
 *  CAlfPerfAppTestCaseFactory class.
 */
class CAlfPerfAppTestCaseFactory : public CBase
    {
public:
    static CAlfPerfAppTestCaseFactory* NewL();
    ~CAlfPerfAppTestCaseFactory();

    /**
     * Creates new test case instance corresponding to the specified id.
     * @param aCaseId Test case id.
     * @param aSequenceIndex Tells which iteartion of the same case this is
     * @return New test case instance. Ownership passed to the client.
     */
    CAlfPerfAppTestCase* CreateL( TInt aCaseId, TInt aSequenceIndex );

private:

    CAlfPerfAppTestCaseFactory();

    };

#endif // C_ALFPERFAPPTESTCASEFACTORY_H

