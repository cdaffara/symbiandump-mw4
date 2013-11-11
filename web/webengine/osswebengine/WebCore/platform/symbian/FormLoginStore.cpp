/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "StringHash.h"
#include "FormLoginStore.h"
#include "FileStreamSymbian.h"
#include <pbe.h>
#include <pbedata.h>
_LIT(KLoginDatFile, "c:\\private\\%08x\\login.dat");

namespace WebCore {

//------------------------------------------------------------------------------
// LoginData
//------------------------------------------------------------------------------
LoginData::LoginData(FormLoginStore* s) : m_store(s)
{
}

LoginData::LoginData(FormLoginStore* s, const String& realm, const String& uname, const String& uvalue, const String& pname, const String& pvalue)
            : m_realm(realm), m_usernameField(uname), m_usernameValue(uvalue), m_passwdField(pname), m_passwdValue(pvalue), m_store(s)
{
}

LoginData::~LoginData()
{
}

void LoginData::read(StreamInput& is)
{
    String encryptedUser, encryptedPasswd;
    is>>m_realm>>m_usernameField>>encryptedUser>>m_passwdField>>encryptedPasswd;
    m_usernameValue = m_store->decrypt(encryptedUser);
    m_passwdValue = m_store->decrypt(encryptedPasswd);
}

void LoginData::write(StreamOutput& os) const
{
    os<<m_realm<<m_usernameField<<m_store->encrypt(m_usernameValue)<<m_passwdField<<m_store->encrypt(m_passwdValue);
}

bool LoginData::partialMatch(const String& realm, const String& uname, const String& uvalue, const String& pname) const
{
    return !(realm != m_realm || uname != m_usernameField || uvalue != m_usernameValue || pname != m_passwdField);
}

bool LoginData::match(const LoginData& b) const
{
    return partialMatch(b.m_realm, b.m_usernameField, b.m_usernameValue, b.m_passwdField) && (m_passwdValue == b.m_passwdValue);
}

//------------------------------------------------------------------------------
// FormLoginStore
//------------------------------------------------------------------------------
FormLoginStore::FormLoginStore() : m_needCommit(false), m_pbEncrypt(0), m_pbDecrypt(0)
{
    initialize();
}

FormLoginStore::~FormLoginStore()
{
    commit();
    clearCache();
    delete m_pbEncrypt;
    delete m_pbDecrypt;
}

void FormLoginStore::setSaveNotAllowed(const String& url)
{
    m_blackList.add(url);
}

bool FormLoginStore::saveAllowed(const String& url) const
{
    return !m_blackList.contains(url);
}

void FormLoginStore::add(const String& realm, const String& uname, const String& uvalue, const String& pname, const String& pvalue)
{
    LoginData* login = new LoginData(this, realm, uname, uvalue, pname, pvalue);
    HashSet<LoginData*>::iterator it = m_logins.begin(), end = m_logins.end();
    for (; it!=end; ++it) {
        if ((*it)->match(*login)) {
            delete login;
            return;
        } else if((*it)->partialMatch(realm, uname, uvalue, pname)) {
            // new password for the same login
            m_logins.remove(it);
            m_logins.add(login);
            m_needCommit = true;
            return;
        }
    }

    // new entry
    m_logins.add(login);
    m_needCommit = true;
}

void FormLoginStore::remove(const String& realm, const String& uname, const String& uvalue, const String& pname, const String& pvalue)
{
    LoginData login(this, realm, uname, uvalue, pname, pvalue);
    HashSet<LoginData*>::iterator it = m_logins.begin(), end = m_logins.end();
    for (; it!=end; ++it) {
        if ((*it)->match(login)) {
            break;
        }
    }

    if (it!=end) {
        m_logins.remove(it);
        m_needCommit = true;
    }
}

void FormLoginStore::removePartial(const String& realm, const String& uname, const String& uvalue, const String& pname)
{
    HashSet<LoginData*>::iterator it = m_logins.begin(), end = m_logins.end();
    for(; it!=end; ++it) {
        if ((*it)->partialMatch(realm, uname, uvalue, pname)) {
            m_logins.remove(it);
            m_needCommit = true;
            return;
        }
    }
}

bool FormLoginStore::contains(const String& realm, const String& uname, const String& uvalue, const String& pname, const String& pvalue)
{
    LoginData login(this, realm, uname, uvalue, pname, pvalue);
    HashSet<LoginData*>::iterator it = m_logins.begin(), end = m_logins.end();
    for (; it!=end; ++it) {
        if ((*it)->match(login)) {
            return true;
        }
    }
    return false;
}

void FormLoginStore::removeLoginsForRealm(const String& realm)
{
    HashSet<LoginData*>::iterator it = m_logins.begin(), end = m_logins.end();
    Vector<LoginData*> loginsToDelete;

    for (; it!=end; ++it) {
        if( (*it)->realm() == realm ) {
            loginsToDelete.append( *it );
        }
    }

    for (int i=0; i<loginsToDelete.size(); ++i) {
        m_logins.remove(loginsToDelete[i]);
        m_needCommit = true;
    }
}

bool FormLoginStore::search(const String& realm, const String& uname, const String& uvalue, const String& pname, String& pvalue)
{
    HashSet<LoginData*>::const_iterator it = m_logins.begin(), end = m_logins.end();
    for (; it != end; ++it) {
        if ((*it)->partialMatch(realm, uname, uvalue, pname)) {
            pvalue = (*it)->passwdValue();
            return true;
        }
    }
    return false;
}

// login file format: (Note: this is not the most compact format, 
// however we need to keep it this way in order to be backward compatible.)
// 
// # of blacklist entries
// url #1
// url #2
// ...
// # of logins
// encryption data for logins
//  #1 realm, usernamefield, usernamevalue, passwdfield, passwdvalue
//  #2 realm, usernamefield, usernamevalue, passwdfield, passwdvalue
// ...
void FormLoginStore::commit()
{
    if (!m_needCommit) return;

	RProcess myProcess;	
	TBuf <256>fileName;    
    fileName.Format(KLoginDatFile, myProcess.Identity());
    String s(fileName.Ptr(), fileName.Length());
	FileStreamOutput stream(s);
 
    if (stream.isOpened()) {
        // blacklist
        stream<<(m_blackList.size());
        for (HashSet<String>::const_iterator it=m_blackList.begin(), end = m_blackList.end(); it != end; ++it) {
            stream<<(*it);
        }

        // login count
        stream<<(m_logins.size());
        
        // write the encryption data if we have logins
        if (m_logins.size()>0) {
            if (!m_pbEncrypt)
                m_pbEncrypt = CPBEncryptElement::NewL(_L8(""), ECipher3DES_CBC);
            m_pbEncrypt->EncryptionData().ExternalizeL(stream.platformStream());
        }
        
        // now logins
        for (HashSet<LoginData*>::const_iterator it=m_logins.begin(), end = m_logins.end(); it != end; ++it) {
            stream<<(*(*it));
        }

        delete m_pbEncrypt;
        m_pbEncrypt = 0;
    }

    m_needCommit = false;
}

void FormLoginStore::initialize()
{
 	RProcess myProcess;	
	TBuf <256>fileName;    
    fileName.Format(KLoginDatFile, myProcess.Identity());
    String s(fileName.Ptr(), fileName.Length());
	FileStreamInput stream(s);
   
    if (stream.isOpened()) {
        // black-list
        int bsize = 0;
        stream>>bsize;
        for (int i=0; i<bsize; ++i) {
            String realm;
            stream>>realm;
            m_blackList.add(realm);
        }

        // logins
        int lsize = 0;
        stream>>lsize;

        CPBEncryptionData* encryptionData = 0;
        if (lsize) {
            // prepare for decryption 
            encryptionData = CPBEncryptionData::NewL(stream.platformStream());
            if (!m_pbDecrypt)
                m_pbDecrypt = CPBEncryptElement::NewL(*encryptionData, _L(""));
        }

        for (int i=0; i<lsize; ++i) {
            LoginData* d = new LoginData(this);
            stream>>(*d);
            m_logins.add(d);
        }

        // encryptionData is not needed any more
        delete encryptionData;
        delete m_pbDecrypt;
        m_pbDecrypt = 0;
    }
}

void FormLoginStore::clearCache()
{
    m_blackList.clear();
    HashSet<LoginData*>::const_iterator it = m_logins.begin(), end = m_logins.end();
    for(; it!=end; ++it)
        delete (*it);
    m_logins.clear();
}

void FormLoginStore::deleteAll()
{
    // clear the file data
    clearCache();
    m_needCommit = true;
    commit();
}

String FormLoginStore::encrypt(const String& src)
{
    if (!m_pbEncrypt)
        return src;

    CPBEncryptor* encryptor = m_pbEncrypt->NewEncryptLC();
    int len = encryptor->MaxFinalOutputLength(src.length()<<1);
    UChar* data = new UChar[len>>1];
    TPtrC8 in((const TUint8*)(src.characters()), src.length()<<1);
    TPtr8 out((TUint8*)data, 0, len);
    encryptor->ProcessFinalL(in, out);
    CleanupStack::Pop();  // encryptor
    delete encryptor;

    String result(data, len>>1);
    delete data;

    return result;
}

String FormLoginStore::decrypt(const String& src)
{
    if (!m_pbDecrypt) 
        return src;

	CPBDecryptor* decryptor = m_pbDecrypt->NewDecryptLC();
    int len = decryptor->MaxFinalOutputLength(src.length()<<1);
    len += (len&0x01);
    UChar* data = new UChar[len>>1];
    memset(data, 0x00, len);
    TPtrC8 in((const TUint8*)(src.characters()), src.length()<<1);
    TPtr8 out((TUint8*)data, 0, len);
    decryptor->ProcessFinalL(in, out);
    CleanupStack::Pop(); // decryptor
    delete decryptor;

    // now figure out the padding position
    int sz=0;
    for (int i=0; i<len>>1; ++i) {
        if (data[i] == 0) break;
        sz++;
    }
        
    String result(data, sz);
    delete data;

    return result;
}

}
