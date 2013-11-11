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


#ifndef __INC_MSVGHYPERLINKLISTENER__
#define __INC_MSVGHYPERLINKLISTENER__


/**
 * This interface is the callback mechanism for the SVG Engine to communicate to
 * interested clients about hyperlinks.  The client (listener) is notified when
 * a pointer has entered or exited a hyperlink and when a hyperlink is activated.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgHyperlinkListener
    {
    public:

        /**
         * Notified when a pointer enters a hyperlink element.
         *
         * @since 1.0
         * @param : aUri -- URI string of hyperlink
         * @return : For future use.  Value is ignored.
         */
        virtual TBool LinkEntered( const TDesC& aUri ) = 0;

        /**
         * Notified when a pointer exits a hyperlink element.
         *
         * @since 1.0
         * @param : aUri -- URI string of hyperlink
         * @return : For future use.  Value is ignored.
         */
        virtual TBool LinkExited( const TDesC& aUri ) = 0;

        /**
         * This method is called to notify the client that a link has been
         * activated.
         *
         * @since 1.0
         * @param : aUri -- URI string of hyperlink
         * @return : For future use.  Value is ignored.
         */
        virtual TBool LinkActivated( const TDesC& aUri ) = 0;

         /**
         * This method is called to notify the client that a link has been
         * activated.  When Show is also present .
         *
         * @since 1.0
         * @param : aUri -- URI string of hyperlink
         * @param : aShow -- xlink:show value
         * @return : For future use.  Value is ignored.
         */
        virtual TBool LinkActivatedWithShow( const TDesC& aUri, const TDesC& aShow ) = 0;

    };

#endif /*__INC_MSVGHYPERLINKLISTENER__*/
