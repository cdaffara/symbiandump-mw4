/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Transition server repository handler.
*
*/


#ifndef C_REPOSITORY_HANDLER_H
#define C_REPOSITORY_HANDLER_H

#include <e32base.h>

class CRepository;

NONSHARABLE_CLASS(CRepositoryHandler) : public CActive
    {
    CRepositoryHandler(TInt aPriority, const TUid& aRepository, TInt aKey);
    void ConstructL();
public:    

    static CRepositoryHandler* NewL(TInt aPriority, const TUid& aRepository, TUint32 aKey);
    ~CRepositoryHandler();
    TInt GetValue(TInt& aValue);
    TInt SetValue(TInt aValue);
private:
	/**
     * From CActive
     * @since S60 3.2
     *
    */  
    void RunL();

	/**
     * From CActive
     * @since S60 3.2
     *
    */  
    void DoCancel();

private: //data
	CRepository* iCrep;

    TUid iRepository;
    TInt iKey;
    TInt iValue;
    TInt iError;
    };

#endif //C_REPOSITORY_HANDLER_H