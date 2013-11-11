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
* Description:   
*
*/



#ifndef HUIFXFILTERLAYER_H_
#define HUIFXFILTERLAYER_H_

#include "HuiFxLayer.h"

//FORWARD DECLARATIONS
class CHuiFxFilter;

class CHuiFxFilterLayer: public CHuiFxLayer
    {
public:
    IMPORT_C ~CHuiFxFilterLayer();
    IMPORT_C static CHuiFxFilterLayer* NewL(CHuiFxFilter* aFilter); // takes ownership
    IMPORT_C TBool PrepareDrawL(CHuiFxEngine& aEngine);
    IMPORT_C void Draw(CHuiFxEngine& aEngine, CHuiGc& aGc, CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource, TBool aHasSurface);
    IMPORT_C CHuiFxFilter& Filter() const;
    IMPORT_C TBool Changed() const;
    IMPORT_C void AdvanceTime(TReal32 aElapsedTime);
    IMPORT_C TBool Margin(TMargins &m);
public: // effect cache
    IMPORT_C CHuiFxFilterLayer *CloneL() const;
    IMPORT_C void SetExtRect( TRect *aExtRect );
    IMPORT_C void SetVisual( CHuiVisual *aVisual );
    IMPORT_C void SetVisual( MHuiEffectable *aVisual );

    TBool IsFiltered() const; 
    
protected:
    CHuiFxFilterLayer();
    void ConstructL(CHuiFxFilter* aFilter);

private:
    CHuiFxFilter*             iFilter;
    };

#endif /*HUIFXFILTERLAYER_H_*/
