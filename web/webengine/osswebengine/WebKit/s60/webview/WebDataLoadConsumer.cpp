/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles the viewing of a single frame. If the page is not frame
*                enabled, this class is used as the single view.  If frame
*                enabled, there is one instance of this class for each frame.
*
*/


#include "WebDataLoadConsumer.h"

WebDataLoadConsumer* WebDataLoadConsumer::initWithMetaData(CBrCtl* brctl, const TDesC& url, const TDataType& dataType,
    TUid charsetUid, TUint contentLength, MBrCtlDataLoadSupplier* brCtlDataLoadSupplier)
{
    WebDataLoadConsumer* self = new WebDataLoadConsumer(brctl, dataType, charsetUid, contentLength, brCtlDataLoadSupplier);
    if (self) {
        self->m_url = url.Alloc();
        self->m_data = HBufC8::New(contentLength);
        if (self->m_url == NULL || self->m_data == NULL) {
            delete self;
            self = NULL;
        }
    }
    return self;
}

WebDataLoadConsumer::WebDataLoadConsumer(CBrCtl* brctl, const TDataType& dataType,
    TUid charsetUid, TUint contentLength, MBrCtlDataLoadSupplier* brCtlDataLoadSupplier) :
m_brctl(brctl),
m_dataType(dataType),
m_charsetUid(charsetUid),
m_contentLength(contentLength),
m_brCtlDataLoadSupplier(brCtlDataLoadSupplier)
{
}
	
WebDataLoadConsumer::~WebDataLoadConsumer()
{
    delete m_url;
    delete m_data;
}
	    
void WebDataLoadConsumer::stopDataLoad()
{
    m_brCtlDataLoadSupplier->CancelLoad();
}

void WebDataLoadConsumer::HandleNextDataChunk(const TDesC8& aData)
{
    if (m_data->Length() + aData.Length() > m_contentLength) {
        m_brCtlDataLoadSupplier->CancelLoad();
        m_brctl->endLoadData();
    }
    else {
        m_data->Des().Append(aData);
    }
}

void WebDataLoadConsumer::HandleLoadComplete()
{
    TRAP_IGNORE(m_brctl->LoadDataL(m_url->Des(), m_data->Des(), m_dataType, m_charsetUid));
    m_brctl->endLoadData();
}

void WebDataLoadConsumer::HandleError(TInt aError)
{
    m_brctl->endLoadData();
}
