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


#ifndef __FORMFILLPOPUP_H__
#define __FORMFILLPOPUP_H__

#include <e32base.h>

namespace WebCore
{
    class Frame;
}

class MFormFillPopup
{
public:
    virtual void addRow(const TPtrC&) = 0;
    virtual void invalidate() = 0;
    virtual void clear() = 0;
    virtual void setLocationHintInDoc(const TRect& r, WebCore::Frame* frame) = 0;
    virtual ~MFormFillPopup() {}
};

#endif //!__FORMFILLPOPUP_H__
