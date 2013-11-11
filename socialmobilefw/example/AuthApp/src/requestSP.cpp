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
* class to handle calls to rest Server API's
*/

#include "requestSP.h"
#include "sessionSP.h"
#include "xmlParser.h"
#include "errorCodes.h"

#include <QNetworkRequest>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QCryptographicHash>
#include <QtAlgorithms>
#include <QDebug>
///////////////////////////////////////////////////////////////////////////////////////////////////
// global

static const QString kAPIVersion = "1.0";
static const QString kAPIFormat = "XML";
static const char kUserAgent[] = "FacebookConnect";
static const QString kStringBoundary = "3i2ndDfv2rTHiSisAbouNdArYfORhtTPEefj3q2f";

static const double kTimeoutInterval = 180.0;


///////////////////////////////////////////////////////////////////////////////////////////////////

static bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
    return s1.toLower() < s2.toLower();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Static class functions

FBRequest* FBRequest::request()
{
    return FBRequest::requestWithSession(FBSession::session());
}

FBRequest* FBRequest::requestWithSession (FBSession* aSession)
{
    FBRequest* request = new FBRequest(aSession);
    return request;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// instance public functions
FBRequest::FBRequest(FBSession* aSession) : iSession ( aSession ), iNetworkAccessManager ( this )
{}

const QDateTime& FBRequest::timeStamp() const
{
    return iTimestamp;
}

void FBRequest::connect()
{
	qDebug()<<"Inside FBRequest::connect()";
    emit requestLoading();

    QString url ;
    if (iMethod.length())
        url = iUrl;
    else
        url = generateGetURL();

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", kUserAgent);

    /* from the Qt docs on QNetworkAccessManager:
       QNetworkAccessManager by default does not have a set cache.
       Qt provides a simple disk cache, QNetworkDiskCache, which can be used.

       However we will not use it.*/

    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

    iTimestamp = QDateTime::currentDateTime();

    //if (iMethod.length())
    //{
        const QString contentType = "multipart/form-data; boundary=" + kStringBoundary;
        request.setRawHeader("Content-Type", contentType.toUtf8());

        /* connect all signals from iNetWorkAccessManager to this */
        QByteArray postBody ;
        generatePostBody (postBody);
        
        pbar = new progressbar;
        pbar->show();
        
        QNetworkReply* reply = iNetworkAccessManager.post(request, postBody);

        QObject::connect(reply, SIGNAL(finished()),  this, SLOT(networkReplyFinished()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                         this, SLOT(networkReplyError(QNetworkReply::NetworkError)));

    //}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// instance private functions
QString FBRequest::md5(const QString& aData)
{
    QByteArray byteArray;
    byteArray.insert(0, aData.toUtf8());

    QByteArray md5Hash = QCryptographicHash::hash(byteArray,QCryptographicHash::Md5 ).toHex();
    QString returnString ( md5Hash );

    return returnString;
}

bool FBRequest::isSpecialMethod() const {
    return ( iMethod.compare("facebook.auth.getSession", Qt::CaseInsensitive) == 0
             ||   iMethod.compare("facebook.auth.createToken", Qt::CaseInsensitive) == 0 );
}

QString FBRequest::urlForMethod (const QString& aMethod) const {

	return iSession->apiURL();
}

QString FBRequest::generateGetURL() const
{
	qDebug()<<"Inside FBRequest::generateGetURL()";
    const QUrl url(iUrl);
    const QString queryPrefix = url.hasQuery() ? "&" : "?";

    QStringList pairs;
    DictionaryIterator i(iParams);

    while (i.hasNext()) {
        i.next();
        pairs << i.key().toUtf8() + "=" + i.value().toUtf8();
    }

    return iUrl + queryPrefix + pairs.join("&");
}

QString FBRequest::generateCallId() const {
    QDateTime dateTime = QDateTime::currentDateTime();
    uint secs = dateTime.toTime_t();
    QString result = QString::number(secs, 10);
    return result;
}

QString FBRequest::generateSig()
{
    QString joined;
    QStringList keys = iParams.keys();

    qSort(keys.begin(), keys.end(), caseInsensitiveLessThan);

    QListIterator<QString> i(keys);
    while (i.hasNext())
    {
        const QString key = i.next();
        joined.append(key.toUtf8());
        joined.append("=");
        joined.append(iParams.value(key).toUtf8());
    }

    if (isSpecialMethod())
    {
        if (iSession->apiSecret().length())
        {
            joined.append(iSession->apiSecret());
        }
    }
    else if (iSession->sessionSecret().length())
    {
        joined.append(iSession->sessionSecret());
    }
    else if (iSession->apiSecret().length())
    {
        joined.append(iSession->apiSecret());
    }

    return md5(joined);
}

void FBRequest::generatePostBody( QByteArray& body )
{
	qDebug()<<"Inside FBRequest::generatePostBody()";
    QString endLine = "\r\n--" + kStringBoundary + "\r\n";
    body.append( "--" + kStringBoundary.toUtf8() + "\r\n" ) ;


    DictionaryIterator i (iParams);

    while (i.hasNext())
    {
        i.next();

        body.append("Content-Disposition: form-data; name=\"" + i.key().toUtf8() + "\"\r\n\r\n" );
        body.append(i.value().toUtf8());
        body.append(endLine.toUtf8());
    }


    if (iDataParam.size())
    {
        if (iDataParamPicture)
        {
            body.append("Content-Disposition: form-data; filename=\"photo\"\r\n" );
            body.append("Content-Type: image/png\r\n\r\n" );
        }
        else
        {
            body.append("Content-Disposition: form-data; filename=\"data\"\r\n");
            body.append("Content-Type: content/unknown\r\n\r\n");
        }

        body.append(iDataParam);
        body.append(endLine.toUtf8());

    }
}

void FBRequest::handleResponseData( const QByteArray& aResponseData )
{
	qDebug()<<"Inside FBRequest::handleResponseData()";
    FBError error;
    QVariant result =  parseXMLResponse( aResponseData, error);
    if (error.code() != 0)
    {
        emit requestFailedWithFacebookError(error);
    }
    else
    {
        emit requestDidLoad(result);
    }
    
    delete pbar;
}

void FBRequest::post( const QString& aUrl, const Dictionary& aParams)
{
	qDebug()<<"Inside FBRequest::post()";
    iUrl = aUrl;
    iParams = aParams;

    iSession->send(this);
}

void FBRequest::cancel()
{
	qDebug()<<"Inside FBRequest::cancel()";
}


void FBRequest::call (const QString& aMethod, const Dictionary& aParams)
{
    QByteArray dataParams;
    callWithDataParams(aMethod, aParams, dataParams, false);

}

void FBRequest::callWithDataParams (const QString& aMethod, const Dictionary& aParams, const QByteArray& aDataParam, bool aDataParamPicture)
{
    iUrl = urlForMethod(aMethod);
    iMethod = aMethod;
    iParams = aParams;
    iDataParam = aDataParam;
    iDataParamPicture = aDataParamPicture;

    iParams["method"] = iMethod;
    iParams["api_key"] = iSession->apiKey();
    iParams["v"] = kAPIVersion;
    iParams["format"] = kAPIFormat;

    if (!isSpecialMethod())
    {
        iParams["session_key"] = iSession->sessionKey();
        iParams["call_id"] = generateCallId();

        if (iSession->sessionSecret().length())
        {
            iParams["ss"] = "1";
        }
    }

    // XXX: workaround what seems to be a Qt bug with the extras-devel libraries.
    QString signature = generateSig();
    iParams["sig"] = signature;
    // XXX: end workaround.
    iSession->send(this);
}


QVariant FBRequest::parseXMLResponse ( const QByteArray& aResponseData, FBError& aError)
{
	qDebug()<<"Inside FBRequest::handleResponseData()";
    QXmlInputSource input;
    input.setData(aResponseData);

    FBXMLHandler handler;
    QXmlSimpleReader parser;
    parser.setContentHandler(&handler);
    bool result = parser.parse(&input);

    QVariant rootObject = handler.rootObject();

    if (handler.parseError() || !result)
    {
        aError.setCode( FBRESPONSE_PARSE_ERROR );
        aError.setDescription("parser was unable to parse the xml response from facebook server.");

        return QVariant();
    }
    else if (handler.rootName().compare("error_response")==0)
    {
        QVariantHash errorDict =  rootObject.toHash();

        bool result;
        int errorCode = errorDict.value("error_code").toInt(&result);

        aError.setCode( errorCode );
        aError.setDescription( errorDict.value("error_msg").toString() );

        return rootObject;
    }
    else
    {
        return rootObject;
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// instance provate slots
void FBRequest::networkReplyError ( QNetworkReply::NetworkError aCode )
{
    emit requestFailedWithNetworkError(aCode );
}

void FBRequest::networkReplyFinished ()
{
qDebug()<<"Inside FBRequest::networkReplyFinished()";
    QNetworkReply* reply = static_cast<QNetworkReply*> ( sender() );
    QByteArray responseData = reply->readAll();

    handleResponseData ( responseData );

}
