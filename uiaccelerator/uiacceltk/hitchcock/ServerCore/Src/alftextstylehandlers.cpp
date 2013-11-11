/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Text style handlers
*
*/



#include "alftextstylehandlers.h"
#include "alf/alfconstants.h"
#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/huitextstylemanager.h>
#include <gdi.h>

//-----------------------------------------------------------------------------
// Create a handler for platform, preconfigured, or cloned text style
//-----------------------------------------------------------------------------
TAlfTextStyleHandler::TAlfTextStyleHandler(
    MAlfInterfaceProvider& aResolver, 
    TInt aId1, 
    TInt aId2,
    TInt aConstructionType)
	: iResolver(aResolver), iTextStyle( NULL )
	{
	// Retrieve the text style manager
	CHuiTextStyleManager& manager = aResolver.SharedHuiEnv()->TextStyleManager();
    TInt id = KErrNotFound;
	

    switch(aConstructionType)
        {
        case EAlfPlatformTextStyleCreate:
        	TRAP_IGNORE(
                {
                // Get parent text style handler
                TAlfTextStyleHandler* parentHandler = 
                    static_cast<TAlfTextStyleHandler*>(
                        aResolver.GetInterfaceL(EAlfTextStyleHandler, aId2));
        	
                // Create a platform text style
                id = manager.CreatePlatformTextStyleL(
                    aId1, 
                    parentHandler->TextStyleId());
                })
                
            iTextStyle = manager.TextStyle(id);
        break;
        
        case EAlfPreconfiguredTextStyleCreate:
	        iTextStyle = manager.TextStyle(aId1);
        break;
        
        case EAlfPlatformTextStyleCopy:
        	TRAP_IGNORE(
                {
                // Get parent text style handler
                TAlfTextStyleHandler* sourceHandler = 
                    static_cast<TAlfTextStyleHandler*>(
                        aResolver.GetInterfaceL(EAlfTextStyleHandler, aId1));
        	
                // Create a platform text style
                id = manager.CopyTextStyleL(sourceHandler->TextStyleId());
                })
                
        	iTextStyle = manager.TextStyle(id);
        break;
        
        default:
        break;
        }
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void TAlfTextStyleHandler::Release()
	{
	if ( iTextStyle )
	    {
	    CHuiTextStyleManager& manager = iResolver.SharedHuiEnv()->TextStyleManager();
	    (void)manager.DeleteTextStyle( iTextStyle->Id() ); // ignore error
	    }
    delete this;	
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
TAny* TAlfTextStyleHandler::GetInterface(const THuiInterfaceSupport& aInterface)
	{
	if(aInterface == EAlfTextStyleHandler)
		{
		return this;
		}
	return NULL;
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void TAlfTextStyleHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& aResponse)
	{
    switch (aCommandId)
        {
		case EAlfTextStyleSetParent:
		    {
		    TInt* param = (TInt*) aInputBuffer.Ptr();		    
			// Get parent text style handler
			TAlfTextStyleHandler* parentHandler = 
			    static_cast<TAlfTextStyleHandler*>(
			        iResolver.GetInterfaceL(EAlfTextStyleHandler, *param));
			iTextStyle->SetParentId(parentHandler->TextStyleId());
		    break;		    	
		    }
		    
        case EAlfTextStyleSetTextColor:
	        {
	        TRgb* param = (TRgb*) aInputBuffer.Ptr();
	        iTextStyle->SetTextColor(*param);
	        break;    
	        }    
    
        case EAlfTextStyleSetTextSizeInTwips:
	        {
	        TInt2* params = (TInt2*) aInputBuffer.Ptr();
	        iTextStyle->SetTextSizeInTwips(params->iInt1, params->iInt2);
	        break;
	        }

        case EAlfTextStyleSetTextSizeInPixels:
	        {
	        TInt2* params = (TInt2*) aInputBuffer.Ptr();
	        iTextStyle->SetTextSizeInPixels(params->iInt1, params->iInt2);
	        break;
	        }
		        
        case EAlfTextStyleSetBold:
	        {
	        TInt* param = (TInt*) aInputBuffer.Ptr();
	        iTextStyle->SetStrokeWeight(*param);
	        break;
	        }	        
	        
        case EAlfTextStyleSetItalic:
	        {
	        TInt* param = (TInt*) aInputBuffer.Ptr();
	        iTextStyle->SetPosture(*param);
	        break;	        	
	        }
	        
        case EAlfTextStyleSetUnderline:
	        {
	        TInt* param = (TInt*) aInputBuffer.Ptr();
	        iTextStyle->SetUnderline(*param);
	        break;	        		        	
	        }
	        
        case EAlfTextStyleSetStrikeThrough:
	        {
	        TInt* param = (TInt*) aInputBuffer.Ptr();
	        iTextStyle->SetStrikeThrough(*param);
	        break;	
	        }
	        
        case EAlfTextStyleTextColor:
            {            
            const TRgb textColor = iTextStyle->TextColor();
            TPckg<TRgb> resultPckg(textColor);
            aResponse = resultPckg;
            break;
            }
            
        case EAlfTextStyleSizeInTwips:
	        {
	        TInt* param = (TInt*) aInputBuffer.Ptr();
            const TInt textSizeInTwips = iTextStyle->TextSizeInTwips(*param);
            TPckg<TInt> resultPckg(textSizeInTwips);
            aResponse = resultPckg;
	        break;	
	        }
        	
        case EAlfTextStyleSizeInPixels:
	        {
	        TInt* param = (TInt*) aInputBuffer.Ptr();
            const TInt textSizeInPixels = iTextStyle->TextSizeInPixels(*param);
            TPckg<TInt> resultPckg(textSizeInPixels);
            aResponse = resultPckg;
	        break;		        	
	        }
            
        case EAlfTextStyleIsBold:
	        {
            const TBool isBold = iTextStyle->StrokeWeight();
            TPckg<TBool> resultPckg(isBold);
            aResponse = resultPckg;
            break;	        	
	        }
	        
        case EAlfTextStyleIsItalic:
	        {
	        const TBool isItalic = iTextStyle->Posture();
	        TPckg<TBool> resultPckg(isItalic);
	        aResponse = resultPckg;
	        break;
	        }	        
	        
        case EAlfTextStyleIsUnderline:
	        {	        
            const TBool isUnderline = iTextStyle->Underline();
            TPckg<TBool> resultPckg(isUnderline);
            aResponse = resultPckg;
            break;
	        }
        
        case EAlfTextStyleIsStrikeThrough:
	        {
            const TBool isStrikeThrough = iTextStyle->StrikeThrough();
            TPckg<TBool> resultPckg(isStrikeThrough);
            aResponse = resultPckg;
            break;
	        }

        case EAlfTextStyleGetTypeface:
	        {
	        TTypeface typeface;
            iTextStyle->GetTypeface( typeface );
            TPckg<TTypeface> resultPckg(typeface);
            aResponse = resultPckg;
            break;
	        }

        case EAlfTextStyleSetTextPaneHeightInPixels:
	        {
	        TInt2* params = (TInt2*) aInputBuffer.Ptr();
	        iTextStyle->SetTextPaneHeightInPixels(params->iInt1, params->iInt2);
	        break;
	        }
        case EAlfTextStyleServerHandle:
	        {
	        TInt id = iTextStyle->Id();
	        TPckg<TInt> resultPckg(id);
            aResponse = resultPckg;
	        break;
	        }
            
        default:
            User::Leave(KErrNotSupported);
        }	
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
TInt TAlfTextStyleHandler::TextStyleId() const
	{
	return iTextStyle->Id();
	}
