/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef DOWNLOADMGRCRKEYS_H
#define DOWNLOADMGRCRKEYS_H

const TUid KCRUidDldMgr = {0x10008D60};

// Setting indicate whether there were ongoing download(s) on power, hardware or software failure.
// If set to 0, Download Manager will NOT be started on Browser Start-up,
// If set to 1, Download Manager will be started on Browser Start-up,
// Valid values: 0, 1
const TUint32 KDownloadMgrHasActiveDownloads = 0x0000005;

// Setting for auto-accept of COD/OMA downloads, removing need for user dialog to confirm.
// Valid values: 0 (user confirms), 1 (auto-accept, no dialog)
const TUint32 KDownloadMgrAutoAcceptCod = 0x0000006;

#endif      // DOWNLOADMGRCRKEYS_H