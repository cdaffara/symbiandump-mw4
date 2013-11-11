/*
* ============================================================================
*  Name     : CBrCtlApiTestContainer from BrCtlBCTestContainer.h
*  Part of  : BrCtlBCTest
*  Created  : 07/14/2004 by 
*  Description:
*     Declares container control for application.
*  Version  :
*  Copyright: 
* ============================================================================
*/

#ifndef BRCTLBCTESTCONTAINER_H
#define BRCTLBCTESTCONTAINER_H

// INCLUDES
//#include <coecntrl.h>
#include <BrCtlInterface.h> // interface under test

//#include "BrCtlApiTestObserver.h"
//#include "BrCtlApiTestStateChangeObserver.h"
#include "BrCtlApiTestDialogsProvider.h"
   
// CONSTATNS
//const TInt KThisAppUid = 0xA0000181;

//_LIT( KUrlLoadTestHtml, "c:\\BrCtlTest\\loadtest.html" );
//_LIT( KUrlLoadTestHtml2, "file:///c:\\BrCtlTest\\loadtest2.html" );


// FORWARD DECLARATIONS
class CBrCtlApiTestObserver;
class CBrCtlApiTestStateChangeObserver;

// CLASS DECLARATION

/**
*  CBrCtlApiTestContainer  container control class.
*  
*/
class CBrCtlApiTestContainer : public CCoeControl,
                                     MCoeControlObserver,
                                     MBrCtlDataLoadSupplier,
                                     MBrCtlCommandObserver
    {
    public: // Constructors and destructor
        void ConstructL (const TRect& aRect );
        ~CBrCtlApiTestContainer();
    
    private: // Functions from base classes 
        void HandleCommandL( TBrCtlDefs::TBrCtlClientCommands aCommand, const CArrayFix<TPtrC>& aAttributesNames,
                                     const CArrayFix<TPtrC>& aAttributeValues ); 
                                     /**
        * From MBrCtlDataLoadSupplier, Cancel an on-going load
        */
        void CancelLoad();
        
    public: // Functions utility
    	void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );
    	TInt SetFocus();
        void CreateBrowserControlInterfaceL();
        CBrCtlInterface* BrCtlInterface(){return iBrCtlInterface;}
        
        
    private: //data
        CBrCtlInterface* iBrCtlInterface;
        TInt iCommandBase;
        CBrCtlApiTestObserver* iBrCtlApiTestObserver;
        CBrCtlApiTestStateChangeObserver* iBrCtlApiTestStateChangeObserver;
        CBrowserDialogsProviderProxy* iBrCtlDialogs;
        CEikMenuPane* iMenuPane;
        TBool iCancelInitDataLoad;
    };

#endif // BRCTLBCTESTCONTAINER_H

// End of File
