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

#ifndef __FORMLOGINSTORE_H__
#define __FORMLOGINSTORE_H__

#include "StreamSymbian.h"
#include "PlatformString.h"
#include "HashSet.h"

using namespace WTF;

class CPBEncryptElement;
class CPBEncryptionData;

namespace WebCore {

class FormLoginStore;

class LoginData : public Streamable
{
public:
    LoginData(FormLoginStore*);
    LoginData(FormLoginStore*, const String&, const String&, const String&, const String&, const String&);
    ~LoginData();

    void read(StreamInput& is);
    void write(StreamOutput& os) const;

    // match
    bool partialMatch(const String& realm, const String& uname, const String& uvalue, const String& pname) const;
    bool match(const LoginData&) const;

    // accessor
    const String& passwdValue() const           { return m_passwdValue; }
    const String& realm() const                 { return m_realm; }

private:
    String m_realm;
    String m_usernameField;
    String m_usernameValue;
    String m_passwdField;
    String m_passwdValue;
    FormLoginStore* m_store;
};

class FormLoginStore
{
public:
    FormLoginStore();
    ~FormLoginStore();

    void setSaveNotAllowed(const String& url);
    bool saveAllowed(const String& url) const;

    // save login info to the buffer
    void add(const String& realm, const String& uname, const String& uvalue, const String& pname, const String& pvalue);
    void remove(const String& realm, const String& uname, const String& uvalue, const String& pname, const String& pvalue);
    void removePartial(const String& realm, const String& uname, const String& uvalue, const String& pname);
    bool contains(const String& realm, const String& uname, const String& uvalue, const String& pname, const String& pvalue);
    bool search(const String& realm, const String& uname, const String& uvalue, const String& pname, String& pvalue);
    
    void removeLoginsForRealm(const String& realm);
    void deleteAll();

    void commit();

    // encryption
    String encrypt(const String&);
    String decrypt(const String&);

private:
    void initialize();
    void clearCache();

    HashSet<String>     m_blackList;
    HashSet<LoginData*> m_logins;
    bool                m_needCommit;
    CPBEncryptElement*  m_pbEncrypt;
    CPBEncryptElement*  m_pbDecrypt;
};

}

#endif //!__FORMLOGINSTORE_H__
