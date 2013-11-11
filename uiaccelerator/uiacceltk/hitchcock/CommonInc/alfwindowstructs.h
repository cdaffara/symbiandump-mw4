#ifndef __ALFWINDOWSTRUCTS_H__
#define __ALFWINDOWSTRUCTS_H__/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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



#include <graphics/wsgraphicdrawer.h>

class RMemWriteStream;
class RMemReadStream;

/**
 * Set of Integers that can be used to idnetify all window server windows in system
 */
NONSHARABLE_STRUCT(TWindowIdentifier)
    {
    /** 
    * Constructor, for convenience 
    **/
    //Default constructor
    TWindowIdentifier() {}
	
    TWindowIdentifier(TInt aWindowGroupId, TInt aWindowIdentifer ):
        iWindowGroupId(aWindowGroupId), iWindowIdentifer(aWindowIdentifer){}
    
    TBool IsNull() const
        {
        return (!(iWindowGroupId || iWindowIdentifer)); 
        }
    
    TBool Equals(const TWindowIdentifier& aId) const
        {
        return ((iWindowGroupId == aId.iWindowGroupId)&&(iWindowIdentifer==aId.iWindowIdentifer)); 
        }
    
    TInt iWindowGroupId;            
    TInt iWindowIdentifer;          
    };

enum TAlfWServFlags
    {
    EAlfWservFlagNone,
    EAlfWservFlagDeleted = 0x1      /** Window was already deleted from wserv scene **/
    };



enum TAlfWindovEvents
    {
    EAlfWindowDestroyed = 0x1,      /** Specific window was destroyed in wserv scene **/
    EAlfWindowPosChanged = 0x2,     /** Window Position in wserv scene was changed **/
    EAlfWindowSizeChanged = 0x4,    /** Window Size in wserv scene was changed **/
    EAlfWindowNewContent = 0x8,     /** New drawing commands to InBuffer()  **/
    EAlfWindowVisibile = 0x10,      /**  window changed to visible in wserv scene **/ 
    EAlfWindowNotVisibile = 0x20,   /** window was hidden in wserv scene **/     
    EAlfWindowIndexChanged = 0x40,  /** z-order of window changed on server **/
    };
    
/**
 * @struct TAlfWServInfo
 *
 * The struct provides encapsulation for specific wserv window data
 * Usually this data is for reference only, e.g. effects plugin must 
 * not alter the data
 **/    
NONSHARABLE_STRUCT(TAlfWServInfo)
    {
    /** 
    * Constructors, for convenience 
    **/
    TAlfWServInfo():
        iPosition(TPoint(0,0)), 
        iSize(TSize(0,0)), 
        iRefId(TWindowIdentifier(0,0)), 
        iClientSideId(TWindowIdentifier(0,0)), 
        iFlags(EAlfWservFlagNone),
        iScreenNumber(0),
        iParentRefId(TWindowIdentifier(0,0)){}


    TAlfWServInfo(const TPoint& aPosition, const TSize& aSize, TInt aWgNodeId, 
                  TInt aWsNodeId, TInt aWsClientHandle, TInt aWgClientHandle, TUint8 aScreenNumber = 0 ):
        iPosition(aPosition), 
        iSize(aSize), 
        iRefId(TWindowIdentifier(aWgNodeId, aWsNodeId)), 
        iClientSideId(TWindowIdentifier(aWsClientHandle, aWgClientHandle)), 
        iFlags(EAlfWservFlagNone),
        iScreenNumber( aScreenNumber ){}

    TPoint iPosition; /** Window position in WServ scene **/
    TSize iSize; /** Window Size in WServ scene **/
    TWindowIdentifier iRefId; /** Unique node ids between wserv and alf **/
    TWindowIdentifier iClientSideId; /** RWindow::ClientHandle(), RWindowGroup::Identifier() **/
    TUint iFlags; 
    MWsWindowTreeNode::TType  iNodeType;
    TUint8 iScreenNumber;
    TWindowIdentifier iParentRefId; /** Unique node ids between wserv and alf **/
    
#ifdef ALF_DEBUG_TRACK_DRAWING	
    TBool iTrackWindow;
#endif
    
    /** 
    * For convenience.
    * @return Indicates whether the window still exists in WServ scene 
    **/
    TBool ExistsInWServ() const
        {
        return !((iRefId.IsNull()) ||(iFlags & EAlfWservFlagDeleted));
        }
    };


NONSHARABLE_STRUCT(TChunkHeader)
    {
    TChunkHeader( ):iReadOffset(0), iWriteOffset(0),iMemWriteStream(NULL),iMemWriteStreamUsers(0),iMemReadStream(NULL){};
        
    TUint32 iReadOffset;
    TUint32 iWriteOffset;
    TUint32 iCommittedWriteOffset;
    RMemWriteStream* iMemWriteStream;
    TUint8 iMemWriteStreamUsers;
    RMemReadStream* iMemReadStream;
    };

#endif
