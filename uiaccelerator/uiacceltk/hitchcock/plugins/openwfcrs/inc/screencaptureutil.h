// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __SCREENCAPTURERENDERSTAGE_H__
#define __SCREENCAPTURERENDERSTAGE_H__

#include <graphics/wsrenderstage.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include <graphics/surfaceupdateclient.h>

class MWsScene;
class MWsDisplayControl;
class MWsDisplayMapping;
class MWsScreenDevice;

/**
This is the implementation of CWsRenderStage which is created by a CScreenCaptureUtilFactory
*/

class CScreenCaptureUtil : public CBase
	{
public:
	static CScreenCaptureUtil * NewL(CWsRenderStage* aOwner);
	~CScreenCaptureUtil();

	CScreenCaptureUtil();
	void ConstructL(CWsRenderStage* aOwner);
	void ComposeScreenL(TInt aHandle);
	void CreateLocalSurfaceL();
	void GetCompositionAreaSize(TSize& aSize) const;
	TSurfaceId CreateSurfaceL(const TSize& aSize, TUidPixelFormat aPixelFormat, TInt aStride);
	TBool VerifyBitmapFormatL(const CFbsBitmap& aBitmap);
	TInt CopySurfaceToBitmapL(CFbsBitmap& aCopyToBitmap);
	
private:
	MWsScene* iScene;
	MWsDisplayMapping* iDisplayMapping;
	MWsScreenDevice* iScreenDevice;
	TSurfaceId iLocalSurface;
	RSurfaceManager iSurfaceManager;
	RSurfaceUpdateSession iSurfaceUpdateSession;
	RTimer iTimer;
	static const TUidPixelFormat KSurfacePixelFormat = EUidPixelFormatXRGB_8888;
	static const TInt KSurfaceBytesPerPixel = 4;
	};

#endif //__SCREENCAPTURERENDERSTAGE_H__
