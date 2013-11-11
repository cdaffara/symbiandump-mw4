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
* Description: Header declaration
*
*/








#ifndef C_SEN_FILTER_STATE_MACHINE_H
#define C_SEN_FILTER_STATE_MACHINE_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>
#include <flogger.h>

#include "SenBaseFragment.h"
#include <SenXmlElement.h> // check if this include is needed(?)

#include "SenPointerMap.h"

#include "MSenFilterAction.h"
#include "SenFilterCondition.h"

// FORWARD DECLARATIONS

class CSenFilterStateNode;
class CSenFilterTableEntry;

// CONSTANTS and DATA TYPES

#define KBufferAllocationBlocks 10*1024

typedef RPointerArray<CSenFilterStateNode> RStateNodeArray;

//typedef RPointerArray<CSenFilterStateNode> RStateNodeArray;

//typedef RSenPointerMap<TInt, CSenFilterTableEntry> RStateTablePointerMap;

// CLASS DEFINITIONS

/**
 * @author lewontin
 * 
 * Implement an XML filter state machine. The supported filter syntax is a bit
 * like XPath, but adds an || operator so you can select together multiple nodes
 * below a specified node (e.g. "/a/b/c||d" selects both the c and d elements
 * below /a/b). The goal is to avoid the necessity of returning intermediate
 * nodes and then reparsing these one-by-one. Instead, the caller can collect
 * these target values together (into a data structure) during a single parse.
 * The parser will support the || operator on intermediate nodes, but this is
 * not recommended as it makes the parser state table very big.
 */
class CSenFilterStateMachine : public CBase
{

private: // State type enumeration.

    enum TStateType
    {
        EEnd = 0,
        EStart
    };


    // Objects to build a tree representation of the filter state machine

public: // Constructors and Destructors

    IMPORT_C CSenFilterStateMachine();


    IMPORT_C ~CSenFilterStateMachine();


    /*
     * The state tree is built up as a list of lists
     * Each and filter adds a new list of states to the filter chain
     * Each or filter appends a new set of states to the current list
     */


    /* private Vector filterChain = new Vector();
     private TInt chainIndex = -1;
     
      
      * Public methods to build filter chains. Parameters:
      * aEvent -- the element name that causes this transition
      *  aCondition -- further conditions for transition
      
     
     // Add an AND filter: implements the "/" operator
     
     public void andFilter(const TDesC8& aEvent, FilterCondition aCondition, FilterAction aStartAction, FilterAction aEndAction) throws FilterException{
        
         filterChain.addElement(new Vector());
         ++chainIndex;
         orFilter(aEvent, aCondition, aStartAction, aEndAction);
     }
     
     
      * Add an OR filter: implements the "|" operator. Note that OR filters in
      * non-terminal nodes blow up the deterministic state table exponentially so they should
      * be used sparingly.
      
     
     public void orFilter(const TDesC8& aEvent, FilterCondition aCondition, FilterAction aStartAction, FilterAction aEndAction) throws FilterException {
         
         if(chainIndex < 0){
             throw new FilterException("ORFilter cannot be first in chain");
         }
         
         // Get the current (possibly empty) list of OR states
         Vector s = (Vector)filterChain.elementAt(chainIndex);
        
         if(chainIndex == 0){
             // No previous states, just add a single new state node to the current list
             s.addElement(new CSenFilterStateNode(aEvent, aCondition, aStartAction, aEndAction));
         }
         
         else{
             //Previous state(s) exist(s). Add a state transition from each previous state.
             // Get the list of previous states 
             Enumeration pStates = ((Vector)filterChain.elementAt(chainIndex - 1)).elements();
             // For each previous state
             while(pStates.hasMoreElements()){
                 // Add a new state node to the current list
                 CSenFilterStateNode iState = new CSenFilterStateNode(aEvent, aCondition, aStartAction, aEndAction);
                 CSenFilterStateNode p = (CSenFilterStateNode)pStates.nextElement();
                 iState.ipPrev = p;
                 s.addElement(iState);
             }
         }
     }*/


    /* Add a node to the state tree */
    IMPORT_C CSenFilterStateNode* AddChildLC(CSenFilterStateNode& aStartState, 
                                             const TDesC8& aEvent,
                                             const CSenFilterCondition& aCondition,
                                             MSenFilterAction& aStartAction,
                                             MSenFilterAction& aEndAction);

    /* Add the root node to the state tree */
    IMPORT_C CSenFilterStateNode* AddParentLC(const TDesC8& aEvent,
                                              const CSenFilterCondition& aCondition,
                                              MSenFilterAction& aStartAction,
                                              MSenFilterAction& aEndAction);

    /*
     * Compile a state table by iterating over the state tree. Call this after building
     * a filter chain from AND and OR filters.
     */

    /* public void compile(){
         
         if(filterChain == NULL){ //Already compiled
             return;     
         }
         
         Enumeration  chain = filterChain.elements();
         while(chain.hasMoreElements()){
             Enumeration iStates = ((Vector)chain.nextElement()).elements();
             while(iStates.hasMoreElements()){
                 CSenFilterStateNode node = (CSenFilterStateNode)iStates.nextElement();
                 TInt p = 0;
                 if(node.ipPrev != NULL){
                     p = node.ipPrev.iState;
                 }
                 addEntry(p, node.iEvent, node.iCondition, node.iState, node.iStartAction, node.iEndAction);
             }
         }
         //Allow the state tree to be garbage collected
         filterChain = NULL;
     }
     */


    /* Recursively compile the tree into a state table */
    IMPORT_C void CompileL(CSenFilterStateNode& aStartNode);


    // Public methods called by parser

    IMPORT_C void StartElementL(const TDesC8& aName, 
                                const RAttributeArray& aAttributes);

    IMPORT_C void EndElementL(const TDesC8& aName);


    // Register a new state by incrementing the state counter
    // and returning this counter as unique ID for this session
    const TInt RegisterState();


    const TInt States() const;


private:

    // Private method to add an entry to the state table
    void AddEntryL(TInt aInitialState, 
                   const TDesC8& aEvent, 
                   const CSenFilterCondition& aCondition, 
                   TInt aNextState, 
                   MSenFilterAction& aStartAction, 
                   MSenFilterAction& aEndAction);


    // Create an unique key for state table node
    const TUint16 GetStateKeyL(TInt aState, 
                               const TDesC8& aEvent, 
                               TInt aStart);


private:

    // Methods to run the state machine
    const TInt TransitionL(TInt aState, 
                           const TDesC8& aEvent, 
                           TInt start, 
                           const RAttributeArray& aAttributes);



private:

    /**
     * File logger.
     *
     */
    RFileLogger* Log() const;


private:

    //The filter state machine state table
    RSenPointerMap<TUint16, CSenFilterTableEntry> iStateTable;

    TInt iFilterState;

    TInt iStates; // Counter of all states


    RFileLogger iLog;

}; // end class CSenFilterStateMachine


// ----------------------------------------
//
// Declaration of class CSenFilterStateNode
//
// ----------------------------------------

/**
     * A node in the state tree
     * Parent      -  points to the previous state in the diagram
     * Children    -  set of successor states in the diagram
     * State       -  state number for this state
     * Event       -  event that triggers transition from ipPrev to this
     * Condition   -  additional test on transition
     * StartAction - 
     * EndAction   - 
     */
class CSenFilterStateNode : public CBase
{

public:

    IMPORT_C CSenFilterStateNode(const TDesC8& aEvent, 
                                 const CSenFilterCondition& aCondition, 
                                 MSenFilterAction& aStartAction, 
                                 MSenFilterAction& aEndAction,
                                 CSenFilterStateMachine& aStateMachine);

    IMPORT_C CSenFilterStateNode(const CSenFilterStateNode& aFilterStateNode);


    IMPORT_C ~CSenFilterStateNode();


public:

    IMPORT_C TInt State() const;


    IMPORT_C TDesC8 Event() const;


    IMPORT_C CSenFilterCondition Condition() const;


    IMPORT_C MSenFilterAction& StartAction() const;


    IMPORT_C MSenFilterAction& EndAction() const;


    IMPORT_C CSenFilterStateNode& Parent() const;


    IMPORT_C RStateNodeArray Children() const;


    IMPORT_C void SetParent(CSenFilterStateNode& aNode);


private:

    /**
     * File logger.
     *
     */
    RFileLogger* Log() const;


private:

    TInt iState; // Node index
    TDesC8 iEvent;
    CSenFilterCondition iCondition;
    MSenFilterAction& iStartAction;
    MSenFilterAction& iEndAction;

    CSenFilterStateNode* ipPrev;

    RStateNodeArray iChildren;

    RFileLogger iLog;

}; // end class CSenFilterStateNode


// -----------------------------------------
//
// Declaration of class CSenFilterTableEntry
//
// -----------------------------------------

class CSenFilterTableEntry : public CBase
{

public:

    CSenFilterTableEntry(TInt aNewState, 
                         const CSenFilterCondition& aCondition,
                         MSenFilterAction& aAction);


    ~CSenFilterTableEntry();

public:

    TInt NewState() const;


    CSenFilterCondition Condition() const;


    MSenFilterAction& Action() const;


private:

    /**
     * File logger.
     *
     */
    RFileLogger* Log() const;


private:

    TInt iNewState;
    CSenFilterCondition iCondition;
    MSenFilterAction& iAction;

    RFileLogger iLog;

}; // end class CSenFilterTableEntry


#endif // C_SEN_FILTER_STATE_MACHINE_H

