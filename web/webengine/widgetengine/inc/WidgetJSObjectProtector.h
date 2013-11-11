/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface MJSObjectProtector
*
*/

#ifndef __WIDGETJSOBJECTPROTECTOR_H__
#define __WIDGETJSOBJECTPROTECTOR_H__

namespace KJS {
	class JSValue;
}

class MJSObjectProtector
{
public:
	virtual void Protect(KJS::JSValue*) = 0;
	virtual void Unprotect(KJS::JSValue*) = 0;
};

#endif // !__WIDGETJSOBJECTPROTECTOR_H__
