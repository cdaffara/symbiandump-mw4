/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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








// INCLUDE FILES
#include "senhandlerbase.h" 

EXPORT_C CSenHandlerBase::CSenHandlerBase()
    {
    
    }

EXPORT_C CSenHandlerBase::~CSenHandlerBase()
    {
    // destroy any base member allocations here.
    }


EXPORT_C void CSenHandlerBase::ProcessHandlerContextL(MSenHandlerContext& /* aCtx */)
    {
    }

EXPORT_C void CSenHandlerBase::WriteAsXMLToL(RWriteStream& aWs)
    {
    CSenBaseFragment::WriteAsXMLToL(aWs);
    }

EXPORT_C HBufC8* CSenHandlerBase::AsXmlL()
    {
    return CSenBaseFragment::AsXmlL();
    }


// End of File
