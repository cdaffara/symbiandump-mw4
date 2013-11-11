/*
* ==============================================================================
*  Name        : BrCtlWinowObserverImpl.h
*  Part of     : WebKit \ BrowserControl
*  Interface   : Browser Control API
*  Description : Handle special load events such as network connection, deal with non-http or non-html requests
*  Version     : %Version%
*
*    Copyright (c) 2006, Nokia Corporation
*    All rights reserved.
*  
*  	Redistribution and use in source and binary forms, with or without
*  	modification, are permitted provided that the following conditions
*  	are met:
*  
*      * Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*      * Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in
*        the documentation and/or other materials provided with the
*        distribution.
*      * Neither the name of the Nokia Corporation nor the names of its
*        contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*  
*  	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
*  	USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
*  	DAMAGE.
*  
*    Please see file patentlicense.txt for further grants.
* ==============================================================================
*/


#ifndef BRCTLWINDOWOBSERVERIMPL_H
#define BRCTLWINDOWOBSERVERIMPL_H

//  INCLUDES
#include <BrCtlWindowObserver.h>


/**
*  This observer handles special load events such as network connection, deal with non-http(s) or non-html requests
*
*  @lib BrowserEngine.lib
*  @since 2.8
*/
class CBrCtlWindowObserver : public CBase, public MBrCtlWindowObserver
    {
    public: // From MBrCtlWindowObserver
        /**
        * Request the host applicaion to open the URL in a new window
        * @since 3.0
        * @param aUrl The Url of the request to be done in the new window
        * @param aTargetName The name of the new window
        * @param aUserInitiated ETrue if the new window is initiated by a user event (click)
        * @param aReserved For future use
        * @return Return Value is for future reference and is currently ignored
        */
        virtual CBrCtlInterface* OpenWindowL(TDesC& /*aUrl*/, TDesC* /*aTargetName*/, 
			TBool /*aUserInitiated*/,  TAny* /*aReserved*/) {return NULL;}
    
        /**
        * Find a window by target name
        * @since 3.0
        * @param aTargetName name of the window to find
        * @return Return Value is the browser control associated with the window name
        */
        virtual CBrCtlInterface* FindWindowL( const TDesC& /*aTargetName*/ ) const { return NULL; }             

        /**
        * Handle window events such as close/focus etc
        * @since 3.0
        * @param aTargetName name of the window to send the event to
        * @param aCommand Command to pass to the window
        * @return void
        */
        virtual void HandleWindowCommandL( const TDesC& /*aTargetName*/, TBrCtlWindowCommand /*aCommand*/ ) {}

    };

#endif      // BRCTLWINDOWOBSERVERIMPL_H
            
// End of File
