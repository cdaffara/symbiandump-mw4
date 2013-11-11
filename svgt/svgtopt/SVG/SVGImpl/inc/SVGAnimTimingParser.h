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
* Description:  SVG Implementation header file
 *
*/


#ifndef _INC_TSVGANIMTIMINGPARSER_
#define _INC_TSVGANIMTIMINGPARSER_

#include <e32base.h>

#include "SVGEvent.h"


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgAnimTimingParser : public CBase
    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgAnimTimingParser*    NewL( const TDesC& aTimingDes, CSvgElementImpl* aElement );

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgAnimTimingParser*    NewLC( const TDesC& aTimingDes, CSvgElementImpl* aElement );

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
                                        ~CSvgAnimTimingParser();
    private:

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
                                        CSvgAnimTimingParser();

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                            ConstructL( const TDesC& aTimingDes, CSvgElementImpl* aElement );


    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
      void                            Parse( TDes& aIdValue,
                                               TSvgEvent& aEvent,
                                               TInt32& aClockValue,
                                               TReal32& aRepeatValue,
                                               TBool aBeginAttribute);

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
      void                            ParseEvent( TInt32 aEventNumber, 
                                               TDes& aIdValue,
                                               TSvgEvent& aEvent,
                                               TInt32& aClockValue ); 
        


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        inline TUint WallClockValue()
            {
            return iWallClockValue;
            }

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        inline TUint16 AccekeyValue()
            {
            return iAccessKeyValue;
            }

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        inline TUint RepeatValue()
            {
            return iRepeatCount;
            }


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void        ParseClockValue( TLex& aLex, TInt32& aClockValue );
        

    private:

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void        ParseWallclockValue( TLex& aLex );   // Not yet implemented

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TSvgEvent   DesToEventId( const TDesC& aEventDes );

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void        SkipUntilNumEnd( TLex& aLex );

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void        SkipAlphaNumeric( TLex& aLex );

    private:
        TUint       iWallClockValue;
        TUint16     iAccessKeyValue;    // A Unicode character
        TUint       iRepeatCount;

        TPtr        iTimingDes;
        HBufC*      iBuf;
        CSvgElementImpl* iElement;
    };


#endif  // _INC_TSvgAnimTimingParser_
