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


#ifndef __INC_CSVGURIREFERENCEIMPL__
#define __INC_CSVGURIREFERENCEIMPL__

#include <e32base.h>
#include <badesca.h>

#include "SVGUriReference.h"

/// Implements Xlink interface. Elements implementing this interface should
/// provide wrappers. Note that not all SVG elements implement this interface
/// in SVG DOM.


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgUriReferenceImpl : public CBase, public MSvgUriReference
    {
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgUriReferenceImpl*    NewL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgUriReferenceImpl*    NewLC();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
                                        ~CSvgUriReferenceImpl();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        const TDesC&                    XlinkActuate();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        const TDesC&                    XlinkArcrole();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        const TDesC&                    XlinkRole();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        const TDesC&                    XlinkShow();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        const TDesC&                    XlinkTitle();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        const TDesC&                    XlinkType();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        const TDesC&                    XlinkHref();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        const TDesC&                    Href(); // for the SVG DOM

				TDesC*										HrefPointer();
				
        // Implementation


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TBool                           SetXlinkAttributeL( const TDesC& aPropertyName,
                                                            const TDesC& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        CSvgUriReferenceImpl* CloneL();

		TInt                          SetUriRefDesL( const TUint16 aPropertyName,
                                                const TDesC& aValue );
    

    private:


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                            ConstructL();

        HBufC*                          iXlinkActuate;
        HBufC*                          iXlinkArcrole;
        HBufC*                          iXlinkRole;
        HBufC*                          iXlinkShow;
        HBufC*                          iXlinkTitle;
        HBufC*                          iXlinkType;
        HBufC*                          iXlinkHref;
    };

#endif /* __INC_CSVGURIReferenceImpl__ */