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
* Description:  Header file for caiwprintingif
*
*/



#ifndef __CAIWIMAGEPRINTIF_H__
#define __CAIWIMAGEPRINTIF_H__

#include <AiwServiceIfMenu.h>
#include <badesca.h>
#include <eikenv.h>

_LIT( KResourceFile, "resource\\imageprintuiprovider.rsc" );

_LIT( KParamFile, "system\\data\\T_AIW_PRINT.DAT");
_LIT( KUnSuppFile, "system\\data\\T_AIW_UNSUPP.DAT");
_LIT(KImagePrintUiSearchPatternBySID, "*1020e470*");
const TInt KImagePrintUID = 0x1020E470;
const TInt KRetryInterval = 5000000; // 500 ms
const TInt KResource = 40;
const TInt KDriver = 3;

class DRMCommon;
class CNoteTimer;


class CAiwImagePrintIf : public CAiwServiceIfMenu
{
    public:

        ~CAiwImagePrintIf();
        
    public:
    
    	TInt   iNumberOfUnSuppFiles;
    	TBool  iUnsupportedFiles;   
    	CNoteTimer* iNoteTimer;
    	HBufC* iPrintFileName;
        HBufC* iUnsuppFileName;


    protected:

        CAiwImagePrintIf();

    protected: // From CAiwServiceIfMenu

        TBool IsPrintingSupported( const CAiwGenericParamList& aInParamList );
        
        TBool IsPrintingSupportedL( const TDesC& aFileName );
        
        TBool IsProtected( const TDesC& aFileName );

        TBool CheckMIMETypeL( const TDesC8& aMimeType, const TDesC& aFileName );
        
    protected: // Implementation
        void ConstructL();

    protected: // data
    
        CEikonEnv& iEikEnv; //Allow CAiwPrintingProvider and CAiwPreviewProvider
                            //to use Eikon Environment without CEikonEnv::Static()

    private: //data
    
        TInt iResourceOffset;

        // Provides common DRM functions
        DRMCommon* iDRM;
        
        
};

#endif