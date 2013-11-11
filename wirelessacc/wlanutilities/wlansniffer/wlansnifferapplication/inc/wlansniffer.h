/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* WLAN Sniffer application main class. 
*/

#ifndef WLANSNIFFER_H
#define WLANSNIFFER_H

// System includes

#include <QSharedPointer>
#include <HbApplication>

// User includes

// Forward declarations

class WlanSnifferEngine;
class HbTranslator;
class WlanSnifferMainWindow;

// External data types

// Constants

// Class declaration

class WlanSniffer : public HbApplication
{
    Q_OBJECT

public:

    // Data types

    WlanSniffer(int argc, char* argv[]);

    ~WlanSniffer();

signals:

public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(WlanSniffer)

    bool findKeepaliveProcess();

private slots:

    void exitApplication();

private: // data

    // Not owned data

    // Owned data
    WlanSnifferEngine *mEngine;                 //!< Sniffer engine
    QSharedPointer<HbTranslator> mTranslator;   //!< Translator for localisation 
    QSharedPointer<WlanSnifferMainWindow> mMainWindow;  //!< Main window instance

    // Friend classes
};

#endif // WLANSNIFFER_H
