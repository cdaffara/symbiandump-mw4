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








#include "sencertutils.h"
#include <ssl.h>

EXPORT_C HBufC8* SenCertUtils::CertInfoToStringLC(const TCertInfo& aCertInfo)
    {
    //
    // Convert TCertInfo to TDesC8
    //
    TInt certLength = aCertInfo.iIssuerDNInfo.iCountry.Length();
    certLength += aCertInfo.iIssuerDNInfo.iOrganizationUnit.Length();
    certLength += aCertInfo.iIssuerDNInfo.iOrganization.Length();
    certLength += aCertInfo.iIssuerDNInfo.iCommonName.Length();
    certLength += aCertInfo.iSerialNo.Length() * 2;
    certLength += aCertInfo.iFingerprint.Length() * 2;
    
    HBufC8* casn = HBufC8::NewLC( certLength + 4);
    TPtr8 ptr = casn->Des();

    
    if ( aCertInfo.iIssuerDNInfo.iCommonName.Length())
        {
        ptr.Append( aCertInfo.iIssuerDNInfo.iCommonName);
        ptr.Append( KSenCASNDelimiter );
        }
    
    if ( aCertInfo.iIssuerDNInfo.iOrganization.Length())
        {
        ptr.Append( aCertInfo.iIssuerDNInfo.iOrganization);
        ptr.Append( KSenCASNDelimiter );
        }
    
    if ( aCertInfo.iIssuerDNInfo.iOrganizationUnit.Length())
        {
        ptr.Append( aCertInfo.iIssuerDNInfo.iOrganizationUnit);
        ptr.Append( KSenCASNDelimiter );
        }
    
    if ( aCertInfo.iIssuerDNInfo.iCountry.Length())
        {
        ptr.Append( aCertInfo.iIssuerDNInfo.iCountry);
        ptr.Append( KSenCASNDelimiter );
        }
    
    if ( aCertInfo.iSerialNo.Length())
        {
        //convert serialnumber to plain text
        for ( TInt i(0); i < aCertInfo.iSerialNo.Length(); i++)
            {
            ptr.AppendNum( aCertInfo.iSerialNo[i], EHex);
            }   
        ptr.Append( KSenCASNDelimiter );
        }

    if ( aCertInfo.iFingerprint.Length())
        {
        //convert fingerprint to plain text
        for ( TInt i(0); i < aCertInfo.iFingerprint.Length(); i++)
            {
            ptr.AppendNum( aCertInfo.iFingerprint[i], EHex);
            }   
        }   
    return casn;
    }

// End of file


