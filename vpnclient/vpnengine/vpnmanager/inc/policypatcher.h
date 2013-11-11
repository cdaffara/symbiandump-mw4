/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Policy importer
*
*/



#ifndef __POLICYPATCHER_H__
#define __POLICYPATCHER_H__

#include <e32base.h>

_LIT8(KDataField, "  DATA:"); // Two preceding spaces just for indentation
_LIT8(KSpace, " ");
_LIT8(KNewLine, "\n");
_LIT8(KName, "NAME");
_LIT8(KKeyLenField, "PRIVATE_KEY_LENGTH: ");
_LIT8(KDNField, "  SUBJECT_DN_SUFFIX: ");


const TChar KBinChar = 'B';
const TChar KFormatChar = 'F';

const TInt KUndefinedOffset = -1;

class TPatchKeyArrayPtr : public TKeyArrayFix
{
public:
    inline TPatchKeyArrayPtr(TInt aOffset, TKeyCmpText aType)
    :TKeyArrayFix(aOffset, aType) {}
    inline TPatchKeyArrayPtr(TInt aOffset, TKeyCmpText aType, TInt aLength)
    :TKeyArrayFix(aOffset, aType, aLength) {}
    inline TPatchKeyArrayPtr(TInt aOffset, TKeyCmpNumeric aType)
    :TKeyArrayFix(aOffset, aType) {}
    virtual TAny* At(TInt aIndex) const;
};


class CPolicyPatchInfo : public CBase
    {
public:
    CPolicyPatchInfo();
    ~CPolicyPatchInfo();

    const TDesC8& CertSubjectName() const;
    void SetCertSubjectNameL(const TDesC8& aCertSubjectName);
    
    /**
     * Sets key length (in bits) of user cert.
     */
    void SetUserCertKeyLen( TInt aKeyLen );
    
    /**
     * Gets key length (in bits) of user cert. 
     */
    TInt UserCertKeyLen() const;
    
    /**
     * Verifies, that subjectname does not include not closed commas inside attributes
     */
    HBufC8* CheckSubjectNameSyntaxL(const TDesC8& aSubj);

public:
    TBuf8<KMaxFileName> iCertFileName;
    TInt iPatchOffset;
    TInt iReminderOffset;

private:
    HBufC8* iCertSubjectName;
    
    /**
     * Key length (in bits) of user cert. 
     */
    TInt    iUserCertKeyLen;
    };

class CPolicyPatchInfoList : public CArrayPtrFlat<CPolicyPatchInfo>
    {
public:
    CPolicyPatchInfoList(TInt aGranularity);
    ~CPolicyPatchInfoList();
    };

class CPatchData : public CBase
    {
public:
    ~CPatchData();

public:
    TInt iPatchOffset;
    TInt iReminderOffset;
    HBufC8* iData;
    };

class CPatchDataList : public CArrayPtrFlat<CPatchData>
    {
public:
    CPatchDataList(TInt aGranularity);
    ~CPatchDataList();
    };

class CPolicyPatcher : public CBase
    {
public:
    static CPolicyPatcher* NewL();

    HBufC8* PatchPolicyL(const TDesC8& aPolicyData,
                         CPolicyPatchInfoList* aPatchInfoList);

private:
    CPolicyPatcher();
    void ConstructL();
    ~CPolicyPatcher();

    void FindOffsetsL();
    HBufC8* CreateOutputL();
    HBufC8* CreatePatchDataL(const CPolicyPatchInfo* aPatchInfo);

private:
    TPtrC8 iInput;
    CPolicyPatchInfoList* iPatchInfoList;
    TBool iUserCertPatched;
    };

#endif // __POLICYPATCHER_H__
