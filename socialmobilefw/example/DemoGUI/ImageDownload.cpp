/****************************************************************************
**
** Trolltech hereby grants a license to use the Qt/Eclipse Integration
** plug-in (the software contained herein), in binary form, solely for the
** purpose of creating code to be used with Trolltech's Qt software.
**
** Qt Designer is licensed under the terms of the GNU General Public
** License versions 2.0 and 3.0 ("GPL License"). Trolltech offers users the
** right to use certain no GPL licensed software under the terms of its GPL
** Exception version 1.2 (http://trolltech.com/products/qt/gplexception).
**
** THIS SOFTWARE IS PROVIDED BY TROLLTECH AND ITS CONTRIBUTORS (IF ANY) "AS
** IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
** PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
** OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** Since we now have the GPL exception I think that the "special exception
** is no longer needed. The license text proposed above (other than the
** special exception portion of it) is the BSD license and we have added
** the BSD license as a permissible license under the exception.
**
****************************************************************************/

#include <qdebug.h>
#include <QImage>
#include <QByteArray>
#include <qnetworkproxy.h>
#include <qfile.h>
#include "ImageDownload.h"
#include "HomeView.h"

bool downloading = false;

#define EMULATORTESTING

ImageDownload::ImageDownload(QObject *parent)
	{
	manager = new QNetworkAccessManager(this);
	
#ifdef EMULATORTESTING
	qDebug()<<"Using PROXY SETTINGS!!!, change for device testing in image download";
	
	// Reading the keys, CSM Stubbed - START
	QFile winFile("c:\\data\\DoNotShare.txt");
	if (!winFile.open(QIODevice::ReadOnly))
		{
		qDebug()<<"File to read the windows username and password could not be opened, returning!!!";
		return;
		}
	
	QByteArray winArr = winFile.readAll();
	QList<QByteArray> winList = winArr.split(' ');
	winFile.close();
	
	QString httpUser(winList[0]);
	QString httpPass(winList[1]);

    // For proxy settings on emulator only - REMOVE for device
    QString httpProxy = "10.1.0.214";
    QString httpPort = "3128";

    //==Classes used from Network Module==
    QNetworkProxy proxy;

    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(httpProxy);
    proxy.setPort(httpPort.toInt());
    proxy.setUser(httpUser);
    proxy.setPassword(httpPass);
		
    QNetworkProxy::setApplicationProxy(proxy);
#endif
	}

ImageDownload::~ImageDownload()
	{
	if(manager)
		delete manager;
	}

void ImageDownload::downloadImage(HomeView *homeView, QString name, QUrl url, const SmfItemIdentifier &identifier)
	{
	m_homeView = homeView;
	m_identifier = identifier;
	qDebug()<<"Inside ImageDownload::downloadImage(name, url)";
	QNetworkRequest request;
	request.setUrl(url);
	qDebug()<<"URL of image to be downloaded = "<<url;
	connect(manager, SIGNAL(finished(QNetworkReply*)), 
			this, SLOT(networkReplyFinished(QNetworkReply*)));
			
	QNetworkReply* reply = manager->get(request);
	m_nameReplyHash.insert(name, reply);
	}



void ImageDownload::networkReplyFinished ( QNetworkReply *aNetworkReply )
	{
	qDebug()<<"Error code if any = "<<aNetworkReply->error();
	qDebug()<<"Response for reply = "<<aNetworkReply;

	QByteArray response = aNetworkReply->readAll();
	qDebug()<<"Response Size :"<<response.size();
	if(response.size())
		{
		QImage image = QImage::fromData(response);
		
		QString fileName("c:\\data\\");
		fileName.append(m_nameReplyHash.key(aNetworkReply));
		fileName.append(".jpg");
		
		qDebug()<<"Image URL :"<<fileName;
		
		bool saved = image.save(fileName, "JPG", -1);
		qDebug()<<"Image saved ? "<<saved;
		m_nameReplyHash.remove(m_nameReplyHash.key(aNetworkReply));
		if(m_nameReplyHash.count() == 0)
			{
			downloading = false;
			if(SmfFriendsFetch == m_identifier)
				m_homeView->populateFriendsWidget();
			else if(SmfPostsFetch == m_identifier)
				m_homeView->populatePostsWidget();
			else if(SmfPhotosFetch == m_identifier)
				m_homeView->populatePhotosGridView();
			// ToDo:- for albums, photos and activity
			else
				qDebug()<<"Unknown request for download, so do nothing";
			}
		}
	else
		{
		qDebug()<<"Response data is NULL";
		}
	}
