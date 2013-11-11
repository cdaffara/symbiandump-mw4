/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Pritam Roy Biswas, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Description:
 * This source file gives different data classes to be used by Smf 
 * Credential Manager Client and Server for data transfer.	
 *
 */

#include <f32file.h>

#include "smfcredmgrclientdatastruct.h"
#include "smfutils.h"

/**
 * Method to externalize the member variables
 * @param aStream The Write Stream to be filled.
 */
void TSmfAuthToken::ExternalizeL(RWriteStream& aStream)
	{

	TPtr firstArgPtr(iKey->Des());
	SmfUtils::ExternalizeDesL(firstArgPtr, aStream);

	TPtr secondArgPtr(iSecret->Des());
	SmfUtils::ExternalizeDesL(secondArgPtr, aStream);
	}

/**
 * Method to internalize the member variables
 * @param aStream The source stream to read from
 */
void TSmfAuthToken::InternalizeL(RReadStream& aStream)
	{
	// Delete the current values
	delete iKey;
	iKey = SmfUtils::InternalizeDesL(aStream);

	delete iSecret;
	iSecret = SmfUtils::InternalizeDesL(aStream);
	}

/**
 * Method to externalize the member variables
 * @param aStream The Write Stream to be filled.
 */
void CSmfFetchAuthTokenSet::ExternalizeL(RWriteStream& aStream)
	{
	aStream.WriteUint32L(iValidity);

	TPtr tokenPtr(iRegistrationToken->Des());
	SmfUtils::ExternalizeDesL(tokenPtr, aStream);

	TInt32 countTokenSet = iAuthTokenArray.Count();
	aStream.WriteInt32L(countTokenSet);

	for (int i = 0; i < countTokenSet; i++)
		{
		iAuthTokenArray[i].ExternalizeL(aStream);
		}
	}

/**
 * Method to internalize the member variables
 * @param aSource The Source Stream to read from
 */
void CSmfFetchAuthTokenSet::InternalizeL(const RBuf8& aSource)
	{
	// Create stream to read from the given buffer
	RDesReadStream stream(aSource);
	CleanupClosePushL(stream);

	iValidity = stream.ReadUint32L();
	
	// Delete the current values
	delete iRegistrationToken;
	iRegistrationToken = SmfUtils::InternalizeDesL(stream);

	TInt32 countTokenSet = stream.ReadInt32L();
	iAuthTokenArray.Reset();
	for (int i = 0; i < countTokenSet; i++)
		{
		TSmfAuthToken set;
		set.InternalizeL(stream);
		iAuthTokenArray.Insert(set, i);
		}
	CleanupStack::PopAndDestroy(&stream);
	}

/**
 * Method to externalize the member variables
 * @param aStream The Write Stream to be filled.
 */
void CSmfURLListParams::ExternalizeL(RWriteStream& aStream)
	{
	TInt32 countPlugin = iURLList.Count();
	aStream.WriteInt32L(countPlugin);
	for (int i = 0; i < countPlugin; i++)
		{
		TPtr pluginBufPtr(iURLList[i]->Des());
		SmfUtils::ExternalizeDesL(pluginBufPtr, aStream);
		}

	TPtr tokenPtr(iPluginID->Des());
	SmfUtils::ExternalizeDesL(tokenPtr, aStream);
	}

/**
 * Method to internalize the member variables
 * @param aSource The source stream to read from
 */
void CSmfURLListParams::InternalizeL(const TDesC8& aSource)
	{
	// Create stream to read from the given buffer
	RDesReadStream stream(aSource);
	CleanupClosePushL(stream);

	//internalize plugin ids
	TInt32 countPlugin = stream.ReadInt32L();
	iURLList.Reset();
	for (int i = 0; i < countPlugin; i++)
		{
		HBufC* bufPlugin(SmfUtils::InternalizeDesL(stream));
		iURLList.Insert(bufPlugin, i);
		}
	iPluginID = SmfUtils::InternalizeDesL(stream);

	CleanupStack::PopAndDestroy(&stream);
	}

/**
 * Method to externalize the member variables
 * @param aStream The Write Stream to be filled.
 */
void CSmfPluginIDListParams::ExternalizeL(RWriteStream& aStream)
	{
	TPtr tokenPtr(iRegistrationToken->Des());
	SmfUtils::ExternalizeDesL(tokenPtr, aStream);

	TInt32 countPlugin = iPluginList.Count();
	aStream.WriteInt32L(countPlugin);
	for (int i = 0; i < countPlugin; i++)
		{
		TPtr pluginBufPtr(iPluginList[i]->Des());
		SmfUtils::ExternalizeDesL(pluginBufPtr, aStream);
		}
	}

/**
 * Method to internalize the member variables
 * @param aSource The source stream to read from
 */
void CSmfPluginIDListParams::InternalizeL(const RBuf8& aSource)
	{
	// Create stream to use given buffer.
	RDesReadStream stream(aSource);
	CleanupClosePushL(stream);

	// Delete the current values
	delete iRegistrationToken;
	iRegistrationToken = SmfUtils::InternalizeDesL(stream);

	//internalize plugin ids
	TInt32 countPlugin = stream.ReadInt32L();
	iPluginList.Reset();
	for (int i = 0; i < countPlugin; i++)
		{
		HBufC* bufPlugin(SmfUtils::InternalizeDesL(stream));
		iPluginList.Insert(bufPlugin, i);
		}
	CleanupStack::PopAndDestroy(&stream);
	}

/**
 * Method to externalize the member variables
 * @param aStream The Write Stream to be filled.
 */
void CSmfStoreAuthParams::ExternalizeL(RWriteStream& aStream)
	{
	aStream.WriteInt32L(pluginIDEnabled);

	aStream.WriteUint32L(iValidity);

	TPtr tokenPtr(iRegistrationToken->Des());
	SmfUtils::ExternalizeDesL(tokenPtr, aStream);

	TPtr AuthAppIdPtr(iAuthAppID->Des());
	SmfUtils::ExternalizeDesL(AuthAppIdPtr, aStream);

	TInt32 countTokenSet = iAuthTokenArray.Count();
	aStream.WriteInt32L(countTokenSet);

	for (int i = 0; i < countTokenSet; i++)
		{
		iAuthTokenArray[i].ExternalizeL(aStream);
		}

	TInt32 countPlugin = iPluginIDList.Count();
	aStream.WriteInt32L(countPlugin);

	for (int i = 0; i < countPlugin; i++)
		{
		TPtr pluginBufPtr(iPluginIDList[i]->Des());
		SmfUtils::ExternalizeDesL(pluginBufPtr, aStream);
		}

	TInt32 countUrl = iURLList.Count();
	aStream.WriteInt32L(countUrl);

	for (int i = 0; i < countUrl; i++)
		{
		TPtr urlBufPtr(iURLList[i]->Des());
		SmfUtils::ExternalizeDesL(urlBufPtr, aStream);
		}
	}

/**
 * Method to internalize the member variables
 * @param aSource The source stream to read from
 */
void CSmfStoreAuthParams::InternalizeL(const RBuf8& aSource)
	{
	// Create stream to read from the given buffer
	RDesReadStream stream(aSource);
	CleanupClosePushL(stream);

	pluginIDEnabled = stream.ReadInt32L();

	iValidity = stream.ReadUint32L();

	// Delete the current values
	delete iRegistrationToken;
	iRegistrationToken = SmfUtils::InternalizeDesL(stream);

	delete iAuthAppID;
	iAuthAppID = SmfUtils::InternalizeDesL(stream);

	TInt32 countTokenSet = stream.ReadInt32L();
	iAuthTokenArray.Reset();
	for (int i = 0; i < countTokenSet; i++)
		{
		TSmfAuthToken set;
		set.InternalizeL(stream);
		iAuthTokenArray.Insert(set, i);
		}

	//internalize plugin ids
	TInt32 countPlugin = stream.ReadInt32L();
	iPluginIDList.Reset();
	for (int i = 0; i < countPlugin; i++)
		{
		HBufC* bufPlugin = SmfUtils::InternalizeDesL(stream);
		iPluginIDList.Insert(bufPlugin, i);
		}

	//internalize URLs
	TInt32 countURL = stream.ReadInt32L();
	iURLList.Reset();
	for (int i = 0; i < countURL; i++)
		{
		HBufC* bufURL = SmfUtils::InternalizeDesL(stream);
		iURLList.Insert(bufURL, i);
		}
	CleanupStack::PopAndDestroy(&stream);
	}

/**
 * Method to externalize the member variables
 * @param aStream The Write Stream to be filled.
 */
void CSmfPluginIDUpdate::ExternalizeL(RWriteStream& aStream)
	{
	aStream.WriteInt32L(pluginIDEnabled);

	TPtr newPtr(iNewPluginID->Des());
	SmfUtils::ExternalizeDesL(newPtr, aStream);

	TPtr oldPtr(iOldPluginID->Des());
	SmfUtils::ExternalizeDesL(oldPtr, aStream);
	}

/**
 * Method to internalize the member variables
 * @param aSource The source stream to read from
 */
void CSmfPluginIDUpdate::InternalizeL(const RBuf8& aSource)
	{
	// Create stream to read from the given buffer
	RDesReadStream stream(aSource);
	CleanupClosePushL(stream);

	pluginIDEnabled = stream.ReadInt32L();

	// Delete the current values
	delete iNewPluginID;
	iNewPluginID = SmfUtils::InternalizeDesL(stream);

	delete iOldPluginID;
	iOldPluginID = SmfUtils::InternalizeDesL(stream);

	CleanupStack::PopAndDestroy(&stream);
	}


/**
 * NewL method
 * @param aMessage The message to be signed
 * @param aKey The key
 * @return The constructed CSmfSignParameters instance
 */
CSmfSignParameters* CSmfSignParameters::NewL(
	const TDesC8& aMessage, const TDesC8& aKey ) 
	{
	CSmfSignParameters* self = new( ELeave ) CSmfSignParameters();
	CleanupStack::PushL( self );
	self->ConstructL( aMessage, aKey );
	CleanupStack::Pop( self );
	return self;
	}

/**
 * Overloaded NewL method
 * @param aMessage The message to be signed
 * @return The constructed CSmfSignParameters instance
 */
CSmfSignParameters* CSmfSignParameters::NewL( const TDesC8& aData ) 
	{
	CSmfSignParameters* self = new( ELeave ) CSmfSignParameters();
	CleanupStack::PushL( self );
	self->ConstructL( aData );
	CleanupStack::Pop( self );
	return self;
	}

/**
 * Destructor 
 */
CSmfSignParameters::~CSmfSignParameters() 
	{
	iMessage.Close();
	iKey.Close();
	}

/**
 * Method to externalize the member variables
 * @param aStream The Write Stream to be filled.
 */
void CSmfSignParameters::ExternalizeL( RWriteStream& aStream ) const
	{
	SmfUtils::ExternalizeDesL( iMessage, aStream );
	SmfUtils::ExternalizeDesL( iKey, aStream );
	}

/**
 * Method to get the Key for the signature
 * @return The Key for the signature
 */
const TDesC8& CSmfSignParameters::Key() const 
	{
	return iKey;
	}

/**
 * Method to get the message
 * @return The message 
 */
const TDesC8& CSmfSignParameters::Message() const 
	{
	return iMessage;
	}

/**
 * Constructor 
 */
CSmfSignParameters::CSmfSignParameters() 
	{
	}

/**
 * Two Phase constructor
 * @param aKey The Key for the signature
 * @return The constructed CSmfSignParameters instance
 */
void CSmfSignParameters::ConstructL( const TDesC8& aMessage, const TDesC8& aKey )
	{
	iMessage.CreateL( aMessage );
	iKey.CreateL( aKey );
	}

/**
 * Two Phase constructor
 * @return The constructed CSmfSignParameters instance
 */
void CSmfSignParameters::ConstructL( const TDesC8& aData ) 
	{
	RDesReadStream stream( aData );
	CleanupClosePushL( stream );
	SmfUtils::InternalizeDesL( iMessage, stream );
	SmfUtils::InternalizeDesL( iKey, stream );
	CleanupStack::PopAndDestroy( &stream );
	}


/**
 * NewL method
 * @param aKeyName
 * @param startDate
 * @param endDate
 * @param aKeydata
 * @return The constructed CSmfRsaKeyParameters instance
 */
CSmfRsaKeyParameters* CSmfRsaKeyParameters::NewL(
		const TDesC& aKeyName, 
		const TTime& startDate,
		const TTime& endDate, 
		const TDesC8& aKeyData )
	{
	CSmfRsaKeyParameters* self = new( ELeave ) CSmfRsaKeyParameters;
	CleanupStack::PushL( self );
	self->ConstructL( aKeyName, startDate, endDate, aKeyData );
	CleanupStack::Pop( self );
	return self;
	}

/**
 * Overloaded NewL method
 * @param aData
 * @return The constructed CSmfRsaKeyParameters instance
 */
CSmfRsaKeyParameters* CSmfRsaKeyParameters::NewL( const TDesC8& aData )
	{
	CSmfRsaKeyParameters* self = new( ELeave ) CSmfRsaKeyParameters;
	CleanupStack::PushL( self );
	self->ConstructL( aData );
	CleanupStack::Pop( self );
	return self;
	}

/**
 * Destructor 
 */
CSmfRsaKeyParameters::~CSmfRsaKeyParameters() 
	{
	iKeyName.Close();
	iKeyData.Close();
	}

/**
 * Method to externalize the member variables
 * @param aStream The Write Stream to be filled.
 */
void CSmfRsaKeyParameters::ExternalizeL( RWriteStream& aStream ) const
	{
	SmfUtils::ExternalizeDesL( iKeyName, aStream );
	SmfUtils::ExternalizeDesL( iKeyData, aStream );
	SmfUtils::ExternalizeInt64L( iStartDate.Int64(), aStream );
	SmfUtils::ExternalizeInt64L( iEndDate.Int64(), aStream );		
	}

/**
 * Method to get the key name
 * @return The key name 
 */
const TDesC& CSmfRsaKeyParameters::KeyName() const
	{
	return iKeyName;
	}

/**
 * Method to get the key data
 * @return The key data
 */
const TDesC8& CSmfRsaKeyParameters::KeyData() const
	{
	return iKeyData;
	}

/**
 * Method to get the start date
 * @return The start date
 */
const TTime& CSmfRsaKeyParameters::StartDate() const
	{
	return iStartDate;
	}

/**
 * Method to get the end date
 * @return The end date
 */
const TTime& CSmfRsaKeyParameters::EndDate() const
	{
	return iEndDate;
	}

/**
 * Two phase constructor
 * @param aKeyName
 * @param startDate
 * @param endDate
 * @param aKeydata
 */
void CSmfRsaKeyParameters::ConstructL( const TDesC& aKeyName, 
		const TTime& startDate,
		const TTime& endDate, 
		const TDesC8& aKeyData )
	{
	iKeyName.CreateL( aKeyName );
	iKeyData.CreateL( aKeyData );
	iStartDate = startDate;
	iEndDate = endDate;
	}

/**
 * Two phase constructor
 * @param aData
 */
void CSmfRsaKeyParameters::ConstructL( const TDesC8& aData )
	{
	RDesReadStream stream( aData );
	CleanupClosePushL( stream );
	SmfUtils::InternalizeDesL( iKeyName, stream );
	SmfUtils::InternalizeDesL( iKeyData, stream );
	
	TInt64 startDate;
	TInt64 endDate;
	SmfUtils::InternalizeInt64L( startDate, stream );
	SmfUtils::InternalizeInt64L( endDate, stream );
	
	iStartDate = TTime( startDate );
	iEndDate = TTime( endDate );
	CleanupStack::PopAndDestroy( &stream );	
	}
