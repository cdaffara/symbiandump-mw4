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
*/


// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

#include "MLEqCompData2DHuiML.h"
#include "MLEqCompDataParse.h"
#include "LayoutCompilerErr.h"
#include "FormulaTree.h"

#include <CdlCompilerToolkit/CdlTkProcess.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "CodeGenConsts.h"


using namespace std;

typedef LayoutProcessArgsErr<MLEqCompDataToDHuiML> MLEqCompDataToDHuiMLArgsErr;

// constants

const string KCompDataFileNameSuffix("compData");
const string KAttributesFileNameSuffix("attributes");


//
// class TEqLayoutSaxWriter
//


class TEqLayoutSaxWriter
	{
public:
	enum 
		{
		KIndentSize=4
		};
public:
	TEqLayoutSaxWriter(ofstream& aStream);
    void StartDocument();
    void EndDocument();
	void OpenStartElement(const string& aLocalName);
	void CloseStartElement();
	void CloseAndEndStartElement();
	void EndElement(const string& aLocalName);
	void Attribute(const string& aLocalName, const string& aValue);
	void Comment(const string& aComment);

private:
	void Indent();
	void IncreaseIndentLevel();
	void DecreaseIndentLevel();
private:
	ofstream& iStream;
	int iIndentLevel;
	};

TEqLayoutSaxWriter::TEqLayoutSaxWriter(ofstream& aStream)
	: 
	iStream(aStream),
	iIndentLevel(0)
	{

	};

void TEqLayoutSaxWriter::StartDocument()
	{
	Indent();
	iStream << string("<xmluiml xmlns=\"http://www.series60.com/xml/xmluiml/1\">") << endl;
	IncreaseIndentLevel();
	}

void TEqLayoutSaxWriter::EndDocument()
	{
	DecreaseIndentLevel();
	Indent();
	iStream << string("</xmluiml>") << endl;
	}

//void TEqLayoutSaxWriter::StartElement(const string& /*namespaceURI*/, const string& localName /*, const string& qName*/ /*, const AttributesT& atts*/)
void TEqLayoutSaxWriter::OpenStartElement(const string& aLocalName)
	{
	Indent();
	iStream << "<" << aLocalName;
	IncreaseIndentLevel();
	}

void TEqLayoutSaxWriter::CloseStartElement()
	{
	iStream << ">" << endl;
	}

void TEqLayoutSaxWriter::CloseAndEndStartElement()
	{
	iStream << " />" << endl;
	DecreaseIndentLevel();
	}

void TEqLayoutSaxWriter::EndElement(const string& aLocalName)
	{
	DecreaseIndentLevel();
	Indent();
	iStream << "</" << aLocalName << ">" << endl;
	}

void TEqLayoutSaxWriter::Attribute(const string& aLocalName, const string& aValue)
	{
	iStream << " " << aLocalName << "=\"" << aValue << "\"";
	}

void TEqLayoutSaxWriter::Comment(const string& aComment)
	{
	iStream << "<!--  " << aComment << " -->" << endl;
	}

void TEqLayoutSaxWriter::Indent()
	{
	stringstream ss;
	int max = iIndentLevel * KIndentSize;
	for(int count = 0; count < max; count++)
		{
		ss << " ";
		}
	iStream << ss.str();
	}

void TEqLayoutSaxWriter::IncreaseIndentLevel()
	{
	if(iIndentLevel > 1000)
		throw GeneralErr("TEqLayoutSaxWriter::DecreaseIndentLevel - nesting inconsistency");
	iIndentLevel++;
	}

void TEqLayoutSaxWriter::DecreaseIndentLevel()
	{
	if(iIndentLevel == 0)
		throw GeneralErr("TEqLayoutSaxWriter::DecreaseIndentLevel - nesting inconsistency");
	iIndentLevel--;
	}



// 
// MLEqCompDataToDHuiML
//

int MLEqCompDataToDHuiML::Process(const vector<string>& args)
	{
	int numExpectedArgs = 5;
	int numArgs = args.size();
	if(numArgs < numExpectedArgs)
		throw MLEqCompDataToDHuiMLArgsErr();
	
    int arg = 2;
	string listName = args[arg++];
	string compDataName = args[arg++];
	string parChildName = args[arg++];
	string dhuimlName = args[arg++];

	auto_ptr<TMLEqCompDataParseLayout> layoutParse = TMLEqCompDataParseLayout::Parse(listName, compDataName, parChildName);
	auto_ptr<TMLEqCompData> layout(layoutParse.get());
	layoutParse.release();

	TMLEqCompData* theLayout = layout.get();
	theLayout->Compile();

	cout << "writing dhuiml file " << dhuimlName << endl;
	ofstream dhuiml(dhuimlName.c_str());

	//int KIndent = 4;
	TEqLayoutSaxWriter saxWriter(dhuiml);
	saxWriter.StartDocument();

	stringstream comment1;
	comment1 << "Generated from list: " << listName;
	saxWriter.Comment(comment1.str());

	stringstream comment2;
	comment2 << "Generated from component data: " << compDataName;
	saxWriter.Comment(comment2.str());

	stringstream comment3;
	comment3 << "Generated from parent child data: " << parChildName;
	saxWriter.Comment(comment3.str());

	LayoutTablesToDeclarative(saxWriter, *theLayout);
    
	saxWriter.EndDocument();

	return 0;
	}

void MLEqCompDataToDHuiML::ShowHelp(ostream& stream)
	{
	stream << "MLEqCompData2DHuiML <MLEqCompListName> <MLEqCompDataName> <MLEqParChildName> <DHuiMLName>" << endl;
    stream << "  e.g. from \\S60\\AknLayout2\\group run " << endl;
    stream << "  aknlayoutcompiler MLEqCompData2DHuiML " << endl;
	stream << "      ..\\..\\xml_master\\xml\\m_p_30\\m_p_30_list.xml " << endl;
	stream << "      ..\\..\\xml_master\\xml\\m_p_30\\m_p_30\\0.161093285736413\\eur_compData.xml " << endl;
	stream << "      ..\\..\\xml_master\\xml\\m_p_30\\m_p_30\\0.161093285736413\\eur_parChildRelation.xml " << endl;
	stream << "      ..\\generated_declarative\\lct_avkon.dhuiml" << endl;
	}

void MLEqCompDataToDHuiML::LayoutTablesToDeclarative(TEqLayoutSaxWriter& aSaxWriter, const TMLEqCompData& aLayout)
	{
	// first the tables are output
	for (TMLEqCompData::const_iterator pTab = aLayout.begin(); pTab != aLayout.end(); ++pTab)
		{
		AddTableToDeclarative(aSaxWriter, **pTab);
		}
	}

void MLEqCompDataToDHuiML::LayoutComponentsToDeclarative(TEqLayoutSaxWriter& aSaxWriter, const TMLEqCompData& aLayout)
	{
	// then the individual components are output
	for (TMLEqCompData::TMLEqCompDataComponents::const_iterator pLine = aLayout.iComponents.begin(); pLine != aLayout.iComponents.end(); ++pLine)
		{
		// @todo this may be needed if the components are to be written out separately from the tables
		}
	}


const string KDHuiMLLayoutmanager("layoutmanager");
const string KDHuiMLLayoutmanagerId("id");
const string KDHuiMLLayoutmanagerName("name");
const string KDHuiMLOptionSet("optionset");
const string KDHuiMLOptionSetId("id");
const string KDHuiMLOptionSetOrientationPortrait("portrait");
const string KDHuiMLOptionSetOrientationLandscape("landscape");
const string KDHuiMLOptionSetOrientationTrue("true");
const string KDHuiMLOptionSetOrientationFalse("false");

void MLEqCompDataToDHuiML::AddTableToDeclarative(TEqLayoutSaxWriter& aSaxWriter, TMLEqCompDataTable& aTable)
	{
	if(aTable.size() == 0)
		return;
	
//	<layoutmanager id="1233" name="listscroll_app_pane">
//		<optionset id="0" portrait="true" landscape="false">
//			<lctcomponent id="1234" name="grid_app_pane">

	aSaxWriter.OpenStartElement(KDHuiMLLayoutmanager);
	aSaxWriter.Attribute(KDHuiMLLayoutmanagerId, CdlTkUtil::IntToString(aTable.iId));
	aSaxWriter.Attribute(KDHuiMLLayoutmanagerName, aTable.iName);
	aSaxWriter.CloseStartElement();

	// iterate through option sets
	for(TMLEqCompDataTable::iterator pOptionSet = aTable.begin(); pOptionSet != aTable.end(); ++pOptionSet)
		{
		int optionSetId = pOptionSet->first;
		TMLEqCompDataTableOptionSet& optionSet = pOptionSet->second;
		aSaxWriter.OpenStartElement(KDHuiMLOptionSet);
		aSaxWriter.Attribute(KDHuiMLOptionSetId, CdlTkUtil::IntToString(optionSetId));

		string isPortrait = (optionSet.iOrientation & EMLEqCompDataOptionSetOrientationPortrait) ? KDHuiMLOptionSetOrientationTrue : KDHuiMLOptionSetOrientationFalse;
		aSaxWriter.Attribute(KDHuiMLOptionSetOrientationPortrait, isPortrait);
		string isLandscape = (optionSet.iOrientation & EMLEqCompDataOptionSetOrientationLandscape) ? KDHuiMLOptionSetOrientationTrue : KDHuiMLOptionSetOrientationFalse;
		aSaxWriter.Attribute(KDHuiMLOptionSetOrientationLandscape, isLandscape);

		aSaxWriter.CloseStartElement();

		for(TMLEqCompDataTableOptionSet::iterator pLine = optionSet.begin(); pLine != optionSet.end(); ++pLine)
			{
			AddLineToDeclarative(aSaxWriter, **pLine, optionSetId);
			}
		aSaxWriter.EndElement(KDHuiMLOptionSet);
		}

	aSaxWriter.EndElement(KDHuiMLLayoutmanager);
	}


const string KDHuiMLLCTComponent("lctcomponent");
const string KDHuiMLLCTComponentId("id");
const string KDHuiMLLCTComponentName("name");
const string KDHuiMLAlfLayout("alflayout");
const string KDHuiMLAlfLayoutType("type");
const string KDHuiMLAlfLayoutTypeAnchor("anchor");

void MLEqCompDataToDHuiML::AddLineToDeclarative(TEqLayoutSaxWriter& aSaxWriter, TMLEqCompDataLine& aLine, int aOptionSetId)
	{
	// beginning of component

//		<lctcomponent id="1234" name="grid_app_pane">
//			<alflayout type="anchor">

	aSaxWriter.OpenStartElement(KDHuiMLLCTComponent);
	aSaxWriter.Attribute(KDHuiMLLCTComponentId, CdlTkUtil::IntToString(aLine.iId));
	aSaxWriter.Attribute(KDHuiMLLCTComponentName, aLine.iName);
	aSaxWriter.CloseStartElement();

	aSaxWriter.OpenStartElement(KDHuiMLAlfLayout);
	aSaxWriter.Attribute(KDHuiMLAlfLayoutType, KDHuiMLAlfLayoutTypeAnchor);
	aSaxWriter.CloseStartElement();

	const string* outputOrder =  KEqCompDataPaneOutputOrder;
	const int outputSize = KEqCompDataPaneOutputOrderSize;
	vector<string> cellOrder(outputOrder, outputOrder + outputSize);

	for (vector<string>::iterator pCell = cellOrder.begin(); pCell != cellOrder.end(); ++pCell)
		{
		string cellName = *pCell;
		AddCellToDeclarative(aSaxWriter, aLine[cellName], cellName, aOptionSetId);
		}

	aSaxWriter.EndElement(KDHuiMLAlfLayout);
	aSaxWriter.EndElement(KDHuiMLLCTComponent);
	}

const string KDHuiMLAnchor("anchor");
const string KDHuiMLAnchorOrdinal("ordinal");
const string KDHuiMLAnchorType("type");
const string KDHuiMLAnchorTypePrefix("EAlfAnchorType"); // not currently used, to save disk space
const string KDHuiMLAnchorAttachmentOrigin("attachmentOrigin");
const string KDHuiMLAnchorAttachmentOriginPrefix("EAlfAnchorAttachmentOrigin");  // not currently used, to save disk space
const string KDHuiMLAnchorAttachmentOrdinal("attachmentOrdinal");
const string KDHuiMLAnchorAttachmentOrdinalAttachToParent("P");
const string KDHuiMLAnchorOffset("offset");
const string KDHuiMLAnchorOffsetMagnitude("magnitude");
const string KDHuiMLAnchorOffsetUnit("unit");
const string KDHuiMLAnchorOffsetUnitPixel("S60");

void MLEqCompDataToDHuiML::AddCellToDeclarative(TEqLayoutSaxWriter& aSaxWriter, TMLEqCompDataValues& aValues, const string& aCellName, int aOptionSetId)
	{
//				<anchor 
//					ordinal="0" // don't need this, as declarative can calculate drawing order at runtime
//					type="EAlfAnchorTypeLeft"
//					attachmentOrigin="EAlfAnchorAttachmentOriginLeft"
//					attachmentOrdinal="EAlfAnchorAttachToParent">
//			        <offset 
//						magnitude="20"
//						unit="EAlfUnitPixel" 
//						zoomId = "233" />
//					<formula formulaStr=""> // @todo this is temporary whilst equation evaluation is developed
//						...
//					</formula>
//				</anchor>

	bool validCell(false);
	TMLEqCompDataFormula* formula = NULL;
	TMLEqCompDataValuesOptionSet& optionSet = aValues.iOptionSets[aOptionSetId];
	// always assume it's the first and only forumla
	if(optionSet.size() > 0)
		{
		formula = &(optionSet[0]);
		if(!formula->iFormulaString.empty())
			{
			validCell = true;
			}
		}

	if(!validCell)
		return;

	aSaxWriter.OpenStartElement(KDHuiMLAnchor);

//		aSaxWriter.Attribute(KDHuiMLAnchorOrdinal, 0); // don't need this, as declarative can calculate drawing order at runtime

	string anchorType = aCellName;
	ConvertPositionValueStr(anchorType);
	aSaxWriter.Attribute(KDHuiMLAnchorType, anchorType); 

	// @todo for now, always attach to the same side, but need to calculate this for e.g. orthogonal anchors
	aSaxWriter.Attribute(KDHuiMLAnchorAttachmentOrigin, anchorType);

	aSaxWriter.Attribute(KDHuiMLAnchorAttachmentOrdinal, KDHuiMLAnchorAttachmentOrdinalAttachToParent);
	
	aSaxWriter.CloseStartElement();

		{
		aSaxWriter.OpenStartElement(KDHuiMLAnchorOffset);
		aSaxWriter.Attribute(KDHuiMLAnchorOffsetMagnitude, "0"); // @todo, replace with value in units!
		aSaxWriter.Attribute(KDHuiMLAnchorOffsetUnit, KDHuiMLAnchorOffsetUnitPixel); // @todo, replace with s60 units when supported
		aSaxWriter.CloseAndEndStartElement();
		}

	AddFormulaToDeclarative(aSaxWriter, *formula);
	
	aSaxWriter.EndElement(KDHuiMLAnchor);
	}


//
// this is all temporary, to illustrate equation parsing
//


const string KDHuiMLAnchorFormula("formula");
const string KDHuiMLAnchorFormulaString("formulastr");
const string KDHuiMLAnchorFormulaNode("node");
const string KDHuiMLAnchorFormulaNodeType("type");
const string KDHuiMLAnchorFormulaNodeValue("value");


void Print(TEqLayoutSaxWriter& aSaxWriter, const FormulaTreeNode& aNode)
	{
	aSaxWriter.OpenStartElement(KDHuiMLAnchorFormulaNode);
	stringstream ss;

	switch (aNode.Type())
		{
		case FormulaTreeNode::EReal:		// Double() = the number
			ss << aNode.Real();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "real");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			break;
		case FormulaTreeNode::EInt:			// Int() = the number
			ss << aNode.Int();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "int");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			break;
		case FormulaTreeNode::ECell:			// Char() = cell name
			ss << aNode.Char();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "cell");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			break;
		case FormulaTreeNode::EParent:		// nothing special
			ss << aNode.Char();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "parent");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			break;
		case FormulaTreeNode::EParentCell:	// Char() = parent cell name
			ss << aNode.Char();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "parent_cell");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			break;
		case FormulaTreeNode::ETableCell:		// Double() = target table
			ss << aNode.Real();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "table_cell");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			break;
		case FormulaTreeNode::EComponent:		// Double() = component id, [0] = cell name
			ss << aNode.Real();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "component");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			Print(aSaxWriter, aNode[0]);
			break;
		case FormulaTreeNode::EAbsolute:		// Text() = whole thing, [0], [1] = real components, [2] = cell name
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "absolute_cell_ref");
//			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			Print(aSaxWriter, aNode[0]);
			Print(aSaxWriter, aNode[1]);
			Print(aSaxWriter, aNode[2]);
			break;
		case FormulaTreeNode::EUnits:			// Double() = units
			ss << aNode.Real();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "units");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			break;
		case FormulaTreeNode::EConstant:		// Double() = constant
			ss << aNode.Real();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "constant");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			break;
		case FormulaTreeNode::EAttribute:		// Int() = attribute
			ss << aNode.Int();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "attribute");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			break;
		case FormulaTreeNode::EMystery:		// Text() = whole thing, [0], [1] = int components
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "mystery");
//			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			Print(aSaxWriter, aNode[0]);
			Print(aSaxWriter, aNode[1]);
			break;
		case FormulaTreeNode::EFunction:		// Text() = function name, [0] = parameter
			ss << aNode.Text();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "function");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			Print(aSaxWriter, aNode[0]);
			break;
		case FormulaTreeNode::EArithmetic:	// Char() = arithmetic operator, [0], [1] = sub expressions
			ss << aNode.Char();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "arithmetic");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			Print(aSaxWriter, aNode[0]);
			Print(aSaxWriter, aNode[1]);
			break;
		case FormulaTreeNode::ECondition:		// Text() = comparison operator, [0], [1] = sub expressions
			ss << aNode.Text();
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "condition");
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			Print(aSaxWriter, aNode[0]);
			Print(aSaxWriter, aNode[1]);
			break;
		case FormulaTreeNode::EConditional:	// no content, [0] = condition, [1] = then expression, [2] = else expression
			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeType, "if_then_else");
//			aSaxWriter.Attribute(KDHuiMLAnchorFormulaNodeValue, ss.str());
			aSaxWriter.CloseStartElement();
			Print(aSaxWriter, aNode[0]);
			Print(aSaxWriter, aNode[1]);
			Print(aSaxWriter, aNode[2]);
			break;
		}
	aSaxWriter.EndElement(KDHuiMLAnchorFormulaNode);
	}


void MLEqCompDataToDHuiML::AddFormulaToDeclarative(TEqLayoutSaxWriter& aSaxWriter, const TMLEqCompDataFormula& aFormula)
	{
//		<formula formulaStr=""> // @todo this is temporary whilst equation evaluation is developed
//			<node type="123" value="456"/>
//		</formula>		
	aSaxWriter.OpenStartElement(KDHuiMLAnchorFormula);
	aSaxWriter.Attribute(KDHuiMLAnchorFormulaString, aFormula.iFormulaString); // @todo, replace with expanded formula xml
	aSaxWriter.CloseStartElement();

	if(aFormula.iFormulaTree)
		{
		Print(aSaxWriter, *(aFormula.iFormulaTree));
		}

	aSaxWriter.EndElement(KDHuiMLAnchorFormula);
	}

extern const string KEqCompDataKeywordParamLeft;
extern const string KEqCompDataKeywordParamTop;
extern const string KEqCompDataKeywordParamRight;
extern const string KEqCompDataKeywordParamBottom;
extern const string KEqCompDataKeywordParamWidth;
extern const string KEqCompDataKeywordParamHeight;

const string KDHuiMLAnchorLeft("left");
const string KDHuiMLAnchorTop("top");
const string KDHuiMLAnchorRight("right");
const string KDHuiMLAnchorBottom("bottom");
const string KDHuiMLAnchorWidth("width");
const string KDHuiMLAnchorHeight("height");

void MLEqCompDataToDHuiML::ConvertPositionValueStr(std::string& aValueStr)
	{
	if(aValueStr == KEqCompDataKeywordParamLeft) 
		aValueStr = KDHuiMLAnchorLeft;
	else if(aValueStr == KEqCompDataKeywordParamTop) 
		aValueStr = KDHuiMLAnchorTop;
	else if(aValueStr == KEqCompDataKeywordParamRight) 
		aValueStr = KDHuiMLAnchorRight;
	else if(aValueStr == KEqCompDataKeywordParamBottom) 
		aValueStr = KDHuiMLAnchorBottom;
	else if(aValueStr == KEqCompDataKeywordParamWidth) 
		aValueStr = KDHuiMLAnchorWidth;
	else if(aValueStr == KEqCompDataKeywordParamHeight) 
		aValueStr = KDHuiMLAnchorHeight;
	else
		aValueStr = KDHuiMLAnchorLeft;
		}


// end of file
