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
* WLAN Entry plugin implementation.
*/
 
#ifndef	CPWLANENTRYPLUGIN_H
#define	CPWLANENTRYPLUGIN_H

// System includes

#include <QObject>
#include <QSharedPointer>
#include <cpplugininterface.h>

// User includes

// Forward declarations

class HbTranslator;

// External data types

// Constants

// Class declaration

class CpWlanEntryPlugin : public QObject, public CpPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CpPluginInterface)
    
public:

    // Data types

    CpWlanEntryPlugin();
    
    virtual ~CpWlanEntryPlugin();
    
    virtual QList<CpSettingFormItemData *> createSettingFormItemData(
        CpItemDataHelper &itemDataHelper) const;

signals:

public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(CpWlanEntryPlugin)

private slots:

private: // data

    //! WLAN dynamic ON/OFF configuration
    bool mWlanSupported;
    //! Translator for text ID's used in this plugin
    QSharedPointer<HbTranslator> mTranslator;

    // Friend classes
};

#endif	// CPWLANENTRYPLUGIN_H
