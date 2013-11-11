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


#include "config.h"
#include "Event.h"
#include "FileChooser.h"
#include "../bidi.h"
#include "Document.h"
#include "FontData.h"
#include "Icon.h"
#include "Frame.h"
#include "LocalizedStrings.h"
#include "RenderFileUploadControl.h"
#include "WebCoreFrameBridge.h"
#include <e32base.h>
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebView.h"
#include "BrCtl.h"
#include <BrCtlDialogsProvider.h>
#include <CommonContentPolicy.h>
#include <StringLoader.h>
#if defined(__BROWSER_SDK)
#include <webkit_sdk.rsg>
#else
#include <Webkit.rsg>
#endif


class AsyncLaunch : public CAsyncOneShot
    {
    public:
        AsyncLaunch() : CAsyncOneShot(CActive::EPriorityHigh + 1) {}

        ~AsyncLaunch() {}
        void setFileChooser( WebCore::FileChooser* fileChooser, WebCore::Document* doc ) 
        {
            m_fileChooser = fileChooser;  
            m_document = doc;
        }

        void Cancel() { }
    private:
        void RunL()
        {
            TRAP_IGNORE(
            if( m_fileChooser ) {
                // fixme s602
                WebFrame* webFrame = kit(m_document->frame());
                MBrCtlDialogsProvider* dialogs = webFrame->frameView()->topView()->brCtl()->brCtlDialogsProvider();
                HBufC* buf = NULL;
                
                if( dialogs->DialogFileSelectLC( m_fileChooser->filename(), KNullDesC, buf) && m_fileChooser )
                    if( !isRestrictedFile( *buf ) )
                        m_fileChooser->chooseFile( *buf );
                    else {
                        TParsePtrC filenameParser( *buf );
                        HBufC* message = StringLoader::LoadLC(R_QTN_XHTML_QUERY_FILE_RESTRICTED, filenameParser.NameAndExt());
                        dialogs->DialogNoteL(*message);
                        CleanupStack::PopAndDestroy();  // message
                    }
                CleanupStack::PopAndDestroy();  // buf
            });
            delete this;
        }

        bool isRestrictedFile(const TDesC& filename)
        {
            bool banned(ETrue);
            TRAP_IGNORE( 
            CCommonContentPolicy* contentPolicy = CCommonContentPolicy::NewLC();
            banned = contentPolicy->IsClosedFileL( filename );
            CleanupStack::PopAndDestroy(); //contentPolicy
            );
            return banned;
        }

        WebCore::FileChooser* m_fileChooser;
        WebCore::Document* m_document;
    };

namespace WebCore {

FileChooser::FileChooser(FileChooserClient* client, const String& initialFilename)
    : m_client(client)
    , m_filename(initialFilename)
    , m_icon(0)
    , m_controller( 0 )
{
}

FileChooser::~FileChooser()
{
    if (m_controller)
        m_controller->setFileChooser(0, 0);
}

void FileChooser::openFileChooser(Document* doc)
{
    m_controller = new AsyncLaunch();

    if ( m_controller ) {
        m_controller->setFileChooser( this, doc );
        m_controller->Call();
    }
}

String FileChooser::basenameForWidth(const Font& font, int width) const
{
    if (width <= 0)
        return String();

    TParsePtrC filenameParser( m_filename );
    return filenameParser.NameAndExt();

// tot:fixme truncate needs work
/*
    String strToTruncate;
    if (m_filename.isEmpty())
        strToTruncate = fileButtonNoFileSelectedLabel();
    else
        strToTruncate = [[NSFileManager defaultManager] displayNameAtPath:m_filename];

    return [WebCoreStringTruncator centerTruncateString:strToTruncate
            toWidth:width withFont:m_uploadControl->style()->font().primaryFont()->getNSFont()];
*/
}

}
