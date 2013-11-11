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
* Description:   Cleans vpn settings
*
*/

#ifndef __VPNCLEANER_H__
#define __VPNCLEANER_H__


class RCmManagerExt;


class TVpnCleaner{
  public:
    TVpnCleaner();
    ~TVpnCleaner();
 
    /**
     * Cleans vpn settings
     */
    void Clean();

  private:
    /**
     * Removes vpn access points
     */
    void CleanAPsL();

    /**
     * Removes vpn access points
     */
    void DelAPsL( RCmManagerExt& aCm );
};


#endif  // __VPNCLEANER_H__
