/*
* Copyright (c) 2006, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Holds metadata, one version per widget.
*
*
*/

#include "WidgetEntry.h"
#include "WidgetRegistryConstants.h"
#include <s32file.h>
#include <f32file.h>
#include <apgtask.h>
//#include <widgetappdefs.rh>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
#define SAPISECURITYPROMPTNEEDED 0x0001
#define SAPIPROMPTLESS           0x0002
#define SAPIACCESSDENIED         0x0003

// MACROS

// LOCAL CONSTANTS AND MACROS

// for externalize
_LIT( KXmlPropStart, "<prop>" );
_LIT( KXmlPropEnd, "</prop>" );
_LIT( KXmlValStart, "<val>" );
_LIT( KXmlValEnd, "</val>" );
_LIT( KXmlTypeStart, "<type>" );
_LIT( KXmlTypeEnd, "</type>" );
_LIT( KXmlNewline, "\x0D\x0A" ); // DOS/Symbian style works with XML parsers

// for internalize
_LIT8( KXmlPropTag, "prop" );
_LIT8( KXmlValTag, "val" );
_LIT8( KXmlTypeTag, "type" );
_LIT( KXmlDataTypeBool, "bool" );
_LIT( KXmlDataTypeInt, "int" );
_LIT( KXmlDataTypeString, "string" );
_LIT( KXmlDataTypeUid, "uid" );

static const TInt KWidgetPropertyListVersion32 = 1;
static const TInt KWidgetPropertyListVersion71 = 3;
// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// ============================================================================
// CWidgetEntry::NewL()
// two-phase constructor
//
// @since 3.1
// ============================================================================
//
CWidgetEntry* CWidgetEntry::NewL()
    {
    CWidgetEntry *self = new ( ELeave ) CWidgetEntry();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ============================================================================
// CWidgetEntry::NewL()
// two-phase constructor
//
// @since 5.0
// ============================================================================
//    
CWidgetEntry* CWidgetEntry::NewL( RPointerArray<CWidgetPropertyValue>** aProps )
{
    CWidgetEntry* tmp = NewL();
    for ( TInt i = 0; i < (*aProps)->Count(); i++ )
    {
        CWidgetPropertyValue* value = CWidgetPropertyValue::NewL();
        tmp->iPropertyValues.AppendL( value );
        (*tmp)[i].iType = (**aProps)[i]->iType;
        (*tmp)[i].iValue = (**aProps)[i]->iValue; // shallow copy of strings
        (**aProps)[i]->iType = EWidgetPropTypeUnknown;
        delete (**aProps)[i];
    }
    (*aProps)->Close();
    delete *aProps;
    *aProps = NULL;
    return tmp;
}

// ============================================================================
// CWidgetEntry::CWidgetEntry()
// C++ constructor
//
// @since 3.1
// ============================================================================
//
CWidgetEntry::CWidgetEntry()
    : iPropertyValues( EWidgetPropertyIdCount ),
      iBlanketPermGranted ( EFalse),
      iFullView ( EFalse),
      iMiniView ( EFalse)
    {
    }

// ============================================================================
// CWidgetEntry::~CWidgetEntry()
// destructor
//
// @since 3.1
// ============================================================================
//
CWidgetEntry::~CWidgetEntry()
    {
    iPropertyValues.ResetAndDestroy();
    }

// ============================================================================
// CWidgetEntry::ConstructL()
// symbian second phase constructor
//
// @since 3.1
// ============================================================================
//
void CWidgetEntry::ConstructL()
    {
    }

// ============================================================================
// CWidgetEntry::InternalizeBinaryL()
// read from persistent storage(file)
//
// @since 3.1
// ============================================================================
//
void CWidgetEntry::InternalizeBinaryL( RReadStream& aReadStream )
    {
    for (TInt i = iPropertyValues.Count() ; i < EWidgetPropertyIdCount; i++)
        {
        CWidgetPropertyValue* val = CWidgetPropertyValue::NewL();
        CleanupStack::PushL(val);
        iPropertyValues.AppendL( val );
        CleanupStack::Pop( val ); 
        }
    // read what should be EWidgetPropertyListVersion
    (*this)[0].DeserializeL( aReadStream );

    // For now, leave if version doesn't match compiled-in version,
    // FUTURE do something smarter
    //WIDGETPROPERTYLISTVERSION is 1 in case of Tiger engine and 3 in case of Leopard engine. Therefore, modifying the check such that 
    //when the Version id is 1 or 3, we do not treat the file as corrupt.
    if ( ( EWidgetPropTypeUnknown == (*this)[EWidgetPropertyListVersion].iType )
         || ( (KWidgetPropertyListVersion32 != (*this)[EWidgetPropertyListVersion] ) && (KWidgetPropertyListVersion71 != (*this)[EWidgetPropertyListVersion] )) )
        {
        User::Leave( KErrCorrupt );
        }

    // fill property values array
    for ( TInt i = 1; i < EWidgetPropertyIdCount; ++i )
        {
        (*this)[i].DeserializeL( aReadStream );
        }
    }


// ============================================================================
// CWidgetEntry::InternalizeXmlL()
// read from persistent storage(file)
//
// @since 5.0
// ============================================================================
//
void CWidgetEntry::InternalizeXmlL( RFs& aFileSession,
                                    xmlDocPtr aDoc,
                                    xmlNode* n,
                                    CWidgetRegistryXml* aXmlProcessor )
    {
    // <prop>name<val>value<type>typename</type></val></prop>
    //
    // prop subtree traversal assumes strict adherence to the
    // prototype structure, otherwise code leaves with corrupt code
    for ( ;
          n;
          )
        {
        // permit some non element stuff (comment, whitespace...) before <prop>
        while ( n && ( n->type != XML_ELEMENT_NODE ) )
            {
            n = n->next;
            }
        if ( NULL == n )
            {
            for (TInt i = iPropertyValues.Count(); i < EWidgetPropertyIdCount; i++)
                {
                CWidgetPropertyValue* val = CWidgetPropertyValue::NewL();
                CleanupStack::PushL(val);
                iPropertyValues.AppendL( val );
                CleanupStack::Pop(); // val
                }
            return;
            }
        TPtrC8 propTag( n->name );
        if ( 0 != propTag.Compare( KXmlPropTag() ) )
            {
            // TODO unrecognized subtree?
            return;
            }
        // TODO validate n->children != NULL and type XML_TEXT_NODE
        HBufC* name;
        aXmlProcessor->GetContentL( aFileSession, aDoc, n->children, &name );

        // get value array index (TWidgetPropertyId) for name
        TPtr namePtr( name->Des() );
        TInt propId =
            aXmlProcessor->GetPropertyId( namePtr );
        delete name;
        name = NULL;
        if ( EWidgetPropertyIdInvalid == propId )
            {
            User::Leave( KErrNoMemory );
            }

        for (TInt i = iPropertyValues.Count(); i <= propId; i++)
            {
            CWidgetPropertyValue* val = CWidgetPropertyValue::NewL();
            CleanupStack::PushL(val);
            iPropertyValues.AppendL( val );
            CleanupStack::Pop(); // val
            }

        n = n->children->next; // down to val
        if ( NULL == n )
            {
            User::Leave( KErrCorrupt );
            }
        TPtrC8 valTag( n->name );
        if ( 0 != valTag.Compare( KXmlValTag() ) )
            {
            User::Leave( KErrCorrupt );
            }
        if (propId >= EWidgetPropertyIdCount) // unsupported property
            {
            HBufC* value = NULL;
            if (n->children)
                {
                aXmlProcessor->GetTextContentAsStringL( aFileSession, aDoc, n->children, &value );
                }
            else
                {
                value = KNullDesC().AllocL();
                }
            (*this)[propId].iValue.s = value;
            (*this)[propId].iType = EWidgetPropTypeBlob;
            n = (n->parent)->next; // up two and next sibling
            continue;
            }
        // TODO validate n->children != NULL and type XML_TEXT_NODE
        HBufC* value;
        aXmlProcessor->GetContentL( aFileSession, aDoc, n->children, &value );
        CleanupStack::PushL( value );

        n = n->children->next; // down to type
        if ( NULL == n )
            {
            User::Leave( KErrCorrupt );
            }
        TPtrC8 typeTag( n->name );
        if ( 0 != typeTag.Compare( KXmlTypeTag() ) )
            {
            User::Leave( KErrCorrupt );
            }
        // TODO validate n->children != NULL and type XML_TEXT_NODE
        HBufC* type;
        aXmlProcessor->GetContentL( aFileSession, aDoc, n->children, &type );
        CleanupStack::PushL( type );

        // now have: name, value, type
        // convert type string to TWidgetPropertyType
        //
        // assume void/unknown is not put in XML format so anything
        // not recognized should be handled like other unrecognized
        // subtree
        TWidgetPropertyType typeEnum = EWidgetPropTypeUnknown;
        if ( 0 == type->Des().Compare( KXmlDataTypeBool() ) )
            {
            typeEnum = EWidgetPropTypeBool;
            }
        else if ( 0 == type->Des().Compare( KXmlDataTypeInt() ) )
            {
            typeEnum = EWidgetPropTypeInt;
            }
        else if ( 0 == type->Des().Compare( KXmlDataTypeString() ) )
            {
            typeEnum = EWidgetPropTypeString;
            }
        else if ( 0 == type->Des().Compare( KXmlDataTypeUid() ) )
            {
            typeEnum = EWidgetPropTypeUid;
            }
        CleanupStack::PopAndDestroy( type );

        // TODO handle unknown type due to future extensions: add prop
        // subtree to list of unrecognized subtrees

        // set prop according to type
        switch ( typeEnum )
            {
        case EWidgetPropTypeBool:
            if ( 0 == value->Des().Compare( _L("0") ) )
                {
                (*this)[propId].iValue.i = 0;
                }
            else
                {
                (*this)[propId].iValue.i = 1;
                }
            break;
        case EWidgetPropTypeInt:
            {
            TLex toInt( value->Des() );
            TInt k;
            if ( KErrNone != toInt.Val( k ) )
                {
                User::Leave( KErrCorrupt );
                }
            (*this)[propId].iValue.i = k;
            }
            break;
        case EWidgetPropTypeString:
            (*this)[propId].iValue.s = value;
            break;
        case EWidgetPropTypeUid:
            {
            TLex toUid( value->Des() );
            TInt u;
            if ( KErrNone != toUid.Val( u ) )
                {
                User::Leave( KErrCorrupt );
                }
            (*this)[propId].iValue.uid = TUid::Uid( u );
            }
            break;
            };

        (*this)[propId].iType = typeEnum;

        CleanupStack::Pop( value );
        if ( EWidgetPropTypeString != typeEnum )
            {
            delete value;
            }

        n = ((n->parent)->parent)->next; // up two and next sibling
        }
    }


// ============================================================================
// CWidgetEntry::ExternalizeBinaryL()
// write to persistent storage(file)
//
// @since 3.1
// ============================================================================
//
void CWidgetEntry::ExternalizeBinaryL( RWriteStream& aWriteStream )
    {
    TInt i = 0;
    for ( ; i < EWidgetPropertyIdCount; ++i )
        {
        (*this)[i].SerializeL( aWriteStream );
        }
    }

void CWidgetEntry::ExternalizeXmlL( RWriteStream& aWriteStream,
                                    CWidgetRegistryXml* aXmlProcessor,
                                    RFs& aFileSession )
    {
    xmlDocPtr doc = NULL; // not really used
    TInt i = 0;
    // For each property, write an XML entry
    for ( ; i < EWidgetPropertyIdCount; ++i )
        {
        // skip props without values
        if ( EWidgetPropTypeUnknown == (*this)[i].iType )
            {
            continue;
            }

        TBuf<KMaxFileName> str;
        // <prop>name
        str.Append( KXmlPropStart );
        str.Append( aXmlProcessor->XmlPropertyName( i ) );  
        aWriteStream.WriteL( str );  
        // <val>value
        str.SetLength( 0 );
        str.Append( KXmlValStart );
        aWriteStream.WriteL( str );
        str.SetLength( 0 );
        switch ( (*this)[i].iType )
            {
        case EWidgetPropTypeBool:
            {
            TInt j = (*this)[i];
            if ( 0 == j )
                {
                str.Append( _L("0") );
                }
            else
                {
                str.Append( _L("1") );
                }
            }
            break;
        case EWidgetPropTypeInt:
            {
            TInt k = (*this)[i];
            str.AppendFormat( _L("%d"), k );
            }
            break;
        case EWidgetPropTypeString:
            {
			// start an encoding process for special characters for xml writing
			// the special characters are:
			// '&', Ampersand:   &amp; 
			// '>', greater-than:   &gt; 
			// '<', less-than:   &lt; 
			// ''', apostrophe:   &apos; 
			// '"', quote:   &quot;

			TBuf<KMaxFileName> orig;
			orig.Append((*this)[i]);
			TUint16 * cur = (TUint16 *)orig.Ptr();
			TUint16 * out = (TUint16 *)str.Ptr();
			TInt len = orig.Length();
			for ( TInt i = 0; i < orig.Length(); i++, cur++ )
			{
			// By default one have to encode at least '<', '>', '"' and '&' !
			 if (*cur == '<') {
				 *out++ = '&';
				 *out++ = 'l';
				 *out++ = 't';
				 *out++ = ';';
				 len += 3;
			 } else if (*cur == '>') {
				 *out++ = '&';
				 *out++ = 'g';
				 *out++ = 't';
				 *out++ = ';';
				 len += 3;
			 } else if (*cur == '&') {
				 *out++ = '&';
				 *out++ = 'a';
				 *out++ = 'm';
				 *out++ = 'p';
				 *out++ = ';';
				 len += 4;
			 } else if (*cur == '"') {
				 *out++ = '&';
				 *out++ = 'q';
				 *out++ = 'u';
				 *out++ = 'o';
				 *out++ = 't';
				 *out++ = ';';
				 len += 5;
			 } else if (*cur == '\'') {
				 *out++ = '&';
				 *out++ = 'a';
				 *out++ = 'p';
				 *out++ = 'o';
				 *out++ = 's';
				 *out++ = ';';
				 len += 5;
			 } else if (*cur == '\r') {
				 *out++ = '&';
				 *out++ = '#';
				 *out++ = '1';
				 *out++ = '3';
				 *out++ = ';';
				 len += 4;
			 } else {
				 *out++ = *cur;
			 }
			}
			str.SetLength(len );
            break;
			}
        case EWidgetPropTypeUid:
            const TUid& u = (*this)[i];
            TInt l = u.iUid;
            str.AppendFormat( _L("%d"), l );
            break;
            };
        aWriteStream.WriteL( str );

        // <type>type
        str.SetLength( 0 );
        str.Append( KXmlTypeStart );
        switch ( (*this)[i].iType )
            {
        case EWidgetPropTypeBool:
            str.Append( _L("bool") );
            break;
        case EWidgetPropTypeInt:
            str.Append( _L("int") );
            break;
        case EWidgetPropTypeString:
            str.Append( _L("string") );
            break;
        case EWidgetPropTypeUid:
            str.Append( _L("uid") );
            break;
            };
        aWriteStream.WriteL( str );

        // </type></val></prop>
        str.SetLength( 0 );
        str.Append( KXmlTypeEnd );
        str.Append( KXmlValEnd );
        str.Append( KXmlPropEnd );
        str.Append( KXmlNewline );
        aWriteStream.WriteL( str );
        }

    for ( ; i < iPropertyValues.Count(); ++i )
        {
        TBuf<KMaxFileName> str;
        // <prop>name
        str.Append( KXmlPropStart );
        str.Append( aXmlProcessor->XmlPropertyName( i ) );
        aWriteStream.WriteL( str );

        // <val>value
        str.SetLength( 0 );
        str.Append( KXmlValStart );
        aWriteStream.WriteL( str );
        str.SetLength( 0 );
        const HBufC* s = (iPropertyValues[i])->iValue.s;
        str.Append( *s );
        aWriteStream.WriteL( str );
        str.SetLength( 0 );
        str.Append( KXmlValEnd );
        str.Append( KXmlPropEnd );
        str.Append( KXmlNewline );
        aWriteStream.WriteL( str );
        }
    }


// ============================================================================
// CWidgetEntry::Active()
// Is widget running? 0 if not, non zero if running.
//
// @since 3.1
// ============================================================================
//
TInt CWidgetEntry::ActiveL()
    {
    if ( iActive )
        {
        // check that WidgetUI didn't crash, this assumes all widgets
        // in the registry are running under WidgetUI
        RWsSession wsSession;
        User::LeaveIfError( wsSession.Connect() );
        CleanupClosePushL( wsSession );
        TApaTaskList taskList( wsSession );
        TApaTask task = taskList.FindApp( KUidWidgetUi );
        if ( EFalse == task.Exists() )
            {
            // widget UI crashed, reset active
            iActive = 0;
            }
        CleanupStack::PopAndDestroy( &wsSession );
        }
    return iActive;
}

// ============================================================================
// CWidgetEntry::SapiAccessState()
// Does widget have sapi access? after accepting security prompt, promptless or no access.
//
// @since 5.0
// ============================================================================
//
TInt CWidgetEntry::SapiAccessState()
    {
    if( GetFullViewState() && !GetMiniViewState())
        {
        return SAPISECURITYPROMPTNEEDED;
        }
    else if ( GetMiniViewState() && GetBlanketPermGranted() )
        {
        return SAPIPROMPTLESS;
        }
    else
        {
        return SAPIACCESSDENIED;
        }
        
    }

//  End of File
