/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of CHuiCanvasRenderBuffer
*
*/



#ifndef __HUICANVASRENDERBUFFER_H__
#define __HUICANVASRENDERBUFFER_H__


#include <e32base.h>
    
/* Forward declarations. */    
class CHuiRenderPlugin;

class CHuiCanvasRenderBuffer : public CBase
    {
public:

	/**
	 * Constructor.
	 */
	IMPORT_C CHuiCanvasRenderBuffer();

	/**
	 * Destructor.
	 */ 
	IMPORT_C ~CHuiCanvasRenderBuffer();
    
    /**
     * Initializes render buffer.
     */
	IMPORT_C virtual void InitializeL(const TSize& aSize);
    
    /**
     * Releases render buffer resources.
     */
	IMPORT_C virtual void UnInitialize();
	
    /**
     * Binds as render target.
     */ 
	IMPORT_C virtual void Bind();

    /**
     * Unbinds as from being the active render target.
     */ 
    IMPORT_C virtual void UnBind();

    /**
     * Returns size in pixels.
     */ 
    IMPORT_C TSize Size() const;
    
    /**
     * Is buffer initilized or not
     */ 
    IMPORT_C TBool IsInitialized() const;
    
    /**
     * Copies content from the given buffer
     */ 
    IMPORT_C virtual void Copy(const CHuiCanvasRenderBuffer& aSourceBuffer);
    
    /**
     * Copies content from the active surface.
     */ 
    IMPORT_C virtual void Copy(TPoint aPoint);    
    
    /**
     * Initializes render buffer, with clear option.
     * @param aSize size of render buffer.
     * @param aClear contents of render buffer are cleared even if alredy initialized.
     */
    IMPORT_C virtual void InitializeL(const TSize& aSize, TBool aClear);
    
protected:
    
    TSize iSize;
    TBool iInitialized;
 
    };

#endif        
    
