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
 * This is the source file for testing Wlan Wizard library.
 */

// System includes
#include <QtCore>
#include <QTest>

// User includes
#include "testwlanwizard.h"
#include "wlanwizardutils.h"

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// FRAMEWORK FUNCTIONS
// ---------------------------------------------------------

/**
 * This function will be called before the first test function is executed.
 */
void TestWlanWizard::initTestCase()
{

}

/**
 * This function will be called after the last test function was executed.
 */
void TestWlanWizard::cleanupTestCase()
{
}

/**
 * This function will be called before each test function is executed.
 */
void TestWlanWizard::init()
{

}

/**
 * This function will be called after each test function is executed.
 */
void TestWlanWizard::cleanup()
{

}

// ---------------------------------------------------------
// TEST CASES
// ---------------------------------------------------------


void TestWlanWizard::testWepHex64Valid()
{
    QString key("a0cd9fACDF");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusOk);
}

void TestWlanWizard::testWepHex128Valid()
{
    QString key("ABCDEFabcdef0123456789AAAA");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusOk);
}

void TestWlanWizard::testWepHexInvalidCharacters()
{
    QString key("wlanwizard");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);
}

void TestWlanWizard::testWepAscii64Valid()
{
    QString key("testi");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusOk);
}

void TestWlanWizard::testWepAscii128Valid()
{
    QString key("wlanwizardjee");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusOk);
}

void TestWlanWizard::testWepAsciiInvalidCharacters()
{
    QString key("wlanwizardje");
    key.append(QChar(31));
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);
}

void TestWlanWizard::testWepLength9Invalid()
{
    QString key("123456789");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusWepInvalidLength);
}

void TestWlanWizard::testWepLength11Invalid()
{
    QString key("12345678901");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusWepInvalidLength);
}

void TestWlanWizard::testWepLength25Invalid()
{
    QString key("1234567890123456789012345");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusWepInvalidLength);
}

void TestWlanWizard::testWepLength27Invalid()
{
    QString key("123456789012345678901234567");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusWepInvalidLength);
}

void TestWlanWizard::testWepLength4Invalid()
{
    QString key("1234");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusWepInvalidLength);
}

void TestWlanWizard::testWepLength6Invalid()
{
    QString key("123456");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusWepInvalidLength);
}

void TestWlanWizard::testWepLength12Invalid()
{
    QString key("123456789012");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusWepInvalidLength);
}

void TestWlanWizard::testWepLength14Invalid()
{
    QString key("12345678901234");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWepKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusWepInvalidLength);
}

// ----------------------------------------------------------------------------
// WPA test cases
// ----------------------------------------------------------------------------
void TestWlanWizard::testWpaHexValid()
{
    QString key("1234567890123456789012345678abcdefabcdefabcdefABCDEFABCDEFABCDEF");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWpaKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusOk);
}

void TestWlanWizard::testWpaHexInvalidCharacters()
{
    QString key("abcdefabcdefabcdefABCDEFABCDEFABCDEF123456789012345678901234567G");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWpaKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);
}
void TestWlanWizard::testWpaAsciiLength8Valid()
{
    QString key("ictsucks");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWpaKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusOk);
}

void TestWlanWizard::testWpaAsciiLength63Valid()
{
    QString key("zxcvbnm,.-asdfghjkl'qwertyuiop1234567890qwertyuiopzxcvbnm,.-123");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWpaKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusOk);
}

void TestWlanWizard::testWpaAsciiLength64Invalid()
{
    QString key("zxcvbnm,.-asdfghjkl'qwertyuiop1234567890qwertyuiopzxcvbnm,.-1234");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWpaKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);  
}

void TestWlanWizard::testWpaAsciiInvalidCharacters()
{
    QString key("wizardrules");
    key.append(QChar(127));
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWpaKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);
}

void TestWlanWizard::testWpaLength7Invalid()
{
    QString key("wizards");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWpaKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusWpaTooShort);
}

void TestWlanWizard::testWpaLength65Invalid()
{
    QString key("zxcvbnm,.-asdfghjkl'qwertyuiop1234567890qwertyuiopzxcvbnm,.-12345");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::validateWpaKey(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusWpaTooLong);  
}


// ----------------------------------------------------------------------------
// ASCII test cases
// ----------------------------------------------------------------------------
void TestWlanWizard::testAsciiValidCharacters()
{
    QString key;
    for (int i = 32 ; i <= 126 ; i++){
        key.append(QChar(i));
    }
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::isAscii(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusOk);
}

void TestWlanWizard::testAsciiInvalidCharactersLower()
{
    QString key("wizardrules");
    key.append(QChar(31));
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::isAscii(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);
}

void TestWlanWizard::testAsciiInvalidCharactersUpper()
{
    QString key("wizardrules");
    key.append(QChar(127));
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::isAscii(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);
}


// ----------------------------------------------------------------------------
// HEX test cases
// ----------------------------------------------------------------------------
void TestWlanWizard::testHexValidCharacters()
{
    QString key("abcdefABCDEF1234567890");
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::isHex(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusOk);   
}

void TestWlanWizard::testHexInvalidCharactersDigitLower()
{
    QString key("afAF09");
    key.append(QChar(47));
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::isHex(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);   
}

void TestWlanWizard::testHexInvalidCharactersDigitUpper()
{
    QString key("abc");
    key.append(QChar(58));
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::isHex(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);
}

void TestWlanWizard::testHexInvalidCharacters_a_Lower()
{
    QString key("abc");
    key.append(QChar(96));
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::isHex(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);
}

void TestWlanWizard::testHexInvalidCharacters_f_Upper()
{
    QString key("abc");
    key.append(QChar(103));
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::isHex(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);
}

void TestWlanWizard::testHexInvalidCharacters_A_Lower()
{
    QString key("abc");
    key.append(QChar(64));
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::isHex(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);
}

void TestWlanWizard::testHexInvalidCharacters_F_Upper()
{
    QString key("abc");
    key.append(QChar(71));
    WlanWizardUtils::KeyStatus status = WlanWizardUtils::isHex(key);
    QCOMPARE(status, WlanWizardUtils::KeyStatusIllegalCharacters);
}

void TestWlanWizard::testSsidValidCharacters()
{
    QString key("12345678901234567890123456789012");
    WlanWizardUtils::SsidStatus status = WlanWizardUtils::validateSsid(key);
    QCOMPARE(status, WlanWizardUtils::SsidStatusOk);
}

void TestWlanWizard::testSsidValidCharacters2()
{
    QString key("1");
    WlanWizardUtils::SsidStatus status = WlanWizardUtils::validateSsid(key);
    QCOMPARE(status, WlanWizardUtils::SsidStatusOk);
}

void TestWlanWizard::testSsidInvalidTooShort()
{
    QString key("");
    WlanWizardUtils::SsidStatus status = WlanWizardUtils::validateSsid(key);
    QCOMPARE(status, WlanWizardUtils::SsidStatusInvalidLength);
}

void TestWlanWizard::testSsidInvalidTooLong()
{
    QString key("123456789012345678901234567890123");
    WlanWizardUtils::SsidStatus status = WlanWizardUtils::validateSsid(key);
    QCOMPARE(status, WlanWizardUtils::SsidStatusInvalidLength);
}
