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
* Description:   Help class for debugging/optimizing the command stream. 
* -----------------------------------------------------------------------------
* How to use CAlfCommandDebug
* 
* - alfred.iby
*      - enable inclusion of alfcommanddebug.dll and its resource 
*      alfcommanddescriptions.rsc
* 
* What information are you interested in?
*      - Select the printed debug information in alfcommanddebug.cpp in 
*      ConstructL method.
* 
* Decide, which point do you want to debug. You can select more than one option.
* 
* - alfrenderstage - can print all serialized commands 
*   - enable flag and macro _ALF_PRINT_WS_COMMANDS_ in alfrenderstageplugin.mmp
*  
*      
* - alfhierarcy - can print all hierarchy related commands (creation, destruction, 
*  posting draw, etc)
*   - define _ALF_LOGGING and undefine it in alfhierarchy.cpp around #include <alflogger.h> .
*    
*   - define MACRO HUI_DEBUG_TRACK_DRAWING and #define HUI_DEBUG_TRACK_DRAWING in 
*    alfappservercore.mmp    
*
*
* - alfstreamerbridge - can give human readable name for the commands added to 
* the queue and processed from the queue
*       
*    - define _ALF_LOGGING and undefine it in alfstreamerbridge.cpp 
*   around #include <alflogger.h> .
*    
*    - define MACRO HUI_DEBUG_TRACK_DRAWING and #define HUI_DEBUG_TRACK_DRAWING in 
*   alfappservercore.mmp    
*
*
* - alfbridge - can tag command buffers with the process name that drew the commands.      
*    - define _ALF_LOGGING and undefine it in alfbridge.cpp 
*   around #include <alflogger.h> .
*    
*  - define MACRO HUI_DEBUG_TRACK_DRAWING and #define HUI_DEBUG_TRACK_DRAWING 
*  in alfappservercore.mmp    
*
* - huicanvaswspainter.cpp - can print the command names and their parameters 
* and the process name that drew them (you must enable the above alfbridge debugs, 
* if you want to see process names)
* 
*   - define MACRO HUI_DEBUG_TRACK_DRAWING and #define HUI_DEBUG_TRACK_DRAWING in huitk_base.mmp
*   
*   - enable wanted prints in alfcommanddebug.cpp ConstructL function.    
*
* NOTE, Logging for all __ALFLOGSTRING variants can be enabled in alfloggingconfiguration.h .
* However, be prepared for massive amount of debug data and even more sluggish performance, 
* if that is enabled.
* 
*/

#ifndef CALFCOMMANDDEBUG_H_
#define CALFCOMMANDDEBUG_H_

#include <e32std.h> 
#include <e32base.h>
#include <s32mem.h>
#include <s32strm.h>
#include <e32debug.h>
#include <e32hashtab.h>
#include <gdi.h>
#include <w32std.h>
#include <alfcommanddescriptions.rsg>

class CAlfScreen;
class CHuiControlGroup;

NONSHARABLE_CLASS( CAlfCommandDebug ) : public CBase
    {
    public:

        IMPORT_C static CAlfCommandDebug* NewL();

        IMPORT_C void StartFrame();

        IMPORT_C void EndFrame();

        IMPORT_C void PrintStatistic();
        
        IMPORT_C void SetDescriptionAndSize( const TUint8& aCommand, const TInt& aSize, TInt aDescriptionList = R_ALF_COMMAND_DESCRIPTION_ARRAY  );
        
        IMPORT_C void SetDescription( const TUint8& aCommand, TInt aDescriptionList = R_ALF_COMMAND_DESCRIPTION_ARRAY );
        
        IMPORT_C void SetHandle( const TInt& aHandle );
        
        IMPORT_C void SetPoint( const TPoint& aPoint );
        
        IMPORT_C void SetRect( const TRect& aRect );
        
        IMPORT_C void SetRegion( const TRegion& aRegion );
        
        IMPORT_C void SetColor( TRgb& aColor );
        
        IMPORT_C void SetText( TPtr& aText );
        
        IMPORT_C void Print();
        
        IMPORT_C void AdjustCommand( const TUint8& aCommand, TInt iAdjustment );
        
        IMPORT_C void SetPrint( TBool aPrint );
        
        IMPORT_C TFileName WServClientFileName(TInt aClientWindowGroupId, RWsSession& aSession );
            
        IMPORT_C TBool TrackProcess( HBufC16* aFileName );
        
        IMPORT_C ~CAlfCommandDebug();
        
        IMPORT_C TDesC16& Text();
                
    private:

        void ConstructL();

        class TCommand : public CBase
            {
            
            public:

                TCommand() : iDescription(NULL), iCount(0), iSize(0){};
                
                HBufC16* iDescription;
                TUint iCount;
                TUint iSize;
            };
        
        void ReadCommandDescriptionsL( RHashMap<TUint32,TCommand>* aDescriptionArray, TInt aId );
        
        
        
    private:
        

        
        RHashMap<TUint32,TCommand>* iCommandDescriptions;
        RHashMap<TUint32,TCommand>* iBridgeCommandDescriptions;
        
        TInt iTotalCmdCount;
        TInt iTotalCmdSize;
        
        TInt iFrameCount;
        Uint64 iFrameTotalSize;
        
        TBuf<1024> iText;
        
        // Change run-time the content of this of set it in ConstructL to track 
        // certain process drawing in device 
        TFileName iTrackedProcess;
        
        enum TLogginMode
            {
            EPrintOnlyCommands = 1,
            EPrintCommandParameters,
            EPrintStatistics,
            EPrintStatisticsForLargeBuffers,
            EPrintSummary
            };
            
        TLogginMode iMode;

public:        

        TInt iPrint;
        TBool iPrintStatistics;
        TInt iLargeBufferDefinition; 
        TBool iPrintColors;
        TBool iPrintRegions;
        TBool iPrintRects;
        TBool iPrintPoints;
        TBool iPrintHandles;
        TBool iPrintTexts;
        TBool iPrintSummary;
        
    };

#endif /* CALFCOMMANDDEBUG_H_ */
