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

#ifndef USBUISETTINGMODEL_H
#define USBUISETTINGMODEL_H


#include <e32base.h>
#include <e32property.h>
#include <QStringList>
#include <QAbstractItemModel>
#include <XQSettingsManager>

typedef QList< QMap< int, QVariant > > UsbUiModelDataSource;

class RUsb;
class UsbUiModelActive;
class QItemSelection;
class QItemSelectionModel;

class UsbUiSettingModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class TestUsbUiModelActive;
 
public:
    UsbUiSettingModel( QObject *parent = 0);
    virtual ~UsbUiSettingModel();

    /*
     *  Returns the index of the item in the model specified by the given row, column and parent index.
     *  @param row is the row number of the specified item
     *  @param column is the column number of the specified item
     *  @param parent QModelIndex parent
     */   
    virtual QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    /*
     * Returns QModelIndex() because this model item has no parent
     * @param child is the index of item model, the parent of which is going to be returned
     * @param child is not used in here
     */
    virtual QModelIndex parent( const QModelIndex &child ) const;
    /*
     * Returns the number of rows under the given parent
     * @param parent QModelIndex parent
     */
    virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    /*
     * Returns the number of columns for the children of the given parent.
     * @param parent QModelIndex parent
     */
    virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    /*
     *Returns the data stored under the given role for the item referred to by the index. 
     */
    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

    const QModelIndex* sourceData() const;
   
    /*
     * informs the model of the changes in selectionModel and the new mode 
     * will be Handeled by the Model
     */
    void  setSelectionModel(QItemSelectionModel *selectionModel);

public slots:
    /*
     * Provides notification of changes in selected usb mode
     * @param key is not used 
     * @param value is the value read from cenrep key 
     */
    void cenrepChanged( const XQSettingsKey &key, const QVariant &value );
    
    /*
     * it checks the response from usbwatcher to see if the new mode change has been successful
     * it will go back to the previous personality if it has not been successful
     * @param status is the error code returned from usbwatcher
     */
    void personalitySetCompleted (int status );
    
    /**
     * This slot handles selection change from the selection model.
     * The personality is set according to the selection.
     * @param selected Item selection of selected items
     * @param deselected Item selection of deselected items
     */
    void handleSelectionChange(const QItemSelection &selected, 
            const QItemSelection &deselected );
    
private:
	/*
	 * Get the translated mode name
	 * @param friendlyName is the usb personality friendly name
	 */   
    QString modeName( QString &friendlyName );

    /*
     * Get the current USB personality ID
     * Returns 0, if the key is missing or the value cannot be converted 
     * to integer.
     * @return The current personality id
     */
    int currentMode(); 

    /**
     * Initialize the model data
     * @param modeId The current mode ID
     */
    void initializeModelData( int modeId );

    /**
     * Update the selection model
     * @param newPersonality is the new personality id
     */
    void updateSelectionModel(int newPersonality);
	
    /**
     * Check from the USB Manager if the USB personality is hidden.
     * @param usbman Already opened USB Manager session
     * @param personalityId The ID of the personality to be checked.
     * @return For hidden personality, true is returned.
     */
    bool isPersonalityHidden(RUsb &usbman, TInt personalityId);
    
    /**
     * Get the personality friendly name from USB Manager
     * The friendly name can be used in text IDs.
     * @param usbman The open USB Manager session
     * @param personalityId The ID of the personality
     * @return Friendly name for personality
     */
    QString getFriendlyName(RUsb &usbman, TInt personalityId);
    
private:
       
    UsbUiModelDataSource mSettingsList;
 
    // current USB personality
    int mCurrentMode;

    //variable to access central repository
    XQSettingsManager mSettingsManager;
    
    QList<int> mPersonalityIds;
    //owned by the class
    // modelactive is used for the interaction with the usbwatcher
    UsbUiModelActive *mModelActive;
    
    QItemSelectionModel *mSelectionModel;
};

#endif // USBUISETTINGMODEL_H
