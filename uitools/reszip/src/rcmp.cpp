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
* Description:
*
*/
// RCMP.CPP
//
//

#include <e32svr.h>
#include <e32cons.h>
#include <s32file.h>
#include <s32mem.h>


#include "rescomp.h"
#include "resdict.h"
#include "resentry.h"

const TInt KConsWidth = 20;
const TInt KConsHeight = 15;

// TOOL version string (will be printed on command line so please keep upto date)
_LIT(KReszipVersionString, "reszip version 2.0.0");

LOCAL_D CConsoleBase* console;
LOCAL_D RFs TheFs;
LOCAL_D CTrapCleanup* TheTrapCleanup;

LOCAL_D CArrayFixFlat<TFileName>* iFileList;

TBuf<256> Arg;


_LIT(KUncompressedExt,".orig");

LOCAL_C void setupCleanup()
//
// Initialise the cleanup stack.
//
    {
	TheTrapCleanup=CTrapCleanup::New();
	}

void AddToFileListL(const TDesC& aName);


void AddFileListL(const TDesC& aName)
	{
	RDebug::Print(_L("Adding files from "));
	RDebug::Print(aName);
	
	RFileReadStream stream;
	User::LeaveIfError( stream.Open(TheFs, aName, EFileRead) );
	CleanupClosePushL(stream);

	TBuf<512> buf;
	TInt ch = 0;
	TInt err = KErrNone;
	while(err == KErrNone)
		{
		TRAP(err, ch = stream.ReadUint8L());
		if (err == KErrNone)
			{
			if (ch != 13 && ch != 10)
				{
				buf.Append(ch);
				}
			else
				{
				AddToFileListL(buf);
				buf.Zero();
				}
			}
		}

	CleanupStack::PopAndDestroy();	// stream
	}

void AddToFileListL(const TDesC& aName)
	{
	if (aName.Length() == 0)
		{
		return;
		}
	RDebug::Print(aName);

	if (aName[0] == '@')
		{
		AddFileListL(aName.Right(aName.Length()-1));
		return;
		}

	TParse parse;
	if (parse.Set(aName, NULL, NULL) != KErrNone)
		{
		return;
		}
	TFileName name(parse.NameAndExt());
	TPtrC driveAndPath(parse.DriveAndPath());

	CDir* fileList = NULL; 
	CDir* dirList = NULL; 


	// Report any missing files (not fatal, as we don't want to stop the list)
	TEntry entry;
	if((!parse.IsWild()) && TheFs.Entry(aName,entry) != KErrNone)
		console->Printf(_L("Error: Cannot find %S\n"), &aName);
			

	TInt err = TheFs.GetDir(aName, KEntryAttNormal,ESortByName,fileList,dirList);	
	if (err != KErrNone)
		{
		delete fileList;
		delete dirList;
		return;
		}

	TInt count = fileList->Count();

	_LIT(KString,"%S\n"); 

	TInt appendErr = KErrNone;
	for (TInt i=0;i<count;i++) 
		{
		TFileName fullName(driveAndPath);
		fullName.Append((*fileList)[i].iName);
		TRAPD(appendErr, iFileList->AppendL(fullName) );
		if(appendErr!=KErrNone)
			break;
		}

	delete fileList;
	delete dirList;
	User::LeaveIfError(appendErr);
	}



void ProcessAllFilesL()
	{
	TInt totalSize = 0;
	TInt totalCompressed = 0;

	if (Arg.Length() == 0)
		{
		// Write help information
		console->Printf(_L("ResZip ROM Resource Compressor (%S)\n------------------------------\n"), &KReszipVersionString);
		console->Printf(_L("Syntax: ResZip [full path to resource files]\n"));
		console->Printf(_L("        ResZip [@full path to text file containing resource filenames]\n"));
		console->Printf(_L("e.g: ResZip r:\\epoc32\\release\\wins\\udeb\\z\\system\\data\\*.rsc\n"));
		console->Printf(_L("     ResZip @r:\\files.txt\n"));
		console->Printf(_L("         where r:\\files.txt contains list of filenames (wildcards allowed)\n"));
		console->Printf(_L("Full Path may include wildcards for compressing all resources in a directory\n"));
		console->Printf(_L("May be used to compress non-ROM resources, with lower run-time performance\n"));
		return;
		}

	iFileList = new(ELeave)CArrayFixFlat<TFileName>(32);

	AddToFileListL(Arg);

	TInt filecount = iFileList->Count();

	if (filecount == 0)
		{
		console->Printf(_L("No files to process\n"));
		}
	else
		{
		TBuf<80> buf;
		buf.Format(_L("Processing %d files\n"), filecount);
		console->Printf(buf);
		console->Printf(_L("Name\t\t\t Resource\tOrig\tUncompressed\tNew\t%%\tDictSize\n"));
		console->Printf(_L("====\t\t\t ========\t====\t============\t===\t==\t========\n"));
		}

	TTime startTime;
	startTime.UniversalTime();

	for (TInt ii=0; ii<filecount; ii++)
		{

		CResComp* compressor = new(ELeave)CResComp(console);
		CleanupStack::PushL(compressor);

		TFileName fullName(iFileList->At(ii));
		compressor->LoadFileL(TheFs, fullName);
		compressor->CompressL();

		if (!compressor->IsValid())
			{
			console->Printf(_L("\r"));
			console->Printf(compressor->iConOutput);
			console->Printf(_L("Not compressed\n"));
			}
		else
			{
			compressor->DisplayStats();
			totalSize += compressor->OriginalFileSize();
			totalCompressed += compressor->CompressedSize();
			TFileName newName(fullName);
			newName.Append(KUncompressedExt);
			TheFs.Delete(newName);
			User::LeaveIfError(TheFs.Rename(fullName, newName));
			TRAPD(err, compressor->WriteFileL(TheFs,fullName));
			if(err != KErrNone)
				{
				TheFs.Delete(fullName);
				TheFs.Rename(newName, fullName);
				User::Leave(err);
				}
			}

		CleanupStack::PopAndDestroy(compressor);	
		}

	console->Printf(_L("------------------------------\n"));
	console->Printf(_L("Total File Size: %d bytes\n"), totalSize);
	console->Printf(_L("Total Compressed Size: %d bytes\n"), totalCompressed);

	if (totalSize > 0)
		{
		TInt compression = ((totalCompressed) * 100) / totalSize;
		console->Printf(_L("Total Compression = %d %%\n"), compression);
		}

	TTime endTime;
	endTime.UniversalTime();

	TTimeIntervalSeconds seconds;
	endTime.SecondsFrom(startTime, seconds);

	TInt secs = seconds.Int();
	
	TInt mins = secs / 60;
	TInt hour = mins / 60;

	secs -= (mins * 60);
	mins -= (hour * 60);

	TBuf<80> time;
	time.Format(_L("Total time: %02d:%02d:%02d"), hour, mins, secs);
	console->Printf(time);

	delete iFileList;
	}


void DoConsoleL()
	{
	console=Console::NewL(_L("EPOC32 - reszip"),
		TSize(KConsWidth,KConsHeight));
	CleanupStack::PushL(console);
	//
	User::CommandLine(Arg);
	//
	User::LeaveIfError( TheFs.Connect() );
	//
	TRAPD(r,ProcessAllFilesL());

	if (r!=KErrNone)
		console->Printf(_L("failed with code %d\n"), r);
	CleanupStack::PopAndDestroy(); // console
	TheFs.Close();
	}

GLDEF_C TInt E32Main()
	{
	setupCleanup();
	TRAPD(err,DoConsoleL());
	delete TheTrapCleanup;
	return KErrNone;
	}
