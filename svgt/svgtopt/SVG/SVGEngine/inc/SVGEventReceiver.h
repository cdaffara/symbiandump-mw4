/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Engine header file
 *
*/


#ifndef __INC_MSVGEVENTRECEIVER__
#define __INC_MSVGEVENTRECEIVER__




class MSvgEvent;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgEventReceiver
    {
    public:

        /**
         * Receive and process and event
         *
         * @since 1.0
         * @param aEvent - A event that matches the event mask registered for
         * @return A boolean that indicates success/failure of event processing
         */
        virtual TBool   ReceiveEventL( MSvgEvent* aEvent ) = 0;

        /**
         * Receive and process and event
         *
         * @since 3.2
         * @param aEvent - A event that matches the event mask registered for
         * @param aSubEventMask - The mask that tells which interactive event is 
         *                        associated with the element. 
         * @return A boolean that indicates success/failure of event processing
         */
        virtual TBool   ReceiveEventL( MSvgEvent* aEvent, const TUint16 aSubEventMask ) = 0;

        /**
         * Reset the state of the target element
         *
         * @since 1.0
         * @param None
         * @return None
         */
        virtual void    ResetReferenceElementL() = 0;

		virtual void	ResetReferenceAttrSet() = 0;

		virtual void    Reset(MSvgEvent* aEvent) = 0;
		virtual void   ReInitializeAnimation() = 0;
		virtual void   DeactivateAnimation() = 0;
   		/* 
   		* This function is specifically introduced to take care of AnimProcL
   		* 
   		*@param None. 
   		* @return TBool. representing whether the AnimProcL was sucessful or some error 
   		* was encountered. 
   		*/
   		virtual TBool   DoAnimProcL(MSvgEvent* aEvent) = 0;
	

    };

#endif /*__INC_MSvgEventReceiver__*/
