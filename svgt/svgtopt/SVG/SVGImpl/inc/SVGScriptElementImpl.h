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
* Description:  SVG Implementation header file
 *
*/


#ifndef CSVGSCRIPTELEMENTIMPL_H
#define CSVGSCRIPTELEMENTIMPL_H

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgScriptElementImpl : public CSvgElementImpl
    {
    public: // Constructor/deconstructor


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgScriptElementImpl* NewL( TUint8 aElementId, CSvgDocumentImpl* aDocument );

    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                         ~CSvgScriptElementImpl();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                            SetAttributeL( const TDesC& aName, const TDesC& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetScriptL( const TDesC& aScriptString );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
         const HBufC8*                  GetScript();

		 void Print( TBool aIsEncodeOn );

    private:

        /**
         * Constructor
         *
         * @since 1.0
         * @param
         * @return
         */
        CSvgScriptElementImpl( CSvgDocumentImpl* aDocument );

        /**
         * Constructor
         *
         * @since 1.0
         * @param
         * @return
         */
        void ConstructL( TUint8 aElementId );

        void FetchExternalScriptL();

    private:
        CSvgDocumentImpl*    iDocument;
        HBufC8*              iScript;
        HBufC*               iXLink;


    };

#endif
