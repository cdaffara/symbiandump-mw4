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

#ifndef WLANSETTINGSPLUGIN_H_
#define WLANSETTINGSPLUGIN_H_

// System includes

#include <QObject>
#include <QScopedPointer>
#include <cpplugininterface.h>

// User includes

// Forward declarations

class HbTranslator;

// External data types

// Constants

// Class declaration

class CpWlanSettingsPlugin : public QObject, public CpPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CpPluginInterface)
    
public:

    // Data types

    CpWlanSettingsPlugin();
    
    virtual ~CpWlanSettingsPlugin();

    virtual QList<CpSettingFormItemData*> createSettingFormItemData(
        CpItemDataHelper &itemDataHelper) const;

signals:

public slots:

protected:

protected slots:

private:
    
    Q_DISABLE_COPY(CpWlanSettingsPlugin)
    
private slots:

private: // data

    QScopedPointer<HbTranslator> mTranslator;   //!< Translator for localisation 

    // Friend classes
};

#endif /* WLANSETTINGSPLUGIN_H_ */
