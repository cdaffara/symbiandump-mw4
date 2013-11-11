/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*/


#ifndef __BACKUPOBSERVER_H__
#define __BACKUPOBSERVER_H__

#include <e32base.h>
#include <e32property.h>

NONSHARABLE_CLASS(CBackupObserver) : public CActive
{
public:

/*!
  @function NewL
   
  @discussion Create a CBackupObserver object
  @result a pointer to the created instance of CBackupObserver
  */
    static CBackupObserver* NewL();

/*!
  @function NewLC
   
  @discussion Create a CBackupObserver object
  @result a pointer to the created instance of CBackupObserver
  */
    static CBackupObserver* NewLC();

/*!
  @function ~CBackupObserver
  
  @discussion Destroy the object and release all memory objects
  */
    ~CBackupObserver();

protected: // from CActive

/*!
  @function DoCancel
  
  @discussion Cancel any outstanding requests
  */
    void DoCancel();

/*!
  @function RunL
  
  @discussion Respond to an event
  */
    void RunL();

private:
    
/*!
  @function CBackupObserver

  @discussion Constructs this object
  */
    CBackupObserver();

/*!
  @function ConstructL

  @discussion Performs second phase construction of this object
  */
    void ConstructL();

private:
    // Member variables
    RProperty       iBackupStatus;
};

#endif // __BACKUPOBSERVER_H__

