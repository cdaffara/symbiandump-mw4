/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Inlined node methods
*
*/



#ifndef __DMADRTNODE_INL__
#define __DMADRTNODE_INL__

inline CArrayPtr<CDmAdCommand>* CDmAdRtNode::Commands(void) const
    {
    return iCommands;
    }

inline TBool CDmAdRtNode::IsSomeLeafAddedToRtNode(void) const
    {
    return iSomeLeafAddedToRtNode;
    }

inline TBool CDmAdRtNode::IsJustFetched(void) const
    {
    return iJustFetched;
    }

inline CDmAdRtNodeData* CDmAdRtNode::Data(void) const
    {
    return iData;
    }

inline void CDmAdRtNode::SetSomeLeafAddedToRtNode(TBool aSomeLeafAddedToRtNode)
    {
    iSomeLeafAddedToRtNode = aSomeLeafAddedToRtNode;
    }

#endif
