/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __WIDGETENGINECALLBACKS_H
#define __WIDGETENGINECALLBACKS_H

class CFbsBitmap;

class MWidgetEngineCallbacks
{
public:    
    virtual void drawWidgetTransition() = 0;            
    virtual CFbsBitmap* offscreenBitmap() = 0;
    virtual void setRightSoftKeyLabel( const TDesC& aText ) = 0;
    virtual void setLeftSoftKeyLabel( const TDesC& aText ) = 0;
    virtual void setTabbednavigation(bool aOn) = 0;
    virtual void setNavigationType(const TDesC& aType) = 0;

};

#endif
