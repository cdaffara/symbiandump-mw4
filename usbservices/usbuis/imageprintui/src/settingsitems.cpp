/*
* Copyright (c) 2006, 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Show capabilities on the screen
*
*/


#include <StringLoader.h>
#include <eiklbx.h>
#include <aknradiobuttonsettingpage.h>


#include "imageprintui.hrh"
#include "settingsitems.h"
#include "settingstables.h"
#include "imageprintuidebug.h"
#include "capabilitymanager.h"
#include "notes.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsItems* CSettingsItems::NewL(
    CCapabilityManager* aManager, CNotes* aNotes )
    {
    CSettingsItems* self =
        CSettingsItems::NewLC( aManager, aNotes );
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsItems* CSettingsItems::NewLC(
    CCapabilityManager* aManager, CNotes* aNotes
    )
    {
    CSettingsItems* self =
        new ( ELeave ) CSettingsItems( aManager, aNotes );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsItems::CSettingsItems(
    CCapabilityManager* aManager, CNotes* aNotes):
    iCapabilityManager( aManager ), iNotes( aNotes)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsItems::~CSettingsItems()
    {
    delete iBuffer; 
    delete iLayout; 
    delete iPaperSize; 
    delete iQuality; 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CSettingsItems::ConstructL()
    {
    iBuffer = HBufC::NewL(KSettingsMaxSize); 
    iLayout = StringLoader::LoadL(R_USB_PRINT_LAYOUT);
    iPaperSize = StringLoader::LoadL(R_USB_PRINT_PAPER_SIZE);
    iQuality = StringLoader::LoadL(R_USB_PRINT_QUALITY);
    }

// ---------------------------------------------------------------------------
// From class MDesCArray.
// Return number of items in listbox
// ----------------------------------------------------------------------------
//
TInt CSettingsItems::MdcaCount() const
    {
    return KNumberOfSettingsViewItems;
    }

// ---------------------------------------------------------------------------
// Creates the text string for the list box item and returns it
// ----------------------------------------------------------------------------
//
TPtrC CSettingsItems::MdcaPointL(
    TInt aIndex ) const
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::MdcaPoint"));
    _LIT(KImagePrintUiEmptyFormatString, " \t \t\t "); 
    TPtr ptr = iBuffer->Des();
    ptr.Zero();  
    HBufC* temp; 
    ptr.Append(KTab);
    switch ( aIndex )
        {
        case ESettingsListItemLayout:
            temp = StringLoader::LoadLC( R_USB_PRINT_LAYOUT );
            ptr.Append(temp->Des());
            CleanupStack::PopAndDestroy(temp);
            GetCurrentStringL( EDpsArgLayouts, KLayoutTable, KLayoutTableCount );
            break;     
            
        case ESettingsListItemPaperSize:
            temp = StringLoader::LoadLC( R_USB_PRINT_PAPER_SIZE ); 
            ptr.Append(temp->Des());
            CleanupStack::PopAndDestroy(temp); 
            GetCurrentStringL( EDpsArgPaperSizes, KPaperSizeTable, KPaperSizeTableCount );
            break;
            
        case ESettingsListItemPrintQuality:
            temp = StringLoader::LoadLC( R_USB_PRINT_QUALITY ); 
            ptr.Append(temp->Des());
            CleanupStack::PopAndDestroy(temp); 
            GetCurrentStringL( EDpsArgQualities, KQualityTable, KQualityTableCount );
            break;    
                  
        default:
            ptr.Append(KImagePrintUiEmptyFormatString); 
            break;
        }
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::MdcaPoint complete"));

    return iBuffer->Des(); 
    }
    
// ---------------------------------------------------------------------------
// Creates the text string for the list box item and returns it
// ----------------------------------------------------------------------------
//
TPtrC CSettingsItems::MdcaPoint(
    TInt aIndex ) const
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::MdcaPoint"));
    
    TRAPD(err, *iBuffer = MdcaPointL(aIndex));
    if(err)
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::MdcaPointL error"));
    	}
    else
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::MdcaPointL no  error"));
    	}
    return iBuffer->Des();	
    }


// ---------------------------------------------------------------------------
// Creates the text string for the list box item and returns it
// ----------------------------------------------------------------------------
//

void CSettingsItems::GetCurrentStringL( TInt aCapability, 
    const TConversionTable* aTable, const TInt aTableSize ) const
{
	FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::GetCurrentStringL START"));
    TPtr ptr = iBuffer->Des(); 
    ptr.Append(KTab);
    ptr.Append(KTab);  
    
    TInt value = iCapabilityManager->CurrentPrintSettings( aCapability ); 
    
    HBufC* temp; 
    for ( TInt i = 0; i < aTableSize; i++ )
        {
        if ( value == aTable[i].iCapabilityID )
            {
            temp = StringLoader::LoadLC( aTable[i].iResourceID );
            ptr.Append(temp->Des()); 
            CleanupStack::PopAndDestroy(temp); 
            break;
            }
        }
  FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::GetCurrentString END"));      
   
}
    
// ---------------------------------------------------------------------------
// From class MEikListBoxObserver.
// Handles the list box (enter) events by launching the pop-up list
// ----------------------------------------------------------------------------
//
void CSettingsItems::HandleListBoxEventL(
    CEikListBox* aListBox,
    TListBoxEvent aEventType )
    {
    if (( aEventType == EEventEnterKeyPressed ) || 
        ( aEventType == EEventItemDoubleClicked )) //for touch support
        {
        switch( aListBox->CurrentItemIndex() )
            {
            case ESettingsListItemLayout:
                HandleEvent(EDpsArgLayouts, KLayoutTable, 
                    KLayoutTableCount, iLayout); 
                break; 
            case ESettingsListItemPaperSize:
                HandleEvent(EDpsArgPaperSizes, KPaperSizeTable, 
                    KPaperSizeTableCount, iPaperSize); 
                break; 
            case ESettingsListItemPrintQuality: 
                HandleEvent(EDpsArgQualities, KQualityTable, 
                    KQualityTableCount, iQuality); 
                break;             
            default: 
                FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::HandleListBoxEventL default switch"));    
            	break;
            }    

        // Redraw item to the screen
        //    
        aListBox->DrawItem( aListBox->CurrentItemIndex() );   

        }
    }


// ---------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CSettingsItems::HandleEventL(TInt aCapability, 
    const TConversionTable* aTable, const TInt aTableSize, const HBufC* aTitle  )
    {
    HBufC* item; 
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::HandleEvent START"));
    RArray<TUint> capabilityArray = iCapabilityManager->GetCapabilitiesL(aCapability); 
    
    TInt currentItemIndex(0);
    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( KDefaultArrayValue );
    CleanupStack::PushL(items); 
    TInt j(0); 
    TInt helpValue(0);
    TInt currentItem = iCapabilityManager->CurrentPrintSettings(aCapability); 

    for(TInt i= 0; i < capabilityArray.Count(); i++ )
        {
        if ( capabilityArray[i] == currentItem )
        	{
            currentItemIndex = i; 
        	}
        for (j=0; j< aTableSize; j++ )
            {
            if ( capabilityArray[i] == aTable[j].iCapabilityID)
                {
                helpValue++;
                item = StringLoader::LoadL(aTable[j].iResourceID);
                CleanupStack::PushL(item); 
                items->AppendL(*item);
                CleanupStack::PopAndDestroy(item);
                break;    
                }
            }
        }    
    FTRACE(FPrint(_L("[IMAGEPRINTUI] CSettingsItems::HandleEventL helpValue is %x"), helpValue ));
    // Show the setting page
    
    CAknRadioButtonSettingPage* dlg = 
       new ( ELeave ) CAknRadioButtonSettingPage(
       R_USB_PRINT_MODE_SETTING_PAGE, currentItemIndex, items );
     CleanupStack::PushL(dlg);
     dlg->SetSettingTextL(*aTitle);
     CleanupStack::Pop(dlg); 
     
     if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        // User has made the selection. Now we need to save it to 
        // capabilityManager
        iCapabilityManager->SetValue( aCapability,  capabilityArray[currentItemIndex]); 
        }
    CleanupStack::PopAndDestroy(items); 
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::HandleEvent END"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CSettingsItems::HandleEvent(TInt aCapability, 
    const TConversionTable* aTable, const TInt aTableSize, const HBufC* aTitle  )
    {
    
    TRAPD(err, HandleEventL(aCapability,  aTable,  aTableSize, aTitle));
    if(err)
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::error in HandleEvenL"));
    	}
    else
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::HandleEvent successfull"));
    	}
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsItems::HandleEvent END"));
    }

// end of file
