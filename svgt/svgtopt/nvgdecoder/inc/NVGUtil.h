/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  NVG Decoder header file
*
*/


#ifndef NVGUTIL_H_
#define NVGUTIL_H_

#include <e32base.h>

#ifndef NVG_DEBUG
    #define NVG_DEBUGP1(x1)
    #define NVG_DEBUGP2(x1, x2)
    #define NVG_DEBUGP3(x1, x2, x3)
    #define NVG_DEBUGP4(x1, x2, x3, x4)
    #define NVG_DEBUGP5(x1, x2, x3, x4, x5)
    #define NVG_DEBUGP6(x1, x2, x3, x4, x5, x6)
    #define NVG_DEBUGP7(x1, x2, x3, x4, x5, x6, x7)
    #define NVG_DEBUGP8(x1, x2, x3, x4, x5, x6, x7, x8)
    #define NVG_DEBUGP9(x1, x2, x3, x4, x5, x6, x7, x8, x9)

    #define NVG_DEBUGSTMT(x1)

    #define VGCREATEPAINT()     vgCreatePaint()
    #define VGDESTROYPAINT(h)   vgDestroyPaint(h)
    
    #define VGCREATEPATH(a1, a2, a3, a4, a5, a6, a7)  vgCreatePath(a1, a2, a3, a4, a5, a6, a7)
    #define VGDESTROYPATH(h)     vgDestroyPath(h)
    
    #define VGCREATEIMAGE(a1, a2, a3, a4) vgCreateImage(a1, a2, a3, a4)
    #define VGDESTROYIMAGE(h)     vgDestroyImage(h)

    #define DECLARE_HANDLECHECKER_GLOBAL()
    #define DECLARE_HANDLECHECKER()
    #define INIT_HANDLECHECKER()
    #define ASSERT_HANDLE_COUNT()

#else
#include <e32debug.h>
    #define NVG_DEBUGP1(x1)                                 RDebug::Printf(x1)
    #define NVG_DEBUGP2(x1, x2)                             RDebug::Printf(x1, x2)
    #define NVG_DEBUGP3(x1, x2, x3)                         RDebug::Printf(x1, x2, x3)
    #define NVG_DEBUGP4(x1, x2, x3, x4)                     RDebug::Printf(x1, x2, x3, x4)
    #define NVG_DEBUGP5(x1, x2, x3, x4, x5)                 RDebug::Printf(x1, x2, x3, x4, x5)
    #define NVG_DEBUGP6(x1, x2, x3, x4, x5, x6)             RDebug::Printf(x1, x2, x3, x4, x5, x6)
    #define NVG_DEBUGP7(x1, x2, x3, x4, x5, x6, x7)         RDebug::Printf(x1, x2, x3, x4, x5, x6, x7)
    #define NVG_DEBUGP8(x1, x2, x3, x4, x5, x6, x7, x8)     RDebug::Printf(x1, x2, x3, x4, x5, x6, x7, x8)
    #define NVG_DEBUGP9(x1, x2, x3, x4, x5, x6, x7, x8, x9) RDebug::Printf(x1, x2, x3, x4, x5, x6, x7, x8, x9)

    #define NVG_DEBUGSTMT(x1)                             do { x1; } while (0)

    struct THandleCounter
        {
        TInt    paintHC;
        TInt    pathHC;
        TInt    imageHC;
        THandleCounter() : paintHC(0), pathHC(0), imageHC(0) {}
        };

inline void incrementPaintHandleCountL(THandleCounter * handleCounter)
    {
    if (handleCounter)
        handleCounter->paintHC++;
    }

inline void decrementPaintHandleCountL(THandleCounter * handleCounter)
    {
    if (handleCounter)
        handleCounter->paintHC--;
    }

inline void decrementImageHandleCountL(THandleCounter * handleCounter)
    {
    if (handleCounter)
        handleCounter->imageHC--;
    }

inline void decrementPathHandleCountL(THandleCounter * handleCounter)
    {
    if (handleCounter)
        handleCounter->pathHC--;
    }

inline void incrementPathHandleCountL(THandleCounter * handleCounter)
    {
    if (handleCounter)
        handleCounter->pathHC++;
    }

inline void incrementImageHandleCountL(THandleCounter * handleCounter)
    {
    if (handleCounter)
        handleCounter->imageHC++;
    }

inline void checkHandleCout(THandleCounter * handleCounter)
    {
    if (handleCounter)
        {
        if (handleCounter->paintHC != 0)
            {
            User::Panic(_L("Paint Handle Deallocation Not Matching"), 1);
            }
        
        if (handleCounter->pathHC != 0)
            {
            User::Panic(_L("Path Handle Deallocation Not Matching"), 1);
            }
        
        if (handleCounter->imageHC != 0)
            {
            User::Panic(_L("Image Handle Deallocation Not Matching"), 1);
            }
        }
    
        delete handleCounter;
        handleCounter = 0;
    }

inline VGHandle vgCreatePaintWrapper(THandleCounter * handleCounter)
    {
    VGPaint pH = vgCreatePaint();
    if (pH)
        {
        incrementPaintHandleCountL(handleCounter);
        }
    return pH;
    }
                                    
inline void vgDestroyPaintWrapper(THandleCounter * handleCounter, VGHandle h)
    {
    if (h)
        {
        vgDestroyPaint(h);
        decrementPaintHandleCountL(handleCounter);
        }
    }
    
    
inline VGHandle vgCreatePathWrapper(THandleCounter * handleCounter, VGint a1,
        VGPathDatatype a2,
        VGfloat a3, VGfloat a4,
        VGint a5,
        VGint a6,
        VGbitfield a7)
    {
    VGPath pH = vgCreatePath(a1, a2, a3, a4, a5, a6, a7);
    if (pH) 
        {
        incrementPathHandleCountL(handleCounter);
        }
    return pH;
    }
   
inline void vgDestroyPathWrapper(THandleCounter * handleCounter, VGHandle h)
    {
    if (h) 
        {
        vgDestroyPath(h);
        decrementPathHandleCountL(handleCounter);
        }
    }
    
inline VGHandle vgCreateImageWrapper(THandleCounter * handleCounter, VGImageFormat a1,
        VGint a2, VGint a3,
        VGbitfield a4)
    {
    VGImage iH = vgCreateImage(a1, a2, a3, a4);
    if (iH) 
        {
        incrementImageHandleCountL(handleCounter);
        }
    return iH;
    }

inline void vgDestroyImageWrapper(THandleCounter * handleCounter, VGHandle h)
    {
    if (h)
        {
        vgDestroyImage(h);
        decrementImageHandleCountL(handleCounter);
        }
    }
    
    
    #define DECLARE_HANDLECHECKER_GLOBAL()  THandleCounter * __handleCounter;
    #define DECLARE_HANDLECHECKER()  private:  THandleCounter * __handleCounter;
    #define INIT_HANDLECHECKER()  __handleCounter = new THandleCounter;
    #define ASSERT_HANDLE_COUNT() checkHandleCout(__handleCounter);

    #define VGCREATEPAINT()     (NVG_DEBUGP3("/*vgCreatePaint() %s:%d*/", __FILE__, __LINE__), vgCreatePaintWrapper(__handleCounter))
    #define VGDESTROYPAINT(h)   (NVG_DEBUGP3("/*vgDestroyPaint() %s:%d*/", __FILE__, __LINE__), vgDestroyPaintWrapper(__handleCounter, h))
    
    #define VGCREATEPATH(a1, a2, a3, a4, a5, a6, a7)  (NVG_DEBUGP3("/*vgCreatePath() %s:%d*/", __FILE__, __LINE__), vgCreatePathWrapper(__handleCounter, a1, a2, a3, a4, a5, a6, a7))
    #define VGDESTROYPATH(h)     (NVG_DEBUGP3("/*vgDestroyPath() %s:%d*/", __FILE__, __LINE__), vgDestroyPathWrapper(__handleCounter, h))
    
    #define VGCREATEIMAGE(a1, a2, a3, a4) (NVG_DEBUGP3("/*vgCreateImage() %s:%d*/", __FILE__, __LINE__), vgCreateImageWrapper(__handleCounter, a1, a2, a3, a4))
    #define VGDESTROYIMAGE(h)     (NVG_DEBUGP3("/*vgDestroyImage() %s:%d*/", __FILE__, __LINE__), vgDestroyImageWrapper(__handleCounter, h))

#endif

#ifdef OPENVG_OBJECT_CACHING

// first one is for openVG object caching, the macro is used for testing purpose
#define COND_COM_OC(obj, s1, s2)       if (obj) { s1; } else { s2; }

// if object caching is enabled take the first one, which is for object caching,
// else the second one
#define COND_COM_OC_NOC(s1, s2)        s1

// if object caching is enabled take the first one, which is for object caching
// else don't do anything
#define COND_COM_OC_OOC(s1)            s1

#else

// first one is for openVG object caching, the macro is used for testing purpose
#define COND_COM_OC(obj, s1, s2)       s2

// if object caching is enabled take the first one, which is for object caching,
// else the second one
#define COND_COM_OC_NOC(s1, s2)        s2

// if object caching is enabled take the first one, which is for object caching
// else don't do anything
// if object caching is enabled take the first one, which is for object caching
// else don't do anything
#define COND_COM_OC_OOC(s1)

// if object caching is enabled take the first one, which is for object caching
// else don't do anything
#endif

GLDEF_C void CleanupArray( TAny * aObj );
GLDEF_C void CleanupTFloatFixArray( TAny * aObj );

/*
 * There is an Align4 function in symbian which does the alignement
 * this is just to check whether the given pointer is aligned or not
 */
template <class T>
inline TBool IsAligned4(T aValue)
    {return !((TUint32)aValue & (sizeof(TUint32) - 1));}

/*
 * There is an Align2 function in symbian which does the alignement
 * this is just to check whether the given pointer is aligned or not
 */
template <class T>
inline TBool IsAligned2(T aValue)
    {return !((TUint16)aValue & (sizeof(TUint16) - 1));}

#endif

