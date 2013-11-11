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
* Settings UI class of SMF 
*/

#ifndef SMFSETTINGS_H
#define SMFSETTINGS_H

//#include <QtGui/QWidget>
#include <qmainwindow.h>
#include "ui_SMFSettings.h"	//for SMFSettingsClass
#include "SettingsRepository.h"
#include "PlatfromIndependentSettings.h"
#include "qlist.h"		//for QList
#include "qstring.h"	//for QString
#include "SettingsConstants.h"

/* 
 * SMFSettings UI Class
 */
class SMFSettings : public /*QWidget*/	QMainWindow
{
    Q_OBJECT

public:
	/*
	 * Constructor
	 */
	SMFSettings(/*QWidget *parent = 0*/);
	/*
	 * Destructor
	 */
    ~SMFSettings();
    
public slots:
	/*
	 * slot written to set/Save the User input/settings to xml.
	 */
    void SaveSettings();
	/*
	 * slot written to Reset the UI settings.
	 */
	void ClearData();
private:
	/*
	 * Method to get the default/initial settings from xml.
	 */
	void GetDefaultSettings();
	
	//void createActions();
	void createMenuActions();
	
//Set & Get Methods
public:
	/*
	 * Method to set the PluginDetails to UI
	 * Param - @aPluginStatus,holds the Plgugin Status.
	 */
	void SetPluginDetails(QString aPluginName,QString aPluginStatus);
	/*
	 * Method to set the Auth Expiry Value to UI
	 * Param - @aVal,Auth Expiry Value
	 */
	void SetAuthExpirationValue(QString aVal);
	/*
	 * Method to set the Max. Data Transfer Limit to UI
	 * Param - @aVal,Max. Data Transfer Limit Value
	 */
	void SetMaxDataTransferLimit(QString aVal);
	/*
	 * Method to set the Roaming Status for Data Transfer to UI
	 * Param - @aStatus,Rpaming Status value for Data Transfer
	 */
	void SetRoamingStatusforDataTransfer(QString aStatus);
	/*
	 * Method to set the Upload File Type to UI
	 * Param - @aStatus,Upload File Type Value
	 */
	void SetUploadFileType(QString aFileType);
	/*
	 * Method to Get the PluginDetails from UI
	 */
	QStringList GetPluginDetails() const;
	/*
	 * Method to Get the Auth Expiry Value from UI
	 */
	QString GetAuthExpirationValue() const;
	/*
	 * Method to Get the Max. Data Transfer Limit from UI
	 */
	QString GetMaxDataTransferLimit() const;
	/*
	 * Method to Get the Roaming Status for Data Transfer from UI
	 */
	QString GetRoamingStatusforDataTransfer() const;
	/*
	 * Method to Get the Upload File Type from UI
	 */
	QString GetUploadFileType() const;
	
private:
	/*
	 * pointer to Settings UI class.
	 */
    Ui::SMFSettingsClass *ui;
    
	#ifdef __FOR_SYMBIAN_CR_USAGE__
		CSettingsRepository* iSettingsRepository;
	#else
		CPFIndSettings* iSettingsRepository;
	#endif
 

};

#endif // SMFSETTINGS_H
