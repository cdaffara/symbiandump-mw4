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
* Description:   ?Description
*
*/



#ifndef __HUIOWNEDPOINTER_H__
#define __HUIOWNEDPOINTER_H__


/* Types. */

/** Ownership options. */
enum THuiOwnership
    {
    EHuiDoesNotHaveOwnership,
    EHuiHasOwnership
    };


/**
 * Pointer that optionally has ownership.
 */
template< class T >
class RHuiOwnedPointer
    {
public:

    /* Constructors and destructor. */

    /**
     * Default constructor.
     */
    RHuiOwnedPointer()
            : iOwnership(EHuiDoesNotHaveOwnership), iPointer(0)
        {
        }
        
    RHuiOwnedPointer(T* aPointer, THuiOwnership aOwnership)
            : iOwnership(aOwnership), iPointer(aPointer)
        {
        }

    /**
     * Destructor. The object is deleted if the pointer has ownership.
     */        
    virtual ~RHuiOwnedPointer()
        {
        Close();
        }
        

    /* Methods. */        

    /**
     * Reset the pointer.
     */
    inline void Close()
        {
        if(iOwnership == EHuiHasOwnership)
            {
            delete iPointer;
            }
        iPointer = 0;
        iOwnership = EHuiDoesNotHaveOwnership;            
        }
        
    inline operator T* ()
        {
        return iPointer;
        }
        
    inline operator const T* () const
        {
        return iPointer;
        }
        
    inline T* operator -> ()
        {
        return iPointer;
        }
        
    inline const T* operator -> () const
        {
        return iPointer;
        }
        
    inline T* Ptr()
        {
        return iPointer;
        }
        
    inline const T* Ptr() const
        {
        return iPointer;
        }
        
    inline T& Ref()
        {
        return *iPointer;
        }
        
    inline const T& Ref() const
        {
        return *iPointer;
        }

    inline T& NonConstRef() const
        {
        return *iPointer;
        }

    inline void Set(T* aPointer, THuiOwnership aOwnership)
        {
        // Delete the previous object.
        if(iOwnership == EHuiHasOwnership)
            {
            delete iPointer;
            iPointer = 0;
            }
            
        iOwnership = aOwnership;
        iPointer = aPointer;
        }
        
    inline THuiOwnership Ownership() const
        {
        return iOwnership;
        }
        
        
private:

    /**
     * Assignment prohibited. This isn't defined anywhere.
     */
    RHuiOwnedPointer& operator = (const RHuiOwnedPointer& aOwnedPointer);
        
        
private:

    /** Have ownership of the object being pointed to. If the owned pointer
        is deleted, the object will be deleted as well. */
    THuiOwnership iOwnership;    
    
    T* iPointer;
    
    };


#endif // __HUIOWNERSHIPPOINTER_H__
