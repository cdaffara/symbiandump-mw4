/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/

#include <PlatformString.h>
#include <f32file.h>

namespace WebCore {

bool fileExists(const String& path)
{
    if (path.des().Length() == 0)
        return false;
    RFs rfs;
    RFile file;
    TInt ret = rfs.Connect(); // used by multiple threads. Cannot us fsSession()
    if (ret == KErrNone) {
        TPtrC fileName = path.des();
        ret = file.Open(rfs, fileName, EFileShareAny);
        if (ret == KErrNone) {
            file.Close();
        }
        rfs.Close();
    }
    return ret == KErrNone;
}

bool deleteFile(const String& path)
{
    if (path.des().Length() == 0)
        return false;
    RFs rfs;
    RFile file;
    TInt ret = rfs.Connect(); // used by multiple threads. Cannot us fsSession()
    if (ret == KErrNone) {
        ret =  rfs.Delete(path.des());
        rfs.Close();
    }
    return ret == KErrNone;
}

bool fileSize(const String& path, long long& result)
{
    if (path.des().Length() == 0)
        return false;
    RFs rfs;
    RFile file;
    TInt ret = rfs.Connect(); // used by multiple threads. Cannot us fsSession()
    if (ret == KErrNone) {
        TPtrC fileName = path.des();
        TInt ret = file.Open(rfs, fileName, EFileShareAny);
        if (ret == KErrNone) {
            TInt size;
            ret = file.Size(size);
            file.Close();
            if (ret == KErrNone) {
                result = size;
            }
        }
        rfs.Close();
    }
    return ret == KErrNone;
}

}

// vim: ts=4 sw=4 et
