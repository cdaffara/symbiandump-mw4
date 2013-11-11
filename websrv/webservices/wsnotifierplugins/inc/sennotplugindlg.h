/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:        This is common interface for notifier plug-in, which supports
*                UI dialogs for WSF. This interface is used by clients 
*                and plug-in implementation.
*
*/








#ifndef __SENNOTIFIERPLUGINDIALOG_H__
#define __SENNOTIFIERPLUGINDIALOG_H__

// INCLUDES
#include <e32std.h>

// CONSTANTS
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
const TUid KSenNotifierPluginUID     = { 0x101F9764 };
#else
const TUid KSenNotifierPluginUID     = { 0x101F9743 };
#endif

const TInt KSenMaxUsernameLength = 32;
const TInt KSenMaxPasswordLength = 16;
const TInt KSenPluginMsgDataMaxSize = 1024;

// CLASS DECLARATION

/**
* Class TSenNotPluginMessage
* Base class for fixed sized messages transferred between client and plug-in
* notifier implementation.
* This owns container buffer, which holds simple object types
* This is used internally.
*/
class TSenNotPluginMessage
    {
    protected:

        /**
        * Types of objects, which this container can hold
        */
        enum TObjectType
        {
        E8BitDescriptor = 0, // Not currently supported
        E16BitDescriptor,
        EInt32
        };
        
        // New functions
        
        /**
        *  Serialize 16-bit buffer in the format <E16BitDescriptor><length><data>
        *  The data is appended to iData.
        *  Caller is responsible for making sure iData has enough space.
        */
        void AppendBuf16(const TDesC& aBuf);

        /**
        *  Serialize 32 bit integer in the format <EInt32><value>
        *  The data is appended to iData;
        *  Caller is responsible for making sure iData has enough space.
        */
        void AppendInt32(TInt32 aInt32);

        /**
        *  Get 16-bit descriptor from known index. It is callers responsibility to
        *  know that given index contains 16-bit buffer object.
        */
        TPtrC16 GetBuf16(TInt aIndex) const;

        /**
        *  Get 32-bit integer object from known index. It is callers
        *  responsibility to know that given index contains 32-bit integer object.
        */
        TInt32 GetInt32(TInt aIndex) const;

    private: // New functions

        /**
        *  Calculate byte size of 32-bit integer object in this container.
        */
        TInt Int32ObjectSize() const;

        /**
        *  Calculate byte size of 16-bit descriptor object in this container.
        *  The aObjectStart must point to valid area within iData.
        */
        TInt Buf16ObjectSize(TUint8 *aObjectStart) const;

        /**
        *  Find pointer to next object within iData. The aObjectStart must point
        *  to a valid object within iData.
        */
        TUint8* NavigatePointerToNextObjext(TUint8* aObjectStart) const;

    protected: // Data
        TBuf8<KSenPluginMsgDataMaxSize> iData;
    };

/**
* Class TSenNotPluginRequest
* Base class for fixed sized requests transferred between client and plug-in
* notifier implementation.
* This is used internally. This should not be instantiated by the clients.
*/
class TSenNotPluginRequest : public TSenNotPluginMessage
    {
    public:

        /**
        *  List of supported functionalities in plug-in implementation
        */
        enum TSenNotPluginMethod
        {
        ENoMethod,
        EAuthenticationDlg,
        EAllowRegisterDlg,
        EAllowUnRegisterDlg,
        EAskSaveDlg
        };
            
        // New functions

        IMPORT_C TSenNotPluginRequest();

        /**
        *  Request method (TSenPluginMethod)
        */
        IMPORT_C TInt Method();

        /**
        *  Default constructor.
        */
        IMPORT_C TSenNotPluginRequest(TInt aMethod);

    protected: // Data
        TInt iMethod; // see TSenPluginMethod
    };

/**
* Class TSenNotPluginResponse
* Base class for fixed sized responses transferred between client and plug-in
* notifier implementation.
* This is used internally. This should not be instantiated by the clients.
*/
class TSenNotPluginResponse : public TSenNotPluginMessage
    {
    };

/**
* Class TAuthenticationDlgRequest
* Concrete public class for sending authentication dialog request to notifier
* plug-in.
* Notifier will show username/password dialog supporting also Save password
* check box.
* This is typically packaged to TPckgBuf and used as follows:
*      RNotifier iNotifier;
*      TPckgBuf<TAuthenticationDlgRequest> iRequest;
*      TPckgBuf<TAuthenticationDlgResponse> iResponse;
*      ...
*      iNotifier.Connect();
*      ...
*      TRequestStatus status; // typically iStatus of CActive
*      iNotifier.StartNotifierAndGetResponse(status, KSenNotifierPluginUID,
*          request, response);
*      User::WaitForRequest(status);
*      if(status == KErrNone && iResponse().OkButtonPressed())
*          ...
*      ...
*      iNotifier.Close();
*
* See also TAuthenticationDlgResponse
*/
class TAuthenticationDlgRequest : public TSenNotPluginRequest
    {
    public: // New functions

        /**
        *  Default constructor.
        *  Username and password fields will be empty and Save password checkbox
        *  will be unchecked.
        */
        IMPORT_C TAuthenticationDlgRequest();

        /**
        *  Set request data.
        *  Username and password must not exceed KSenMaxUsernameLength and
        *  KSenMaxPasswordLength respectively.
        *  This will fill the iData in base class.
        */
        IMPORT_C TInt SetData(const TDesC& aUsername, const TDesC& aPassword);

        /**
        *  Get view to username value.
        */
        IMPORT_C TPtrC16 Username() const;

        /**
        *  Get view to password value
        */
        IMPORT_C TPtrC16 Password() const;
    };

/**
* Class TAuthenticationDlgResponse
* Concrete public class for receiving authentication dialog response from
* notifier plug-in.
* See also usage in documentation of TAuthenticationDlgRequest
*/
class TAuthenticationDlgResponse : public TSenNotPluginResponse
    {
    public: // New functions

        /**
        *  Default constructor.
        *  Username and password fields will be empty and Save password checkbox
        *  will be unchecked.
        */
        IMPORT_C TAuthenticationDlgResponse();

        /**
        *  Set response data. This is typically used by plug-in implemetation.
        *  Username and password must not exceed KSenMaxUsernameLength and
        *  KSenMaxPasswordLength respectively.
        *  This will fill the iData in base class.
        */
        IMPORT_C TInt SetData(const TDesC& aUsername, const TDesC& aPassword,
                              TBool aOkButtonPressed);

        /**
        *  Get view to username value.
        */
        IMPORT_C TPtrC16 Username() const;

        /**
        *  Get view to password value.
        */
        IMPORT_C TPtrC16 Password() const;

        /**
        *  Get info whether user pressed Ok or Cancel command button in the
        *  dialog.
        */
        IMPORT_C TBool OkButtonPressed() const;
    };


/**
* Class TAllowRegisterDlgRequest
* Concrete public class for sending Yes/No query request to notifier
* plug-in. Implementation will query user, if he/she will allow an application
* to register as a web service.
* This is typically packaged to TPckgBuf and used as follows:
*      RNotifier iNotifier;
*      TPckgBuf<TAllowRegisterDlgRequest> iRequest;
*      TPckgBuf<TAllowRegisterDlgResponse> iResponse;
*      ...
*      iNotifier.Connect();
*      ...
*      TRequestStatus status; // typically iStatus of CActive
*      iNotifier.StartNotifierAndGetResponse(status, KSenNotifierPluginUID,
*          request, response);
*      User::WaitForRequest(status);
*      if(status == KErrNone && iResponse().OkButtonPressed())
*          ...
*      ...
*      iNotifier.Close();
*/
class TAllowRegisterDlgRequest: public TSenNotPluginRequest
    {
    public:

        /**
        *  Default constructor.
        */
        IMPORT_C TAllowRegisterDlgRequest();
    };

/**
* Class TAllowRegisterDlgResponse
* Concrete public class for receiving Yes/No query result.
* See also usage in documentation of TAllowRegisterDlgRequest
*/
class TAllowRegisterDlgResponse : public TSenNotPluginResponse
    {
    public:

        /**
        *  Default constructor.
        */
        IMPORT_C TAllowRegisterDlgResponse();

        // New functions
        
        /**
        *  Set response data. This is typically used by plug-in implemetation.
        *  This will fill the iData in base class.
        */
        IMPORT_C void SetOkButton(TBool aValue);

        /**
        *  Get info whether user pressed Ok or Cancel command button in the
        *  dialog.
        */
        IMPORT_C TBool OkButtonPressed() const;
    };

/**
* Class TAllowUnRegisterDlgRequest
* Concrete public class for sending Yes/No query request to notifier
* plug-in. Implementation will query user, if he/she will allow an application
* to unregister as a web service.
* This is typically packaged to TPckgBuf and used as follows:
*      RNotifier iNotifier;
*      TPckgBuf<TAllowUnRegisterDlgRequest> iRequest;
*      TPckgBuf<TAllowUnRegisterDlgResponse> iResponse;
*      ...
*      iNotifier.Connect();
*      ...
*      TRequestStatus status; // typically iStatus of CActive
*      iNotifier.StartNotifierAndGetResponse(status, KSenNotifierPluginUID,
*          request, response);
*      User::WaitForRequest(status);
*      if(status == KErrNone && iResponse().OkButtonPressed())
*          ...
*      ...
*      iNotifier.Close();
*/
class TAllowUnRegisterDlgRequest: public TSenNotPluginRequest
    {
    public:

        /**
        *  Default constructor.
        */
        IMPORT_C TAllowUnRegisterDlgRequest();
    };

/**
* Class TAllowUnRegisterDlgResponse
* Concrete public class for receiving Yes/No query result.
* See also usage in documentation of TAllowUnRegisterDlgRequest
*/
class TAllowUnRegisterDlgResponse : public TSenNotPluginResponse
    {
    public:

        /**
        *  Default constructor.
        */
        IMPORT_C TAllowUnRegisterDlgResponse();

        // New functions
    
        /**
        *  Set response data. This is typically used by plug-in implemetation.
        *  This will fill the iData in base class.
        */
        IMPORT_C void SetOkButton(TBool aValue);

        /**
        *  Get info whether user pressed Ok or Cancel command button in the
        *  dialog.
        */
        IMPORT_C TBool OkButtonPressed() const;
    };

/**
* Class TAskSaveDlgRequest
* Concrete public class for sending Yes/No query request to notifier
* plug-in. Implementation will query user, if he/she will allow an application
* to save username and password.
* This is typically packaged to TPckgBuf and used as follows:
*      RNotifier iNotifier;
*      TPckgBuf<TAskSaveDlgRequest> iRequest;
*      TPckgBuf<TAskSaveDlgResponse> iResponse;
*      ...
*      iNotifier.Connect();
*      ...
*      TRequestStatus status; // typically iStatus of CActive
*      iNotifier.StartNotifierAndGetResponse(status, KSenNotifierPluginUID,
*          request, response);
*      User::WaitForRequest(status);
*      if(status == KErrNone && iResponse().OkButtonPressed())
*          ...
*      ...
*      iNotifier.Close();
*/
class TAskSaveDlgRequest: public TSenNotPluginRequest
    {
    public:

        /**
        *  Default constructor.
        */
        IMPORT_C TAskSaveDlgRequest();
    };

/**
* Class TAskSaveDlgResponse
* Concrete public class for receiving Yes/No query result.
* See also usage in documentation of TAskSaveDlgRequest
*/
class TAskSaveDlgResponse : public TSenNotPluginResponse
    {
    public:

        /**
        *  Default constructor.
        */
        IMPORT_C TAskSaveDlgResponse();

        // New functions

        /**
        *  Set response data. This is typically used by plug-in implemetation.
        *  This will fill the iData in base class.
        */
        IMPORT_C void SetOkButton(TBool aValue);

        /**
        *  Get info whether user pressed Ok or Cancel command button in the
        *  dialog.
        */
        IMPORT_C TBool OkButtonPressed() const;
    };

#endif // __SENNOTIFIERPLUGINDIALOG_H__

// End of File
