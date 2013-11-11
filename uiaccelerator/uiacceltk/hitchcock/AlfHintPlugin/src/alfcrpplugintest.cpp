/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

void CWsGraphicBase::DoTestDrawInvalidBitmapIDL()
	{
	iTestLog.Append(_L("DrawInvalidBitmapID"));
 		
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;
	CFbsBitmap *bitmap2 = NULL;
	CFbsBitmap *mask2 = NULL;
	
	User::LeaveIfError(bitmap1.Load(MY_TEST_BITMAP,0));
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
	
	// valid bitmap
	CWsGraphicBitmap* bTest = CWsGraphicBitmap::NewL(&bitmap1,&mask1);
	
	// invalid bitmap
	CWsGraphicBitmap* bTest2 = CWsGraphicBitmap::NewL(bitmap2,mask2);
 	
	PrepGc();
	iGc->DrawWsGraphic(bTest->Id(),iPosition1);
	iGc->DrawWsGraphic(bTest2->Id(),iPosition2);
	RetireGc();
	
	// compare the graphic in both positions, should only be graphic in position 1
	TestForDifferentBitmaps();
	
	delete bTest2;
	delete bTest;
	}
	