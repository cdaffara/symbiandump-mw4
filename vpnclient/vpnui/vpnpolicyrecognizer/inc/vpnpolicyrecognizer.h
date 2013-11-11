/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Recognize vpn policy files
*
*/



#ifndef CVPNPOLICYRECOGNIZER_H
#define C_VPNPOLICYRECOGNIZER_H

#include <e32base.h>
#include <apmrec.h>


class CVpnPolicyRecognizer : public CApaDataRecognizerType
    {
public:
    static CVpnPolicyRecognizer* NewL();
    ~CVpnPolicyRecognizer();
    
    TDataType SupportedDataTypeL(TInt aIndex) const;
    void DoRecognizeL(const TDesC &aName, const TDesC8 &aBuffer);
    
private:
    CVpnPolicyRecognizer();

    };

#endif // C_VPNPOLICYRECOGNIZER_H

