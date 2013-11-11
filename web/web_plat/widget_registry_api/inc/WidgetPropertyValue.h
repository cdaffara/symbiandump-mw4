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
* Description:  Holds property values.
*
*
*/


#ifndef WIDGETPROPERTYVALUE_H
#define WIDGETPROPERTYVALUE_H

#include <e32base.h>
#include <s32strm.h>
#include "WidgetRegistryConstants.h"

// serialize length = header + value
const TInt32 KWidgetPropertyValSerializeMaxLength = 32 + KWidgetRegistryVal;

enum TWidgetPropertyType
    {
    EWidgetPropTypeUnknown = 0,
    EWidgetPropTypeBool,
    EWidgetPropTypeInt,
    EWidgetPropTypeString,
    EWidgetPropTypeUid,
    EWidgetPropTypeBlob
    };

/**
*  CWidgetData
*  @since 3.1
*/
class CWidgetPropertyValue : public CBase
    {
public:
    /**
     * Two phase constructor
     * @return none
     * @since 3.1
     **/
    static CWidgetPropertyValue* NewL()
        {
        CWidgetPropertyValue *self = new ( ELeave ) CWidgetPropertyValue();
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop();
        return self;
        }

    /**
     * Destructor
     * @return none
     * @since 3.1
     **/
    ~CWidgetPropertyValue()
        {
        if ( EWidgetPropTypeString == iType )
            {
            delete iValue.s;
            }
        }

    operator TInt() const
        {
        if ( EWidgetPropTypeInt == iType )
            {
            return iValue.i;
            }
        return 0;
        }

    operator const TDesC&()
        {
        if ( EWidgetPropTypeString == iType )
            {
            return *iValue.s;
            }
        return KNullDesC;
        }

    operator const TUid&()
        {
        if ( EWidgetPropTypeUid == iType )
            {
            return iValue.uid;
            }
        return KNullUid;
        }

    CWidgetPropertyValue& operator= (TInt i)
        {
        iValue.i = i;
        iType = EWidgetPropTypeInt;
        return *this;
        }

    CWidgetPropertyValue& operator= (const TDesC& s)
        {
        // self-assignment check
        if ( iValue.s != &s )
            {
            delete iValue.s;
            iValue.s = s.AllocL();
            iType = EWidgetPropTypeString;
            }
            return *this;
        }

    CWidgetPropertyValue& operator= (TUid uid)
        {
        iValue.uid = uid;
        iType = EWidgetPropTypeUid;
        return *this;
        }

    void Reset()
        {
        if ( EWidgetPropTypeString == iType )
            {
            delete iValue.s;
            iValue.s = NULL;
            }
        iType = EWidgetPropTypeUnknown;
        }

    void SerializeL( RWriteStream& aStream ) const
        {
        switch ( iType )
            {
        case EWidgetPropTypeUnknown:
            aStream.WriteInt32L( 7 );
            aStream.WriteL( _L("unknown"), 7 );
            break;
        case EWidgetPropTypeBool:
            aStream.WriteInt32L( 4 );
            aStream.WriteL( _L("bool"), 4 );
            aStream.WriteInt32L( iValue.i );
            break;
        case EWidgetPropTypeInt:
            aStream.WriteInt32L( 3 );
            aStream.WriteL( _L("int"), 3 );
            aStream.WriteInt32L( iValue.i );
            break;
        case EWidgetPropTypeString:
            aStream.WriteInt32L( 3 );
            aStream.WriteL( _L("str"), 3 );
            aStream.WriteInt32L( iValue.s->Length() );
            aStream.WriteL( *(iValue.s), iValue.s->Length() );
            break;
        case EWidgetPropTypeUid:
            aStream.WriteInt32L( 3 );
            aStream.WriteL( _L("uid"), 3 );
            aStream.WriteInt32L( iValue.uid.iUid );
            break;
        default:
            User::Leave( KErrCorrupt );
            }
        }

    void DeserializeL( RReadStream& aStream )
        {
        Reset();
        TInt x = 0;
        TInt len = aStream.ReadInt32L();
        if ( len > KMaxFileName )
            {
            User::Leave( KErrCorrupt );
            }
        TFileName buf;
        aStream.ReadL( buf, len );
        if ( 0 == buf.Compare( _L("unknown") ) )
            {
            iType = EWidgetPropTypeUnknown;
            iValue.i = 0;
            }
        else if ( 0 == buf.Compare( _L("bool") ) )
            {
            iType = EWidgetPropTypeBool;
            iValue.i = aStream.ReadInt32L();
            }
        else if ( 0 == buf.Compare( _L("int") ) )
            {
            iType = EWidgetPropTypeInt;
            iValue.i = aStream.ReadInt32L();
            }
        else if ( 0 == buf.Compare( _L("str") ) )
            {
            iType = EWidgetPropTypeString;
            len = aStream.ReadInt32L();
            if ( len <= KMaxFileName )
                {
                HBufC* vBuf = HBufC::NewLC( len );
                TPtr16 p = vBuf->Des();
                aStream.ReadL( p, len );
                iValue.s = vBuf;
                CleanupStack::Pop(); // vBuf
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            }
        else if ( 0 == buf.Compare( _L("uid") ) )
            {
            iType = EWidgetPropTypeUid;
            TUid uid = TUid::Uid( aStream.ReadInt32L() );
            iValue.uid = uid;
            }
        else
            {
            User::Leave( KErrCorrupt );
            }
        }

protected:
    /**
     * Constructor
     */
    CWidgetPropertyValue() {}

    /**
     * 2-phase constructor
     */
    void ConstructL() {}

private:
    // no copy constructor
    CWidgetPropertyValue( const CWidgetPropertyValue& );
    // no assignment
    CWidgetPropertyValue& operator=( const CWidgetPropertyValue& );

public:
    TWidgetPropertyType iType;
    union
        {
        TInt i;
        HBufC* s;
        TUid uid;
        } iValue;
    };

// RPointerArray<CWidgetPropertyValue>  RWidgetPropertyValues(EWidgetPropertyIdCount)
// and use is ended by ResetAndDestroy()


#endif // WIDGETPROPERTYVALUE_H
