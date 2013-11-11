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



#ifndef __HUI_OBSERVER_ARRAY_H__
#define __HUI_OBSERVER_ARRAY_H__

#include <uiacceltk/HuiPanic.h>

/**
 * Observer array template.
 */
template< class T >
class RHuiObserverArray
    {
public:
    RHuiObserverArray()
        {
        }

    void Reset()
        {
        iObservers.Reset();
        }
        
    void Close()
        {
        iObservers.Close();
        }        
    
    void AppendL(T& aObserver)
        {
        __ASSERT_ALWAYS(iObservers.Find(&aObserver) == KErrNotFound,
                        THuiPanic::Panic(THuiPanic::EDuplicateObserver));
        User::LeaveIfError( iObservers.Append(&aObserver) );
        }

    void AppendIfNotFoundL(T& aObserver)
        {
        if(iObservers.Find(&aObserver) == KErrNotFound)
            {
            User::LeaveIfError( iObservers.Append(&aObserver) );
            }
        }
        
    void Remove(T& aObserver)
        {
        TInt index = iObservers.Find(&aObserver);
        if(index == KErrNotFound)
            {
            THuiPanic::Panic(THuiPanic::EObserverNotFound);
            }
        iObservers.Remove(index);
        }

    void RemoveIfFound(T& aObserver)
        {
        TInt index = iObservers.Find(&aObserver);
        if(index != KErrNotFound)
            {
            iObservers.Remove(index);
            }
        }

    inline TInt Count() const
        {
        return iObservers.Count();
        }
        
    inline T& operator[] (TInt aIndex) const
        {
        return *iObservers[aIndex];
        }


private:

    /* Private methods. */
    
    /**
     * Assignment is not allowed.  
     */
    RHuiObserverArray& operator = (const RHuiObserverArray& aObserverArray);
    
    
private:
    RArray<T*> iObservers;
    TInt iSpare1;
    TInt iSpare2;
    };
    

#endif // __HUI_OBSERVER_ARRAY_H__
