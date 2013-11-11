/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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

#include <sysutil.h>
#include "../../bidi.h"

#include "Frame.h"

#include "PluginStream.h"
#include "PluginSkin.h"
#include "PluginWin.h"
#include "PluginHandler.h"
#include "StaticObjectsContainer.h"
#include "PluginStreamLoaderClient.h"

_LIT(KPath,"c:\\system\\temp\\");

using namespace WebCore;

PluginStream::PluginStream(PluginSkin* pluginskin, 
        NetscapePlugInStreamLoaderClient* loaderclient, void* notifydata) :
                                                m_notifydata(notifydata),
                                                m_pluginskin(pluginskin),
                                                m_loaderclient(loaderclient),
                                                m_stream(0),                                                
                                                m_type(NP_NORMAL),                                                
                                                m_fileName(0),
                                                m_streamDestroyed(false)
{   
    m_pluginskin->addPluginStream(this);
}

PluginStream::~PluginStream()
{        
    User::Free(m_stream);
    delete m_fileName;    
    m_pluginskin->removePluginStream(this);
}

void PluginStream::close()
{
    m_loaderclient->stop();
}

void PluginStream::createNPStreamL(TPtrC8 url, TPtrC16 mimetype, long long length)
{
    
    NPError error( NPERR_NO_ERROR );
        
    
    // convert to 16 bit
    HBufC* url16 = HBufC::NewLC(url.Length());                
    url16->Des().Copy(url);                

    // convert to 8 bit
    HBufC8* mime8 = HBufC8::NewLC(mimetype.Length());
    mime8->Des().Copy(mimetype);
        

    m_pluginskin->createPluginWinL(url, mimetype);         
    m_pluginskin->loadPluginL(*mime8);
    
    if (m_pluginskin->getNPP() && m_pluginskin->getNPPluginFucs()) {
        
        m_stream = (NPStream*) User::AllocL( sizeof(NPStream) );       
            
        if (m_stream) {        
            m_stream->pdata = m_pluginskin->getNPP()->pdata;
            m_stream->ndata =  m_pluginskin->pluginWin();        
            m_stream->url = url16->AllocL();
            m_stream->end = length;
            m_stream->lastmodified = 0; 
            m_stream->notifyData = m_notifydata;    

            
            error = m_pluginskin->getNPPluginFucs()->newstream ( m_pluginskin->getNPP(), 
                                                            *mime8, 
                                                            m_stream, 
                                                            EFalse,
                                                            &m_type );                                                
        }

    }

        
    CleanupStack::PopAndDestroy(2);  

    if (error == NPERR_NO_ERROR) {
        if (m_type == NP_ASFILEONLY || m_type == NP_ASFILE) {
            generateTempFileL();                    
        }                
    }

    switch ( error ) {
        case NPERR_OUT_OF_MEMORY_ERROR: {
            User::Leave( KErrNoMemory );
            break;
        }
        case NPERR_GENERIC_ERROR: {
            User::Leave( KErrNotSupported );
            break;
        }
    }    

}

void PluginStream::writeStreamL(const char* data, int length)
{

    switch ( m_type ) {
        case NP_NORMAL: {
            writeStreamToPluginL( data, length );
            break;
        }
        case NP_ASFILEONLY: {
            writeStreamToFileL( data, length );
            break;
        }
        case NP_ASFILE: {
            writeStreamToFileL( data, length );
            writeStreamToPluginL( data, length );
            break;
        }
        case NP_SEEK:{    
            break;
        }
        default: {    
        break;
        }
    }

}

void PluginStream::writeStreamToPluginL(const char* data, int length)
{
    int offset = 0;
    while (offset<length) {
            
        if (m_pluginskin->getNPPluginFucs())  {        

            int spaceavaiable = m_pluginskin->getNPPluginFucs()->writeready(m_pluginskin->getNPP(), m_stream);
            
            if (spaceavaiable <= 0) {                
                User::Leave( KErrNotSupported );                
            }
             
            int len = (spaceavaiable > (length-offset))? (length-offset) : spaceavaiable;                                
            int bytesconsumed = m_pluginskin->getNPPluginFucs()->write( m_pluginskin->getNPP(), m_stream, offset, len, (void*)data);                
             
            if (bytesconsumed <= 0){
                User::Leave( KErrNotSupported );                
            } 
                                           
            offset += bytesconsumed;
        
        }    
        
    }

}


void PluginStream::writeStreamToFileL(const char* data, int length)
{

    if (m_fileName) {

        RFs&    rfs = StaticObjectsContainer::instance()->fsSession();
        RFile  file;    

               
        // Write to the file only if we are not below critical disk level
        if (SysUtil::DiskSpaceBelowCriticalLevelL (&rfs, length, EDriveC)) {
            User::Leave(KErrDiskFull);
        }
        
        User::LeaveIfError(file.Open(rfs, *m_fileName, EFileWrite));
        CleanupClosePushL(file);
        
        TInt   pos(0);
        User::LeaveIfError(file.Seek(ESeekEnd, pos));
                
        TPtrC8 ptrc8((TUint8*)data,length);
        file.Write(ptrc8);
        file.Flush();

        CleanupStack::PopAndDestroy(); // file        
    }

}

void PluginStream::destroyStream(int reason)
{
    if (m_streamDestroyed) return;
        m_streamDestroyed = true;

    TInt16 npreason( NPRES_BASE );

    if (m_type == NP_ASFILEONLY || m_type == NP_ASFILE) {

        if ( reason == KErrNone ) {
        
            if ( m_pluginskin->getNPPluginFucs() && m_pluginskin->getNPPluginFucs()->asfile ) {
                m_pluginskin->getNPPluginFucs()->asfile( m_pluginskin->getNPP(), m_stream, *m_fileName );
            }
        }
    }
    
    switch ( reason ) {
        case KErrNone:
        npreason = NPRES_DONE;
        break;

        case KErrCancel:
        npreason = NPRES_USER_BREAK;
        break;

        default:
        npreason = NPRES_NETWORK_ERR;
        break;
    }


    if ( m_pluginskin->getNPPluginFucs() && m_pluginskin->getNPPluginFucs()->destroystream ) {            
        m_pluginskin->getNPPluginFucs()->destroystream( m_pluginskin->getNPP(), m_stream,  npreason);
    }
    
    if (reason == KErrNone) {
        m_pluginskin->forceRedraw(true);
    }
}

void PluginStream::generateTempFileL()
{

    RFs&    rfs = StaticObjectsContainer::instance()->fsSession();
    RFile  file;    

    rfs.MkDirAll(KPath);
    
    // Create unique file name from the temp file name and file extension from Url
    TFileName tempFileName;
    User::LeaveIfError(file.Temp(rfs, KPath, tempFileName, EFileWrite));
    CleanupClosePushL(file);

    TParse p;
    p.Set(tempFileName,NULL,NULL);
    TPtrC fileNameOnly(p.Name());

    // Create a temp file with same file extension as the plugin content,
    // by extracting the file extension from the URL. Using example URL,
    // "http://www.xyz.com/flashy.swf?h=234&w=321&script=all.js"
    HBufC8* urlName = NULL;
    urlName = HBufC8::NewLC(m_stream->url->Length());
    urlName->Des().Copy(m_stream->url->Des());
    TPtrC8 fileExtPtr(urlName->Des());    
    if (fileExtPtr.Length() > 0) {

        // Ignore last character if it is '/'
        if ( fileExtPtr[fileExtPtr.Length() - 1] == '/' ) {
            fileExtPtr.Set(fileExtPtr.Left(fileExtPtr.Length() - 1));
        }

        // Trim anything right of the query '?', the url example
        // leaves us with "http://www.xyz.com/flashy.swf"
        TInt i = fileExtPtr.Locate('?');
        if (i >= 0) {
            fileExtPtr.Set(fileExtPtr.Left(i));
        }

        // remove any '/' at the end  
    if ( fileExtPtr[fileExtPtr.Length() - 1] == '/' ) {
            fileExtPtr.Set(fileExtPtr.Left(fileExtPtr.Length() - 1));
        } 
        
        //  Trim anything left of path, the last '/'
        // "http://www.xyz.com/flashy.swf" -> "flashy.swf"
        i = fileExtPtr.LocateReverse('/');
        if (i >= 0) {
            fileExtPtr.Set(fileExtPtr.Mid(i+1));
        }

        // Trim the url to get the dot-extension, after the last '.'
        // "flashy.swf" -> ".swf". This works with "flashy.ad.swf".
        i = fileExtPtr.LocateReverse('.');
        if (i >= 0) {
            fileExtPtr.Set(fileExtPtr.Mid(i));
        }
        else{
                        TBuf8<KMaxFileName> fileName(fileExtPtr);
                        fileName.Append(tempFileName.Mid((tempFileName.LocateReverse('.'))));
                        fileExtPtr.Set(fileName);
            }
    }
        
            
    TInt fileLen(KPath().Length() + fileNameOnly.Length() + fileExtPtr.Length());
    m_fileName = HBufC::NewL(fileLen);
    
    TPtr fNamePtr = m_fileName->Des();
    fNamePtr.Copy(fileExtPtr);
    fNamePtr.Insert(0, fileNameOnly);
    fNamePtr.Insert(0, KPath());
    file.Rename(m_fileName->Des());
    
    m_pluginskin->m_tempFilesArray.AppendL(m_fileName->Alloc());
    
    CleanupStack::PopAndDestroy(2); // file, urlName            

}    
