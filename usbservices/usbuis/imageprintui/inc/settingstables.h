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
* Description:  Contain info about capabilities phone are supporting
*
*/




#ifndef SETTINGSTABLES_H
#define SETTINGSTABLES_H

#include <dpsdefs.h>
#include <imageprintui.rsg>

/**
* Definitions for phone supported capabilities
*/

const TInt KLayoutTableCount( 6 );
const TInt KLayoutTableIndexCount( 2 );

const TInt KPaperSizeTableCount( 6 );
const TInt KPaperSizeTableIndexCount( 2 );

const TInt KQualityTableCount( 4 );
const TInt KQualityTableIndexCount( 2 );

const TInt KTableCount( 16 ); 
const TInt KTableIndexCount( 2 ); 

struct TConversionTable
    {
    TInt iCapabilityID;
    TInt iResourceID; 
    };


const TConversionTable KLayoutTable[] = 
    { 
    { EDpsLayoutDefault,    R_USB_PRINT_LAYOUT_DEFAULT },
    { EDpsLayout1Up,        R_USB_PRINT_LAYOUT_1_UP },
    { EDpsLayout2Up,        R_USB_PRINT_LAYOUT_2_UP }, 
    { EDpsLayout4Up,        R_USB_PRINT_LAYOUT_4_UP }, 
    { EDpsLayout6Up,        R_USB_PRINT_LAYOUT_6_UP },  
    { EDpsLayout9Up,        R_USB_PRINT_LAYOUT_9_UP }
    }; 


const TConversionTable KPaperSizeTable[]=
    { 
    { EDpsPaperSizeDefault,    R_USB_PRINT_PAPER_SIZE_AUTOMATIC },
    { EDpsPaperSizeCard,       R_USB_PRINT_PAPER_SIZE_CARD }, 
    { EDpsPaperSize4x6,        R_USB_PRINT_PAPER_SIZE_INCH }, 
    { EDpsPaperSizeLetter,     R_USB_PRINT_PAPER_SIZE_LETTER }, 
    { EDpsPaperSizeA4,         R_USB_PRINT_PAPER_SIZE_A4 },
    { EDpsPaperSizeA6,         R_USB_PRINT_PAPER_SIZE_A6 }
    }; 


const TConversionTable KQualityTable[] =
    { 
    { EDpsPrintQualityDefault,    R_USB_PRINT_QUALITY_DEFAULT },
    { EDpsPrintQualityFine,       R_USB_PRINT_QUALITY_HIGH },
    { EDpsPrintQualityNormal,     R_USB_PRINT_QUALITY_NORMAL }, 
    { EDpsPrintQualityDraft,      R_USB_PRINT_QUALITY_DRAFT } 
    }; 


#endif