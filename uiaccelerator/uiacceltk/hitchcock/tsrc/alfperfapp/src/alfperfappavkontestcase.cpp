/*
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
* Description:  CAlfPerfAppAvkonTestCase implementation.
*
*/


#include "alfperfappbasetestcasecontrol.h"
#include "alfperfappavkontestcase.h"
#include "alfperfapp.hrh"

#include <aknutils.h>
#include <e32math.h>

#include <coemain.h>
#include <coecntrl.h>
#include <eikapp.h>

#include <alf/alfenv.h>
#include <alf/alfevent.h>
#include <alf/alfdisplay.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfroster.h>

/**
 * Control group for Avkon test cases.
 */
const TInt KAlfPerfAppAvkonControlGroup = 1;

/**
 * Start next 'cycle' command.
 */
const TInt KAlfPerfAppAvkonCmdNext = 0x5001;

/**
 * Delay between frames
 */
const TInt KAnimSleepTimeUs = 0;

/**
 * Cycle time, contais several frames
 */
const TInt KCycleDurationMs = 1000;

/**
 * The whole test case duration
 */
const TInt KCaseDurationMs = KCycleDurationMs * 10;

/**
 * Avkon test coe control
 */

class CAvkonTestCoeControl: public CCoeControl
{
public:
    // Construction & destruction
    CAvkonTestCoeControl();
    ~CAvkonTestCoeControl();
    void ConstructL(const TRect& aRect);
    RWindow* GetWindow();

private:

    // from CCoeControl
    void Draw(const TRect&) const;
    TInt CountComponentControls() const {return 0;}
    CCoeControl* ComponentControl( TInt /*aIndex*/ ) const {return NULL;}

public: 
    
};

// ============================ MEMBER FUNCTIONS ===============================

CAvkonTestCoeControl::CAvkonTestCoeControl()
    {
    }


void CAvkonTestCoeControl::ConstructL(const TRect& aRect)
    {
    // Create window
    CreateWindowL();
    SetRect(aRect);
    ActivateL();
    }

CAvkonTestCoeControl::~CAvkonTestCoeControl()
    {
    }

RWindow* CAvkonTestCoeControl::GetWindow()
    {
    return &Window();   
    }


void CAvkonTestCoeControl::Draw(const TRect& /*aRect*/) const
    {
    }

/**
 * Basic WindGC tests.
 */
class CAlfPerfAppAvkonTestCaseBasic : public CAlfPerfAppBaseTestCaseControl
    {
public:
    CAlfPerfAppAvkonTestCaseBasic();
    ~CAlfPerfAppAvkonTestCaseBasic();
    
    virtual void ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea );      
    virtual void DoStartExecuteL();
    virtual TBool OfferEventL( const TAlfEvent& aEvent );
    virtual void SetVisibleArea( const TRect& aVisibleArea );    

    /**
     * animation timer callback
     */
    static TInt AnimTimerCbL(TAny* aThis);
   
    /**
     * Calculate fps for the current phase
     */
    double CalcPhaseFpsL();   
    
private:
    
    /**
     * Next cycle.
     */
    void NextCycleL();
    
    /**
     * Next frame inside cycle
     */
    void NextAnimFrameL();

private:
       
    CAvkonTestCoeControl* iAvkonControl; // Owned
    TRect iWinRect;
 
private:

   /**
     * Cycle counter.
     */        
    TInt iCycleCounter;
    
    /**
     * animation timer
     */        
   CPeriodic* iAnimTimer;
    
   /**
    * Current frame number
    */        
    TInt iAnimFrameNum;
    
    /**
     * 
     */        
    TTime iCycleStartTime;

    /**
     * Font
     */        
    CFont* iFont;

    /**
     * Test bitmap and mask 
     */        
    CFbsBitmap* iPictureBm;
    CFbsBitmap* iMaskBm;

    /**
     * Client performace data
     */        
   TTime iTestCaseStartTime_ys;
   TUint iTestCaseFrameCount;
};
        
        
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Checks if specified case is supported by this class.
// -----------------------------------------------------------------------------
//
TBool CAlfPerfAppAvkonTestCase::IsSupported( TInt aCaseId )
    {
    return aCaseId > EAlfPerfAppAvkonMin && 
           aCaseId < EAlfPerfAppAvkonMax;
    }

CAlfPerfAppAvkonTestCase* CAlfPerfAppAvkonTestCase::NewL( TInt aCaseId, TInt aSequenceIndex )
    {
    if ( !IsSupported( aCaseId ) )
        {
        User::Leave( KErrNotSupported );
        }

    CAlfPerfAppAvkonTestCase* self = 
        new (ELeave) CAlfPerfAppAvkonTestCase( aCaseId, aSequenceIndex );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAlfPerfAppAvkonTestCase::~CAlfPerfAppAvkonTestCase()
    {
    if ( iEnv && iControl )
        {
        iControl->CancelExecution();
        iEnv->DeleteControlGroup( KAlfPerfAppAvkonControlGroup );
        }
    }

// -----------------------------------------------------------------------------
// Starts setup phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppAvkonTestCase::SetupL( 
        CAlfEnv& aEnv, const TRect& /*aVisibleArea*/, TRequestStatus& aStatus )
    {
    // Note: we could remove usage of Alf client API as it is not mandatory for the test case
    
    iEnv = &aEnv;

    CAlfDisplay* display = 0;
    CAlfControlGroup& group = iEnv->NewControlGroupL( KAlfPerfAppAvkonControlGroup );
    CAlfPerfAppBaseTestCaseControl* control = 0;
    
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EApplicationWindow,
        rect );
    display = &iEnv->NewDisplayL( rect, CAlfEnv::ENewDisplayAsCoeControl);
    display->SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );

    switch ( iCaseId )
        {
        case EAlfPerfAppAvkonBasicWinGc:
            control = new (ELeave) CAlfPerfAppAvkonTestCaseBasic;
            CleanupStack::PushL( control );
            control->ConstructL( *iEnv, iCaseId, rect );
            break;
            
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    group.AppendL( control ); // ownership passed to control group.
    iControl = control;
    CleanupStack::Pop( control );    
    
    display->Roster().ShowL( group );
    
    // This must be the last statement.
    CAlfPerfAppTestCase::CompleteNow( aStatus, KErrNone );
    }

// -----------------------------------------------------------------------------
// Starts execution phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppAvkonTestCase::ExecuteL( TRequestStatus& aStatus )
    {
    // SetupL must have been called first.
    __ASSERT_ALWAYS( iEnv, User::Invariant() );
    __ASSERT_ALWAYS( iControl, User::Invariant() );
    
    iControl->StartExecuteL( aStatus );
    }

// -----------------------------------------------------------------------------
// Tears down.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppAvkonTestCase::TearDown()
    {
    // Execution side will delete CAlfEnv, so it will delete everything
    // related to this.
    iEnv = NULL;
    iControl = NULL;
    }

void CAlfPerfAppAvkonTestCase::HandleVisibleAreaChange( const TRect& /*aRect*/ )
    {
    }

TInt CAlfPerfAppAvkonTestCase::CaseID()
    {
    return iCaseId;
    }

CAlfPerfAppAvkonTestCase::CAlfPerfAppAvkonTestCase( TInt aCaseId, TInt aSequenceIndex )
    : CAlfPerfAppTestCase(aSequenceIndex), iCaseId ( aCaseId )
    {
    }

void CAlfPerfAppAvkonTestCase::ConstructL()
    {
    }

TTestCaseSpecificResultText CAlfPerfAppAvkonTestCase::getCaseSpecificResultL()
    {
    TTestCaseSpecificResultText result = HBufC::NewL(KAlfPerfAppMaxCharsInSpecificResultText);
    double fps = 0.0;
    if (iControl)
        {
        fps = ((CAlfPerfAppAvkonTestCaseBasic*)iControl)->CalcPhaseFpsL();
        }
    result->Des().AppendFormat(_L("Client fps: %3.2f"), fps);
    return result;        
    }

// ********

// Implementation of CAlfPerfAppAvkonTestCaseBasic:
CAlfPerfAppAvkonTestCaseBasic::CAlfPerfAppAvkonTestCaseBasic()
    {
    }

CAlfPerfAppAvkonTestCaseBasic::~CAlfPerfAppAvkonTestCaseBasic()
    {
    delete iPictureBm;
    delete iMaskBm;
    
   if (iFont)
        {
        CCoeEnv::Static()->ScreenDevice()->ReleaseFont(iFont);
        }
    delete iAnimTimer;
    delete iAvkonControl;    
    }

void CAlfPerfAppAvkonTestCaseBasic::ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea )
    {
    CAlfPerfAppBaseTestCaseControl::ConstructL( aEnv, aCaseId, aVisibleArea );   
    
    iWinRect = aVisibleArea;
    
    iAvkonControl = new(ELeave) CAvkonTestCoeControl();
    iAvkonControl->ConstructL(iWinRect);

    iAnimTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    
    TFontSpec myFontSpec(_L("Arial"), 3*120);
    CCoeEnv::Static()->ScreenDevice()->GetNearestFontInTwips(iFont, myFontSpec);
    
    // Find my private path
    TFileName pathWithoutDrive;
    TFileName driveAndPath;
    CEikonEnv::Static()->FsSession().PrivatePath( pathWithoutDrive );
    driveAndPath.Copy(CEikonEnv::Static()->EikAppUi()->Application()->AppFullName().Left(2));
    driveAndPath.Append(pathWithoutDrive);
    
    // Create pictures
    iPictureBm = new(ELeave) CFbsBitmap;
    driveAndPath.Append(_L("alfperfapp_test1.mbm"));
    User::LeaveIfError(iPictureBm->Load(driveAndPath));
    iMaskBm = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iMaskBm->Create(iPictureBm->SizeInPixels(), EGray256));
 
    iTestCaseStartTime_ys.UniversalTime();
    iTestCaseFrameCount = 0;
   }
    

void CAlfPerfAppAvkonTestCaseBasic::DoStartExecuteL()
    {
    iTestCaseStartTime_ys.UniversalTime();
    iTestCaseFrameCount = 0;
    
    NextCycleL();
    
    CompleteAfterL( KCaseDurationMs );
    }

void CAlfPerfAppAvkonTestCaseBasic::SetVisibleArea( 
        const TRect& /*aVisibleArea*/ )
    {
    if ( IsExecutionOngoing() )
        {
        Env().CancelCustomCommands( this, KAlfPerfAppAvkonCmdNext );
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        }
    }

TBool CAlfPerfAppAvkonTestCaseBasic::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsCustomEvent() && 
         aEvent.CustomParameter() == KAlfPerfAppAvkonCmdNext )
        {
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        return ETrue;
        }    
    return CAlfPerfAppBaseTestCaseControl::OfferEventL( aEvent );
    }

void CAlfPerfAppAvkonTestCaseBasic::NextCycleL()
    {
    TAlfCustomEventCommand command( KAlfPerfAppAvkonCmdNext, this );
    User::LeaveIfError( Env().Send( command, KCycleDurationMs ) );
    
    iCycleCounter++;
   
    iAnimTimer->Cancel();
    iAnimTimer->Start(KAnimSleepTimeUs, KAnimSleepTimeUs, TCallBack(AnimTimerCbL, this));
    iAnimFrameNum = 0;
    iCycleStartTime.UniversalTime();
    }


TInt CAlfPerfAppAvkonTestCaseBasic::AnimTimerCbL(TAny* aThis)
        {
        static_cast<CAlfPerfAppAvkonTestCaseBasic*>(aThis)->NextAnimFrameL();
        return NULL;
        }

void CAlfPerfAppAvkonTestCaseBasic::NextAnimFrameL()
    {
    // Begin drawing
    RWindow& window = static_cast< RWindow& >( *iAvkonControl->DrawableWindow() );
    CWindowGc& gc = iAvkonControl->SystemGc();
    TRect updateRect(iAvkonControl->Rect());
    window.Invalidate( updateRect );
    window.BeginRedraw( updateRect );
    gc.Activate(window);

    // Draw background
    TRgb color (KRgbWhite);
    gc.SetBrushColor(color);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
    gc.SetPenSize(TSize(10,10));
    gc.SetPenColor(color);    
    gc.DrawRect(updateRect);  
       
    // Calc timeline
    TTime now;
    now.UniversalTime();
    TUint millisecondFromCycleStart = now.MicroSecondsFrom(iCycleStartTime).Int64() / 1000;
    double timelinePercentage = (double)millisecondFromCycleStart / KCycleDurationMs; 
    timelinePercentage *= 0.5; // scale it a bit...
   
    // Calc rect
    TRect fullRect(updateRect);
    TSize size(fullRect.Width()*(1.0 - timelinePercentage), fullRect.Height()*(1.0 - timelinePercentage));
    TPoint windowCenter = fullRect.Center();
    TPoint tl(windowCenter.iX - size.iWidth/2, windowCenter.iY - size.iHeight/2);
    TRect rect(tl, size);
        
    // Draw
    gc.SetPenColor(KRgbBlue);
    gc.SetBrushColor(KRgbRed);
    const TPoint Point1(rect.iTl);
    const TPoint Point2(rect.iBr.iX, rect.iTl.iY);
    const TPoint Point3(rect.iBr);
    const TPoint Point4(rect.iTl.iX, rect.iBr.iY);
    const TPoint center(rect.Center());

    switch (iCycleCounter)
        {
        case 1: // DrawEllipse
            {
            gc.DrawEllipse(rect);   
            break;
            }
        case 2: // DrawRect
            {
            gc.DrawRect(rect);    
            break;
            }
        case 3: // DrawRoundRect
            {
            TSize corner(rect.Width()/5, rect.Height()/5);
            gc.DrawRoundRect(rect, corner);   
            break;
            }
        case 4: // Draw lines
            {
            gc.SetPenColor(TRgb(255,0,0));
            gc.DrawLine(Point1, Point2);       
            
            gc.SetPenColor(TRgb(200,50,0));
            gc.DrawLineTo(Point3);
            
            gc.SetPenColor(TRgb(150,100,0));
            gc.DrawLineTo(Point4);
            
            gc.SetPenColor(TRgb(100,150,0));
            gc.DrawLineBy(TPoint(0, -rect.Height()));
            
            gc.SetPenColor(TRgb(50,200,0));
            gc.MoveTo(Point2);
            gc.DrawLineTo(Point4);
                       
            gc.SetPenColor(TRgb(0,255,0));
            gc.MoveBy(TPoint(0, -rect.Height()));
            gc.DrawLineTo(Point3);
            
            gc.SetPenColor(TRgb(255,0,0));
            gc.Plot(center);
            
            break;
           }
            
        case 5: // Draw 
            {
            gc.SetPenColor(TRgb(255,0,0));
            gc.DrawArc(rect, Point2, Point1);
            gc.DrawPie(rect, Point4, Point3);
            break;
            }
            
        case 6: // Draw polygons
            {
            const TInt KNumPoints = 9;
            TPoint pointList[KNumPoints];
            pointList[0] = TPoint(Point1.iX+rect.Width()*0.25, Point1.iY);
            pointList[1] = TPoint(Point1.iX+rect.Width()*0.75, Point1.iY);
            pointList[2] = TPoint(Point2.iX, Point2.iY+rect.Height()*0.25);
            pointList[3] = TPoint(Point2.iX, Point2.iY+rect.Height()*0.75);
            pointList[4] = TPoint(Point3.iX-rect.Width()*0.25, Point3.iY);
            pointList[5] = TPoint(Point3.iX-rect.Width()*0.75, Point3.iY);
            pointList[6] = TPoint(Point4.iX, Point4.iY-rect.Height()*0.25);
            pointList[7] = TPoint(Point4.iX, Point4.iY-rect.Height()*0.75);
            pointList[8] = TPoint(Point1.iX+rect.Width()*0.25, Point1.iY);
            
            CArrayFix<TPoint>* mypoints = new CArrayFixFlat<TPoint>(KNumPoints);
            CleanupStack::PushL(mypoints);
            for(TInt i=0; i<KNumPoints; i++)
                {
                mypoints->AppendL(pointList[i]);
                }

            gc.SetPenColor(TRgb(255,0,0));
            gc.SetPenSize(TSize(20,20));
            gc.DrawPolyLine(mypoints);
            
            gc.SetPenColor(TRgb(0,255,0));
            gc.SetPenSize(TSize(15,15));
            gc.DrawPolyLine(pointList, KNumPoints);
            
            gc.SetPenColor(TRgb(255,255,0));
            gc.SetPenSize(TSize(10,10));
            gc.DrawPolygon(mypoints);
            
            gc.SetPenColor(TRgb(0,0,255));
            gc.SetPenSize(TSize(5,5));
            gc.DrawPolygon(pointList, KNumPoints);
            
            CleanupStack::PopAndDestroy(); // mypoints           
            break;
            }
            
        case 7: // Draw texts
            {
            gc.UseFont(iFont);
            gc.SetDrawMode(CGraphicsContext::EDrawModePEN);
            gc.SetPenStyle(CGraphicsContext::ESolidPen);
            gc.SetBrushStyle(CGraphicsContext::ESolidBrush);    
            
            TInt h = rect.Height() / 3;
            TInt y = rect.iTl.iY;
            TRect tinyBox(rect);
            tinyBox.SetHeight(h);
            TInt fontDescent=iFont->DescentInPixels();
            gc.SetBrushColor(TRgb(0, 0, 255)); // blue
            
            gc.SetPenColor(TRgb(0,255,0)); // green
            gc.DrawText(_L("Ilves"), tinyBox.iTl+TPoint(0, fontDescent));
            
            tinyBox.Move(0,h);
            TInt posY = tinyBox.Height()-fontDescent;
            gc.SetPenColor(TRgb(255,0,0)); 
            gc.DrawText(_L("Tappara"), tinyBox, posY);
            
            gc.SetPenColor(TRgb(0,255,0)); // green
            gc.DrawTextVertical(_L("Ilves"), tinyBox.iTl+TPoint(fontDescent, 0 ), ETrue);
            
            tinyBox.Move(0,h);
            posY = tinyBox.Height()-fontDescent;
            gc.SetPenColor(TRgb(255,0,0)); 
            gc.DrawTextVertical(_L("Tappara"), tinyBox, posY, ETrue);
            
            break;
            }

        case 8: // Draw bitmaps
            {
            TPoint pos(rect.iTl);
            gc.BitBlt(pos, iPictureBm);
            
            pos = TPoint(rect.iTl.iX + rect.Width()/3, rect.iTl.iY);
            gc.BitBlt(pos, iPictureBm, TRect(iPictureBm->SizeInPixels()));

            pos = TPoint(rect.iTl.iX + rect.Width()*2/3, rect.iTl.iY);
            gc.BitBltMasked(pos, iPictureBm, TRect(iPictureBm->SizeInPixels()), iMaskBm, EFalse);

            pos = TPoint(rect.iTl.iX, rect.iTl.iY+ rect.Height()/3);
            TRect dstRect(pos, TSize(rect.Width()/3, rect.Height()/3));
            gc.DrawBitmap(dstRect, iPictureBm, TRect(iPictureBm->SizeInPixels()));

            pos = TPoint(rect.iTl.iX + rect.Width()/3, rect.iTl.iY+ rect.Height()/3);
            dstRect =  TRect(pos, dstRect.Size());
            gc.DrawBitmap(dstRect, iPictureBm);

            pos = TPoint(rect.iTl.iX, rect.iTl.iY+ rect.Height()*2/3);
            gc.DrawBitmap(pos, iPictureBm);

            pos = TPoint(rect.iTl.iX + rect.Width()/3, rect.iTl.iY+ rect.Height()*2/3);
            dstRect =  TRect(pos, dstRect.Size());
            gc.DrawBitmapMasked(dstRect, iPictureBm, TRect(iPictureBm->SizeInPixels()), iMaskBm, EFalse);

            break;
            }
            
         case 9: // Miscellanious
            {
            TRect rect1(rect);
            rect1.SetWidth(rect.Width()/2);
            rect1.SetHeight(rect.Height()/2);
            TRect rect2(rect1);
            rect2.Move(rect1.Width(),0);
            TRect rect3(rect1);
            rect3.Move(0, rect1.Height());
            TRect rect4(rect1);
            rect4.Move(rect1.Width(), rect1.Height());
            
            // Clear
            gc.Clear();
            // Brush pattern
            gc.UseBrushPattern(iPictureBm);
            gc.SetBrushStyle(CGraphicsContext::EPatternedBrush);
            gc.DrawRect(rect1);
            gc.DiscardBrushPattern();
            // Fading & copy rect
            gc.SetFaded(ETrue);
            gc.CopyRect(rect2.iTl, rect1);
            gc.SetFadingParameters(255,0);
            gc.CopyRect(rect3.iTl, rect1);
            // Map colors  
            gc.SetPenColor(KRgbBlue);
            gc.SetBrushColor(KRgbRed);
            gc.DrawRect(rect4);   
            TRgb colors[2] = {KRgbRed, KRgbGreen}; // change brush color
            gc.MapColors(rect4,colors,1,ETrue);
             
            break;
            }
            
     default:
           gc.DrawRect(rect);          
       }

    // End drawing
    gc.Deactivate();
    window.EndRedraw();
 
    iTestCaseFrameCount++;
    }


double CAlfPerfAppAvkonTestCaseBasic::CalcPhaseFpsL()
    {
    // Calc time spend in the phase
    TTime now;
    now.UniversalTime();
    TUint millisecondsFromCaseStart = now.MicroSecondsFrom(iTestCaseStartTime_ys).Int64() / 1000;
    double fps = 0.0;
    if (millisecondsFromCaseStart)
        {
        fps = (double)iTestCaseFrameCount * 1000 / millisecondsFromCaseStart;       
        }
    iTestCaseStartTime_ys.UniversalTime();
    iTestCaseFrameCount = 0;
    
    return fps;
    }

