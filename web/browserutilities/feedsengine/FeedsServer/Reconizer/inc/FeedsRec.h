/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Recognizer for the bowser supported MIME types.
*
*/


#ifndef FEEDS_REC_H
#define FEEDS_REC_H

#include <apmrec.h>

// Constants
const TUid KUidMimeFeedsRec = { 0x102072ef };
const TInt KFeedsRecImplUIDValue = 0x102072EE;
 
const TInt KSupportedMimetypes(1);


class CFeedsRec: public CApaDataRecognizerType
    {
    public: // Constructors
        /**
        * Two-phased constructor.
        */
        CFeedsRec();

        /**
        * Creates an recognizer instance.
        *
        * @since ?
        * @return The new instance.
        */
        static CApaDataRecognizerType* CreateRecognizerL();

        /**
        * Returns the preferred buffer size.
        *
        * @since ?
        * @return The size.
        */
        virtual TUint PreferredBufSize(); 

        /**
        * Returns the index'ed supported mime-type.
        *
        * @since ?
        * @param aIndex The index.
        * @return The mime-type.
        */
        virtual TDataType SupportedDataTypeL(TInt aIndex) const;


    private: // From CApaDataRecognizerType
        /**
        * Given the buffer it examines the buffer and sets the inherited
        * member variable, iConfidence.
        *
        * @since ?
        * @param aName The name of the buffer's source.
        * @param aBuffer The buffer to examine.
        * @return void.
        */
        virtual void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);

        /**
        * Given the buffer it examines the buffer and returns the 
        * confidence level that this document is rss.
        * @since 3.0
        * @param aName The name of the buffer's source.
        * @param aBuffer The buffer to examine.
        * @return The confidence level.
        */
        TInt CFeedsRec::RecognizeRss(const TDesC& aName, const TDesC8& aBuffer);
    };

#endif
