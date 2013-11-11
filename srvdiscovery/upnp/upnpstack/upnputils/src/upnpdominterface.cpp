/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  UpnpDomInterface
*
*/


// INCLUDE FILES
#include <e32def.h>
#include <e32math.h>
#include <f32file.h>
#include <s32file.h>
#include <e32std.h>

#define KLogFile _L("DLNAWebServer.txt")

#include "upnpdominterface.h"
#include "upnpstring.h"
#include "upnpcustomlog.h"


#include <xml/dom/xmlengdom.h>
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengserializationoptions.h>



// ============================ MEMBER FUNCTIONS ===============================

namespace UpnpDomInterface
    {

// -----------------------------------------------------------------------------
// UpnpDomInterface::GetAttrValueL
// Gets the element attribute's value
// -----------------------------------------------------------------------------
//
	EXPORT_C TPtrC8 GetAttrValueL( const TXmlEngElement& aElement,
                    	      	   const TDesC8& aAttribute )	
		{
		if( aElement.NotNull() )
			{
			
			TPtrC8 val = aElement.AttributeValueL( aAttribute );
			
			if( val.Length() )	
				{
				// Cleanup
				return val;
				}
			
			// Cleanup
			return KNullDesC8();
		
			}
			
		return KNullDesC8();
		}
	
// -----------------------------------------------------------------------------
// UpnpDomInterface::GetElementValueL
// Gets value of the indicated element.
// -----------------------------------------------------------------------------
//
	EXPORT_C TPtrC8 GetElementValueL( const TXmlEngElement& aElement )
		{
		if( aElement.IsNull() )
			User::Leave( KErrNotFound );
		
		if( !aElement.Value().Length() )
			return KNullDesC8();
		
		return aElement.Value();
		
		}
    

// -----------------------------------------------------------------------------
// UpnpDomInterface::AllocElementValueL
// Gets value of the indicated element and allocs it on heap.
// -----------------------------------------------------------------------------
//
	EXPORT_C HBufC* AllocElementValueL( const TXmlEngElement& aElement )
		{
		if( aElement.IsNull() )
			User::Leave( KErrNotFound );
		
		if( !aElement.Value().Length() )
			return NULL;
		
		HBufC* ret = HBufC::NewL(aElement.Value().Length());
		ret->Des().Copy(aElement.Value());
		return ret;
		}
    
// -----------------------------------------------------------------------------
// UpnpDomInterface::CheckTagL
// Checks the element's tag
// -----------------------------------------------------------------------------
//
	EXPORT_C TBool CheckTagL( const TXmlEngElement& aElement,
                    	      const TDesC8& aTag )
		{
            		
        // Check the local name against the wanted tag.
		TInt difference = aElement.Name().Compare( aTag );
		
		// Check the name of the element, 
        // if it equals to Tag (the wanted value) 
        if ( difference == 0 )
      		return ETrue;
        else 
        	return EFalse;
          				
		}

// -----------------------------------------------------------------------------
// UpnpDomInterface::CheckTagL
// Checks the element's tag
// -----------------------------------------------------------------------------
//
	EXPORT_C TBool CheckTagL( const TXmlEngElement& aElement,
                       		  const TDesC8& aTag,
                       		  const TDesC8& aTagValue )
		{
		
		// If tag name equals
		if( CheckTagL( aElement, aTag ) )
			{
			 
	        TInt difference = aElement.Value().Compare( aTagValue );
	            	
             // If value also equals then that's the tag
             // we're looking for
             if ( difference == 0 || ( !aElement.Value().Length() && !aTagValue.Length() ) )
      		   	return ETrue;
                    	
			}
		
		return EFalse;  				
		}

// -----------------------------------------------------------------------------
// UpnpDomInterface::CheckSubTagL
// Check tag.
// -----------------------------------------------------------------------------
//            
    EXPORT_C TBool CheckSubTagL( const TXmlEngElement& aElement,
                                 const TDesC8& aTag )
        {
        
        // Go through the elements of the current level.   
        if ( aElement.NotNull() )
            {
          	
			RXmlEngNodeList<TXmlEngElement> childElements;
			CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
				 
				// Check the name of the sub element, 
                // if it equals to Tag (the wanted value) 
                if ( CheckTagL( childElements.Next(), aTag ) )
                    {
                    CleanupStack::PopAndDestroy(&childElements);
      		        return ETrue;
                    }				
				}
			CleanupStack::PopAndDestroy(&childElements);
            }

        return EFalse;
        }

// -----------------------------------------------------------------------------
// UpnpDomInterface::CheckSubTagL
// Check sub tag.
// -----------------------------------------------------------------------------
//          
    EXPORT_C TBool CheckSubTagL( const TXmlEngElement& aElement,
                                 const TDesC8& aTag,
                                 const TDesC8& aTagValue )
        {
        
        // Go through the elements of the current level.   
        if ( aElement.NotNull() )
            {
          	
			RXmlEngNodeList<TXmlEngElement> childElements;
			CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
				
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();
				
				if( CheckTagL( sub, aTag, aTagValue ) )
				    {
				    CleanupStack::PopAndDestroy(&childElements);
					return ETrue;
				    }
			
				}
			CleanupStack::PopAndDestroy(&childElements);
            }

		return EFalse;
        }

// -----------------------------------------------------------------------------
// UpnpDomInterface::CheckSubTagPrefixL
// Check sub tag prefix.
// -----------------------------------------------------------------------------
//              
    EXPORT_C TBool CheckSubTagPrefixL( const TXmlEngElement& aElement,
                                       const TDesC8& aNameSpacePrefix,
                                       const TDesC8& aTag,
                                       const TDesC8& aTagValue )
        {
        
      	// Go through the elements of the current level.   
        if ( aElement.NotNull() )
            {
          	
			RXmlEngNodeList<TXmlEngElement> childElements;
			CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
				
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();
				
				if( CheckTagL( sub, aTag, aTagValue ) )
					{
					
					TXmlEngNamespace ns = sub.LookupNamespaceByPrefixL( aNameSpacePrefix );
					
					//Does the prefix match or is there is no prefix if empty string was passed
					if( ns.NotNull() || ( ns.IsNull() && aNameSpacePrefix.Length() == 0 ) )
						{
						CleanupStack::PopAndDestroy(&childElements);
						return ETrue;							
						}
					}
                }
            CleanupStack::PopAndDestroy(&childElements);
            }
        
        return EFalse;
        
        }

// -----------------------------------------------------------------------------
// UpnpDomInterface::CheckAttributeValueL
// Check attribute value.
// -----------------------------------------------------------------------------
//            
    EXPORT_C TBool CheckAttributeValueL( const TXmlEngElement& aElement,
                                         const TDesC8& aAttribute,
                                         const TDesC8& aValue )
        {
        if ( aElement.NotNull() )
            {
            
            // Check existence and value of the argument
         	TInt difference = aElement.AttributeValueL( aAttribute ).Compare( aValue );
 
            if(!difference)
            	return ETrue;
            else
            	return EFalse;

            }

        return EFalse;
        }

// -----------------------------------------------------------------------------
// UpnpDomInterface::DeleteElement
// Delete DOM element from the tree.
// -----------------------------------------------------------------------------
//    
    EXPORT_C TXmlEngElement DeleteElement(TXmlEngElement& aElement)
        {
        
		if( aElement.IsNull() )
            {
            TXmlEngElement nullElement;
            return nullElement;
            }
            
        TXmlEngElement parent = aElement.ParentNode().AsElement();
		
		aElement.Remove();
		       
        return parent;
        }
        
// -----------------------------------------------------------------------------
// UpnpDomInterface::GetElementL
// Get element.
// -----------------------------------------------------------------------------
//  
    EXPORT_C TBool GetElementL( const TXmlEngElement& aElement, 
                                TXmlEngElement& aWanted, 
                                const TDesC8& aMainTag, 
                                TInt aDepth, 
                                TInt aCurrentDepth )
        {

        if ( aElement.NotNull() )
            {
            aCurrentDepth++;
			
			RXmlEngNodeList<TXmlEngElement> childElements;
			CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
				
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();
				 
				// Check the name of the element, 
                // if it equals to MainTag (the wanted value) 
                if ( CheckTagL( sub, aMainTag ) )
                    {
                    aWanted = sub;
                    CleanupStack::PopAndDestroy(&childElements);
      		        return ETrue;
                    }				
				// Else go deeper into the tree. 
                else if ( aDepth < 0 || aDepth > aCurrentDepth )
                    {
	    	     	               
                    if ( GetElementL( sub,
                                     aWanted,
                                     aMainTag, 
                                     aDepth, 
                                     aCurrentDepth ) 
                                     == ( TBool ) ETrue )
                        
                        {
                        CleanupStack::PopAndDestroy(&childElements);
                        return ETrue;
                        }
                    }
                else
                    {
                    }
				}
			CleanupStack::PopAndDestroy(&childElements);
            }

        return EFalse;
        }
    
// -----------------------------------------------------------------------------
// UpnpDomInterface::GetElementL
// Get element.
// -----------------------------------------------------------------------------
//       
    EXPORT_C TBool GetElementL( const TXmlEngElement& aElement, 
    						    TXmlEngElement& aWanted,
                                const TDesC8& aMainTag,
                                const TDesC8& aSubTag,
                                TInt aDepth,
                                TInt aCurrentDepth )
        {
        
        if ( aElement.NotNull() )
            {
            
            // Add one Depth level to the counter 
            // (affects only current and deeper levels) 
            aCurrentDepth++;
			
			RXmlEngNodeList<TXmlEngElement> childElements;
			CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
				
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();

				// Check the name of the element, 
                // if it equals to MainTag (the wanted value) 
                if ( CheckTagL( sub, aMainTag ) )
                    {
                    
                    // Check that it holds the wanted subtag 
                    // and that the subtags value is the wanted one. 
                    if ( CheckSubTagL( sub, aSubTag ) == ( TBool ) ETrue )
                        {
                        aWanted = sub;
                        CleanupStack::PopAndDestroy(&childElements);
      		        	return ETrue;
                        }
                    }		
                    		
				// Else go deeper into the tree. 
                else
                    {

                    if ( aDepth < 0 || aDepth > aCurrentDepth )
                        {

                        if ( GetElementL( sub,
                                         aWanted,
                                         aMainTag,
                                         aSubTag,
                                         aDepth,
                                         aCurrentDepth ) 
                                         == ( TBool )ETrue )
                            {
                            CleanupStack::PopAndDestroy(&childElements);
                            return ETrue;
                            }
                        }
                    }
				}
		    CleanupStack::PopAndDestroy(&childElements);
            }
            
        return EFalse;
        }
    

// -----------------------------------------------------------------------------
// UpnpDomInterface::GetElementL
// Get element.
// -----------------------------------------------------------------------------
//            
    EXPORT_C TBool GetElementL( const TXmlEngElement& aElement, 
    						    TXmlEngElement& aWanted,
                                const TDesC8& aMainTag,
                                const TDesC8& aSubTag,
                                const TDesC8& aSubTagValue, 
                                TInt aDepth, TInt aCurrentDepth )
        {
        
        if ( aElement.NotNull() )
            {
            
            // Add one Depth level to the counter 
            // (affects only current and deeper levels) 
            aCurrentDepth++;
			
			RXmlEngNodeList<TXmlEngElement> childElements;
			CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			CleanupStack::Check(&childElements);
			while( childElements.HasNext() )
				{
				
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();

				// Check the name of the element, 
                // if it equals to MainTag (the wanted value) 
                if ( CheckTagL( sub, aMainTag ) )
                    {
                    CleanupStack::Check(&childElements);
                    // Check that it holds the wanted subtag and 
                    // that the subtags value is the wanted one. 
                    if ( CheckSubTagL( sub,
                                      aSubTag,
                                      aSubTagValue ) 
                                      == ( TBool ) ETrue )
                        {
                        aWanted = sub;
                        CleanupStack::PopAndDestroy(&childElements);
      		        	return ETrue;
                        }
                    }		
                    		
				// Else go deeper into the tree. 
                else
                    {

                    if ( aDepth < 0 || aDepth > aCurrentDepth )
                        {

                        if ( GetElementL( sub,
                                         aWanted, 
                                         aMainTag,
                                         aSubTag,
                                         aSubTagValue,
                                         aDepth,
                                         aCurrentDepth )
                                         == ( TBool ) ETrue )
                            {
                            CleanupStack::PopAndDestroy(&childElements);
                            return ETrue;
                            }
                        }
                    }
				}
			CleanupStack::PopAndDestroy(&childElements);
            }
            
        return EFalse;
        }


// -----------------------------------------------------------------------------
// UpnpDomInterface::GetContentElementL
// Get content element.
// -----------------------------------------------------------------------------
//        
    EXPORT_C TBool GetContentElementL( const TXmlEngElement& aElement, 
    								   TXmlEngElement& aWanted,
                                       const TDesC8& aSubTag,
                                       const TDesC8& aSubTagValue,
                                       TInt aDepth, 
                                       TInt aCurrentDepth )
        {
		
		if ( aElement.NotNull() )
        	{
            
            // Add one Depth level to the counter 
            // (affects only current and deeper levels) 
            aCurrentDepth++;
			
			RXmlEngNodeList<TXmlEngElement> childElements;
			CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
				
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();

                if ( CheckTagL( sub, aSubTag, aSubTagValue ) )
                    {
                    aWanted = sub;
                    CleanupStack::PopAndDestroy(&childElements);
      		        return ETrue;
                    }				
				// Else go deeper into the tree. 
                else if ( aDepth < 0 || aDepth > aCurrentDepth )
                    {
	    	     	               
                     if ( GetContentElementL( sub,
                                             aWanted,
                                             aSubTag,
                                             aSubTagValue,
                                             aDepth,
                                             aCurrentDepth ) 
                                               == ( TBool ) ETrue )
                     	{
                     	CleanupStack::PopAndDestroy(&childElements);
						return ETrue;
                        }

                    }
                else
                    {
                    }
				}
			CleanupStack::PopAndDestroy(&childElements);
            }

        return EFalse;
        }

// -----------------------------------------------------------------------------
// UpnpDomInterface::GetDirectoryElementL
// Get directory element.
// -----------------------------------------------------------------------------
//    
    EXPORT_C TBool GetDirectoryElementL( const TXmlEngElement& aElement,
                                         TXmlEngElement& aWanted,
                                         const TDesC8& aMainTag,
                                         const TDesC8& aAttribute,
                                         const TDesC8& aAttributeValue, 
                                         TInt aDepth, 
                                         TInt aCurrentDepth)
        
        {
     
       	if ( aElement.NotNull() )
        	{
            
            // Add one Depth level to the counter 
            // (affects only current and deeper levels) 
            aCurrentDepth++;
			
			RXmlEngNodeList<TXmlEngElement> childElements;
			CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
				
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();

                if ( CheckTagL( sub, aMainTag ) )
                    {
                    if ( CheckAttributeValueL( sub,aAttribute,aAttributeValue ) )
                        {
                        aWanted = sub;
                        CleanupStack::PopAndDestroy(&childElements);
                        return ETrue;
                        }

                    }				
				// Else go deeper into the tree. 
                else
                    {

                    if ( aDepth < 0 || aDepth > aCurrentDepth )
                        {

                        if ( GetDirectoryElementL( sub,
                                                  aWanted, 
                                                  aMainTag, 
                                                  aAttribute, 
                                                  aAttributeValue, 
                                                  aDepth, 
                                                  aCurrentDepth ) == ( TBool ) ETrue )
                            {
                            CleanupStack::PopAndDestroy(&childElements);
                            return ETrue;
                            }
                        }
                    }            
				}
			CleanupStack::PopAndDestroy(&childElements);
            }
                
        return EFalse;
        }

// -----------------------------------------------------------------------------
// UpnpDomInterface::GetDirectoryElementL
// Get directory element.
// -----------------------------------------------------------------------------
//      
    EXPORT_C TBool GetDirectoryElementL( const TXmlEngElement& aElement,
                                         TXmlEngElement& aWanted,
                                         const TDesC8& aAttribute,
                                         const TDesC8& aAttributeValue, 
                                         TInt aDepth, 
                                         TInt aCurrentDepth)
        
        {
        
       	if ( aElement.NotNull() )
        	{
            
            // Add one Depth level to the counter 
            // (affects only current and deeper levels) 
            aCurrentDepth++;
			
			RXmlEngNodeList<TXmlEngElement> childElements;
			CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
				
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();
				
				if ( CheckAttributeValueL( sub, 
                                          aAttribute, 
                                          aAttributeValue ) ==( TBool ) ETrue )
                    {
                    aWanted = sub;
                    CleanupStack::PopAndDestroy(&childElements);
                    return ETrue;
                    }

                if ( aDepth < 0 || aDepth > aCurrentDepth )
                    {

                    if ( GetDirectoryElementL( sub, 
                                              aWanted, 
                                              aAttribute, 
                                              aAttributeValue, 
                                              aDepth, 
                                              aCurrentDepth ) == ( TBool ) ETrue )
                        
                        {
                        CleanupStack::PopAndDestroy(&childElements);
                        return ETrue;
                        }
                    }
                
				}
			CleanupStack::PopAndDestroy(&childElements);
            }
        
        return EFalse;
        }

// -----------------------------------------------------------------------------
// UpnpDomInterface::GetElementListL
// Get element list.
// -----------------------------------------------------------------------------
//
    EXPORT_C TBool GetElementListL( const TXmlEngElement& aElement,
                                    RArray<TXmlEngElement>& aElementList,
                                    const TDesC8& aMainTag,
                                    TBool aSearchable )
        {

        TBool status = EFalse;
        
        if ( aElement.NotNull() )
			{
			
			RXmlEngNodeList<TXmlEngElement> childElements;
			CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
			
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();
				
				//Get "searchable" attribute value
				TPtrC8 searchable = GetAttrValueL( sub, KSearchable() );
       
                // aSearchable == false || tag != "container" || aSearchable == true && searchable == "true"
				// check this "true" value
				if ( aSearchable == ( TBool ) EFalse 
					|| ( !CheckTagL( sub, KContainer() ) )
					|| ( aSearchable == ( TBool ) ETrue && searchable.Compare( KTrueVal() ) == 0 ) )
                	{

                	if ( CheckTagL( sub, aMainTag ) )
                        {
                        User::LeaveIfError(aElementList.Append( sub));
                        status = ETrue;
                        }

                    else
                        {
                        if ( status == ( TBool ) ETrue )
                            {
                            GetElementListL( sub, 
                                            aElementList, 
                                            aMainTag, 
                                            aSearchable );
                            }

                        else
                            {
                            status = GetElementListL( sub, 
                                                     aElementList, 
                                                     aMainTag, 
                                                     aSearchable );
                            }
                        }
                    }                
                }
            CleanupStack::PopAndDestroy(&childElements);
            }
             
        return status; 
        }

// -----------------------------------------------------------------------------
// UpnpDomInterface::GetElementListL
// Get element list.
// -----------------------------------------------------------------------------
// 
    EXPORT_C TBool GetElementListL( const TXmlEngElement& aElement,
                                    RArray<TXmlEngElement>& aElementList,
                                    const TDesC8& aMainTag, 
                                    const TDesC8& aSubTag, 
                                    const TDesC8& aSubValue,
                                    TBool aSearchable )
        {
        
        TBool status = EFalse;
 
        if ( aElement.NotNull() )
            {

            RXmlEngNodeList<TXmlEngElement> childElements;
            CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
			
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();
                
                //Get "searchable" attribute value
				TPtrC8 searchable = GetAttrValueL( sub, KSearchable() );
      
                // aSearchable == false || tag != "container" || aSearchable == true && searchable == "true"
				// check this "true" value
				if ( aSearchable == ( TBool ) EFalse 
					|| ( !CheckTagL( sub, KContainer() ) )
					|| ( aSearchable == ( TBool ) ETrue && searchable.Compare( KTrueVal() ) == 0 ) )
                	{
					
					if ( CheckTagL( sub, aMainTag ) )
                        {
                        if ( CheckSubTagL( sub, aSubTag, aSubValue ) )
                            {
                            User::LeaveIfError(aElementList.Append( sub));
                            status = ETrue;
                            }
						}
                    else
                        {
                        if ( status == ( TBool ) ETrue )
                            {
                            GetElementListL( sub, 
                                            aElementList, 
                                            aMainTag, 
                                            aSubTag, 
                                            aSubValue, 
                                            aSearchable );

                            }
                        else
                            {
                            status = GetElementListL( sub, 
                                                     aElementList, 
                                                     aMainTag, 
                                                     aSubTag, 
                                                     aSubValue, 
                                                     aSearchable );

                            }
                        }

                    }
                }
            CleanupStack::PopAndDestroy(&childElements);
            }

        return status; 
        }

// -----------------------------------------------------------------------------
// UpnpDomInterface::GetElementListL
// Get element list.
// -----------------------------------------------------------------------------
//    
    EXPORT_C TBool GetElementListL( const TXmlEngElement& aElement,
                                    RArray<TXmlEngElement>& aElementList,
                                    const TDesC8& aMainTag, 
                                    const TDesC8& aSubTag, 
                                    const TDesC8& aSubValue,
                                    const TDesC8& aNSPrefix,
                                    TBool aSearchable )
        {
        TBool status = EFalse;

        if ( aElement.NotNull() )
            {

            RXmlEngNodeList<TXmlEngElement> childElements;
            CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
			
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();
                
                //Get "searchable" attribute value
				TPtrC8 searchable = GetAttrValueL( sub, KSearchable() );
				
                // aSearchable == false || tag != "container" || aSearchable == true && searchable == "true"
				// check this "true" value
				if ( aSearchable == ( TBool ) EFalse 
					|| ( !CheckTagL( sub, KContainer() ) )
					|| ( aSearchable == ( TBool ) ETrue && searchable.Compare( KTrueVal() ) == 0 ) )
                	{
					
					if ( CheckTagL( sub, aMainTag ) )
                        {
                        if( CheckSubTagPrefixL( sub,
                                               aNSPrefix,
                                               aSubTag,
                                               aSubValue ) )

                            {
                            User::LeaveIfError(aElementList.Append( sub));
                            status = ETrue;
                            }
						}
                     else
                        {

                        if ( status == ( TBool ) ETrue )
                            {
                            GetElementListL( sub, 
                                            aElementList, 
                                            aMainTag, 
                                            aSubTag, 
                                            aSubValue, 
                                            aNSPrefix, 
                                            aSearchable );

                            }
                        else
                            {
                            status = GetElementListL( sub, 
                                                     aElementList, 
                                                     aMainTag, 
                                                     aSubTag, 
                                                     aSubValue, 
                                                     aNSPrefix, 
                                                     aSearchable );

                            }
                        }

                    }
            	}
            CleanupStack::PopAndDestroy(&childElements);
            }
            
		if ( status == ( TBool ) ETrue )
            {
            return ETrue;
            }
            
        return EFalse; 
        }

// -----------------------------------------------------------------------------
// UpnpDomInterface::GetContentElementListL
// Get content element list.
// -----------------------------------------------------------------------------
//
    EXPORT_C TBool GetContentElementListL( const TXmlEngElement& aElement, 
                                           RArray<TXmlEngElement>& aElementList,
                                           const TDesC8& aSubTag, 
                                           const TDesC8& aSubValue,
                                           TBool aSearchable )
        {
        // Operators: MainTag
        // Example: GetStateVariableList
        // MainTag     = "stateVariableTable"       
    
        
        if ( aElement.NotNull() )
			{
            TBool status = EFalse;
            
            RXmlEngNodeList<TXmlEngElement> childElements;
            CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
			
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();
                
               //Get "searchable" attribute value
				TPtrC8 searchable = GetAttrValueL( sub, KSearchable() );
                
                // aSearchable == false || tag != "container" || aSearchable == true && searchable == "true"
				// check this "true" value
				if ( aSearchable == ( TBool ) EFalse 
					|| ( !CheckTagL( sub, KContainer() ) )
					|| ( aSearchable == ( TBool ) ETrue && searchable.Compare( KTrueVal() ) == 0 ) )
                	{

					if( CheckTagL( sub, aSubTag, aSubValue ) )
						{
						User::LeaveIfError(aElementList.Append( sub));
                        status = ETrue;
						}
                    else
                        {

                        if(status == ( TBool ) ETrue )
                            {
                            GetContentElementListL( sub, 
                                                   aElementList,
                                                   aSubTag,
                                                   aSubValue,
                                                   aSearchable );
                            }

                        else
                            {
                            status = GetContentElementListL( sub, 
                                                            aElementList,
                                                            aSubTag,
                                                            aSubValue,
                                                            aSearchable );
                            }
                        }
                	}
				}
			CleanupStack::PopAndDestroy(&childElements);
            return status; 
            }

        return EFalse;
		}



// -----------------------------------------------------------------------------
// UpnpDomInterface::GetDirectoryElementListL
// Get directory element list.
// -----------------------------------------------------------------------------
//       
    EXPORT_C TBool GetDirectoryElementListL( const TXmlEngElement& aElement,
                                             RArray<TXmlEngElement>& aElementList,
                                             const TDesC8& aAttrName,
                                             const TDesC8& aAttrValue,
                                             TBool aSearchable )
        {
        TBool status = EFalse;
       
    	if ( aElement.NotNull() )
			{
			
			RXmlEngNodeList<TXmlEngElement> childElements;
			CleanupClosePushL(childElements);
			aElement.GetChildElements( childElements );
			
			while( childElements.HasNext() )
				{
			
				// Get a single element of the current level
				TXmlEngElement sub = childElements.Next();
				
				//Get "searchable" attribute value
				TPtrC8 searchable = GetAttrValueL( sub, KSearchable() );
				
				// aSearchable == false || tag != "container" || aSearchable == true && searchable == "true"
				// check this "true" value
				if ( aSearchable == ( TBool ) EFalse 
					|| ( !CheckTagL( sub, KContainer() ) )
					|| ( aSearchable == ( TBool ) ETrue && searchable.Compare( KTrueVal() ) == 0 ) )
                	{
                	
                	if ( CheckAttributeValueL( sub, aAttrName, aAttrValue ) )
                        {
                        
                        User::LeaveIfError(aElementList.Append( sub));
                        status = ETrue;
                        }
                    
                    if ( status )
                        {
                        GetDirectoryElementListL( sub, 
                                                 aElementList, 
                                                 aAttrName, 
                                                 aAttrValue, 
                                                 aSearchable );
                        }

                    else
                        {
                        status = GetDirectoryElementListL( sub, 
                                                          aElementList, 
                                                          aAttrName, 
                                                          aAttrValue,
                                                          aSearchable );
                        }
                    }
				}
			CleanupStack::PopAndDestroy(&childElements);
			}

        return status; 
        }

// -----------------------------------------------------------------------------
// UpnpDomInterface::SaveToFileL
// Save to file.
// -----------------------------------------------------------------------------
//
    EXPORT_C TBool SaveToFileL( const TDesC16& aFileName, 
                                const TXmlEngElement& aRoot )
        {
        TBool result = EFalse;
        // Check element
        if ( aRoot.IsNull() )
            {
            return result;
            }
        
        // Get XML document handler from DOM tree root node and save it to the file
	    RXmlEngDocument doc = aRoot.OwnerDocument();
	        
	    //Dump XML tree into buffer
	    RBuf8 xmlBuf;
	    TXmlEngSerializationOptions options(TXmlEngSerializationOptions::KOptionIndent | 
	    							  TXmlEngSerializationOptions::KOptionEncoding);
	      
	    doc.SaveL( xmlBuf, aRoot, options);
        CleanupClosePushL(xmlBuf);
        result = SaveToFileL( aFileName, xmlBuf );
        CleanupStack::PopAndDestroy(&xmlBuf);
   		
   		return result;
        }

    
// -----------------------------------------------------------------------------
// UpnpDomInterface::SaveToFileL
// Save to file.
// -----------------------------------------------------------------------------
//
    EXPORT_C TBool SaveToFileL( const TDesC16& aFileName, 
                                const TDesC8& aData )
        {
        // Create file server session
        RFs fs;
        TInt error = fs.Connect();

        if ( error != KErrNone )
            {
            LOGS("SERVICE *** RFs::Connect() id 12 failed; ERROR");

            return EFalse;
            }
        
        RFile file;

        // Create or Replace the requested file, 
        // if operation does not succeed return EFalse
        if ( file.Replace( fs, aFileName, EFileWrite ) == KErrNone )
            {
            RFileWriteStream writeStream( file );
            CleanupClosePushL(fs);
            CleanupClosePushL(file);
            CleanupReleasePushL(writeStream);
            writeStream.WriteL( aData );
            writeStream.CommitL();
            
            
            CleanupStack::PopAndDestroy(&writeStream);
            CleanupStack::PopAndDestroy(&file);
            CleanupStack::PopAndDestroy(&fs);
            
            return ETrue;
            }

        else
            {
            LOGS("SERVICE *** RFile::Replace() id 12 failed; ERROR");
            fs.Close();
            return EFalse;
            }
        }

	} 
	
//  End of File 
