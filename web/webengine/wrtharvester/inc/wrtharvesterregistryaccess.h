/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef WRHARVESTERREGISTRYACCESS_H
#define WRHARVESTERREGISTRYACCESS_H 

// INCLUDES
#include <WidgetRegistryClient.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CWidgetInfo;

// CONSTANTS
template < class T > class RWrtArray : public RPointerArray< T >
    {
    public:
        inline RWrtArray() : RPointerArray< T >()
            {
            };
        inline void PushL()
            {
            TCleanupItem item( DoCleanup, this );
            CleanupStack::PushL( item );
            }
            
        inline void ResetAll()
            {
            for ( TInt i( this->Count() - 1 ); i >= 0; --i )
                {
                delete (*this)[ i ];
                }
            this->Close();
            }
        
    private:
        static void DoCleanup( TAny* aPtr )
            {
            __ASSERT_DEBUG( aPtr, User::Invariant() );
            RWrtArray< T >* ptr( reinterpret_cast< RWrtArray< T >* >( aPtr ) );
            for ( TInt i( ptr->Count() - 1 ); i >= 0; --i )
                {
                delete ( *ptr )[ i ];
                }
            ptr->Close();
            }
    };

class CWrtInfo : public CBase
    {
    public:
        /**
        * Constructor
        */
        inline CWrtInfo()
            {
            iUid = TUid::Uid(0);
            iDisplayName = NULL;
            iBundleId = NULL;
            }

        /**
        * Destructor.
        */
        inline virtual ~CWrtInfo()
            {
            delete iDisplayName;
            delete iBundleId;
            }

    public:
        TUid        iUid;
        HBufC*      iDisplayName;// widget display name
        HBufC*      iBundleId; //  widget bundle identifier
    };

// CLASS DECLARATION

/**
 *  Widget Registry interface.
 *
 *  Handles communication & widget bookkeeping of miniview-capable widgets.
 *
 *  @lib wrtharvester.dll
 *  @since S60 S60 v5.0
 */
class WrtHarvesterRegistryAccess
	{
	public:
        /**
        * Default constructor.
        */
        WrtHarvesterRegistryAccess();
        
        /**
        * Destructor.
        */
        ~WrtHarvesterRegistryAccess();
        
        /**
	    * Get widget bundle ids for widgets supporting miniviews.
	    *
	    * NOTE: Ownership of pointers in the array is not transferred!
	    * Caller must not delete them.
	    * 
	    * @param aArray Array where the descriptor pointers are to be stored.
	    */
	    void WidgetInfosL( RWrtArray< CWrtInfo >& aWidgetInfoArray );
	    
	private:
        /**
        * Check if the widget support miniview.
        * 
        * @param aSession Widget registry session
        * @param aUid UID of widget.
        * @return Yes or no.
        */
        TBool SupportsMiniviewL( RWidgetRegistryClientSession& aSession, const TUid& aUid );
        
        /**
        * Returns the property value for the widget as a string. Ownership transferred.
        * 
        * @param aSession Widget registry session
        * @param aUid UID of widget.
        * aPropertyId Id of the property.
        * @return Identifier in a descriptor.
        */
        HBufC* WidgetPropertyL( RWidgetRegistryClientSession& aSession, const TUid& aUid, TWidgetPropertyId aPropertyId  );
        


    };

#endif // WRHARVESTERREGISTRYACCESS_H 
