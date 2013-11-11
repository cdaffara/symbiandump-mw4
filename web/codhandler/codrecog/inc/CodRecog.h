/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Declaration of class CCodRecog.   
*      
*
*/


#ifndef COD_RECOG_H
#define COD_RECOG_H

// INCLUDES

#include <apmrec.h>

// CLASS DECLARATION

/**
* COD Recognizer.
*/
class CCodRecog : public CApaDataRecognizerType
    {
    public:     // construction

        /**
        * Constructor.
        */
        CCodRecog();

    public:     // from CApaDataRecognizerType

        /**
        * Return preferred buffer size.
        * @return Preferred buffer size.
        */
        TUint PreferredBufSize();

        /**
        * Return supported data type.
        * @param aIndex Index of data type.
        * @return Supported data type.
        */
        TDataType SupportedDataTypeL( TInt aIndex ) const;

    public:     // new methods

        /**
        * Factory function.
        * @return Recognizer.
        */
        static CApaDataRecognizerType* CreateRecognizerL();

    private:    // from CApaDataRecognizerType

        /**
        * Recognize content. Leaves only on general failures (OOM etc), but
        * does not leave on any recognition failures.
        * @param aName File name.
        * @param aBuffer Content buffer.
        */
        void DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer );

    };

#endif /* def COD_RECOG_H */
