/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header declaration
*
*/




#include "senmtomutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Serializes SoapEnvelope message in XOP message
// ---------------------------------------------------------------------------
//
EXPORT_C void SenMtomUtils::SerializeEnvelope2L(CSenSoapEnvelope2& aSoapEnvelope2, 
                                                RBuf8& aXopEnvelope)
    {
	__ASSERT_ALWAYS(&aSoapEnvelope2, 
	                User::Panic(KMtomNoRequestSoapEnvelopePanicText, EMtomNoRequestSoapEnvelope));
	RSenDocument doc = aSoapEnvelope2.AsDocumentL();
    CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(ESerializerXOPInfoset);
	__ASSERT_ALWAYS(serializer, User::Panic(KMtomPanicText, KErrNotFound));
    CleanupStack::PushL(serializer);
    TXmlEngSerializationOptions options(TXmlEngSerializationOptions::KOptionIndent & TXmlEngSerializationOptions::KOptionOmitXMLDeclaration);
    serializer->SetOutput(aXopEnvelope);	
    serializer->SetSerializationOptions(options);
    serializer->SerializeL(doc);
    CleanupStack::PopAndDestroy(serializer);
    }

// ---------------------------------------------------------------------------
// Generates random content-ID for header of XOP message and for headers of BLOBs 
// as randomNumber@homeTime
// @param aRootCid is ReAlloc'd, which means that any existing data is freed
// prior the generated root cid is assinged to that buffer.
// ---------------------------------------------------------------------------
//
EXPORT_C void SenMtomUtils::GenerateRandomRootCidL(RBuf8& aRootCid)
    {
    _LIT8(KAt, "@");
	TTime now;
	now.HomeTime();
	TInt64 homeTime = now.Int64();
	TInt64 randomNumber = Math::Random();
	aRootCid.ReAllocL(32+KAt().Length());
	aRootCid.AppendNum(randomNumber);
	aRootCid.Append(KAt);
	aRootCid.AppendNum(homeTime);
    }

// ---------------------------------------------------------------------------
// Generates random boundary for mime header as randomNumber
// @param aBoundary is ReAlloc'd, which means that any existing data is freed
// prior the generated root cid is assinged to that buffer.
// ---------------------------------------------------------------------------
//
EXPORT_C void SenMtomUtils::GenerateRandomBoundaryL(RBuf8& aBoundary)
    {
  	TInt64 randomNumber = Math::Random();
  	aBoundary.ReAllocL(16);
	aBoundary.AppendNum(randomNumber);
    }

// ---------------------------------------------------------------------------
// Generates MimeBoundary to separate mime parts of the message
// as --randomNumber
// ---------------------------------------------------------------------------
//
EXPORT_C void SenMtomUtils::BoundaryLineStartL(const RBuf8& aBoundary, RBuf8& aBoundaryStart)
	{
	aBoundaryStart.ReAllocL(KNewLine().Length()+KDush().Length()+aBoundary.Length());
	aBoundaryStart.Append(KDush);
	aBoundaryStart.Append(aBoundary);
	aBoundaryStart.Append(KNewLine);
	}

// ---------------------------------------------------------------------------
// Generates MimeBoundaryEnd to end the multipart message (MTOM message)
// as --randomNumber--
// ---------------------------------------------------------------------------
//
EXPORT_C void SenMtomUtils::BoundaryLineEndL(const RBuf8& aBoundary, RBuf8& aBoundaryEnd)
	{
	aBoundaryEnd.ReAllocL(3*KNewLine().Length()+2*KDush().Length()+aBoundary.Length());
	aBoundaryEnd.Append(KNewLine);
	aBoundaryEnd.Append(KNewLine);
	aBoundaryEnd.Append(KDush);
	aBoundaryEnd.Append(aBoundary);
	aBoundaryEnd.Append(KDush);
	aBoundaryEnd.Append(KNewLine);
	}
         
// ---------------------------------------------------------------------------
// Creates header for root of XOP message
// The header sould be the next:
//
// Content-Type: application/xop+xml;
// charset=UTF-8; 
// type="text/xml" (for SOAP 1.1) or "application/soap+xml" (for SOAP 1.2)
// Content-Transfer-Encoding: 8bit
// Content-ID: <randomNumber@homeTime>
//
// where "Content-ID" is the same as "start" in Header of Outer Package
// ---------------------------------------------------------------------------
//
EXPORT_C void SenMtomUtils::HeaderRootL(CSenSoapEnvelope2& aSoapEnvelope2,
										const RBuf8& aRootCid, RBuf8& aHeaderRoot)
	{
	switch (aSoapEnvelope2.SoapVersion())
		{
 		case ESOAP11:
			{
			aHeaderRoot.ReAllocL(KHeaderRootStart11().Length()+KHeaderRootEnd().Length()+aRootCid.Length());
			aHeaderRoot.Append(KHeaderRootStart11);
			break;
			}
		case ESOAP12:
			{
			aHeaderRoot.ReAllocL(KHeaderRootStart12().Length()+KHeaderRootEnd().Length()+aRootCid.Length());
			aHeaderRoot.Append(KHeaderRootStart12);		
			break;
			}
		default:
			{
			User::Panic(KMtomSoapVersionInvalidPanicText, EMtomSoapVersionInvalid);
			}			
		}
	aHeaderRoot.Append(aRootCid);
	aHeaderRoot.Append(KHeaderRootEnd);
	}
	
// ---------------------------------------------------------------------------
// Creates  the header for binary data[aIndex] of XOP message
// The header sould be the next:
//
// --MIME_boundary
// Content-Type: image/png (for images) or some other type for other BLOBs
// Content-Transfer-Encoding: binary
// Content-ID: <randomNumber@homeTime>
//
// ---------------------------------------------------------------------------
//
EXPORT_C void SenMtomUtils::HeaderBinaryDataL(TUint aIndex, 
										      RArray<TXmlEngDataContainer>& aBinaryDataList,
			 					     	      RBuf8& aHeaderBinaryData)
	{
	__ASSERT_ALWAYS(aBinaryDataList.Count(), User::Panic(KMtomNoBlobsPanicText, EMtomNoBlobs)); 
	__ASSERT_ALWAYS((aIndex < aBinaryDataList.Count()), User::Panic(KMtomPanicText, KErrArgument));	    
	
	TPtrC8 cid = aBinaryDataList[aIndex].Cid();
	
	RXmlEngNodeList<TXmlEngAttr> attributes; // to take value of "ContentType" attribute
	aBinaryDataList[aIndex].ParentNode().AsElement().GetAttributes(attributes);
	CleanupClosePushL(attributes);

	RBuf8 contentType;
	CleanupClosePushL(contentType);
	while (attributes.HasNext())
		{
		TXmlEngAttr attr = attributes.Next();
		if (attr.Name() == KContentTypeName)
			{
			contentType.ReAllocL(attr.Value().Length());
			contentType.Copy(attr.Value()); 
			break;
			}
		}

	__ASSERT_ALWAYS((contentType != _L8("")), 
	                User::Panic(KMtomNoContentTypeInBlobHeaderPanicText, EMtomNoContentTypeInBlobHeader));

	aHeaderBinaryData.ReAllocL(KHeaderBinaryDataContentType().Length()+
							   KHeaderBinaryDataContentID().Length()+
							   KHeaderBinaryDataEnd().Length()+
							   contentType.Length()+
							   cid.Length());
	aHeaderBinaryData.Append(KHeaderBinaryDataContentType);
	aHeaderBinaryData.Append(contentType);
	aHeaderBinaryData.Append(KHeaderBinaryDataContentID);
	aHeaderBinaryData.Append(cid);
	aHeaderBinaryData.Append(KHeaderBinaryDataEnd);

	CleanupStack::PopAndDestroy(&contentType);
	CleanupStack::PopAndDestroy(&attributes);
	}

// ---------------------------------------------------------------------------
// Extracts a part of BLOB (10KB) from a file
// ---------------------------------------------------------------------------
//
EXPORT_C void SenMtomUtils::FileDataPartL(TUint aPart, TUint aIndex, 
									      RArray<TXmlEngDataContainer>& aBinaryDataList,
						       		      RBuf8& aBinaryData)
	{
	__ASSERT_ALWAYS(aBinaryDataList.Count(), User::Panic(KMtomNoBlobsPanicText, EMtomNoBlobs)); 
	__ASSERT_ALWAYS((aIndex < aBinaryDataList.Count()), User::Panic(KMtomPanicText, KErrArgument));	    
	
   	aBinaryData.ReAllocL(KTenKBytes);

	RFile& file = aBinaryDataList[aIndex].AsFileContainer().File();
	file.Flush();
	file.Read(aPart*KTenKBytes, aBinaryData, KTenKBytes);
	}

// ---------------------------------------------------------------------------
// Extracts the rest of BLOB (less than 10KB) from a file
// ---------------------------------------------------------------------------
//	
EXPORT_C void SenMtomUtils::FileDataRestL(TUint aParts, TUint aRest, TUint aIndex, 
									      RArray<TXmlEngDataContainer>& aBinaryDataList,
						       		      RBuf8& aBinaryData)
	{
	__ASSERT_ALWAYS(aBinaryDataList.Count(), User::Panic(KMtomNoBlobsPanicText, EMtomNoBlobs)); 
	__ASSERT_ALWAYS((aIndex < aBinaryDataList.Count()), User::Panic(KMtomPanicText, KErrArgument));	    

   	aBinaryData.ReAllocL(aRest);
	RFile& file = aBinaryDataList[aIndex].AsFileContainer().File();
	file.Flush();
	file.Read(aParts*KTenKBytes, aBinaryData, aRest);
	}

// ---------------------------------------------------------------------------
// Extracts CIDs of BLOBs from response MTOM message
// ---------------------------------------------------------------------------
//	
EXPORT_C void SenMtomUtils::CidL(const RBuf8& aBlobHeader, RArray<RBuf8>& aCids)
	{
    _LIT8(KContentIDName, "Content-ID: <");
	TInt offset = aBlobHeader.Find(KContentIDName);
	if (offset == KErrNotFound)
		{
		User::Panic(KMtomNoCidPanicText, EMtomNoCid);
		}
	TPtrC8 blobHeaderPtr; // to extract BLOB header from Response message
	blobHeaderPtr.Set(aBlobHeader.Right(aBlobHeader.Length()-offset-KContentIDName().Length()));
		
	offset = blobHeaderPtr.Find(_L8(">"));
	if (offset == KErrNotFound)
		{
		User::Panic(KMtomCidInvalidPanicText, EMtomCidInvalid);
		}
	RBuf8 cid; // CID of the BLOB
	cid.ReAllocL(offset);		
	cid.Copy(blobHeaderPtr.Left(offset));
	aCids.AppendL(cid); // collects CIDs of all BLOBs
	}	

// ---------------------------------------------------------------------------
// Generates the file name for BLOB of response MTOM message 
// and collects it in array of file names for the all BLOBs
// ---------------------------------------------------------------------------
//	
void SenMtomUtils::FileNameL(TUint aIndex, RArray<RBuf8>& aFileNames)
	{
	RBuf8 fileName;
	fileName.ReAllocL(KFileName().Length()+7);
	fileName.Append(KFileName);
	fileName.AppendNum(aIndex);
	aFileNames.Append(fileName);
	}

// ---------------------------------------------------------------------------
// Extracts BLOB from response MTOM message and writes it in a file
// ---------------------------------------------------------------------------
//	
EXPORT_C TInt SenMtomUtils::SetFileL(TUint aIndex, TBool aFirst, TDesC8& aBlob, 
						             RArray<RBuf8>& aFileNames)
	{
	RFs rfs;
	RFile file;
	User::LeaveIfError(rfs.Connect());
	CleanupClosePushL(rfs);	
	TInt retVal = rfs.ShareProtected();
    CleanupClosePushL(file);
    
    TInt sizeFile = 0;
    RBuf fileName; //fileName is necessary to convert from 8bites(aFileNames[aIndex]) to 16
    
    if (aFirst) // check if is it the first part of a BLOB to write in a file
        {
        FileNameL(aIndex, aFileNames); // generates the new file
        fileName.ReAllocL(aFileNames[aIndex].Length());
        fileName.Copy(aFileNames[aIndex]);
        User::LeaveIfError(file.Replace(rfs, fileName, EFileWrite)); // replases the old one to the new one
        }
    else // if it's not the first part of BLOB then only open the existing file and write a BLOB part in it 
        {
        fileName.ReAllocL(aFileNames[aIndex].Length());
        fileName.Copy(aFileNames[aIndex]);
        User::LeaveIfError(file.Open(rfs, fileName, EFileWrite));
        }
  
    file.Size(sizeFile);
	User::LeaveIfError(file.Write(sizeFile, aBlob, aBlob.Length()));
    
    file.Flush();
    file.Close();

    User::LeaveIfError(file.Open(rfs, fileName, EFileRead));
    fileName.Close();
    CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&rfs);
	return (sizeFile+aBlob.Length());
	}

// End of file
	
