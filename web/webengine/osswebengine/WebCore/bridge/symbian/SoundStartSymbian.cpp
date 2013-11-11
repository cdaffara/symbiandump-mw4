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

#include <e32base.h>
#include "config.h"
#include "SoundStartSymbian.h"
#include "KURL.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "Widget.h"
#include "HTMLNames.h"
#include "WebCoreWidget.h"
#include "WebCoreFrameBridge.h"

const TInt KMaxNumAttributes = 3;
_LIT(KDefaultLoopAttr, "1");
_LIT(KSoundstart,"soundstart");

namespace WebCore {
using namespace HTMLNames;

SoundStart::SoundStart():
                        iPlugin(0),
                        iLoopAttr(KDefaultLoopAttr),
                        iOnFocus(false)
{
}

SoundStart::~SoundStart()
{
    delete iPlugin;
}
   
void SoundStart::SetStartAttribute(const String& aStartAttr)
{   
    if (aStartAttr == "focus") {
        iOnFocus = true;    
    } 
    else {
        iOnFocus = false;    
    }
    
}

void SoundStart::SetSrcAttribute(const String& aSrcAttr)
{   
    iSrcAttr = aSrcAttr;       
}

void SoundStart::SetLoopAttribute(const String& aLoopAttr)
{   
    iLoopAttr = aLoopAttr;
}

void SoundStart::SetVolumeAttribute(const String& aVolumeAttr)
{
    iVolumeAttr = aVolumeAttr;    
}

void SoundStart::LoadPlugin(Frame* aFrame)
{
    if (!iSrcAttr.isEmpty()) {
        
        KURL completedURL = aFrame->loader()->completeURL(iSrcAttr.deprecatedString());
            
        CArrayFixFlat<TPtrC> *attributeNames = new CArrayFixFlat<TPtrC>(KMaxNumAttributes);
        CArrayFixFlat<TPtrC> *attributeValues = new CArrayFixFlat<TPtrC>(KMaxNumAttributes);

        if (!attributeNames || !attributeValues) {
            delete attributeNames;
            delete attributeValues;
            return;
        }
                 
        TRAPD(leave,        
            if (!iSrcAttr.isEmpty()) {
                attributeNames->AppendL(srcAttr.toString());
                attributeValues->AppendL(iSrcAttr);
            }

            if (!iLoopAttr.isEmpty()) {
                attributeNames->AppendL(loopAttr.toString());
                attributeValues->AppendL(iLoopAttr);
            }


            if (!iVolumeAttr.isEmpty()) {
                attributeNames->AppendL(volumeAttr.toString());
                attributeValues->AppendL(iVolumeAttr);        
            }    
        );
       
        if (leave == KErrNone)  {
            // tot:fixme
            //iPlugin = aFrame->bridge()->Callback().viewForPluginWithURL( completedURL.des(),
            //                                                            *attributeNames,
            //                                                            *attributeValues,
            //                                                            KNullDesC8(),
            //                                                           KSoundstart,
            //                                                            true);                                                                                                
        }

        delete attributeNames;
        delete attributeValues;
    }

}

void SoundStart::OnHover(Frame* aFrame, bool aOver)
{   

    if (iPlugin) {
    
        if (!iSrcAttr.isEmpty() && iOnFocus) {
        
            KURL completedURL = aFrame->loader()->completeURL(iSrcAttr.deprecatedString());

            // tot:fixme
            /*if (!aOver) {
                iPlugin->StopPluginContent(completedURL.des());    
            }
            else {              
                iPlugin->PlayPluginContent(completedURL.des());            
            }*/
        }
            
    }
    
}

void SoundStart::OnClick(Frame* aFrame)
{   
    if (iPlugin) {
    
        if (!iSrcAttr.isEmpty() && !iOnFocus) {
        
            KURL completedURL = aFrame->loader()->completeURL(iSrcAttr.deprecatedString());

            // tot:fixme
            /*if (iPlugin->IsActive()) {                
                iPlugin->StopPluginContent(completedURL.des());    
            }
            else {                              
                iPlugin->PlayPluginContent(completedURL.des());            
            }*/
        }
            
    }
    
}


}


