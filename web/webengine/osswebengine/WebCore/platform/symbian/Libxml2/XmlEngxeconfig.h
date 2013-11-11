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
* Description:  This file contains declarations and definitions
*
*/

/*
This file contains declarations and definitions that affect
all code of XML Engine: libxml2 and all C++ wrappers around it.

TODO: separate definitions that are designated for C++ wrappers only
      into another file, e.g. xecpp.h
*/

#ifndef XMLENGINE_XECONFIG_H_INCLUDED
#define XMLENGINE_XECONFIG_H_INCLUDED

//----------------------------------------------------
#define XMLENGINE_DOM
#define XMLENGINE_SAX
#define XMLENGINE_XPATH
#define XMLENGINE_XMLINTERFACE
#define XMLENGINE_XFORMS_EXTENSIONS
#define XMLENGINE_XPATH_EXTENSIONS

#define XMLENGINE_NODEFILTER
#define XMLENGINE_XMLSCHEMA_DATATYPES
#define XMLENGINE_XPATH_EXTENSIONS

//#define XMLENGINE_XSLT //unsuported in 3.1 release
#define LIBXML_HTML_ENABLED
// TODO: Disable/make configurable  LIBXML_ENABLE_NODE_LINEINFO for libxslt too
#define LIBXML_ENABLE_NODE_LINEINFO
#define XSLT_EXTENSION
#define XMLENGINE_NODEFILTER
#define XMLENGINE_XMLSCHEMA_DATATYPES
//----------------------------------------------------

// Affects both libxml2 and DOM
#define XMLENGINE_USE_DOMSTRINGARGUMENT

#endif /* XMLENGINE_XECONFIG_H_INCLUDED */
