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
*       Defines the entry point for the console application
*
*/


//
// class SAXErrorHandler
//

#include "SaxErrorHandler.h"

void SAXErrorHandler::warning(const Arabica::SAX::SAXParseException<std::string>& exception)
{
  std::cerr << "WARNING: " << exception.what() << std::endl;
} // warning

void SAXErrorHandler::error(const Arabica::SAX::SAXParseException<std::string>& exception)
{
  std::cerr << "ERROR  : " << exception.what() << std::endl;
} // error

void SAXErrorHandler::fatalError(const Arabica::SAX::SAXParseException<std::string>& exception)
{
  std::cerr << "FATAL  : " << exception.what() << std::endl;
} // fatalError
