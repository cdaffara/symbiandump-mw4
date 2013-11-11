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

#ifndef __FORMDATASTORE_H__
#define __FORMDATASTORE_H__

#include <wtf/Vector.h>
#include <wtf/HashSet.h>
#include "SQLDatabase.h"
#include "PlatformString.h"
#include "StringHash.h"


namespace WebCore {

class SQLDatabase;
class FormFillController;
class String;
class SQLStatement;

class FormDataStore
{
public:
    FormDataStore(FormFillController*);
    ~FormDataStore();

    bool search(const String& name_, const String& value_, Vector<String>& results);
    void add(const String& name_, const String& value_);
    void deleteAll();

private:
    FormDataStore(const FormDataStore&);                // not implemented
    FormDataStore& operator=(const FormDataStore&);     // not implemented

    // database mainpulation
    void close();
    bool openFormDatabase();
    void createDatabaseTable();
    void setFormValueQuery(const String& name, const String& value);
    SQLStatement *m_setFormValueStatement;
    void getFormValueQuery(const String& name);
    SQLStatement *m_getFormValueStatement;
    void deleteFormDataQuery();
    SQLStatement *m_deleteFormDataStatement;

    SQLDatabase m_formDB;

    // caching
    String m_lastQueriedName;
    HashSet<String> m_valueCache;
};

}

#endif //!__FORMDATASTORE_H__
