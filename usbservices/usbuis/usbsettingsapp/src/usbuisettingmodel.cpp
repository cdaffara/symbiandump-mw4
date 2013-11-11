/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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

#include "usbuisettingmodel.h"
#include <QStringList>
#include <UsbWatcherInternalCRKeys.h>
#include <XQSettingsKey>
#include <usbman.h>
#include <HbIcon>
#include <QItemSelection>
#include <QItemSelectionModel>
#include "usbuimodelactive.h"
#include "usbdebug.h"


const QString TextIdPrefix = ("txt_usb_dblist_");
const QString DescriptionIdPostfix = ("_val");
const QString ModeIconNamePrefix = ("qtg_large_");

/*!
    Constructor.
 */
UsbUiSettingModel::UsbUiSettingModel( QObject *parent )
    : QAbstractItemModel( parent), mSelectionModel(NULL)
{
    mModelActive = new UsbUiModelActive();
    mCurrentMode = currentMode();
    initializeModelData(mCurrentMode);
  	bool ret = mSettingsManager.startMonitoring( 
   	        XQSettingsKey( XQSettingsKey::TargetCentralRepository, 
   	                KCRUidUsbWatcher.iUid, KUsbWatcherPersonality ) );
    myDebug() << ">>> UsbUiSettingModel::startMonitoring value=" 
              << ret; 
                              
   	// signal: personality changed in the central repository                
   	ret = connect( &mSettingsManager, 
   	        SIGNAL( valueChanged( const XQSettingsKey&, const QVariant& ) ), 
   	        this, 
   	        SLOT( cenrepChanged( const XQSettingsKey&, const QVariant& ) ) );
    myDebug() << ">>> UsbUiSettingModel::UsbUiSettingModel connect valueChanged="
            << ret;
   	        
    // signal: response from usbwatcher to our attempt to set the personality 	        
   	ret = connect( mModelActive, 
   	        SIGNAL( requestCompleted( int ) ), 
   	        this, 
   	        SLOT( personalitySetCompleted( int )));
    myDebug() << ">>> UsbUiSettingModel::UsbUiSettingModel connect requestCompleted="
            << ret;
}

/*!
    Destructor.
 */
UsbUiSettingModel::~UsbUiSettingModel()
{
    myDebug() << ">>> UsbUiSettingModel::~UsbUiSettingModel";
    mSettingsManager.stopMonitoring( 
            XQSettingsKey( XQSettingsKey::TargetCentralRepository, 
                    KCRUidUsbWatcher.iUid, KUsbWatcherPersonality ) );
    delete mModelActive;
    myDebug() << "<<< UsbUiSettingModel::~UsbUiSettingModel";
}

/*!
    Provides notification of changes in selected usb mode
 */
void UsbUiSettingModel::cenrepChanged( const XQSettingsKey &key,  
        const QVariant &value )  
{
    Q_UNUSED(key);
    myDebug() << ">>> UsbUiSettingModel::cenrepChanged";
    // key is not checked, as we monitor only one key
    updateSelectionModel(value.toInt());                                 
    myDebug() << "<<< UsbUiSettingModel::cenrepChanged"; 
}

/*!
 * updates the selection model
 * The selection model will signal the view.
 */
void UsbUiSettingModel::updateSelectionModel(int newPersonality)
{
    myDebug() << ">>> UsbUiSettingModel::updateSelectionModel value=" 
            << newPersonality; 
            
    mCurrentMode = newPersonality;
    mSelectionModel->clear();    
    int row = mPersonalityIds.indexOf(newPersonality);
    myDebug() << ">>> UsbUiSettingModel::updateSelectionModel row=" 
            << row; 
    // in case of the hidden personality , the selection model is left empty
    if ( row >= 0 ) {
        //set selection model for the new selection
        QModelIndex selectionIndex = index(row, 0, QModelIndex());
        QItemSelection selection(selectionIndex, selectionIndex);
        mSelectionModel->select(selection, QItemSelectionModel::Select);    
    }
    myDebug() << "<<< UsbUiSettingModel::updateSelectionModel"; 
}

/*!
 * Returns the index of the item in the model specified by the given row, column and parent index.
 */
QModelIndex UsbUiSettingModel::index( int row, int column, const QModelIndex &parent ) const
{
    return hasIndex( row, column, parent ) ? createIndex( row, column ) : QModelIndex();
}

/*
    This model does not support hierarchy, so this returns an empty model index. 
 */
QModelIndex UsbUiSettingModel::parent( const QModelIndex &child ) const
{
    Q_UNUSED( child );
    return QModelIndex();
}

/*!
 * Returns the number of rows under the given parent
 */
int UsbUiSettingModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return mSettingsList.count();
}

/*!
 * Returns the number of columns for the children of the given parent.
 * This model is one-dimensional, so this returns 1.
 */
int UsbUiSettingModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return 1;
}

/*!
 * Returns the data stored under the given role for the item referred to by the index.
 */
QVariant UsbUiSettingModel::data( const QModelIndex &index, int role ) const
{
    return mSettingsList.value( index.row() ).value( role );
}

/*!
 * This is called when usb selection is changed in the view (selection model). 
 */
void UsbUiSettingModel::handleSelectionChange(const QItemSelection &selected, 
        const QItemSelection &deselected )
{
    Q_UNUSED( deselected );
    myDebug() << ">>>  UsbUiSettingModel::handleSelectionChange";
    QModelIndexList items = selected.indexes();
    if (!items.isEmpty()) {
        myDebug() << "     UsbUiSettingModel::handleSelectionChange item exists";
        QModelIndex index = items[0];
        int newPersonalityId = mPersonalityIds.at(index.row());
        if ( newPersonalityId != mCurrentMode ) {
            myDebug() << "     UsbUiSettingModel::handleSelectionChange setting personality";
            mModelActive->SetUsbPersonality(newPersonalityId);
        }
    }
    myDebug() << "<<<  UsbUiSettingModel::handleSelectionChange return";
}

/*!
 * Getter for the source data.
 */
const QModelIndex* UsbUiSettingModel::sourceData() const
{
    return new QModelIndex( createIndex( 0, 0 ) );
}

void  UsbUiSettingModel::setSelectionModel(QItemSelectionModel *selectionModel)
    {
    myDebug() << ">>>  UsbUiSettingModel::setSelectionModel";
    mSelectionModel = selectionModel;
    connect( mSelectionModel, 
                SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection & ) ), 
                this, 
                SLOT( handleSelectionChange( const QItemSelection &, const QItemSelection & ) ) );
    updateSelectionModel(mCurrentMode);
    myDebug() << "<<<  UsbUiSettingModel::setSelectionModel return";
    }

/*!
 * Get the translated mode name for the personality friendly name.
 */
QString UsbUiSettingModel::modeName( QString &friendlyName )
{
    myDebug() << ">>>  UsbUiSettingModel::modeName";
    QString textId = TextIdPrefix + friendlyName;
    QString modeName = hbTrId( textId.toAscii() );
    myDebug() << "<<< UsbUiSettingModel::modeName " << modeName;
    return modeName;
}

/*!
 * Get the current USB mode (personality) ID
 */
int UsbUiSettingModel::currentMode()
{
    myDebug() << ">>>  UsbUiSettingModel::CurrentMode";
    int currentMode = mSettingsManager.readItemValue(
                XQSettingsKey( XQSettingsKey::TargetCentralRepository, 
                KCRUidUsbWatcher.iUid, KUsbWatcherPersonality ) ).toInt();
    myDebug() << "<<< UsbUiSettingModel::CurrentMode " << currentMode;
    return currentMode;
}

void UsbUiSettingModel::initializeModelData( int aModeId )
{
    myDebug() << ">>> UsbUiSettingModel::initializeModelData aModeId="
            << aModeId;
    RUsb usbMan;
    if ( usbMan.Connect() == KErrNone ) {
        RArray<TInt> personalityIds;
        mPersonalityIds.clear();
        if ( usbMan.GetPersonalityIds( personalityIds ) == KErrNone ) {
            for ( int i = 0; i < personalityIds.Count(); i++ ) {
                myDebug() << ">>> UsbUiSettingModel::initializeModelData personality ID ="
                    << personalityIds[i];
                if ( !isPersonalityHidden(usbMan, personalityIds[i]) ) {
                    mPersonalityIds.append( personalityIds[i] );                    
                    QString friendlyName = getFriendlyName(usbMan, personalityIds[i]);
                    
                    QStringList displayList;
                    //text-1 mode name
                    displayList.append( modeName( friendlyName ) );
                    
                    //text-2 description
                    QString textId = TextIdPrefix + friendlyName + DescriptionIdPostfix;
                    displayList.append( hbTrId(textId.toAscii()) );
                    
                    QMap< int, QVariant > dataRow;
                    dataRow[ Qt::DisplayRole ] = QVariant( displayList );
                    
                    //icon-1
                    QString iconName = ModeIconNamePrefix + friendlyName;
                    HbIcon icon(iconName);
                    QList<QVariant> icons;
                    icons << icon;                    
                    dataRow[ Qt::DecorationRole ] = QVariant( icons );
                    
                    mSettingsList << dataRow;
                }
            }
        }
        personalityIds.Close();
        usbMan.Close();
    }
    myDebug() << "<<< UsbUiSettingModel::initializeModelData";
}

/*!
 * it checks the response from usbwatcher to see if the new mode change has been successful
 * it will go back to the previous personality if it has not been successful
 */
void UsbUiSettingModel::personalitySetCompleted (int status )
{
    myDebug() << ">>> UsbUiSettingModel::personalitySetCompleted status= "
        << status;      
    // status contains Symbian error code from usbwatcher
    // if the status is KErrNone, we are ready to process the next request
    if (status != KErrNone) {
        // changing the personality failed, so we need to set back the previous personality
        // the value will be read from central repository and also updates mCurrentMode  
        updateSelectionModel(currentMode());
    }
   
    myDebug() << "<<< UsbUiSettingModel::personalitySetCompleted";      
}

bool UsbUiSettingModel::isPersonalityHidden(RUsb &usbman, TInt personalityId)
{
    myDebug() << ">>> UsbUiSettingModel::isPersonalityHidden from USB Manager";
    bool hidden = false;
    TUint32 property = 0;
    TInt ret = usbman.GetPersonalityProperty(personalityId, property);
    if (ret == KErrNone) {
        myDebug() << "property " << property;
        if (property & KUsbPersonalityPropertyHidden) {
            hidden = true;
        }
    } 
    myDebug() << "<<< UsbUiSettingModel::isPersonalityHidden " << hidden;
    return hidden;
}

QString UsbUiSettingModel::getFriendlyName(RUsb &usbman, TInt personalityId)
{
    myDebug() << ">>> UsbUiSettingModel::getFriendlyName";
    QString friendlyName;
    HBufC* description = NULL;
    TInt err = usbman.GetDescription(personalityId, description);
    if (err == KErrNone) {
        friendlyName = QString::fromUtf16(description->Ptr(), description->Length());
        friendlyName.replace( QChar(' '), QChar('_') );
        delete description;
    } else {
        myDebug() << "    UsbUiSettingModel::getFriendlyName RUsb error "
            << err;    
    }
    myDebug() << "    UsbUiSettingModel::getFriendlyName friendlyName=" << friendlyName;
    myDebug() << "<<< UsbUiSettingModel::getFriendlyName";
    return friendlyName;
}
