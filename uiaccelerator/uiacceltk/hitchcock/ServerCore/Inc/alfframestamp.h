/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Helper class for frame rate calculation
*
*/


#ifndef C_ALFFRAMESTAMP_H_
#define C_ALFFRAMESTAMP_H_

NONSHARABLE_CLASS(TFrameStamp)
    {
public:
    
    TFrameStamp(){};
    
    TFrameStamp( TUint aFrameIndex )
        { 
        iFrameIndex = aFrameIndex;
        iTime.UniversalTime();
        };
    
    /*
     * FrameRate
     *
     * Calculates framerate between this and the given frame
     * 
     * @aParam  aStamp  The beginning frame for FPS calculation. 
     *                  This object represents the end frame.
     */
    TReal32 FrameRate(TFrameStamp& aStamp)
        {
        TInt64 delta = iTime.MicroSecondsFrom( aStamp.iTime ).Int64();

#ifdef EKA2
        TReal32 elapsed = delta / 1.0e6;
#else
        TReal32 elapsed = delta.GetTReal() / 1.0e6;
#endif
        TReal32 elapsedFrames = iFrameIndex - aStamp.iFrameIndex; 
        TReal32 fps = 0;
        RDebug::Print(_L("FrameRate - frames %4.2f, elapsed time in microseconds: %4.2f"), elapsedFrames, elapsed);
        
        if ( elapsed != 0.0 )
            {
            fps = ( elapsedFrames  / elapsed);
            }
        return fps;
        };

private:

    TTime iTime;

    TUint iFrameIndex;

    };


#endif /* C_ALFFRAMESTAMP_H_ */
