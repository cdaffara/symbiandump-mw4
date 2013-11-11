/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
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

#include "wlansettingstest.h"


int main(int /*argc*/, char *argv[])
{
  WlanSettingsTest* test =  new WlanSettingsTest;
 
  //Save test reults to a file
 char *pass[3]; 
 pass[0] = argv[0];
 pass[1] = "-o";    
 pass[2] = "c:\\data\\WlanSettingsTest.txt";
 
 int res = QTest::qExec(test, 3, pass);
 
 delete test;
 return res;
}

#include "moc_wlansettingstest.cpp"
