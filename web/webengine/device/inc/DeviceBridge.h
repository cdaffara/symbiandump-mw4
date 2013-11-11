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


#ifndef __DEVICEBRIDGE
#define __DEVICEBRIDGE

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATION
namespace KJS
    {
    class JSObject;
    class ExecState;
    class Device;
    }


// CLASS DECLARATION

// Calls from browser engine (implemented by DeviceBridge)

class MDeviceBridge
    {
    public:
        inline virtual void* Device( void* exec ) = 0;
        inline virtual ~MDeviceBridge() {};
        inline virtual void SetUid( const TUint& aValue) = 0;
        virtual void Clear() = 0;
		inline virtual  void* GetSecuritySession() = 0;
    };

/**
*  TDeviceBridge
*
*  @lib jsdevice.dll
*  @since 3.1
*/

NONSHARABLE_CLASS( TDeviceBridge ): public MDeviceBridge
    {

    public:

       /**
        * Constructor
        * @since 5.0
        * @param
        */
        TDeviceBridge();

       /**
        * Destructor.
        * @since 5.0
        * @param
        */
        virtual ~TDeviceBridge();

    public: //From MDeviceBridge

       /**
        * Device
        * @since 5.0
        * @param
        * @return
        */
        void* Device( void* exec );

       /**
        * SetBlanketPermission
        * @since 5.0
        * @param 
        * @return none
        */
        void SetUid( const TUint& aValue);
        void Clear();
		
		void* GetSecuritySession();

    private:

        KJS::Device* m_device;// Not owned

 };

#endif
