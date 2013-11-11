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


#ifndef __INC_MSVGREQUESTOBSERVER__
#define __INC_MSVGREQUESTOBSERVER__


class   CSvgElementImpl;


/**
 * This interface is the callback mechanism for the SVG Engine to communicate to
 * interested clients and to request for external data, such as images.
 * The callbacks will be active when the methods of this interace are implemented and
 * the pointer to the implementation object is passed in as the second parameter
 * of the 'ConstructL' method of the CSvgEngineInterface.  A 'null' value may be
 * passed-in if the callbacks are to be ignored.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgRequestObserver
    {
    public:

        /**
         * This method is called by the SVG Engine when all data for a certain
         * step, such as animation frame, have been updated.  This usually means
         * the off-screen image (passed in to 'ConstructL' of CSvgEngineInterface)
         * has been updated.  The client usually redraws the image onto
         * the screen.
         *
         * @since 1.0
         * @param : none
         * @return : none
         */
        virtual void    UpdateScreen() = 0;

        /**
         * This method is for future extension, in which an external script engine
         * could be used to evaluate a script description.
         *
         * @since 1.0
         * @param : aScript -- A description of script from the "on" attribute.
         *
         * @param : aCallerElement -- The element that calls the script by a
         *                            an event of the "on" attribute.
         * @return : For future use.  Value is ignored.
         */
        virtual TBool   ScriptCall( const TDesC& aScript,
                                    CSvgElementImpl* aCallerElement ) = 0;

        /**
         * This method is called to retrieve the file handle of an image name.
         * The session (RFs) object passed in is guaranteed to be connected, so
         * the client must NOT call aSession.Connect().
         *
         * The implementation for the client of this method is usually if not always:
         * TInt X::FetchImage( const TDesC& aFilename, RFs& aSession, RFile& aFileHandle )
         *     {
         *     // Check for "http", get data from remote site, save to disk
         *     // Get absolute path to aFilename: absolutepath
         *     return aFileHandle.Open( aSession, absolutepath, EFileRead );
         *     }
         *
         * Note: Use EFileShareReadersOnly to prevent file locking.
         *
         * The API AssignImageData() can be used to avoid block the parsing
         * thread due this method taking too long to return.  This is
         * accomplished by returning a non-KErrNone value  (i.e. KErrNotFound),
         * and storing the URI parameter.  The image data then could be
         * assigned the the URI in another thread/Active-Object.
         *
         * @since 1.0
         * @param : aUri -- the relative path of an image file.
         * @param : aSession -- File session used by client to open file.
         * @param : aFileHandle -- File handle used by client to open file.
         * @return : KErrNone if Open command successful, otherwise error-code
         *           from Open command.  The SVG engine will proceed loading the file
         *           only if the return value is KErrNone.
         */
        virtual TInt FetchImage( const TDesC& aUri, RFs& aSession, RFile& aFileHandle ) = 0;

		/**
         * This method is called to retrieve the file handle of an svg font file.
         * This file must be SVG 1.1 .svg file.  When the engine comes acrossed
         * a font-family name that it doesnt know it will request "fontfamilyname".svg
         * file through this method.  The client can either provide the svg file
         * or the engine will default to system text.
         *
         * The usage of this method is exactly the same as the FetchImage method above
         *
         * @since 1.0
         * @param : aUri -- the relative path of an image file.
         * @param : aSession -- File session used by client to open file.
         * @param : aFileHandle -- File handle used by client to open file.
         * @return : KErrNone if Open command successful, otherwise error-code
         *           from Open command.  The SVG engine will proceed loading the file
         *           only if the return value is KErrNone.
         */
		virtual TInt FetchFont( const TDesC& aUri, RFs& aSession, RFile& aFileHandle ) = 0;

        /**
         * This Method updates the presentation status
         * @since 1.0
         * @param : TInt32&  aNoOfAnimation
         * @return :
         */
        virtual  void UpdatePresentation(const TInt32&  aNoOfAnimation)=0;


    };

#endif /*__INC_MSvgRequestObserver__*/
