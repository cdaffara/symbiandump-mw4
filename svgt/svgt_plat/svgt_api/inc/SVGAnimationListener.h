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


#ifndef __INC_MSVGANIMATIONLISTENER__
#define __INC_MSVGANIMATIONLISTENER__


/**
 * This interface is the callback mechanism for the SVG Engine to communicate to
 * interested clients about hyperlinks.  The client (listener) is notified when
 * a pointer has entered or exited a hyperlink and when a hyperlink is activated.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgAnimationListener
    {
    public:

        /**
         * Notified when a pointer enters a hyperlink element.
         *
         * @since 1.0
         * @param : aUri -- URI string of hyperlink
         * @return : For future use.  Value is ignored.
         */
        virtual TBool AnimationStarted(TBool isAnimationIndefinite) = 0;

        /**
         * Notified when a pointer exits a hyperlink element.
         *
         * @since 1.0
         * @param : aUri -- URI string of hyperlink
         * @return : For future use.  Value is ignored.
         */
        virtual TBool AnimationPaused() = 0;

        /**
         * This method is called to notify the client that a link has been
         * activated.
         *
         * @since 1.0
         * @param : aUri -- URI string of hyperlink
         * @return : For future use.  Value is ignored.
         */
        virtual TBool AnimationEnded() = 0;

    };

#endif /*__INC_MSVGHYPERLINKLISTENER__*/
