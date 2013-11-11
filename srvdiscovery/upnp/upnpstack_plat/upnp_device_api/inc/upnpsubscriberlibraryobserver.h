/** @file
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declaration of MUpnpSubscriberLibraryObserver
*
*/


#ifndef C_CUPNPSUBSCRIBERLIBRARYOBSERVER_H
#define C_CUPNPSUBSCRIBERLIBRARYOBSERVER_H

class CUpnpStateVariable;
class CUpnpSubscriberLibraryElement;
// CLASS DECLARATION
/**
*  An interface to Subscriber Library.
*  
*  @since Series60 2.6
*/
class MUpnpSubscriberLibraryObserver
{
public: // New functions 

    /**
    * A virtual function to be implemented in derived classes.
    * It is called after timeout is passed (event is sent).
    * @since Series60 2.6
    * @return List of eventable variables.
    */
    virtual RPointerArray<CUpnpStateVariable>& EventedStateVariables() = 0;
    /**
    * A virtual function to be implemented in derived classes.
    * It is called after removing subscriber
    * @param aSubscriberElement pointer to romoved subscriber
    * @param aPos position taken by subscriber in SubscriberLibrary
    */    
    virtual void SubscriberRemoved( CUpnpSubscriberLibraryElement* aSubscriber, TInt aPos  ) = 0;
};


#endif  // C_CUPNPSUBSCRIBERLIBRARYOBSERVER_H

//End of File