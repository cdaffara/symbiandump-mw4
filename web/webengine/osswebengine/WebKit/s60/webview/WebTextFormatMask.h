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

#ifndef __WEBTEXTFORMATMASK_H__
#define __WEBTEXTFORMATMASK_H__

#include <unicode/umachine.h>
#include <e32cmn.h>
#include <ctype.h>

typedef enum {

    ELeLoSymPuc,
    ELeUpSymPuc,   
    ENumSymPuc,    
    ENumChar,      
    ELeLoNumSymPuc,
    ELeUpNumSymPuc,
    EAnyLow,
    EAnyUpper,
    EStatic,
    ENoFormat 

}TInputFormatMaskType;  

namespace WebCore
{
    class Frame;
    class String;
}

class WebTextFormatMask;

class MaskBase
{
public:
    friend class WebTextFormatMask;

    MaskBase() : m_next(0) {}
    virtual ~MaskBase()         { m_next=NULL; }

    virtual bool check(UChar) = 0;
    virtual TInputFormatMaskType getInputFormatMaskType() = 0;
    virtual MaskBase* nextMask()            { return m_next; }
    virtual bool isComposite()              { return false; }
    virtual int multitude() const  { return 1; }

protected:
    MaskBase* m_next;

private:
    MaskBase(const MaskBase&);              // not implemented
    MaskBase& operator=(const MaskBase&);   // not implemented
};

class MaskSingle : public MaskBase
{
public:
    MaskSingle(TInputFormatMaskType t) : m_type(t)     {}
    bool check(UChar);
    TInputFormatMaskType getInputFormatMaskType() { return m_type; };
    
private:
    TInputFormatMaskType m_type;
};

class MaskStatic : public MaskBase
{
public:
    MaskStatic(UChar ch) : m_char(ch)       {}
    bool check(UChar);
    UChar getStatic() { return m_char; }
    TInputFormatMaskType getInputFormatMaskType() { return EStatic; };

private:
    UChar m_char;
};

class MaskComposite : public MaskSingle
{
public:
    MaskComposite(TInputFormatMaskType, int);

    MaskBase* nextMask();    
    bool isComposite()              { return true; }
    int multitude() const  { return m_length; }

private:
    int m_offset;
    int m_length;
};

class WebTextFormatMask
{
public:
    struct ErrorBlock
    {
        int m_start;
        int m_extent;

        ErrorBlock() : m_start(-1), m_extent(-1)    {}
        void set(int s, int e)                      { m_start = s; m_extent = e; }
    };

    WebTextFormatMask(const WebCore::String&, bool);
    ~WebTextFormatMask();

    bool checkText(const WebCore::String&, ErrorBlock&);
    MaskBase* getMask(int aOffset);
    
public:    
    int getMultitude();
    TInputFormatMaskType getInputFormatMaskType(WebCore::Frame *frame, int aOffset);
       
public:
    static bool checkTelWtaiNumber(const WebCore::String&, bool);

private:
    void buildMaskList(const WebCore::String&);
    bool appendMask(MaskBase* m);

    void clearMaskList();

    bool createMask(TInputFormatMaskType, int&);
    bool createStaticMask(const UChar*&);
    int parseMultitude(const UChar*&, bool&);

private:
    MaskBase*   m_masks;
    MaskBase*   m_currentMask;
    bool        m_acceptAll;
    bool        m_inputRequired;
};

#endif
