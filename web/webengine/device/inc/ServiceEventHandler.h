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
* Description:  Implemetation of ServiceEventHandler
*
*/


#ifndef __SERVICE_EVT_HANDLER_H__
#define __SERVICE_EVT_HANDLER_H__

namespace KJS
    {
    class JSObject;
    class JSValue;
    class ExecState;
    class ProtectedObject;
    class ProtectedValue;
    class List;

    class ServiceEventHandler
        {
        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            ServiceEventHandler(
                ExecState* execState,
                const JSObject* aObject,
                const JSValue* aValue,
				const TInt aTransId);

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~ServiceEventHandler();

        public:

           /**
            * Invoke callback function
            * @return none
            * @since 5.0
            **/
            void InvokeCall( const List& aParam );

           /**
            * Get event handler
            * @return Value
            * @since 5.0
            **/
            JSValue* CallbackFunc() const;

           /**
            * Get transaction id
            * @return Value
            * @since 5.0
            **/
			TInt TransId() const { return iTransId; }



        private:
            JSObject* iInterfaceFunc;
            JSValue* iCallbackFunc;
			TInt iTransId;
            ExecState* iGlobalExecState;
        };

    }

#endif
