/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Definitions for My Videos*
*/





#ifndef VCXMYVIDEOSDEFS_H
#define VCXMYVIDEOSDEFS_H

#include <e32base.h>
#include <mpxattribute.h>
#include <mdeconstants.h>
#include <mpxcollectionpath.h>

#define VCX_DOWNLOADS_CATEGORY
#define VCX_ALBUMS

//These are written to TMPXItemId::iId2
const TInt KVcxMvcMediaTypeVideo    = 0;
const TInt KVcxMvcMediaTypeCategory = 1;
const TInt KVcxMvcMediaTypeAlbum    = 2;

/**
* My Videos Collection category IDs.
* TMPXItemId has these as iId1 member and KVcxMvcMediaTypeCategory as iId2.
*/
const TInt KVcxMvcCategoryIdAll          = 0;
const TInt KVcxMvcCategoryIdDownloads    = 1;
const TInt KVcxMvcCategoryIdTvRecordings = 2;
const TInt KVcxMvcCategoryIdCaptured     = 3;
const TInt KVcxMvcCategoryIdOther        = 4;
const TInt KCategoryIdLastWatched        = 5;
const TInt KCategoryIdExtraItem1         = 6;
const TInt KCategoryIdExtraItem2         = 7;
const TInt KCategoryIdExtraItem3         = 8;

/**
* My Videos Custom command ID
*
* This is sent to MPX video player app UI, 
* when the my videos main view is initialized, and
* player app should do the background initialization.
*/
const TInt KVcxMyVideosCmdDoLateConstruct =  0x20016B9D;

/**
* Same as KIptvUrlMaxLength in IptvUtil.h.
* Comment copied from IptvUtil.h:
* As far as I know url specification allows url to be as big as 2048, but
* S60 (v3.1/w31) download manager crashes if url length goes somewhere around
* 1200-1400 so we need to limit it here. If someone finds out that
* download manager get fixed please feel free to change value to 2048 
*/
const TInt KVcxMvcMaxUrlLength = 1024;

/**
* Maximum length of KMPXMediaGeneralTitle attribute. Limiting factor
* comes from MDS.
*/
const TInt KVcxMvcMaxTitleLength = 255;

/**
*  Content ID (UID) identifying My Videos category of content provided
*  in the media object and associated attributes. 
*/
const TInt KVcxMediaIdMyVideos = 0x20016B9D;

// real values!!
const TInt KVcxNsMpxMediatorDomain                 = 0x20016B9D;
const TInt KVcxNsMpxMediatorCategory               = 0x1;
const TInt KVcxNsMpxEventDownloadStarted           = 0x1;
const TInt KVcxNsMpxEventAllDownloadsEnded         = 0x2;
const TInt KVcxNsMpxEventDownloadCompleted         = 0x4;
const TInt KVcxNsMpxEventNewVideosCountDecreased   = 0x5;
const TInt KVcxNsMpxEventResetDownloadNotification = 0x6;
const TInt KVcxNsMpxEventVersion                   = 1;
typedef enum // Video flags
    {
    EVcxMyVideosVideoNew                = (1 << 0), // New video flag.
    EVcxMyVideosVideoScheduledDownload  = (1 << 1), // Scheduled flag, not downloaded yet.
    EVcxMyVideosVideoDeleteWarning      = (1 << 2), // Deletion warning flag.
    EVcxMyVideosVideoProtected          = (1 << 3), // Protection flag.
    EVcxMyVideosVideoDrmProtected       = (1 << 4), // DRM protection flag.
    EVcxMyVideosVideoMemoryCard         = (1 << 5), // Video located at the memorycard flag.
    EVcxMyVideosVideoPreview            = (1 << 6), // Preview, not full clip.
    EVcxMyVideosVideoRecPartiallyFailed = (1 << 7), // Recording partially failed  // DUPLICATE in harvesterrtpmetadatarader.h : do not change this!
    EVcxMyVideosVideoRecFailed          = (1 << 8), // Recording failed  // DUPLICATE in harvesterrtpmetadatarader.h : do not change this!
    EVcxMyVideosServiceHasReadOnlyIap   = (1 << 9),
    EVcxMyVideosSilent                  = (1 << 10) // Set to indicate that there is no UI present, used in KVcxCommandMyVideosStartDownload command only 
    } TVcxMyVideosVideoFlags;     

/**
* Values for KVcxMediaMyVideosDownloadState attribute.
*/
typedef enum
    {
    EVcxMyVideosDlStateNone,
    EVcxMyVideosDlStateDownloading,
    EVcxMyVideosDlStateFailed,       
    EVcxMyVideosDlStatePaused,
    
    /*
    * Collection updates media object to this state only once when the downloaded event occurs.
    * Later when the collection is opened the downloaded state is not in the MPX/MDS object anymore.
    * It is clients responsiblity to keep track whether MPX/MDS item is downloaded or not.
    * Client should do that by saving MDS ID and remote URL pairs. If the saved MDS ID is found from the
    * collection and download ID(KVcxMediaMyVideosDownloadId) is 0, then the item is downloaded.
    * If MDS ID is found and download ID != 0, then download state (KVcxMediaMyVideosDownloadState)
    * has a valid value in collection.
    */
    EVcxMyVideosDlStateDownloaded
    }  TVcxMyVideosDownloadState;
    
/**
* Values for KVcxMediaMyVideosOrigin attribute. Currently this is exactly the
* same as MDS Origin attribute. EVcxMyVideosOriginSideLoaded is treated like
* EVcxMyVideosOriginOther in My Videos collection plugin, ie side loaded
* videos are cathegorized as "other".
*/
typedef enum
    {
    EVcxMyVideosOriginOther              = MdeConstants::Object::EOther,
    EVcxMyVideosOriginCapturedWithCamera = MdeConstants::Object::ECamera,
    EVcxMyVideosOriginDownloaded         = MdeConstants::Object::EDownloaded,
    EVcxMyVideosOriginSideLoaded         = MdeConstants::Object::ESideLoaded,
    EVcxMyVideosOriginTvRecording        = MdeConstants::Object::ERecorded
    }  TVcxMyVideosOrigin;

const TInt KVcxMyVideosCollectionCenrepUid                      = 0x2001B2A9;
const TInt KVcxMyVideosCollectionCenrepKeySortingOrder          = 0x01; 
const TInt KVcxMyVideosCollectionCenrepKeyLastWatchedMpxId      = 0x02; 
const TInt KVcxMyVideosCollectionCenrepKeyLastWatchedName       = 0x03; 
const TInt KVcxMyVideosCollectionCenrepKeyLastWatchedPath       = 0x04; 
const TInt KVcxMyVideosCollectionCenrepKeyLastWatchedIndicator  = 0x05; 

const TInt KVcxMyVideosCenRepUid = 0x102750E2; // same as KIptvCenRepUid in CIptvUtil.h
const TInt KVcxMyVideosCenRepPreferredMemoryKey = 0x01; // same as KIptvCenRepPreferredMemoryKey in CIptvUtil.h
const TInt KVcxMyvideosCenRepParentControlKey = 0xC; // same as KIptvCenRepParentControlKey in CIptvUtil.h
const TInt KVcxMyvideosCenRepParentControlKeyOff = 0; // same as KIptvCenRepParentControlKeyOff in CIptvUtil.h
const TInt KVcxMyvideosCenRepParentControlKeyDefault = KVcxMyvideosCenRepParentControlKeyOff; // same as KIptvCenRepParentControlKeyDefault in CIptvUtil.h

/**
* Sorting order, this value is written to Cenrep.
* UID = KVcxMyVideosCollectionCenrepUid
* Key = KVcxMyVideosCollectionCenrepKeySortingOrder
*/
enum TVcxMyVideosSortingOrder
    {
    EVcxMyVideosSortingNone,
    EVcxMyVideosSortingId, //not supported, results can be anything
    EVcxMyVideosSortingCreationDate,
    EVcxMyVideosSortingModified, // not supported, results can be anything
    EVcxMyVideosSortingName,
    EVcxMyVideosSortingSize
    };

/**
* These values are written to EMPXItemModified events (KVcxMediaMyVideosInt32Value attribute)
* to give extra information about the event.
*/
enum TVcxMyVideosEventInfo
    {
    EVcxMyVideosListNoInfo,
    
    /**
    * This is sent for a container item, it indicates that the list under
    * the container has changed greatly ( order changed or items added or
    * removed so many that the new list has to be fetched).
    * If UI is showing the list under the container which receives this event,
    * UI must fetch new list by calling OpenL().
    */
    EVcxMyVideosVideoListOrderChanged
    };

/**
* These values are written to video list (received by HandleOpen)
* KVcxMediaMyVideosInt32Value attribute to give extra information about the list status.
*/
enum TVcxMyVideosVideoListInfo
    {    
    /**
    * Indicates that the list is complete and no more items are appended
    * to the list by KVcxMessageMyVideosItemsAppended event.
    */ 
    EVcxMyVideosVideoListComplete
    };
    
//
// These are the general attributes used in My Videos media object.
// "(BRIEF)" means that this field is filled when requesting list of videos from
// collection (using OpenL method). Other than "BRIEF" fields must be requested
// separately with MediaL method.
//

/**
*  1. (BRIEF)
*  MPX ID, TMPXItemId.
*  ID of the item.
*  If iId2 member != 0, then the item is category (=container).
*  If iId2 member == 0, then the item is media object and iId1 member is MDS ID.
*  TMPXItemId::iId1 member is saved to MDS. Ready only.
*/
//KMPXMediaGeneralId 

/**
*  2. (BRIEF)
*  Descriptor, contains video name.
*  Maximum length is KVcxMvcMaxTitleLength.
*  Saved to MDS.
*/
//KMPXMediaGeneralTitle

/**
*  3. (FULL)
*  Descriptor, description of the video. 
*  Saved to MDS.
*/
//KMPXMediaGeneralComment 

/**
*  4. (BRIEF)
*  Descriptor. File path. 
*  Saved to MDS.
*/
//KMPXMediaGeneralUri

/**
*  5. (BRIEF)
*  This should be made TUint32
*  TInt, size in bytes. 
*  Obsolete, use KMPXMediaGeneralExtSizeInt64 instead.
*/
//KMPXMediaGeneralSize

/**
*  6. (BRIEF)
*  TTime.Int64(). Storing as TTime instead TInt64
*  will result in compilation warning.
*  If video item, contains creation date of the item.
*  if category, contains New Video item's creation date.
*  Ready only.
*/
//KMPXMediaGeneralDate

/**
*  7. (BRIEF)
*  Flags, TUint32, see TMpxMyVideosVideoFlags for values.
*  Saved to MDS.
*/
//KMPXMediaGeneralFlags

/**
*  8. (FULL)
*  Descriptor, copyright information.
*  Saved to MDS.
*/
//KMPXMediaGeneralCopyright

/**
*  9. (FULL)
*  Descriptor
*  Saved to MDS. Ready only.
*/
//KMPXMediaGeneralMimeType 

/**
 *  10. (FULL)
 *  TInt, last playback position.
 *  Saved to MDS.
 */
//KMPXMediaGeneralLastPlaybackPosition

/**
 *  10.1 (FULL)
 *  TInt, clip bitrate
 *  Saved to MDS.
 */
//KMPXMediaVideoBitRate

/**
 *  10.2 (FULL)
 *  TUint16, Y-axis
 *  Saved to MDS.
 */
//KMPXMediaVideoWidth

/**
 *  10.3 (FULL)
 *  TUint16, X-axis
 *  Saved to MDS.
 */
//KMPXMediaVideoHeight

/**
 *  10.4 (FULL)
 *  Descriptor, artist name.
 *  Saved to MDS.
 */
//KMPXMediaVideoArtist

/**
*  11. (BRIEF)
*  TInt64, size in bytes. 
*  Saved to MDS.
*/
//KMPXMediaGeneralExtSizeInt64

//
// Here are My Videos Collection specific attributes:
// "(BRIEF)" means that this field is filled when requesting list of videos from
// collection. Other fields must be requested separately with Media requests.
//

/**
*  12. (FULL)
*  TTime.Int64(). Storing as TTime instead TInt64
*  will result in compilation warning.
*  Modified date.
*  Saved to MDS.
*/
const TMPXAttributeData KVcxMediaMyVideosModifiedDate = {KVcxMediaIdMyVideos, 1 << 0};

/**
 *  13. (BRIEF)
 *  TInt32, Age profile / rating age.
 *  Saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosAgeProfile = {KVcxMediaIdMyVideos, 1 << 1};

/**
 *  14. (FULL)
 *  Descriptor
 *  Saved to MDS. 
 */
const TMPXAttributeData KVcxMediaMyVideosAudioLanguage = {KVcxMediaIdMyVideos, 1 << 2};

/**
 *  15. (FULL)
 *  Descriptor
 *  Saved to MDS. 
 */
const TMPXAttributeData KVcxMediaMyVideosAuthor = {KVcxMediaIdMyVideos, 1 << 3};

/**
 *  16. (BRIEF)    
 *  TUint8, The origin of the object, see values from TVcxMyVideosOrigin.
 *  Saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosOrigin = {KVcxMediaIdMyVideos, 1 << 4};

/**
 *  17. (BRIEF)
 *  TReal32, duration of the video (play time).
 *  KMPXMediaGeneralDuration can't be used since it is TInt.
 *  Saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosDuration = {KVcxMediaIdMyVideos, 1 << 5};

/**
 *  18. (BRIEF)    
 *  TUint32. Download ID in Download Manager, 0 means none.
 *  Saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosDownloadId = {KVcxMediaIdMyVideos, 1 << 6};

/**
 *  19. (BRIEF)    
 *  TUint8. Rating.
 *  Saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosRating = {KVcxMediaIdMyVideos, 1 << 7};

/**
 *  20. (BRIEF)
 *  TUint32.
 *  Saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosAudioFourCc = {KVcxMediaIdMyVideos, 1 << 8};

/**
 *  21. (BRIEF)
 *  Descriptor. KMPXMediaGeneralUri contains local filename and this has the
 *  remote url.
 *  Not saved to MDS, only used in downloading.
 */
const TMPXAttributeData KVcxMediaMyVideosRemoteUrl = {KVcxMediaIdMyVideos, 1 << 9};

/**
 *  22. (BRIEF)
 *  TUint8, download state, see values from TVcxMyVideosDownloadState. 
 *  Not saved to MDS, only used in downloading.
 */
const TMPXAttributeData KVcxMediaMyVideosDownloadState = {KVcxMediaIdMyVideos, 1 << 10};

/**
 *  23.
 *  TUint32, IAP ID in CommsDb.
 *  Not saved to MDS, only used in downloading.
 */
const TMPXAttributeData KVcxMediaMyVideosIapId = {KVcxMediaIdMyVideos, 1 << 11};

/**
 *  24.
 *  Descriptor. Username. This is used in KVcxCommandMyVideosStartDownload only.
 *  Client gets the value from services database.
 *  Not saved to MDS, only used in downloading.
 */
const TMPXAttributeData KVcxMediaMyVideosUsername = {KVcxMediaIdMyVideos, 1 << 12};

/**
 *  25.
 *  Descriptor. Password. This is used in KVcxCommandMyVideosStartDownload only.
 *  Client gets the value from services database.
 *  Not saved to MDS, only used in downloading.
 */
const TMPXAttributeData KVcxMediaMyVideosPassword = {KVcxMediaIdMyVideos, 1 << 13};

/**
 *  26.
 *  TUint32. My Videos command/message ID. Used when sending commands to my videos collection
 *  and when collection sends messages to client.
 *  KMPXCommandGeneralId contains KVcxCommandIdMyVideos.
 *  KVcxMediaMyVideosCommandId contains the operation ID.
 *  Not saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosCommandId = {KVcxMediaIdMyVideos, 1 << 14};

/**
 *  27.
 *  TInt8. Download progress. My Videos collection updates this value when download is
 *  active. Note that collection does not send events when progress is changed. User
 *  should read the value frequently when download is progressing.
 *  Not saved to MDS, only used in downloading.
 */
const TMPXAttributeData KVcxMediaMyVideosDownloadProgress = {KVcxMediaIdMyVideos, 1 << 15};

/**
 *  28.
 *  TUint32, This attribute is only used in category items. Indicates how many items are under it.
 *  Not saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosCategoryItemCount = {KVcxMediaIdMyVideos, 1 << 16};

/**
 *  29.
 *  TUint32. This attribute is only used in category items. Indicates how many new items are under it.
 *  Not saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosCategoryNewItemCount = {KVcxMediaIdMyVideos, 1 << 17};

/**
 *  30.
 *  TUint32. Identifies collection message, used for debugging. Collection increases the ID by
 *  one every time a message is sent. If client detects that ID has increased more than by one
 *  since the last message, it knows that a message has been lost.
 *  Not saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosMessageId = {KVcxMediaIdMyVideos, 1 << 18};

/**
 * 31.
 * TInt32. When download fails, the reason is stored here. See possible values from
 * HttpDownloadMgrCommon.h THttpDownloadMgrError type.
 * Not saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosDownloadError = {KVcxMediaIdMyVideos, 1 << 19};

/**
 * 32.
 * TInt32. When download fails, the global reason is stored here. See possible values from
 * HttpDownloadMgrCommon.h EDlAttrGlobalErrorId.
 * Not saved to MDS.
 */
const TMPXAttributeData KVcxMediaMyVideosDownloadGlobalError = {KVcxMediaIdMyVideos, 1 << 20};

/**
 * 33.
 * Descriptor. Name of the latest non watched item. Used only in categories.
 * Not saved to MDS. 
 */
const TMPXAttributeData KVcxMediaMyVideosCategoryNewItemName = {KVcxMediaIdMyVideos, 1 << 21};

/**
 * 34.
 * TInt32, general parameter, the usage depends on context.
 * Not saved to MDS
 */
const TMPXAttributeData KVcxMediaMyVideosInt32Value = {KVcxMediaIdMyVideos, 1 << 22};

/**
 * 35.
 * TUint32, general parameter, the usage depends on context.
 * Not saved to MDS
 */
const TMPXAttributeData KVcxMediaMyVideosUint32Value = {KVcxMediaIdMyVideos, 1 << 23};

/**
 * 36.
 * TUint32, transaction ID, client sets this field when making asynchronous request.
 * Collection sets the same ID to response messages. Client can use this to pair
 * requests and responses.
 * Not saved to MDS
 */
const TMPXAttributeData KVcxMediaMyVideosTransactionId = {KVcxMediaIdMyVideos, 1 << 24};

/**
 * 37.
 * TBool, video list is partial. This is set to Category level video list to inform
 * client that video cache is not complete yet.
 */
const TMPXAttributeData KVcxMediaMyVideosVideoListIsPartial = {KVcxMediaIdMyVideos, 1 << 24};


// -------- end of my videos media attributes -------- //

/*
* Unique ID for My Videos collection commands and messages, this is set to KMPXCommandGeneralId
* field in CMPXCommand and CMPXMessage classes.
*/
const TUint KVcxCommandIdMyVideos = 0x20016BA0;

/**
* My Videos command and message IDs. These
* are set to KVcxMediaMyVideosCommandId attribute, in both
* CMPXCommand and CMPXMessage classes.
*/
const TInt KVcxCommandMyVideosStartDownload              = 0;
const TInt KVcxCommandMyVideosCancelDownload             = 1;
const TInt KVcxCommandMyVideosResumeDownload             = 2;
const TInt KVcxCommandMyVideosPauseDownload              = 3;
const TInt KVcxCommandMyVideosGetMediaFullDetailsByMpxId = 4; //async
const TInt KVcxCommandMyVideosGetMediasByMpxId           = 5; //async
const TInt KVcxCommandMyVideosMove                       = 6; //async
const TInt KVcxCommandMyVideosCopy                       = 7; //async
const TInt KVcxCommandMyVideosDelete                     = 8; //async
const TInt KVcxCommandMyVideosAddToAlbum                 = 9; //async
const TInt KVcxCommandMyVideosRemoveFromAlbum            = 10; //async
const TInt KVcxCommandMyVideosAddAlbum                   = 11; //async & sync
const TInt KVcxCommandMyVideosRemoveAlbums               = 12; //async

/** This command cancels the move or copy operation only if it
 *  is allready being processed by the collection plugin. If the
 *  command is still in queue, this cancel has no effect.
 *  If Move or Copy operations were not in progress the command
 *  leaves with KErrNotFound.
 */
const TInt KVcxCommandMyVideosCancelMoveOrCopy = 13; //sync

/** This command cancels the delete operation only if it
 *  is allready being processed by the collection plugin. If the
 *  command is still in queue, this cancel has no effect.
 *  If delete operation was not in progress the command
 *  leaves with KErrNotFound.
 */
const TInt KVcxCommandMyVideosCancelDelete = 14; //sync

const TInt KVcxMessageMyVideosGetMediasByMpxIdResp = 15;

/**
* Collection sends this message when new items are added
* to the end of the collection plugins cache. If client has not yet received
* KVcxMessageMyVideosListComplete event, then the media list received with
* previous HandleOpen contains already the new items at the end of the list.
* Client can draw them immedetially. However client must still do a new OpenL()
* call to keep collection frameworks path variable updated.
* KVcxMessageMyVideosItemsAppended and KVcxMessageMyVideosListComplete events
* are broadcasted to all clients. So they might appear even when client hasn't
* done OpenL.
*/
const TInt KVcxMessageMyVideosItemsAppended = 16; //event

/**
* Collection sends this message when media list fetching has ended
* and no more KVcxMessageMyVideosItemsAppended events will be sent.
* Collection also cuts off the shared memory connection to video lists
* between clients and collection. Ie, if new KVcxMessageMyVideosItemsAppended
* events arrive after this event, the client knows that they are not automatically
* in the list it had received earlier (unless client itself initiated the new fetching
* by calling OpenL().
*/
const TInt KVcxMessageMyVideosListComplete = 17; //event

/* Collection sends this message when it starts processing
 * Move Or Copy command. When this event is received,
 * KVcxCommandMyVideosCancelMoveOrCopy can be sent. 
 */
const TInt KVcxMessageMyVideosMoveOrCopyStarted = 18; //event

/* Collection sends this message when it starts processing
 * KVcxCommandMyVideosDelete command. After this event is received,
 * KVcxCommandMyVideosCancelDelete can be sent. 
 */
const TInt KVcxMessageMyVideosDeleteStarted = 19; //event

/* End events for Move/Copy/Delete operations. The response contains
 * MDS ID and error codes for each item.
 */
const TInt KVcxMessageMyVideosMoveResp   = 20; //event 
const TInt KVcxMessageMyVideosCopyResp   = 21; //event
const TInt KVcxMessageMyVideosDeleteResp = 22; //event

/**
* Indicates that media array variable contains other messages.
* This message is only sent as the highest level item in the message tree.
*/
const TInt KVcxMessageMyVideosMessageArray = 23;

/*
 OpenL() (video list) MSC
 ------------------------
 Opening the collection is quite complicated due the fact that it allows multiple clients to open
 video list at the same time. The scenario described below involves video list fetching from MDS. If the collection
 has the video list already fetched, then the HandleOpenL will contain all videos and KVcxMessageMyVideosListComplete
 is received immedetially.
 
 The list complete status (EVcxMyVideosVideoListComplete) is also written to aEntries KVcxMediaMyVideosInt32Value
 attribute when the list is complete. 
  
                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().OpenL()                                                               |
                    |--------------------------------------------------------------------------------->|
                    |                                                                                  |
                    |  HandleOpenL(const CMPXMedia& aEntries)                                          |
                    |    This is returned immedetially by the collection, the list might be empty.     |
                    |    Client should prevent all calls which cause access to MDS, until              |
                    |    KVcxMessageMyVideosListComplete event is received. This is due the fact that  |
                    |    MDS fails commands when there is video list fetching going on.                |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    |                                                                                  |
                    |  HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                   |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId       = KVcxCommandIdMyVideos                          |
                    |      KVcxMediaMyVideosCommandId = KVcxMessageMyVideosItemsAppended               |
                    |      KMPXMessageCollectionId    = KVcxUidMyVideosMpxCollection                   |
                    |                                                                                  |
                    |  At this point the new items are at the end of aEntries received earlier with    |
                    |  "HandleOpenL".                                                                  | 
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    |                                                                                  |
                    |                                                                                  |
                    |  HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                   |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId       = KVcxCommandIdMyVideos                          |
                    |      KVcxMediaMyVideosCommandId = KVcxMessageMyVideosListComplete                |
                    |      KMPXMessageCollectionId    = KVcxUidMyVideosMpxCollection                   |
                    |                                                                                  |
                    |  The shared memory link between collection video list and list received          |
                    |  by the client is cut off. Single video media items still point to same shared   |
                    |  memory.                                                                         |
                    |                                                                                  |
                    |  If new KVcxMessageMyVideosItemsAppended events arrive                           |
                    |  after this point (without client doing OpenL()), it means that someone else     |
                    |  changed sorting order and started fetching new video list.                      |
                    |  In that case this client should also fetch new list by calling OpenL().         |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    |                                                                                  |


GET MEDIA FULL DETAILS BY MPX ID MSC
------------------------------------

Fetches media full details from MDS using MPX ID.

 
                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().CommandL( *getMediaFullDetailsByMpxIdCommand )                        |
                                                                                                       |
getMediaFullDetailsByMpxIdCommand (CMPXCommand type) contains:                                         |
    KMPXCommandGeneralId           = KVcxCommandIdMyVideos                                             |
    KVcxMediaMyVideosCommandId     = KVcxCommandMyVideosGetMediaFullDetailsByMpxId                     |
    KMPXMediaGeneralId             = MPX ID (item to fetch)                                            |
                    |--------------------------------------------------------------------------------->|
                    |                                                                                  |
                    |                                                                                  | 
                    |  HandleCommandComplete(getMediaFullDetailsByMpxIdCommand, FAIL/SUCCESS)          |
                    |                                                                                  |
                    |  getMediaFullDetailsByMpxIdCommand contains:                                     |
                    |      KVcxMediaMyVideosCommandId = KVcxMessageMyVideosGetMediaFullDetailsByMpxId  |
                    |      KMPXCommandColAddMedia     = The fetched video (CMPXMedia* type)            |
                    |  Contains also all the same data which was sent in getMediaFullDetailsByMpxIdCommand.
                    |  On failure the KMPXCommandColAddMedia is not supported                          |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |                 

GET MEDIAS BY MPX ID MSC
------------------------

 This command loads requested MDS items to MPX My Videos collection cache and returns the requested
 items to the client ( subset of the cache ). Notice that if this command is given while OpenL is
 being processed, then OpenL is interrupted and started from scratch when this command finishes.
 
                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().CommandL( *getMediasByMpxIdCommand )                                  |
                                                                                                       |
getMediasByMpxIdCommand (CMPXCommand type) contains:                                                   |
    KMPXCommandGeneralId           = KVcxCommandIdMyVideos                                             |
    KVcxMediaMyVideosCommandId     = KVcxCommandMyVideosGetMediasByMpxId                               |
    KMPXCommandGeneralCollectionId = KVcxUidMyVideosMpxCollection                                      |
    KMPXMediaArrayContents         = Media array (CMPXMediaArray)                                      |
        Media array contains array of CMPXMedia objects which contain:                                 |
        KMPXMessageMediaGeneralId  = MPX ID                                                            |
                    |--------------------------------------------------------------------------------->|
                    |                                                                                  |
                    |                                                                                  | 
                    |  HandleCommandComplete(getMediasByMpxIdCommand, FAIL/SUCCESS)                    |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    |                                                                                  |
                    |  HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                   |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXCommandGeneralId       = KVcxCommandIdMyVideos                          |
                    |      KVcxMediaMyVideosCommandId = KVcxMessageMyVideosGetMediasByMpxIdResp        |
                    |      KMPXMediaArrayContents     = Media array (CMPXMediaArray)                   |
                    |      KMPXMessageCollectionId    = KVcxUidMyVideosMpxCollection                   |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |                 


VIDEO ADD MSC
-------------

Currently only synchronous version is supported. Only one item can be added per call. This is currently
available only for udeb builds for testing purposes. Urel will leave with KErrNotSupported.
Does not make checks for duplicate items for the same filename. If urel version is required in a future,
then the command should be asynchronous and perform necessarry checks.
 
                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().CommandL( *videoAddCommand )                                          |
                                                                                                       |
videoAddCommand (CMPXCommand type) contains:                                                           |
    KMPXCommandGeneralId           = KMPXCommandIdCollectionAdd                                        |
    KMPXCommandGeneralCollectionId = 0x20016B97                                                        |
    KMPXCommandColAddMedia         = media to be added (CMPXMedia type)                                |
        KMPXCommandColAddMedia contains all or subset of parameters which are saved to MDS,            |
        other parameters are ignored. On return the KMPXCommandColAddMedia KMPXMediaGeneralId          |
        attribute contains the new MPX ID.                                                             |
                    |--------------------------------------------------------------------------------->|
                    |                                                                                  |
                    |                                                                                  |
                    |                                                                                  |
                    | HandleCollectionMessage( CMPXMessage aMessage, TInt aError )                     |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId       = KMPXMessageIdItemChanged                       |
                    |      KMPXMessageChangeEventType = EMPXItemInserted                               |
                    |      KMPXMessageCollectionId    = KVcxUidMyVideosMpxCollection                   |
                    |      KMPXMessageMediaGeneralId  = MPX ID                                         |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |

VIDEO SET MSC
-------------

Currently only synchronous version is supported. Only one item can be modified per call.

NOTE: Don't write directly to a media object fetched with OpenL(). Those objects are shared with collection.
Instead create a new media object with CMPXMedia::NewL() and set the attributes you want to change.
Set MPX ID of the modified item to KMPXMediaGeneralId attribute. It is used by the collection to find
out which item will be modified. Set this media object to KMPXCommandColSetMedia attribute in CMPXCommand.
 
                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().CommandL( *videoSetCommand )                                          |
                                                                                                       |
videoSetCommand (CMPXCommand type) contains:                                                           |
    KMPXCommandGeneralId           = KMPXCommandIdCollectionSet                                        |
    KMPXCommandGeneralCollectionId = 0x20016B97                                                        |
    KMPXCommandColSetMedia         = media to be set (CMPXMedia type)                                  |
        Media to be set (CMPXMedia type) contains all or subset of parameters which are saved to MDS,  |
        other parameters are ignored. KMPXMediaGeneralId, KMPXMediaGeneralDate and                     | 
        KMPXMediaGeneralMimeType are read only parameters, they can't be modified either.              |
        Set values to attributes which are needed to change, that gives the best performance, ie       |
        don't make a copy of media item from collection, change something and use that. Instead create |
        a new empty media item and set only values you wish to change.                                 |
                    |--------------------------------------------------------------------------------->|
                    |                                                                                  |
                    |                                                                                  |
                    |                                                                                  |
                    | HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                    |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId       = KMPXMessageIdItemChanged                       |
                    |      KMPXMessageChangeEventType = EMPXItemModified                               |
                    |      KMPXMessageCollectionId    = KVcxUidMyVideosMpxCollection                   |
                    |      KMPXMessageMediaGeneralId  = MPX ID                                         |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |

VIDEO MOVE MSC
--------------

 Copy is similar, only message ids are different.
 
                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().CommandL( *videoMoveCommand )                                         |
                                                                                                       |
videoMoveCommand (CMPXCommand type) contains:                                                          |
    KMPXCommandGeneralId           = KVcxCommandMyVideosMove                                           |
    KMPXCommandGeneralCollectionId = 0x20016B97                                                        |
    KMPXCommandGeneralDoSync       = EFalse                                                            |
    KVcxMediaMyVideosInt32Value    = Target Drive                                                      |
    KVcxMediaMyVideosTransactionId = TR ID                                                             |
    KMPXMediaArrayContents         = Media array (CMPXMediaArray)                                      |
        Media array contains array of CMPXMedia objects which contain:                                 |
        KMPXMessageMediaGeneralId  = MPX ID                                                            |
                    |--------------------------------------------------------------------------------->|
                    |                                                                                  |
                    |                                                                                  |
                    |                                                                                  |
                    | HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                    |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId       = KVcxCommandIdMyVideos                          |
                    |      KVcxMediaMyVideosCommandId = KVcxMessageMyVideosMoveOrCopyStarted           |
                    |      KMPXMessageCollectionId    = KVcxUidMyVideosMpxCollection                   |
                    |      KVcxMediaMyVideosTransactionId = TR ID                                      |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    |                                                                                  |
                    |                                                                                  |
                    |                                                                                  |
                    | HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                    |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId           = KVcxCommandIdMyVideos                      |
                    |      KVcxMediaMyVideosCommandId     = KVcxMessageMyVideosMoveResp                |
                    |      KMPXMessageCollectionId        = KVcxUidMyVideosMpxCollection               |
                    |      KVcxMediaMyVideosTransactionId = TR ID                                      |
                    |       KMPXMediaArrayContents         = Media array (CMPXMediaArray)              |
                    |          Media array contains array of CMPXMedia objects which contain:          |
                    |          KMPXMessageMediaGeneralId   = MPX ID                                    |
                    |          KVcxMediaMyVideosInt32Value = result (system wide error code)           |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    |  HandleCommandComplete(videoMoveCommand, KErrNone)                               |
                    |      videoDeleteCommand KVcxMediaMyVideosInt32Value contains KErrNone if everything went
                    |      OK. If value is != KErrNone, then something went wrong and command might    |
                    |      have ended prematurely                                                      |
                    |<---------------------------------------------------------------------------------|
                    

VIDEO DELETE MSC
----------------

 
                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().CommandL( *videoDeleteCommand )                                       |
                                                                                                       |
videoDeleteCommand (CMPXCommand type) contains:                                                        |
    KMPXCommandGeneralId           = KVcxCommandMyVideosDelete                                         |
    KMPXCommandGeneralCollectionId = 0x20016B97                                                        |
    KMPXCommandGeneralDoSync       = EFalse                                                            |
    KVcxMediaMyVideosTransactionId = TR ID                                                             |
    KMPXMediaArrayContents         = Media array (CMPXMediaArray)                                      |
        Media array contains array of CMPXMedia objects which contain:                                 |
        KMPXMessageMediaGeneralId  = MPX ID                                                            |
                    |--------------------------------------------------------------------------------->|
                    |                                                                                  |
                    |                                                                                  |
                    |                                                                                  |
                    | HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                    |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId       = KVcxCommandIdMyVideos                          |
                    |      KVcxMediaMyVideosCommandId = KVcxMessageMyVideosDeleteStarted               |
                    |      KMPXMessageCollectionId    = KVcxUidMyVideosMpxCollection                   |
                    |      KVcxMediaMyVideosTransactionId = TR ID                                      |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    |                                                                                  |
                    |                                                                                  |
                    |                                                                                  |
                    | HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                    |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId           = KVcxCommandIdMyVideos                      |
                    |      KVcxMediaMyVideosCommandId     = KVcxMessageMyVideosDeleteResp              |
                    |      KMPXMessageCollectionId        = KVcxUidMyVideosMpxCollection               |
                    |      KVcxMediaMyVideosTransactionId = TR ID                                      |
                    |       KMPXMediaArrayContents         = Media array (CMPXMediaArray)              |
                    |          Media array contains array of CMPXMedia objects which contain:          |
                    |          KMPXMessageMediaGeneralId   = MPX ID                                    |
                    |          KVcxMediaMyVideosInt32Value = result (system wide error code)           |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    |  HandleCommandComplete(videoDeleteCommand, KErrNone)                             |
                    |      videoDeleteCommand KVcxMediaMyVideosInt32Value contains KErrNone if everything went
                    |      OK. If value is != KErrNone, then something went wrong and command was ended|
                    |      prematurely.                                                                |
                    |<---------------------------------------------------------------------------------|
                    

CANCEL VIDEO DELETE MSC
-----------------------



ADD VIDEOS TO ALBUM MSC
-----------------------
Notice that adding items to album does not add videos from collection. Ie item added events
will not arrive for this operation. Instead the modify event is sent for the album item which is
affected.
                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().CommandL( *cmd )                                                      |
                                                                                                       |
cmd (CMPXCommand type) contains:                                                                       |
    KMPXCommandGeneralId           = KVcxCommandIdMyVideos                                             |
    KVcxMediaMyVideosCommandId     = KVcxCommandMyVideosAddToAlbum                                     |
    KMPXCommandGeneralCollectionId = 0x20016B97                                                        |
    KMPXCommandGeneralDoSync       = EFalse                                                            |
    KVcxMediaMyVideosTransactionId = TR ID                                                             |
    KVcxMediaMyVideosUint32Value   = Album MDS ID (album where to add)                                 |
    KMPXMediaArrayContents         = Media array (CMPXMediaArray)                                      |
        Media array contains array of CMPXMedia objects which contain:                                 |
           KMPXMediaGeneralId = video MPX ID (video to add)                                            |           
                    |--------------------------------------------------------------------------------->|
                    |                                                                                  |
                    |  HandleCommandComplete(cmd, KErrNone)                                            |
                    |      cmd is the same as was given in .CommandL() call.                           |
                    |      cmd contains in addition to parameters set earlier:                         |
                    |         1. Media array has error codes set to each array item:                   |
                    |            KVcxMediaMyVideosInt32Value = KErrNone if successful,                 |
                    |             KErrGeneral or KErrAlreadyExists otherwise.                          |
                    |         2. cmd KVcxMediaMyVideosInt32Value contains KErrNone if everything went  |
                    |            OK. If value is != KErrNone, then something went wrong and individual |
                    |            error codes for array items might not contain values.                 |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    | HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                    |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId        = KMPXMessageIdItemChanged                      |
                    |      KMPXMessageChangeEventType  = EMPXItemModified                              |
                    |      KVcxMediaMyVideosInt32Value = EVcxMyVideosVideoListOrderChanged             |
                    |      KMPXMessageCollectionId     = KVcxUidMyVideosMpxCollection                  |
                    |      KMPXMessageMediaGeneralId   = MPX ID (ID of the album which had videos added)
                    |<---------------------------------------------------------------------------------|


REMOVE VIDEOS FROM ALBUM MSC
----------------------------

Notice that removing items from album does not remove videos from collection. Ie item deleted events
will not arrive for this operation. Instead the modify event is sent for the album item which is
affected.
                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().CommandL( *cmd )                                                      |
                                                                                                       |
cmd (CMPXCommand type) contains:                                                                       |
    KMPXCommandGeneralId           = KVcxCommandIdMyVideos                                             |
    KVcxMediaMyVideosCommandId     = KVcxCommandMyVideosRemoveFromAlbum                                |
    KMPXCommandGeneralCollectionId = 0x20016B97                                                        |
    KMPXCommandGeneralDoSync       = EFalse                                                            |
    KVcxMediaMyVideosTransactionId = TR ID                                                             |
    KVcxMediaMyVideosUint32Value   = Album MDS ID (iId1 member of TMPXItemId, album where from to remove) 
    KMPXMediaArrayContents         = Media array (CMPXMediaArray)                                      |
        Media array contains array of CMPXMedia objects which contain:                                 |
           KMPXMediaGeneralId = video MPX ID (video to remove)                                         |           
                    |--------------------------------------------------------------------------------->|
                    |                                                                                  |
                    |  HandleCommandComplete(cmd, KErrNone)                                            |
                    |      cmd is the same as was given in .CommandL() call.                           |
                    |      cmd contains in addition to parameters set earlier:                         |
                    |         1. Media array has error codes set to each array item:                   |
                    |            KVcxMediaMyVideosInt32Value = KErrNone if successful, KErrGeneral otherwise.
                    |         2. cmd KVcxMediaMyVideosInt32Value contains KErrNone if everything went  |
                    |            OK. If value is != KErrNone, then something went wrong and individual |
                    |            error codes for array items might not contain values.                 |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    | HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                    |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId        = KMPXMessageIdItemChanged                      |
                    |      KMPXMessageChangeEventType  = EMPXItemModified                              |
                    |      KVcxMediaMyVideosInt32Value = EVcxMyVideosVideoListOrderChanged             |
                    |      KMPXMessageCollectionId     = KVcxUidMyVideosMpxCollection                  |
                    |      KMPXMessageMediaGeneralId   = MPX ID (ID of the album which had videos removed)
                    |<---------------------------------------------------------------------------------|

ADD ALBUM MSC
-------------

Asynchronous version:

                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().CommandL( *cmd )                                                      |
                                                                                                       |
cmd (CMPXCommand type) contains:                                                                       |
    KMPXCommandGeneralId           = KVcxCommandIdMyVideos                                             |
    KVcxMediaMyVideosCommandId     = KVcxCommandMyVideosAddAlbum                                       |
    KMPXCommandGeneralCollectionId = KVcxUidMyVideosMpxCollection (0x20016B97)                         |
    KMPXCommandGeneralDoSync       = EFalse                                                            |
    KVcxMediaMyVideosTransactionId = TR ID                                                             |
    KMPXMediaGeneralTitle          = Album title text                                                  |
                    |--------------------------------------------------------------------------------->|
                    |                                                                                  |
                    |  HandleCommandComplete(cmd, KErrNone)                                            |
                    |      cmd is the same as was given in .CommandL() call.                           |
                    |      cmd contains in addition to parameters set earlier:                         |
                    |         1. cmd KVcxMediaMyVideosInt32Value contains KErrNone if everything went  |
                    |            OK. If value is != KErrNone, then something went wrong.               |
                    |         2. KMPXMediaGeneralId contains the MPX ID of the new album.              |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    | HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                    |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId        = KMPXMessageIdItemChanged                      |
                    |      KMPXMessageChangeEventType  = EMPXItemInserted                              |
                    |      KVcxMediaMyVideosInt32Value = EVcxMyVideosListNoInfo                        |
                    |      KMPXMessageCollectionId     = KVcxUidMyVideosMpxCollection                  |
                    |      KMPXMessageMediaGeneralId   = MPX ID (ID of the new album)                  |
                    |      KMPXCommandColAddMedia      = MPX media of the new album (CMPXMedia)        |
                    |<---------------------------------------------------------------------------------|

Synchronous version:

CommandL leaves if error occurs.

                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().CommandL( *cmd )                                                      |
                                                                                                       |
cmd (CMPXCommand type) contains:                                                                       |
    KMPXCommandGeneralId           = KVcxCommandIdMyVideos                                             |
    KVcxMediaMyVideosCommandId     = KVcxCommandMyVideosAddAlbum                                       |
    KMPXCommandGeneralCollectionId = KVcxUidMyVideosMpxCollection (0x20016B97)                         |
    KMPXCommandGeneralDoSync       = ETrue                                                             |
    KVcxMediaMyVideosTransactionId = TR ID                                                             |
    KMPXMediaGeneralTitle          = Album title text                                                  |
                                                                                                       |
    After the call KMPXMediaGeneralId attribute contains the MPX ID of the new album.                  |
                    |--------------------------------------------------------------------------------->.
                    |<---------------------------------------------------------------------------------'
                    |                                                                                  |
                    | HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                    |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId        = KMPXMessageIdItemChanged                      |
                    |      KMPXMessageChangeEventType  = EMPXItemInserted                              |
                    |      KVcxMediaMyVideosInt32Value = EVcxMyVideosListNoInfo                        |
                    |      KMPXMessageCollectionId     = KVcxUidMyVideosMpxCollection                  |
                    |      KMPXMessageMediaGeneralId   = MPX ID (ID of the new album)                  |
                    |      KMPXCommandColAddMedia      = MPX media of the new album (CMPXMedia)        |
                    |<---------------------------------------------------------------------------------|



REMOVE ALBUMS MSC
-----------------

                 .------.                                                                 .-------------------------.
                 |CLIENT|                                                                 |MPX My Videos Collection |
                 '------'                                                                 '-------------------------'
                    |                                                                                  |
                    |                                                                                  |
iCollectionUtility->Collection().CommandL( *cmd )                                                      |
                                                                                                       |
cmd (CMPXCommand type) contains:                                                                       |
    KMPXCommandGeneralId           = KVcxCommandIdMyVideos                                             |
    KVcxMediaMyVideosCommandId     = KVcxCommandMyVideosRemoveAlbums                                   |
    KMPXCommandGeneralCollectionId = KVcxUidMyVideosMpxCollection (0x20016B97)                         |
    KMPXCommandGeneralDoSync       = EFalse (only async supported)                                     |
    KVcxMediaMyVideosTransactionId = TR ID                                                             |
    KMPXMediaArrayContents         = Media array (CMPXMediaArray)                                      |
        Media array contains array of CMPXMedia objects which contain:                                 |
           KMPXMediaGeneralId = album MPX ID (album to remove)                                         |           
                    |--------------------------------------------------------------------------------->|
                    |                                                                                  |
                    |  HandleCommandComplete(cmd, KErrNone)                                            |
                    |      cmd is the same as was given in .CommandL() call.                           |
                    |      cmd contains in addition to parameters set earlier:                         |
                    |         1. Media array has error codes set to each array item:                   |
                    |            KVcxMediaMyVideosInt32Value = KErrNone if successful, KErrGeneral otherwise.
                    |         2. cmd KVcxMediaMyVideosInt32Value contains KErrNone if everything went  |
                    |            OK. If value is != KErrNone, then something went wrong and individual |
                    |            error codes for array items might not contain values.                 |
                    |<---------------------------------------------------------------------------------|
                    |                                                                                  |
                    | HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )                    |
                    |                                                                                  |
                    |  aMessage contains:                                                              |
                    |      KMPXMessageGeneralId        = KMPXMessageIdItemChanged                      |
                    |      KMPXMessageChangeEventType  = EMPXItemDeleted                               |
                    |      KVcxMediaMyVideosInt32Value = EVcxMyVideosListNoInfo                        |
                    |      KMPXMessageCollectionId     = KVcxUidMyVideosMpxCollection                  |
                    |      KMPXMessageMediaGeneralId   = MPX ID (ID of the album which was removed)    |
                    |<---------------------------------------------------------------------------------|

*/

#endif // VCXMYVIDEOSDEFS_H

