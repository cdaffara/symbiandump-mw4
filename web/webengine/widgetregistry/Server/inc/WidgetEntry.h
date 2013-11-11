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
* Description:  This file contains the header file of the CWidgetEntry class.
*
*                This class implements the CWidgetEntry class.
*
*/


#ifndef WIDGETENTRY_H
#define WIDGETENTRY_H

// INCLUDES
#include "WidgetPropertyValue.h"
#include "WidgetRegistryXml.h"

// FORWARD DECLARATIONS
class RFileWriteStream;
class RFileReadStream;


/**
*
*  This class holds all information related to a widget
*
*  @since 3.1
*/
class CWidgetEntry : public CBase
    {

public: // constructors and destructor

    /**
     * Two phase constructor
     * @return none
     * @since 3.1
     **/
    static CWidgetEntry* NewL();

     /**
     * Two phase constructor
     * @return none
     * @since 5.0
     **/
    static CWidgetEntry* NewL( RPointerArray<CWidgetPropertyValue>** aProps );

    /**
     * Destructor
     * @return none
     * @since 3.1
     **/
    ~CWidgetEntry();

public:

    /** Read the widget binary info from file stream into memory
    * @return none
    * @since 3.1
    **/
    void InternalizeBinaryL( RReadStream& aReadStream );

    /** Read the widget XML info from file stream into memory
    * @return none
    * @since 5.0
    **/
    void InternalizeXmlL( RFs& aFileSession,
                          xmlDocPtr aDoc,
                          xmlNode* n,
                          CWidgetRegistryXml* aXmlProcessor );

    /**
     * Write the widget binary info into file stream
     * @return none
     * @since 3.1
     **/
    void ExternalizeBinaryL( RWriteStream& aWriteStream );

    /**
     * Write the widget XML info into file stream
     * @return none
     * @since 5.0
     **/
    void ExternalizeXmlL( RWriteStream& aWriteStream,
                          CWidgetRegistryXml* aXmlProcessor,
                          RFs& aFileSession);

    inline CWidgetPropertyValue& operator[] ( TInt id )
        {
        return *(iPropertyValues[id]);
        }

    /**
     * Is widget running (active)?
     * @return active status
     * @since 3.1
     **/
    TInt ActiveL();

    /**
     * Does widget have sapi access
     * after accepting security prompt, promptless or no access
     * @return sapi access  status
     * @since 5.0
     **/
    TInt SapiAccessState();

    /**
     * Set running status.
     * @return none
     * @since 3.1
     **/
    inline void SetActive( TInt aStatus ) { iActive = aStatus; }

    /**
    * Set MiniView .
    * @return none
    * @since 5.0
    **/
    inline void SetMiniView( TInt aStatus ) { iMiniView = aStatus; }

    /**
     * Set running in full view status.
     * @return none
     * @since 
     **/
    inline void SetFullView( TInt aStatus ) { iFullView = aStatus; }

    /**
    * Set running in full view status.
    * @return none
    * @since 
    **/
   inline void SetBlanketPermission ( TInt aStatus ) { iBlanketPermGranted = aStatus; 
                                                           *(iPropertyValues[EBlanketPermGranted]) = iBlanketPermGranted;
                                                         }

    /**
    * Is widget in MiniView
    * @return MiniView status
    * @since
    **/
    inline TInt GetMiniViewState(){ return iMiniView;}

    /**
    * Is widget in MiniView
    * @return MiniView status
    * @since
    **/
    inline TInt GetFullViewState(){ return iFullView;}

    /**
    * Has widget got blanket permission granted
    * @return blanket permission status
    * @since
    **/
    inline TInt GetBlanketPermGranted(){ return iBlanketPermGranted;}

    /**
     * Property value array accessor.
     * @return none
     * @since 5.0
     **/
    inline RPointerArray<CWidgetPropertyValue>* Properties()
      { return &iPropertyValues; }

protected:
    /**
     * Constructor
     */
    CWidgetEntry();

    /**
     * 2-phase constructor
     */
    void ConstructL();

protected:
    RPointerArray<CWidgetPropertyValue>  iPropertyValues;

    TInt iNetworkAccess; // bool, 1 if widget wants network access
    // info.plist EAllowNetworkAccess and part of EAllowFullAccess

    TInt iActive; // bool, 1 if widget is executing

   	TInt iMiniView; // bool, 1 if launched in miniview

	TInt iFullView; // bool, 1 if launched in fullview
    TInt iBlanketPermGranted; // bool, 1 if platform service access needs to be prompted
    
    };

typedef RPointerArray<CWidgetEntry> RWidgetArray;

#endif // WIDGETENTRY_H
