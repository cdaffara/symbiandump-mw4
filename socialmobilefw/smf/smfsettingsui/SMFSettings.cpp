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

#include "SMFSettings.h"
#include "qmessagebox.h"
#include "qdesktopwidget.h"
#include "qmenubar.h"	//menubar

/*
 * Constructor
 * param@Qwidget - Parenet widget
 */
SMFSettings::SMFSettings(/*QWidget *parent*/)
    
{
	ui = new Ui::SMFSettingsClass;
	QWidget *widget = new QWidget;
	setCentralWidget(widget);
	//Set up UI 
	ui->setupUi(widget);
	QDesktopWidget *desktop = QApplication::desktop();
	//get client rect.
	QRect clientRect = desktop->availableGeometry(); 
	widget->setGeometry(clientRect);
	
	//createActions();
	createMenuActions();
	
	//Instance of CSettingsRepository
	#ifdef __FOR_SYMBIAN_CR_USAGE__
		iSettingsRepository = CSettingsRepository::NewL();
	#else
		iSettingsRepository = new CPFIndSettings;
	#endif
		
	//Call method to get default/initial settings
	GetDefaultSettings();
	//make a connection for savebutton,on click it should call SaveSettings slot.
	//connect(ui->SaveButton,SIGNAL(clicked()),this,SLOT(SaveSettings()));
	//make a connection for Cancelebutton,on click it should clear the settings in UI.
	//connect(ui->CancelButton,SIGNAL(clicked()),this,SLOT(ClearData()));
}

/*
 * Destructor
 */
SMFSettings::~SMFSettings()
{
	//delete the ui object.
	delete ui;
	
	if(iSettingsRepository){
		delete iSettingsRepository;
		iSettingsRepository = NULL;
	}
}
void SMFSettings::createMenuActions()
{
    QAction *saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S", "Save"));
    menuBar()->addAction(saveAct);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(SaveSettings()));

    QAction *resetAct = new QAction(tr("&Reset"), this);
    menuBar()->addAction(resetAct);
    connect(resetAct, SIGNAL(triggered()), this, SLOT(ClearData()));

}
/*void SMFSettings::createMenus()
{
    QMenu *saveMenu = menuBar()->addMenu(tr("&Save"));
    //saveMenu->addAction(saveAct);
    connect(saveMenu->activeAction(), SIGNAL(triggered()), this, SLOT(SaveSettings()));

    menuBar()->addSeparator();

    QMenu *resetMenu = menuBar()->addMenu(tr("&Reset"));
    connect(resetMenu->activeAction(), SIGNAL(triggered()), this, SLOT(ClearData()));
    //resetMenu->addAction(resetAct);
}*/
/*
 * Method to set the PluginDetails to UI
 * Param - @aPluginStatus,holds the Plgugin Status.
 */
void SMFSettings::SetPluginDetails(QString aPluginName,QString aPluginStatus)
{
	int indice;
	
	indice = ui->PluginlistboxBox->findText(aPluginName);
	if(indice == -1)
		ui->PluginlistboxBox->setCurrentIndex(0) ;
	else
		ui->PluginlistboxBox->setCurrentIndex(indice) ;
	
	indice = ui->pluginstatusbox->findText(aPluginStatus);
	if(indice == -1)
		ui->pluginstatusbox->setCurrentIndex(0) ;
	else
		ui->pluginstatusbox->setCurrentIndex(indice) ;
	
}
/*
 * Method to set the Auth Expiry Value to UI
 * Param - @aVal,Auth Expiry Value
 */
void SMFSettings::SetAuthExpirationValue(QString aVal)
{
	int indice = ui->AuthExplimitbox->findText(aVal);
	if(indice == -1)
		ui->AuthExplimitbox->setCurrentIndex(0) ;
	else
		ui->AuthExplimitbox->setCurrentIndex(indice) ;
}
/*
 * Method to set the Max. Data Transfer Limit to UI
 * Param - @aVal,Max. Data Transfer Limit Value
 */
void SMFSettings::SetMaxDataTransferLimit(QString aVal)
{
	int indice = ui->MaxDataTransferlimitBox->findText(aVal);
	if(indice == -1)
		ui->MaxDataTransferlimitBox->setCurrentIndex(0) ;
	else
		ui->MaxDataTransferlimitBox->setCurrentIndex(indice) ;
}
/*
 * Method to set the Roaming Status for Data Transfer to UI
 * Param - @aStatus,Rpaming Status value for Data Transfer
 */
void SMFSettings::SetRoamingStatusforDataTransfer(QString aStatus)
{
	int indice = ui->RoamingStatusBox->findText(aStatus);
	if(indice == -1)
		ui->RoamingStatusBox->setCurrentIndex(0) ;
	else
		ui->RoamingStatusBox->setCurrentIndex(indice) ;
}
/*
 * Method to set the Upload File Type to UI
 * Param - @aStatus,Upload File Type Value
 */
void SMFSettings::SetUploadFileType(QString aFileType)
{
	int indice = ui->Uploadfiletypebox->findText(aFileType);
	if(indice == -1)
		ui->Uploadfiletypebox->setCurrentIndex(0) ;
	else
		ui->Uploadfiletypebox->setCurrentIndex(indice) ;
}
/*
 * Method to Get the PluginDetails from UI
 */
QStringList SMFSettings::GetPluginDetails() const
{
	QStringList PluginDetails;
	PluginDetails.append(ui->PluginlistboxBox->currentText());
	PluginDetails.append(ui->pluginstatusbox->currentText());
	return PluginDetails;
}
/*
 * Method to Get the Auth Expiry Value from UI
 */
QString SMFSettings::GetAuthExpirationValue() const
{
	return ui->AuthExplimitbox->currentText();
}
/*
 * Method to Get the Max. Data Transfer Limit from UI
 */
QString SMFSettings::GetMaxDataTransferLimit() const
{
	return ui->MaxDataTransferlimitBox->currentText();
}
/*
 * Method to Get the Roaming Status for Data Transfer from UI
 */
QString SMFSettings::GetRoamingStatusforDataTransfer() const
{
	return ui->RoamingStatusBox->currentText();
}
/*
 * Method to Get the Upload File Type from UI
 */
QString SMFSettings::GetUploadFileType() const
{
	return ui->Uploadfiletypebox->currentText();
}
/*
 * Method to Reset the UI settings.
 */
void SMFSettings::ClearData()
{
	//Reset the 
	ui->PluginlistboxBox->setCurrentIndex(0) ;
	ui->pluginstatusbox->setCurrentIndex(0) ;
	ui->AuthExplimitbox->setCurrentIndex(0) ;
	ui->MaxDataTransferlimitBox->setCurrentIndex(0) ;
	ui->RoamingStatusBox->setCurrentIndex(0) ;
	ui->Uploadfiletypebox->setCurrentIndex(0) ;
}

/*
 * slot written to set/Save the User input/settings to xml.
 */
void SMFSettings::SaveSettings()
{
	QStringList PLuginDetails = GetPluginDetails();

	#ifdef __FOR_SYMBIAN_CR_USAGE__
		//conversion from QString to Descriptor
		TPtrC PluginName(static_cast<const TUint16*>(PLuginDetails[0].utf16()), PLuginDetails[0].length());
		TPtrC PluginStatus(static_cast<const TUint16*>(PLuginDetails[1].utf16()), PLuginDetails[1].length());
	#else
		QString PluginName(PLuginDetails[0]);
		QString PluginStatus(PLuginDetails[1]);
	#endif
		
	iSettingsRepository->SetPluginDetails(PluginName,PluginStatus);
	
	
	#ifdef __FOR_SYMBIAN_CR_USAGE__
		//conversion from QString to Descriptor
		TPtrC AthExpValue(static_cast<const TUint16*>(GetAuthExpirationValue().utf16()), GetAuthExpirationValue().length());
	#else
		QString AthExpValue = GetAuthExpirationValue();
	#endif
	
	iSettingsRepository->SetAuthExpirationValue(AthExpValue);
	
	#ifdef __FOR_SYMBIAN_CR_USAGE__
		//conversion from QString to Descriptor
		TPtrC MaxDataTrValue(static_cast<const TUint16*>(GetMaxDataTransferLimit().utf16()), GetMaxDataTransferLimit().length());
	#else
		QString MaxDataTrValue = GetMaxDataTransferLimit();
	#endif
	iSettingsRepository->SetMaxDataTransferLimit(MaxDataTrValue);
	
	#ifdef __FOR_SYMBIAN_CR_USAGE__
		//conversion from QString to Descriptor
		TPtrC RoamingStatus(static_cast<const TUint16*>(GetRoamingStatusforDataTransfer().utf16()), GetRoamingStatusforDataTransfer().length());
	#else
		QString RoamingStatus = GetRoamingStatusforDataTransfer();
	#endif
		
	iSettingsRepository->SetRoamingStatusforDataTransfer(RoamingStatus);
	
	#ifdef __FOR_SYMBIAN_CR_USAGE__
	//conversion from QString to Descriptor
	TPtrC UploadFileType(static_cast<const TUint16*>(GetUploadFileType().utf16()), GetUploadFileType().length());
	#else
		QString UploadFileType = GetUploadFileType();
	#endif
	iSettingsRepository->SetUploadFileType(UploadFileType);
	
	
	//Create information message box 
	QMessageBox::information(this, "Information", "Settings Saved!"); 
	

}
/*
 * Method to get the default/initial settings from xml.
 */
void SMFSettings::GetDefaultSettings()
{
	
	
	#ifdef __FOR_SYMBIAN_CR_USAGE__
		QString myString;
		TBuf<KMaxSettingValue> Buf;
		//conversion from Descriptor to QString
		myString = QString::fromUtf16(iSettingsRepository->GetPluginDetails()[0].Ptr(), iSettingsRepository->GetPluginDetails()[0].Length());
		QString PluginStatus = QString::fromUtf16(iSettingsRepository->GetPluginDetails()[1].Ptr(), iSettingsRepository->GetPluginDetails()[1].Length());
		//Reset the Plugin Details array 
		iSettingsRepository->GetPluginDetails().Close();
		SetPluginDetails(myString,PluginStatus);
		
		Buf.Copy(iSettingsRepository->GetAuthExpirationValue());
		//conversion from Descriptor to QString
		myString = QString::fromUtf16(Buf.Ptr(), Buf.Length());
		SetAuthExpirationValue(myString);
		
		Buf.Copy(iSettingsRepository->GetMaxDataTransferLimit());
		//conversion from Descriptor to QString
		myString = QString::fromUtf16(Buf.Ptr(), Buf.Length());
		SetMaxDataTransferLimit(myString);
		
		Buf.Copy(iSettingsRepository->GetRoamingStatusforDataTransfer());
		//conversion from Descriptor to QString
		myString = QString::fromUtf16(Buf.Ptr(), Buf.Length());
		SetRoamingStatusforDataTransfer(myString);
		
		Buf.Copy(iSettingsRepository->GetUploadFileType());
		//conversion from Descriptor to QString
		myString = QString::fromUtf16(Buf.Ptr(), Buf.Length());
	SetUploadFileType(myString);
	#else
	
		SetPluginDetails(iSettingsRepository->GetPluginDetails()[0],iSettingsRepository->GetPluginDetails()[1]);

		SetAuthExpirationValue(iSettingsRepository->GetAuthExpirationValue());
		SetMaxDataTransferLimit(iSettingsRepository->GetMaxDataTransferLimit());
		SetRoamingStatusforDataTransfer(iSettingsRepository->GetRoamingStatusforDataTransfer());
		SetUploadFileType(iSettingsRepository->GetUploadFileType());
	#endif
	
}
