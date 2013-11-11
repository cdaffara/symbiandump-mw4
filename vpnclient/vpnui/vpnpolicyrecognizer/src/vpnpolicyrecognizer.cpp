/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Recognize vpn policy files
*
*/

#include <f32file.h>
#include <ecom/implementationproxy.h>

#include "vpnpolicyrecognizer.h"
#include "vpnpolicyrecognizerconstants.h"


const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KVpnPolicyRecognizerId, CVpnPolicyRecognizer::NewL)
    };


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }


CVpnPolicyRecognizer* CVpnPolicyRecognizer::NewL()
    {
    CVpnPolicyRecognizer* self = new (ELeave) CVpnPolicyRecognizer;
    
    return self;
    }


CVpnPolicyRecognizer::CVpnPolicyRecognizer()
: CApaDataRecognizerType(KVpnPolicyRecognizerUid, EHigh)
    {
    iCountDataTypes = KSupporstedMimeTypeCount;
    }


CVpnPolicyRecognizer::~CVpnPolicyRecognizer()
    {
    }


TDataType CVpnPolicyRecognizer::SupportedDataTypeL(TInt aIndex) const
    {
    
    if (aIndex == 0)
        {        
        return TDataType(KZippedPolicyFile);            
        }
    else
        {
        return TDataType(KPolicyInfoFile); 
        }
    }   
    
    
void CVpnPolicyRecognizer::DoRecognizeL(const TDesC &aName, const TDesC8& /*aBuffer*/)
    {
        
    iConfidence=ENotRecognized;
    
    if (aName.Length() >= KFileExtensionLength)
        {
        TPtrC extension(aName.Right(KFileExtensionLength));
                           
        if (extension.CompareF(KVpnZipPolicyFileExtension) == 0)
            {
            iConfidence=ECertain;
            iDataType=TDataType(KZippedPolicyFile); 
            }
        else if (extension.CompareF(KVpnPinPolicyFileExtension) == 0)
            {
            iConfidence=ECertain;
            iDataType=TDataType(KPolicyInfoFile); 
            }
        }
    }
