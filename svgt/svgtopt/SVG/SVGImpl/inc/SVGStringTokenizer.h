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
* Description:  SVG Implementation header file
 *
*/


#ifndef __TSTRINGTOKENIZER_H__
#define __TSTRINGTOKENIZER_H__

#include <e32std.h>


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class TStringTokenizer
    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
                TStringTokenizer( const TDesC& aSrc, const TDesC& aDelim );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TPtrC   NextToken();

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TPtrC   NextToken( const TDesC& aDelim );

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TBool   HasMoreTokens();

    private:
        TLex    iSrc;
        TPtrC   iDelim;

    private:

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TBool   isDeliminator( TChar aChar, const TDesC& aDelim );

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void    SkipDeliminator( const TDesC& aDelim );
    };

#endif // __TStringTokenizer_H__
