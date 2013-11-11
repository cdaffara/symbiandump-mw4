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
* Description:  
*
*/

#include "PlatformString.h"

class MWebCoreObjectWidget;

namespace WebCore {

class Frame;

class SoundStart
{
public:
    SoundStart();
    ~SoundStart();

public:
    void SetStartAttribute(const String& aStartAttr);
    void SetSrcAttribute(const String& aSrcAttr);
    void SetLoopAttribute(const String& aLoopAttr);
    void SetVolumeAttribute(const String& aVolumeAttr);

public:    
    void LoadPlugin(Frame* aFrame);    
    void OnHover(Frame* aFrame, bool aOver);
    void OnClick(Frame* aFrame);

private:
    MWebCoreObjectWidget* iPlugin;
    
    String iSrcAttr;       
    String iLoopAttr;
    String iVolumeAttr;
    
    bool iOnFocus;    
};

}
