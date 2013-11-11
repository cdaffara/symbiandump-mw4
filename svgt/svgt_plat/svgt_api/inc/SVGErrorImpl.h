/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Engine header file
 *
*/


#ifndef SVGERROR_H
#define SVGERROR_H

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "SVGEngineInterfaceImpl.h"

/**
 * This class implements the interface for a MSvgError object, which contains
 * an error code and a description of the error if one exists.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgErrorImpl : public CBase, public MSvgError
    {
    public:

        /**
         * Two-phase constructor.  The default error code is ESvgNoError and description is
         * "No Error"
         *
         * @since 1.0
         * @return : CSvgErrorImpl object
         */
        static CSvgErrorImpl* NewL();

        /**
         * Two-phase constructor.  The default error code is ESvgNoError and description is
         * "No Error"
         *
         * @since 1.0
         * @return : CSvgErrorImpl object
         */
        static CSvgErrorImpl* NewLC();

        /**
         * Destructor
         */
         virtual ~CSvgErrorImpl();

        /**
         * Determine wether an error is indicated by this object.
         *
         * @since 1.0
         * @return : ETrue, if ErrorCode() != ESvgNoError
         */
        TBool HasError();

        /**
         * Determine wether an error is only a warning.
         * This should be a state when the svg may be display, despite
         * a conflict.
         *
         * @since 1.0
         * @return ETrue if HasError() is only a warning.
         */
        TBool IsWarning();

        /**
         * Set the flag to indicate the error indicated by this object
         * is only a warning.
         *
         * @since 1.0
         * @return ETrue if HasError() is only a warning.
         */

        void SetIsWarning( TBool aBool );

        /**
         * Get the error code contained by this object.
         *
         * @since 1.0
         * @return : the TSvgErrorCode value
         */
        TSvgErrorCode ErrorCode();

        /**
         * Get the error code contained by this object.
         *
         * @since 1.0
         * @return : the TSvgErrorCode value
         */
        TInt SystemErrorCode();

        /**
         * Get the error description string for this object.
         *
         * @since 1.0
         * @return : error description string.
         */
        TDesC& Description();

        /**
         * Set the error code.
         *
         * @since 1.0
         * @param aError : error code
         * @return
         */
        void SetErrorCode( TSvgErrorCode aError );

        /**
         * Set the error code.
         *
         * @since 1.0
         * @param aError : error code
         * @return
         */
        void SetSystemErrorCode( TInt aError );

        /**
         * Set the error description.
         *
         * @since 1.0
         * @param aDescription : error description string
         * @return
         */
        void SetDescription( const TDesC& aDescription );

        /**
         * Set the error description.
         *
         * @since 1.0
         * @param aText1 : error description string 1
         * @param aText2 : error description string 2
         * @return
         */
        void SetDescription( const TDesC& aText1, const TDesC& aText2 );

        /**
         * Append the given text to the existing description .
         *
         * @since 1.0
         * @param aDescription : error description string
         * @param aText2 : error description string 2
         * @return
         */
        void AppendDescription( const TDesC& aDescription );


    protected:
        /**
         * Second phase of constructor
         *
         * @since 1.0
         */
        void ConstructL();

    private:

        /**
         * Constructor.  The default error code is ESvgNoError and description is
         * "No Error"
         *
         * @since 1.0
         */
        CSvgErrorImpl();

    private:
        TSvgErrorCode iErrorCode;
        HBufC*        iDescription;
        TBool         iIsWarning;
        TInt          iSystemErrorCode;
    };

#endif

