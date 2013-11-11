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


#ifndef _INC_CSVGDOMIMPLEMENTATIONIMPL_
#define _INC_CSVGDOMIMPLEMENTATIONIMPL_

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "SVGXmlDOMImplementation.h"
#include "SvgBitmapFontProvider.h"

class MXmlDocumentType;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgDOMImplementationImpl : public CBase, public MXmlDOMImplementation
    {
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        MXmlDocument*                       CreateDocumentL( const TDesC& aNamespaceUri,
                                                             const TDesC& aQualifiedName/*, MXmlDocumentType* aDocType*/ );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgDOMImplementationImpl*   NewL(CSvgBitmapFontProvider *aSvgBitmapFontProvider);


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgDOMImplementationImpl*   NewLC(CSvgBitmapFontProvider *aSvgBitmapFontProvider);


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual                             ~CSvgDOMImplementationImpl();

    private:


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                ConstructL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
                                            CSvgDOMImplementationImpl(CSvgBitmapFontProvider *aSvgBitmapFontProvider);
    private:
    		
    	//Stores Font Type information as passed by CSvgEngineInterfaceImpl
        CSvgBitmapFontProvider *iSvgBitmapFontProvider;
    };

#endif /* _INC_CSVGDOMIMPLEMENTATIONIMPL_ */
