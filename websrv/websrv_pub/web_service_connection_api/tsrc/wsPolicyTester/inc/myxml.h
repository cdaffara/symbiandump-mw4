/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/







#ifndef MYXMLELE_H
#define MYXMLELE_H

#include <e32std.h>
#include <f32file.h>
#include "SenDomFragment.h"


class CMyXmlEle :public CSenDomFragment
{
public:    
    static CMyXmlEle* NewL();
    static CMyXmlEle* NewLC();
    ~CMyXmlEle();
    
    void ReadFileL(TFileName aPath);
    void WriteAllL(CSenElement* aXml);
    CSenElement* XMLDocL();
private:

    CMyXmlEle();
    void ConstructL();
    TInt count;

};

#endif