/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header declaration
*
*/









 

#ifndef WSSTAR_MESSAGE_UTILS_H
#define WSSTAR_MESSAGE_UTILS_H


#include "SenElement.h"
#include "SenDomFragmentBase.h"


/**
 * Set of static convenience methods to help in message exploring
 */
class CWSStarMessageUtils : public CBase
    {
    typedef RArray<TXmlEngElement> RArrayElements;
    
    public:
    typedef RPointerArray<CSenElement> RArraySenElements;
        /*
        * Encode data into Base64 format
        * @param aParent
        * @param aValue - name of element or attribute
        * @param aAttribute = determine if we want find tag name or attribute name
        * @return 
        */
        static CSenElement* GetMarkedElementL(CSenElement& aParent, const TDesC8& aValue, 
                                                TBool aAttribute = ETrue);
        
        /*
        * Find an Element in XML tree
        * @param aElementName -seeking element name
        * @param aNamespace - seeking element  namespace
        * @param aParent - node where the seeking is starting
        * @param aElements - find elements' array
        */
        static void FindElementL(const TDesC8& aElementName, const TDesC8& aNamespace, 
                                const TXmlEngElement& aParent, RArrayElements* aElements);
        /*
        * Find an Element in XML tree
        * @param aLocalName -seeking element name
        * @param aParent - node where the seeking is starting
        * @param aElements - find elements' array
        */
        static CSenElement* FindElementL(const TDesC8& aLocalName, CSenElement& aParent);
        
        static void FindElementL(const TDesC8& aLocalName,CSenElement& aParent,RArraySenElements& aElements);
    private:
        /**
        * Hide default C++ constructor.
        */
        CWSStarMessageUtils();
        
        /**
        * Prohibit copy constructor if not deriving from CBase.
        */
        CWSStarMessageUtils( const CWSStarMessageUtils& );

        /**
        * Prohibit assignment operator if not deriving from CBase.
        */
        CWSStarMessageUtils& operator=( const CWSStarMessageUtils& );        
    };

#endif // WSSTAR_MESSAGE_UTILS_H

