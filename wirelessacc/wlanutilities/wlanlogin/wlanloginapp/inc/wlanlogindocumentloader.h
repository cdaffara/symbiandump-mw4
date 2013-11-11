/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Document loader of WLAN Login application
*
*/

#ifndef WLANLOGINDOCUMENTLOADER_H
#define WLANLOGINDOCUMENTLOADER_H

// System includes
#include <QObject>
#include <HbDocumentLoader>

// User includes

// Forward declarations
class HbMainWindow;
class QString;

// External data types

// Constants

// Class declaration
class WlanLoginDocumentLoader : public HbDocumentLoader
{
public:
    WlanLoginDocumentLoader(const HbMainWindow *window);
    ~WlanLoginDocumentLoader();
protected:
    QObject *createObject(const QString& type, const QString &name);

};

#endif // WLANLOGINDOCUMENTLOADER_H
