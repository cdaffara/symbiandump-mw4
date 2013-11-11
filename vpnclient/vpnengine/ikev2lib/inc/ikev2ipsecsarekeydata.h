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
* Description:  Rekey data
*
*/

#ifndef IKEV2IPSECSAREKEYDATA_H_
#define IKEV2IPSECSAREKEYDATA_H_

#include <e32base.h>
#include "ipsecsalifetime.h"
#include "ikev2trafficselector.h"

class TIkeV2TrafficSelector;

NONSHARABLE_CLASS(CIpsecSARekeyData) : public CBase
    {
public:
        static CIpsecSARekeyData* NewL(const TInt aReplayWindow, 
                                       const TIpsecSALifetime* aHard, 
                                       const TIpsecSALifetime* aSoft,
                                       const CArrayFix<TIkeV2TrafficSelector>& aTS_i, 
                                       const CArrayFix<TIkeV2TrafficSelector>& aTS_r, 
                                       const TDesC8& aLocalId, const TDesC8& aRemoteId);
        ~CIpsecSARekeyData();
        
        TInt ReplayWindow() const;
        TIpsecSALifetime HardLifetime() const;
        TIpsecSALifetime SoftLifetime() const;
        
        const TPtrC8 LocalId() const;
        const TPtrC8 RemoteId() const;
        
        /**
         *  Copies the initiator side traffic selectors.
         *  Ownership is transferrer
         */
        CArrayFix<TIkeV2TrafficSelector>* TsIL() const;
        
        /**
         *  Copies the responder side traffic selectors.
         *  Ownership is transferrer
         */
        CArrayFix<TIkeV2TrafficSelector>* TsRL() const;       
private:
        CIpsecSARekeyData(const TInt aReplayWindow, 
                          const TIpsecSALifetime* aHard, 
                          const TIpsecSALifetime* aSoft);
        
        void ConstructL(const CArrayFix<TIkeV2TrafficSelector>& aTS_i, 
                        const CArrayFix<TIkeV2TrafficSelector>& aTS_r, 
                        const TDesC8& aLocalId, const TDesC8& aRemoteId);
            
        
        
        CArrayFix<TIkeV2TrafficSelector>* CopyTsL(const CArrayFix<TIkeV2TrafficSelector>& aTS) const;
        
        //
        // Saved from Acquire object for possible IPSec SA rekeying
        //
        TInt iReplayWindow;
        TIpsecSALifetime iHard;
        TIpsecSALifetime iSoft;
        
        
        CArrayFix<TIkeV2TrafficSelector>*   iTS_i;
        CArrayFix<TIkeV2TrafficSelector>*   iTS_r; 
        HBufC8*   iLocalId; 
        HBufC8*   iRemoteId;
    };


#endif /* IKEV2IPSECSAREKEYDATA_H_ */
