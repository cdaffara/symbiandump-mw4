/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpTmInfoElement class declaration 
*
*/

#ifndef __UPNPTMINFOELEMENT_H__
#define __UPNPTMINFOELEMENT_H__

#include <e32base.h>
#include <badesca.h>

//Literals
_LIT8(KRemotingInfo,   "remotingInfo" );
_LIT8(KAppInfo,        "appInfo" );
_LIT8(KDisplayInfo,    "displayInfo" );
_LIT8(KAudioInfo,      "audioInfo" );

/**
 * Class represents the list of key-value pairs for each terminal mode info type.
 * Used by the AutomotiveServer to provide name-value pairs for the associated application
 */
class CUpnpTmInfoElement : public CBase
    {
public:
    enum TTerminalModeInfoType
        {
        ERemotingInfo,
        EAppInfo,
        EDisplayInfo,
        EAudioInfo
        };
    /**
    * Standard Symbian Two-phased constructor.  
    */
    static CUpnpTmInfoElement* NewL( TTerminalModeInfoType aTerminalModeInfoType );
    // Destructor
    ~CUpnpTmInfoElement();
    /**
    * Method is used to set terminal mode info key-value pair
    */
    IMPORT_C void AddTmInfoElementL( const TDesC8& aName,const TDesC8& aValue );
    // Unexported getter method
    void GetTmInfoElementL( RBuf8& aInfoElementName, 
                               RBuf8& aInfoElementValue, TInt aIndex )const;
    // inlined getter methods
    inline const TDesC8& TerminalModeInfoType( )const;
    inline TInt TmInfoElementCount()const;

private:
    CUpnpTmInfoElement();
    void ConstructL( TTerminalModeInfoType aTerminalModeInfoType );

private:
    HBufC8*                   iInfoTypeBuffer;
    RArray<RBuf8>             iInfoElementNameArray;
    RArray<RBuf8>             iInfoElementValueArray;
    RBuf8                     iElementName;
    RBuf8                     iElementValue;
    };

#include <upnptminfoelement.inl>

#endif  //__UPNPTMINFOELEMENT_H__
