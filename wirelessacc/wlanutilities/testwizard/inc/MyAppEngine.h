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
 * Description: 
 *   Test Application for wizards
 *
 */

#ifndef MYAPPENGINE_H_
#define MYAPPENGINE_H_

#include <QObject>

/*!
 * @addtogroup group_test_eap_wizard
 * @{
 */

class MyAppEngine : public QObject
    {
    Q_OBJECT

public:
    MyAppEngine(QObject *parent = 0);
    virtual ~MyAppEngine();
    
signals:
    void toFirstView();
    void toSecondView();
    
    };

/*! @} */

#endif /* MYAPPENGINE_H_ */
