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
* Description:  NVG Decoder header file
 *
*/

#ifndef NVGFITTOVIEWBOX_H
#define NVGFITTOVIEWBOX_H

 
#include <e32std.h>
#include <e32base.h>
#include <VG/openvg.h>
#include <nvg.h>


class CNvgFitToViewBoxImpl:  public CBase
    {
private:
    CNvgFitToViewBoxImpl();
     
     /**
          * @Purpose: 2 Phase Constructor
          * @Version
          * @parameter: None
          * @return: None
     */
     void ConstructL();
     
     /**
          * @Purpose: Destructor
          * @Version
          * @parameter: None
          * @return: None
     */
    public:
    
    /**
         * @Purpose: 2 Phase Constructor
         * @Version
         * @parameter: None
         * @return: None
    */
    static CNvgFitToViewBoxImpl*    NewL();
    
    /**
         * @Purpose: 2 Phase Constructor
         * @Version
         * @parameter: None
         * @return: None
    */
    static CNvgFitToViewBoxImpl*    NewLC();
    
    /**
         * @Purpose: Constructor
         * @Version
         * @parameter: None
         * @return: None
    */
 
    virtual ~CNvgFitToViewBoxImpl();
    
     /**
         * @Purpose: Setter function for viewbox    
         * @Version:    
         * @parameter:  TRect which will be the viewbox for the content
         * @return: None
    */
    inline void SetViewBox(TReal avbX, TReal avbY,TReal avbW,TReal avbH);
    
    /**
         * @Purpose: Setter function for Aligning
         * @Version
         * @parameter: Indicates to fit the viewport in the viewbox
         * @return: None
    */
    inline void SetAlign(TNvgAlignStatusType aAlignStatus);
    
    /**
         * @Purpose: Setter function for MeetorSlice
         * @Version
         * @parameter: Indicates whether to streach or slice the content
         * @return: None
    */
    inline void SetScaling(TNvgMeetOrSliceType aMeetSlice);    
    
    /**
         * @Purpose: Setter function for Transformation matrix
         * @Version
         * @parameter: The first two rows of the transformation matrix
         * @return: None
    */
    inline void SetTransform(TReal aM00,TReal aM01,TReal aM02,TReal aM10,TReal aM11,TReal aM12);
    
    
    /**
         * @Purpose: Update the transformation matrix for Translating the 
                     Contnet
         * @Version
         * @parameter: Translation components by aX, aY 
                       coordinates
         * @return: None
    */
    inline void Translate(TReal aX, TReal aY);
    
    /**
         * @Purpose: Update the transformation matrix for 
                     Scaling the Contnet
         * @Version
         * @parameter: Scaling factors Sx, Sy
         * @return: None
    */
    inline void Scale(TReal aX, TReal aY);
    
   /**
         * @Purpose: Calculating the transfomation matrix to 
                     carry out the viewport to viewbox 
                     transformation
         * @Version
         * @parameter: The viewport which is to be transformed
                       If viewbox is not defined, set the size of the viewbox 
         * @return: None
    */
    void SetWindowViewportTrans( TRect aViewPort, TSize aSize );
    
        
    void Concatenate(TReal aMatrix[]);
        
    void Concatenate(TReal aM00, TReal aM01, TReal aM02, TReal aM10, TReal aM11, TReal aM12);

public:        
    // viewbox to viewport transformation matrix
    TReal iM00;
    TReal iM01;
    TReal iM02;
    TReal iM10;
    TReal iM11;
    TReal iM12;
    
    //The viewBox onto which the content will be drawn
    TReal   ivbX;
    TReal   ivbY;
    TReal   ivbW;
    TReal   ivbH;
    
    //Indicate that the content has a viewbox defined
    TBool iViewBoxDefined;
    
    //Define how to fit the content into the viewport
    TNvgAlignStatusType iAlign;
    
    //Weather to streach or clip the content to fit it into the viewbox       |
    TNvgMeetOrSliceType iMeetSlice;
    };
    
#include "nvgfittoviewbox.inl"
#endif
//----------------------------EndOfFile-----------------------