/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#ifndef WIDGETBACKUPRESTORE_H
#define WIDGETBACKUPRESTORE_H

#include <e32base.h>
#ifdef _DEBUG
#include <flogger.h>
#endif

class CWidgetActiveCallback;

class CWidgetBackupRestore : public CActive
	{
public:

	static CWidgetBackupRestore* NewL();
	virtual ~CWidgetBackupRestore();

// from base class CActive
	void RunL();
    TInt RunError( TInt aError );
	void DoCancel();

private:

    CWidgetBackupRestore();
    void ConstructL();

    // Handles changes in backup state.
	void HandleBackupStateL( const TInt aValue );

    // Subsribes notifications of backup/restore p&s key.
	void SubscribePSKey();

private: // data

    /**
     * Callback interface for widget's active data owner.
     * Own.
     */
    CWidgetActiveCallback* iCallBack;

    /**
     * Active backup client.
     * Own.
     */
	conn::CActiveBackupClient* iActiveBackupClient;

    /**
     * Property to listen to.
     */
    RProperty iBackupProperty;

    /**
     * Last state of BUR: either backup or restore or normal or unset
     */
    TInt iLastType;

#ifdef _DEBUG
    RFileLogger             iFileLogger;
    TBool                   iCanLog;
#endif
	};


#endif
