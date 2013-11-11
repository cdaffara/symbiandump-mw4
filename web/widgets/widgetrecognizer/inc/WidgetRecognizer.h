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
* Description:  Recognizer for the widgets
*
*/


#ifndef WIDGETRECOGNIZER_H
#define WIDGETRECOGNIZER_H

// INCLUDE FILES
#include <apmrec.h>  // For CApaDataRecognizerType

// CONSTANTS
const TUid KUidMimeWidgetRecognizer = { 0x10282820 };
const TInt KWidgetRecognizerImplUIDValue = 0x10282843;
const TUid KUidWidgetRecognizerImpl = { KWidgetRecognizerImplUIDValue };


//
// CWidgetsRecognizer
//
class CWidgetRecognizer : public CApaDataRecognizerType
    {
    public: 
        // constructor
        CWidgetRecognizer();
        
        static CApaDataRecognizerType* CreateRecognizerL();
        
    public: // from CApaDataRecognizerType

        /**
        * From CApaDataRecognizerType
        * Gets the size of buffer preferred for the purpose of recognizing the data type.
        * @return The preferred data size.
        * @since 3.1
        */
        virtual TUint PreferredBufSize();
        
        /**
        * From CApaDataRecognizerType
        * Gets one of the data (MIME) types that the recognizer can recognize.
        * @param aIndex - An index that identifies the data type. Typically, 
        *        the minimum value is zero and the maximum value is the value of MimeTypesCount() - 1. 
        *
        * @return The data (MIME) type.
        * @since 3.1
        */
        virtual TDataType SupportedDataTypeL(TInt aIndex) const;
        
    private: // from CApaDataRecognizerType

        /**
        * From CApaDataRecognizerType
        * This implements recognition behaviour -- called by RecognizeL()
        * @param aName - The name of the data; typically this is a file name 
        *                containing the data to be recognized.
        * @param aBuffer - A buffer containing data to be recognized; typically, this is read from the start of the file containing the data. 
        *
        * @return void.
        * @since 3.1
        */
        virtual void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
    };

#endif // WIDGETRECOGNIZER_H

// END FILE
