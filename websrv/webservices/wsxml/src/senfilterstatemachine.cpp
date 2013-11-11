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
*/








#include "SenFilterCondition.h"

#include "SenFilterStateMachine.h"

#include "SenDebug.h"
#include "SenLogger.h"


_LIT(KLogFileDir, "WsLog");
_LIT(KLogFileName, "SenXml.log");

// ----------------------------------------------
//
// Implementation of class CSenFilterStateMachine
//
// ----------------------------------------------


EXPORT_C CSenFilterStateMachine::CSenFilterStateMachine()
: iStateTable(ETrue, ETrue)
{
    TLSLOG_OPEN(KSenXmlLogChannel, KSenXmlLogLevel, KSenXml, KSenXmlLog);
    TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterStateMachine::CSenFilterStateMachine(): Log opened"));

    iStates = 0;
    iFilterState = 0;

		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel ,  _L8("CSenFilterStateMachine::CSenFilterStateMachine(): End"));
} 


EXPORT_C CSenFilterStateMachine::~CSenFilterStateMachine()
{
    iStateTable.Reset();

    // Close the log file and the connection to the server.
  	TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel ,  (_L("Log file closed.")));
		TLSLOG_CLOSE(KSenXmlLogChannel);
}


/* Add a node to the state tree */
EXPORT_C CSenFilterStateNode* 
CSenFilterStateMachine::AddChildLC(CSenFilterStateNode& aStartState,
                                 const TDesC8& aEvent, 
                                 const CSenFilterCondition& aCondition,
                                 MSenFilterAction& aStartAction,
                                 MSenFilterAction& aEndAction)
{
    CSenFilterStateNode* newNode = new CSenFilterStateNode(aEvent, 
                                                           aCondition, 
                                                           aStartAction, 
                                                           aEndAction, 
                                                           *this);

    CleanupStack::PushL(newNode);
    newNode->SetParent(aStartState);

    // Array takes care of memory deallocation
    aStartState.Children().Append(newNode);

    return newNode;
}


/* Add the root node to the state tree */
EXPORT_C CSenFilterStateNode* 
CSenFilterStateMachine::AddParentLC(const TDesC8& aEvent, 
                                 const CSenFilterCondition& aCondition,
                                 MSenFilterAction& aStartAction,
                                 MSenFilterAction& aEndAction)
{
    CSenFilterStateNode* newNode = new CSenFilterStateNode(aEvent,
                                                           aCondition, 
                                                           aStartAction, 
                                                           aEndAction, 
                                                           *this);
    CleanupStack::PushL(newNode);
    return newNode;
}


/* Recursively compile the tree into a state table */
EXPORT_C void CSenFilterStateMachine::CompileL(CSenFilterStateNode& aStartNode)
{
    TInt p = 0; // initial state

    p = aStartNode.Parent().State();

    AddEntryL(p, 
             aStartNode.Event(), 
             aStartNode.Condition(), 
             aStartNode.State(), 
             aStartNode.StartAction(), 
             aStartNode.EndAction());

    TInt count = aStartNode.Children().Count();
    for ( TInt i=0; i < count; i++ )
    {
        CSenFilterStateNode* node = (aStartNode.Children())[i];
        CompileL(*node);
    }
}


EXPORT_C void CSenFilterStateMachine::StartElementL(const TDesC8& aName, 
                                                    const RAttributeArray& aAttributes)
{
    TransitionL(iFilterState, aName, (TInt)EStart, aAttributes);
}


EXPORT_C void CSenFilterStateMachine::EndElementL(const TDesC8& aName)
{
    RAttributeArray attrs;
    TransitionL(iFilterState, aName, (TInt)EEnd, attrs);
}



const TInt CSenFilterStateMachine::RegisterState()
{
    return ++iStates;
}


const TInt CSenFilterStateMachine::States() const
{
    return iStates;
}


void CSenFilterStateMachine::AddEntryL(TInt aInitialState, 
                                      const TDesC8& aEvent, 
                                      const CSenFilterCondition& aCondition, 
                                      TInt aNextState, 
                                      MSenFilterAction& aStartAction, 
                                      MSenFilterAction& aEndAction)
{
    CSenFilterTableEntry *entry = NULL;
    TUint16 c;


    // Start node
    c = GetStateKeyL(aInitialState, 
                    aEvent, 
                    (TInt)EStart);

    entry = new CSenFilterTableEntry(aNextState, 
                                     aCondition, 
                                     aStartAction);
    CleanupStack::PushL(entry);

    // Map takes care of memory deallocation
    iStateTable.Append(&c, entry);

    CleanupStack::Pop(); // entry

    // End node
    c = GetStateKeyL(aNextState, 
                    aEvent, 
                    (TInt)EEnd);

    entry = new CSenFilterTableEntry(aInitialState, 
                                     aCondition, 
                                     aEndAction);
    CleanupStack::PushL(entry);

    // Map takes care of memory deallocation
    iStateTable.Append(&c, entry);

    CleanupStack::Pop(); // entry

}


// Create an unique key for state table node
const TUint16 CSenFilterStateMachine::GetStateKeyL(TInt aState, 
                                                  const TDesC8& aEvent, 
                                                  TInt aStart)
{
    TUint16 ret = 0;
    TInt intMaxLen = 32;

    TInt bufLen = aEvent.Size() + (2 * intMaxLen);
    HBufC8* buf = HBufC8::NewLC(bufLen);
    TPtr8 bufPtr = (buf->Des());
    bufPtr.Append(aEvent);
    bufPtr.AppendNum(aState);
    bufPtr.AppendNum(aStart);

    Mem::Crc(ret, (TAny *)bufPtr.Ptr(), bufPtr.Size());

    CleanupStack::PopAndDestroy(); // buf

    return(ret);
}


const TInt CSenFilterStateMachine::TransitionL(TInt aState, 
                                              const TDesC8& aEvent, 
                                              TInt start, 
                                              const RAttributeArray& aAttributes)
{

    TUint16 c = GetStateKeyL(aState, aEvent, start);
    TInt i = iStateTable.Find(c);
    if ( KErrNotFound != i ) {
        const CSenFilterTableEntry* entry = iStateTable.ValueAt(i);
        if ( entry != NULL )
        {
            // Evaluate conditions
            if ( !entry->Condition().Test() )
            {
                return iFilterState;
            }
            //Perform action
            entry->Action().PerformL(aEvent, aAttributes);
    
            // Make the state transition
            iFilterState = entry->NewState();
        }
    }
    return iFilterState;
}


RFileLogger* CSenFilterStateMachine::Log() const
    {
    return (RFileLogger*) &iLog;
    }


// -------------------------------------
//
// Implementation of class CSenFilterStateNode
//
// -------------------------------------


EXPORT_C CSenFilterStateNode::CSenFilterStateNode(const TDesC8& aEvent, 
                                                  const CSenFilterCondition& aCondition, 
                                                  MSenFilterAction& aStartAction, 
                                                  MSenFilterAction& aEndAction,
                                                  CSenFilterStateMachine& aStateMachine)
: ipPrev(NULL),
  iEvent(aEvent),
  iCondition(aCondition),
  iStartAction(aStartAction),
  iEndAction(aEndAction)
{
    TLSLOG_OPEN(KSenXmlLogChannel, KSenXmlLogLevel, KSenXml, KSenXmlLog);
    TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterStateNode::CSenFilterStateNode(): Log opened"));

    iState = aStateMachine.RegisterState();

    TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterStateNode::CSenFilterStateNode(): End"));
} 


EXPORT_C CSenFilterStateNode::CSenFilterStateNode(const CSenFilterStateNode& aFilterStateNode)
: ipPrev(NULL),
  iEvent(aFilterStateNode.Event()),
  iCondition(aFilterStateNode.Condition()),
  iStartAction(aFilterStateNode.StartAction()),
  iEndAction(aFilterStateNode.EndAction())
{
    TLSLOG_OPEN(KSenXmlLogChannel, KSenXmlLogLevel, KSenXml, KSenXmlLog);
    TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterStateNode::CSenFilterStateNode(): Log opened"));

    iState = aFilterStateNode.State();
    

    if (NULL != aFilterStateNode.ipPrev)
    {
        ipPrev = new CSenFilterStateNode(aFilterStateNode.Parent());
    }
    
    for (TInt i = 0; i < aFilterStateNode.Children().Count(); i++)
    {
        iChildren.Append((const_cast<CSenFilterStateNode>(aFilterStateNode).Children())[i]);
    }

    TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterStateNode::CSenFilterStateNode(): End"));
}


EXPORT_C CSenFilterStateNode::~CSenFilterStateNode()
{
    iChildren.ResetAndDestroy();

    // Close the log file and the connection to the server.
    TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L("Log file closed."));
 		TLSLOG_CLOSE(KSenXmlLogChannel);
}


EXPORT_C TInt CSenFilterStateNode::State() const
{
    return iState;
}


EXPORT_C TDesC8 CSenFilterStateNode::Event() const
{
    return iEvent;
}


EXPORT_C CSenFilterCondition CSenFilterStateNode::Condition() const
{
    return iCondition;
}


EXPORT_C MSenFilterAction& CSenFilterStateNode::StartAction() const
{
    return iStartAction;
}


EXPORT_C MSenFilterAction& CSenFilterStateNode::EndAction() const
{
    return iEndAction;
}


EXPORT_C CSenFilterStateNode& CSenFilterStateNode::Parent() const
{
    return *ipPrev;
}


EXPORT_C RStateNodeArray CSenFilterStateNode::Children() const
{
    return iChildren;
}


EXPORT_C void CSenFilterStateNode::SetParent(CSenFilterStateNode& aNode)
{
    ipPrev = &aNode;
}


RFileLogger* CSenFilterStateNode::Log() const
    {
    return (RFileLogger*) &iLog;
    }


// --------------------------------------
//
// Implementation of class CSenFilterTableEntry
//
// --------------------------------------


CSenFilterTableEntry::CSenFilterTableEntry(TInt aNewState, 
                                           const CSenFilterCondition& aCondition,
                                           MSenFilterAction& aAction)
: iAction(aAction),
  iCondition(aCondition)
{
    TLSLOG_OPEN(KSenXmlLogChannel, KSenXmlLogLevel, KSenXml, KSenXmlLog);
    TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterTableEntry::CSenFilterTableEntry(): Log opened"));

    iNewState = aNewState;

    TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterTableEntry::CSenFilterTableEntry(): End"));
}


CSenFilterTableEntry::~CSenFilterTableEntry()
{
    // Close the log file and the connection to the server.
    TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L("Log file closed."));
		TLSLOG_CLOSE(KSenXmlLogChannel);
}


TInt CSenFilterTableEntry::NewState() const
{
    return iNewState;
}


CSenFilterCondition CSenFilterTableEntry::Condition() const
{
    return iCondition;
}


MSenFilterAction& CSenFilterTableEntry::Action() const
{
    return iAction;
}


RFileLogger* CSenFilterTableEntry::Log() const
    {
    return (RFileLogger*) &iLog;
    }



