/**
 * @note This class is provided only if module test hooks are set on.
 */

#include "alfmoduletestconf.h"
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF

#ifndef ALFMODULETESTMAP_H
#define ALFMODULETESTMAP_H

#include <e32def.h> 
#include "alfmoduletestitem.h"
#include "alfmoduletesttype.h"


/**
 * Class CAlfModuleTestMap
 * 
 * Provides map functionality for the key-value-pairs.
 * In test cases, this should most likely be used so, that 
 * first test case classes create items with certain keys, for example with handle values.
 * Then, define hooks are used in the code to update values that corresond the correct handles.
 * In the end, test case classes can check that items have correct values set and if the test
 * is passed.
 * 
 * @note When this item is copied, also the contents of the map item array are copied
 *       into new map item array. In other words, copy can be thought as deep copy
 *       instead of just shallow copy of array pointer.
 *       
 * @see CAlfModuleTestItem
 */
template< class T >
NONSHARABLE_CLASS( TAlfModuleTestMap )
    {
public:

    // Maximum item count in the map
    static const TInt KMaxArrayCount = 100;

    // Maximum accepted test type count in the map
    static const TInt KMaxAcceptArrayCount = 20;
    
    
    /**
     * Constructor to initialize variables.
     */
    TAlfModuleTestMap():
        iArrayCount( 0 ),
        iAcceptArrayCount( 0 )
        {
        }

    
    /**
     * @note This will return normal items and also items that are links.
     * 
     * @param aTestType Informs what type of test is accepted. Others are skipped.
     * @param aKey Key of the map item.
     * @return T* Ownership is not transferred.
     *            NULL if item is not found.
     */
    TAlfModuleTestItem< T >* Find( const TAlfModuleTestType& aTestType, TInt aKey )
        {
        // Try to find the item corresponding the given key.
        for ( TInt i = 0; i < iArrayCount; ++i )
            {
            TAlfModuleTestItem< T >& testItem( iArray[ i ] );
            if ( testItem.Key() == aKey
                 && testItem.TestTypeMatch( aTestType ) )
                {
                return &( testItem );
                }
            }
        // Item corresponding the given key was not found.
        return NULL;
        }    

    
    /**
     * Finds the actual item.
     * 
     * @note If the given key identifies a link item, 
     *       then search is continued to the actual item
     *       through the link chain.
     *       Link item is not returned here.
     * 
     * @param aTestType Test type of the linked item and link item.
     * @param aKey Key of the item. This can also be a key to a link item.
     * @return TAlfModuleTestItem< T >* Actual item. 
     *                                  NULL if item was not found.
     *                                  Ownership is not transferred.
     */
    TAlfModuleTestItem< T >* FindActual( const TAlfModuleTestType& aTestType, TInt aKey )
        {
        TAlfModuleTestItem< T >* item( Find( aTestType, aKey ) );
        while ( item && item->LinkTargetKey() != KErrNotFound )
            {
            // Item is link.
            // So, find the actual item through link targets.
            item = Find( aTestType, item->LinkTargetKey() );
            }
        return item;
        }
    
    
    /**
     * Function to append new item into the map.
     * 
     * @param aTestType Describes for what case the appended test item is created for.
     * @param aKey Key of the map item.
     * @param aDefaultValue Default value for the map item.
     * @return TInt System wide error code.
     */    
    TInt Append( const TAlfModuleTestType& aTestType, TInt aKey, const T& aDefaultValue )
        {
        if ( !Accept( aTestType ) )
            {
            // Given test type has not been set as accepted.
            return KErrNotSupported;
            }
        else if ( Find( aTestType, aKey ) )
            {
            // Item already exists in the map.
            return KErrAlreadyExists;
            }        
        else if ( iArrayCount == KMaxArrayCount )
            {
            // Array already full.
            return KErrOverflow;
            }
        
        // Append new item into the array.
        iArray[ iArrayCount ] = TAlfModuleTestItem< T >( aTestType, aKey, aDefaultValue );
        ++iArrayCount;
        return KErrNone;        
        }

    
    /**
     * Creates and appends link item that links to another item.
     * 
     * @param aTestType Test type of the linked item and link item.
     * @param aLinkKey Link item key.
     * @param aTargetKey Target item key.
     * @return TInt System wide error code.
     */
    TInt AppendLink( const TAlfModuleTestType& aTestType, TInt aLinkKey, TInt aTargetKey )
        {
        if ( !Accept( aTestType ) )
            {
            // Given test type has not been set as accepted.
            return KErrNotSupported;
            }
        else if ( Find( aTestType, aLinkKey ) )
            {
            // Link item already exists in the map.
            return KErrAlreadyExists;
            }
        else if ( !FindActual( aTestType, aTargetKey ) )
            {
            // Link has to point to another existing item.
            // Also, link has to point to the chain that ends to actual item.
            // Then, links will not create forever loops. Loops could occur
            // if links in chain point to each other creating loops.
            return KErrNotFound;
            }
        else if ( iArrayCount == KMaxArrayCount )
            {
            // Array already full.
            return KErrOverflow;
            }
        
        // Create link item because original item exists and link chain is correct.
        // Append new link item into the array and link it to another link or to
        // actual item.
        TAlfModuleTestItem< T >* target( Find( aTestType, aTargetKey ) );
        // Set value of the link same as its target's value.
        iArray[ iArrayCount ] = TAlfModuleTestItem< T >( aTestType, aLinkKey, target->Value() );
        iArray[ iArrayCount ].SetLinkTargetKey( target->Key() );
        ++iArrayCount;
        return KErrNone;            
        }

    
    /**
     * Sets the value for the actual item.
     * Item itself should already appended into map and exist
     * in the array before setting its value here.
     * 
     * @note Some items may be links. But, only the value of the
     *       actual item in the end of the link chain is set here.
     * 
     * @param aTestType Describes for what case the item is for.
     * @param aKey Key of the map item.
     * @param aValue Value for the map item.
     * @return TInt System wide error code.
     */
    TInt SetActualValue( const TAlfModuleTestType& aTestType, TInt aKey, const T& aValue )
        {
        TAlfModuleTestItem< T >* item( FindActual( aTestType, aKey ) );
        if ( !item )
            {
            // Actual item was not found from the array.        
            return KErrNotFound;            
            }
        
        // Actual item exists and it is not link item. So, set its values.
        item->SetValue( aValue );        
        return KErrNone;
        }

    
    /**
     * Find an actual item if it exists and gets its value if item is found
     * and value has been set.
     * 
     * @param aTestType Describes for what case the item is for.
     * @param aKey Key of the map item.
     * @param aValue If map item is found, value of the item is set here.
     * @param aAcceptDefault ETrue if an existing object having its default value
     *                       is accepted. EFalse if value should have been updated
     *                       separately.
     * @return TBool ETrue if map item is found and value has been set. Else EFalse. 
     */    
    TBool GetActualValue( 
        const TAlfModuleTestType& aTestType, TInt aKey, T& aValue, TBool aAcceptDefault )
        {
        const TAlfModuleTestItem< T >* item( FindActual( aTestType, aKey ) );
        if ( item && ( aAcceptDefault || item->ValueSetCount() > 0 ) )
            {
            aValue = item->Value();
            return ETrue;
            }
        return EFalse;
        }


    /**
     * Finds an actual item if it exists and checks if it equals the given value.
     * 
     * @param aTestType Describes for what case the item is for.
     * @param aKey Key of the map item.
     * @param aValue Value of the map item.
     * @return TBool ETrue if item is found and its value equals given value. Else EFalse. 
     */
    TBool ActualEquals( const TAlfModuleTestType& aTestType, TInt aKey, const T& aValue )
        {
        T value( aValue );
        if ( GetActualValue( aTestType, aKey, value, ETrue ) && value == aValue )
            {
            return ETrue;
            }
        return EFalse;
        }


    /**
     * @return TInt Number of map items
     */
    TInt ItemCount() const
        {
        return iArrayCount;
        }


    /**
     * @return const TAlfModuleTestItem< T >& Reference to the map item
     */
    const TAlfModuleTestItem< T >& Item( TInt aIndex ) const
        {
        return iArray[ aIndex ];
        }

    
    /**
     * Appends an accepted test type into the accepted array.
     * 
     * @note Only items of accepted test type can be appended
     *       into this map.
     * 
     * @param aTestType Test item type that is accepted for this map.
     * @return TInt System wide error code.
     */
    TInt AppendAccept( const TAlfModuleTestType& aTestType )
        {
        if ( Accept( aTestType ) )
            {
            // Type already exists in the array.
            return KErrAlreadyExists;
            }
        else if ( iAcceptArrayCount == KMaxAcceptArrayCount )
            {
            // Array already full.
            return KErrOverflow;
            }

        iAcceptArray[ iAcceptArrayCount ] = aTestType;
        ++iAcceptArrayCount;
        return KErrNone;
        }
    
    
    /**
     * @param aTestType Test item type to be checked.
     * @return TBool ETrue if given test type is set as accepted for this map.
     *               Else EFalse.
     */
    TBool Accept( const TAlfModuleTestType& aTestType ) const
        {
        for ( TInt i = 0; i < iAcceptArrayCount; ++i )
            {
            if ( aTestType == iAcceptArray[ i ] )
                {
                return ETrue;
                }
            }
        return EFalse;
        }


    /**
     * Resets all the array items that match the given test type
     * to given default value.
     * 
     * @param aTestType The test type of items that should be resetted.
     * @param aValue Reference to the value that items are resetted to.
     */
    void ResetItems( const TAlfModuleTestType& aTestType, const T& aDefaultValue )
        {
        for ( TInt i = 0; i < iArrayCount; ++i )
            {
            if ( iArray[ i ].TestTypeMatch( aTestType ) )
                {
                iArray[ i ].Reset( aDefaultValue );
                }
            }
        }


    /**
     * Resets the map
     */
    void Reset()
        {
        // Just reset the counter.
        // We do not bother to reset map items, because when counter is reseted
        // already set items and their info is left out of the scope.
        iArrayCount = 0;
        iAcceptArrayCount = 0;
        }

    
private: // data
    
    TAlfModuleTestItem< T > iArray[ KMaxArrayCount ];
    TInt iArrayCount;
    
    TAlfModuleTestType iAcceptArray[ KMaxAcceptArrayCount ];
    TInt iAcceptArrayCount;
    
    };

#endif // ALFMODULETESTMAP_H

#endif // USE_MODULE_TEST_HOOKS_FOR_ALF

// End of File
