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

#ifndef __WEBDATALOADCONSUMER_H__
#define __WEBDATALOADCONSUMER_H__

#include <e32base.h>
#include "BrCtl.h"

//forward declarations

class WebDataLoadConsumer : public MBrCtlDataLoadConsumer
    {
    public:
        static WebDataLoadConsumer* initWithMetaData(CBrCtl* brctl, const TDesC& url, const TDataType& dataType,
            TUid charsetUid, TUint contentLength, MBrCtlDataLoadSupplier* brCtlDataLoadSupplier);
	    ~WebDataLoadConsumer();
        void stopDataLoad();
	    
    public: // from MBrCtlDataLoadConsumer
        /**
        * Pass the next data chunk
        * @since 2.8
        * @param aData The data chunk to process
        * @return void
        */
        void HandleNextDataChunk(const TDesC8& aData);

        /**
        * Inform the Browser Control about completion of data load
        * @since 2.8
        * @return void
        */
        void HandleLoadComplete();

        /**
        * Inform the Browser Control that an error occured
        * if HandleError() is called, then LoadComplete() is infered.
        * @since 2.8
        * @param aError The error code
        * @return void
        */
        void HandleError(TInt aError);
    private:
        WebDataLoadConsumer(CBrCtl* brctl, const TDataType& dataType, TUid charsetUid,
            TUint contentLength, MBrCtlDataLoadSupplier* brCtlDataLoadSupplier);
			    
    private:
        CBrCtl* m_brctl;
        HBufC* m_url;
        TDataType m_dataType;
        TUid m_charsetUid;
        int m_contentLength;
        MBrCtlDataLoadSupplier* m_brCtlDataLoadSupplier;
        HBufC8* m_data;
};

#endif //__WEBDATALOADCONSUMER_H__
