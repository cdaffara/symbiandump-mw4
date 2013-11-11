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

#ifndef FIRSTVIEW_H_
#define FIRSTVIEW_H_

#include <HbView.h>

class HbComboBox;
class HbLineEdit;
class HbCheckBox;
class WlanWizard;
class HbLabel;

/*!
 * @addtogroup group_test_eap_wizard
 * @{
 */

class FirstView : public HbView
{
Q_OBJECT

public:
    FirstView();
    virtual ~FirstView();
    
    
private slots:
    void start(bool);
    void finished(int iapId, bool connected);
    void cancelled();

private:
    void deleteWizard();
    
private:
    WlanWizard *mWizard;
    
    HbCheckBox *mUseConf;
    HbLineEdit *mSsid;
    HbComboBox *mNetworkMode;
    HbComboBox *mSecurityMode;
    HbCheckBox *mHidden;
    HbCheckBox *mUseWpaPsk;
    HbCheckBox *mUseWps;
    HbLabel *mStatus;
    
};

/*! @} */

#endif /* FIRSTVIEW_H_ */

