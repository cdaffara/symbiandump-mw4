/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpTmInfoElement class implementation.
*
*/

//  Include Files  
#include "upnptminfoelement.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptminfoelementTraces.h"
#endif

// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpTmInfoElement::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpTmInfoElement* CUpnpTmInfoElement::NewL( TTerminalModeInfoType aTerminalModeInfoType )
    {
    OstTraceFunctionEntry0( CUPNPTMINFOELEMENT_NEWL_ENTRY );
    CUpnpTmInfoElement* self = new (ELeave) CUpnpTmInfoElement();   
    CleanupStack::PushL(self);
    self->ConstructL( aTerminalModeInfoType );
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CUPNPTMINFOELEMENT_NEWL_EXIT );
    return self;    
    }

// ---------------------------------------------------------------------------------
// CUpnpTmInfoElement::CUpnpTmInfoElement
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpTmInfoElement::CUpnpTmInfoElement( )
     {

     }

// ---------------------------------------------------------------------------------
// CUpnpTmInfoElement::ConstructL
// Symbian 2nd phase constructor can leave.
// @param aTerminalModeInfoType    Type of info element. Enum value.
// @see TTerminalModeInfoType enumeration
// ---------------------------------------------------------------------------------
//
void CUpnpTmInfoElement::ConstructL(  TTerminalModeInfoType aTerminalModeInfoType )
    {
    OstTraceFunctionEntry0( CUPNPTMINFOELEMENT_CONSTRUCTL_ENTRY );
    switch( aTerminalModeInfoType )
        {
        case ERemotingInfo:
            {
            iInfoTypeBuffer = KRemotingInfo().AllocL();     
            }
            break;
        case EAppInfo:
            {
            iInfoTypeBuffer =  KAppInfo().AllocL();          
            }
            break;
        case EDisplayInfo:
            {
            iInfoTypeBuffer = KDisplayInfo().AllocL();           
            }
            break;
        case EAudioInfo:
            {
            iInfoTypeBuffer = KAudioInfo().AllocL();           
            }
            break;
        default: 
            {
            // Undefined. Should not come here at all.
            User::Leave(KErrArgument);
            }
            break;
        }
    OstTraceFunctionExit0( CUPNPTMINFOELEMENT_CONSTRUCTL_EXIT );
    }

// ------------------------------------------------------------------------------------------
// CUpnpTmInfoElement::~CUpnpTmInfoElement
// Destructor
// ------------------------------------------------------------------------------------------
//
CUpnpTmInfoElement::~CUpnpTmInfoElement()
    {
    OstTraceFunctionEntry0( CUPNPTMINFOELEMENT_CUPNPTMINFOELEMENT_ENTRY );
    // Cleans up the arrays
    for ( TInt i(0); i < iInfoElementNameArray.Count(); i++ )
        {
        iInfoElementNameArray[i].Close();    
        }
    for ( TInt i(0); i < iInfoElementValueArray.Count(); i++ )
        {
        iInfoElementValueArray[i].Close();    
        }
    iInfoElementNameArray.Close();
    iInfoElementValueArray.Close(); 
    delete iInfoTypeBuffer;
    OstTraceFunctionExit0( CUPNPTMINFOELEMENT_CUPNPTMINFOELEMENT_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmInfoElement::AddTmInfoElementL
// Method is used to add key-value pair
// @param aName   Name of the key
// @param aValue  value for the key
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpTmInfoElement::AddTmInfoElementL( const TDesC8& aName,
                                                         const TDesC8& aValue )                                                                     
    {
    OstTraceFunctionEntry0( CUPNPTMINFOELEMENT_ADDTMINFOELEMENTL_ENTRY );
    iElementName.CreateL(aName);
    iElementValue.CreateL(aValue);
    iInfoElementNameArray.AppendL(iElementName);
    iInfoElementValueArray.AppendL(iElementValue);
    OstTraceFunctionExit0( CUPNPTMINFOELEMENT_ADDTMINFOELEMENTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmInfoElement::GetTmInfoElement
// Method is used to fetch the key-value pair for the index provided in the input.
// @param aInfoElementName[out]   Name of the key
// @param aInfoElementValue[out]  Value for the key
// @param aIndex     Index of the array for which key-value pair is needed
// ---------------------------------------------------------------------------------
//
void CUpnpTmInfoElement::GetTmInfoElementL( RBuf8& aInfoElementName, 
                                           RBuf8& aInfoElementValue, TInt aIndex )const
    {
    OstTraceFunctionEntry0( CUPNPTMINFOELEMENT_GETTMINFOELEMENTLIST_ENTRY );
    OstTrace1( TRACE_NORMAL, CUPNPTMINFOELEMENT_GETTMINFOELEMENTL, "CUpnpTmInfoElement::GetTmInfoElementL;aIndex=%d", aIndex );
    if ( ( aIndex < KErrNone ) || ( aIndex >= iInfoElementNameArray.Count()) )
        {
        // Leaves in case of invalid array index
        User::Leave(KErrArgument);
        }
    aInfoElementName.Close();
    aInfoElementName.CreateL(iInfoElementNameArray[aIndex]); 
    aInfoElementValue.Close();
    aInfoElementValue.CreateL(iInfoElementValueArray[aIndex]);
    OstTraceFunctionExit0( CUPNPTMINFOELEMENT_GETTMINFOELEMENTLIST_EXIT );
    }

