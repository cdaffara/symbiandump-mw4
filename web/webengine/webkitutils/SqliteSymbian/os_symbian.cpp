/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#ifdef  OS_SYMBIAN

#ifdef __cplusplus
extern "C" {
#endif

#include "sqliteInt.h"
#include "os.h"

#ifdef __cplusplus
}  /* End of the 'extern "C"' block */
#endif

#include "os_common.h"
#include "os_symbian.h"

#include <f32file.h>
#include <e32math.h>
#include <hal.h>
#include <bautils.h>

///////////////////////////////////////////////////////////////////////////////////////////

//Panic category - used by asserts in this file.
_LIT(KPanicCategory, "SqliteSymbian");

//Panic codes - used by asserts in this file.
enum TPanicCodes
    {
    EPanicNullTlsPtr            = 1,
    EPanicInvalidWAmount        = 2,
    EPanicOffset64bit           = 3,
    EPanicInvalidLockType1      = 4,
    EPanicInvalidLockType2      = 5,
    EPanicInvalidLockType3      = 6,
    EPanicInvalidLockType4      = 7,
    EPanicInvalidLockType5      = 8,
    EPanicInvalidLockType6      = 9,
    EPanicInvalidLockType7      =10,
    EPanicInvalidOpType         =11,
    EPanicInvalidFhStr          =12,
    EPanicInvalidFhData         =13
    };

//Define __COUNT_FILE_IO__ if want to get information about the file I/O 
//operations count and the time spent in file I/O operations.
#ifdef __COUNT_FILE_IO__

//File I/O counters.
static TDbFileIOCounters TheDbFileIOCounters;

/**
Resets all file I/O counters.

This function is part of Symbian OS specific SQLITE API.

@internalComponent
*/
void sqlite3SymbianZeroDbFileIOCounters()
    {
    Mem::FillZ(&TheDbFileIOCounters, sizeof(TheDbFileIOCounters));
    }

/**
Copies file I/O counters to the place pointed by aDbFileIOCounters argument.

@param aDbFileIOCounters Output parameter. The place where file I/O counters will be copied.

This function is part of Symbian OS specific SQLITE API.

@internalComponent
*/
void sqlite3SymbianGetDbFileIOCounters(TDbFileIOCounters& aDbFileIOCounters)
    {
    aDbFileIOCounters = TheDbFileIOCounters;
    }

//The following macros are used for managing file I/O counter values.
//They are evaluated to nothing if __COUNT_FILE_IO__ is not defined.
#define INC_COUNTER(cnt)            ++cnt
#define GET_TIME(start)             TUint32 start = User::FastCounter()
#define ADD_TIME_DIFF(start, var)   var += (User::FastCounter() - start);

#else//__COUNT_FILE_IO__  //////////////////////////////////////////

//The following macros are used for managing file I/O counter values.
//They are evaluated to nothing if __COUNT_FILE_IO__ is not defined.
#define INC_COUNTER(cnt)            void(0)
#define GET_TIME(start)             void(0)
#define ADD_TIME_DIFF(start, var)   void(0)

#endif//__COUNT_FILE_IO__

///////////////////////////////////////////////////////////////////////////////////////////

//If the following global variable points to a string which is the
//name of a directory, then that directory will be used to store
//temporary files.
//
//"PRAGMA temp_store_directory <dir_path>" command will set it!
//This is a potential platform security hole and the temp dir path
//is not used in os_symbian.cpp file. The temporaty files will be created
//in the process's private data cage.
//
//There is a memory leak in SQLite if "PRAGMA temp_store_directory <dir_path>" is used,
//because sqlite3_temp_directory stays undeleted when the program terminates.
//So, it will be deleted in sqlite3SymbianFsClose() method.
char *sqlite3_temp_directory;

//Current thread's heap. Won't work in a mutithreaded environment.
//Stored as a global variable to reduce Exec::Heap() calls count (made from User::Alloc()).
static RAllocator* TheAllocator = NULL;

///////////////////////////////////////////////////////////////////////////////////////////

// Static variables used for thread synchronization
static TInt     TheMutexRefCounter = 0;

//Zeroed thread data instance.
const ThreadData TheZeroThreadData = {0};

//Const buffer used for generating temporary file names.
//No lower case letters in the buffer because the OS won't make difference between lower and upper case in file names.
const char TheChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////       TTlsData class - declaration, definition    ///////////////////
///////////////////////////////////////////////////////////////////////////////////////////

//TTlsData class manages a per-thread copy of the following data: 
//  - file session instance;
//  - process's private data path, where the temporary file will be stored (on Drive C);
//  - last OS error code, every Symbian OS API call will set it;
//  - stored OS error code, initialized with the last OS error code only if stored OS error code is KErrNone.
//      Each StoredOsErrorCode() call will reset it to KErrNone;
//  - reference counter. Only one TTlsData instance gets created per thread;
//
//The reason of having two data members for storing the OS error codes is that if there is just one variable
//and it gets initialized with the error value reported by a failed OS API call, the next successful OS API
//call will reset it and the TTlsData client will miss the last "real" OS API error.
class TTlsData
    {
public: 
    static TInt Create();
    static void Release();
    static TTlsData& Instance();
    
    TInt SetOsErrorCode(TInt aError);
    inline TInt StoredOsErrorCode();
    
    inline void StoreFhData(const RMessage2* aMsg, TBool aReadOnly);
    inline void RetrieveAndResetFhData(const RMessage2*& aMsg, TBool& aReadOnly);
    
private:
    TTlsData();
    ~TTlsData();
    TInt DoCreate();
    
public:
    //File session instance.
    RFs         iFs;
    //"<system drive>:\" + process's private data path. Initialized in sqlite3SymbianFsOpen().
    //Used for storing sqlite temporary files.
    TFileName   iSysPrivDir;
    TParse      iParse;
    
private:    
    //Contains the last OS error code.
    TInt                iStoredOsErrorCode;
    //Counts the number of times when an attempt has been made to create TTlsData instance.
    //TTlsData instance is created just once. This counter is useless in a true client-server
    //environment, but is very usefull when testing the prototype.
    TInt                iRefCounter;
    //Fh data
    const RMessage2*    iMessage;
    TBool               iReadOnly;  
    };

//Returns the address of TlsHandle1() function. Used as a unique handle when calling UserSvr::DllTls()
//and UserSvr::DllSetTls(). 
//So, if the SQL server is a single threaded process, there is a need of only one TLS instance, identified
//uniquely by TlsHandle1() address.
static TInt TlsHandle1()
    {
    return reinterpret_cast <TInt> (&TlsHandle1);
    }

//Checks if the TTlsData instance exists and if not the method creates it and stores it in the TLS.
//The TTlsData instance reference counter is incremented.
TInt TTlsData::Create()
    {
    TTlsData* data = static_cast <TTlsData*> (UserSvr::DllTls(TlsHandle1()));
    if(!data)
        {
        data = new TTlsData;
        if(!data)
            {
            return KErrNoMemory;    
            }
        TInt err = data->DoCreate();
        if(err == KErrNone)
            {
            err = UserSvr::DllSetTls(TlsHandle1(), data);
            }
        if(err != KErrNone)
            {
            delete data;
            return err;
            }
        }
    ++data->iRefCounter;
    //Do not delete "data"! The TLS keeps a pointer to it.
    return KErrNone;
    }

//Destroys the TTlsData instance if it exists and the decremented reference counter reaches 0.
void TTlsData::Release()
    {
    TTlsData* data = static_cast <TTlsData*> (UserSvr::DllTls(TlsHandle1()));
    if(data && !--data->iRefCounter)
        {
        sqliteFree((void*)sqlite3_temp_directory);
        sqlite3_temp_directory = 0;
        sqlite3_thread_cleanup();
        delete data;
        (void)UserSvr::DllSetTls(TlsHandle1(), 0);
        }
    }

//Returns a reference to the TTlsData instance. 
TTlsData& TTlsData::Instance()
    {
    TTlsData* data = static_cast <TTlsData*> (UserSvr::DllTls(TlsHandle1()));
    if (data == NULL) {
        sqlite3SymbianLibInit();
        data = static_cast <TTlsData*> (UserSvr::DllTls(TlsHandle1()));
    }
    __ASSERT_ALWAYS(data != NULL, User::Panic(KPanicCategory, EPanicNullTlsPtr));
    return *data;
    }

//Sets the last OS error code data member. The stored OS error code data member will be set only if it is
//KErrNone. (If it is not KErrNone it means that its value has not been accessed yet)
//An exception from the rule described above is KErrDiskFull error which, if happens, will be set always, because
//this error has a special meaning for the database clients - special actions may have to be taken if the
//disk is full.
//If aError is KErrNoMemory SQLITE will be notified with a sqlite3FailedMalloc() call.
TInt TTlsData::SetOsErrorCode(TInt aError)
    {
    if(aError == KErrNoMemory)
        {
        sqlite3FailedMalloc();          
        }
    if(iStoredOsErrorCode == KErrNone || aError == KErrDiskFull)
        {
        iStoredOsErrorCode = aError;
        }
    return aError;
    }

//Returns the last stored OS error code, which was stored by SetOsErrorCode() call.
//The function also resets the stored OS error code to KErrNone.
inline TInt TTlsData::StoredOsErrorCode()
    {
    TInt err = iStoredOsErrorCode;
    iStoredOsErrorCode = KErrNone;
    return err;
    }

//Stores RMessage2 address, file and file session handles and the read-only flag for use when SQLITE issues a 
//request for open the database file. 
inline void TTlsData::StoreFhData(const RMessage2* aMsg, TBool aReadOnly)
    {
    iMessage = aMsg;
    iReadOnly = aReadOnly;
    }

//Retrieves RMessage2 address, file and file session handles. The stored data will be reset.    
inline void TTlsData::RetrieveAndResetFhData(const RMessage2*& aMsg, TBool& aReadOnly)
    {
    __ASSERT_DEBUG(iMessage != NULL, User::Panic(KPanicCategory, EPanicInvalidFhData));
    aMsg = iMessage; 
    aReadOnly = iReadOnly;
    iMessage = NULL;
    }

//Reference counter is set to 0.
TTlsData::TTlsData() :
    iStoredOsErrorCode(KErrNone),
    iRefCounter(0),
    iMessage(0),
    iReadOnly(EFalse)   
    {
    }

//Closes the file session.
TTlsData::~TTlsData()
    {
    iFs.Close();    
    }

//Creates per-thread file session instance. 
//Creates the private path, where the temporary files will be stored. (on the system drive)
TInt TTlsData::DoCreate()
    {
    TInt err = iFs.Connect();
    if(err != KErrNone)
        {
        return err; 
        }
    //Get the system drive
    TDriveNumber drv;
    err = BaflUtils::GetSystemDrive( drv );
    if(err == KErrNotFound )
        {
        drv = EDriveC;
        err = KErrNone;
        }
    if( err == KErrNone )
        {
        TInt sysDrive = static_cast<TInt>(drv);
        if((err = iFs.CreatePrivatePath(sysDrive)) != KErrNone && err != KErrAlreadyExists)
            {
            return err; 
            }
        TFileName privateDir;
        if((err = iFs.PrivatePath(privateDir)) != KErrNone)
            {
            return err; 
            }
        TDriveUnit drive(sysDrive);
        TDriveName driveName = drive.Name();
        (void)iParse.Set(driveName, &privateDir, 0);//this call can't fail
        iSysPrivDir.Copy(iParse.DriveAndPath());
        return KErrNone;
        }
    return err;    
    }

///////////////////////////////////////////////////////////////////////////////////////////

//aFileName argument is expected to point to UTF8 encoded, zero terminated string.
//The function converts aFileName to UTF16 encoded file name, and stores the UTF16 encoded file name
//to the place pointed by aFileNameDestBuf argument.
//If the UTF16 conversion of the file name failed because the file name is too long or NULL, 
//the function returns EFalse. 
//Max allowed aFileName length is KMaxFileName (excluding terminating 0 character).
//aFileNameDestBuf max length must be at least KMaxFileName characters.
//
//aFileNameDestBuf will hold UTF16, non-zero-terminated string
static TBool ConvertToUnicode(const char *aFileName, TDes& aFileNameDestBuf)
    {
    if(aFileName)
        {
        wchar_t* dest = reinterpret_cast <wchar_t*> (const_cast <TUint16*> (aFileNameDestBuf.Ptr()));
        TInt len = mbstowcs(dest, aFileName, KMaxFileName);
        //Check the file name length. If it is longer than KMaxFileName characters, then the file name is not valid.
        if(len > 0 && len <= KMaxFileName)
            {
            aFileNameDestBuf.SetLength(len);
            return ETrue;
            }
        }
    return EFalse;
    }

//aFileName argument is expected to point to UTF16 encoded, zero terminated string.
//The function converts aFileName to UTF8 encoded file name, and stores the UTF8 encoded file name
//to the place pointed by aFileNameDestBuf argument.
//If the UTF8 conversion of the file name failed because the file name is too long or NULL, 
//the function returns EFalse. 
//Max allowed aFileName length is KMaxFileName (excluding terminating 0 character).
//aFileNameDestBuf max length must be at least KMaxFileName characters.
//
//aFileNameDestBuf will hold UTF8, non-zero-terminated string
static TBool ConvertFromUnicode(const TDesC& aFileName, TDes8& aFileNameDestBuf)
    {
    char* dest = reinterpret_cast <char*> (const_cast <TUint8*> (aFileNameDestBuf.Ptr()));
    const wchar_t* src = reinterpret_cast <const wchar_t*> (aFileName.Ptr());
    TInt len = wcstombs(dest, src, KMaxFileName);
    //Check the file name length. If it is longer than KMaxFileName characters, then the file name is not valid.
    if(len > 0 && len <= KMaxFileName)
        {
        aFileNameDestBuf.SetLength(len);
        return ETrue;
        }
    return EFalse;
    }

///////////////////////////////////////////////////////////////////////////////////////////
//////////////////          File name, containing handles, related   //////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

const char KFhSeparator = '|';  //The symbol, which when used in the file name means that the string does not contain a real file name but file handles
const TInt KFhSessHandleIdx = 2;//The index of the file session handle in RMessage2 object
const TInt KFhFileHandleIdx = 3;//The index of the file handle in RMessage2 object
const TInt KFhReconPos = 0;     //if the symbol in this position KFhSeparator, then the string contains file handles
const TInt KFhRoPos = 1;        //read-only flag position in the string
const TInt KFhMsgAddrPos = 2;   //RMessage2 address position in the string
const TInt KFhMsgAddrLen = 8;   //RMessage2 address length
//const TInt KFhDrivePos = 1;   //Drive position in the string (after removing the read-only flag and RMessage2 object's address)

//Possible file name string types.
enum TFhStrType
    {
    ENotFhStr,                      //The string does not contain file handles
    EFhStr,                         //The string contain file handles, but is not main db file
    EFhMainDbStr                    //The string contain file handles and is the main db file
    };

//Returns:
// - ENotFhStr    - aFileName is a normal file name
// - EFhMainDbStr - aFile name contains the handle of a database file opened on the client side (and some other relevant information)
// - EFhStr       - aFileName is a journal file name for example. The main database file was opened from a handle in this case.
static TFhStrType FhStringProps(const char* aFileName)
    {
    char* first = strchr(aFileName, KFhSeparator);
    if(!first)
        {
        return ENotFhStr;
        }
    char* last = strchr(first + 1, KFhSeparator);
    if(!last)
        {
        return ENotFhStr;
        }
    return *(last + 1) == 0 ? EFhMainDbStr : EFhStr;
    }

//Replaces all invalid characters in aFileName.
static void FhConvertToFileName(TDes& aFileName, TParse& aParse, const TDesC& aPrivateDir)
    {
    TInt firstPos = aFileName.Locate(TChar(KFhSeparator));
    if(firstPos >= 0)
        {
        aFileName.Delete(firstPos, 1);
        TInt lastPos = aFileName.LocateReverse(TChar(KFhSeparator));
        if(lastPos >= 0)
            {
            aFileName.Delete(lastPos, 1);
            (void)aParse.Set(aFileName, &aPrivateDir, 0);//the file name should be verified by the server
            aFileName.Copy(aParse.FullName());
            }
        }
    }

//Extracts read-only flag and RMessage address from aDbFileName and stores them in TLS.
//aDbFileName will be reformatted and won't contain the already extracted data.
//aDbFileName format is:
//      |<R/O flag><RMessage2 pointer><drive><app SID><file_name><file_ext>|
static void FhExtractAndStore(TDes& aDbFileName, TTlsData& aTls)
    {
    TInt fhStartPos = aDbFileName.Locate(TChar(KFhSeparator));
    __ASSERT_DEBUG(fhStartPos == KFhReconPos, User::Panic(KPanicCategory, EPanicInvalidFhStr));
    //If this file name string contains file handles
    if(fhStartPos == KFhReconPos)
        {
        //Extract from aDbFileName string RMessage2 object's address
        TLex lex(aDbFileName.Mid(fhStartPos + KFhMsgAddrPos, KFhMsgAddrLen));
        TUint32 addr;
        TInt err = lex.Val(addr, EHex);
        __ASSERT_DEBUG(err == KErrNone, User::Panic(KPanicCategory, EPanicInvalidFhStr));
        if(err == KErrNone)
            {
            //Cast the address to RMessage2 pointer.
            const RMessage2* msg = reinterpret_cast <const RMessage2*> (addr);
            __ASSERT_DEBUG(msg != NULL, User::Panic(KPanicCategory, EPanicInvalidFhStr));
            if(msg)
                {
                //Store the data from aDbFileName in TLS
                TBool readOnly = aDbFileName[fhStartPos + KFhRoPos] > '0';
                aTls.StoreFhData(msg, readOnly);
                //Remove: read-only flag and RMessage2 object's address
                aDbFileName.Delete(fhStartPos + KFhRoPos, 1 + KFhMsgAddrLen);
                }
            }
        }
    }

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////     Symbian OS specific SQLITE API         ////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/**
Returns the last OS error which occured durring the operations with the database files.
The per-thread variable, where the last OS error is hold, will be set to KErrNone.

This function is part of Symbian OS specific SQLITE API.

@return The last OS error.
@internalComponent
*/
EXPORT_C TInt sqlite3SymbianLastOsError(void)
    {
    TTlsData& tls = TTlsData::Instance();
    return tls.StoredOsErrorCode();
    }

/**
This function must be called once before any other SQLITE API call. It does Symbian OS specific
initialization. Each sqlite3SymbianLibInit() call must be paired with a sqlite3SymbianLibFinalize() call
when finishing working with SQLITE.

This function is part of Symbian OS specific SQLITE API.

@return Symbian OS specific error code, including KErrNoMemory.
@internalComponent
*/
EXPORT_C TInt sqlite3SymbianLibInit(void)
    {
    TheAllocator = &User::Allocator();
    return TTlsData::Create();
    }

/**
This function must be called once after finishing working with sqlite.

This function is part of Symbian OS specific SQLITE API.

@internalComponent
*/
EXPORT_C void sqlite3SymbianLibFinalize(void)
    {
    TTlsData::Release();
    TheAllocator = NULL;
    }

/**
This function is part of Symbian OS specific SQLITE API.

@return A reference to RFs instance used for sqlite file I/O operations.
@internalComponent
*/
EXPORT_C RFs& sqlite3SymbianFs(void)
    {
    return TTlsData::Instance().iFs;
    }

///////////////////////////////////////////////////////////////////////////////////////////

//Attempt to open a file descriptor for the directory that contains a
//file.  This file descriptor can be used to fsync() the directory
//in order to make sure the creation of a new file is actually written
//to disk.
//
//This routine is only meaningful for Unix.  It is a no-op under
//Symbian OS since Symbian OS does not support hard links.
//
//On success, a handle for a previously open file is at *aOsFile is
//updated with the new directory file descriptor and SQLITE_OK is
//returned.
//
//On failure, the function returns SQLITE_CANTOPEN and leaves
//*aOsFile unchanged.
int sqlite3SymbianOpenDirectory(OsFile* /*aOsFile*/, const char * /*aDirName*/)
    {
    return SQLITE_OK;
    }

///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////       CDbFile class - declaration, definition    ////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

// CDbFile class has a set of methods and data members used for the file I/O operations performed by SQLITE.
// Following the recomendations made in the comments of OsFile structure in os.h file, CDbFile was declared
// as a class derived from OsFile.
// The class consists of two sets of methods:
// - Non-static methods (most of them inlined). They just forward the call to the related RFile method call.
// - Static methods. Used for the initialization of OsFile::pMethod data member (IoMethod structure).
// The class also holds information about the current file lock type and a flag indicating should the
// file be deleted after closing it.
class CDbFile : public OsFile
    {
public: 
    static CDbFile* New();
    inline TInt Create(const TDesC& aFileName);
    inline TInt CreateExclusive(const TDesC& aFileName, TBool aDeleteOnClose);
    inline TInt Open(const TDesC& aFileName);
    inline TInt OpenReadOnly(const TDesC& aFileName);
    inline TInt OpenFromHandle(const RMessage2& aMsg);
    inline TInt Read(TDes8& aDes, TInt aLength);
    inline TInt Write(const TDesC8& aData, TInt aLength);
    inline TInt Size(TInt& aSize) const;
    inline TInt SetSize(TInt aSize);
    inline TInt Flush();
    ~CDbFile();
    
    inline TInt Handle() const;
    inline void SetLockType(TInt aLockType);
    inline TInt LockType() const;

    //"IoMethod" methods
    static int Close(OsFile** aOsFile);
    static int Read(OsFile* aOsFile, void*, int amt);
    static int Write(OsFile* aOsFile, const void*, int amt);
    static int Seek(OsFile* aOsFile, i64 offset);
    static int Truncate(OsFile* aOsFile, i64 size);
    static int Sync(OsFile* aOsFile, int);
    static void SetFullSync(OsFile* aOsFile, int setting);
    static int FileHandle(OsFile* aOsFile);
    static int FileSize(OsFile* aOsFile, i64 *pSize);
    static int Lock(OsFile* aOsFile, int aLockType);
    static int Unlock(OsFile* aOsFile, int);
    static int LockState(OsFile* aOsFile);
    static int CheckReservedLock(OsFile* aOsFile);
    /////
    
private:
    CDbFile();  
    static inline CDbFile& Instance(void* aDbFile);

private:    
    RFile   iFile;
    HBufC*  iFileToBeDeleted;//Not NULL if CDbFile is a temporary file and will be deleted when closed
    TInt    iLockType;      // Type of lock currently held on this file: NO_LOCK, SHARED_LOCK, 
                            // RESERVED_LOCK, PENDING_LOCK, EXCLUSIVE_LOCK
    TInt    iFilePos;
    TInt    iFileSize;
    };

#define CREATE_FILE_MODE()              EFileRead | EFileWrite
#define CREATE_FILE_EXCLUSIVE_MODE()    EFileRead | EFileWrite
#define OPEN_FILE_SHARED_MODE()         EFileRead | EFileWrite
#define OPEN_FILE_READONLY_MODE()       EFileRead
    
//Creates non-initializad CDbFile instance.
CDbFile* CDbFile::New()
    {
    return new CDbFile;
    }

//Creates a file with aFileName. The file will be created for shared reading/writing.
//This call initializes CDbFile instance.
inline TInt CDbFile::Create(const TDesC& aFileName)
    {
    return iFile.Create(TTlsData::Instance().iFs, aFileName, CREATE_FILE_MODE());
    }
    
//Creates a file with aFileName. The file will be created for exclusive reading/writing.
//This call initializes CDbFile instance.
//The function may return KErrNoMemory.
inline TInt CDbFile::CreateExclusive(const TDesC& aFileName, TBool aDeleteOnClose)
    {
    if(aDeleteOnClose)
        {
        iFileToBeDeleted = aFileName.Alloc();
        if(!iFileToBeDeleted)
            {
            return KErrNoMemory;    
            }
        }
    return iFile.Create(TTlsData::Instance().iFs, aFileName, CREATE_FILE_EXCLUSIVE_MODE());
    }
    
//Opens a file with aFileName. The file will be opened for shared reading/writing.
//This call initializes CDbFile instance.
inline TInt CDbFile::Open(const TDesC& aFileName)
    {
    TInt err = iFile.Open(TTlsData::Instance().iFs, aFileName, OPEN_FILE_SHARED_MODE());
    if(err == KErrNone)
        {
        err = Size(iFileSize);
        }
    return err;
    }
    
//Opens a file with aFileName. The file will be opened in shared read-only mode.
//This call initializes CDbFile instance.
inline TInt CDbFile::OpenReadOnly(const TDesC& aFileName)
    {
    TInt err = iFile.Open(TTlsData::Instance().iFs, aFileName, OPEN_FILE_READONLY_MODE());
    if(err == KErrNone)
        {
        err = Size(iFileSize);
        }
    return err;
    }

//Opens database file from handle
inline TInt CDbFile::OpenFromHandle(const RMessage2& aMsg)
    {
    TInt err = iFile.AdoptFromClient(aMsg, KFhSessHandleIdx, KFhFileHandleIdx);
    if(err == KErrNone)
        {
        err = Size(iFileSize);
        }
    return err;
    }

//Reads aLength bytes from the file and stores them to the place pointed by aDes argument.
inline TInt CDbFile::Read(TDes8& aDes, TInt aLength)
    {
    TInt err = iFile.Read(iFilePos, aDes, aLength);
    if(err == KErrNone)
        {
        if(aDes.Length() < aLength)
            {
            err = KErrEof;  
            }
        else
            {
            iFilePos += aLength;
            }
        }
    return err;
    }

//Writes aLength bytes to the file usign as data source aData argument.
inline TInt CDbFile::Write(const TDesC8& aData, TInt aLength)
    {
    TInt err = iFile.Write(iFilePos, aData, aLength);
    if(err == KErrNone)
        {
        iFilePos += aLength;
        if(iFilePos > iFileSize)
            {
            iFileSize = iFilePos;   
            }
        }
    return err;
    }

//Returns the file size in bytes.
inline TInt CDbFile::Size(TInt& aSize) const
    {
    return iFile.Size(aSize);
    }
    
//Sets the file size. aSize - in bytes.
//Not that if SetSize() truncates the file, iFilePos may become invalid!
inline TInt CDbFile::SetSize(TInt aSize)
    {
    return iFile.SetSize(aSize);
    }

//Flushes all unwritten file buffers to the file.   
inline TInt CDbFile::Flush()
    {
    return iFile.Flush();
    }

//Closes the file. If iFileToBeDeleted is not NULL, the file will be deleted.
CDbFile::~CDbFile()
    {
    iFile.Close();
    if(iFileToBeDeleted)
        {
        GET_TIME(start);
        (void)TTlsData::Instance().iFs.Delete(*iFileToBeDeleted);
        ADD_TIME_DIFF(start, TheDbFileIOCounters.iDeleteOpTime);
        INC_COUNTER(TheDbFileIOCounters.iDeleteOpCnt);
        delete iFileToBeDeleted;
        }
    }

//Returns the file handle. Used for debug purposes only.
inline TInt CDbFile::Handle() const
    {
    return iFile.SubSessionHandle();    
    }

//Sets the file lock type.
inline void CDbFile::SetLockType(TInt aLockType)
    {
    iLockType = aLockType;
    }
    
//Returns the current file lock type.
inline TInt CDbFile::LockType() const
    {
    return iLockType;
    }

//Closes the file. The file will be deleted if it has been marked for deletion (iFileToBeDeleted not NULL).
int CDbFile::Close(OsFile** aOsFile)
    {
    if(aOsFile)
        {
        CDbFile* dbFile = static_cast <CDbFile*> (*aOsFile);
        TRACE2("CLOSE %X\n", dbFile->Handle());
        
        GET_TIME(start);
        delete dbFile;
        ADD_TIME_DIFF(start, TheDbFileIOCounters.iCloseOpTime);
        INC_COUNTER(TheDbFileIOCounters.iCloseOpCnt);
        
        OpenCounter(-1);
        *aOsFile = 0;
        }
    return SQLITE_OK;
    }

//Read data from a file into a buffer.  Return SQLITE_OK if all
//bytes were read successfully and SQLITE_IOERR if anything goes
//wrong.
int CDbFile::Read(OsFile *aOsFile, void *aBuf, int aAmt)
    {
    TTlsData& tls = TTlsData::Instance();
    CDbFile& dbFile = CDbFile::Instance(aOsFile);
    
    SimulateIOError(SQLITE_IOERR);
    
    TRACE3("READ %X lock=%d\n", dbFile.Handle(), dbFile.LockType());
    
    TPtr8 ptr(reinterpret_cast <TUint8*> (aBuf), aAmt);
    INC_COUNTER(TheDbFileIOCounters.iReadOpCnt);
    GET_TIME(start);
    TInt err = dbFile.Read(ptr, aAmt);
    ADD_TIME_DIFF(start, TheDbFileIOCounters.iReadOpTime);
    
    tls.SetOsErrorCode(err);
    return err == KErrNone ? SQLITE_OK : SQLITE_IOERR;
    }

//Write data from a buffer into a file.  Return SQLITE_OK on success
//or some other error code on failure.
int CDbFile::Write(OsFile *aOsFile, const void *aBuf, int aAmt)
    {
    TTlsData& tls = TTlsData::Instance();
    CDbFile& dbFile = CDbFile::Instance(aOsFile);
    
    SimulateIOError(SQLITE_IOERR);
    SimulateDiskfullError;
        
    TRACE3("WRITE %X lock=%d\n", dbFile.Handle(), dbFile.LockType());
    
    __ASSERT_DEBUG(aAmt > 0, User::Panic(KPanicCategory, EPanicInvalidWAmount));
    TPtrC8 ptr(reinterpret_cast <const TUint8*> (aBuf), aAmt);
    INC_COUNTER(TheDbFileIOCounters.iWriteOpCnt);
    GET_TIME(start);
    TInt err = KErrNone;
    if(dbFile.iFilePos > dbFile.iFileSize)
        {
        err = dbFile.SetSize(dbFile.iFilePos);
        if(err == KErrNone)
            {
            dbFile.iFileSize = dbFile.iFilePos;
            }
        }
    if(err == KErrNone)
        {
        err = dbFile.Write(ptr, aAmt);
        }
    ADD_TIME_DIFF(start, TheDbFileIOCounters.iWriteOpTime);
    
    tls.SetOsErrorCode(err);
    return err == KErrNone ? SQLITE_OK : SQLITE_FULL;
    }

//Move the read/write pointer in a file.
//The function does not actually move the file pointer, it only stores the requested offset in the related CDbFile object.
int CDbFile::Seek(OsFile* aOsFile, i64 aOffset)
    {
    //Symbian OS supports 32 bit file size only!
    __ASSERT_DEBUG((TInt32)aOffset == aOffset, User::Panic(KPanicCategory, EPanicOffset64bit));
#ifdef SQLITE_TEST
    if(aOffset) 
        {
        SimulateDiskfullError;
        }
#endif
    SEEK((TInt)(aOffset / 1024 + 1));   
    CDbFile::Instance(aOsFile).iFilePos = (TInt)aOffset;
    return SQLITE_OK;
    }

//Make sure all writes to a particular file are committed to disk.
int CDbFile::Sync(OsFile *aOsFile, int)
    {
    TTlsData& tls = TTlsData::Instance();
    CDbFile& dbFile = CDbFile::Instance(aOsFile);
    
    TRACE3("SYNC %X lock=%d\n", dbFile.Handle(), dbFile.LockType());
    
    INC_COUNTER(TheDbFileIOCounters.iSyncOpCnt);
    GET_TIME(start);
    TInt err = dbFile.Flush();
    ADD_TIME_DIFF(start, TheDbFileIOCounters.iSyncOpTime);
    
    tls.SetOsErrorCode(err);
    return err == KErrNone ? SQLITE_OK : SQLITE_IOERR;
    }

//Truncate an open file to a specified size
//This operation invalidates iFilePos!!!
int CDbFile::Truncate(OsFile *aOsFile, i64 aOffset)
    {
    TTlsData& tls = TTlsData::Instance();
    CDbFile& dbFile = CDbFile::Instance(aOsFile);
    //Symbian OS supports 32 bit file size only!
    __ASSERT_DEBUG((TInt32)aOffset == aOffset, User::Panic(KPanicCategory, EPanicOffset64bit));
    TInt32 offset32 = (TInt)aOffset;//Symbian OS supports 32 bit file size only!
    TRACE3("TRUNCATE %X %d\n", dbFile.Handle(), offset32);
    SimulateIOError(SQLITE_IOERR);
    
    GET_TIME(start);
    TInt err = dbFile.SetSize(offset32);
    ADD_TIME_DIFF(start, TheDbFileIOCounters.iTruncateOpTime);
    INC_COUNTER(TheDbFileIOCounters.iTruncateOpCnt);
    
    tls.SetOsErrorCode(err);
    if(err == KErrNone)
        {
        dbFile.iFilePos = -1;
        dbFile.iFileSize = offset32;
        return SQLITE_OK;
        }
    return SQLITE_IOERR;    
    }

//Determine the current size of a file in bytes
int CDbFile::FileSize(OsFile *aOsFile, i64 *aSize)
    {
    TTlsData& tls = TTlsData::Instance();
    CDbFile& dbFile = CDbFile::Instance(aOsFile);
    
    SimulateIOError(SQLITE_IOERR);
        
    TInt size32 = 0;//Symbian OS supports 32 bit file size only!
    
    GET_TIME(start);
    TInt err = dbFile.Size(size32);
    ADD_TIME_DIFF(start, TheDbFileIOCounters.iFileSizeOpTime);
    *aSize = size32;
    INC_COUNTER(TheDbFileIOCounters.iFileSizeOpCnt);
    
    tls.SetOsErrorCode(err);
    return err == KErrNone ? SQLITE_OK : SQLITE_IOERR;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////          FILE LOCKING - SERVER              ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// - SERVER
//Lock the file with the lock specified by parameter locktype - one
//of the following:
//
//     (1) SHARED_LOCK
//     (2) RESERVED_LOCK
//     (3) PENDING_LOCK
//     (4) EXCLUSIVE_LOCK
//
//Sometimes when requesting one lock state, additional lock states
//are inserted in between.  The locking might fail on one of the later
//transitions leaving the lock state different from what it started but
//still short of its goal.  The following chart shows the allowed
//transitions and the inserted intermediate states:
//
//   UNLOCKED -> SHARED
//   SHARED -> RESERVED
//   SHARED -> (PENDING) -> EXCLUSIVE
//   RESERVED -> (PENDING) -> EXCLUSIVE
//   PENDING -> EXCLUSIVE
//
//This routine will only increase a lock.  The sqlite3OsUnlock() routine
//erases all locks at once and returns us immediately to locking level 0.
//It is not possible to lower the locking level one step at a time.  You
//must go straight to locking level 0.
//This happens on the server side, which is a single threaded process - no need of a file locking/unlocking.
int CDbFile::Lock(OsFile *aOsFile, int aLockType)
    {
    CDbFile& dbFile = CDbFile::Instance(aOsFile);
    //If there is already a lock of this type or more restrictive on the OsFile, do nothing.
    if(dbFile.LockType() >= aLockType)
        {
        return SQLITE_OK;
        }
    dbFile.SetLockType(aLockType);
    return SQLITE_OK;
    }

// - SERVER
//This routine checks if there is a RESERVED lock held on the specified
//file by this or any other process. If such a lock is held, return
//non-zero, otherwise zero.
//This happens on the server side, which is a single threaded process - no need of a file locking/unlocking.
int CDbFile::CheckReservedLock(OsFile *aOsFile)
    {
    CDbFile& dbFile = CDbFile::Instance(aOsFile);
    return dbFile.LockType() >= RESERVED_LOCK ? 1 : 0;
    }

// - SERVER
//Lower the locking level on file descriptor id to locktype.  locktype
//must be either NO_LOCK or SHARED_LOCK.
//
//If the locking level of the file descriptor is already at or below
//the requested locking level, this routine is a no-op.
//
//This happens on the server side, which is a single threaded process - no need of a file locking/unlocking.
int CDbFile::Unlock(OsFile *aOsFile, int aLockType)
    {
    CDbFile& dbFile = CDbFile::Instance(aOsFile);
    dbFile.SetLockType(aLockType);
    return SQLITE_OK;
    }
    
// The fullSync option is meaningless on Symbian. This is a no-op.
void CDbFile::SetFullSync(OsFile*, int)
    {
    }

// Return the underlying file handle for an OsFile
int CDbFile::FileHandle(OsFile *aOsFile)
    {
    return CDbFile::Instance(aOsFile).Handle();
    }

// Return an integer that indices the type of lock currently held
// by this handle.  (Used for testing and analysis only.)
int CDbFile::LockState(OsFile *aOsFile)
    {
    return  CDbFile::Instance(aOsFile).LockType();
    }

//TheIoMethods holds a pointers to the file functions used later for initialization of 
//OsFile::pMethod data member.
static const IoMethod TheIoMethods = 
    {
    &CDbFile::Close,
    &sqlite3SymbianOpenDirectory,
    &CDbFile::Read,
    &CDbFile::Write,
    &CDbFile::Seek,
    &CDbFile::Truncate,
    &CDbFile::Sync,
    &CDbFile::SetFullSync,
    &CDbFile::FileHandle,
    &CDbFile::FileSize,
    &CDbFile::Lock,
    &CDbFile::Unlock,
    &CDbFile::LockState,
    &CDbFile::CheckReservedLock
    };
    
CDbFile::CDbFile() :
    iFileToBeDeleted(NULL),
    iLockType(NO_LOCK),
    iFilePos(0),
    iFileSize(0)
    {
    pMethod = &TheIoMethods;
    }

inline CDbFile& CDbFile::Instance(void* aDbFile)
    {
    __ASSERT_DEBUG(aDbFile != NULL, User::Invariant());
    return *(static_cast <CDbFile*> (aDbFile));
    }

///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////       SQLITE OS proting layer, API definitions    ///////////////////
///////////////////////////////////////////////////////////////////////////////////////////

//Delete the named file.
//aFilename is expected to be UTF8 encoded, zero terminated string.
//Although the function returns an integer, representing the error code, it always returns SQLITE_OK
//(To keep it compatible with what the SQLITE library is expecting).
//But if an error occurs while deleting the file, the function will set the returned error code in
//TheOsErrorCode variable, which may be analyzed later by the SQLITE client(s).
int sqlite3SymbianDelete(const char *aFileName)
    {
    TFhStrType fhStrType = FhStringProps(aFileName);
    TFileName fname;
    if(ConvertToUnicode(aFileName, fname))
        {
        GET_TIME(start);
        TTlsData& tls = TTlsData::Instance();
        if(fhStrType == EFhMainDbStr)
            {//Deleting files in somebody else's private data cage - not allowed!
            tls.SetOsErrorCode(KErrPermissionDenied);
            }
        else
            {
            if(fhStrType == EFhStr)
                {
                FhConvertToFileName(fname, tls.iParse, tls.iSysPrivDir);//If fname does not have a path, iSysPrivDir will be used
                }
            tls.SetOsErrorCode(tls.iFs.Delete(fname));
            }
        ADD_TIME_DIFF(start, TheDbFileIOCounters.iDeleteOpTime);
        }
    TRACE2("DELETE \"%s\"\n", aFileName);
    INC_COUNTER(TheDbFileIOCounters.iDeleteOpCnt);
    return SQLITE_OK;
    }

//Return TRUE if the named file exists.
//aFilename is expected to be UTF8 encoded, zero terminated string.
//Returns:
//     0 -  a file with "aFileName" name  does not exist or RFs()::Entry() call failed;
// non-0 -  a file with "aFileName" exists;
int sqlite3SymbianFileExists(const char *aFileName)
    {
    TFhStrType fhStrType = FhStringProps(aFileName);
    TBool res = EFalse;
    TFileName fname;
    if(ConvertToUnicode(aFileName, fname))
        {
        GET_TIME(start);
        TTlsData& tls = TTlsData::Instance();
        if(fhStrType == EFhStr)
            {
            FhConvertToFileName(fname, tls.iParse, tls.iSysPrivDir);//If fname does not have a path, iSysPrivDir will be used
            }
        TEntry entry;
        res = tls.iFs.Entry(fname, entry) == KErrNone;
        ADD_TIME_DIFF(start, TheDbFileIOCounters.iExistOpTime);
        }
    INC_COUNTER(TheDbFileIOCounters.iExistOpCnt);
    return res;
    }

//All possible "file open" operations
enum TOpenFileOpType {EOpenReadWrite, EOpenExclusive, EOpenReadOnly};

//File open function
//aReadOnly flag is an output parameter, indicating wheter the file was open in read-only mode or not
//It is a non-null pointer only for EOpenReadWrite operations.
static TInt DoOpenFile(TOpenFileOpType aOpType, const char *aFileName, OsFile** aOsFile, int* aReadOnly, int aDeleteOnClose)
    {
    TFhStrType fhStrType = FhStringProps(aFileName);
    TTlsData& tls = TTlsData::Instance();
    //Convert the name from UTF8 to UTF16
    TFileName fname;
    if(!ConvertToUnicode(aFileName, fname))
        {
        tls.SetOsErrorCode(KErrBadName);
        return SQLITE_CANTOPEN; 
        }
    //Create new, unitialized CDbFile object
    CDbFile* dbFile = CDbFile::New();
    if(!dbFile)
        {
        tls.SetOsErrorCode(KErrNoMemory);
        return SQLITE_NOMEM;
        }
    /////////////////////////////////
    TInt err = KErrNone;//Symbian OS error
    /////////////////////////////////  FILE OPEN/CREATE CODE  BEGIN  /////////////////////////////////////////
    GET_TIME(start);
    if(fhStrType == EFhMainDbStr)
        {//Main db file, open from handle
        const RMessage2* msg;
        TBool readOnly;
        tls.RetrieveAndResetFhData(msg, readOnly);
        *aReadOnly = readOnly;
        err = msg != NULL ? dbFile->OpenFromHandle(*msg) : KErrGeneral;
        }
    else
        {
        if(fhStrType == EFhStr)
            {//Not the main db file. Replace invalid characters in the file name
            FhConvertToFileName(fname, tls.iParse, tls.iSysPrivDir);//If fname does not have a path, iSysPrivDir will be used
            }
        //Open for read/write
        if(aOpType == EOpenReadWrite)
            {
            *aReadOnly = 0;
            //If the file exists - open it, otherwise - create it.(R/W mode)
            //The reason that "Open" and "Create" calls are packed in a "for" loop is:
            //1) Current thread calls  dbFile->Open() and the returned error code is KErrNotFound. Then the thread will try to create the file.
            //2) But another thread takes the CPU time and creates the file before the curent thread.
            //3) Current thread tries to create the file but gets KErrAlreadyExists error code.
            //4) Then the current thread has to call dbFile->Open() again to open the file if it already exists.
            for(err=KErrAlreadyExists;err==KErrAlreadyExists;)
                {
                if((err = dbFile->Open(fname)) == KErrNotFound)
                    {
                    err = dbFile->Create(fname);
                    }
                }
            if(err != KErrNone) 
                {
                TInt prevErr = err;
                err = dbFile->OpenReadOnly(fname);
                if(err == KErrNone)
                    {
                    *aReadOnly = 1;
                    }
                else if(prevErr == KErrAccessDenied) //this is attempt to create a file on a read-only drive
                    {
                    err = KErrAccessDenied;
                    }
                }
            }
        //Open for exclusive access
        else if(aOpType == EOpenExclusive)
            {
            err = dbFile->CreateExclusive(fname, aDeleteOnClose);
            }
        //Open for read-only access
        else if(aOpType == EOpenReadOnly)
            {
            err = dbFile->OpenReadOnly(fname);
            }
        else
            {
            __ASSERT_DEBUG(0, User::Panic(KPanicCategory, EPanicInvalidOpType));
            }
        }//end of - "if(fromHandle)"
    ADD_TIME_DIFF(start, TheDbFileIOCounters.iOpenOpTime);
    /////////////////////////////////  FILE OPEN/CREATE CODE  END    /////////////////////////////////////////
    tls.SetOsErrorCode(err);
    if(err != KErrNone)
        {
        delete dbFile;
        return err == KErrNoMemory ? SQLITE_NOMEM : SQLITE_CANTOPEN;
        }
    *aOsFile = dbFile;
    OpenCounter(+1);
    INC_COUNTER(TheDbFileIOCounters.iOpenOpCnt);
    return SQLITE_OK;
    }

// Attempt to open a file for both reading and writing.  If that
// fails, try opening it read-only.  If the file does not exist,
// try to create it.
//
// On success, a handle for the open file is written to *aOsFile
// and *aReadOnly is set to 0 if the file was opened for reading and
// writing or 1 if the file was opened read-only.  The function returns
// SQLITE_OK.
//
// On failure, the function returns SQLITE_CANTOPEN and leaves
// *aOsFile and *aReadOnly unchanged.
int sqlite3SymbianOpenReadWrite(const char *aFileName, OsFile** aOsFile, int* aReadOnly)
    {
    return DoOpenFile(EOpenReadWrite, aFileName, aOsFile, aReadOnly, 0);
    }

// Attempt to open a new file for exclusive access by this process.
// The file will be opened for both reading and writing.  To avoid
// a potential security problem, we do not allow the file to have
// previously existed.  Nor do we allow the file to be a symbolic
// link.
//
// If aDelFlag is true, then make arrangements to automatically delete
// the file when it is closed.
//
// On success, write the file handle into *aOsFile and return SQLITE_OK.
//
// On failure, return SQLITE_CANTOPEN.
int sqlite3SymbianOpenExclusive(const char *aFileName, OsFile** aOsFile, int aDelFlag)
    {
    return DoOpenFile(EOpenExclusive, aFileName, aOsFile, NULL, aDelFlag);
    }

// Attempt to open a new file for read-only access.
//
// On success, write the file handle into *aOsFile and return SQLITE_OK.
//
// On failure, return SQLITE_CANTOPEN.
int sqlite3SymbianOpenReadOnly(const char *aFileName, OsFile** aOsFile)
    {
    return DoOpenFile(EOpenReadOnly, aFileName, aOsFile, NULL, 0);
    }

//Create a temporary file name in aBuf.  aBuf must be big enough to
//hold at least SQLITE_TEMPNAME_SIZE characters.
//After the call aBuf will hold the temporary file name, UTF8 encoded, zero terminated string.
//The function does not use "sqlite3_temp_directory" global variable. All temporary files will
//be created in the process's private data cage.
int sqlite3SymbianTempFileName(char *aBuf)
    {
    GET_TIME(start);
    TBuf<SQLITE_TEMPNAME_SIZE> tmpFileName;
    tmpFileName.Copy(TTlsData::Instance().iSysPrivDir);
    const TInt KFileNamePos = tmpFileName.Length();
    TUint32 randomVal = Math::Random(); 
    TInt64 seed = (TInt64)randomVal;
    const TInt KFileNameLen = 15;
    tmpFileName.SetLength(tmpFileName.Length() + KFileNameLen);
    
    for(;;)
        {
        TInt pos = KFileNamePos;
        for(TInt i=0;i<KFileNameLen;++i,++pos)
            {
            TInt j = Math::Rand(seed) % (sizeof(TheChars) - 1);
            tmpFileName[pos] = TheChars[j];
            }
        TTlsData& tls = TTlsData::Instance();
        TUint attr;
        if(tls.iFs.Att(tmpFileName, attr) == KErrNotFound)
            {
            break;
            }
        }

    //No need to convert the temporary file name to its unicode presentation: the file name contains only 
    //ASCII characters!!!
    TPtr8 dest(reinterpret_cast <TUint8*> (aBuf), SQLITE_TEMPNAME_SIZE);
    dest.Copy(tmpFileName);
    dest.Append(TChar(0));
    
    ADD_TIME_DIFF(start, TheDbFileIOCounters.iTempFileNameOpTime);
    INC_COUNTER(TheDbFileIOCounters.iTempFileNameOpCnt);
    TRACE2("TEMP FILENAME: %s\n", aBuf);
    return SQLITE_OK; 
    }

//Sync the directory zDirname. This is a no-op on operating systems other
//than UNIX.
int sqlite3SymbianSyncDirectory(const char* /*aDirName*/)
    {
    SimulateIOError(SQLITE_IOERR);
    return SQLITE_OK;
    }
    
#ifndef SQLITE_OMIT_PAGER_PRAGMAS

//Check that a given pathname is a directory and is writable.
//aDirName is expected to be UTF8 encoded, zero terminated string.
int sqlite3SymbianIsDirWritable(char *aDirName)
    {
    int res = 0;
    TFileName dirName;
    if(ConvertToUnicode(aDirName, dirName))
        {
        GET_TIME(start);
        TEntry entry;
        if(TTlsData::Instance().iFs.Entry(dirName, entry) == KErrNone)
            {
            if(entry.IsDir() && !entry.IsReadOnly())
                {
                res = 1;    
                }
            }
        ADD_TIME_DIFF(start, TheDbFileIOCounters.iIsDirWOpTime);
        }
    INC_COUNTER(TheDbFileIOCounters.iIsDirWOpCnt);
    
    return res;
    }

#endif//SQLITE_OMIT_PAGER_PRAGMAS

//Turn a relative pathname into a full pathname.  Return a pointer
//to the full pathname stored in space obtained from sqliteMalloc().
//The calling function is responsible for freeing this space once it
//is no longer needed.
//
//The input file name is expected to be UTF8, zero-terminated. The output file name will be UTF8, zero-terminated.
char *sqlite3SymbianFullPathname(const char *aRelative)
    {
    TTlsData& tls = TTlsData::Instance();
    tls.StoreFhData(NULL, EFalse);
    if(!aRelative)  //NULL argument
        {
        return 0;
        }
    TFhStrType strType = FhStringProps(aRelative);//Detect string type - it may not be a real file name
    //Convert the received file name to UTF16
    TBuf<KMaxFileName + 1> fname;
    if(!ConvertToUnicode(aRelative, fname))
        {
        return 0;
        }
    //Zero-terminate the converted file name
    fname.Append(TChar(0));
    char* result = static_cast <char*> (sqliteMalloc(KMaxFileName + 1));
    if(!result)
        {
        return 0;
        }
    //    If the format of aRelative argument is <[SID]FileName.[EXT]>, then the database file name will be 
    //treated as a name of a secure database file which has to be created/opened in the server's private 
    //directory on the system drive.
    //    If the format of aRelative argument is <Drive:[SID]FileName.[EXT]>, then the database file name 
    //will be treated as a name of a secure database file which has to be created/opened in the server's 
    //private directory on <Drive:> drive. 
    //    If the format of aRelative argument is <Drive:\Path\FileName.[EXT]>, then the database file name
    //will be treated as a name of a non-secure database file in <Drive:\Path\> directory.
    //    If aRelative contains file handles, then it will be treated as a name of a file belonging to server's
    //private data cage. 
    if(strType == EFhMainDbStr)
        {//The additonal information has to be extracted and fnmae reformatted, because SQLITE will
         //use the returned full file name when making a decission to share the cache.
        FhExtractAndStore(fname, tls);
        (void)tls.iParse.Set(fname, 0, 0);//the file name has to be verified by the server
        }
    else
        {
        (void)tls.iParse.Set(fname, &tls.iSysPrivDir, 0);//If fname does not have a path, iSysPrivDir will be used
        }
    TPtr8 dest8(reinterpret_cast <TUint8*> (result), KMaxFileName + 1); 
    if(!ConvertFromUnicode(tls.iParse.FullName(), dest8))
        {
        tls.StoreFhData(NULL, EFalse);
        sqliteFree(result);
        return 0;   
        }
    return result;
    }

// ***************************************************************************
// ** Everything above deals with file I/O.  Everything that follows deals
// ** with other miscellanous aspects of the operating system interface
// ***************************************************************************

//Get information to seed the random number generator.  The seed
//is written into the buffer aBuf[256].  The calling function must
//supply a sufficiently large buffer.
int sqlite3SymbianRandomSeed(char *aBuf)
    {
    //We have to initialize aBuf to prevent valgrind from reporting
    //errors.  The reports issued by valgrind are incorrect - we would
    //prefer that the randomness be increased by making use of the
    //uninitialized space in aBuf - but valgrind errors tend to worry
    //some users.  Rather than argue, it seems easier just to initialize
    //the whole array and silence valgrind, even if that means less randomness
    //in the random seed.
    //
    //When testing, initializing aBuf[] to zero is all we do.  That means
    //that we always use the same random number sequence.* This makes the
    //tests repeatable.
    Mem::FillZ(aBuf, 256);
    TUint32 randomVal[2];
    randomVal[0] = Math::Random();
    randomVal[1] = Math::Random();
    Mem::Copy(aBuf, randomVal, sizeof(randomVal));
    return SQLITE_OK;
    }

//Sleep for a little while.  Return the amount of time slept.
int sqlite3SymbianSleep(int ms)
    {
    User::AfterHighRes(TTimeIntervalMicroSeconds32(ms * 1000));
    return ms;
    }

//The following pair of routine implement mutual exclusion for
//multi-threaded processes.  Only a single thread is allowed to
//executed code that is surrounded by EnterMutex() and LeaveMutex().
//
//SQLite uses only a single Mutex.  There is not much critical
//code and what little there is executes quickly and without blocking.
//
//Version 3.3.1 and earlier used a simple mutex.  Beginning with
//version 3.3.2, a recursive mutex is required.
void sqlite3SymbianEnterMutex()
    {
    ++TheMutexRefCounter;
    }
    
void sqlite3SymbianLeaveMutex()
    {
    --TheMutexRefCounter;
    }

//Return TRUE if the mutex is currently held.
//
//If the thisThreadOnly parameter is true, return true if and only if the
//calling thread holds the mutex.  If the parameter is false, return
//true if any thread holds the mutex.
int sqlite3SymbianInMutex(int /*aThisThreadOnly*/)
    {
    return TheMutexRefCounter > 0;
    }

/*
** The following variable, if set to a non-zero value, becomes the result
** returned from sqlite3OsCurrentTime().  This is used for testing.
*/
#ifdef SQLITE_TEST
int sqlite3_current_time = 0;
#endif

//Find the current time (in Universal Coordinated Time).  Write the
//current time and date as a Julian Day number into *prNow and
//return 0.  Return 1 if the time and date cannot be found.
int sqlite3SymbianCurrentTime(double *prNow)
    {
    TTime now;
    now.UniversalTime();
    TDateTime date = now.DateTime();
    TInt year = date.Year(), month = date.Month() + 1, day = date.Day() + 1;
    
    TInt jd = ( 1461 * ( year + 4800 + ( month - 14 ) / 12 ) ) / 4 +
          ( 367 * ( month - 2 - 12 * ( ( month - 14 ) / 12 ) ) ) / 12 -
          ( 3 * ( ( year + 4900 + ( month - 14 ) / 12 ) / 100 ) ) / 4 +
          day - 32075;
          
    *prNow = jd;
#ifdef SQLITE_TEST
    if( sqlite3_current_time )
        {
        *prNow = sqlite3_current_time / 86400.0 + 2440587.5;
        }
#endif
  return 0;
}

static TInt TlsHandle2()
    {
    return reinterpret_cast <TInt> (&TlsHandle2);
    }

// If called with aAllocateFlag>1, then return a pointer to thread
// specific data for the current thread.  Allocate and zero the
// thread-specific data if it does not already exist necessary.
//
// If called with aAllocateFlag==0, then check the current thread
// specific data.  Return it if it exists.  If it does not exist,
// then return NULL.
//
// If called with aAllocateFlag<0, check to see if the thread specific
// data is allocated and is all zero.  If it is then deallocate it.
// Return a pointer to the thread specific data or NULL if it is
// unallocated or gets deallocated.
ThreadData* sqlite3SymbianThreadSpecificData(int aAllocateFlag)
    {
    ThreadData* data = static_cast <ThreadData*> (UserSvr::DllTls(TlsHandle2()));
    if(aAllocateFlag > 0)
        {
        if(!data)
            {
            data = static_cast <ThreadData*> (sqlite3OsMalloc(sizeof(ThreadData)));
            if(data)
                {
                Mem::FillZ(data, sizeof(ThreadData));
                TTlsData& tls = TTlsData::Instance();
                TInt err = UserSvr::DllSetTls(TlsHandle2(), data);
                tls.SetOsErrorCode(err);
                if(err != KErrNone)
                    {
                    sqlite3OsFree(data);
                    return 0;
                    }
                }
            }
        }
    else if(data != 0 && aAllocateFlag < 0)
        {
        if(Mem::Compare(reinterpret_cast <const TUint8*> (data), sizeof(ThreadData), 
                        reinterpret_cast <const TUint8*> (&TheZeroThreadData), sizeof(ThreadData)) == 0)
            {
            sqlite3OsFree(data);
            data = 0;
            (void)UserSvr::DllSetTls(TlsHandle2(), 0);
            }
        }
    return data;
    }

///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////       SQLITE OS proting layer      //////////////////////////////////
////////////////////       memory allocation routines    //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

#ifdef __MEM_TRACE__

////////////////////////////////////////////////////////////////////////////////////////////////////////

static TInt TheMallocCount = 0;
static TInt TheReallocCount = 0;
static TInt TheFreeCount = 0;
static TInt TheAllocSizeCount = 0;
static TInt TheTotalAllocated = 0;
static TInt TheTotalFreed = 0;

/**
*/
void* sqlite3SymbianMalloc(int aSize)
    {
    void* res = TheAllocator->Alloc(aSize);
    TInt len = res ? TheAllocator->AllocLen(res) : 0;
    RDebug::Print(_L("*** OS MALLOC: Size=%d, rounded=%d, p=%X\r\n"), aSize, len, res);
    ++TheMallocCount;
    return res;
    }
    
/**
*/
void* sqlite3SymbianRealloc(void* aPtr, int aSize)
    {
    TInt oldSize = aPtr ? TheAllocator->AllocLen(aPtr) : 0;
    void* res = TheAllocator->ReAlloc(aPtr, aSize);
    RDebug::Print(_L("*** OS REALLOC: Old size=%d, Size=%d, p_old=%X, p_new=%X\r\n"), oldSize, aSize, aPtr, res);
    ++TheReallocCount;
    return res;
    }
    
/**
*/
void sqlite3SymbianFree(void* aPtr)
    {
    TInt len = aPtr ? TheAllocator->AllocLen(aPtr) : 0;
    RDebug::Print(_L("*** OS FREE: size=%d, p=%X\r\n"), len, aPtr);
    TheAllocator->Free(aPtr);
    ++TheFreeCount;
    TheTotalFreed += len;   
    }
    
/**
*/
int sqlite3SymbianAllocationSize(void* aPtr)
    {
    TInt len = aPtr ? TheAllocator->AllocLen(aPtr) : 0;
    RDebug::Print(_L("*** OS ALLOCSIZE: Size=%d, p=%X\r\n"), len, aPtr);
    ++TheAllocSizeCount;
    return len;
    }

/**
*/
void sqlite3SymbianPrintMemCounters(void)
    {
    RDebug::Print(_L("*** OS MALLOC COUNT=%d\r\n"), TheMallocCount);
    RDebug::Print(_L("*** OS REALLOC COUNT=%d\r\n"), TheReallocCount);
    RDebug::Print(_L("*** OS FREE COUNT=%d\r\n"), TheFreeCount);
    RDebug::Print(_L("*** OS ALLOCSIZE COUNT=%d\r\n"), TheAllocSizeCount);
    RDebug::Print(_L("*** OS TOTAL ALLOCATED=%d\r\n"), TheTotalAllocated);
    RDebug::Print(_L("*** OS TOTAL FREED=%d\r\n"), TheTotalFreed);
    }

/**
*/
void sqlite3SymbianResetMemCounters(void)
    {
    TheMallocCount = TheReallocCount = TheFreeCount = TheAllocSizeCount = 0;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////

#else //__MEM_TRACE__

////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
*/
void* sqlite3SymbianMalloc(int aSize)
    {
    return TheAllocator->Alloc(aSize);
    }
    
/**
*/
void* sqlite3SymbianRealloc(void* aPtr, int aSize)
    {
    return TheAllocator->ReAlloc(aPtr, aSize);
    }
    
/**
*/
void sqlite3SymbianFree(void* aPtr)
    {
    TheAllocator->Free(aPtr);
    }
    
/**
*/
int sqlite3SymbianAllocationSize(void* aPtr)
    {
    return aPtr ? TheAllocator->AllocLen(aPtr) : 0;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif//__MEM_TRACE__

////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif//OS_SYMBIAN
