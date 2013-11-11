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


// INCLUDE FILES
#include "WebCharsetData.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TCharsetEntry supportedCharsetTable[] =
    {
    {KCharacterSetIdentifierIso88591,   _S("iso-8859-1") },
    {KCharacterSetIdentifierIso88591,   _S("iso-8859-1") },
    {KCharacterSetIdentifierIso88591,   _S("iso8859-1") },
    {0x101F873B,                        _S("windows-1252")},
    {KCharacterSetIdentifierUtf8,       _S("utf-8")},
    {KCharacterSetIdentifierGb2312,     _S("gb2312")},
    {KCharacterSetIdentifierBig5,       _S("big5")},
    {KCharacterSetIdentifierIso88592,   _S("iso-8859-2")},
    {KCharacterSetIdentifierIso88594,   _S("iso-8859-4")},
    {KCharacterSetIdentifierIso88595,   _S("iso-8859-5")},
    {KCharacterSetIdentifierIso88596,   _S("iso-8859-6")},
    {KCharacterSetIdentifierIso88597,   _S("iso-8859-7")},
    {KCharacterSetIdentifierIso88598,   _S("iso-8859-8")},
    {KCharacterSetIdentifierIso88599,   _S("iso-8859-9")},
    {KCharacterSetIdentifierIso885915,  _S("iso-8859-15")},
    {0x100059d6,                        _S("windows-1250")}, // no header file is provided with the converter windows-1250
    {0x100059D7,                        _S("windows-1251")}, // no header file is provided with the converter windows-1251
    {0x100059D8,                        _S("windows-1253")}, // no header file is provided with the converter windows-1253
    {0x100059D9,                        _S("windows-1254")}, // no header file is provided with the converter windows-1254
    {0x101F8547,                        _S("windows-1255")}, // no header file is provided with the converter windows-1255
    {0x101F8547,                        _S("iso-8859-8i")}, // no header file is provided with the converter windows-1255
    {0x101F8548,                        _S("windows-1256")}, // no header file is provided with the converter windows-1256
    {0x100059DA,                        _S("windows-1257")}, // no header file is provided with the converter windows-1257
    {0x102073B8,                        _S("windows-1258")}, // no header file is provided with the converter windows-1258
    {0x101F8549,                        _S("tis-620")}, // no header file is provided with the converter tis-620
    {KCharacterSetIdentifierShiftJis,   _S("shift_jis")},
    {KCharacterSetIdentifierShiftJis,   _S("shift-jis")},
    {KCharacterSetIdentifierShiftJis,   _S("x-sjis")},
    {KCharacterSetIdentifierJis,        _S("jis_x0201-1997")},
    {KCharacterSetIdentifierJis,        _S("jis_x0208-1997")},
    {KCharacterSetIdentifierEucJpPacked,_S("euc-jp")},
    {KCharacterSetIdentifierEucJpPacked,_S("x-euc-jp")},
    {KCharacterSetIdentifierIso2022Jp,  _S("iso-2022-jp")},
    {0x101F854A,                        _S("windows-874")}, // no header file is provided with the converter windows-874
    {0x101F8778,                        _S("koi8-r")}, // no header file is provided with the converter Russian
    {0x101F8761,                        _S("koi8-u")}, // no header file is provided with the converter Ukrainian
    {KCharacterSetIdentifierUcs2,       _S("iso-10646-ucs-2")},
    {KCharacterSetIdentifierUcs2,       _S("unicode")},
    {KCharacterSetIdentifierUnicodeBig, _S("ucs-2-big")},
    {KCharacterSetIdentifierUnicodeLittle,_S("ucs-2-little")},
    {0x1027508E,                        _S("iscii")},
    {0x2000E526,                        _S("euc-kr")},
    {0x2000E526,                        _S("csEUCKR")},
    {0x200113CD,                        _S("ko")},
    {0x200113CD,                        _S("ks_c_5601-1987")},
    {0x200113CD,                        _S("iso-ir-149")},
    {0x200113CD,                        _S("ks_c_5601-1989")},
    {0x200113CD,                        _S("ksc_5601")},
    {0x200113CD,                        _S("csksc56011987")},
    {0x200100FF,                        _S("ms949")},
    {0x200100FF,                        _S("cp949")},
    {0,                                 NULL}
};

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


unsigned int uidForCharset(TText* charset) 
{
    int i;
    int count = sizeof(supportedCharsetTable)/sizeof(TCharsetEntry);
    TPtrC charsetPtr(charset);
    for (i = 0; i < count; i++) {
        TPtrC supportedCharsetPtr(supportedCharsetTable[i].charsetName);
        if (charsetPtr.CompareF(supportedCharsetPtr) == 0) {
            break;
        }
    }
    if (i < count && supportedCharsetTable[i].charsetName != NULL) {
        return supportedCharsetTable[i].uid;
    }
    return 0;
}



const TText* charsetForUid(unsigned int uid)
{
    int i;
    int count = sizeof(supportedCharsetTable)/sizeof(TCharsetEntry);
    for (i = 0; i < count; i++) {
        if (uid == supportedCharsetTable[i].uid) {
            break;
        }
    }
    if (i < count && supportedCharsetTable[i].charsetName != NULL) {
        return supportedCharsetTable[i].charsetName;
    }
    return NULL;
}

//  End of File
