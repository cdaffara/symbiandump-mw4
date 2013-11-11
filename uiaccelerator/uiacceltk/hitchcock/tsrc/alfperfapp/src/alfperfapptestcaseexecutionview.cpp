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
* Description:  CAlfPerfAppTestCaseExecutionView implementation.
 *
*/


#include <aknutils.h>
#include <aknviewappui.h>
#include <alfperfapp.rsg>
#include <alf/alfenv.h>
#include <hal.h>
#include "alfperfapptestcaseexecutionview.h"
#include "alfperfappconstants.h"
#include "alfperfapptestcasefactory.h"
#include "alfperfapptestcase.h"
#include "alfperfappmodel.h"

#include "../../alfdebugextension/inc/alfdebug.h"

/**
 * Client-server interface for daemon.
 */
class RAlfPerfAppDaemon : public RApaAppServiceBase
    {
public:
    /**
     * Constructor.
     */
    RAlfPerfAppDaemon();

    /**
     * Connects to daemon.
     * @return error code, KErrNone upon success.
     */
    TInt Connect();

    /**
     * Gets measurements.
     * @param aMeasurements this will contain measurements.
     * @return error code.
     */
    TInt GetMeasurements( TAlfDebugServerMeasurements& aMeasurements );

    /**
     * Enables activity polling. 
     * @param aEnable ETrue if enabled, EFalse if disabled.
     * @return error code.
     */
    TInt EnableActivityPoll( TBool aEnable );

private:
    /**
     * From RApaAppServiceBase, returns service UID.
     * @return service UID.
     */
    virtual TUid ServiceUid() const;
    };

/**
 * CAlfPerfAppTestCaseExecutionView::CContainer container class.
 */
class CAlfPerfAppTestCaseExecutionView::CContainer : public CCoeControl
    {
    public:

        CContainer( CAlfPerfAppTestCaseExecutionView& aView );
        void ConstructL();
        ~CContainer();

        // From base class CCoeControl    
        virtual TKeyResponse OfferKeyEventL( 
                const TKeyEvent& aKeyEvent, TEventCode aType );
        virtual void HandleResourceChange( TInt aType );
        virtual void Draw(const TRect& aRect) const;

    private:
        /**
         * Reference to execution view.
         * Now owned.
         */
        CAlfPerfAppTestCaseExecutionView& iView;
    };

/**
 * Active object to execute test cases.
 */
class CAlfPerfAppTestCaseExecutionView::CActiveExecuter 
: public CActive, private MAlfPerfAppTestCaseInterface
    {
    public:
        CActiveExecuter( 
                CAlfPerfAppTestCaseExecutionView& aView,
                CAlfPerfAppTestCaseFactory& aFactory,
                CAlfPerfAppModel& aModel,
                RAlfPerfAppDaemon& aDaemon,
                TBool aUseContinuousRun = EFalse);
        ~CActiveExecuter();

        /**
         * Starts executing test cases.
         */
        void StartNextCase();

        /**
         * Sets visible area.
         * @param aRect new visible area.
         */ 
        void SetVisibleArea( const TRect& aRect );

        /**
         * Ends the continuous run if it was on, otherwise does nothing.
         */
        void EndContinuousRun();

    private:

        // From base class CActive
        virtual void RunL();
        virtual void DoCancel();
        virtual TInt RunError( TInt aError );

        // From base class MAlfPerfAppTestCaseInterface
        virtual void MeasureNow();

    private:

        /**
         * Gets next test case.
         * @return ETrue if test case received ok.
         */
        TBool GetNextTestCaseL();

        /**
         * Reports error. Calls DoReportErrorL in TRAP harness.
         * @param aErrorCode error code.
         */
        void ReportError( TInt aErrorCode );

        /**
         * Reports error to model.
         * @param aErrorCode error code.
         */
        void DoReportErrorL( TInt aErrorCode );

        /**
         * Reports error and continues to next case.
         * @param aErrorCode error code.
         */
        void ReportErrorAndContinue( TInt aErrorCode );

        /**
         * Resets current state.
         */
        void Reset( TBool aOkToCallObserver );

        /**
         * Completes request status with specified error code.
         * @param aErrorCode error code.
         */
        void CompleteNow( TInt aErrorCode );

        /**
         * Determine next state based on current state.
         * It's expected that current state has been completed
         * successfully.
         */
        void DetermineNextState();

        /**
         * Creates alf environment and initializes
         * debug extension plugin.
         */
        void CreateEnvL();

        /**
         * Deletes alf environment.
         */
        void DeleteEnv();

        /**
         * Performs measurements.
         */
        void PerformMeasurementsL();

    private:

        /**
         * Reference to execution view.
         * Not owned.
         */
        CAlfPerfAppTestCaseExecutionView& iView;

        /**
         * Reference to test case factory.
         * Not owned.
         */
        CAlfPerfAppTestCaseFactory& iFactory;

        /**
         * Reference to model.
         * Not owned.
         */
        CAlfPerfAppModel& iModel;

        /**
         * Reference to daemon.
         * Not owned.
         */
        RAlfPerfAppDaemon& iDaemon;

        /**
         * Visible area rect.
         */
        TRect iVisibleArea;

        /**
         * Current test case id.
         */
        TInt iCaseId;

        /*
         *  Current cases place in the sequence
         */
        TInt iSequenceIndex;

        /**
         * Enumerates different possibilities for current state.
         */
        enum TCurrentState
            {
            EStateIdle,
            EStateCreateTestCase,
            EStateSetupTestCase,
            EStateExecuteTestCase,
            EStateTearDownTestCase
            };

        /**
         * Current state.
         */
        TCurrentState iCurrentState;

        /**
         * Alf environment.
         * Owned.
         */
        CAlfEnv* iEnv;

        /**
         * Current test case under execution.
         * Owned.
         */      
        CAlfPerfAppTestCase* iCurrentTestCase;

        /**
         * NTickCount tick period.
         */
        TInt iTickPeriod;  

        /**
         * Tells if the continuous run is enabled.
         */
        TBool iContinuousRunOngoing;

        /**
         * Tells if this is the last case. Used only to end continuous run correctly.
         */
        TBool iFinalCase;
    };

// ============================ MEMBER FUNCTIONS ===============================

CAlfPerfAppTestCaseExecutionView* CAlfPerfAppTestCaseExecutionView::NewLC(
        CAlfPerfAppModel& aModel )
    {
    CAlfPerfAppTestCaseExecutionView* self = 
    new (ELeave) CAlfPerfAppTestCaseExecutionView( aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CAlfPerfAppTestCaseExecutionView::~CAlfPerfAppTestCaseExecutionView()
    {
    delete iExecuter;
    delete iFactory;

    if ( iDaemon )
        {
        iDaemon->Close();
        delete iDaemon;
        }
    }

void CAlfPerfAppTestCaseExecutionView::NotifyLayoutChange( 
        const TRect& aRect )
    {
    if ( iExecuter )
        {
        iExecuter->SetVisibleArea( aRect );
        }
    }

void CAlfPerfAppTestCaseExecutionView::NotifyExecutionCompletedL()
    {
    AppUi()->ActivateLocalViewL( KAlfPerfAppTestCaseResultsViewId );
    }

void CAlfPerfAppTestCaseExecutionView::NotifyTestCaseVisible( TBool aVisible )
    {
    if ( iContainer )
        {
        if ( iDaemon )
            {
            // Call User::ResetInactivityTime periodically if test case
            // is visible. This ensures that screensaver is not activated
            // and thus test case execution is not disturbed.
            (void)iDaemon->EnableActivityPoll( aVisible );
            }

        iContainer->MakeVisible( !aVisible );
        }
    }

CAlfPerfAppTestCaseExecutionView::CAlfPerfAppTestCaseExecutionView(
        CAlfPerfAppModel& aModel )
: iModel( aModel )
            {
            }

#include <apgcli.h>
#include <apacmdln.h>
#include "alfperfapp.hrh"

        static void StartDaemonL()
            {
            _LIT( KAlfPerfAppDaemonExe, "alfperfappdaemon.exe" );
            const TUid KAlfPerfAppDaemonUid = { KAlfPerfAppDaemonUidValue };

            RApaLsSession apa;
            User::LeaveIfError(apa.Connect());
            CleanupClosePushL(apa);

            CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( KAlfPerfAppDaemonExe );
            cmdLine->SetServerRequiredL( KAlfPerfAppDaemonUid.iUid );
            cmdLine->SetCommandL( EApaCommandBackground );
            TThreadId dummy;

            TRequestStatus requestStatusForRendezvous;
            User::LeaveIfError( apa.StartApp(*cmdLine, dummy, &requestStatusForRendezvous) );
            User::WaitForRequest( requestStatusForRendezvous );
            User::LeaveIfError( requestStatusForRendezvous.Int() );

            CleanupStack::PopAndDestroy(2, &apa);   
            }

        void CAlfPerfAppTestCaseExecutionView::ConstructL()
            {
            BaseConstructL( R_ALFPERFAPP_TEST_CASE_EXECUTION_VIEW );
            iFactory = CAlfPerfAppTestCaseFactory::NewL();

            iDaemon = new (ELeave) RAlfPerfAppDaemon;
            TInt err = iDaemon->Connect();
            if ( err == KErrNotFound )
                {
                TRAP( err, StartDaemonL() );
                if ( err == KErrNone ||
                        err == KErrAlreadyExists )
                    {
                    err = iDaemon->Connect();
                    }
                }
            if ( err != KErrNotFound )
                {
                User::LeaveIfError( err );
                }
            }

        TUid CAlfPerfAppTestCaseExecutionView::Id() const
        {
        return KAlfPerfAppTestCaseExecutionViewId;
        }

        // -----------------------------------------------------------------------------
        // Handles view activation
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseExecutionView::DoActivateL(
                const TVwsViewId& /*aPrevViewId*/,
                TUid /*aCustomMessageId*/,
                const TDesC8& aCustomMessage )
            {
            CContainer* container = new (ELeave) CContainer( *this );
            CleanupStack::PushL( container );
            container->ConstructL();
            AppUi()->AddToStackL( *this, container );
            CleanupStack::Pop( container );
            iContainer = container;

            if(aCustomMessage == _L8("UseContinuousRun"))
                {
                // If we should use the continuous run, use it (the ETrue in the end)
                iExecuter = new (ELeave) CActiveExecuter( *this, *iFactory, iModel, *iDaemon, ETrue );
                Cba()->SetCommandSetL(R_AVKON_SOFTKEYS_OK_EMPTY);
                Cba()->DrawDeferred();
                }
            else
                {
                // If not, then just use the default parameters
                iExecuter = new (ELeave) CActiveExecuter( *this, *iFactory, iModel, *iDaemon);
                Cba()->SetCommandSetL(R_AVKON_SOFTKEYS_EMPTY);
                Cba()->DrawDeferred();

                }
            iExecuter->SetVisibleArea( 
                    TRect( container->Position(), container->Size() ) );
            iExecuter->StartNextCase();
            }

        // -----------------------------------------------------------------------------
        // Handles view deactivation
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseExecutionView::DoDeactivate()
            {
            if ( iContainer )
                {
                AppUi()->RemoveFromStack( iContainer );
                delete iContainer;
                iContainer = NULL;
                }

            delete iExecuter;
            iExecuter = NULL;
            }

        // -----------------------------------------------------------------------------
        // Handles test case selection view specific commands.
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseExecutionView::HandleCommandL( TInt aCommand )
            {
            if(aCommand == EAknSoftkeyOk) // For some reason, switch didn't catch this
                {
                iExecuter->EndContinuousRun();
                return;
                }
            switch ( aCommand )
                {
                default:
                    AppUi()->HandleCommandL( aCommand );
                    break;
                }
            }

        //
        // Implementation of CAlfPerfAppTestCaseExecutionView::CContainer:
        //

        CAlfPerfAppTestCaseExecutionView::CContainer::CContainer(
                CAlfPerfAppTestCaseExecutionView& aView )
        : iView( aView )
            {
            }

                void CAlfPerfAppTestCaseExecutionView::CContainer::ConstructL()
                    {
                    CreateWindowL();

                    TRect rect;
                    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
                    SetRect( rect );

                    ActivateL();
                    }

                CAlfPerfAppTestCaseExecutionView::CContainer::~CContainer()
                    {
                    }



                TKeyResponse CAlfPerfAppTestCaseExecutionView::CContainer::OfferKeyEventL( 
                        const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
                    {
                    TKeyResponse resp = EKeyWasNotConsumed;
                    if ( IsVisible() )
                        {
                        resp = EKeyWasConsumed;
                        }
                    return resp;
                    }

                void CAlfPerfAppTestCaseExecutionView::CContainer::HandleResourceChange( TInt aType )
                    {
                    CCoeControl::HandleResourceChange( aType );

                    if ( aType == KEikDynamicLayoutVariantSwitch )
                        {
                        TRect rect;
                        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
                        SetRect( rect );

                        iView.NotifyLayoutChange( rect );
                        }
                    }

                void CAlfPerfAppTestCaseExecutionView::CContainer::Draw( 
                        const TRect& /*aRect*/ ) const
                        {
                        CWindowGc& gc = SystemGc();
                        gc.SetPenStyle( CGraphicsContext::ENullPen );
                        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
                        gc.SetBrushColor( TRgb( 128, 0, 0 ) ); // red color
                        gc.DrawRect( Rect() );
                        gc.SetBrushStyle( CGraphicsContext::ENullBrush );
                        }

                //    
                // Implementation of CAlfPerfAppTestCaseExecutionView::CActiveExecuter:
                //
                CAlfPerfAppTestCaseExecutionView::CActiveExecuter::CActiveExecuter( 
                        CAlfPerfAppTestCaseExecutionView& aView,
                        CAlfPerfAppTestCaseFactory& aFactory,
                        CAlfPerfAppModel& aModel,
                        RAlfPerfAppDaemon& aDaemon,
                        TBool aUseContinuousRun)
                : CActive( CActive::EPriorityHigh ),
                iView( aView ),
                iFactory( aFactory ),
                iModel( aModel ),
                iDaemon( aDaemon ),
                iCurrentState( EStateIdle ),
                iContinuousRunOngoing( aUseContinuousRun ),
                iFinalCase(EFalse),
                iSequenceIndex(0)
            {
            CActiveScheduler::Add( this );

            TInt err = HAL::Get( HAL::ENanoTickPeriod, iTickPeriod );
            if ( err != KErrNone )
                {
                iTickPeriod = 1000; // assume 1ms
                }
            }

                        CAlfPerfAppTestCaseExecutionView::CActiveExecuter::~CActiveExecuter()
                            {
                            Reset( EFalse ); // calls Cancel()
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::StartNextCase()
                            {
                            Reset( ETrue ); // reset sets current state to idle

                            CompleteNow( KErrNone );
                            SetActive();
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::SetVisibleArea(
                                const TRect& aRect )
                            {
                            iVisibleArea = aRect;

                            if ( iCurrentTestCase )
                                {
                                iCurrentTestCase->HandleVisibleAreaChange( aRect );
                                }
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::EndContinuousRun()
                            {
                            if(iContinuousRunOngoing)
                                {
                                iFinalCase = ETrue;
                                }

                            iContinuousRunOngoing = EFalse;
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::RunL()
                            {
                            const TInt errorCode = iStatus.Int();

                            if ( errorCode != KErrNone )
                                {
                                // If current test step failed, report and continue to next
                                // test case.
                                __ASSERT_ALWAYS( iCurrentState != EStateIdle, User::Invariant() );
                                ReportErrorAndContinue( errorCode );
                                return;
                                }

                            if ( iCurrentState == EStateIdle )
                                {
                                // When current state is idle and RunL is reached,
                                // it's time to take next case from queue and start
                                // execution.
                                if ( iFinalCase || !GetNextTestCaseL())
                                    {
                                    // No more test cases - stop.
                                    iView.NotifyExecutionCompletedL();
                                    return;
                                    }
                                }

                            if(iCurrentTestCase && iContinuousRunOngoing && iCurrentState == EStateExecuteTestCase)
                                {
                                TInt CaseID = iCurrentTestCase->CaseID();
                                iModel.AddToExecuteArrayL(CaseID);
                                }

                            // Perform measurements before switching to next state
                            switch ( iCurrentState )
                                {
                                case EStateSetupTestCase:
                                case EStateExecuteTestCase:
                                    // Going to execution or teardown phase.
                                    PerformMeasurementsL();
                                    break;           
                                case EStateCreateTestCase:
                                case EStateTearDownTestCase:
                                case EStateIdle:
                                default:
                                    // Either these do not contain any activity or
                                    // then it's performed synchronously.
                                    break;
                                }

                            DetermineNextState();

                            // Execute according to current state
                            switch ( iCurrentState )
                                {   
                                case EStateCreateTestCase:
                                    __ASSERT_ALWAYS( !iCurrentTestCase, User::Invariant() );
                                    iCurrentTestCase = iFactory.CreateL( iCaseId, iSequenceIndex);

                                    iStatus = KRequestPending;
                                    CompleteNow( KErrNone );
                                    SetActive();
                                    break;

                                case EStateSetupTestCase:
                                    __ASSERT_ALWAYS( iCurrentTestCase, User::Invariant() );
                                    // Make sure test case is visible.
                                    // NotifyTestCaseVisible is called with EFalse in Reset().
                                    iView.NotifyTestCaseVisible( ETrue );

                                    PerformMeasurementsL();
                                    CreateEnvL();

                                    iStatus = KRequestPending;
                                    iCurrentTestCase->SetInterface( this );
                                    iCurrentTestCase->SetupL( *iEnv, iVisibleArea, iStatus );
                                    SetActive();
                                    break;

                                case EStateExecuteTestCase:
                                    __ASSERT_ALWAYS( iCurrentTestCase, User::Invariant() );
                                    PerformMeasurementsL();

                                    iStatus = KRequestPending;
                                    iCurrentTestCase->ExecuteL( iStatus );
                                    SetActive();
                                    break;

                                case EStateTearDownTestCase:
                                    __ASSERT_ALWAYS( iCurrentTestCase, User::Invariant() );
                                    PerformMeasurementsL();

                                    iCurrentTestCase->TearDown();

                                    PerformMeasurementsL();

                                    // Perform some reset actions. Cancel() is not
                                    // needed, because we are in RunL method.
                                    iCurrentTestCase->SetInterface( NULL );
                                    delete iCurrentTestCase;
                                    iCurrentTestCase = NULL;
                                    DeleteEnv();


                                    Reset( ETrue );

                                    iStatus = KRequestPending;
                                    CompleteNow( KErrNone );
                                    SetActive();
                                    break;

                                case EStateIdle:
                                    break;

                                default:
                                    break;
                                }
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::DoCancel()
                            {
                            delete iCurrentTestCase;
                            iCurrentTestCase = NULL;
                            }

                        TInt CAlfPerfAppTestCaseExecutionView::CActiveExecuter::RunError( 
                                TInt aError )
                            {
                            if ( iCurrentState != EStateIdle )
                                {
                                if(iCurrentTestCase)
                                    {
                                    iCurrentTestCase->TearDown();
                                    iCurrentTestCase->SetInterface( NULL );
                                    }
                                ReportErrorAndContinue( aError );
                                }

                            return KErrNone;
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::MeasureNow()
                            {
                            TRAP_IGNORE( PerformMeasurementsL() );
                            }

                        TBool CAlfPerfAppTestCaseExecutionView::CActiveExecuter::GetNextTestCaseL()
                            {
                            TBool casesLeft = iModel.GetFromExecuteArray( iCaseId );

                            if(casesLeft)
                                {
                                if(iCaseId < 0)
                                    {
                                    iSequenceIndex++;
                                    }
                                else
                                    {
                                    iSequenceIndex = 1;
                                    }
                                }
                            
                            return casesLeft;
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::ReportError( 
                                TInt aErrorCode )
                            {
                            TRAP_IGNORE( DoReportErrorL( aErrorCode ) );
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::DoReportErrorL( 
                                TInt aErrorCode )
                            {
                            _LIT( KAlfPerfAppErrorMessage, "0x%x failed with %d (state:%d)" );
                            HBufC* messageBuffer = HBufC::NewLC( 128 );

                            messageBuffer->Des().Format(
                                    KAlfPerfAppErrorMessage,
                                    iCaseId,
                                    aErrorCode,
                                    iCurrentState );

                            iModel.AddToErrorArrayL( *messageBuffer );    
                            CleanupStack::PopAndDestroy( messageBuffer );
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::ReportErrorAndContinue( 
                                TInt aErrorCode )
                            {
                            ReportError( aErrorCode );
                            StartNextCase();
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::Reset(
                                TBool aOkToCallObserver )
                            {
                            Cancel();

                            delete iCurrentTestCase;
                            iCurrentTestCase = NULL;

                            DeleteEnv();

                            iCaseId = 0;
                            iCurrentState = EStateIdle;

                            if ( aOkToCallObserver )
                                {
                                iView.NotifyTestCaseVisible( EFalse );
                                }
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::CompleteNow( 
                                TInt aErrorCode )
                            {
                            TRequestStatus* status = &iStatus;
                            User::RequestComplete( status, aErrorCode );   
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::DetermineNextState()
                            {
                            // Determine next state after step has been completed.
                            switch ( iCurrentState )
                                {
                                case EStateIdle:
                                    iCurrentState = EStateCreateTestCase;
                                    break;

                                case EStateCreateTestCase:
                                    iCurrentState = EStateSetupTestCase;
                                    break;

                                case EStateSetupTestCase:
                                    iCurrentState = EStateExecuteTestCase;
                                    break;

                                case EStateExecuteTestCase:
                                    iCurrentState = EStateTearDownTestCase;
                                    break;

                                case EStateTearDownTestCase:
                                    iCurrentState = EStateIdle;
                                    break;

                                default:
                                    break;
                                }
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::CreateEnvL()
                            {
                            __ASSERT_ALWAYS( !iEnv, User::Invariant() );
                            iEnv = CAlfEnv::NewL();
                            iEnv->SetMaxFrameRate(100);
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::DeleteEnv()
                            {
                            delete iEnv;
                            iEnv = NULL;
                            }

                        void CAlfPerfAppTestCaseExecutionView::CActiveExecuter::PerformMeasurementsL()
                            {
                            CAlfPerfAppModel::TTestCaseResultItem item;

                            // Test case identifiers
                            item.iCaseId = iCaseId;

                            // Phase (0 = setup, 1 = execution, 2 = teardown)
                            switch ( iCurrentState )
                                {
                                case EStateIdle:
                                case EStateCreateTestCase:
                                case EStateSetupTestCase:
                                    item.iPhase = 0;
                                    break;

                                case EStateExecuteTestCase:
                                    item.iPhase = 1;
                                    break;

                                case EStateTearDownTestCase:
                                    item.iPhase = 2;
                                    break;

                                default:
                                    item.iPhase = -1;
                                    break;
                                }

                            TInt dummy; // not used.

                            // Amount of cells allocated in Application heap.
                            // Total size taken by allocated cells in Application heap.
                            // Amount of free space in Application heap.
                            item.iAppCells = User::AllocSize( item.iAppMemory );
                            item.iAppFree = User::Available( dummy );

                            // Perform server side measurements    
                            TAlfDebugServerMeasurements serverMeasurements;
                            TBool measureOk = 
                            ( iDaemon.GetMeasurements( serverMeasurements ) == KErrNone );

                            if ( !measureOk )
                                {
                                serverMeasurements.iTimeStamp = User::NTickCount();
                                serverMeasurements.iServerCells = 0;
                                serverMeasurements.iServerMemory = 0;
                                serverMeasurements.iServerFree = 0;
                                serverMeasurements.iFrameCount = 0;
                                }

                            // Convert timestamps to ms
                            if ( iTickPeriod != 1000 )
                                {
                                TUint64 stamp = serverMeasurements.iTimeStamp;
                                stamp *= iTickPeriod;
                                stamp /= 1000;
                                stamp &= 0xFFFFFFFF; // modulo 2**32
                                serverMeasurements.iTimeStamp = I64LOW( stamp );
                                }


                            // Amount of cells allocated in Server heap.
                            // Total size taken by allocated cells in Server heap.
                            // Amount of free space in Server heap.
                            // Time stamp (preferably from server side)
                            // Frame count
                            item.iTimeStamp = serverMeasurements.iTimeStamp;
                            item.iServerCells = serverMeasurements.iServerCells;
                            item.iServerMemory = serverMeasurements.iServerMemory;
                            item.iServerFree = serverMeasurements.iServerFree;
                            item.iFrameCount = serverMeasurements.iFrameCount;

                            // Amount of free memory in the system.
                            // Memory consumption of the system.
                            TInt systemFreeRam = 0;
                            HAL::Get( HALData::EMemoryRAMFree, systemFreeRam );
                            TInt systemTotalRam = 0;
                            HAL::Get( HALData::EMemoryRAM, systemTotalRam );
                            item.iSystemMemory = systemTotalRam - systemFreeRam;
                            item.iSystemFree = systemFreeRam;

                            TTestCaseSpecificResultText specificResult = 0;
                            specificResult = iCurrentTestCase->getCaseSpecificResultL(); 
                            if(specificResult)
                                {
                                item.specificResult8.Copy(specificResult->Des());
                                delete specificResult;
                                }

                            // Append to model
                            iModel.AddToResultFilesL( item );        
                            }

                        //
                        // Implementation of RAlfPerfAppDaemon
                        //

                        RAlfPerfAppDaemon::RAlfPerfAppDaemon()
                            {
                            }

                        TInt RAlfPerfAppDaemon::Connect()
                            {
                            TInt result = KErrNone;
                            if ( !Handle() )
                                {
                                _LIT(KServerNameFormat, "%08x_%08x_AppServer");        
                                TFullName serverName;
                                serverName.Format(
                                        KServerNameFormat, 
                                        KAlfPerfAppDaemonUidValue, 
                                        KAlfPerfAppDaemonUidValue );
                                TRAP( result, ConnectExistingByNameL( serverName ) );
                                }
                            return result;
                            }

                        TInt RAlfPerfAppDaemon::GetMeasurements( 
                                TAlfDebugServerMeasurements& aMeasurements )
                            {
                            if ( !Handle() )
                                {
                                return KErrNotSupported;
                                }

                            TPckg<TAlfDebugServerMeasurements> pckg( aMeasurements );
                            return SendReceive( EAlfPerfAppIpcGetMeasurements, TIpcArgs( &pckg ) );
                            }

                        TInt RAlfPerfAppDaemon::EnableActivityPoll( TBool aEnable )
                            {
                            if ( !Handle() )
                                {
                                return KErrNotSupported;
                                }

                            return SendReceive( EAlfPerfAppIpcEnableActivityPoll, TIpcArgs( aEnable ? 1 : 0 ) );
                            }

                        TUid RAlfPerfAppDaemon::ServiceUid() const
                        {
                        return TUid::Uid( KAlfPerfAppDaemonUidValue );
                        }

