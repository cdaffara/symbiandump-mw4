/*
* ==============================================================================
*  Name        : BrCtlSpecialLoadObserverImpl.h
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


#ifndef BRCTLSPECIALLOADOBSERVERIMPL_H
#define BRCTLSPECIALLOADOBSERVERIMPL_H

//  INCLUDES
#include <BrCtlSpecialLoadObserver.h>

/**
*  This observer handles special load events such as network connection, deal with non-http(s) or non-html requests
*
*  @lib BrowserEngine.lib
*  @since 2.8
*/
//NONSHARABLE_CLASS(CBrCtlSpecialLoadObserver) : public CBase, public MBrCtlSpecialLoadObserver
class CBrCtlSpecialLoadObserver : public CBase, public MBrCtlSpecialLoadObserver
    {
    public: // From MBrCtlSpecialLoadObserver
        /**
        * Request to create a network connection.
        * @since 2.8
        * @param aConnectionPtr A pointer to the new connection. If NULL, the proxy filter will automatically create a network connection
        * @param aSockSvrHandle A handle to the socket server.
        * @param aNewConn A flag if a new connection was created. If the connection is not new, proxy filter optimization will not read the proxy again from CommsBd
        * @param aBearerType The bearer type of the new connection
        * @return void
        */
        virtual void NetworkConnectionNeededL(TInt* /*aConnectionPtr*/,
                                              TInt* /*aSockSvrHandle*/,
                                              TBool* /*aNewConn*/,
                                              TApBearerType* /*aBearerType*/) {}

        /**
        * Request the host applicaion to handle non-http request.
        * @since 2.8
        * @param aUrl The non-http(s) or file URL
        * @param aParamList Parameters to pass to the host application. Contain referer header. It could be NULL
        * @return ETrue is handled by the host application. EFlase if not
        */
        virtual TBool HandleRequestL(RArray<TUint>* /*aTypeArray*/, CDesCArrayFlat* /*aDesArray*/) {return EFalse;}

        /**
        * Request the host applicaion to handle downloads
        * @since 2.8
        * @param aTypeArray array of download parameter types
        * @param aDesArray array of values associated with the types in the type array
        * @return ETrue is handled by the host application. EFlase if not
        */
        virtual TBool HandleDownloadL(RArray<TUint>* /*aTypeArray*/,
						CDesCArrayFlat* /*aDesArray*/) {return EFalse;}
    };

#endif      // BRCTLSPECIALLOADOBSERVERIMPL_H
            
// End of File
