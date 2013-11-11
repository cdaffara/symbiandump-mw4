/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MUpnpTmServerObserver interface definition
*
*/

#ifndef __UPNPTMSERVEROBSERVER_H__
#define __UPNPTMSERVEROBSERVER_H__

#include <terminalmodeconsts.h>
#include <upnptmclienticonpref.h>

/**
 * This is the main callback interface which the Terminal Mode Server application 
 * needs to implement to handle the soap action requests from the Terminal Mode Clients.
 * It also provides a method to fetch client's icon preferences from AutomotiveServer.
 * 
 */
class MUpnpTmServerObserver
    {
public:
    /**
    * Method to intimate Automotive Server about the arrival of GetApplicationList soap action. 
    * This is a synchronous call and the implementing class must return immediately
    * @param   aProfileId Profile ID of the client profile whose parameter settings will be 
    *                     applied for generating the application list
    * @return  Returns Terminal Mode Error code
    */    
    virtual TTerminalModeErrorCode OnGetApplicationList( TUint aProfileId ) = 0;
    /**
    * Method to launch an app through the Automotive Server. 
    * This is a synchronous call and the implementing class must return after
	* launching the app. 
    * @param   aAppId	  The App ID as registered with the Terminal Mode Service.
    * @param   aUrl[out]  AS provides the actual URL where the application is running.
    * @param   aProfileId Profile ID of the client profile whose parameter settings will be 
    *                     applied for launching and executing the application
    * @return  Returns Terminal Mode Error code
    */    
    virtual TTerminalModeErrorCode OnLaunchApp( TUint aAppId, RBuf8& aUrl, TUint aProfileId = 0 ) = 0;
    /**
    * Method to terminate an app that is already running.
    * @param   aAppID	  The ID of the App as registered with the Terminal Mode Service.
    * @param   aProfileId Profile ID of the client profile whose parameter settings will be 
    *                     applied to the application during execution
    * @return  Returns    Terminal Mode Error code
    */    
    virtual TTerminalModeErrorCode OnTerminateApp( TUint aAppId, TUint aProfileId = 0 ) = 0;
    /**
    * Method through which the Terminal Mode Service Service can retrieve the status of a
	* given app. 
    * @param	aAppID		The ID of the App as registered with the Terminal Mode Service.
    * @param    aProfileId  [out]Profile ID of the client profile
	* @param	aStatusType	[out]The App Status object to be populated
	* @return   Returns Terminal Mode Error code
    */    
    virtual TTerminalModeErrorCode OnGetAppStatus( TUint aAppId, TUint& aProfileId ,RBuf8& aStatusType ) = 0;
    /** 
     * AutomotiveServer returns the the value of the state variable MaxNumProfiles 
     * in response to the GetMaxNumProfiles action 
     * @param aProfileIdCount[out] The maximum number of client profiles that are supported
     *                              simultaneously by the TmClientProfile service. 
     * @return   Returns TerminalModeError code                            
     */
    virtual TTerminalModeErrorCode OnGetMaxNumProfiles( TUint& aProfileIdCount ) = 0;
    /** 
     * Method to intimate AutomotiveServer whenever the control point invokes action to register 
     * a client profile and notify the Terminal Mode device about its preferences, settings 
     * and capabilities. 
     * @param aProfileId The identifier of the profile record where the client profile 
     *                   settings must be stored
     * @param aClientProfile Profile information about Terminal Mode client and its capabilities
     *                       which needs to be updated in the profile record    
     * @param  aResultProfile[out]  The updated client profile.     
     * @return  Returns Terminal Mode Error code                            
     */
    virtual TTerminalModeErrorCode OnSetClientProfile( TUint aProfileId, const TDesC8& aClientProfile, 
                                                                          RBuf8& aResultProfile ) = 0;
    /** 
     * Method to intimate AutomotiveServer whenever the control point invokes action to 
     * to access the contents of a client profile stored in the Terminal Mode device
     * @param aProfileId The identifier of the profile record where the client profile 
     *                   settings must be stored
     * @param aClientProfile[out] Client profile corresponding to the profileID input variable 
     * @return Returns  Terminal Mode Error code                                  
     */
    virtual TTerminalModeErrorCode OnGetClientProfile( TUint aProfileId, RBuf8& aClientProfile ) = 0;
    /**
     * Method to fetch the Terminal Mode Client( Car Kit )'s icon preferences from the 
     * AutomotiveServer for a particular client profile corresponding to the profileID supplied.
     * In case client profile has no icon preferences associated with it, AutomotiveServer 
     * returns the Client Icon Preference object with default values.
     * This method is invoked whenever the http request for a particular application icon 
     * comes to Terminal Mode Service. 
     * @param aProfileId   ProfileID corresponding to a particular client profile.
     * @return  Returns reference to the Client Icon Preference object.
     * @see  CUpnpTmClientIconPref
     */
    virtual const CUpnpTmClientIconPref& GetClientIconPrefsL( TUint aProfileId ) = 0;
    };

#endif // __UPNPTMSERVEROBSERVER_H__

