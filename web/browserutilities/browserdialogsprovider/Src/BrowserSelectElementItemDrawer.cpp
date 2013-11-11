/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*
*/


// INCLUDE FILES
#include "BrowserSelectElementItemDrawer.h"
#include "BrowserSelectElementModel.h"
#include "BrCtlDialogsProvider.h"

#include <gulicon.h>
#include <AknIconArray.h>
#include <AknsUtils.h>

#include <data_caging_path_literals.hrh>

// CONSTANTS
#include "BrowserDialogsProviderConstants.h"


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CBrowserSelectElementItemDrawer::CBrowserSelectElementItemDrawer
// ----------------------------------------------------------------------------
//
CBrowserSelectElementItemDrawer::CBrowserSelectElementItemDrawer(
							MTextListBoxModel& aTextListBoxModel,
							const CFont& aFont,
							CFormattedCellListBoxData& aFormattedCellData,
							TBrCtlSelectOptionType aBrCtlSelectOptionType,
							const CArrayFix<TBrCtlSelectOptionData>& aOptions )
				: 
				CFormattedCellListBoxItemDrawer( &aTextListBoxModel,
													&aFont,
													&aFormattedCellData ),
				iOptions( aOptions ),
				iBrCtlSelectOptionType( aBrCtlSelectOptionType )
	{
	}

// ---------------------------------------------------------
// CBrowserSelectElementItemDrawer::LoadIconsL
// ---------------------------------------------------------
//
void CBrowserSelectElementItemDrawer::LoadIconsL()
	{
	TInt selected;
	TInt selectedm;
	TInt deselected;
	TInt deselectedm;
	CGulIcon* newIcon;
	CGulIcon* icon;
	CFbsBitmap *newIconBmp;
	CFbsBitmap *newIconMaskBmp;
	TAknsItemID selectOn(KAknsIIDQgnPropCheckboxOn);
	TAknsItemID selectOff(KAknsIIDQgnPropCheckboxOff);

	// selected option
	CArrayPtrFlat<CGulIcon>* icons =
					new ( ELeave ) CAknIconArray( KGranularityMedium );
	CleanupStack::PushL( icons );

	if ( iBrCtlSelectOptionType != ESelectTypeMultiple )
		{
		selected = EMbmAvkonQgn_prop_radiobutt_on;
		selectedm = EMbmAvkonQgn_prop_radiobutt_on_mask;
		deselected = EMbmAvkonQgn_prop_radiobutt_off;
		deselectedm = EMbmAvkonQgn_prop_radiobutt_off_mask;
		selectOn = KAknsIIDQgnPropRadiobuttOn;
		selectOff = KAknsIIDQgnPropRadiobuttOff;
		}
	else
		{
		selected = EMbmAvkonQgn_prop_checkbox_on;
		selectedm = EMbmAvkonQgn_prop_checkbox_on_mask;
		deselected = EMbmAvkonQgn_prop_checkbox_off;
		deselectedm = EMbmAvkonQgn_prop_checkbox_off_mask;
		}

    TParse* fp = new(ELeave) TParse(); 

    TInt err = fp->Set( AknIconUtils::AvkonIconFileName(), 
                        &KDC_RESOURCE_FILES_DIR, 
                        NULL ); 
    if ( err != KErrNone)
        {
        User::Leave( err );
        }   

    TFileName fileName = fp->FullName();
    delete fp;

	AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                                           selectOn,
                                           newIconBmp,
										   newIconMaskBmp,
                                           fileName,
                                           selected,
										   selectedm);    
	newIcon = CGulIcon::NewL( newIconBmp, newIconMaskBmp);
    CleanupStack::PushL( newIcon );
	icons->AppendL( newIcon );
    CleanupStack::Pop( newIcon );

	AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                                           selectOff,
                                           newIconBmp,
										   newIconMaskBmp,
                                           fileName,
                                           deselected,
										   deselectedm);    
	newIcon = CGulIcon::NewL( newIconBmp, newIconMaskBmp);
    CleanupStack::PushL( newIcon );
	icons->AppendL( newIcon );
    CleanupStack::Pop( newIcon );

	// optiongroup
	CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
	CleanupStack::PushL( bitmap );
    CleanupStack::PushL( newIcon );
	User::LeaveIfError( bitmap->Create(TSize(1,1),EColor64K));
    CleanupStack::Pop( newIcon );

    CFbsBitmap* mask = new ( ELeave ) CFbsBitmap;
	CleanupStack::PushL( mask );
	User::LeaveIfError( mask->Create(TSize(1,1),EGray2));
	
	icon = CGulIcon::NewL( bitmap, mask );
	CleanupStack::PushL( icon );
	icons->AppendL( icon );
	
    CleanupStack::Pop( icon );
	
	CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );

	CleanupStack::Pop( icons );
	ColumnData()->SetIconArray( icons );
	}

// ----------------------------------------------------------------------------
// CBrowserSelectElementItemDrawer::DrawItem
// ----------------------------------------------------------------------------
//
void CBrowserSelectElementItemDrawer::DrawItem( TInt aItemIndex,
	                                            TPoint aItemRectPos,
	                                            TBool aItemIsSelected,
	                                            TBool aItemIsCurrent,
	                                            TBool aViewIsEmphasized,
	                                            TBool aViewIsDimmed ) const
	{
	TBool selected = aItemIsSelected;
	TBool currentIsOptGr = EFalse; 

	if( ((CBrowserSelectElementModel*)iModel)->Filter() )
		{
		if ((iOptions.At(((CBrowserSelectElementModel*)iModel)->Filter()
		->FilteredItemIndex(aItemIndex)).IsOptGroup() ))
			{ 
			currentIsOptGr = ETrue;
			selected = EFalse;
			}
		}

	CFormattedCellListBoxItemDrawer::DrawItem(
		aItemIndex,
		aItemRectPos,
		selected,
		aItemIsCurrent,
		aViewIsEmphasized,
		aViewIsDimmed );

	// this separator isn't specified in details
	if ( aItemIndex && currentIsOptGr )
		{
		iGc->SetPenColor( AKN_LAF_COLOR( KBrCtlOptgroupSeparatorColor ) );
		iGc->DrawLine( aItemRectPos + TSize( KBrCtlOptgroupSeparatorOffsetX1 ,
											 KBrCtlOptgroupSeparatorOffsetY ),
					   aItemRectPos + TSize( KBrCtlOptgroupSeparatorOffsetX2 ,
											 KBrCtlOptgroupSeparatorOffsetY ) );
		}
	}

// End of file
