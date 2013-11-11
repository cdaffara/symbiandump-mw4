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
* Description:  A class that fetches resources via RFile.
*
*/


#include <e32base.h>
#include <e32std.h>
#include <f32file.h>

#include "FileHandler.h"
#include "LeakTracker.h"
#include "Logger.h"


// -----------------------------------------------------------------------------
// CFileHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFileHandler* CFileHandler::NewL()
    {
    CFileHandler* self = new (ELeave) CFileHandler();
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CFileHandler::CFileHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFileHandler::CFileHandler():
        iLeakTracker(CLeakTracker::EFileHandler)
    {
    }
        

// -----------------------------------------------------------------------------
// CFileHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFileHandler::ConstructL()
    {
    iIdle = CIdle::NewL(CActive::EPriorityIdle);
    }        


// -----------------------------------------------------------------------------
// CFileHandler::~CFileHandler
// Deconstructor.
// -----------------------------------------------------------------------------
//
CFileHandler::~CFileHandler()
    {
    delete iIdle;    
    delete iUrl;
    }


// -----------------------------------------------------------------------------
// CFileHandler::LoadUrl
// 
// Loads the given url -- asynchronously
// -----------------------------------------------------------------------------
//
void CFileHandler::LoadUrlL(const TDesC& aUrl, MLoadObserver& aObserver)
    {
    iUrl = aUrl.AllocL();
    iObserver = &aObserver;

    // To be consistent with other asynchronous UrlHandlers the buffer isn't 
    // processed until after the callstack unrolls.
    iIdle->Start(TCallBack(LoadCompleted, this));
    }


// -----------------------------------------------------------------------------
// CFileHandler::LoadCompleted
// 
// Loads and passes the buffer to the observer.
// -----------------------------------------------------------------------------
//
TInt CFileHandler::LoadCompleted(TAny* aPtr)
    {
    CFileHandler*  self = static_cast<CFileHandler*>(aPtr);    
    HBufC8*        buffer = NULL;

    TRAPD(err, buffer = self->LoadCompletedHelperL());
    
    // Pass the buffer to the observer.    
    self->iObserver->LoadCompleted(err, buffer, KNullDesC, KNullDesC);

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CFileHandler::LoadCompletedHelperL
// 
// Loads and passes the buffer to the observer.
// -----------------------------------------------------------------------------
//
HBufC8* CFileHandler::LoadCompletedHelperL()
    {
    _LIT(KFileScheme, "file://");

    TInt           size;
    HBufC16*       correcttedPath = NULL;
    HBufC8*        buffer = NULL;
    TPtr8          bufferPtr(NULL, 0);

    // Extract the file path from the url.
    TPtr  path(iUrl->Des());

    if (iUrl->Find(KFileScheme) == 0)
        {
        path.Set((TUint16*) iUrl->Ptr() + KFileScheme().Length(), 
                iUrl->Length() - KFileScheme().Length(), iUrl->Length() - 
                KFileScheme().Length());

        correcttedPath = HBufC16::NewLC(path.Length());
        correcttedPath->Des() = path;
        path.Set(correcttedPath->Des());

        for (TInt i = 0; i < path.Length(); i++)
            {
            if (path[i] == '/')
                {
                path[i] = '\\';
                }
            }
        }

    // Otherwise leave as this isn't a file url.
    else
        {
        User::Leave(KErrNotSupported);
        }

    // Open the file.
    RFs    rfs;
    RFile  file;

    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);

    User::LeaveIfError(file.Open(rfs, path, EFileRead | EFileShareReadersOnly));
    CleanupClosePushL(file);

    // Read file
    User::LeaveIfError(file.Size(size));
    
    buffer = HBufC8::NewL(size);
    bufferPtr.Set(buffer->Des());

    file.Read(bufferPtr, size);
    
    // Clean up.
    CleanupStack::PopAndDestroy(/*file*/);
    CleanupStack::PopAndDestroy(/*rfs*/);
    CleanupStack::PopAndDestroy(correcttedPath);

    return buffer;
    }
