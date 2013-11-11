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
* Description:   Layout metrics
*
*/



#ifndef C_ALFLAYOUTMETRICS_H
#define C_ALFLAYOUTMETRICS_H

#include <e32base.h>

class CAlfTextVisual;
class CAlfEnv;
struct TMarkedVisualEntry;

/**
 *  Layout metrics utility for measuring text visuals. Measuring is
 *  is done based on text style or font id. Mesuring texts that has been 
 *  defined using font specification are not yet supported.
 *  
 *
 *  Example of use:
 *  
 *  void CHelloWorldAppUi::ConstructL()
 *     {
 *     iEnv = CAlfEnv::NewL();
 *  #ifdef __DEBUG
 *     // Enable layout metrics utility visual automarking in debug builds.
 *     iEnv->LayoutMetricsUtility()->EnableAutoMarking(); 
 *  #endif
 *     ...
 *     ...
 *     // Create some textvisuals, these will be automatically included
 *     // into metrics reports. 
 *     iTextVisual = CAlfTextVisual::AddNewL(*this, iLayout);
 *     iTextVisual->SetText(KMyText);
 *     iTextVisual->SetStyle(EAlfTextStyleSmall);
 *     ...
 *     ...
 *     }
 *
 *   // handle any menu commands
 *  void CHelloWorldAppUi::HandleCommandL(TInt aCommand)
 *     {
 *     switch(aCommand)
 *         {
 *         case EHellowWorldCmdMeasureVisuals:
 *             {
 *             // Measure currently visible text visuals
 *             iEnv->LayoutMetricsUtility()->MeasureVisuals();
 *             // Write a metrics report into a file 
 *             iEnv->LayoutMetricsUtility()->WriteReport();
 *             break;
 *             }
 *          ...
 *          ...    
 *         }
 *     ...
 *     }        
 *
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfLayoutMetricsUtility ): public CBase
    {
public:

    /**
     * Destructor.
     */
    virtual ~CAlfLayoutMetricsUtility();
    
    /**
     * 2-phase constructor.
     */
    static CAlfLayoutMetricsUtility* NewL(CAlfEnv& aEnv);

    /**
     * 2-phase constructor.
     */
    static CAlfLayoutMetricsUtility* NewLC(CAlfEnv& aEnv);
    
    /**
     * Marks visual to be included into layout metric reports.
     * @param aLogicalName Logical name for the visual, e.q. "title_pane_t1" or "qtn_title_text"...
     *                     If NULL is given, text visual text will be used as logical name
     *                     in the reports.
     * @param aVisual      The visual to be included into reports.
     */
    IMPORT_C void MarkVisual(TDesC* aLogicalName, CAlfTextVisual* aVisual);
    
    /**
     * Unmarks visual, it will not be included into layout metric reports.
     * @param aVisual      The visual to be excluded from reports.
     */
    IMPORT_C void UnmarkVisual(CAlfTextVisual* aVisual);

    /**
     * Measures all currently visible visuals.
     */
    IMPORT_C void MeasureVisuals();

    /**
     * Writes a layout metric report into a file, by default C: drive is used.
     * Report contains metrics data for each visual at the time of last call to the
     * MeasureVisuals()-method.
     *
     * Report is intended to be used when text strings lenghts are decided and localized to 
     * different language variants.
     *
     * Report contains data for each measured textvisual such as:
     *
     * - Component type ("Text" for text visual)
     * - Id (Generated id number)
     * - Variety (Generated variety id number)
     * - Logical name (Given with MarkVisual()-method) 
     * - Alias (Text that the text visual shows)
     * - Font type (qfn_primary, qfn_secondary...)
     * - Text width (Text width in pixels)
     * - Text height (Text height in pixels)
     * - Visual coordinates relative to parent layout left, right, top and bottom coordinates
     * - Remarks (Free text, if any)
     * - Layout parent id (Generated id for parent layout) 
     * - Layout parent variety id (Generated variety id for parent layout) 
     * - Wheter text visual is a multi row or not 
     * - Max number of text rows 
     * - Font style 1 (plain, bold -attributes)
     * - Font style 2 (regular, italic -attributes)
     * - Font style 3 (underline, strikethrough, outline, boxed or shadow)
     * - Concatenated rows or not
     *
     */
    IMPORT_C void WriteReport();

    /**
     * Enables (or disables) automarking of visuals. If enabled, when visuals are 
     * created they are automatical marked to be included into reports.
     */
    IMPORT_C void EnableAutoMarking(TBool aEnabled=ETrue);

    /**
     * Tells if automatic visual marking is enabled.
     * @retrun ETrue if automatic visual marking is enabled, otherwise EFalse.
     */     
    IMPORT_C TBool AutoMarking();

private:

    CAlfLayoutMetricsUtility();
    
    void ConstructL(CAlfEnv& aEnv);

    void MarkVisualL(TDesC* aLogicalName, CAlfTextVisual* aVisual);

    void MeasureVisualsL();

    void PrintToFile( TDesC16& aBuf );
    
                           
private:

    struct TMetricsPrivateData;
    TMetricsPrivateData* iMetricsData;
    };


#endif // C_ALFLAYOUTMETRICS_H
