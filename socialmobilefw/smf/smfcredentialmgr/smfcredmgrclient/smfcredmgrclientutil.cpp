/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Pritam Roy Biswas, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Description:
 *  Header file for the utility to handle conversion between symbian and Qt 
 *  in Credential Manager.
 */


#include "smfcredmgrclientutil.h"

namespace smfcredmgrclientutil
	{

	RPointerArray<HBufC> convertToSymbianURLList(QList<QUrl> URLList)
		{
		RPointerArray<HBufC> urlListSymbian;

		for (int i = 0; i < URLList.count(); i++)
			{
			QString SingleURLString(URLList.at(i).toString(QUrl::None));

			HBufC* buf = qt_QString2HBufC(SingleURLString);
				QT_TRAP_THROWING(urlListSymbian.InsertL(buf, i));

			SingleURLString.clear();
			}

		return urlListSymbian;
		}

	RPointerArray<HBufC> convertToSymbianPluginList(QStringList PluginList)
		{
		RPointerArray<HBufC> pluginListSymbian;

		for (int i = 0; i < PluginList.count(); i++)
			{
			QString SinglePlugin = PluginList.at(i);

			HBufC* buf = qt_QString2HBufC(SinglePlugin);
				QT_TRAP_THROWING(pluginListSymbian.InsertL(buf, i));

			SinglePlugin.clear();
			}
		return pluginListSymbian;
		}

	void convertToQStringList(RPointerArray<HBufC> aPluginList,
			QStringList &ListInQt)
		{

		for (int i = 0; i < aPluginList.Count(); i++)
			{
			TPtr ptr(aPluginList[i]->Des());

			QString pluginString = qt_TDesC2QString(ptr);
			ListInQt.insert(i, pluginString);

			ptr.Zero();

			}
		}

	void convertToQUrlList(RPointerArray<HBufC> aURLList, QList<QUrl> &List)
		{

		for (int i = 0; i < aURLList.Count(); i++)
			{
			TPtr ptr(aURLList[i]->Des());

			QString URLString = qt_TDesC2QString(ptr);
			QUrl URL(URLString);

			List.insert(i, URL);
			ptr.Zero();

			}
		}

	void RArrayToSmfAuthParams(RArray<TSmfAuthToken> aArray,
			SmfAuthParams& Params)
		{
		Params.clear();
		for (int i = 0; i < aArray.Count(); i++)
			{
			TSmfAuthToken Set(aArray[i]);

			QByteArray Key((qt_TDesC2QString(Set.iKey->Des())).toAscii());
			QByteArray Secret(
					(qt_TDesC2QString(Set.iSecret->Des())).toAscii());

			Params.insertMulti(Key, Secret);
			}

		}
	
	void SmfAuthParamstoRArray(SmfAuthParams Params, 
				RArray<TSmfAuthToken>& aArray)
		{
		int arrayPos = 0;
		SmfAuthParams::const_iterator itr;
		for (itr = Params.constBegin(); itr != Params.constEnd(); ++itr)
			{
			TSmfAuthToken set;
			
			QString KeyString(itr.key());
			QString ValueString(itr.value());
			qDebug()<<"Key name = "<<KeyString;
			qDebug()<<"Key value = "<<ValueString;
			
			set.iKey = qt_QString2HBufC(KeyString);
			set.iSecret = qt_QString2HBufC(ValueString);
			//insert into the array
			aArray.Insert(set, arrayPos);
			//clear previous values
			KeyString.clear();
			ValueString.clear();
			arrayPos++;
			}
		
		}

	}

