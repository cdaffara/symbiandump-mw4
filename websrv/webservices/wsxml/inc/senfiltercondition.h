/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/









#ifndef C_SEN_FILTER_CONDITION_H
#define C_SEN_FILTER_CONDITION_H

// INCLUDES

#include <e32base.h>
#include <flogger.h>

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DEFINITIONS

/**
 * @author lewontin
 *
 */
class CSenFilterCondition : public CBase
{

public: // Constructors and Destructors

    IMPORT_C static CSenFilterCondition* NewL(TInt aIndex, 
                                              const TDesC8& aElement, 
                                              const TDesC8& aAttribute);


    CSenFilterCondition(const CSenFilterCondition& aFilterCondition);


    IMPORT_C ~CSenFilterCondition();


    IMPORT_C TBool Test();


protected:

    void ConstructL(TInt aIndex, 
                    const TDesC8& aElement, 
                    const TDesC8& aAttribute);


    CSenFilterCondition();


private:
    
    /**
     * File logger.
     *
     */
    RFileLogger* Log() const;


private:

    TInt iIndex;
    TInt iCurrentIndex;
    HBufC8* ipElement;
    HBufC8* ipAttribute;

    RFileLogger iLog;

};

#endif // C_SEN_FILTER_CONDITION_H

