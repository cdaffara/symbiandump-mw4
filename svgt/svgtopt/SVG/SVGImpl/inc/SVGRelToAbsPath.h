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


#ifndef __TSVGRELTOABSPATH_H__
#define __TSVGRELTOABSPATH_H__

#include <e32std.h>


/**
 * Class description
 *
 *  Converts relative paths to absolute paths
 *  
 */
class TSvgRelToAbsPath
    {
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static TInt GetFullPath( TParse& aParse,
                                 const TText8* upath,
                                 RFs& aSession,
                                 TDes* aFileName );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static TInt GetFullFile( TFileName& aName,
                                 const TText8* upath,
                                 RFs& aSession );
    };

#endif // __TStringTokenizer_H__
