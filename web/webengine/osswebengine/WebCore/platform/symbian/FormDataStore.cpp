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
#include "FormDataStore.h"
#include "FormFillController.h"
#include "SQLStatement.h"
_LIT(KDBFile, "c:\\private\\%08x\\form.db");

namespace WebCore {

// prepareSQLStatement() is a direct copy of readySQLStatement() in IconDatabase.cpp
inline void prepareSQLStatement(SQLStatement*& statement, SQLDatabase& db, const String& str)
{
    if (statement && (statement->database() != &db || statement->isExpired())) {
        if (statement->isExpired()) {
            //LOG(IconDatabase, "SQLStatement associated with %s is expired", str.ascii().data());
        }
        delete statement;
        statement = 0;
    }
    if (!statement) {
        statement = new SQLStatement(db, str);
        int result;
        result = statement->prepare();
        ASSERT(result == SQLResultOk);
    }
}


FormDataStore::FormDataStore(FormFillController*) 
    : m_setFormValueStatement(0), m_getFormValueStatement(0), m_deleteFormDataStatement(0)
{
    if(!openFormDatabase()) {
        //LOG_ERROR("Could not open form fill database.");
    }
}

FormDataStore::~FormDataStore()
{
    close();
}

static inline bool compareString(String a, String b)
{
    int i = 0;

    // empty string
    if( a.isEmpty() ) 
        return true;

    if( b.isEmpty() )
        return false;

    int len = a.length() < b.length() ? a.length() : b.length();
    while (i++<len) {
        if (a[i] < b[i])
            return true;
        else if(a[i] > b[i])
            return false;
    }
    
    // a is shorter?
    return (a.length() < b.length());
}

bool FormDataStore::search(const String& name_, const String& value_, Vector<String>& results)
{
    results.clear();
    if (!m_formDB.isOpen()) 
        return false;
    
    // so that we don't have to query the database on every search.
    if (m_lastQueriedName != name_)
        getFormValueQuery(name_);
    
    // no search the values in cache
    if (!m_valueCache.isEmpty()) {
        HashSet<String>::const_iterator it = m_valueCache.begin(), end = m_valueCache.end();
        for (; it != end; ++it) {
            if ((*it).startsWith(value_)) {
                results.append(*it);
            }
        }

        // FIXME: sorting the result? - maybe we could do the sorting in data table insersion.
        std::stable_sort(results.begin(), results.end(), compareString);
    }

    return true;
}

void FormDataStore::add(const String& name_, const String& value_)
{
    if (m_lastQueriedName == name_ && m_valueCache.contains(value_))
        return;
    if (!m_formDB.isOpen())
        return;

    // update the local cache
    if (m_lastQueriedName == name_)
        m_valueCache.add(value_);

    setFormValueQuery(name_, value_);
}

void FormDataStore::createDatabaseTable()
{
    if (!m_formDB.executeCommand("CREATE TABLE Input (name TEXT NOT NULL ON CONFLICT FAIL, value TEXT NOT NULL ON CONFLICT FAIL);")) {
        //LOG_ERROR("Could not create Input table in database (%i) - %s", m_formDB.lastError(), m_formDB.lastErrorMsg());
        m_formDB.close();
        return;
    }
}

void FormDataStore::setFormValueQuery(const String& name, const String& value)
{
    prepareSQLStatement( m_setFormValueStatement, m_formDB, "INSERT INTO Input (name, value) VALUES ((?), (?));");
    m_setFormValueStatement->bindText16(1, name, false);
    m_setFormValueStatement->bindText16(2, value, false);

    if (m_setFormValueStatement->step() != SQLResultDone) {
        LOG_ERROR("setFormValueQuery failed");
    }
    
    m_setFormValueStatement->reset();
}

void FormDataStore::getFormValueQuery(const String& name)
{
    prepareSQLStatement(m_getFormValueStatement, m_formDB, "SELECT Input.value FROM Input WHERE Input.name = (?);");
    m_getFormValueStatement->bindText16(1, name, false);

    int result = m_getFormValueStatement->step();

    // update the result cache
    m_valueCache.clear();
    
    while (result == SQLResultRow) {
        m_valueCache.add(m_getFormValueStatement->getColumnText16(0));
        result = m_getFormValueStatement->step();
    }

    if (result != SQLResultDone) {
        LOG_ERROR("getFormValueQuery failed");
    }
    
    m_getFormValueStatement->reset();
    m_lastQueriedName = name;
}

void FormDataStore::deleteFormDataQuery()
{
    prepareSQLStatement(m_deleteFormDataStatement, m_formDB, "DELETE FROM Input");
    int result = m_deleteFormDataStatement->step();
    if (result != SQLResultDone) {
        LOG_ERROR("deleteFormDataQuery failed");
    }
}

bool FormDataStore::openFormDatabase()
{
    if (m_formDB.isOpen()) {
        LOG_ERROR("Attempt to reopen the formDatabase which is already open.  Must close it first.");
        return false;
    }

    // create the database file in C drive
    // FIXME: the path shouldn't be hardcoded, move to data-caging directory
	RProcess myProcess;	
	TBuf <256>fileName;    
    fileName.Format(KDBFile, myProcess.Identity());
    String s(fileName.Ptr(), fileName.Length());
    if (!m_formDB.open(s)) {
        return false;
    }

    if (!m_formDB.tableExists("Input")) {
        m_formDB.clearAllTables();
        createDatabaseTable();
    }

    m_formDB.setSynchronous(SQLDatabase::SyncOff);
    m_formDB.setFullsync(false);

    return m_formDB.isOpen();
}

void FormDataStore::close()
{
    m_valueCache.clear();
    delete m_setFormValueStatement;
    m_setFormValueStatement = NULL;
    delete m_getFormValueStatement;
    m_getFormValueStatement = NULL;
    delete m_deleteFormDataStatement;
    m_deleteFormDataStatement = NULL;
    m_formDB.close();
}

void FormDataStore::deleteAll()
{
    m_valueCache.clear();
    deleteFormDataQuery();
}

} // END OF FILE
