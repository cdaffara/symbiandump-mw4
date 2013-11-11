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
* Description:        
*
*/








// INCLUDE FILES
#include "sennotplugindlg.h"

// Epoc DLL entry point, return that everything is ok
#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
    {
    return(KErrNone);
    }
#endif
    

/******************************************************************************
 *              TSenNotPluginMessage implementation
 *****************************************************************************/

// Serialize 16-bit buffer in the format <E16BitDescriptor><length><data>
void TSenNotPluginMessage::AppendBuf16(const TDesC& aBuf)
    {
    TObjectType descType(E16BitDescriptor);
    TInt32 length = aBuf.Length();
    iData.Append((const TUint8*)&descType, sizeof(TObjectType));
    iData.Append((const TUint8*)&length, sizeof(TInt32));
    iData.Append((const TUint8*)aBuf.Ptr(), aBuf.Size());
    }

// Serialize 32 bit integer in the format <EInt32><value>
void TSenNotPluginMessage::AppendInt32(TInt32 aInt32)
    {
    TObjectType intType(EInt32); //
    iData.Append((const TUint8*)&intType, sizeof(TObjectType));
    iData.Append((const TUint8*)&aInt32, sizeof(TInt32));
    }

TPtrC16 TSenNotPluginMessage::GetBuf16(TInt aIndex) const
    {
    TPtr16 result(NULL, 0);
    TUint8* ptr = (TUint8*)iData.Ptr();
    for(TInt i=0; i<aIndex; i++)
        {
        ptr = NavigatePointerToNextObjext(ptr);
        }
    ptr += sizeof(TObjectType);
    TInt32 length = *((TInt32*)ptr);
    ptr += sizeof(TInt32);
    result.Set((TUint16*)ptr, length, length); // Note: length == max length
    return result;
    }

TInt32 TSenNotPluginMessage::GetInt32(TInt aIndex) const
    {
    TUint8* ptr = (TUint8*)iData.Ptr();
    for(TInt i=0; i<aIndex; i++)
        {
        ptr = NavigatePointerToNextObjext(ptr);
        }
    ptr += sizeof(TObjectType);
    TInt32 result = *((TInt32*)ptr);
    return result;
    }

TInt TSenNotPluginMessage::Int32ObjectSize() const
    {
    return sizeof(TObjectType) + sizeof(TInt32);
    }

TInt TSenNotPluginMessage::Buf16ObjectSize(TUint8 *aObjectStart) const
    {
    TUint8 *ptr = aObjectStart;
    ptr += sizeof(TObjectType);
    TInt32 length = *((TInt32*)ptr);
    return sizeof(TObjectType) + sizeof(TInt32) + length * 2; // 2x : 16 bytes per one character
    }

TUint8* TSenNotPluginMessage::NavigatePointerToNextObjext(TUint8* aObjectStart) const
    {
    TUint8* ptr = aObjectStart;
    TObjectType objectType = *((TObjectType*)ptr);
    if(objectType == E16BitDescriptor)
        ptr += Buf16ObjectSize(ptr);
    else if(objectType == EInt32)
        ptr += Int32ObjectSize();
    // else this should panic
    return ptr;
    }

/******************************************************************************
 *              TSenNotPluginRequest implementation
 *****************************************************************************/

EXPORT_C TSenNotPluginRequest::TSenNotPluginRequest()
: iMethod(ENoMethod)
    {
    }

EXPORT_C TInt TSenNotPluginRequest::Method()
    {
    return iMethod;
    }

EXPORT_C TSenNotPluginRequest::TSenNotPluginRequest(TInt aMethod)
: iMethod(aMethod)
    {
    }

/******************************************************************************
 *              TAuthenticationDlgRequest implementation
 *****************************************************************************/


EXPORT_C TAuthenticationDlgRequest::TAuthenticationDlgRequest()
    : TSenNotPluginRequest(EAuthenticationDlg)
    {
    SetData(KNullDesC, KNullDesC);
    }

EXPORT_C TInt TAuthenticationDlgRequest::SetData(const TDesC& aUsername,
                                                    const TDesC& aPassword)
    {
    iData.Zero();
    if(aUsername.Length() > KSenMaxUsernameLength ||
       aPassword.Length() > KSenMaxPasswordLength)
        return KErrBadDescriptor;
    AppendBuf16(aUsername);
    AppendBuf16(aPassword);
    return KErrNone;
    }

EXPORT_C TPtrC16 TAuthenticationDlgRequest::Username() const
    {
    return GetBuf16(0);
    }

EXPORT_C TPtrC16 TAuthenticationDlgRequest::Password() const
    {
    return GetBuf16(1);
    }

/******************************************************************************
 *              TAuthenticationDlgResponse implementation
 *****************************************************************************/

EXPORT_C TAuthenticationDlgResponse::TAuthenticationDlgResponse()
    {
    SetData(KNullDesC, KNullDesC, EFalse);
    }

EXPORT_C TInt TAuthenticationDlgResponse::SetData(const TDesC& aUsername, 
    const TDesC& aPassword, TBool aOkButtonPressed) 
    { 
    iData.Zero(); 
    if(aUsername.Length() > KSenMaxUsernameLength || 
       aPassword.Length() > KSenMaxPasswordLength) 
        return KErrBadDescriptor; 

    //TUint8* ptr = (TUint8*)iData.Ptr();  // ptr not used atm

    TUint16 value16; 
    if (aOkButtonPressed) value16 = 1; 
    else value16 = 0; 
    iData.Append((const TUint8*)&value16, sizeof(TUint16)); 
    iData.Append((const TUint8*)aUsername.Ptr(), aUsername.Size()); 
    if (aUsername.Size() < KSenMaxUsernameLength*2) 
            { 
                iData.AppendFill(0,KSenMaxUsernameLength*2-aUsername.Size()); 
            } 
    iData.Append((const TUint8*)aPassword.Ptr(), aPassword.Size()); 
    if (aPassword.Size() < KSenMaxPasswordLength*2) 
            { 
                iData.AppendFill(0,KSenMaxPasswordLength*2-aPassword.Size()); 
            } 
    return KErrNone; 
    } 

EXPORT_C TPtrC16 TAuthenticationDlgResponse::Username() const 
    { 
    TUint16* ptr = (TUint16*)iData.Ptr(); 
    TInt size = sizeof(TUint16)/2; 
    ptr += size; 
    TUint16* start = ptr; 
    while (*ptr != 0) 
            { 
            ptr += size; 
            } 
        TInt length = (ptr-start)/size; 
    TPtr16 result(NULL, 0); 
    result.Set((TUint16*)start, length, length); // Note: length == max length 
    return result; 
    } 

EXPORT_C TPtrC16 TAuthenticationDlgResponse::Password() const 
    { 
    TUint16* ptr = (TUint16*)iData.Ptr(); 
    TInt size = sizeof(TUint16)/2; 
    ptr += size*(1+KSenMaxUsernameLength); 
    TUint16* start = ptr; 
    while (*ptr != 0) 
            { 
            ptr += size; 
            } 
        TInt length = (ptr-start)/size; 
    TPtr16 result(NULL, 0); 
    result.Set((TUint16*)start, length, length); // Note: length == max length 
    return result; 
    } 

EXPORT_C TBool TAuthenticationDlgResponse::OkButtonPressed() const 
    { 
    if (*(TUint16*)iData.Ptr() == 1) return ETrue; 
    else return EFalse; 
    }

/******************************************************************************
 *              TAllowRegisterDlgRequest implementation
 *****************************************************************************/

EXPORT_C TAllowRegisterDlgRequest::TAllowRegisterDlgRequest()
    : TSenNotPluginRequest(EAllowRegisterDlg)
    {
    }


/******************************************************************************
 *              TAllowRegisterDlgResponse implementation
 *****************************************************************************/

EXPORT_C TAllowRegisterDlgResponse::TAllowRegisterDlgResponse()
    {
    SetOkButton(EFalse);
    }

EXPORT_C void TAllowRegisterDlgResponse::SetOkButton(TBool aValue)
    {
    AppendInt32((TInt32)aValue);
    }

EXPORT_C TBool TAllowRegisterDlgResponse::OkButtonPressed() const
    {
    return (TBool)GetInt32(0);
    }

/******************************************************************************
 *              TAllowUnRegisterDlgRequest implementation
 *****************************************************************************/

EXPORT_C TAllowUnRegisterDlgRequest::TAllowUnRegisterDlgRequest()
    : TSenNotPluginRequest(EAllowUnRegisterDlg)
    {
    }


/******************************************************************************
 *              TAllowUnRegisterDlgResponse implementation
 *****************************************************************************/

EXPORT_C TAllowUnRegisterDlgResponse::TAllowUnRegisterDlgResponse()
    {
    SetOkButton(EFalse); // False by default
    }

EXPORT_C void TAllowUnRegisterDlgResponse::SetOkButton(TBool aValue)
    {
    AppendInt32((TInt32)aValue);
    }

EXPORT_C TBool TAllowUnRegisterDlgResponse::OkButtonPressed() const
    {
    return (TBool)GetInt32(0);
    }

/******************************************************************************
 *              TAskSaveDlgRequest implementation
 *****************************************************************************/

EXPORT_C TAskSaveDlgRequest::TAskSaveDlgRequest()
    : TSenNotPluginRequest(EAskSaveDlg)
    {
    }

/******************************************************************************
 *              TAskSaveDlgResponse implementation
 *****************************************************************************/

EXPORT_C TAskSaveDlgResponse::TAskSaveDlgResponse()
    {
    SetOkButton(EFalse); // False by default
    }

EXPORT_C void TAskSaveDlgResponse::SetOkButton(TBool aValue)
    {
    AppendInt32((TInt32)aValue);
    }

EXPORT_C TBool TAskSaveDlgResponse::OkButtonPressed() const
    {
    return (TBool)GetInt32(0);
    }

// End of File



