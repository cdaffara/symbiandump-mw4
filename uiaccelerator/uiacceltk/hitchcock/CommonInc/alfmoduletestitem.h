/**
 * @note This class is provided only if module test hooks are set on.
 */

#include "alfmoduletestconf.h"
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF

#ifndef ALFMODULETESTITEM_H
#define ALFMODULETESTITEM_H

#include <e32def.h> 
#include "alfmoduletesttype.h"


/**
 * CAlfModuleTestItem
 * 
 * Provides key-value pair that is used in TAlfModuleTestMap.
 */
template< class T >
NONSHARABLE_CLASS( TAlfModuleTestItem )
    {
    
public:

    /**
     * Default constructor
     * 
     * This is provided for copy operations that may required default constructor.
     * For normal cases, use another constructor instead to initialize the item
     * with correct values.
     */
    TAlfModuleTestItem():
        iTestType( EAlfModuleTestTypeNone ),    
        iKey( 0 ),
        iValueSetCount( 0 ),
        iLinkTargetKey( KErrNotFound )
        {    
        }


    /**
     * Constructor to initialize variables.
     * 
     * @param aTestType Defines for what this test item is meant for.
     * @param aKey Key that identifies the item.
     *             In test cases this could be for example handle.
     * @param aDefaultValue Default value for the map item.
     */
    TAlfModuleTestItem( const TAlfModuleTestType& aTestType, TInt aKey, const T& aDefaultValue ):
        iTestType( aTestType ),
        iKey( aKey ),
        iValue( aDefaultValue ),
        iValueSetCount( 0 ),
        iLinkTargetKey( KErrNotFound )
        {
        }

    
    /**
     * @note Be carefull when comparing items because some items may
     * be link items instead of original items.
     * 
     * @param aTestType Test type of accepted item.
     * @param aValue Item value to be compared.
     * @return ETrue if given object equals the value of this item.
     *         Else EFalse.
     */
    TBool Equals( const TAlfModuleTestType& aTestType, const T& aValue ) const
        {
        // Also, check that value has been set. If it has not been set,
        // then think objects as unequals.
        return ( iValueSetCount > 0 
                 && iValue == aValue 
                 && TestTypeMatch( aTestType ) );
        }

    
    /**
     * @return TInt Key that should be set during creation of this object.
     */
    TInt Key() const
        {
        return iKey;
        }

    
    /**
     * @see TAlfModuleTestItem::ValueSetCount to check if the value has already been set.
     * 
     * @return const T& Value that corresonds the key.
     */
    const T& Value() const
        {
        return iValue;
        }
    
    
    /**
     * @param aValue Value to be set for the key
     */
    void SetValue( const T& aValue )
        {
        iValue = aValue;
        ++iValueSetCount;
        }

    
    /**
     * @return TInt Informs how many times the value has been set. 
     */
    TInt ValueSetCount() const
        {
        return iValueSetCount;
        }
    
    
    /**
     * @return const TAlfModuleTestType& Defines what the test is for.
     */
    const TAlfModuleTestType& TestType() const
        {
        return iTestType;
        }

    
    /**
     * @param aTestType Type of the test this item is for.
     * @return TBool ETrue if flag matches this item. Else EFalse.
     */
    TBool TestTypeMatch( const TAlfModuleTestType& aTestType ) const
        {
        return ( EAlfModuleTestTypeAll == aTestType
                 || EAlfModuleTestTypeAll == iTestType
                 || iTestType == aTestType );
        }

    
    /**
     * @param aLinkTargetKey Key of the item that this item links to. 
     *                       KErrNotFound if this is not a link item.
     */
    void SetLinkTargetKey( TInt aLinkTargetKey )
        {
        iLinkTargetKey = aLinkTargetKey;
        }

    
    /**
     * @return TInt Key of the item that this item links to. 
     *              KErrNotFound if this is not a link item.
     */    
    TInt LinkTargetKey() const
        {
        return iLinkTargetKey;
        }

    
    /**
     * Resets this item to the given default value and 
     * sets value set count to zero.
     * 
     * @note Type, key and link target remain unchanged.
     * 
     * @param aDefaultValue Default value for the item.
     */
    void Reset( const T& aDefaultValue )
        {
        iValue = aDefaultValue;
        iValueSetCount = 0;
        }

    
private: // data    

    TAlfModuleTestType iTestType;
    TInt iKey;
    T iValue;
    TInt iValueSetCount;
    TInt iLinkTargetKey;
    
    };

#endif // ALFMODULETESTITEM_H

#endif // USE_MODULE_TEST_HOOKS_FOR_ALF

// End of File
