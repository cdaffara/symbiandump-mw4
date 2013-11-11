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

#ifndef __FILECONNECTION_H__
#define __FILECONNECTION_H__

#include <e32base.h>
#include <f32file.h>
#include "UrlConnection.h"

class CFileReader;

class FileConnection : public MUrlConnection
{
public:
    /**
    */
    FileConnection(WebCore::ResourceHandle* _handle);
    /**
    */
    virtual ~FileConnection();
    /**
    */
    virtual int submit();
    /**
    */
    virtual void cancel();
    /**
    */
    virtual void download(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& response);

    virtual int totalContentSize() { return m_maxSize; }
    /**
    */
    void response();
    /**
    */
    void complete(int error);

private:
    /**
    */
    void submitL();
    /**
    */
    void responseL();
    /**
    */
    HBufC* parseFileNameLC(const TDesC8& aUrl );
    /**
    * Determine the content type of the file.
    */
    HBufC8* contentTypeL();

    /**
    * Determine the content encoding of the file.
    */
    TPtrC8 contentEncoding(const TDesC8& aContentTypeString ) const;
private:
    HBufC* m_fileName;
    RFile m_file;
    int m_maxSize;
    CFileReader* m_fileReader;
    int m_chunkIndex;
	HBufC* m_charset;
	HBufC* m_contentType;
};


#endif // __FILECONNECTION_H__
// end of file
