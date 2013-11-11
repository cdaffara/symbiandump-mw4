/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/








#ifndef WSSTAR_ADDRESSING_EPR_H
#define WSSTAR_ADDRESSING_EPR_H



// INCLUDES
#include <e32std.h>
#include <SenXmlElement.h>
#include "wsstarmessagehandlerscons.h"

// CONSTANTS

// FORWARD DECLARATIONS


/**
 * Class:       
 *
 */
class CWSStarAddressingEPR : public CSenXmlElement
    {
public:
    /**
     * Function:    NewL
     *
     * Description: Create instance of concrete implementation. Note that ECOM
     *              interface implementations can only have two signatures for
     *              NewL:
     *
     * Returns:    Instance of this class.
     *
     * Note:       The interface, which is abstract base class of this
     *             implementation, also provides NewL method. Normally abstract
     *             classes do not provide NewL, because they cannot create
     *             instances of themselves.
     */
    static CWSStarAddressingEPR* NewL(CSenElement& aParent, const TDesC8& aLocalName, 
                                        const TDesC8& aContent, const TDesC8& aVersion);
    static CWSStarAddressingEPR* NewLC(CSenElement& aParent, const TDesC8& aLocalName, 
                                        const TDesC8& aContent, const TDesC8& aVersion);


private:

    
public: // destructor
    virtual ~CWSStarAddressingEPR();


protected:
    /**
     * Function:   CWSStarHandlerAddressingInbound
     *
     * Discussion: Perform the first phase of two phase construction
     */
    CWSStarAddressingEPR();

    /**
     * Function:   ConstructL
     *
     * Discussion: Perform the second phase construction of a
     *             CImplementationClassPlus object.
     */
    void ConstructL(CSenElement& aParent, const TDesC8& aLocalName, const TDesC8& aContent, 
                        const TDesC8& aVersion);

public:
    void AddDeviceAddressL(const TDesC8& aMwsNamespace, const TDesC8& aContent);
private:

//data
    };

#endif // WSSTAR_ADDRESSING_ERP_H
