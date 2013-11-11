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
* Description:   Mixin class for objects to interface with the FEP
*
*/



#ifndef C_ALFINPUTCAPABILITIESSUPPLIER_H
#define C_ALFINPUTCAPABILITIESSUPPLIER_H

#include <e32base.h>
class TCoeInputCapabilities;

/**
* This mixin allows objects to be handled within the Alf system to 
* interface to a FEP.  Objects implementing this mixin are likely to be those
* implementing editor functionality
*/ 
class MAlfInputCapabilitiesSupplier
    {
    public:

    /**
    * The method that will be called by CAlfDisplayCoeControl on the currently focused editor
    * in its implementation of CCoeControl::InputCapabilities().
    *
    * @return   Cone-defined FEP interface structure
    */
    virtual TCoeInputCapabilities InputCapabilities() const = 0;

    /**
    * Extension interface factory
    */
    virtual TAny* MAlfInputCapabilitiesSupplier_ExtendedInterface( const TUid& aInterfaceUid ) const = 0;    
    };

#endif  // C_ALFINPUTCAPABILITIESSUPPLIER_H
