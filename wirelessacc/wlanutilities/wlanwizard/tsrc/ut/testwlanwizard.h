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
* This is the header file for testing Wlan Qt Utilities library.
*/

#ifndef TESTWLANWIZARD_H_
#define TESTWLANWIZARD_H_

// System includes
#include <QObject>

// User includes

// Forward declarations
class QSignalSpy;
class WlanQtUtils;

// External data types

// Constants

class TestWlanWizard: public QObject
{
    Q_OBJECT
    
private slots:

    // Functions from QTest framework.
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // WEP test cases
    void testWepHex64Valid();
    void testWepHex128Valid();
    void testWepHexInvalidCharacters();

    void testWepAscii64Valid();
    void testWepAscii128Valid();
    void testWepAsciiInvalidCharacters();
    void testWepLength9Invalid();
    void testWepLength11Invalid();
    void testWepLength25Invalid();
    void testWepLength27Invalid();
    void testWepLength4Invalid();
    void testWepLength6Invalid();
    void testWepLength12Invalid();
    void testWepLength14Invalid();    
    
    // WPA test cases
    void testWpaHexValid();
    void testWpaHexInvalidCharacters();
    void testWpaAsciiLength8Valid();
    void testWpaAsciiLength63Valid();
    void testWpaAsciiLength64Invalid();
    void testWpaAsciiInvalidCharacters();
    void testWpaLength7Invalid();
    void testWpaLength65Invalid();
    
    // ASCII test cases
    void testAsciiValidCharacters();
    void testAsciiInvalidCharactersLower();
    void testAsciiInvalidCharactersUpper();
    
    // HEX test cases
    void testHexValidCharacters();
    void testHexInvalidCharactersDigitLower();
    void testHexInvalidCharactersDigitUpper();
    void testHexInvalidCharacters_a_Lower();
    void testHexInvalidCharacters_f_Upper();
    void testHexInvalidCharacters_A_Lower();
    void testHexInvalidCharacters_F_Upper();
    
    // SSID test cases
    void testSsidValidCharacters();
    void testSsidValidCharacters2();
    void testSsidInvalidTooShort();
    void testSsidInvalidTooLong();
    
private:
 
};

#endif /* TESTWLANWIZARD_H_ */

