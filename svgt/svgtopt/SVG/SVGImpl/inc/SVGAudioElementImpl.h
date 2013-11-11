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
* Description:  SVG Implementation header file
 *
*/


#ifndef C_SVGAUDIOELEMENTIMPL_H
#define C_SVGAUDIOELEMENTIMPL_H

#include "SVGMediaElementBase.h"
#include "SVGRequestObserver.h"
#include <mdaaudiosampleplayer.h>

class CSvgDocumentImpl;
class MXmlDocument;

/**
 * Class description: 
 *  This class provides the implementation for the audio 
 *  element as mentioned in the svg-t 1.2 specification.
 *  @lib SVGEngine.lib
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CSvgAudioElementImpl ) : public CSvgMediaElementBase,
                            public MMdaAudioPlayerCallback
    {
    public:
        /**
         * Deep Copy of the parent
         *
         * @since S60 3.2
         * @param element to be copied
         * @return
         */
        MXmlElement*  CloneL(MXmlElement* aParentElement);

        /**
         * Create new element of audioelementimpl
         *
         * @since S60 3.2
         * @param
         * @return
         */
        static CSvgAudioElementImpl* NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

        /**
         * Create new element of audioimpl
         *
         * @since S60 3.2
         * @param
         * @return
         */
        static CSvgAudioElementImpl* NewLC(  const TUint8 aElemID,
                                       CSvgDocumentImpl* aDoc );

    private:
        /**
         * Create new element of audioelementimpl
         *
         * @since S60 3.2
         * @param
         * @return
         */
         void  ConstructL(  const TUint8 aElemID,CSvgDocumentImpl* aDoc );

    public:
        void CSvgAudioElementImpl::SetUriL( const TDesC& aUri );

        /**
         * Constructor for AudioElement
         *
         * @since S60 3.2
         * @param SvgDocument
         * @return None
         */
        CSvgAudioElementImpl( CSvgDocumentImpl* aDoc );


        /**
         * Destructor for Audioelement
         *
         * @since S60 3.2
         * @param None
         * @return None
         */
        virtual ~CSvgAudioElementImpl();




        /**
         * Need method description
         *
         * @since S60 3.2
         * @param
         * @return
         */
        void LoadUriL();

        // From MXmlElement API


        /**
         * Sets attributes of audio element
         *
         * @since S60 3.2
         * @param name of attribute and value to be set to attribute
         * @return
         */
        TInt SetAttributeL( const TDesC& aName, const TDesC& aValue );

        /**
         * Sets attributes of audio element
         *
         * @since S60 3.2
         * @param name of attribute and value to be set to attribute
         * @return
         */
        TInt SetAttributeDesL( const TInt aNameId, const TDesC& aValue );


        /**
         * Get attributes of audio element
         *
         * @since S60 3.2
         * @param name of attribute and value to be set to attribute
         * @return
         */
        TInt GetAttributeFloat( const TInt aNameId, TFloatFixPt& aValue );

        /**
         * Sets attributes of audio element
         *
         * @since S60 3.2
         * @param name of attribute and value to be set to attribute
         * @return
         */
        TInt SetAttributeFloatL( const TInt aNameId, TFloatFixPt aValue );


        /**
         * From CSvgMediaElementBase
         * ReceiveEventL: This function will be called whenever the registered
		 * event was happened.
         *
         * @since S60 3.2
         * @param MSvgEvent* aEvent
         * @return TBool ETrue if redraw is needed
         */
        TBool ReceiveEventL( MSvgEvent* aEvent );

        /**
         * From CSvgMediaElementBase        
         *
         * @since S60 3.2
         * @param none.
         * @return none.
         */
        void ResetAnimationL();

        /**
         * From CSvgMediaElementBase        
         *
         * @since S60 3.2
         * @param MSvgEvent* aEvent
         * @return none.
         */
        void Reset( MSvgEvent* aEvent );

        /**
         * Call AnimaProcL
         *
         * @since S60 3.2
         * @param MsvgEvent
         * @return Boolean indicating successfull completion of the event
         */
    	TBool DoAnimProcL(MSvgEvent* aEvent);


        /**
         * Return wether an svg element is displayable, such as <rect>, 
         * <circle>, or an abstract element, such as <g>, <animate>.
         *
         * @since S60 3.2
         * @param None
         * @return ETrue if element is viewable.
         */
        TBool IsViewable() { return EFalse; }

        /*
        * Set the volume of the Audio Player to the specified level
        *
        * @since S60 3.2
        * @param TInt Volume
        * @return none
        */
        void SetVolume(TInt aVolume);
		void Print( TBool aIsEncodeOn );
    
    protected:

        /**
         * Need method description
         *
         * @since S60 3.2
         * @param CSvgAudioElementImpl
         * @return None
         */
        void CopyL( CSvgAudioElementImpl* aDestElement );

    private:
        /*
        * From MsvgAnimationBase
        */
        virtual TBool AnimProcL( MSvgTimerEvent* aEvent );
        /*
        * From MsvgAnimationBase
        */
        virtual void InitAnimationL();

        /*
        * From CMdaAudioPlayerUtility
        * Callback from Audio Player indicating the audio has finished 
        * initialisation
        * @ since S60 3.2
        */
    	virtual void MapcInitComplete( TInt aError, 
    	                                const TTimeIntervalMicroSeconds& aDuration );
    	/*
        * From CMdaAudioPlayerUtility
        * Callback from Audio Player indicating the audio has finished 
        * playing
        * @ since S60 3.2
        */                                
    	virtual void MapcPlayComplete(TInt aError);
    
    private:
    // From base classes
        
        /**
        * From CSvgMediaElementBase
        * When the timed entity acts as timing master in the time container, 
        * the time container gets the timed entity clock using this method 
        * and feeds to rest of timed entities.
        * @since S60 3.2
        * @param aEntityCurTime Current Entity Time in msecs. 
        * @return none.
        */
        void GetEntityCurrentTime( TUint32& aEntityCurTime );


        /**
        * From CSvgMediaElementBase
        * Check if timed entity can provide timing ticks to rest of time 
        * container elements. This behavior could change dynamically. 
        * For example, if audio clip is over, the audio element can't generate 
        * ticks for others.
        * @since S60 3.2
        * @param none.
        * @return TBool True if can generate timing tick.
        */
        TBool CanGenerateTick();
        
        /**
        * From CSvgMediaElementBase
        * If the timed entity needs to be in sync with the time container and 
        * it has slipped beyond the sync tolerance limit, the method is called to 
        * bring the element in sync with the time container.
        * @since S60 3.2
        * @param aSynctime Time for resync in msecs.
        * @return none.
        */
        void ResyncTimedEntity( TUint32 aSynctime );

        /**
        * From CSvgMediaElementBase
        * This would be used for pausing the timed entity while other locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void PauseTimedEntity();

        /**
        * From CSvgMediaElementBase
        * This would be used for resuming the timed entity once all locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void ResumeTimedEntity();

        /**
        * From CSvgMediaElementBase
        * This would be used for stoping the timed entity once all locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void StopTimedEntity();


        /*
        * From MSvgTimedEntityInterface
        * Would return the type of object
        * @param none
        * @return type of object
        */
        TSvgObjectType ObjectType();

        CSvgTimeContainer* GetChildTimeContainer();
        
        
        void DeactivateAnimation();
        
        
    private:

        //Audio Player Utility 
    	CMdaAudioPlayerUtility* iAudioPlayer;

    	enum TPlayerState
    	    {
    		EPlayerStateIdle,
    		EPlayerStatePlayInit,
    		EPlayerStatePlaying,
    		EPlayerStatePaused,
    		EPlayerStateStop,
    		EPlayerProhibit,
    		EPlayerStateComplete,
    		
    	    };

        //Current status of player 
        TPlayerState iAudioStatus;
        //Current Volume of the player
        TReal32 iVolume;
        TInt iPercentLevelVolume;
        // Media duration in usecs
        TTimeIntervalMicroSeconds iMediaDuration;
        // Indicates whether time container issued a pause command
        enum  TcCommandStatus 
		    { 
			ESvgTEPausedState, 
			ESvgTEStoppedState, 
			ESvgTEPlayingState
		    };
		
		//Current Command on the player
        TcCommandStatus iTcCommandState;
        // Offset in msecs from document start since the audio 
        // element became active
        TUint32 iAudioStartOffset;
        
        //Flag indicating if duration is specified as media
        TBool iDurMedia;
        
        HBufC* iUri;
        
        //Indicates the length of the clip that has already played
        //Introduced to ensure that getentitycurrent time returns correct value
        //At the end of a clip getPosition returns zero 
        //because of some bug in audio player
        TTimeIntervalMicroSeconds iStoredPos;

    };

#endif //C_SVGAUDIOELEMENTIMPL_H
// ---------------------------------------------------------------------------
// End of file
// ---------------------------------------------------------------------------
