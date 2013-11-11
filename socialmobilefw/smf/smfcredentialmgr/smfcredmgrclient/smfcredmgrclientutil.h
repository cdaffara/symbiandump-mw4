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

#ifndef SMFQTSYMBIANCONVERSIONUTILS_H_
#define SMFQTSYMBIANCONVERSIONUTILS_H_

#include <QObject>
#include <QUrl>
#include <QDateTime>
#include <private/qcore_symbian_p.h>
#include <QtCore/qglobal.h>
#include <qstring.h>
#include <QStringList>
#include <qbytearray.h>
#include <qlist.h>
#include <QDebug>



#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#include <e32std.h>
#include <e32cmn.h>
#include <s32strm.h>
#include <S32MEM.H> 
#include <e32des16.h> 
#include <smfcredmgrclientdatastruct.h>
#include <smfcredmgrclientglobal.h> 
#endif
/**
 * Utility to handle conversion between symbian and Qt in Credential Manager.
 */
namespace smfcredmgrclientutil
	{

	/**
	 * Method to convert QList<QUrl> type to RPointerArray<HBufC> type  
	 * @param URLList List of URL in Qt
	 * @return the symbian list of thype RPointerArray<HBufC>
	 */
	RPointerArray<HBufC> convertToSymbianURLList(QList<QUrl> URLList);

	/** 
	 * Method to convert QStringList type to RPointerArray<HBufC>
	 * @param PluginList The QStringList to be converted
	 * @return The Converted plugin list in RPointerArray<HBufC> 
	 */
	RPointerArray<HBufC> convertToSymbianPluginList(QStringList PluginList);

	/**
	 * Conversion method
	 * @param aPluginList List in RPointerArray<HBufC> 
	 * @param ListInQt List in QStringList
	 */
	void convertToQStringList(RPointerArray<HBufC> aPluginList,
			QStringList &ListInQt);

	/**
	 * Conversion method
	 * @param aURLList List in RPointerArray<HBufC> 
	 * @param List List in QStringList
	 */
	void convertToQUrlList(RPointerArray<HBufC> aURLList, QList<QUrl> &List);

	/**
	 * Conversion Method
	 * @param aArray Array in RArray<TSmfAuthToken>
	 * @param Params Array in SmfAuthParams
	 */
	void RArrayToSmfAuthParams(RArray<TSmfAuthToken> aArray,
			SmfAuthParams& Params);

	/**
	 * Conversion Method
	 * @param aArray Array in RArray<TSmfAuthToken>
	 * @param Params Array in SmfAuthParams
	 */
	void SmfAuthParamstoRArray(SmfAuthParams Params, 
			RArray<TSmfAuthToken>& aArray);

	}

#endif /* SMFQTSYMBIANCONVERSIONUTILS_H_ */
