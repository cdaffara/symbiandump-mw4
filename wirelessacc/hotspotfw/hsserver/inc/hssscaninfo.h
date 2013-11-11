/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class to dig information from scan results.
*
*/



#ifndef HSSSCANINFO_H
#define HSSSCANINFO_H


//  INCLUDES
#include "hssscanlistiterator.h"

// CONSTANTS
const TUint8 BSSID_LENGTH = 6;
const TUint16 SCAN_CAPABILITY_BIT_MASK_ESS     = 0x0001;
const TUint16 SCAN_CAPABILITY_BIT_MASK_PRIVACY = 0x0010;
const TUint8 SCAN_IE_OUI_LENGTH = 3;
const TUint8 TIMESTAMP_LENGTH = 8;

// LOCAL DATATYPES

typedef enum _HssScanError
{
    HssScanError_Ok,
    HssScanError_IeNotFound

} HssScanError;

typedef enum _HssSecurityMode
    {
    HssSecurityModeOpen,
    HssSecurityModeWep,
    HssSecurityMode802_1x,
    HssSecurityModeWpaEap,
    HssSecurityModeWpaPsk,
    HssSecurityModeWpa2Eap,
    HssSecurityModeWpa2Psk    
    } HssSecurityMode;

typedef enum _HssOperatingMode
    {
    HssOperatingModeInfra,
    HssOperatingModeAdhoc
    } HssOperatingMode;

typedef TUint8 HssIeOui[SCAN_IE_OUI_LENGTH];

// LOCAL CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Wrapper class to parse data fields from scan info.
*/
class HssScanInfo : public HssScanListIterator
{
public:  // Methods

   // Constructors and destructor
    
    /**
    * C++ default constructor.
    */
    HssScanInfo( const HssScanList& scan_list);

  // New methods
    
    /**
    * Return the signal noise ratio of the received (info) packet.
    * @return Signal noise ratio (SNR).
    */
    inline TUint8 SignalNoiseRatio() const;

    /**
    * Return RX level of the received (info) packet.
    * @return RX level.
    */
    inline TUint8 RXLevel() const;

    /**
    * Return BSSID of the scan info.
    * @param  bssid    (OUT) ID of the access point or IBSS network.
    */
    inline void BSSID( TUint8 bssid[BSSID_LENGTH] ) const;


    /**
    * Get beacon interval of the network.
    * @return the beacon interval.
    */
    inline TUint16 BeaconInterval() const;

    /**
    * Get capability of the network (see IEEE 802.11 section 7.3.1.4.
    * @return The capability information.
    */
    inline TUint16 Capability() const;

    /**
    * Return whether the privacy bit is enabled in AP capabilities.
    * @return Whether the privacy is enabled. ETrue when enabled, EFalse when disabled.
    */
    inline TBool Privacy() const;

    /**
    * Return the operating mode of the AP.
    * @return The operating mode of the AP.
    */
    inline HssOperatingMode OperatingMode() const;

    /**
    * Return the security mode of the AP.
    * @return The security mode of the AP.
    */
    HssSecurityMode SecurityMode();

    /**
    * Return the timestamp field of the frame.
    */
    inline void Timestamp( TUint8 timestamp[TIMESTAMP_LENGTH] ) const;

    /**
    * Return requested information element.
    * @param ie        Id of the requested IE data. See 802dot11.h
    * @param length    (OUT) Length of the IE. Zero if IE not found.
    * @param data      (OUT) Pointer to the beginning of the IE data. NULL if IE not found.
    * @return          General error message.
    */
    HssScanError InformationElement( TUint8 ie, 
                                               TUint8& length, 
                                               const TUint8** data );
    /**
    * Return WPA information element.
    * @param length    (OUT) Length of the IE. Zero if IE not found.
    * @param data      (OUT) Pointer to the beginning of the IE data. NULL if IE not found.
    * @return          General error message.
    */
    HssScanError WpaIE( TUint8& length, 
                                  const TUint8** data );

    /**
    * Return the first information element.
    * @param ie        (OUT) Id of the IE. See 802dot11.h.
    * @param length    (OUT) Length of the IE. Zero if IE not found.
    * @param data      (OUT) Pointer to the beginning of the IE data. NULL if IE not found.
    * @return          General error message.
    */
    HssScanError FirstIE( TUint8& ie, 
                                    TUint8& length, 
                                    const TUint8** data );
    /**
    * Return next information element.
    * @param ie        (OUT) Id of the IE. See 802dot11.h.
    * @param length    (OUT) Length of the IE. Zero if IE not found.
    * @param data      (OUT) Pointer to the beginning of the IE data. NULL if IE not found.
    * @return          General error message.
    */
    HssScanError NextIE( TUint8& ie, 
                                   TUint8& length, 
                                   const TUint8** data );                                       

    /**
    * Return requested information element data.
    * @param ie_id          (IN)  Id of the requested IE data. See 802dot11.h
    * @param ie_oui         (IN)  OUI of the requested IE data. See 802dot11.h
    * @param ie_oui_type    (IN)  OUI Type of the requested IE data. See 802dot11.h
    * @param length         (OUT) Length of the IE. Zero if IE not found.
    * @param data           (OUT) Pointer to the beginning of the IE data. NULL if IE not found.
    * @return               General error message.
    */
    HssScanError InformationElement( TUint8 ie_id,
                                      const HssIeOui& ie_oui,
                                      TUint8 ie_oui_type,
                                      TUint8& ie_length, 
                                      const TUint8** ie_data );
      
    /**
    * Return requested information element data.
    * @param ie_id          (IN)  Id of the requested IE data. See 802dot11.h
    * @param ie_oui         (IN)  OUI of the requested IE data. See 802dot11.h
    * @param ie_oui_type    (IN)  OUI Type of the requested IE data. See 802dot11.h
    * @param ie_oui_subtype (IN)  OUI Subtype of the requested IE data. See 802dot11.h
    * @param length         (OUT) Length of the IE. Zero if IE not found.
    * @param data           (OUT) Pointer to the beginning of the IE data. NULL if IE not found.
    * @return               General error message.
    */
    HssScanError InformationElement( TUint8 ie_id,
                                      const HssIeOui& ie_oui,
                                      TUint8 ie_oui_type,
                                      TUint8 ie_oui_subtype,
                                      TUint8& ie_length, 
                                      const TUint8** ie_data );                                       

private: //Methods

    // Prohibit copy constructor if not deriving from CBase.
    HssScanInfo( const HssScanInfo& );
    // Prohibit assigment operator if not deriving from CBase.
    HssScanInfo& operator= ( const HssScanInfo& );

    /**
    * Return current information element.
    * @param ie        (OUT) Id of the IE. See 802dot11.h.
    * @param length    (OUT) Length of the IE. Zero if IE not found.
    * @param data      (OUT) Pointer to the beginning of the IE data. NULL if IE not found.
    * @return           General error message.
    */
    HssScanError CurrentIE( TUint8& ie, 
                             TUint8& length, 
                             const TUint8** data ) const;

private:   // Data
    /** Iterator for going through IEs of scan info. */
    const TUint8* ie_iter_m;
};

#include "hssscaninfo.inl"

#endif      // HssSCANINFO_H
            
// End of File
