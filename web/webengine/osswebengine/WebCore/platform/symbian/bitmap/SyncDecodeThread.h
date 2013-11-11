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
* Description:  
*
*/


#ifndef SYNCDECODETHREAD_H
#define SYNCDECODETHREAD_H

// INCLUDES

#include <e32base.h>

class BmElem;
class CMaskedBitmap;
class CSynDecoder;
// CLASS DECLARATION
/**
*  CSynDecodeThread
*
*  Implementation of file based image class.
*  @lib IHL.lib
*  @since 3.0
*/

class CSynDecodeThread  : public CBase
    {
    public:  // Constructors and destructor

        static CSynDecodeThread* NewL();
        virtual ~CSynDecodeThread();

  public:

        TInt Decode(const TDesC8& aData);
        void Handle( TInt& aBitmapHandle, TInt& aMaskHandle );
        
  private: // Private constructors

        CSynDecodeThread();
        void ConstructL();
        static  TInt ScaleInThread(TAny *aPtr);

  private: // Data

        // Image status & state
        RThread iDecoderThread;
        static CSynDecoder* iSyncDecoder;
    };

#endif   // SYNCDECODETHREAD_H

// End of File
