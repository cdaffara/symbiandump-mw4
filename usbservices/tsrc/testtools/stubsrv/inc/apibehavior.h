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
*
*/

#ifndef APIBEHAVIOR_H
#define APIBEHAVIOR_H

#include <e32base.h>

const TInt KApiOutputSize = 512;

/**
 * The specification of the behavior of an API.
 */
NONSHARABLE_CLASS(TApiBehavior)
{
public:

    /**
     * Constructs by default.
     * Members are initialized to:
     *	iLib: 0
     *	iApi:  0
     *	iCompleteCode:  0.
     *  iAsyncCompleteCode:  0.
     *	iOutput: KNullDes8.
     */
    IMPORT_C TApiBehavior();

    /**
     * Constructs by specified.
     * @param aLib the library identifier
     * @param aApi the API identifier
     *
     * Other members are initialized to:
     *	iCompleteCode:  0.
     *  iAsyncCompleteCode:  0.
     *	iOutput: KNullDes8.
     */
    IMPORT_C TApiBehavior(TUint aLib, TUint aApi);

    /**
     * Construct by specified.
     *
     * @param aLib the library identifier
     * @param aApi the API identifier
     * @param aCompleteCode the expected completion code of the API call
     * @param aAsyncCompleteCode the expected asynchronous completion code of the API call
     * @param aOutput the expected output
     */
    IMPORT_C TApiBehavior(TUint aLib, TUint aApi, 
        TInt aCompleteCode, TInt aAsyncCompleteCode,
        const TDesC8& aOutput);

    /**
     * Compares if the identifiers of this API match with the specified.
     * @param aLib the library identifier
     * @param aApi the APi identifier
     *
     * @return ETrue if the ids match; EFalse otherwise.
     */
    IMPORT_C TBool MatchId(TUint aLib, TUint aApi) const;
    
    /**
     * Compares if the identifiers of this API match with the identifiers of the specified.
     * @param aLib the library identifier
     * @param aApi the APi identifier
     *
     * @return ETrue if the ids match; EFalse otherwise.
     */
    IMPORT_C TBool MatchId(const TApiBehavior& aBeh) const;
    
    /**
     * Operator ==
     *
     * @return ETrue if if the content of this is identical with the specified; EFalse otherwise.
     */
    IMPORT_C TBool operator==(const TApiBehavior& aBeh) const;
    
    /**
     * Operator !=
     *
     * @return ETrue if if the content of this is different with the specified; EFalse otherwise.
     */
    IMPORT_C TBool operator!=(const TApiBehavior& aBeh) const;

public:
    
    /** 
     * The identifier of the library that provides this API
     */
	TUint iLib;
	
    /** 
     * The identifier of this API in the library.
     *
     * iLib and iApi identify a unique API in the scope of testing framework.
     */
	TUint iApi;
	
	/**
	 * The completion code of the API call, either the return error code if 
	 * the function returns one or the leave code if the function is leavable.
	 * This should be ignored by void type APIs.
	 */
	TInt iCompleteCode;
	
    /** 
     * The completion code of asynchronous service provided by this API.
     * This should be ignored by synchronous API.
     */
    TInt iAsyncCompleteCode;
    
    /**
     * The expected output of this API.
     */
	TBuf8<KApiOutputSize> iOutput;
	};

typedef TPckgBuf<TApiBehavior> TApiBehaviorPckgBuf;
typedef TPckg<TApiBehavior> TApiBehaviorPckg;

#endif
            
