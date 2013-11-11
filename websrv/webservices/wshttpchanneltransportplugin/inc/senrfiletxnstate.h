/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    
*
*/




#ifndef WSF_RFILE_TXN_STATE_H
#define WSF_RFILE_TXN_STATE_H

//  INCLUDES
#include <e32base.h>
#include "sentxnstate.h"
#include "senhttpchannelimpl.h"

// CLASS DECLARATION
/**
 * Class for handling the transaction state in the http client class.
 * Implements the MHTTPDataSupplier interface.
 *	
 */
class CSenRfileTxnState : public CSenTxnState
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     */
    static CSenRfileTxnState* NewLC(MSenResponseObserver& aObserver,
                               //RFileLogger* aLog,
                               const TDesC8* aUri,
                               CSenHttpChannelImpl* aHttpClient,
                               const TDesC8& aContentType,
                               const RFile& aContent);

    /**
     * Two-phased constructor.
     *
     */
    static CSenRfileTxnState* NewL(MSenResponseObserver& aObserver,
                              //RFileLogger* aLog,
                              const TDesC8* aUri,
                              CSenHttpChannelImpl* aHttpClient,
                              const TDesC8& aContentType,
                              const RFile& aContent);

    /**
     * Virtual destructor.
     *
     */
    virtual ~CSenRfileTxnState();

        // New functions

    // Functions from base classes

    /**
     * From MHTTPDataSupplier::GetNextDataPart.
     * Obtain a data part from the supplier.  The data is guaranteed
     * to survive until a call is made to ReleaseData().
     * @param aDataPart The data part.
     * @return ETrue if this is the last part, EFalse otherwise.
     */
    virtual TBool GetNextDataPart(TPtrC8& aDataPart);

    /**
     * From MHTTPDataSupplier::ReleaseData.
     * Release the current data part being held at the data
     * supplier.  This call indicates to the supplier that the part
     * is no longer needed, and another one can be supplied, if
     * appropriate.
     */
    virtual void ReleaseData();

    /**
     * From MHTTPDataSupplier::OverallDataSize.
     * Obtain the overall size of the data being supplied, if known
     * to the supplier.  Where a body of data is supplied in several
     * parts this size will be the sum of all the part sizes. If
     * the size is not known, KErrNotFound is returned; in this case
     * the client must use the return code of GetNextDataPart to find
     * out when the data is complete.
     *
     * @return A size in bytes, or KErrNotFound if the size is not known.
     */
    virtual TInt OverallDataSize();

    /**
     * From MHTTPDataSupplier::Reset.
     * Reset the data supplier.  This indicates to the data supplier
     * that it should return to the first part of the data.
     * This could be used in a situation where the data consumer has
     * encountered an error and needs the data to be supplied afresh.
     * Even if the last part has been supplied (i.e. GetNextDataPart
     * has returned ETrue), the data supplier should reset to the
     * first part. If the supplier cannot reset it should return an
     * error code; otherwise it should return KErrNone, where the
     * reset will be assumed to have succeeded.
     *
     * @return KErrNone if successful, otherwise another error code.
     */
    virtual TInt Reset();
    
protected:
    /**
     * C++ default constructor.
     *
     */
    CSenRfileTxnState(MSenResponseObserver& aObserver);//,
                 //RFileLogger* aLog);
   
    /**
     * File logger.
     *
     */
    //RFileLogger* Log() const; 
private://data  
     
	RFile iFile;
	CSenHttpChannelImpl *iHttpClient ;
	TBool iMoreContent;
	HBufC8* iReadData;
	/**Message content data pointer for POSTs.Not own.*/
//	const TDesC8* iContent;
	TInt iSize;
/**
     * Return flag if more body to be send. Only in chunked mode.
     * 
     * @since S60 ?S60_version
     * @return True if more body to be send.
     */    
    TBool MoreBody() const;	
	
	
private:

    /**
     * 2nd phase constructor.
     *
     */
    void ConstructL(const TDesC8* aUri,
                    CSenHttpChannelImpl* aHttpClient,
                    const TDesC8& aContentType,
                    const RFile& aContent);


    };

#endif // WSF_TXN_STATE_H

// End of File
