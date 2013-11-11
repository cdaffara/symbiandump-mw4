/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  provides mapping from iana code, symbian charset UID code and common
*                 internet names.
*
*/
#ifndef __CHARSETDATA_H
#define __CHARSETDATA_H

#include <charconv.h>
struct TCharsetEntry
{
  TUint32 uid;
  const TText* charsetName;
};

const TText* charsetForUid(unsigned int uid);
unsigned int  uidForCharset(TText* charset);

//  UID (symbian),                      standard + common charset name

#endif /* __CHARSETDATA_H */

// End of File
