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
* Description:   Abstract base class for renderer plugins
*
*/



// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline CHuiRenderPluginBase::CHuiRenderPluginBase()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline CHuiRenderPluginBase::~CHuiRenderPluginBase()
    {
	REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline CHuiRenderPluginBase* CHuiRenderPluginBase::NewL(const TDesC& aMatchData)
	{
	// Convert match data to 8-bits
	HBufC8* buf = HBufC8::NewLC(aMatchData.Length());
	TPtr8 data = buf->Des();
	data.Copy(aMatchData);
	data.LowerCase();	// change to lower case

	// Set resolving parameters
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(data);

	// Get the instantiation
	CHuiRenderPluginBase* renderPlugin = REINTERPRET_CAST(CHuiRenderPluginBase*,
									REComSession::CreateImplementationL(KUidHuiRenderPluginBase,
																		_FOFF(CHuiRenderPluginBase, iDtor_ID_Key),
																		resolverParams));
	CleanupStack::PopAndDestroy(buf);
	return renderPlugin;
	}
