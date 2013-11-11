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
* Description:           
*
*/










#include "wsstarmessageutils.h"
#include <xml/dom/xmlengnodelist.h> 

CSenElement* CWSStarMessageUtils::GetMarkedElementL(CSenElement& aParent, 
                                                    const TDesC8& aValue, TBool aAttribute)
    {
     CSenElement* markedEl = NULL;

//1.flat choose 
    RPointerArray<CSenElement>& elements = aParent.ElementsL();
    TInt count(elements.Count());
    TInt i=0;
    while (i<count && !markedEl)
        {
        markedEl = elements[i];
        if (aAttribute)
            {
            if (!markedEl->AttrValue(aValue))
                {
                markedEl = NULL;
                }    
            }
        else
            {
            if (markedEl->LocalName() != aValue)
                {
                markedEl = NULL;
                }    
            }
        
        i++;
        }

//2.go deeper 
    if (!markedEl)
        {
        elements = aParent.ElementsL();
        TInt count(elements.Count());
        TInt i=0;
        while (i<count && !markedEl)
            {
            markedEl = CWSStarMessageUtils::GetMarkedElementL(*elements[i], aValue, aAttribute);
            i++;
            }
        }
    return markedEl;
    }

void CWSStarMessageUtils::FindElementL(const TDesC8& aElementName, 
                                        const TDesC8& aNamespace, 
                                        const TXmlEngElement& aParent, 
                                        RArrayElements* aElements)
{
    RXmlEngNodeList<TXmlEngElement> nodeList;
    TXmlEngElement parent;
    aParent.GetElementsByTagNameL( nodeList, aElementName , aNamespace );
    if( nodeList.Count() )
    {
        parent = nodeList.Next();
        aElements->AppendL(parent);
    }
    
    RXmlEngNodeList<TXmlEngElement> nodeChildList;
    aParent.GetChildElements(nodeChildList);
    
    TInt count( nodeChildList.Count() );
    for(TInt i(0) ; i<count ; i++)
    {
        if( nodeChildList.HasNext() )
        {
            CWSStarMessageUtils::FindElementL(aElementName , 
                                                aNamespace , 
                                                nodeChildList.Next() ,
                                                aElements);
        }
    }
    nodeList.Close();
    nodeChildList.Close(); 
}

CSenElement* CWSStarMessageUtils::FindElementL(const TDesC8& aLocalName, CSenElement& aParent)
    {
     CSenElement* elem = NULL;

    RPointerArray<CSenElement>& elements = aParent.ElementsL();
    TInt count(elements.Count());
    TInt i=0;
    while (i<count && !elem)
        {
        elem = elements[i];
        if (elem->LocalName() != aLocalName)
            {
            elem = NULL;
            }    
        i++;
        }

    if (!elem)
        {
        elements = aParent.ElementsL();
        TInt count(elements.Count());
        TInt i=0;
        while (i<count && !elem)
            {
            elem = CWSStarMessageUtils::FindElementL(aLocalName, *elements[i]);
            i++;
            }
        }
    return elem;
    
    }
void CWSStarMessageUtils::FindElementL(
                                const TDesC8& aLocalName,
                                CSenElement& aParent,
                                RArraySenElements& aElements)
    {
    CSenElement* elem = NULL;

    RPointerArray<CSenElement>& elements = aParent.ElementsL();
    TInt count(elements.Count());
    TInt i=0;
    while (i<count)
        {
        elem = elements[i];
        if (elem->LocalName() == aLocalName)
        {
            aElements.Append(elem);
        }
        CWSStarMessageUtils::FindElementL(aLocalName, *elem,aElements);
        elem = NULL;
        i++;
        }
    }
