/**
* Copyright (c) 2010 Sasken Communication Technologies Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the "{License}"
* which accompanies  this distribution, and is available
* at the URL "{LicenseUrl}".
*
* Initial Contributors:
* Narasimhulu Kavadapu, Sasken Communication Technologies Ltd - Initial contribution
*
* Contributors:
*
* Description:
* XML Parser class
*/

#ifndef FB_XML_HANDLER_H
#define FB_XML_HANDLER_H

#include <QObject>
#include <QString>
#include <QXmlDefaultHandler>
#include <QVariantList>

#include "authAppConstants.h"

// FORWARD DECLARATIONS
class FBRequest;

///////////////////////////////////////////////////////////////////////////////////////////////////
class FBXMLHandler : public QObject, public QXmlDefaultHandler
{
    Q_OBJECT

private:
    QVariantList        iStack;
    QList<QString>      iNameStack;
    QVariant            iRootObject;
    QString             iRootName;
    QString             iChars;

    QString         iParseErrorMessage;
    bool            iError;

public: /* class functions */

    FBXMLHandler();
    ~FBXMLHandler();

    inline const QString& rootName() const { return iRootName; }
    inline QVariant rootObject() const { return iRootObject; }

    inline bool parseError() const { return iError; }

private:
    /* methods from QXmlDefaultHandler */
    bool startElement( const QString & namespaceURI,
                       const QString & localName,
                       const QString & qName,
                       const QXmlAttributes & atts);
    bool characters(const QString& text);
    bool endElement( const QString & namespaceURI,
                     const QString & localName,
                     const QString & qName );

    /* methods from QXmlErrorHandler */
    bool error(const QXmlParseException& exception);
    bool fatalError(const QXmlParseException& exception);

    /* private functions*/
    const QString& topName() const;
    void flushCharacters();
    QVariant topObject(bool aCreate);

    QVariant topContainer();


    void initWhiteSpaceHash();
    QHash<QChar, bool> iWhiteSpaceAndNewLineCharSet;

};

#endif // FB_XML_HANDLER_H
