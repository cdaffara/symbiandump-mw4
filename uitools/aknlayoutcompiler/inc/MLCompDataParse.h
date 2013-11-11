/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MLCOMPDATAPARSE_H
#define MLCOMPDATAPARSE_H

// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

#include "LayoutCompilerErr.h"
#include "MLCompData.h"

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


class MSaxLayoutHandler
	{
public:
	typedef Arabica::SAX::Attributes<std::string> TAttribs;
	virtual MSaxLayoutHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs) { return this; };
	virtual void HandleSaxEnd(const std::string& aElement) { };
	};


// this corresponds to a "param" in the xml
class TMLCompDataParseValues : public TMLCompDataValues, public MSaxLayoutHandler
	{
public:
	TMLCompDataParseValues(TMLCompDataLine* aLine);
	static int ConvertZoomStr(const std::string& aValueStr);
public: // from MSaxLayoutHandler
	MSaxLayoutHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
private:
	void HandleSaxEnd(const std::string& aElement);
	void HandleSaxVariety(const std::string& aElement, const TAttribs& aAttribs);
	void HandleSaxCalc(const std::string& aElement, const TAttribs& aAttribs);
private:
	void ConvertValueStr(std::string& aValueStr);
private:
	TMLCompDataZoomLevels* iSaxZoomLevels;
	int iSaxVariety;
	};

// this corresponds to the contents of "ParentInfo" in the xml
class TMLCompDataParseParentInfo : public TMLCompDataParentInfo, public MSaxLayoutHandler
	{
public:
	TMLCompDataParseParentInfo(TMLCompDataLine* aLine);
public: // from MSaxLayoutHandler
	MSaxLayoutHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
private:
	void HandleSaxEnd(const std::string& aElement);
	void HandleSaxParent(const std::string& aElement, const TAttribs& aAttribs);
	void HandleSaxVariety(const std::string& aElement, const TAttribs& aAttribs);
private:
	TMLCompDataParentInfoSelector* iSaxParentInfoSelector;
	int iSaxVariety;
	};


class TMLCompDataParseAttributeInfo;

// this corresponds to a "component" in the xml
class TMLCompDataParseLayoutLine : public TMLCompDataLine, public MSaxLayoutHandler
	{
public:
	TMLCompDataParseLayoutLine();
	string ShortParamName(string& aName);
public: // from MSaxLayoutHandler
	MSaxLayoutHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
private:
	void HandleSaxEnd(const std::string& aElement);
	void HandleSaxAttributes(const TAttribs& aAttribs);
	MSaxLayoutHandler* HandleSaxParent(const std::string& aElement, const TAttribs& aAttribs);
	MSaxLayoutHandler* HandleSaxParam(const std::string& aElement, const TAttribs& aAttribs);
	MSaxLayoutHandler* HandleSaxAttributeInfo(const std::string& aElement, const TAttribs& aAttribs);
private:
	TMLCompDataParseValues* iSaxValues;
	TMLCompDataParseParentInfo* iSaxParentInfo;
	TMLCompDataParseAttributeInfo* iSaxAttributeInfo;
	};

// this corresponds to the contents of "AttributeInfo" in the xml
class TMLCompDataParseAttributeInfo : public TMLCompDataAttributeInfo, public MSaxLayoutHandler
	{
public:
	TMLCompDataParseAttributeInfo(TMLCompDataLine* aLine);
public: // from MSaxLayoutHandler
	MSaxLayoutHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
private:
	void HandleSaxEnd(const std::string& aElement);
	void HandleSaxVariety(const std::string& aElement, const TAttribs& aAttribs);
	void HandleSaxAttributeSet(const std::string& aElement, const TAttribs& aAttribs);
private:
	TMLCompDataAttributeInfoSelector* iSaxAttributeInfoSelector;
	int iSaxVariety;
	};



// this corresponds to a common parent component, and therefore does
// not correspond directly to a structure in the xml, although in general
// there is one per non-leaf component
class TMLCompDataParseLayoutTable : public TMLCompDataTable, public MSaxLayoutHandler
	{
public:
	TMLCompDataParseLayoutTable(TMLCompData* aTables);
public: // from MSaxLayoutHandler
	MSaxLayoutHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
	};

// this corresponds to a layout in the xml
class TMLCompDataParseLayout : public TMLCompData, public MSaxLayoutHandler
	{
public:
	static auto_ptr<TMLCompDataParseLayout> Parse(const string& aLayName);
public: // from MSaxLayoutHandler
	MSaxLayoutHandler* HandleSax(const std::string& aElement, const TAttribs& aAttribs);
	};


class TLayoutSaxParser : private Arabica::SAX::DefaultHandler<std::string>
	{
private:
	typedef stack<MSaxLayoutHandler*> TSaxHandlerStack;
	typedef Arabica::SAX::ErrorHandler<std::string>::SAXParseExceptionT TException;

public:
	TLayoutSaxParser(MSaxLayoutHandler* aHandler);
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


#endif // MLCOMPDATAPARSE_H

// End of File
