/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MLEQCOMPDATAPARSE_H
#define MLEQCOMPDATAPARSE_H

// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

#include "LayoutCompilerErr.h"
#include "MLEqCompData.h"

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
#define ELayoutCompilerFontCategoryUndefined KAknFontCategoryUndefined /* 0 */
#define ELayoutCompilerFontCategoryPrimary KAknFontCategoryPrimary /* 1*/
#define ELayoutCompilerFontCategorySecondary KAknFontCategorySecondary /* 2 */
#define ELayoutCompilerFontCategoryTitle KAknFontCategoryTitle /* 3 */
#define ELayoutCompilerFontCategoryPrimarySmall KAknFontCategoryPrimarySmall /* 4 */
#define ELayoutCompilerFontCategoryDigital KAknFontCategoryDigital /* 5 */


class MSaxLayoutEqHandler
	{
public:
	typedef Arabica::SAX::Attributes<std::string> TAttribs;
	virtual MSaxLayoutEqHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs) { return this; };
	virtual void HandleSaxEnd(const std::string& aElement) { };
	};



class TMLEqCompDataParseFormula : public TMLEqCompDataFormula, public MSaxLayoutEqHandler
	{
public:
	TMLEqCompDataParseFormula();
public: // from MSaxLayoutEqHandler
	MSaxLayoutEqHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
public:
	void HandleSaxEnd(const std::string& aElement);
	void HandleSaxFormulaAttributes(const std::string& aElement, const TAttribs& aAttribs);
private:
	string iSaxFormulaString;
	};


class TMLEqCompDataParseValuesOptionSet : public TMLEqCompDataValuesOptionSet, public MSaxLayoutEqHandler
	{
public:
	TMLEqCompDataParseValuesOptionSet();
public: // from MSaxLayoutEqHandler
	MSaxLayoutEqHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
public:
	void HandleSaxEnd(const std::string& aElement);
	void HandleSaxOptionSetNo(const std::string& aElement, const TAttribs& aAttribs);
	MSaxLayoutEqHandler* HandleSaxFormula(const std::string& aElement, const TAttribs& aAttribs);
public:
	int OptionSetNo() const {return iSaxOptionSetNo;}
private:
	TMLEqCompDataParseFormula* iSaxFormula;
	int iSaxFormulaNo;
	int iSaxOptionSetNo;
	};

// this corresponds to a "param" in the xml
class TMLEqCompDataParseValues : public TMLEqCompDataValues, public MSaxLayoutEqHandler
	{
public:
	TMLEqCompDataParseValues(TMLEqCompDataLine* aLine);
	static int ConvertZoomStr(const std::string& aValueStr);
public: // from MSaxLayoutEqHandler
	MSaxLayoutEqHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
private:
	void HandleSaxEnd(const std::string& aElement);
	MSaxLayoutEqHandler* HandleSaxOptionSet(const std::string& aElement, const TAttribs& aAttribs);
private:
	void ConvertValueStr(std::string& aValueStr);
private:
	TMLEqCompDataParseValuesOptionSet* iSaxOptionSet;
	};


// this corresponds to a "component" in the xml
class TMLEqCompDataParseLine : public TMLEqCompDataLine, public MSaxLayoutEqHandler
	{
public:
	TMLEqCompDataParseLine();
	string ShortParamName(string& aName);
public: // from MSaxLayoutEqHandler
	MSaxLayoutEqHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
private:
	void HandleSaxEnd(const std::string& aElement);
	void HandleSaxAttributes(const TAttribs& aAttribs);
//	MSaxLayoutEqHandler* HandleSaxParent(const std::string& aElement, const TAttribs& aAttribs);
	MSaxLayoutEqHandler* HandleSaxParam(const std::string& aElement, const TAttribs& aAttribs);
//	MSaxLayoutEqHandler* HandleSaxAttributeInfo(const std::string& aElement, const TAttribs& aAttribs);
private:
	TMLEqCompDataParseValues* iSaxValues;
//	TMLEqCompDataParseParentInfo* iSaxParentInfo;
//	TMLEqCompDataParseAttributeInfo* iSaxAttributeInfo;
	};


// This is the component element stored in the list xml. 
class TMLEqListParseComponent : public TMLEqListComponent, public MSaxLayoutEqHandler
	{
public:
	TMLEqListParseComponent();
public: // from MSaxLayoutEqHandler
	MSaxLayoutEqHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
	void HandleSaxEnd(const std::string& aElement);
	};


// This is the component element stored in the list xml. 
class TMLEqParChildParseComponent : public TMLEqParChildComponent, public MSaxLayoutEqHandler
	{
public:
	TMLEqParChildParseComponent();
public: // from MSaxLayoutEqHandler
	MSaxLayoutEqHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
	void HandleSaxEnd(const std::string& aElement);
private:
	void HandleSaxParent(const std::string& aElement, const TAttribs& aAttribs);
	};

// this corresponds to a layout in the xml
class TMLEqCompDataParseLayout : public TMLEqCompData, public MSaxLayoutEqHandler
	{
public:
	static auto_ptr<TMLEqCompDataParseLayout> Parse(const string& aListName, const string& aCompDataName, const string& aParChildName);
public: // from MSaxLayoutEqHandler
	MSaxLayoutEqHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
	void HandleSaxEnd(const std::string& aElement);
private:
	MSaxLayoutEqHandler* HandleSaxComponent(const std::string& aElement, const TAttribs& aAttribs);
private:
	TMLEqCompDataParseLine* iSaxLine;
	string iSaxLayoutType;
	};


class TEqLayoutSaxParser : private Arabica::SAX::DefaultHandler<std::string>
	{
private:
	typedef stack<MSaxLayoutEqHandler*> TSaxHandlerStack;
	typedef Arabica::SAX::ErrorHandler<std::string>::SAXParseExceptionT TException;

public:
	TEqLayoutSaxParser(MSaxLayoutEqHandler* aHandler);
	void Parse(const std::string& aFileName);

private: // from basic_DefaultHandler
	void startElement(const std::string& namespaceURI, const std::string& localName, const std::string& qName, const Arabica::SAX::Attributes<std::string>& atts);
	void endElement(const std::string& namespaceURI, const std::string& localName, const std::string& qName);

	void warning(const TException& aException);
	void error(const TException& aException);
	void fatalError(const TException& aException);

private:
	TSaxHandlerStack iStack;	// stack content not owned
	};


#endif // MLEQCOMPDATAPARSE_H

// End of File
