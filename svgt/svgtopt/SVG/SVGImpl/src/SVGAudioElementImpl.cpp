/*
* Copyright (c) 2005 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation source file
 *
*/



#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include <AudioPreference.h> // For priority/pref values

#include "SVGAudioElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGEngineImpl.h"
#include "SVGSchemaData.h"
#include "SVGTimeContainer.h"

_LIT(KAudioAMR, "audio/amr");
_LIT(KAudioAWB, "audio/amr-wb");
_LIT(KAudioMP3, "audio/mpeg");
_LIT(KAudioMP4, "audio/mp4");
_LIT(KAudio3GP, "audio/3gp");
_LIT(KAudio3G2, "audio/3g2");
_LIT(KAudioAAC, "audio/aac");
_LIT(KAudioMID, "audio/midi");
_LIT(KAudioMID2, "audio/sp-midi");
_LIT(KAudioRMF, "audio/rmf");
_LIT(KAudioRMF2, "audio/x-rmf");
_LIT(KAudioRMF3, "audio/x-beatnik-rmf");
_LIT(KAudioMXMF, "audio/mobile-xmf");
_LIT(KAudioWMA, "audio/x-ms-wma");
_LIT(KAttrAudioType,  "type" );
_LIT(KXlinkHref,  "xlink:href" );
_LIT(KDur,"dur");
_LIT(KMedia,"media");
_LIT(KAudioLevelStr,  "audio-level");
_LIT(KVolumeStr,  "volume");

// audio-level property by default is  100%
const TInt KAudioLevelDefaultPercent = 100;
const  TInt KMillisecondsPerMicrosecond = 1000;
// ---------------------------------------------------------------------------
// Create new element of audioelementimpl
// ---------------------------------------------------------------------------
CSvgAudioElementImpl* CSvgAudioElementImpl::NewL(  const TUint8 aElemID,
                                                  CSvgDocumentImpl* aDoc )
    {
    CSvgAudioElementImpl*   self    = new ( ELeave )
                                      CSvgAudioElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID, aDoc );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Create new element of audioelementimpl
// ---------------------------------------------------------------------------
CSvgAudioElementImpl* CSvgAudioElementImpl::NewLC(  const TUint8 aElemID,
                                                   CSvgDocumentImpl* aDoc )
    {
    CSvgAudioElementImpl*   self    = new ( ELeave )
                                      CSvgAudioElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID,aDoc );
    return self;
    }

// ---------------------------------------------------------------------------
// Create new element of audioelementimpl
// ---------------------------------------------------------------------------
void CSvgAudioElementImpl::ConstructL(  const TUint8 aElemID,
                                       CSvgDocumentImpl* /* aDoc */ )
    {
    iPercentLevelVolume = KAudioLevelDefaultPercent;
	CSvgMediaElementBase::ConstructL( aElemID );
	iStoredPos = 0;
    }

// ---------------------------------------------------------------------------
// Destructor for Audioelement
// ---------------------------------------------------------------------------
CSvgAudioElementImpl::~CSvgAudioElementImpl()
    {
	if ( iAudioPlayer )
        {
        iAudioPlayer->Stop();
   	    delete iAudioPlayer;
   	    iAudioPlayer = NULL;
        }
    if( iUri)
        {
        delete iUri;
        iUri= NULL;
        }
    }

// ---------------------------------------------------------------------------
// Sets attributes of audio element
// ---------------------------------------------------------------------------
TInt CSvgAudioElementImpl::SetAttributeL( const TDesC& aName,
                                          const TDesC& aValue )
    {
    if (!iTargetSet)
        {
	    iTargetElement = ( CSvgElementImpl * ) ParentNode();// default is parent element
		if (iTargetElement)
			{
    	    iTargetSet = ETrue;
			}
        }

    if(aName == KDur && aValue != KMedia)
        {
    	iDurMedia = EFalse;
    	
        }
    
    if( aName == KXlinkHref)
        {
        this->SetXlinkAttributeL(aName,aValue);
        iUri = aValue.AllocL();        
        }
    if ( aName == KAttrAudioType )
        {
        HBufC16* tempValue = aValue.Alloc();
        TPtr16 lowercaseValue = tempValue->Des();
        lowercaseValue.LowerCase();

        if ( !( (lowercaseValue == KAudioAMR) ||
            (lowercaseValue == KAudioAWB)  ||
            (lowercaseValue == KAudioMP3)  ||
            (lowercaseValue == KAudioMP4)  ||
            (lowercaseValue == KAudio3GP)  ||
            (lowercaseValue == KAudio3G2)  ||
            (lowercaseValue == KAudioAAC)  ||
            (lowercaseValue == KAudioMID)  ||
            (lowercaseValue == KAudioMID2) ||
            (lowercaseValue == KAudioRMF)  ||
            (lowercaseValue == KAudioRMF2) ||
            (lowercaseValue == KAudioRMF3) ||
            (lowercaseValue == KAudioMXMF) ||
            (lowercaseValue == KAudioWMA) ))
                {
                iAudioStatus = EPlayerProhibit;
                }
        delete tempValue;
        }
    else if  (( aName ==  KAudioLevelStr)  || ( aName ==  KVolumeStr ))
        {
        TLex value  ( aValue );
        TReal32 volume;
        if( value.Val( volume, '.' ) == KErrNone)
            {
            iVolume = volume < 0 ? 0 : volume;
            iVolume = volume > 1 ? 1 : volume;
            }
        return KErrNone;
        }
    return CSvgMediaElementBase::SetAttributeL( aName, aValue );
    }

// ---------------------------------------------------------------------------
// Sets attributes of audio element
// ---------------------------------------------------------------------------

TInt  CSvgAudioElementImpl::SetAttributeDesL( const TInt aNameId,
											   const TDesC& aValue )
	{
	return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );
    }

// ---------------------------------------------------------------------------
// Sets attributes of audio element
// ---------------------------------------------------------------------------
TInt CSvgAudioElementImpl::SetAttributeFloatL( const TInt aNameId,
                                               TFloatFixPt aValue )
    {
	return CSvgMediaElementBase::SetAttributeFloatL(aNameId,aValue);
    }


// ---------------------------------------------------------------------------
// Gets attributes of audio element
// ---------------------------------------------------------------------------
TInt CSvgAudioElementImpl::GetAttributeFloat(const TInt aNameId,
                                                TFloatFixPt& aValue )
    {
	return CSvgMediaElementBase::GetAttributeFloat(aNameId,aValue);
    }


// perform a deep clone of this object
// ---------------------------------------------------------------------------
// Deep Copy of the parent
// ---------------------------------------------------------------------------
MXmlElement* CSvgAudioElementImpl::CloneL(MXmlElement* aParentElement)
    {
    CSvgAudioElementImpl* newElement = CSvgAudioElementImpl::NewL(
        this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );

    CleanupStack::PushL(newElement);
    newElement->iParentNode = aParentElement;
    // copy everything over
    this->CopyL(newElement);
    CleanupStack::Pop();
    return newElement;
    }


// ---------------------------------------------------------------------------
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
TBool CSvgAudioElementImpl::ReceiveEventL( MSvgEvent* aEvent )
    {
    return CSvgMediaElementBase::ReceiveEventProcL( aEvent, this );
    }

// ---------------------------------------------------------------------------
// CSvgAudioElementImpl::InitAnimationL
// From CSvgMediaElementBase
// ---------------------------------------------------------------------------
void CSvgAudioElementImpl::InitAnimationL()
    {
    CSvgMediaElementBase::InitAnimationL();
    }

// ---------------------------------------------------------------------------
// CSvgAudioElementImpl::ResetAnimationL
// From CSvgMediaElementBase
// ---------------------------------------------------------------------------
void CSvgAudioElementImpl::ResetAnimationL()
    {
	if (( iAnimStatus == KAnimActive ) ||
		( iAnimStatus == KAnimFinished) ||
		( iAnimStatus == KAnimEnd))
		{
			
		if (( iAudioStatus == EPlayerStatePlaying ) ||
		( iAudioStatus == EPlayerStateComplete ))
    		{
    		// Save the audio start offset
            iAudioStartOffset = 
                ( ( CSvgDocumentImpl* )iOwnerDocument)->CurrentTime();
            iStoredPos = 0;    
    		// if previously audio is still playing, and asked to play again...
        	iAudioPlayer->Stop();
            //For  Negative Begin
        	if ( iNegativeBeginTime < 0)
    	        {
    	            const TInt64 aInterval = iNegativeBeginTime * -KMillisecondsPerMicrosecond;
    	            TTimeIntervalMicroSeconds aTime(aInterval);        
    	            iAudioPlayer->SetPosition(aTime);
    	            iStoredPos = aTime;
    	        }
            //For  Negative Begin

        	iAudioPlayer->Play();
        	iAudioStatus = EPlayerStatePlaying;
    		}
		}
    }

// ---------------------------------------------------------------------------
// CSvgAudioElementImpl::Reset
// From CSvgMediaElementBase
// ---------------------------------------------------------------------------
void CSvgAudioElementImpl::Reset( MSvgEvent* aEvent )
	{

	iIsUserSeek = ETrue;
    iStoredPos = 0;
	// call the animation base function.
	TSvgTimerEvent* timerEvent  = ( TSvgTimerEvent* ) aEvent;
	if((TInt32)timerEvent->Time() < iAbsoluteBeginTime)
		{
		// let it come to initial position.
		//((CSvgDocumentImpl*)iOwnerDocument)->iInitialDrawFlag = ETrue;
		ReInitializeAnimation();
		return;
		}
	TRAPD(error,CSvgAnimationBase::ResetL( aEvent, this));
    if (error != KErrNone)
        {
        // error processing not processed
        return;
        }
	}  
	
// ---------------------------------------------------------------------------
// Call AnimaProcL
// ---------------------------------------------------------------------------
TBool CSvgAudioElementImpl::DoAnimProcL(MSvgEvent* aEvent)
    {
    return this->AnimProcL((MSvgTimerEvent*)aEvent);
    }


// ---------------------------------------------------------------------------
// From MsvgAnimationBase
// ---------------------------------------------------------------------------
TBool CSvgAudioElementImpl::AnimProcL( MSvgTimerEvent* /*aEvent*/ )
    {
#ifdef _DEBUG    
   	RDebug::Print(_L("%d %d"), iAnimStatus, IsAnimating());
#endif   	
    if ( iAudioStatus == EPlayerProhibit )
        return EFalse;
    
    if ( /*iAnimTime->DurationTime() == KTimeIndefinite ||*/ iAnimTime->DurationTime() == 0 )
        {
         return EFalse;
        }
	
    if ( !iAudioPlayer )
        {
    	iAudioPlayer = CMdaAudioPlayerUtility::NewL(*this);
        RDebug::Print(_L("SvgtAudioElem: Player created"));
        }

    if ( iAudioStatus != EPlayerStateIdle && !iAudioPlayer )
        {
        // Error case, when player is initialised but
        // iAudioPlayer pointer is NULL.
        return EFalse;
        }

    if( iAnimStatus != KAnimActive )
        {
        if ( iAudioStatus == EPlayerStatePlaying )
    		{
    		iStoredPos = 0;
        	iAudioPlayer->Stop();
        	iAudioStatus = EPlayerStateStop;
        	return EFalse;
    		}
        }
    else //iAnimStatus == KAnimActive
        {
        if ( !iHadBegun )
            {
            iHadBegun = ETrue;
            // Save the time the audio element starts
            iAudioStartOffset = 
                ( ( CSvgDocumentImpl* )iOwnerDocument)->CurrentTime();

            // this is a restart from begin list.
            if ((iAudioStatus != EPlayerStateIdle) &&
            	(iAudioStatus != EPlayerStatePlayInit))
            	{
		    	iStoredPos = 0;
		    	iAudioPlayer->Stop();
		    	iAudioPlayer->Play();
		    	iAudioStatus = EPlayerStatePlaying;
            	}
            return ETrue;
         }

        if ( iAudioStatus == EPlayerStateIdle ) // stopped
        {
        // Indicate to the time container that audio element 
        // is not ready
        ( ( CSvgDocumentImpl* )
            iOwnerDocument )->TimeContainer()->TimedEntityNotReady( 
            this );
        // Save the audio start offset
        iAudioStartOffset = 
            ( ( CSvgDocumentImpl* )iOwnerDocument)->CurrentTime();
        TPtrC href = Href();
        #ifdef _DEBUG            
        RDebug::Print(_L("CSvgAudioElementImpl::NewFilePlayerL is to be called"));
        RDebug::RawPrint(href);
        #endif           	           	

        //Check and load file here
        RFs  session;
        CSvgErrorImpl* SvgError  =  CSvgErrorImpl::NewL();
        CSvgDocumentImpl::OpenSession( session,  *SvgError  );

        RFile fileHandle;

        CSvgDocumentImpl* document = (CSvgDocumentImpl*)OwnerDocument();
        CSvgEngineImpl* engine  = document->Engine();
        MSvgRequestObserver* lRequestObserver = NULL;
        TInt lFetchStatus = KErrNone;
        if(engine!= NULL)
            {
            lRequestObserver =  engine->iRequestObserver;
            }
        else
            {
            return EFalse;
            }
            
        if(lRequestObserver != NULL)
            {
            lFetchStatus = lRequestObserver->FetchImage( *iUri, session, fileHandle );
            }
        else
            {
            return EFalse;
            }
        
        if(lFetchStatus == KErrNone)
         {
         iAudioPlayer->OpenFileL(fileHandle);
         
         }
           else
             {
             return EFalse;
             }
        if(SvgError) 
         {
         delete SvgError;
         }
        session.Close();
        iAudioStatus = EPlayerStatePlayInit;  	
        #ifdef _DEBUG       		
        RDebug::Print(_L("SvgtAudioElem: File opened"));
        #endif           	
        return ETrue;
        }
        if( iAudioStatus == EPlayerStateStop && iTcCommandState != ESvgTEPausedState)
        {

        		iAudioStartOffset = 
                ( ( CSvgDocumentImpl* )iOwnerDocument)->CurrentTime();
            // if previously audio is still playing, and asked to play again...
        	iAudioPlayer->Play();
        	iAudioStatus = EPlayerStatePlaying;
            }
        }
        return ETrue;
    }



// ---------------------------------------------------------------------------
// Set the volume of the Audio Player to the specified level
// ---------------------------------------------------------------------------
void CSvgAudioElementImpl::SetVolume(TInt aPercentage)
    {
    if(
    iAudioPlayer && 
    (iAudioStatus != EPlayerStateIdle && iAudioStatus != EPlayerStatePlayInit && iAudioStatus != EPlayerProhibit)
    )
        {
        TInt lVolLevel = 0;
        // Check if within limits
        if  ( aPercentage > 0  && aPercentage <= 100 )
            {
#ifdef __WINSCW__            
            const TReal32 KMaxAudioVolumeLevels = 65535;
#else
            const TReal32 KMaxAudioVolumeLevels = iAudioPlayer->MaxVolume();
#endif   
            // Calculating the volume based on system volume & element volume
            TReal32 lRealLevel = ((TReal32)aPercentage/100.0) * KMaxAudioVolumeLevels * iVolume;
            lVolLevel = lRealLevel;
            }
        iAudioPlayer->SetVolume(lVolLevel);
        }
    iPercentLevelVolume = aPercentage;
    }

// ---------------------------------------------------------------------------
// From CMdaAudioPlayerUtility
// Callback from Audio Player indicating the audio has finished initalisation
// ---------------------------------------------------------------------------
void CSvgAudioElementImpl::MapcInitComplete(TInt aError,
                const TTimeIntervalMicroSeconds& aDuration)
	{
	if ( aError == KErrNone )
	    {
       	if(iDurMedia)
          	{
            TTimeIntervalMicroSeconds durInMicroS;
            durInMicroS   = aDuration;


            TInt32 durInMilliS;
            durInMilliS  =  durInMicroS.Int64()  /  KMillisecondsPerMicrosecond;

            iAnimTime->SetDurationTime(durInMilliS);
            //In ReInitialize function of AnimTimeController
            // the duration is reinitialized from Org Duration
            iAnimTime->SetOrgDurationTime(durInMilliS);
           	}
#ifdef _DEBUG           	
    	RDebug::Print(_L("CSvgAudioElementImpl::MapcInitComplete :-> Open audio file OK!"));
#endif    	
    	iMediaDuration = aDuration;

        TInt lVolLevel = 0;

        // Check if within limits
        if  ( iPercentLevelVolume > 0  && iPercentLevelVolume <= 100 )
            {
#ifdef __WINSCW__            
            const TReal32 KMaxAudioVolumeLevels = 65535;
#else
            const TReal32 KMaxAudioVolumeLevels = iAudioPlayer->MaxVolume();
#endif            
            // Calculating the volume based on system volume & element volume
            TReal32 lRealLevel = ((TReal32)iPercentLevelVolume/100.0) * KMaxAudioVolumeLevels * iVolume;
            lVolLevel = lRealLevel;
            }
        iAudioPlayer->SetVolume(lVolLevel);
        // If the pause command was issued do not play, the resume should set 
        // it playing
        if(iTcCommandState == ESvgTEPlayingState)
            {
#ifdef _DEBUG            
            RDebug::Print(_L("To play"));
#endif            
            iAudioPlayer->Play();
            iAudioStatus = EPlayerStatePlaying;
            }
        else if(iTcCommandState == ESvgTEStoppedState)
            {
            iAudioStatus = EPlayerStateStop;
            }
        else
            {
             iAudioStatus = EPlayerStatePaused;
            }
        }
    else
        {
        #ifdef _DEBUG
    	RDebug::Print(_L("CSvgAudioElementImpl::MapcPlayComplete :-> Error code [%d]"), aError);
    	#endif
        }
    if ( iNegativeBeginTime < 0 )
        {
        const TInt64 aInterval = iNegativeBeginTime * -1000;
        TTimeIntervalMicroSeconds aTime(aInterval);        
        iAudioPlayer->SetPosition(aTime);
        iStoredPos = aTime;
        }
	// Indicate to time container that audio element is ready
    ( ( CSvgDocumentImpl* )iOwnerDocument )->TimeContainer()->TimedEntityReady( 
                this );	
        
	}

// ---------------------------------------------------------------------------
// From CMdaAudioPlayerUtility
// Callback from Audio Player indicating the audio has finished playing
// ---------------------------------------------------------------------------
void CSvgAudioElementImpl::MapcPlayComplete(TInt aError)
	{
    // normal play exit
	iAudioStatus = EPlayerStateComplete;

	if ( aError != KErrNone )
	    {
        #ifdef _DEBUG
	    RDebug::Print(_L("CSvgAudioElementImpl::MapcPlayComplete :-> Error code [%d]"), aError);
    	#endif
	    }
	}

    
// -----------------------------------------------------------------------------
// CSvgAudioElementImpl::CanGenerateTick
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
TBool CSvgAudioElementImpl::CanGenerateTick()
    {
    // If media is playing then audio element can generate tick
    if ( iAudioPlayer && 
        ( iAudioStatus == EPlayerStatePlaying || 
            iAudioStatus == EPlayerStatePlayInit ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSvgAudioElementImpl::GetEntityCurrentTime
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
//        
void CSvgAudioElementImpl::GetEntityCurrentTime( TUint32& 
            aEntityCurTime ) // Current Entity Time in msecs. 
    {
    if ( iAudioPlayer && iAudioStatus != EPlayerStateIdle )
        {
        // Convert to  milliseconds and account for negative  begin  time
        // and the audio start offset  and  send back
        if(iAudioStatus == EPlayerStateComplete)
        {
            TTimeIntervalMicroSeconds lDuration = iAudioPlayer->Duration();
            aEntityCurTime = iAudioStartOffset +  lDuration.Int64() / KMillisecondsPerMicrosecond;
        }
        else
        {
            TTimeIntervalMicroSeconds lPos; 
            iAudioPlayer->GetPosition( lPos );
            
            // at EOF lPos returns zero, no point of time should 
            // iStorePos be greater than lPos
            if(iStoredPos > lPos)
                {
                lPos = iStoredPos;
                }
            else
                {
                iStoredPos = lPos;
                }
        #ifdef _DEBUG
    	RDebug::Printf("====================================");
    	RDebug::Printf("Audio gets current position \n");
    	RDebug::Printf("lpos %d", lPos.Int64());
    	RDebug::Printf("====================================");
    	#endif
        aEntityCurTime = iAudioStartOffset + lPos.Int64() / KMillisecondsPerMicrosecond;
        }
        if(iAudioStatus == EPlayerStatePlaying)
            {
            aEntityCurTime += iNegativeBeginTime;
            }
        }
    else
        {
        aEntityCurTime = iAudioStartOffset;
        }
    }

// -----------------------------------------------------------------------------
// CSvgAudioElementImpl::ResyncTimedEntity
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
void CSvgAudioElementImpl::ResyncTimedEntity( 
            TUint32 aSyncTime ) // Time for resync in msecs.
    {
    if ( iAudioPlayer && iAudioStatus == EPlayerStatePlaying )
        {
        // Convert sync to microsecs before setting position
        TTimeIntervalMicroSeconds  lSeekPos(  aSyncTime  *  KMillisecondsPerMicrosecond );
        iAudioPlayer->SetPosition( lSeekPos );
        iStoredPos = lSeekPos;
        }
    } 

// -----------------------------------------------------------------------------
// CSvgAudioElementImpl::PauseTimedEntity
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
void CSvgAudioElementImpl::PauseTimedEntity()
    {

    iTcCommandState = ESvgTEPausedState;
      if ( iAudioPlayer && iAudioStatus == EPlayerStatePlaying )
        {
            iAudioPlayer->Pause();
        	iAudioStatus = EPlayerStatePaused;
        }
    }

// -----------------------------------------------------------------------------
// CSvgAudioElementImpl::ResumeTimedEntity
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
void CSvgAudioElementImpl::ResumeTimedEntity()
    {
    iTcCommandState = ESvgTEPlayingState; 
    // Resume of the player is done only when it is Pause State
    // When resume is done on Stopped/Completed, it might start playing it
    // even though animation is not active or it is not supposed to repeat.
	if ( iAudioStatus == EPlayerStatePaused )
      {
    	iAudioPlayer->Play();
    	iAudioStatus = EPlayerStatePlaying;
      }
    }

// -----------------------------------------------------------------------------
// CSvgAudioElementImpl::StopTimedEntity
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
void CSvgAudioElementImpl::StopTimedEntity()
    {
    iTcCommandState = ESvgTEStoppedState;
    if ( iAudioPlayer && iAudioStatus != EPlayerStateIdle )
        {
        iStoredPos = 0;
        iAudioPlayer->Stop();
        iAudioStatus = EPlayerStateStop;
        }
    }
    
 
// ---------------------------------------------------------------------------
//From MSvgTimedEntityInterface
// ---------------------------------------------------------------------------
TSvgObjectType CSvgAudioElementImpl::ObjectType()
    {
	return ESvgAudioElement;	
    }



// ---------------------------------------------------------------------------
// returns the child time container of the element 
// used in timecontainer
// ---------------------------------------------------------------------------
CSvgTimeContainer* CSvgAudioElementImpl::GetChildTimeContainer()
    {
    return NULL;
    }
    
    
// ---------------------------------------------------------------------------
// Constructor for CSvgAudioElementImpl
// ---------------------------------------------------------------------------
CSvgAudioElementImpl::CSvgAudioElementImpl( CSvgDocumentImpl* aDoc )
                            : CSvgMediaElementBase( aDoc ),
                            iAudioStatus(EPlayerStateIdle),
                            iVolume(1),
                            iTcCommandState(ESvgTEPlayingState),
                            iDurMedia(ETrue)
    {
	iAttrId = KAtrAudioId;
	iAudioPlayer = NULL;
    }


// ---------------------------------------------------------------------------
// perform a deep copy of this object
// ---------------------------------------------------------------------------
void CSvgAudioElementImpl::CopyL( CSvgAudioElementImpl* aDestElement )
    {
    if(aDestElement)
        {
          // copy stuff from superclass
        this->CSvgElementImpl::CopyL(aDestElement);

        // copy the reference to idoc (CSvgDocumentImpl)
        aDestElement->iOwnerDocument = this->iOwnerDocument;
        aDestElement->iAudioPlayer = this->iAudioPlayer;
        aDestElement->iAudioStatus = this->iAudioStatus;
        aDestElement->iVolume = this->iVolume;
        aDestElement->iPercentLevelVolume = this->iPercentLevelVolume;
        aDestElement->iMediaDuration = this->iMediaDuration;
        aDestElement->iTcCommandState = this->iTcCommandState;
        aDestElement->iAudioStartOffset = this->iAudioStartOffset;
        aDestElement->iDurMedia = this->iDurMedia;
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAudioElementImpl::DeactivateAnimation()
	{
	CSvgAnimationBase::DeactivateAnimation(this);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CSvgAudioElementImpl::Print( TBool aIsEncodeOn )
    {
	if (!aIsEncodeOn)
    	{
    	#ifdef _DEBUG
    	RDebug::Printf("<audio>");
    	#endif
        }
    }

// ---------------------------------------------------------------------------
// End of file
// ---------------------------------------------------------------------------
