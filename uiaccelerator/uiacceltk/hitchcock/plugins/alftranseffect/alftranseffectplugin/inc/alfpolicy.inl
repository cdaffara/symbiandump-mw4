/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Inline functions for alfpolicy.h
*
*/




inline TUint CFullscreenPolicy::Action()
	{
	return iAction;
	}

inline const TUid& CFullscreenPolicy::Uid()
	{
	return iUid;
	}
	
inline const TThreadId& CFullscreenPolicy::ThreadId()
	{
	return iThreadId;
	}
		
inline TDesC& CFullscreenPolicy::ResourceDir()
	{
	return *iResourceDir;
	}
	
inline TDesC& CFullscreenPolicy::Filename()
	{
	return *iFilename;
	}

inline TInt CFullscreenPolicy::CachePriority()
	{
	return iCachePriority;
	}

inline void CFullscreenPolicy::SetCachePriority(TInt aCachePriority)
	{
	iCachePriority = aCachePriority;
	}

inline TInt CFullscreenPolicy::WantedTime()
	{
	return iWantedTime;
	}
	
inline TInt CFullscreenPolicy::MinTime()
	{
	return iMinTime;
	}

inline TDesC& CActionPolicy::ActionString()
	{
	return *iActionString;
	}

inline const TUid& CControlPolicy::Uid()
	{
	return iUid;
	}
	
inline const TThreadId& CControlPolicy::ThreadId()
	{
	return iThreadId;
	}

inline TDesC& CControlPolicy::ResourceDir()
	{
	return *iResourceDir;
	}

inline TDesC& CControlPolicy::Filename()
	{
	return *iFilename;
	}

inline TInt CControlPolicy::CachePriority()
	{
	return iCachePriority;
	}

inline void CControlPolicy::SetCachePriority(TInt aCachePriority)
	{
	iCachePriority = aCachePriority;
	}

inline TInt CControlPolicy::WantedTime()
	{
	return iWantedTime;
	}
	
inline TInt CControlPolicy::MinTime()
	{
	return iMinTime;
	}
	
inline const TUid& CListBoxPolicy::Uid() const
	{
	return iUid;
	}

inline const TThreadId& CListBoxPolicy::ThreadId() const
	{
	return iThreadId;
	}

inline const CAlfTransitionServerClient::TListBoxType CListBoxPolicy::ListBoxType() const
    {
    return iListBoxType;
    }

inline const TDesC& CListBoxPolicy::ResourceDir() const
	{
	return iResourceDir->Des();
	}

inline const TDesC& CListBoxPolicy::BackgroundFilename() const
	{
	return iBackgroundFilename->Des();
	}

inline const TDesC& CListBoxPolicy::ItemFilename() const
	{
	return iItemFilename->Des();
	}

inline TInt CListBoxPolicy::CachePriority() const
	{
	return iCachePriority;
	}

inline void CListBoxPolicy::SetCachePriority( TInt aCachePriority )
	{
	iCachePriority = aCachePriority;
	}

inline TInt CListBoxPolicy::WantedTime() const
	{
	return iWantedTime;
	}

inline TInt CListBoxPolicy::MinTime() const
	{
	return iMinTime;
	}

inline TInt CListBoxPolicy::Specificity() const
    {
    return iSpecificity;
    }

// --- EOF ---
