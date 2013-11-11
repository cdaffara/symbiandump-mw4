/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   AppUi class
*
*/



#ifndef CALFSHAREDDISPLAYCOECONTROL_H_
#define CALFSHAREDDISPLAYCOECONTROL_H_

#include <coecntrl.h>
#include <uiacceltk/HuiDisplay.h>

class CAlfAppUi;

// Helper class, which is used only if the server functions with one window.
NONSHARABLE_CLASS(CAlfSharedDisplayCoeControl): public CCoeControl
    {
public:
   
    CAlfSharedDisplayCoeControl();
    
    ~CAlfSharedDisplayCoeControl();
    
    void ConstructL();
    
    void Draw(const TRect& aRect) const;
    
    void SizeChanged();
    
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
    RPointerArray<CHuiDisplay> iDisplays;

private:
    
    TRect iPrevRect;
    
    };
   
#endif /*CALFSHAREDWINDOW_H_*/
