/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   UID definitions
*
*/



#ifndef ALFREALLINE_H
#define ALFREALLINE_H

struct TAlfRealLine
    {
    TAlfRealLine(){}
    TAlfRealLine(const TAlfRealPoint& aStart, const TAlfRealPoint& aEnd):iStart(aStart),iEnd(aEnd){}

    TAlfRealPoint iStart;
    TAlfRealPoint iEnd;    

	private:
    TInt iSpare1;
    TInt iSpare2;
    };
        
        
    
#endif    