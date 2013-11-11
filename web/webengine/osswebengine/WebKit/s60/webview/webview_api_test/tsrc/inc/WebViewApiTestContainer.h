/*
* ============================================================================
*  Name     : CWebViewApiTestContainer from WEBVIEWTESTCONTAINER.h
*  Part of  : BrCtlBCTest
*  Created  : 07/14/2004 by
*  Description:
*     Declares container control for application.
*  Version  :
*  Copyright:
* ============================================================================
*/

#ifndef WEBVIEWTESTCONTAINER_H
#define WEBVIEWTESTCONTAINER_H

// INCLUDES
//#include <coecntrl.h>
#include <BrCtlInterface.h>

#include "BrCtlApiTestObserver.h"
#include "BrCtlApiTestDialogsProvider.h"

#include <WebView.h> // interface under test


// CONSTATNS
const TInt KThisAppUid = 0xA0000181;

_LIT( KUrlLoadTestHtml, "c:\\WebViewTest\\loadtest.html" );
_LIT( KUrlLoadTestHtml2, "file:///c:\\WebViewTest\\loadtest2.html" );


// FORWARD DECLARATIONS


namespace WebCore {
    class String;
}

// CLASS DECLARATION

/**
*  CWebViewApiTestContainer  container control class.
*
*/
class CWebViewApiTestContainer : public CCoeControl,
                                     MCoeControlObserver,
                                     MBrCtlDataLoadSupplier,
                                     MBrCtlCommandObserver
    {
    public: // Constructors and destructor
        static CWebViewApiTestContainer* NewL();
        void ConstructL (const TRect& aRect );
        ~CWebViewApiTestContainer();

	//new
	private: // Functions from base classes
        void HandleCommandL( TBrCtlDefs::TBrCtlClientCommands aCommand, const CArrayFix<TPtrC>& aAttributesNames,
                                     const CArrayFix<TPtrC>& aAttributeValues );
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex ) const;
        void Draw( const TRect& aRect ) const;
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );

       /**
        * From MBrCtlDataLoadSupplier, Cancel an on-going load
        */
        void CancelLoad();
	//-new

    public: // Functions utility
        void RunAllTestsL();

    private: //Helper functions
        void CreateBrowserControlInterfaceL(const TRect& aRect);


    public: // Functions to test Web View Interface
    		TInt CreateWebViewL();
    		void MakeWebViewVisible(TBool visible);
    		void MakeViewVisible(TBool visible);
    		TInt mainFrame();
    		TInt mainFrameDocument();
    		TInt page();
    		TInt isMIMETypeRegisteredAsPlugin();
    		TInt pluginForExtension();
    		TInt loadBackForwardListFromOtherView();
    		TInt shouldClose();
    		TInt isLoading();
    		TInt syncRepaint();
    		TInt scheduleRepaint();
    		TInt doRepaint();
    		TInt pageLoadFinished();
    		TInt updatePageScaler();
    		TInt doLayout();
    		TInt maxBidiSize();
			TInt clearOffScreenBitmap();
			TInt scrollBuffer();
			TInt offscreenRect();
			TInt pageScaler();
			TInt scalingFactor();
			TInt openUrl();
			TInt brCtl();
			TInt fepTextEditor();
			TInt formFillPopup();
			TInt setFormFillPopup();
			TInt setEditable();
			TInt isEditable();
			TInt OfferKeyEvent();
			TInt setFocusedElementType();
			TInt focusedElementType();
			TInt pageScrollHandler();
			TInt pageFullScreenHandler();
			TInt pointerEventHandler();
			TInt updateScrollbars();
			TInt openPageView();
			TInt closePageView();
			TInt cancelPageView();
			TInt pageView();
			TInt inPageViewMode();
			TInt searchFor();
			TInt searchAgain();
			TInt exitFindState();
			TInt inFindState();
			TInt pageScalerEnabled();
			TInt isSmallPage();
			TInt willSubmitForm();



    private: //data
        CBrCtlInterface* iBrCtlInterface;
        WebView* iView;
        TInt iCommandBase;
        CBrCtlApiTestObserver* iBrCtlApiTestObserver;
        CBrowserDialogsProviderProxy* iBrCtlDialogs;
    };

#endif // WEBVIEWTESTCONTAINER_H

// End of File
