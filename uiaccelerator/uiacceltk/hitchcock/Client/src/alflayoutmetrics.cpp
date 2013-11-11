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
* Description:   Layout metrics
*
*/



#include "alf/alflayoutmetrics.h"
#include "alf/alftextstylemanager.h"
#include "alf/alftextstyle.h"
#include "alf/alftextvisual.h"
#include "alf/alfvisual.h"
#include "alf/alfutil.h"
#include "alf/alfenv.h"
#include "alfclient.h"

#include <e32debug.h>
#include <avkon.hrh>
#include <flogger.h>
#include <AknFontCategory.hrh>


_LIT(KHeader,"Comp Type|id|variety|name|Alias|type|Width|Height|Left|Right|Top|Bottom|Remarks|parent_id|parent_variety|Multi Row|Number of rows|style 1|style 2|style 3|Concatenated rows");
_LIT(KDelimeterChar,"|");
_LIT(KTypeText,"Text");
_LIT(KTypePane,"Pane");
_LIT(KVariety,"0");
_LIT(KFontPrimary,"qfn_primary");
_LIT(KFontPrimarySmall,"qfn_primary_small"); 
_LIT(KFontSecondary,"qfn_secondary");
_LIT(KFontTitle,"qfn_title");
_LIT(KFontDigital,"qfn_digital");
_LIT(KFontEmpty," ");
_LIT(KFontUnknown,"unknown font");
_LIT(KRemarks," ");
_LIT(KParentVariety,"0");
_LIT(KMultiRowYes,"true");
_LIT(KMultiRowNo,"-");
_LIT(KNumberOfRowsNone,"-");
_LIT(KStyle1Plain,"plain");
_LIT(KStyle1Bold,"bold");
_LIT(KStyle1Empty,"-");
_LIT(KStyle2Regular,"regular");
_LIT(KStyle2Italic,"italic");
_LIT(KStyle2Empty,"-");
_LIT(KStyle3None,"none");
_LIT(KStyle3Empty,"-");
_LIT(KStyleConcatenatedRowsDefault,"true");
_LIT(KStyleConcatenatedRowsEmpty,"-");
_LIT(KNewLine,"\n");
_LIT(KReportFileNameDefaultStart, "\\pqp_apps_"); 
_LIT(KReportFileNameDefaultLscEnd, "_lsc.txt"); 
_LIT(KReportFileNameDefaultPrtEnd, "_prt.txt"); 



const TInt KParentIdStart = 200000;
const TInt KIdStart = 100000;


struct TMarkedVisualEntry
    {    
    public:
    	~TMarkedVisualEntry()
        {
        }
    	TMarkedVisualEntry(): 
    	    iLogicalName(NULL), 
    	    iAlias(NULL),
    	    iVisual(NULL),
    	    iTextSize(0,0),
    	    iMaxLineCount(0),
    	    iFontId(0),
    	    iTextStyle(EAlfTextStyleNotAHuiStyle),
    	    iVisualPosition(0,0),
    	    iVisualSize(0,0)                                           
        {                            
        }
    
    HBufC* iLogicalName; 
    HBufC* iAlias; 
    CAlfTextVisual* iVisual;
    TSize iTextSize;
    TInt iMaxLineCount;
    TInt iFontId;
    TAlfPreconfiguredTextStyle iTextStyle;
    TPoint iVisualPosition;
    TPoint iVisualSize;
    };

struct CAlfLayoutMetricsUtility::TMetricsPrivateData
    {
    RArray<TMarkedVisualEntry> iEntries; // Owned
    TBool iAutoMarking;
    CAlfEnv* iEnv;
    };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfLayoutMetricsUtility::CAlfLayoutMetricsUtility()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfLayoutMetricsUtility::~CAlfLayoutMetricsUtility()
    {
    if ( iMetricsData )
        {
        TInt count = iMetricsData->iEntries.Count();
        for(TInt i = 0; i < count; i++)
          	{
          	delete iMetricsData->iEntries[i].iLogicalName;
          	iMetricsData->iEntries[i].iLogicalName = NULL;
        	
        	delete iMetricsData->iEntries[i].iAlias;
        	iMetricsData->iEntries[i].iAlias = NULL;
        	}
        iMetricsData->iEntries.Close();   
        }
        
    delete iMetricsData;
    iMetricsData = NULL;
    }

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
CAlfLayoutMetricsUtility* CAlfLayoutMetricsUtility::NewL(CAlfEnv& aEnv)
	{
	CAlfLayoutMetricsUtility* self = 
		CAlfLayoutMetricsUtility::NewLC(aEnv);        
    CleanupStack::Pop( self );
    return self;		
	}
	

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
CAlfLayoutMetricsUtility* CAlfLayoutMetricsUtility::NewLC(CAlfEnv& aEnv)
	{
    CAlfLayoutMetricsUtility* self = new( ELeave ) CAlfLayoutMetricsUtility;
    CleanupStack::PushL( self );
    self->ConstructL(aEnv);
    return self;	
	}

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
void CAlfLayoutMetricsUtility::ConstructL(CAlfEnv& aEnv)
	{
    iMetricsData = new (ELeave) TMetricsPrivateData;
    iMetricsData->iAutoMarking = EFalse;
    iMetricsData->iEnv = &aEnv;
	}
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfLayoutMetricsUtility::MarkVisual(TDesC* aLogicalName, 
    CAlfTextVisual* aVisual)
    {
    TRAP_IGNORE(MarkVisualL(aLogicalName, aVisual))
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfLayoutMetricsUtility::MarkVisualL(TDesC* aLogicalName, 
    CAlfTextVisual* aVisual)
    {
    TInt count = iMetricsData->iEntries.Count();
    for(TInt i = 0; i < count; i++)
      	{
      	if (aVisual == iMetricsData->iEntries[i].iVisual)
      	    {
          	delete iMetricsData->iEntries[i].iLogicalName;
          	iMetricsData->iEntries[i].iLogicalName = NULL;
        	
        	delete iMetricsData->iEntries[i].iAlias;
        	iMetricsData->iEntries[i].iAlias = NULL;

            iMetricsData->iEntries.Remove(i);
            break;      	        
      	    }
    	}

    TMarkedVisualEntry entry;
    if (aLogicalName)
        {
        entry.iLogicalName = aLogicalName->AllocL();        
        }
    
    entry.iVisual = aVisual;
    iMetricsData->iEntries.Append(entry);        
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfLayoutMetricsUtility::UnmarkVisual(CAlfTextVisual* aVisual)
    {
    TInt count = iMetricsData->iEntries.Count();
    for(TInt i = 0; i < count; i++)
      	{
      	if (aVisual == iMetricsData->iEntries[i].iVisual)
      	    {
          	delete iMetricsData->iEntries[i].iLogicalName;
          	iMetricsData->iEntries[i].iLogicalName = NULL;
        	
        	delete iMetricsData->iEntries[i].iAlias;
        	iMetricsData->iEntries[i].iAlias = NULL;
            
            iMetricsData->iEntries.Remove(i);
            break;      	        
      	    }
    	}
        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfLayoutMetricsUtility::MeasureVisuals()
    {
    TRAP_IGNORE(MeasureVisualsL())
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfLayoutMetricsUtility::MeasureVisualsL()
    {
    TInt count = iMetricsData->iEntries.Count();
    for(TInt i = 0; i < count; i++)
      	{
      	CAlfTextVisual* visual = iMetricsData->iEntries[i].iVisual;
    	if (visual->EffectiveOpacity() > 0.0)
    	    {
        	iMetricsData->iEntries[i].iTextSize = 
        	    TSize(visual->Size().IntValueNow().iX - visual->HorizontalPadding()*2, 
        	    visual->TextExtents().iHeight);        	
        	
        	iMetricsData->iEntries[i].iMaxLineCount = visual->MaxLineCount();    	        
    	    // iMetricsData->iEntries[i].iFontId = visual->FontId();
    	    // CAlfTextVisual::FontId() has been deprecated for long and returns only zero.
    	    iMetricsData->iEntries[i].iFontId = 0;
    	    iMetricsData->iEntries[i].iTextStyle = visual->Style();      	        
    	    iMetricsData->iEntries[i].iVisualPosition = visual->Pos().IntValueNow();
    	    iMetricsData->iEntries[i].iVisualPosition.iX += visual->HorizontalPadding(); 
    	    iMetricsData->iEntries[i].iVisualSize = visual->Size().IntValueNow();
    	    iMetricsData->iEntries[i].iVisualSize.iX -= visual->HorizontalPadding(); 

        	delete iMetricsData->iEntries[i].iAlias;
        	iMetricsData->iEntries[i].iAlias = NULL;
            iMetricsData->iEntries[i].iAlias = visual->Text().AllocL();                 

            if (!iMetricsData->iEntries[i].iLogicalName)
                {
                iMetricsData->iEntries[i].iLogicalName = visual->Text().AllocL();         
                }
                
            // If max linecount has not been set to textvisual, 
            // we kindly here calculate it.    
            if (iMetricsData->iEntries[i].iMaxLineCount > 1)
                {
                if (iMetricsData->iEntries[i].iMaxLineCount == KMaxTInt && 
                    iMetricsData->iEntries[i].iTextSize.iHeight)
                    {                
                    iMetricsData->iEntries[i].iMaxLineCount = 
                        iMetricsData->iEntries[i].iVisualSize.iY/iMetricsData->iEntries[i].iTextSize.iHeight;
                    }
                }
    	    }
    	}        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfLayoutMetricsUtility::EnableAutoMarking(TBool aEnabled)
    {
    iMetricsData->iAutoMarking = aEnabled;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfLayoutMetricsUtility::AutoMarking()
    {
    return iMetricsData->iAutoMarking;                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfLayoutMetricsUtility::WriteReport()
    {  	   	
   	TBuf<512> b;
    
    b.Zero();
    b.Append(KHeader);
    b.Append(KNewLine);    
    
    PrintToFile(b);

    TInt count = iMetricsData->iEntries.Count();
    for(TInt i = 0; i < count; i++)
      	{
  	    TMarkedVisualEntry e = iMetricsData->iEntries[i];

        TSize screensize = AlfUtil::ScreenSize();
        
        const TDesC* multiRow = &KMultiRowNo;
    	TBuf<12> rows;
        rows.Zero();
    	
        if (e.iMaxLineCount > 1)
            {
            multiRow = &KMultiRowYes;
            rows.AppendNum(e.iMaxLineCount);        
            }
        else
            {
            rows.Append( KNumberOfRowsNone );    
            }    


    	TBuf<64> font;
        font.Zero();

    	TBuf<64> style1;
        style1.Zero();

    	TBuf<64> style2;
        style2.Zero();

    	TBuf<64> style3;
        style3.Zero();
        
        if (e.iFontId)
            {
            // We use secret knowledge to decode font id
        	const TUint encodedMask = 0x80000000; // 1 bits starting at bit 31 = 1000.0000.0000.0000.0000.0000.0000.0000
        	const TUint categoryMask = 0x0000000F; // 4 bits starting at bit 0 = 0000.0000.0000.0000.0000.0000.0000.1111
        	const TUint weightMask = 0x00000010; // 1 bits starting at bit 4 = 0000.0000.0000.0000.0000.0000.0001.0000
        	const TUint postureMask = 0x00000020; // 1 bits starting at bit 5 = 0000.0000.0000.0000.0000.0000.0010.0000        

            if ((e.iFontId & encodedMask) != 0)
                {
                TInt category = (e.iFontId & categoryMask);
                    {
                    if (category == KAknFontCategoryPrimary)
                        {
                        font.Append(KFontPrimary);                                                                                                                                        
                        }
                    else if (category == KAknFontCategorySecondary)
                        {
                        font.Append(KFontSecondary);                                                                                                            
                        }
                    else if (category == KAknFontCategoryTitle)
                        {
                        font.Append(KFontTitle);                                                                                
                        }
                    else if (category == KAknFontCategoryPrimarySmall)
                        {
                        font.Append(KFontPrimarySmall);                                                    
                        }
                    else if (category == KAknFontCategoryDigital)
                        {
                        font.Append(KFontDigital);                        
                        }
                    else
                        {
                        font.Append(KFontUnknown);                                                
                        }                            
                    }
                
                TInt weight = (e.iFontId & weightMask) >> 4;
                if (weight == EStrokeWeightBold)
                    {
                    style1.Append(KStyle1Bold);                                                
                    }
                else
                    {
                    style1.Append(KStyle1Plain);                        
                    }    
                
                TInt posture = (e.iFontId & postureMask) >> 5;
                if (posture == EPostureItalic)
                    {
                    style2.Append(KStyle2Italic);                        
                    }
                else
                    {                    
                    style2.Append(KStyle2Regular);
                    }    
                                    
                style3.Append(KStyle3None);                    
                }
            else
                {
                font.Append(KFontUnknown);                    
                style1.Append(KStyle1Plain);
                style2.Append(KStyle2Regular);
                style3.Append(KStyle3None);                    
                }
            }
        else
            {
            TInt fontStyleId = 0;
            iMetricsData->iEnv->Client().LayoutMetricsTextStyleData(fontStyleId, e.iTextStyle);

            CAlfTextStyleManager* tsm = &iMetricsData->iEnv->TextStyleManager();                                
            CAlfTextStyle* ts = tsm->TextStyle(e.iTextStyle);

            if (ts->IsBold())
                {
                style1.Append(KStyle1Bold);                        
                }
            else
                {                        
                style1.Append(KStyle1Plain);
                }

            if (ts->IsItalic())
                {
                style2.Append(KStyle2Italic);                        
                }
            else
                {
                style2.Append(KStyle2Regular);                        
                }    
                    
            style3.Append(KStyle3None);

            switch (fontStyleId)
                {                                
                case EAknLogicalFontPrimaryFont:
                    {
                    font.Append(KFontPrimary);
                    break;    
                    }
                case EAknLogicalFontSecondaryFont:
                    {
                    font.Append(KFontSecondary);                    
                    break;    
                    }
                case EAknLogicalFontTitleFont:
                    {
                    font.Append(KFontTitle);                                        
                    break;    
                    }
                case EAknLogicalFontPrimarySmallFont:
                    {
                    font.Append(KFontPrimarySmall);
                    break;    
                    }
                case EAknLogicalFontDigitalFont:
                    {
                    font.Append(KFontDigital);                    
                    break;    
                    }
                default:
                    {
                    font.Append(KFontPrimary);                    
                    break;    
                    }                                                                                      
                }    
            }

    	b.Zero();
        b.Append( KTypeText );    
        b.Append( KDelimeterChar );    
        b.AppendNum( i + KIdStart );    
        b.Append( KDelimeterChar );    
        b.Append( KVariety );    
        b.Append( KDelimeterChar );    
        b.Append( (_L("%S"), *e.iLogicalName));    
        b.Append( KDelimeterChar );    
        b.Append( (_L("%S"), *e.iAlias));    
        b.Append( KDelimeterChar );    
        b.Append( font );    
        b.Append( KDelimeterChar );    
        b.AppendNum( e.iTextSize.iWidth );    
        b.Append( KDelimeterChar );    
        b.AppendNum( e.iTextSize.iHeight );    
        b.Append( KDelimeterChar );    
        b.AppendNum( e.iVisualPosition.iX );    
        b.Append( KDelimeterChar );    
        b.AppendNum( screensize.iWidth - e.iVisualPosition.iX - e.iVisualSize.iX );    
        b.Append( KDelimeterChar );    
        b.AppendNum( e.iVisualPosition.iY );    
        b.Append( KDelimeterChar );    
        b.AppendNum( screensize.iHeight -  e.iVisualPosition.iY - e.iVisualSize.iY );    
        b.Append( KDelimeterChar );    
        b.Append( KRemarks );    
        b.Append( KDelimeterChar );    
        if (e.iMaxLineCount > 1)
            {
            b.AppendNum( i + KParentIdStart );        
            }
        else
            {
            b.Append( KParentVariety );                    
            }    
        b.Append( KDelimeterChar );    
        b.AppendNum( 0 );    // Parent variety, always 0
        b.Append( KDelimeterChar );    
	    b.Append( KMultiRowNo ); // Multi row attribute is in the parent pane
        b.Append( KDelimeterChar );    
	    b.Append( KNumberOfRowsNone ); // Number of rows attribute is in the parent pane
        b.Append( KDelimeterChar );    
        b.Append( style1 );    
        b.Append( KDelimeterChar );    
        b.Append( style2 );    
        b.Append( KDelimeterChar );    
        b.Append( style3 );    
        b.Append( KDelimeterChar );    
        b.Append( KStyleConcatenatedRowsDefault );    
        b.Append( KNewLine );    

        PrintToFile(b);

        // For multirow texts a parent pane is needed.
        if (e.iMaxLineCount > 1)
            {
            b.Zero(); 
            b.Append( KTypePane );    
            b.Append( KDelimeterChar );    
            b.AppendNum( i + KParentIdStart );    
            b.Append( KDelimeterChar );    
            b.Append( KVariety );    
            b.Append( KDelimeterChar );    
            b.Append( (_L("%S"), *e.iLogicalName));    
            b.Append( _L("_parent_pane"));    
            b.Append( KDelimeterChar );    
            b.Append( _L("Parent pane for "));    
            b.Append( (_L("%S"), *e.iLogicalName));    
            b.Append( _L(" (multirow text)"));                
            b.Append( KDelimeterChar );    
            b.Append( KFontEmpty );    
            b.Append( KDelimeterChar );    
            b.AppendNum( e.iTextSize.iWidth );    
            b.Append( KDelimeterChar );    
            b.AppendNum(e.iVisualSize.iY );    
            b.Append( KDelimeterChar );    
            b.AppendNum( e.iVisualPosition.iX );    
            b.Append( KDelimeterChar );    
            b.AppendNum( screensize.iWidth - e.iVisualPosition.iX - e.iVisualSize.iX );    
            b.Append( KDelimeterChar );    
            b.AppendNum( e.iVisualPosition.iY );    
            b.Append( KDelimeterChar );    
            b.AppendNum( screensize.iHeight -  e.iVisualPosition.iY - e.iVisualSize.iY );    
            b.Append( KDelimeterChar );    
            b.Append( KRemarks );    
            b.Append( KDelimeterChar );    
            b.AppendNum( 0 );    
            b.Append( KDelimeterChar );    
            b.Append( KParentVariety );    
            b.Append( KDelimeterChar );    
    	    b.Append( *multiRow );
            b.Append( KDelimeterChar );    
    	    b.Append( rows );
            b.Append( KDelimeterChar );    
            b.Append( KStyle1Empty );    
            b.Append( KDelimeterChar );    
            b.Append( KStyle2Empty );    
            b.Append( KDelimeterChar );    
            b.Append( KStyle3Empty );    
            b.Append( KDelimeterChar );    
            b.Append( KStyleConcatenatedRowsEmpty );    
            b.Append( KNewLine );                    
            PrintToFile(b);
            }
    	}        
    }
    

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
void CAlfLayoutMetricsUtility::PrintToFile( TDesC16& aBuf )
    {
    RFs fs;
   	TBuf<512> filename;
    filename.Zero();
    TSize screensize = AlfUtil::ScreenSize();
    filename.Append( KReportFileNameDefaultStart );
    if (screensize == TSize(240,320))
        {
        filename.Append(_L("qvga"));    
        filename.Append(KReportFileNameDefaultPrtEnd);
        }
    else if (screensize == TSize(320,240))
        {
        filename.Append(_L("qvga"));    
        filename.Append(KReportFileNameDefaultLscEnd);            
        }
    else if (screensize == TSize(480,640))
        {
        filename.Append(_L("vga"));    
        filename.Append(KReportFileNameDefaultPrtEnd);                                    
        }
    else if (screensize == TSize(640,480))
        {
        filename.Append(_L("vga"));    
        filename.Append(KReportFileNameDefaultLscEnd);                        
        }
    else if (screensize == TSize(360,640))
        {
        filename.Append(_L("qhd"));    
        filename.Append(KReportFileNameDefaultPrtEnd);                                    
        }
    else if (screensize == TSize(640,360))
        {
        filename.Append(_L("qhd"));    
        filename.Append(KReportFileNameDefaultLscEnd);                        
        }
    else if (screensize == TSize(320,480))
        {
        filename.Append(_L("hvga"));    
        filename.Append(KReportFileNameDefaultPrtEnd);                                    
        }
    else if (screensize == TSize(480,320))
        {
        filename.Append(_L("hvga"));    
        filename.Append(KReportFileNameDefaultLscEnd);                        
        }
    else
        {
        filename.AppendNum(screensize.iWidth);    
        filename.Append(_L("x"));    
        filename.AppendNum(screensize.iHeight);    
        if (screensize.iWidth < screensize.iHeight)
            {
            filename.Append(KReportFileNameDefaultPrtEnd);                                                    
            }
        else
            {
            filename.Append(KReportFileNameDefaultLscEnd);                                                                    
            }                
        }    
        
    
    if ( fs.Connect() == KErrNone )
        {
        RFile file;

        // Open file in an exclusive mode so that only one thread 
        // can acess it simultaneously
        TUint fileMode = EFileWrite | EFileShareExclusive;
       
        TInt err = file.Open( fs, filename, fileMode );

        // Create a file if it doesn't exist
        if ( err == KErrNotFound )
            {
            err = file.Create( fs, filename, fileMode );
            }

        // Check if we have access to a file
        if ( err == KErrNone )
            {
            TInt offset = 0;
            if ( file.Seek( ESeekEnd, offset ) == KErrNone )
                {
                // Append text to the end of file
                TPtr8 ptr8( (TUint8*)aBuf.Ptr(), aBuf.Size(), aBuf.Size() );
                file.Write( ptr8 );
                }
            file.Close();
            }

        fs.Close();
        }
    }



