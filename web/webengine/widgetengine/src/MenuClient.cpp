/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class represents the Widget Extension object
*
*/

// INCLUDE FILES
#include "MenuClient.h"
#include "MenuItemClient.h"
#include "Menu.h"
#include "MenuItem.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KMenuItemCommandIdBase = 20000;
const TInt KMaxOptionsMenu = 63;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
// The binary tree representation of a multiway tree is based on
// first child-next sibling representation of the tree. In this representation
// every node is linked with its leftmost child and its next (right nearest)
// sibling.

/*
Example: Consider the following multiway tree

                        1
                      / | \
                     /  |  \
                    /   |   \
                   2    3    4
                  / \        |
                 5   6       7
                            / \
                           8   9


This tree can be represented in first child-next sibling manner as follows :

                        1
                       /
                      /
                     /
                    2---3---4
                   /       /
                  5---6   7
                         /
                        8---9


If we look at the first child-next sibling representation of the tree closely,
we will see that it forms a binary tree. To see this better, rotate every
next-sibling edge 45 degrees clockwise. After that, the following binary tree is got:

                        1
                       /
                      2
                     / \
                    5   3
                     \   \
                      6   4
                         /
                        7
                       /
                      8
                       \
                        9

*/


// ----------------------------------------------------------------------------
// CMenuClient::NewL
//
//
//
// ----------------------------------------------------------------------------
CMenuClient* CMenuClient::NewL(MWidgetCallback& aWidgetCallback, 
                               MWidgetEngineCallbacks& aWidgetEngineCallback)
{
    CMenuClient* self = new ( ELeave ) CMenuClient(aWidgetCallback,aWidgetEngineCallback);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


// ----------------------------------------------------------------------------
// CMenuClient::CMenuClient
//
//
//
// ----------------------------------------------------------------------------
CMenuClient::CMenuClient(MWidgetCallback& aWidgetCallback, 
                         MWidgetEngineCallbacks& aWidgetEngineCallback) : 
                        iWidgetCallback(&aWidgetCallback), 
                        m_widgetenginecallback(&aWidgetEngineCallback),
                        m_jsmenu(0),            
                        m_jsmenuitemctr(0)
{
}


// ----------------------------------------------------------------------------
// CMenuClient::~CMenuClient
//
//
//
// ----------------------------------------------------------------------------
CMenuClient::~CMenuClient()
{
    // delete tree
    if ( iMenuRoot )
        {
        deleteTree( iMenuRoot->firstChild() );
        delete iMenuRoot;
        iMenuRoot = NULL;
        }

    iCascadeArr.Close();
}


// ----------------------------------------------------------------------------
// CMenuClient::ConstructL
//
//
//
// ----------------------------------------------------------------------------
void CMenuClient::ConstructL()
{        
}


// ----------------------------------------------------------------------------
// CMenuClient::jsmenu
//
//
//
// ----------------------------------------------------------------------------
KJS::JSMenu* CMenuClient::jsmenu(MJSObjectProtector* aProtector) 
{ 
    if (!m_jsmenu) {
        m_jsmenu = new KJS::JSMenu(this, aProtector);
    }
    
    return m_jsmenu; 
}


// ----------------------------------------------------------------------------
// CMenuClient::jsmenuitem
//
//
//
// ----------------------------------------------------------------------------
KJS::JSMenuItemConstructor* CMenuClient::jsmenuitem(MJSObjectProtector* protector) 
{
    if (!m_jsmenuitemctr){
        m_jsmenuitemctr = new KJS::JSMenuItemConstructor(this, protector);
    }

    return m_jsmenuitemctr;    
}


// ----------------------------------------------------------------------------
// CMenuClient::addOptionMenuItemsL
//
//
//
// ----------------------------------------------------------------------------
//
void CMenuClient::addOptionMenuItemsL(CEikMenuPane& aMenuPane, TInt aResourceId)
{

    CMenuItem* node = NULL;
    CMenuItem* parentNode = NULL;
    TInt parentInternalId = -1;

    //one of the cascade menu pane
    if ( aResourceId >= R_CASCADE_MENU_1 )  {
        for ( TInt i = 0; i < iCascadeArr.Count(); i++ ) {
            if ( iCascadeArr[i].iResourceId == aResourceId ) {
                parentInternalId = iCascadeArr[i].parentInternalId;
                parentNode = findNodeById( iMenuRoot, parentInternalId, EReal );
                if ( parentNode )
                    break;
            }
        }
        
        if ( parentInternalId != -1 && parentNode ) {
            menuItemSelected( parentInternalId + KMenuItemCommandIdBase );
            node = parentNode->firstChild();
        }
    }
    else {
        // constructing root pane
        iResourceId = R_CASCADE_MENU_1;

        //invoke menu onshow callback
        if ( m_jsmenu && m_jsmenu->onShowCallback() ) {
            m_jsmenu->onShowCallback()->InvokeCall();
        }

        if ( aMenuPane.NumberOfItemsInPane() > 1 ) {
            aMenuPane.DeleteBetweenMenuItems( 0, aMenuPane.NumberOfItemsInPane() - 2 );
        }
        
        if ( iMenuRoot ) {
            node = iMenuRoot->firstChild();
        }
    }

    if ( node ) {
    
        //add first child
        addMenuItem( aMenuPane, node, parentInternalId == -1 );

        //add all other children (siblings of first child)
        CMenuItem* sibling = node->nextSibling();
        while( sibling ) {
            addMenuItem( aMenuPane, sibling, parentInternalId == -1 );
            sibling = sibling->nextSibling();
        }
    }

}


// -----------------------------------------------------------------------------
// CMenuClient::menuShowed
//
//
//
// -----------------------------------------------------------------------------
void CMenuClient::menuShowed()
{
    if ( m_jsmenu && m_jsmenu->onShowCallback() ) {
        m_jsmenu->onShowCallback()->InvokeCall();
    }
}

// -----------------------------------------------------------------------------
// CMenuClient::menuItemSelected
//
//
//
// -----------------------------------------------------------------------------
void CMenuClient::menuItemSelected( TInt aInternalId )
{
    TInt cmdId = 0;
    if (menuItemObserver(aInternalId, cmdId )) {
        menuItemObserver(aInternalId, cmdId)->InvokeCall(cmdId);
    }
}



////////////////////////////////////////////////////////////////////////////////
// From MWidgetExtension
// -----------------------------------------------------------------------------
// CMenuClient::HandleCommandL
//
//
//
// -----------------------------------------------------------------------------
//
TBool CMenuClient::HandleCommandL( TInt aCommandId )
{
    if ( aCommandId == EAknSoftkeyExit && m_jsmenu && m_jsmenu->rightKeyCallback() ) {
        m_jsmenu->rightKeyCallback()->InvokeCall();
        return ETrue;
    }
    
    if ( aCommandId == EAknSoftkeyOptions && m_jsmenu && m_jsmenu->leftKeyCallback() ) {
            m_jsmenu->leftKeyCallback()->InvokeCall();
            return ETrue;
    }

    return EFalse;
}


////////////////////////////////////////////////////////////////////////////////
//From MJSMenuCallbacks
// -----------------------------------------------------------------------------
// CMenuClient::setRightSoftKeyLabel
//
//
//
// -----------------------------------------------------------------------------
//
void CMenuClient::setRightSoftKeyLabel( const TDesC& aText )
{
    m_widgetenginecallback->setRightSoftKeyLabel(aText); 
}

////////////////////////////////////////////////////////////////////////////////
//From MJSMenuCallbacks
// -----------------------------------------------------------------------------
// CMenuClient::setLeftSoftKeyLabel
//
//
//
// -----------------------------------------------------------------------------
//
void CMenuClient::setLeftSoftKeyLabel( const TDesC& aText )
{
    m_widgetenginecallback->setLeftSoftKeyLabel(aText); 
}

// -----------------------------------------------------------------------------
// CMenuClient::clearMenuItems
//
//
//
// -----------------------------------------------------------------------------
//
void CMenuClient::clearMenuItems()
{
    if ( iMenuRoot )
        showTree( iMenuRoot->firstChild(), EFalse );  
}

// -----------------------------------------------------------------------------
// CMenuClient::getFromMenuItemCollection
//
//
//
// -----------------------------------------------------------------------------
//
void* CMenuClient::getFromMenuItemCollection( const TDesC& aTextVal ) 
{
    CMenuItem* node = NULL;
    node = findNodeByText( iMenuRoot, aTextVal );

    if ( node )
        return node->obj();
    
    return NULL;
}

// -----------------------------------------------------------------------------
// CMenuClient::getFromMenuItemCollection
//
//
//
// -----------------------------------------------------------------------------
//
void* CMenuClient::getFromMenuItemCollection( TInt aCmdId ) 
{
    CMenuItem* node = NULL;
    node = findNodeByCmdId( iMenuRoot, aCmdId );

    if ( node )
        return node->obj();
    
    return NULL;
}

// -----------------------------------------------------------------------------
// CMenuClient::hideSoftkeys
//
//
//
// -----------------------------------------------------------------------------
//
void CMenuClient::hideSoftkeys()
{    
    iWidgetCallback->SetSoftkeysVisible(EFalse);
}

// -----------------------------------------------------------------------------
// CMenuClient::showSoftkeys
//
//
//
// -----------------------------------------------------------------------------
//
void CMenuClient::showSoftkeys()        
{   
    iWidgetCallback->SetSoftkeysVisible(ETrue);
}

////////////////////////////////////////////////////////////////////////////////
//From MJSMenuItemCallbacks

// -----------------------------------------------------------------------------
// CMenuClient::createOptionsMenuItem
//
//
//
// -----------------------------------------------------------------------------
//
TInt CMenuClient::createOptionsMenuItem( const TDesC& aText, TInt aCmdId, TInt aInternalId, void* aObj )
{    
    if ( !iMenuRoot ) {
        iMenuRoot = CMenuItem::NewL( _L(""), -1, -1 );
    }

    CMenuItem* item = CMenuItem::NewL(aText, aCmdId, aInternalId, EFalse, iMenuRoot, aObj, EFalse);
        
    return append( iMenuRoot, item );            
}

// -----------------------------------------------------------------------------
// CMenuClient::addOptionsMenuItem
//
//
//
// -----------------------------------------------------------------------------
//
TInt CMenuClient::addOptionsMenuItem( TInt aInternalId, TInt aParentInternalId, TBool aShow )
{    
    if ( !iMenuRoot ) {
        iMenuRoot = CMenuItem::NewL( _L(""), -1, -1 );
    }    

    // Find item in actual or real tree by the internal id
    CMenuItem* item = findNodeById( iMenuRoot, aInternalId, ERealOrVirtual );

    if ( !item )
        return KErrGeneral;

    if ( aShow ) {
        
        // if there is a clash by cmdid or text of this item or any of its descendents 
        // with any real tree item return KErrAlreadyExists
        if ( findNodeByCmdId( iMenuRoot, item->cmdId() ) || 
            findNodeByText( iMenuRoot, item->text() ) || 
            isDuplicate( item->firstChild() ) ) {
            return KErrAlreadyExists;
        }
            
    }
  
    // Find parent in real or virtual tree
    CMenuItem* parent = findNodeById( iMenuRoot, aParentInternalId, ERealOrVirtual );
    // if this is a new parent, rearrange the tree
    CMenuItem* currParent = item->parent();

    if ( ( aParentInternalId == -1 && aShow ) || 
        ( currParent && currParent->internalId() != aParentInternalId ) ) {
    
        rearrangeTree( parent, item, aShow );// item is deleted in RearrangeTree
        return KErrNone;
    }
    
    return KErrGeneral;
}

// -----------------------------------------------------------------------------
// CMenuClient::deleteMenuItem
//
//
//
// -----------------------------------------------------------------------------
//
void CMenuClient::deleteMenuItem( TInt aInternalId )
{    
    CMenuItem* item = NULL;
    CMenuItem* node = findNodeById( iMenuRoot, aInternalId, EReal );

    if ( !node )
        return;

    CMenuItem* parent = node->parent();

    if ( !parent || !parent->firstChild() )
        return;

    //it could be first child
    if ( parent->firstChild()->internalId() == aInternalId ) {
        item = parent->firstChild();
        //the sibling of firstchild will be the new firstchild
        parent->setFirstChild( item->nextSibling() );
    }    
    //or one of the sibling of firstchild
    else {
        CMenuItem* prev = parent->firstChild();
        if ( prev ) {
            item = prev->nextSibling();
        }

        while( item ) {
            if ( item->internalId() == aInternalId ) {
                prev->setNextSibling( item->nextSibling() );
                break;
            }
            prev = item;
            item = item->nextSibling();
        }
    }

    if ( item && item->internalId() == aInternalId ) {
        // rearrange the tree and insert the item and its children in the virtual tree
        rearrangeTree( iMenuRoot, item, EFalse );      
        return;          
    }  
}

// -----------------------------------------------------------------------------
// CMenuClient::setDimmed
//
//
//
// -----------------------------------------------------------------------------
//
void CMenuClient::setDimmed( TInt aInternalId, TBool aValue )
{
    CMenuItem* node = NULL;
    node = findNodeById( iMenuRoot, aInternalId, EReal );

    if ( node ) {
        node->setDimmed( aValue );
    }

}

// -----------------------------------------------------------------------------
// CMenuClient::menuItemObserver
//
//
//
// -----------------------------------------------------------------------------
//
KJS::WidgetEventHandler* CMenuClient::menuItemObserver( TInt aInternalId, TInt& aCmdId )
{
    CMenuItem* node = NULL;
    node = findNodeById( iMenuRoot, aInternalId, EReal );

    if ( node ) {
        aCmdId = node->cmdId();
        return node->callback();
    }
    
    return NULL;    

}

// -----------------------------------------------------------------------------
// CMenuClient::setMenuItemObserver
//
//
//
// -----------------------------------------------------------------------------
//
void CMenuClient::setMenuItemObserver( TInt aInternalId, KJS::WidgetEventHandler* aOnSelectCallback )
{    
    CMenuItem* node = NULL;
    node = findNodeById( iMenuRoot, aInternalId, ERealOrVirtual );

    if ( node ) {
        node->setCallback( aOnSelectCallback );
    }
    
}


////////////////////////////////////////////////////////////////////////////////
//private
// ----------------------------------------------------------------------------
// CMenuClient::deleteTree
// Delete node and all its children.
//
//
// ----------------------------------------------------------------------------
//
void CMenuClient::deleteTree( CMenuItem* aNode )
{

    if ( aNode != NULL ) {
        deleteTree( aNode->firstChild() );
        deleteTree( aNode->nextSibling() );         
        delete aNode;
        aNode = NULL;
    }

}

// ----------------------------------------------------------------------------
// CMenuClient::insertTree
// this function inserts all items in a menupane with all its desendents 
//    (i.e) all its cascade menupanes
//
//
// ----------------------------------------------------------------------------
//
void CMenuClient::insertTree( CMenuItem* aParent, CMenuItem* aItem )
{

    if ( !aParent || !aItem )
        return;
    
    CMenuItem* newItem = CMenuItem::NewL( aItem->text(), aItem->cmdId(), 
                                         aItem->internalId(), aItem->dimmed(), 
                                         aParent, aItem->obj(), aParent->show() );
    append( aParent, newItem );        
    // Insert all children
    insertTree( newItem, aItem->firstChild() );
    // Insert all siblings
    insertTree( aParent, aItem->nextSibling() );    

    return;
}

// ----------------------------------------------------------------------------
// CMenuClient::rearrangeTree
// this function rearranges the tree when the parent of a node changes
//
//
// ----------------------------------------------------------------------------
//
void CMenuClient::rearrangeTree( CMenuItem* aParent, CMenuItem* aItem, TBool aShow )
{

    CMenuItem* newItem = CMenuItem::NewL( aItem->text(), aItem->cmdId(), 
                                        aItem->internalId(), aItem->dimmed(), 
                                        aParent, aItem->obj(), aShow );
    append( aParent, newItem );
    insertTree( newItem, aItem->firstChild() );
    deleteTree( aItem->firstChild() );                    
    deleteItem( aItem ); 

}

// ----------------------------------------------------------------------------
// CMenuClient::showTree
// Set show flag for node, its siblings, and all their children.
//
//
// ----------------------------------------------------------------------------
//
void CMenuClient::showTree( CMenuItem* aItem, TBool aShow )
{
    if ( !aItem )
        return;

    aItem->setShow( aShow );
    showTree( aItem->firstChild(), aShow );
    showTree( aItem->nextSibling(), aShow );  

}
    
// -----------------------------------------------------------------------------
// CMenuClient::DeleteItem
//
//
//
// -----------------------------------------------------------------------------
//
void CMenuClient::deleteItem( CMenuItem* aItem )
{

    if ( !aItem )
        return;

    CMenuItem* parent = aItem->parent();

    if ( !parent )
        return;

    if ( parent->firstChild() == aItem ) { //first child
            
        parent->setFirstChild( aItem->nextSibling() );
        delete aItem;
        aItem = NULL;
    }
    else { //sibling of first child        
        deleteSibling( parent->firstChild(), aItem );       
    }

    return;
}

// -----------------------------------------------------------------------------
// CMenuClient::deleteSibling
//
//
//
// -----------------------------------------------------------------------------
//
void CMenuClient::deleteSibling( CMenuItem* aNode, CMenuItem* aItem )
{
    if ( !aNode || !aItem )
        return;

    if ( aNode->nextSibling() == aItem  ) { //first sibling        
        aNode->setNextSibling( aItem->nextSibling() );
        delete aItem;
        aItem = NULL;
    }
    else {    
        deleteSibling( aNode->nextSibling(), aItem );
    }

    return;
}
    
// ----------------------------------------------------------------------------
// CMenuClient::addMenuItem
//
//
//
// ----------------------------------------------------------------------------
//
void CMenuClient::addMenuItem( CEikMenuPane& aMenuPane, CMenuItem* aNode, TBool aIsRootMenuPane)
{
    if ( !aNode->show() )
        return;

    CEikMenuPaneItem::SData menuData;
    menuData.iCommandId = aNode->internalId() + KMenuItemCommandIdBase;

    if ( aNode->firstChild() ) {
        if ( iResourceId > R_CASCADE_MENU_1 + KMaxOptionsMenu ) {
            return; //Dont add any menu item any more
        }
        else {
            menuData.iCascadeId = iResourceId;
        }
    }
    else {
        menuData.iCascadeId = 0;
    }

    menuData.iFlags = 0;
    menuData.iText = aNode->text();

    TCascadeIDMap entry;
    entry.iResourceId = iResourceId++;
    entry.parentInternalId = aNode->internalId();
    iCascadeArr.Append( entry );

    //Since root menu pane has "Exit" at the botton, menu items must be added at last - 1
    //else items will get appended below "Exit"
    TInt index = aIsRootMenuPane ? aMenuPane.NumberOfItemsInPane() - 1 : aMenuPane.NumberOfItemsInPane();
    aMenuPane.InsertMenuItemL( menuData, index );
    aMenuPane.SetItemDimmed( aNode->internalId()+KMenuItemCommandIdBase, aNode->dimmed() );

}


// -----------------------------------------------------------------------------
// CMenuClient::append
//
//
//
// -----------------------------------------------------------------------------
//
TInt CMenuClient::append( CMenuItem* aParent, CMenuItem* aItem )
{

    if ( !aParent || !aItem )
        return KErrGeneral;

    if ( aParent->firstChild() == NULL ) {//first child        
        aParent->setFirstChild( aItem );
    }
    else { //insert as sibling of first child        
        appendSibling( aParent->firstChild(), aItem );
    }

    return KErrNone;
}


// -----------------------------------------------------------------------------
// CMenuClient::appendSibling
//
//
//
// -----------------------------------------------------------------------------
//
void CMenuClient::appendSibling( CMenuItem* aNode, CMenuItem* aItem )
{

    if ( !aNode || !aItem )
        return;

    if ( aNode->nextSibling() == NULL ) { //first sibling        
        aNode->setNextSibling( aItem );
    }
    else {
        appendSibling( aNode->nextSibling(), aItem );
    }

    return;
}


// ----------------------------------------------------------------------------
// CMenuClient::findNodeById
//
//
//
// ----------------------------------------------------------------------------
//
CMenuItem* CMenuClient::findNodeById( CMenuItem* node, TInt aInternalId, TMenuItemState aState )
{
    
    CMenuItem* item = NULL;
    
    if ( !node )
        return NULL;

    if ( node->internalId() == aInternalId ) {
        if ( aState == EReal && node->show() )
            return node;
        else if ( aState == EVirtual && !node->show() )
            return node;
        else if ( aState == ERealOrVirtual )
            return node;
    }
    else {
        
        if ( node->firstChild() ) {
            item = findNodeById( node->firstChild(), aInternalId, aState );
            if ( item )
                return item;
        }
    
        if ( node->nextSibling() ) {
            item = findNodeById( node->nextSibling(), aInternalId, aState );
        }
    }
    
    return item;
}


// ----------------------------------------------------------------------------
// CMenuClient::isDuplicate
//
//
//
// ----------------------------------------------------------------------------
//
TBool CMenuClient::isDuplicate( CMenuItem* node )
{
    
    if ( !node )
        return EFalse;

    if ( findNodeByCmdId( iMenuRoot, node->cmdId() ) || 
         findNodeByText( iMenuRoot, node->text() ) )
        return ETrue;

    if ( isDuplicate( node->firstChild() ) )
        return ETrue;
    else
        return isDuplicate( node->nextSibling() );    
    
}

// ----------------------------------------------------------------------------
// CMenuClient::findNodeByCmdId
//
//
//
// ----------------------------------------------------------------------------
//
CMenuItem* CMenuClient::findNodeByCmdId( CMenuItem* node, TInt aCmdId )
{
    
    CMenuItem* item = NULL;
    
    if ( !node )
        return NULL;

    if ( node->cmdId() == aCmdId && node->show() ) {        
        return node;      
    }
    else {
        if ( node->firstChild() ) {
            item = findNodeByCmdId( node->firstChild(), aCmdId );
            if ( item )
                return item;
        }
        
        if ( node->nextSibling() ) {
            item = findNodeByCmdId( node->nextSibling(), aCmdId );
        }
    }
    
    return item;
}

// ----------------------------------------------------------------------------
// CMenuClient::findNodeByText
//
//
//
// ----------------------------------------------------------------------------
//
CMenuItem* CMenuClient::findNodeByText( CMenuItem* node, const TDesC& aText )
{
    CMenuItem* item = NULL;

    if ( !node )
        return NULL;

    if ( node->text().CompareC( aText ) == 0 && node->show() )
        return node;

    item = findNodeByText( node->firstChild(), aText );

    if ( item )
        return item;
        
    item = findNodeByText( node->nextSibling(), aText );    

    return item;
}





//END OF FILE





