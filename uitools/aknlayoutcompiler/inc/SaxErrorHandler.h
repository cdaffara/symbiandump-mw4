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

#ifndef __SAXERRORHANDLER_H_
#define __SAXERRORHANDLER_H_

#include <DOM/SAX2DOM/SAX2DOM.hpp>
#include <SAX/ErrorHandler.hpp>

class SAXErrorHandler : public Arabica::SAX::ErrorHandler<std::string>
	{
public:
  SAXErrorHandler() { }
  virtual ~SAXErrorHandler() { }
public: // from SAX::ErrorHandler
  virtual void warning(const Arabica::SAX::SAXParseException<std::string>&);
  virtual void error(const Arabica::SAX::SAXParseException<std::string>&);
  virtual void fatalError(const Arabica::SAX::SAXParseException<std::string>&);
	};

#endif // __SAXERRORHANDLER_H_
