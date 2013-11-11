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
 *                This is file is not compiled by default. Enable flag 
 *                _ALF_PRINT_WS_COMMANDS_ in alfrenderstageplugin.mmp in order
 *                to use it.
 *              
 *                 SEE USAGE INSTRUCTIONS IN ALRCOMMANDDEBUG.H 
 */
#include <e32def.h>
#include <barsread.h>
#include <barsc.h>
#include <ConeResLoader.h>
#include <s32mem.h> // RDesWriteStream
#include <alflogger.h> 
#include <huiwscanvascommands.h>
#include "alfcommanddebug.h"
#include <data_caging_path_literals.hrh>
#include <alfcommanddescriptions.rsg>    
#include <alfbridgecommands.h>
#include <gdi.h>
#include <uiacceltk/HuiStatic.h>

_LIT( KRendererCommandDescriptions,"alfcommanddescriptions.rsc" );

// ---------------------------------------------------------------------------
// ReadCommandDescriptionsL
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCommandDebug* CAlfCommandDebug::NewL()
    {
    CAlfCommandDebug* self = new (ELeave) CAlfCommandDebug;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
//
// Note, you can enable iPrintStatistics at run-time in debugger  
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCommandDebug::~CAlfCommandDebug()
    {
    TInt test1 = 0;
    TInt testCount1 = iCommandDescriptions->Count();
    
    THashMapIter <TUint32,TCommand> ptrHashSetIter(*iCommandDescriptions);
     for ( ; ; )        
         {        
         const TCommand* resNext = ptrHashSetIter.NextValue();        
         if (!resNext)            
             {             
             break;            
             }  
         //RDebug::Print(_L("!!HV: ~CAlfCommandDebug. DELETE: command.iDescription=%S (0x%x)"), 
         //        resNext->iDescription, resNext->iDescription);
         delete resNext->iDescription;
         test1++;
         }

    if (test1 != testCount1)
        {
        RDebug::Print(_L("CAlfCommandDebug::~CAlfCommandDebug(). Mismatch in iCommandDescriptions delete: deleted=%d, array count=%d"), test1, testCount1);
        }

    TInt test2 = 0;
    TInt testCount2 = iBridgeCommandDescriptions->Count();
    
    THashMapIter <TUint32,TCommand> ptrHashSetIter2(*iBridgeCommandDescriptions);
     for ( ; ; )        
         {        
         const TCommand* resNext = ptrHashSetIter2.NextValue();        
         if (!resNext)            
             {             
             break;            
             }  
         //RDebug::Print(_L("!!HV: ~CAlfCommandDebug. DELETE2: command.iDescription=%S (0x%x)"), 
         //        resNext->iDescription, resNext->iDescription);
         delete resNext->iDescription;
         test2++;
         }
  
    if (test2 != testCount2)
        {
        RDebug::Print(_L("CAlfCommandDebug::~CAlfCommandDebug(). Mismatch in iBridgeCommandDescriptions delete: deleted=%d, array count=%d"), test2, testCount2);
        }
    
    iCommandDescriptions->Close();
    iBridgeCommandDescriptions->Close();
    
    delete iCommandDescriptions;
    delete iBridgeCommandDescriptions;
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfCommandDebug::ConstructL()
    {
    iCommandDescriptions = new (ELeave) RHashMap<TUint32, TCommand> ;
    iBridgeCommandDescriptions = new (ELeave) RHashMap<TUint32, TCommand> ;
    ReadCommandDescriptionsL( iCommandDescriptions, R_ALF_COMMAND_DESCRIPTION_ARRAY );
    ReadCommandDescriptionsL( iBridgeCommandDescriptions, R_ALF_BRIDGE_COMMAND_DESCRIPTION_ARRAY );
    
    // TODO: define only exe name, TrackProcess can do the parsing
#ifdef __WINSCW__
    _LIT( KTrackThisProcess, "Z:\\sys\\bin\\matrixmenu.exe");
#else
    _LIT( KTrackThisProcess, "C:\\sys\\bin\\matrixmenu.exe");
#endif
    iTrackedProcess.Copy( KTrackThisProcess );
    
	// iMode = EPrintOnlyCommands;
    iMode = EPrintCommandParameters; 
    // iMode = EPrintSummary;
    //iMode = EPrintStatisticsForLargeBuffers;
     
    switch( iMode )
        {
        default:  // 0
            {
            iPrint = EFalse; 
            break;
            }
        case EPrintCommandParameters:
            {
            iPrintColors = ETrue;
            iPrintRegions  = ETrue;
            iPrintRects = ETrue;
            iPrintPoints = ETrue;
            iPrintHandles = ETrue;
            iPrintTexts = ETrue;
            iPrint = ETrue;
            break;
            }
        case EPrintOnlyCommands:
            {
            iPrint = ETrue;
            break;
            }
        case EPrintStatistics:
            {
            iPrintStatistics = ETrue;
            iLargeBufferDefinition = 0;
            break;
            }
        case EPrintStatisticsForLargeBuffers:
            {
            iPrintStatistics = ETrue;
            iLargeBufferDefinition = 1500;
            break;
            }
        case EPrintSummary:
            {
            iPrintSummary = ETrue;
            iLargeBufferDefinition = 0;
            break;
            }
                
        }
    }

// ---------------------------------------------------------------------------
// ReadCommandDescriptionsL
// ---------------------------------------------------------------------------
//
void CAlfCommandDebug::ReadCommandDescriptionsL( RHashMap<TUint32,TCommand>* aDescriptionArray, TInt aId )
    {
    // Read unsupported command resource
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RResourceFile resFile;
    TInt result;

    TFileName resourceFileName;
    TPtrC driveLetter = TParsePtrC(RProcess().FileName()).Drive();
    resourceFileName.Copy(driveLetter);
    resourceFileName.Append(KDC_ECOM_RESOURCE_DIR);
    resourceFileName.Append(KRendererCommandDescriptions);

    TRAP( result, resFile.OpenL(fs,resourceFileName); );
    if (result == KErrNone)
        {
        CleanupClosePushL(resFile);
        resFile.ConfirmSignatureL(0); // offset value.
        TInt resId = aId;

        HBufC8* readData = resFile.AllocReadL(resId);
        // now first get rid of RResourceFile and RFs as they are not needed any more
        CleanupStack::PopAndDestroy(); // resFile
        CleanupStack::PopAndDestroy(); // fs
        CleanupStack::PushL(readData);

        // now parse it and store the values in cmdTextArray
        TResourceReader reader;
        reader.SetBuffer(readData);
        const TInt count = reader.ReadInt16();
        TInt8 commandId;
        for (TInt i = 0; i < count; i++)
            {
            commandId = reader.ReadInt8();
            TCommand command;
            command.iDescription = reader.ReadHBufC16L();

            TCommand* commandPtr = NULL;
            commandPtr = aDescriptionArray->Find(commandId);
            if (commandPtr==NULL)
                {
                aDescriptionArray->Insert(commandId, command);
                //RDebug::Print(_L("!!HV: CAlfCommandDebug::ReadCommandDescriptionsL. INSERT: commandId=%d, command.iDescription=%S (0x%x)"), 
                //        commandId, command.iDescription, command.iDescription);
                }
            else
                {
                RDebug::Print(_L("!!HV: CAlfCommandDebug::ReadCommandDescriptionsL. ERROR. Duplicate command descriptions in the resource file! Skip the latter description. commandId=%d, command.iDescription=%S"),
                        commandId, command.iDescription); 
                delete command.iDescription;
                }
            }
        CleanupStack::PopAndDestroy(readData);
        }
    else
        {
        __ALFLOGSTRING1("CAlfRsSendBuffer::ConstructL - WARNING! Failed to read unsupported commands from resource file: %S! Error code:st %d.", &resourceFileName );
        CleanupStack::PopAndDestroy(); // fs
        }
    }

// ---------------------------------------------------------------------------
// SetHandle
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::SetHandle( const TInt& aHandle )
    {
    if (!iPrintHandles )
        {
        return;
        }
    iText.AppendFormat( _L("Handle: [%d] "), aHandle );
    }

// ---------------------------------------------------------------------------
// SetPoint
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::SetPoint( const TPoint& aPoint )
    {
    if (!iPrintPoints )
        {
        return;
        }
    iText.AppendFormat( _L("Point: (%d,%d) "), aPoint.iX, aPoint.iY );
    }

// ---------------------------------------------------------------------------
// SetRect
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::SetRect( const TRect& aRect )
    {
    if (!iPrintRects )
        {
        return;
        }
    iText.AppendFormat( _L("Rect: (%d,%d)-(%d,%d) "), aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY );
    }

// ---------------------------------------------------------------------------
// SetRegion
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::SetRegion( const TRegion& aRegion)
    {
    _LIT( KDescRegion, "Region of %d");
    if (!iPrintRegions )
        {
        return;
        }
    /*if ( iText.MaxLength() < iText.Length() + KDescRegion->Length() )
        {
        return;
        }*/
    iText.AppendFormat( KDescRegion, aRegion.Count() );
    for( TInt i = 0; i < aRegion.Count() ; i++ )
        {
        TRect rect = aRegion[i];
        iText.AppendFormat( _L("[%d]: Rect: (%d,%d)-(%d,%d) "), i, rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY );
        }
    }

// ---------------------------------------------------------------------------
// SetRegion
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::SetText( TPtr& aText )
    {
    if (!iPrintTexts )
        {
        return;
        }
    iText.AppendFormat( _L("Text: [%S] "), &aText );
    }

// ---------------------------------------------------------------------------
// SetColor
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::SetColor( TRgb& aColor )
    {
    if (!iPrintColors )
        {
        return;
        }
    iText.AppendFormat( _L("Color: R[%d]G[%d]B[%d] A[%d] "), aColor.Red(), aColor.Green(), aColor.Blue(), aColor.Alpha() );
    }

// ---------------------------------------------------------------------------
// Print
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::Print()
    {
    if ( iPrint )
        {
        RDebug::Print(_L("%S"), &iText );
        }
    }

// ---------------------------------------------------------------------------
// AdjustCommand
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::SetDescription(const TUint8& aCommand, TInt aDescriptionList )
    {
    TInt32 commandId = aCommand;
    TCommand* command = NULL;

    if ( aDescriptionList == R_ALF_COMMAND_DESCRIPTION_ARRAY )
        {
        command = iCommandDescriptions->Find(commandId);
        }
    else
        {
        // assuming, aDescriptionList is R_ALF_BRIDGE_COMMAND_DESCRIPTION_ARRAY
        command = iBridgeCommandDescriptions->Find(commandId);
        }
     iTotalCmdCount++;

     if (command )
            {
            HBufC16* buf = command->iDescription;
            iText.Format( _L("Cmd: %S - "), buf );
            }
        else
            {
            iText.Format( _L("Cmd: %d - "), aCommand );
            }
    }
// ---------------------------------------------------------------------------
// PrintDescription
//
// Note, you can enable/disable command at run-time in debugger by modifying 
// printCommands variable.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::SetDescriptionAndSize(const TUint8& aCommand,
        const TInt& aParametersSize, TInt aDescriptionList )
    {
    TInt32 commandId = aCommand;
    TCommand* command = NULL;

    if ( aDescriptionList == R_ALF_COMMAND_DESCRIPTION_ARRAY )
          {
          command = iCommandDescriptions->Find(commandId);
          }
      else
          {
          // assuming, aDescriptionList is R_ALF_BRIDGE_COMMAND_DESCRIPTION_ARRAY
          command = iBridgeCommandDescriptions->Find(commandId);
          }
      
    if ( command )
    	{
		command->iCount++;
		command->iSize = sizeof(TUint8) + aParametersSize;
		iTotalCmdSize += command->iSize;
    	}
    iTotalCmdCount++;
    
    if ( !iPrint )
        {
        // empty the string. Otherwise parameters start piling up to iText
        iText.Format(_L("")); 
        return;
        }
    
    if (command)
        {
        HBufC16* buf = command->iDescription;
        iText.Format( _L("Cmd: %S (%d/Size: %d) "), buf, aCommand, command->iSize );
        }
    else
        {
        iText.Format( _L("Cmd: %d"), aCommand );
        }

    }

// ---------------------------------------------------------------------------
// AdjustCommand
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::AdjustCommand(const TUint8& aCommand,
        TInt aAdjustment)
    {
    TCommand* command = NULL;

    command = iCommandDescriptions->Find(aCommand);

    if (command)
        {
        command->iCount--;

        iTotalCmdCount--;
        iTotalCmdSize += aAdjustment;
        }
    }

// ---------------------------------------------------------------------------
// StartFrame
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::StartFrame()
    {
    // clear statistics
    for (TInt i = 0; i < EAlfLastCommand; i++)
        {
        TCommand* command = iCommandDescriptions->Find(i);
        if (command)
            {
            command->iCount = 0;
            }
        }
    iTotalCmdCount = 0;
    iTotalCmdSize = 0;

    }

// ---------------------------------------------------------------------------
// EndFrame
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::EndFrame()
    {
    if (iPrintSummary)
        {
        RDebug::Print(_L("CAlfCommandDebug::EndFrame, Commands: %d, Size %d"), 
                iTotalCmdCount,
                iTotalCmdSize );
                
        }
    }

// ---------------------------------------------------------------------------
// PrintStatistic
//
// Note, you can enable iPrintStatistics at run-time in debugger  
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::PrintStatistic()
    {
    if (iPrintStatistics && iTotalCmdSize > iLargeBufferDefinition )
        {
        for (TInt i = 0; i < EAlfLastCommand; i++)
            {
            TCommand* command = iCommandDescriptions->Find(i);
            if (command)
                {
                TInt totalSize = command->iCount * command->iSize;
                RDebug::Print(
                        _L("Cmd: %S - Count: %d - Size: %d (%d)"), command->iDescription,
                        command->iCount, totalSize, command->iSize);
                }
            }
        iFrameTotalSize += iTotalCmdSize;
        iFrameCount++;
        TInt averageFrame = iFrameTotalSize / iFrameCount;
        RDebug::Print(_L("Total, Count: %d, Size %d (average: %d)"), iTotalCmdCount,
                iTotalCmdSize, averageFrame);
        }
    }

// ---------------------------------------------------------------------------
// SetPrint
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCommandDebug::SetPrint( TBool aPrint )
    {
    iPrint = aPrint;
    }

// ---------------------------------------------------------------------------
// DebugPrintControlGroupOrder
// Can be used only from CAlfBridge
// ---------------------------------------------------------------------------
// 
EXPORT_C TFileName CAlfCommandDebug::WServClientFileName( TInt aClientWindowGroupId, RWsSession& aSession )
    {
    
    TFileName processName;
    if( aClientWindowGroupId != KErrNotFound || aClientWindowGroupId != 0 )
        {
        TThreadId threadId;
        TInt error = aSession.GetWindowGroupClientThreadId( aClientWindowGroupId, threadId );
        if( error )
            {
            }
        else
            {
            RThread thread;
            TInt err = thread.Open( threadId );
            if( !err )
                {
                RProcess process;
                err = thread.Process( process );
                if( !err )
                    {
                    processName = process.FileName();
                    }
                process.Close();
                }
            thread.Close();
            }
        }        
    return processName;
    }

// ---------------------------------------------------------------------------
// TrackProcess
// ---------------------------------------------------------------------------
// 
EXPORT_C TBool CAlfCommandDebug::TrackProcess( HBufC16* aFileName )
    {
    // this is separate function to allow in the future tracking of multiple processes
    return aFileName->Compare( iTrackedProcess ) == 0;
    }

// ---------------------------------------------------------------------------
// Text
// ---------------------------------------------------------------------------
// 
EXPORT_C TDesC16& CAlfCommandDebug::Text()
    {
    return iText;
    }
