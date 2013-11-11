/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#include "HttpCallbacks.h"
#include "HttpConnection.h"
#include "HttpDlConnection.h"
#include <http/rhttpsession.h>

// CONSTANTS


HttpCallbacks::HttpCallbacks()
{
}

HttpCallbacks::~HttpCallbacks()
{
}

void HttpCallbacks::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent &aEvent)
{
    HttpConnection* httpConnection = HttpConnection::connectionFromTransaction(aTransaction);
    if (httpConnection) {
        httpConnection->MHFRunL(aEvent);
    } else {
		RHTTPTransactionPropertySet propSet = aTransaction.PropertySet();
		RStringPool strPool = aTransaction.Session().StringPool();
		THTTPHdrVal callback;
		RStringF downloadPropStr;

		HttpDlConnection* httpDlConnection = HttpDlConnection::dlConnectionFromTransaction(aTransaction);
		if (httpDlConnection) {
			RStringF downloadPropStr = strPool.OpenFStringL( _L8( "TransactionCallback" ) );
			if( propSet.Property( downloadPropStr, callback ) ){
				MHTTPTransactionCallback* callbackPtr = REINTERPRET_CAST( MHTTPTransactionCallback*, callback.Int() );
				if( callbackPtr ){
					callbackPtr->MHFRunL( aTransaction, aEvent );
				}
			}
		}
		else {
			RStringF callbackStr = strPool.OpenFStringL( _L8( "SelfDownloadCallback" ) );
			TBool selfDl = propSet.Property(callbackStr, callback);
			if(selfDl) {
				MHTTPTransactionCallback* callbackPtr = REINTERPRET_CAST( MHTTPTransactionCallback*, callback.Int());
				if(callbackPtr) {
					callbackPtr->MHFRunL(aTransaction, aEvent);
				}
			}
		}
	}
}

TInt HttpCallbacks::MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent &aEvent)
{
    HttpConnection* httpConnection = HttpConnection::connectionFromTransaction(aTransaction);
    if (httpConnection) {
        httpConnection->handleError(aError);
    }	
	else {
		RHTTPTransactionPropertySet propSet = aTransaction.PropertySet();
		RStringPool strPool = aTransaction.Session().StringPool();
		THTTPHdrVal callback;
		RStringF downloadPropStr;

		HttpDlConnection* httpDlConnection = HttpDlConnection::dlConnectionFromTransaction(aTransaction);
		if (httpDlConnection) {
			RStringF downloadPropStr = strPool.OpenFStringL( _L8( "TransactionCallback" ) );
			if( propSet.Property( downloadPropStr, callback ) ){
				MHTTPTransactionCallback* callbackPtr = REINTERPRET_CAST( MHTTPTransactionCallback*, callback.Int() );
				if( callbackPtr ){
					callbackPtr->MHFRunError( aError, aTransaction, aEvent );
				}
			}
		}
		else {
			RStringF callbackStr = strPool.OpenFStringL( _L8( "SelfDownloadCallback" ) );
			TBool selfDl = propSet.Property(callbackStr, callback);
			if(selfDl) {
				MHTTPTransactionCallback* callbackPtr = REINTERPRET_CAST( MHTTPTransactionCallback*, callback.Int());
				if(callbackPtr) {
					callbackPtr->MHFRunError(aError, aTransaction, aEvent);
				}
			}
		}
	}
    return KErrNone;
}
// end of file
