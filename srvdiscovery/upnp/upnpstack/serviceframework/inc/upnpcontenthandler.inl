/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Defines inline funtions for CUpnpContentHandler class
 *
*/


// -----------------------------------------------------------------------------
// CUpnpContentHandler::IsTagRepeatedL
// Returns ETrue if tag on aPosition has been already found.
// If not just set that now it is found.
// -----------------------------------------------------------------------------
//
template <class T> inline TBool CUpnpContentHandler::IsTagRepeated(
    TInt aPosition, TBitFlagsT<T>& aFoundTags )
    {
    if ( aFoundTags.IsSet( aPosition ) )
        {
        return ETrue;
        }
    aFoundTags.Set( aPosition );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandler::RepeatedTagCheckL
// Leaves if tag on aPosition has been already found.
// If not just set that now it is found.
// -----------------------------------------------------------------------------
//
template <class T> inline void CUpnpContentHandler::RepeatedTagCheckL(
    TInt aPosition, TBitFlagsT<T>& aFoundTags )
    {
    if ( IsTagRepeated( aPosition, aFoundTags ) )
        {
        User::Leave( KErrArgument ); //maxOccurents=1
        }
    }

//  End of File
