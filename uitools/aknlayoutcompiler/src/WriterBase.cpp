/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include "WriterBase.h"

// class TWriterBase

TWriterBase::TWriterBase(const std::string& aName)
: iName(aName)
	{
	}

TWriterBase::~TWriterBase()
	{
	}


// class TLayWriterBase

TLayWriterBase::TLayWriterBase(TLayout& aLayout, const std::string& aName)
: TWriterBase(aName), iLayout(aLayout)
	{
	}

TLayWriterBase::~TLayWriterBase()
	{
	}


// End of File
