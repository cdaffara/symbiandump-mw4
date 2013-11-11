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
* Description:  Definition of CChunkedPostDataItems and CPostDataItem
*
*/



#ifndef _POSTDATAITEM_H
#define _POSTDATAITEM_H

//  INCLUDES

#include <e32base.h>
#include <f32file.h>
// CONSTANTS

// MACROS

// DATA TYPES
namespace WebCore { 
    class FormData;
    class FormDataElement;
};

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  container of form data
*
*  @lib resloader.lib
*  @since 3.0
*/
class PostDataItem
{
public:
    
    /**
    * Constructor
    */
    PostDataItem(){}
    
    /**
    * Two-phased constructor.
    * @since 3.0
    * @return PostDataItem
    */
    virtual void initL(const WebCore::FormDataElement* formDataElement);

    /**
    * Retuns data contained by the post data item
    * @since 3.0
    * @return HBufC8* 
    */
    virtual HBufC8* dataL(int sizeToSupply = -1);

    /**
    * resets the state of the objects
    */
    virtual void reset();

    /**
    * Returns amount of data of an item peding to be posted
    * @since 3.0
    * @return int , size left to be posted
    */
    virtual int pendingContentSize() const;

    /**
    * Destructor
    */
    virtual ~PostDataItem();

  protected:
    // protected member data
    HBufC8* m_data;// owned, post content part 
    TUint m_dataOffset;// offset of the data content 
  };

/**
*  container of form data
*
*  @lib resloader.lib
*  @since 3.0
*/
class FileDataItem:public PostDataItem
  {
public:
    /**
    * Constructor
    */
    FileDataItem(){}
    
    /**
    * Two-phased constructor.
    * @since 3.0
    * @return CPostDataItem
    */
    virtual void initL(const WebCore::FormDataElement* formDataElement);


    /**
    * Retuns data contained by the post data item
    * if the item is of type file , then the contents of
    * the file are read
    * @since 3.0
    * @return HBufC8* 
    */
    HBufC8* dataL(int sizeToSupply = -1);


    /**
    * Returns amount of data of an item pending to be posted
    * @since 3.0
    * @return int , size left to be posted
    */
    int pendingContentSize() const;

    /**
    * Destructor
    */
    ~FileDataItem();

  private:


  private:
    // private member data
    RFile m_file;
    int m_fileSize;
    bool m_fileLocked;
  };

#endif // _POSTDATAITEM_H
