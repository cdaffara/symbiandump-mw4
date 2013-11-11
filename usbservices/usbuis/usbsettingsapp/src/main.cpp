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
#include <QTranslator>
#include <QString>
#include <QLocale>
#include "usbsettingsapp.h"
#include <hbapplication.h>

int main(int argc, char *argv[])
{
    HbApplication usbApp(argc, argv);
    QTranslator translator;
    
     // add translator for application library
    QString locale = QLocale::system().name();
    QString filename = QString("usbview_") + locale; 
    bool success = translator.load( filename, QString("/resource/qt/translations") ); 
    if (success)
        {
        usbApp.installTranslator(&translator);
        }
  
    USBSettingsApp settingsapp;
    settingsapp.show();

    return usbApp.exec();
    
}
