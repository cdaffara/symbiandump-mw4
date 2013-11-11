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

#ifndef __MENUCLIENT
#define __MENUCLIENT

//  INCLUDES
#include <eikenv.h>
#include <avkon.hrh>
#include <widgetmenu.rsg>

#include "brctlinterface.h"
#include "MenuCallbacks.h"
#include "MenuItemCallbacks.h"
#include "WidgetEngineCallbacks.h"

// CONSTANTS

// MACROS

// DATA TYPES
enum TMenuItemState
{
    EReal=0,                        //in real tree
    EVirtual,                       //in virtual tree
    ERealOrVirtual                  //either real or virtual
};

// FUNCTION PROTOTYPES

// FORWARD DECLARATION

// CLASS DECLARATION
class CMenuItem;
class MJSObjectProtector;

namespace KJS {
    class JSMenu;
    class JSMenuItemConstructor;
}


/**
*  CWidgetExtension
*
*  @lib browserengine.dll
*  @since 3.2
*/
class CMenuClient : public CBase, public MJSMenuCallbacks, public MJSMenuItemCallbacks
{

public:
    static CMenuClient* NewL(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback);
    virtual ~CMenuClient();
    
public:
    KJS::JSMenu* jsmenu(MJSObjectProtector*);
    KJS::JSMenuItemConstructor* jsmenuitem(MJSObjectProtector*);
    
    void addOptionMenuItemsL( CEikMenuPane& aMenuPane,TInt aResourceId );     
    void menuShowed();
    void menuItemSelected( TInt aInternalId );


public: // From MWidgetExtension
    TBool HandleCommandL( TInt aCommandId );        

public: //From MJSMenuCallbacks
    void* getFromMenuItemCollection( TInt aCmdId );            
    void* getFromMenuItemCollection( const TDesC& aTextVal );              
    void hideSoftkeys();
    void showSoftkeys();          
    void setRightSoftKeyLabel( const TDesC& aText );
    void setLeftSoftKeyLabel( const TDesC& aText );
    void clearMenuItems();    

public://From MJSMenuItemCallbacks
    TInt createOptionsMenuItem( const TDesC& aText,  TInt aCmdId, TInt aInternalId, void* aObj );                
    TInt addOptionsMenuItem( TInt aInternalId, TInt aParentInternalId, TBool aShow=ETrue );
    void deleteMenuItem( TInt aInternalId );       
    void setDimmed( TInt aInternalId, TBool aValue );        
    KJS::WidgetEventHandler* menuItemObserver( TInt aInternalId, TInt& aCmdId );            
    void setMenuItemObserver( TInt aInternalId, KJS::WidgetEventHandler* aOnSelectCallback );                
  
private:
    void deleteTree( CMenuItem* aNode );
    void insertTree( CMenuItem* aParent, CMenuItem* aItem );
    void rearrangeTree( CMenuItem* aParent, CMenuItem* aItem, TBool aShow );
    void showTree( CMenuItem* aNode, TBool aShow );
    void deleteItem( CMenuItem* aItem );
    void deleteSibling( CMenuItem* aRoot, CMenuItem* aItem );
   
private:
    void addMenuItem( CEikMenuPane& aMenuPane, CMenuItem* aNode , TInt aIsRootMenuPane = EFalse);
    TInt append( CMenuItem* root, CMenuItem* item );
    void appendSibling( CMenuItem* root, CMenuItem* item );
    CMenuItem* findNodeById( CMenuItem* node, TInt aInternalId, TMenuItemState aState );
    TBool isDuplicate( CMenuItem* node );
    CMenuItem* findNodeByCmdId( CMenuItem* node, TInt aCmdId );
    CMenuItem* findNodeByText( CMenuItem* node, const TDesC& aText );
  
private:        
    CMenuClient(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback);
    void ConstructL();
  
private:

        struct TCascadeIDMap
            {
            TInt iResourceId;
            TInt parentInternalId;
            };

        RArray<TCascadeIDMap>   iCascadeArr;
        TInt                    iResourceId;
        CMenuItem*              iMenuRoot;
        
private:
        MWidgetCallback*       iWidgetCallback;
        MWidgetEngineCallbacks* m_widgetenginecallback;
        
        KJS::JSMenu*                m_jsmenu;            
        KJS::JSMenuItemConstructor* m_jsmenuitemctr;
        
};


#endif
