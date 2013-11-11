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
* Description:   a policy
*
*/


#ifndef C_POLICY_H
#define C_POLICY_H

#include <e32base.h>

//#include <gfxtranseffect/gfxtransdatatype.h>

#include "alftransitionserver.h"

/**
 *  contains a policy for a fullscreen transition type.
 *
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS(CFullscreenPolicy) : public CBase
	{
public:
  
	static CFullscreenPolicy* NewL(	TUint aAction, 
									const TUid& aUid, 
									const TThreadId& aThreadId, 
									const TDesC& aResourceDir, 
					 				const TDesC& aFilename, 
					 				TInt aCachePriority, 
					 				TInt aWantedTime, 
					 				TInt aMinTime);
	
	virtual ~CFullscreenPolicy();
		
	/**
     * Returns the currently stored action.
     * @since S60 3.2
     *
     * @return The action.
    */  
	inline TUint Action();

	/**
     * Returns the UID stored in this object.
     * @since S60 3.2
     *
     * @return Reference to the Uid.
    */  
	inline const TUid& Uid();
	
	/**
     * Returns the thread ID stored in this object.
     * @since S60 3.2
     *
     * @return Reference to the thread ID.
    */  
	inline const TThreadId& ThreadId();
	/**
     * Returns a pointer to the name of the stored resource directory.
     * @since S60 3.2
     *
     * @return Pointer to the resource directory.
    */  
	inline TDesC& ResourceDir();

	/**
     * Returns the currently stored filename.
     * @since S60 3.2
     *
     * @return Pointer to the filename.
    */  
	inline TDesC& Filename();
	
	/**
     * Returns the cache priority of this object.
     * @since S60 3.2
     *
     * @return Integer containing the cache priority.
    */  
	inline TInt CachePriority();
	
	/**
     * Sets the cache priority
     * @since S60 3.2
     *
     * @return Integer containing hte cache priority.
    */  
	inline void SetCachePriority(TInt aCachePriority);
	
	/**
     * Returns the currently set wanted time.
     * @since S60 3.2
     *
     * @return Integer containing the wanted time.
    */  
	inline TInt WantedTime();
	
	/**
     * Return the minimum time.
     * @since S60 3.2
     *
     * @return Integer containing the minimum time.
    */  
	inline TInt MinTime();
	
private:
	/**
     * Private constructor.
     * @since S60 3.2
     *
     * @param aAction The action.
     * @param aUid The Uid.
     * @param aCachePriority The cachepriority.
     * @param aWantedTime The wanted time
     * @param aMinTime The minimum time.
    */  
	CFullscreenPolicy(TUint aAction, 
					  const TUid& aUid, 
					  const TThreadId& aThreadId,
					  TInt aCachePriority, 
					  TInt aWantedTime, 
					  TInt aMinTime);

	/**
     * Private part of two phase construction.
     * @since S60 3.2
     *
     * @param aResourceDir Descriptor containing the resource directory.
     * @param aFilename Descriptor containing the filename.
    */  
	void ConstructL(const TDesC& aResourceDir, const TDesC& aFilename);
	
private: //data
	TUint iAction;
	TUid iUid;
	TThreadId iThreadId;
	TInt iCachePriority; 	//below zero means default
	TInt iWantedTime; 		//below zero means default
	TInt iMinTime;			//below zero means default
	HBufC* iFilename;
	HBufC* iResourceDir;
	};

NONSHARABLE_CLASS( CListBoxPolicy ) : public CBase
	{
public:
	static CListBoxPolicy* NewL( const TUid& aUid, 
								 const TThreadId& aThreadId,
	                             const CAlfTransitionServerClient::TListBoxType aListBoxType,
	                             const TDesC& aResourceDir,
								 const TDesC& aBackgroundFilename,
								 const TDesC& aItemFilename,
								 TInt aCachePriority, 
					             TInt aWantedTime, TInt aMinTime );
	~CListBoxPolicy();
	void ConstructL( const TDesC& aResourceDir,
	                 const TDesC& aBackgroundFilename,
	                 const TDesC& aItemFilename );
	
	/**
     * Returns the UID stored in this object.
     *
     * @return Reference to the Uid.
    */  
	inline const TUid& Uid() const;
	
	/**
     * Returns the thread ID stored in this object.
     *
     * @return Reference to the thread ID.
    */  
	inline const TThreadId& ThreadId() const;

    /**
     * Returns the list box type of this policy.
     *
     * @return The list box type.
     */
    inline const CAlfTransitionServerClient::TListBoxType ListBoxType() const;

	/**
     * Returns a pointer to the name of the stored resource directory.
     *
     * @return Pointer to the resource directory.
    */  
	inline const TDesC& ResourceDir() const;

	/**
     * Returns the currently stored background filename.
     *
     * @return Pointer to the background filename.
    */  
	inline const TDesC& BackgroundFilename() const;

	/**
     * Returns the currently stored item filename.
     *
     * @return Pointer to the item filename.
    */  
	inline const TDesC& ItemFilename() const;

	/**
     * Returns the cache priority of this object.
     *
     * @return Integer containing the cache priority.
    */  
	inline TInt CachePriority() const;

	/**
     * Sets the cache priority
     *
     * @return Integer containing hte cache priority.
    */  
	inline void SetCachePriority( TInt aCachePriority );

	/**
     * Returns the currently set wanted time.
     *
     * @return Integer containing the wanted time.
    */  
	inline TInt WantedTime() const;

	/**
     * Return the minimum time.
     *
     * @return Integer containing the minimum time.
    */  
	inline TInt MinTime() const;

	/**
	 * Return the specificity value of the policy, ie. how much information
	 * is specified.  This depends on hether the app uid and the listbox  
	 * type are specified.
	 *
	 * @return The specificity of the policy.
	 */
    inline TInt Specificity() const;

private:
	CListBoxPolicy( const TUid &aUid, 
					const TThreadId &aThreadId,
	                const CAlfTransitionServerClient::TListBoxType aListBoxType,
	                TInt aCachePriority,
	                TInt aWantedTime, TInt aMinTime );


	TUid iUid;
	TThreadId iThreadId;
	CAlfTransitionServerClient::TListBoxType iListBoxType;
	HBufC* iResourceDir;
	HBufC* iBackgroundFilename;
	HBufC* iItemFilename;
	TInt iCachePriority; 	//below zero means default
	TInt iWantedTime; 		//below zero means default
	TInt iMinTime;			//below zero means default
    TInt iSpecificity;
	};

/**
 * This class contains one policy action for a policy.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CActionPolicy ) : public CBase
	{
public:
	~CActionPolicy();

	TInt iAction;		// The action that is supported.
	TPolicy iPolicy;	// Policy type (supported/not supported)
	
	/**
	 * Stores the action string that will be executed whith this action
	 * @sine S60 v3.1
	 *
	 * param Descriptor containing the action.
	 */
	void SetActionStringL(const TDesC& aActionString);
	
	/**
	 * Returns the action to be performed.
	 * since S60 v3.1
	 * 
	 * return Pointer to the action sting.
	 */
	inline TDesC& ActionString();
	
private:
	HBufC* iActionString;	// Buffer where the action string will be stored.
	};
	
/**
 * This class contains base information on control policies supported
 * like resource directory, filename and UID of the control.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS(CControlPolicy) : public CBase
	{
public:
	/**
     * Parts of two-stage construction. Called when this class is instantiated.
     * @since S60 3.2
     *
     * @param UID for the control policy for which data is to be created.
     * @param Directory where the KML file is located.
     * @param Name of the KML file.
     * @return Pointer to a newly created CControlPolicy object.
    */  
	static CControlPolicy* NewL(const TUid &aUid, 
								const TThreadId& aThreadId, 
								const TDesC& aResourceDir, 
						  		const TDesC& aFilename, 
						  		TInt aCachePriority,
						  		TInt aWantedTime,
						  		TInt aMinTime);
	/**
     * Destructor
     * @since S60 3.2
     *
    */  
	~CControlPolicy();
	
	// Array containing action policies that are supported for
	// this UID.
	RPointerArray<CActionPolicy> iControlPolicies;

	/**
     * Returns the UID that is stored in this control policy.
     * @since S60 3.2
     *
     * @return the UID.
    */  
	inline const TUid& Uid();
	
	/**
     * Returns the thread ID that is stored in this control policy.
     * @since S60 3.2
     *
     * @return the thread ID.
    */  
	inline const TThreadId& ThreadId();

	/**
     * Returns the resource dir where the KML files are located 
     * for this UID.
     * @since S60 3.2
     *
     * @return Pointer to the stored resource directory.
    */  
	inline TDesC& ResourceDir();

	/**
     * Returns the filename associated with this uid.
     * @since S60 3.2
     *
     * @return Pointer to the filename.
    */  
	inline TDesC& Filename();
	
	/**
     * Returns the cache priority of this object.
     * @since S60 3.2
     *
     * @return Integer containing the cache priority.
    */  
	inline TInt CachePriority();
	
	/**
     * Sets the cache priority
     * @since S60 3.2
     *
     * @return Integer containing hte cache priority.
    */  
	inline void SetCachePriority(TInt aCachePriority);
	
	/**
     * Gets the wanted frame time
     * @since S60 3.2
     *
     * @return Integer containing the frame time in ms
    */  
	inline int WantedTime();

	/**
     * Gets the minumum frame time
     * @since S60 3.2
     *
     * @return Integer containing the frame time in ms
    */  
	inline int MinTime();
	
private:
	/**
     * Constructor.
     * @since S60 3.2
     *
     * @param UID for this policy.
     * @param ThreadID for this policy.
    */  
	CControlPolicy(const TUid aUid, const TThreadId aThreadId, TInt aCachePriority, TInt aWantedTime, TInt aMinTime);

	/**
     * Part of two stage constuction.
     * @since S60 3.2
     *
     * @param Descriptor containing the resource directory.
     * @param Descriptor containing the filename.
    */  
	void ConstructL(const TDesC& aResourceDir, const TDesC& aFilename);

private: // Data
	TUid iUid;
	TThreadId iThreadId;
	TInt iCachePriority;
	TInt iWantedTime;
	TInt iMinTime;
	HBufC* iResourceDir;
	HBufC* iFilename;
	};
	
#include "alfpolicy.inl"

#endif // C_POLICY_H
