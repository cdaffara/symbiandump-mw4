/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MLATTRIBUTESPARSE_H
#define MLATTRIBUTESPARSE_H

// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

#include "LayoutCompilerErr.h"
#include "MLAttributes.h"

#include <avkon.hrh> // for logical font ids

#include "SaxErrorHandler.h"

#include <vector>
#include <string>
#include <iosfwd>
#include <stack>

using namespace std;


//
// defines
//

/**
 * this is the interface to the SAX handler
 */
class MSaxLayoutAttributesHandler
	{
public:
	typedef Arabica::SAX::Attributes<std::string> TAttribs; // this is XML Attribs, not to be confused with layout attibutes!
	virtual MSaxLayoutAttributesHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs) { return this; };
	virtual void HandleSaxEnd(const std::string& aElement) { };
	};


/**
 * this corresponds to an attribute set in the xml
 */
class TMLAttributeSetParse : public TMLAttributeSet, public MSaxLayoutAttributesHandler
	{
public:
	TMLAttributeSetParse(TMLAttributes* aAttributes);
public: // from MSaxLayoutHandler
	MSaxLayoutAttributesHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
	void HandleSaxEnd(const std::string& aElement);
public: // new methods
	int CompId() const;
	string Name() const;
private:
	void HandleSaxAttribute(const TAttribs& aAttribs);
	void HandleSaxCalc(const TAttribs& aAttribs);
private:
	int iId;
	int iCompId;
	string iName;
	TMLAttributeZoomLevels* iSaxZoomLevels;
	};

/**
 *  this corresponds to attributes in the xml
 */
class TMLAttributesParse : public TMLAttributes, public MSaxLayoutAttributesHandler
	{
public:
	static auto_ptr<TMLAttributesParse> Parse(const string& aLayName);
	TMLAttributesParse();
public: // from MSaxLayoutHandler
	MSaxLayoutAttributesHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
	void HandleSaxEnd(const std::string& aElement);
private: // owned
	TMLAttributeSetParse* iSaxAttributeSet;
	};

/**
 * this is the main SAX parser implementation
 */
class TLayoutAttributesSaxParser : private Arabica::SAX::DefaultHandler<std::string>
	{
private:
	typedef stack<MSaxLayoutAttributesHandler*> TSaxAttributesHandlerStack;
	typedef Arabica::SAX::ErrorHandler<std::string>::SAXParseExceptionT TException;

public:
	TLayoutAttributesSaxParser(MSaxLayoutAttributesHandler* aHandler);
	void Parse(const std::string& aFileName);

private: // from basic_DefaultHandler
	void startElement(const std::string& namespaceURI, const std::string& localName, const std::string& qName, const Arabica::SAX::Attributes<std::string>& atts);
	void endElement(const std::string& namespaceURI, const std::string& localName, const std::string& qName);

	void warning(const TException& aException);
	void error(const TException& aException);
	void fatalError(const TException& aException);

private:
	TSaxAttributesHandlerStack iStack;	// stack content not owned
	};


#endif // MLATTRIBUTESPARSE_H

// End of File
