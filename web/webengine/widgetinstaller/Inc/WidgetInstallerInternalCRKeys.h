/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CR Keys
*
*/


#ifndef WIDGETINSTALLERINTERNALCRKEYS_H
#define WIDGETINSTALLERINTERNALCRKEYS_H


// The setting for not installing clearly incompatible widgets
// Valid values: 0 = disabled, 1 = enabled
const TUint32 KWidgetInstallerStrictMode =            0x00000001;

// The setting to control prompting user when widget needs network access
// Valid values: 0 = allow access without prompting, 1 = prompt user to allow (grant) access
const TUint32 KWidgetPomptForNetworkAccess =            0x00000002;

#endif
