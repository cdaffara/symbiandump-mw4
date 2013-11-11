/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline method definitions of class TCodDownloadImpl.   
*
*/


#ifndef COD_DOWNLOAD_IMPL_INL
#define COD_DOWNLOAD_IMPL_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodDownloadImpl::CCodDownloadImpl()
// ---------------------------------------------------------
//
CCodDownloadImpl::CCodDownloadImpl
    (
    CCodDownload& aProxy,
    MCodDownloadObserver& aObserver,
    CEikonEnv* aEikEnv
    )
: CActive( CActive::EPriorityStandard ),
  iState( EInit ),
  iCod( EFalse ),
  iRoap( EFalse ),
  iProxy( aProxy ),
  iObserver( aObserver ),
  iEikEnv( aEikEnv ),
  iAutoAccept( EFalse ),
  iSilentMode( EFalse ),
  iSuppressNextUrl( EFalse ),
  iSuppressLaunch( EFalse ),
  iFota( EFalse )
    {
    CActiveScheduler::Add( this );
    ResetProgress();
    }

// ---------------------------------------------------------
// CCodDownloadImpl::ResetProgress()
// ---------------------------------------------------------
//
void CCodDownloadImpl::ResetProgress()
    {
    iProgress.iState = TCodDownloadProgress::EInit;
    iProgress.iError = KErrNone;
    iProgress.iBytesCur = 0;
    iProgress.iBytesMax = 0;
    }

#endif /* def COD_DOWNLOAD_IMPL_INL */
