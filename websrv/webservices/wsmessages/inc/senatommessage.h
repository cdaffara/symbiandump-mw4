/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/






#ifndef CSENATOMMESSAGE_H_
#define CSENATOMMESSAGE_H_

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h>

#include <SenDomFragmentBase.h>
#include <MSenMessage.h>

// CONSTANTS
_LIT8(KSenAtomEntryXmlns,                 "http://www.w3.org/2005/Atom");
_LIT8(KSenAtomFeedName,                   "feed");
_LIT8(KSenAtomEntryName,                  "entry");

_LIT8(KSenAtomCategoryName,               "category");
_LIT8(KSenAtomGroupName,                  "group");
_LIT8(KSenAtomTitleName,                     "title");
_LIT8(KSenAtomUpdatedName,                "updated");
_LIT8(KSenAtomPublishedName,                "published");
_LIT8(KSenAtomIdName,                    "id");
_LIT8(KSenAtomLinkName,                    "link");
_LIT8(KSenAtomSubtitleName,                "subtitle");
_LIT8(KSenAtomGeneratorName,                "generator");
_LIT8(KSenAtomIconName,                      "icon");
_LIT8(KSenAtomLogoName,                      "logo");
_LIT8(KSenAtomRightsName,                 "rights");
_LIT8(KSenAtomContributorName,           "contributor");
_LIT8(KSenAtomContentName,                "content");
_LIT8(KSenAtomAuthorName,                "author");

_LIT8(KSenAtomName,                         "name"); 
_LIT8(KSenAtomURI,                          "URI");
_LIT8(KSenAtomEmail,                      "email");
_LIT8(KSenAtomVersion,                      "version");
_LIT8(KSenAtomHref,                        "href");
_LIT8(KSenAtomRel,                        "rel");
_LIT8(KSenAtomType,                        "type");
_LIT8(KSenAtomHrefLang,                     "hrefLang");
_LIT8(KSenAtomTitle,                        "title");
_LIT8(KSenAtomLength,                       "length");
_LIT8(KSenAtomPrefix,                        " ");
_LIT8(KSenAtomTerm,                        "term");
_LIT8(KSenAtomScheme,                        "scheme");
_LIT8(KSenAtomLabel,                        "label");

_LIT8(KSenAtomSpace,                      "");



/// id for data class instances
typedef TInt64 TAtomDataId;

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*     Provides generic object to keep and manipulate tags of atom-pub API.
* 
*   @lib SenMessages.DLL
*   @since 5.0
*/

class CSenAtomMessage : public CSenFragmentBase, public MSenMessage
    {
    public:  // Constructors and destructor
                      
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CSenAtomMessage();   
        
    public:
        IMPORT_C TAtomDataId Id() const;
        
    public: // From MSenMessage
        IMPORT_C virtual TClass Type();
        IMPORT_C virtual TDirection Direction();
        IMPORT_C virtual TInt SetContext(MSenMessageContext* apOwnedContext);
        IMPORT_C virtual MSenMessageContext* Context();
        IMPORT_C virtual TInt SetProperties(MSenProperties* apOwnedProperties);
        IMPORT_C virtual MSenProperties* Properties();
        IMPORT_C virtual TBool IsSafeToCast(TClass aType);
        IMPORT_C virtual TInt TxnId();
        IMPORT_C virtual MSenMessage* CloneL();
        
        // From CSenFragmentBase
        IMPORT_C virtual void OnStartElementL(const RTagInfo& aElement,
                                              const RAttributeArray& aAttributes,
                                              TInt aErrorCode);                                            

        IMPORT_C virtual void OnEndElementL(const RTagInfo& aElement,
                                            TInt aErrorCode);
        IMPORT_C virtual CSenFragmentBase* CreateBodyFragmentL(const TDesC8& aNsUri,
                                                       const TDesC8& aLocalName,
                                                       const TDesC8& aPrefix,
                                                       const RAttributeArray& aAttributes,
                                                       TXmlEngElement& aParent,
                                                       RSenDocument& aOwnerDocument);
        
        // Own functions
        
        /*
        *        Method returns content of "title" tag
        *         @return - descriptor containing value of title.
        */        
        IMPORT_C const TDesC8& Title() const;
        
        /*
        *        Method sets content of "title" tag
        *         @param aTitle - descriptor that contains value of subtitle. 
        */
        IMPORT_C void SetTitleL( const TDesC8& aTitle );
        
        /*
        *        Method returns content of "updated" tag
        *         @return - descriptor containing value of updated tag.
        */        
        IMPORT_C const TDesC8& Updated() const;
        
        /*
        *        Method sets content of "updated" tag
        *         @param aUpdated - descriptor that contains value of updated tag.
        */        
        IMPORT_C void SetUpdatedL( const TDesC8& aUpdated );
        
        /*
        *        Method returns content of "published" tag
        *         @return - descriptor containing value of published tag.
        */        
        IMPORT_C const TDesC8& Published() const;
        
        /*
        *        Method sets content of "published" tag
        *         @param aPublished - descriptor that contains value of published tag.
        */        
        IMPORT_C void SetPublishedL( const TDesC8& aPublished );
        
        /*
        *        Method returns content of "id" tag. Content of this tag usually identifies underlying entry or feed. 
        *         @return - descriptor containing value of id tag.
        */        
        IMPORT_C const TDesC8& IdTag() const;
        
        /*
        *        Method sets content of "id" tag. Content of this tag usually identifies underlying entry or feed.
        *         @param aId - descriptor that contains value of 'id' tag.
        */        
        IMPORT_C void SetIdTagL( const TDesC8& aId );
        
        /*
        *        Adds a "link" tag to the message. Parameters describe subtags with a corresponding name.
        *         In case of subtag's value of KNullDesC8, it is ommited. Allows adding multiple links.
        *         @param aHref - set the content of 'href' attribute
        *         @param aRel - set the content of 'rel' attribute 
        *         @param aType - set the content of 'type' attribute 
        *         @param aHreflang - set the content of 'hrefLang' attribute
        *         @param aTitle - set the content of 'title' attribute
        *         @param aLenght - set the content of 'length' attribute
        * 
        */
        IMPORT_C void AddLinkL( const TDesC8& aHref, const TDesC8& aRel, const TDesC8& aType, const TDesC8& aHreflang, const TDesC8& aTitle, const TDesC8& aLength);
        
        /*
        *        Removes "link" tag. 
        *       @param aLenght - it's an order of a link in an xml document(or element) containing atom message. 
        */
        IMPORT_C void RemoveLinkL( TInt aIndex);
        
        /*
        *        Removes first "link" tag from the message with a "href" described with @href parameter (href is a must be in a link)
        *         @param aHref - href of a link to delete.
        */
        IMPORT_C void RemoveLinkL(const TDesC8& aHref);
        
        /*
        *        Returns a total count of "link" tags in the message.
        *         @return - count of links
        */
        IMPORT_C TInt LinkCountL();
        
        /*
        *        Returns a total count of "category" tags in the entry.
        *         @return - count of category
        */
        IMPORT_C TInt CategoryCountL();
        
        /*
        *        Method adds a "category" tag to the entry. Parameters describe subtags with a corresponding name. If param is KNullDesC8
        *         it is ommited.
        *         @param aTerm - content of  'term' tag
        *         @param aScheme - content of  'scheme' tag
        *         @param aLabel- content of  'label' tag
        */
        IMPORT_C void AddCategoryL( const TDesC8& aTerm, const TDesC8& aScheme, const TDesC8& aLabel);
        
        /*
        *        Removes first "category" tag from the entry with a "term" described with @term parameter.
        *         @param aTerm - value of 'term' tag of category to remove
        */
        IMPORT_C void RemoveCategoryL(const TDesC8& aTerm);   
        
        /*
        *        Removes all "author" tags from the entry, as there is only one author tag allowed, 
        */
        IMPORT_C void RemoveAuthorL();
        
        /*
        *        Sets "author" tag to the entry. Parameters describe subtags with a corresponding name.
        *         In case of subtag's value of KNullDesC8, it is ommited. Allows adding multiple authors.
        *         @param aName - author's name 
        *         @param aUri - author's uri 
        *         @param aEmail - author's email 
        */        
        IMPORT_C void SetAuthorL( const TDesC8& aName, const TDesC8& aUri, const TDesC8& aEmail);
        
        /*
        *        Removes first "contributor" tag from the entry with 'name' specified by @name .
        *         @param aName - name of a contributor to remove 
        */
        IMPORT_C void RemoveContributorL( const TDesC8& aName);
        
        /*
        *        Adds "contributor" tag to the entry. Parameters describe subtags with a corresponding name.
        *         In case of subtag's value of KNullDesC8, it is ommited. 
        *        @param aName - contributor's name 
        *         @param aUri - contributor's uri 
        *         @param aEmail - - contributor's email 
        */
        IMPORT_C void AddContributorL( const TDesC8& aName, const TDesC8& aUri, const TDesC8& aEmail);
        
        /*
        *        Returns a total count of "contributor" tags in the entry.
        *         @return - count of contributor's
        */
        IMPORT_C TInt ContributorCountL();
        
        /*
        *        @return content of the "righst" tag.
        */
        IMPORT_C const TDesC8& RightsL();
        
        /*
        *        Method sets content of "rights" tag
        *         @param aRights - value of a "rights" tag
        */
        IMPORT_C void SetRightsL( const TDesC8& aRights);
        
        /*
        *        This method return count of any tag with a name specified by @name and namespace specified by @nameSpaceURI
        *         @param aName - value of a "name" tag
        *         @param aNameSpaceURI - value of a "NameSpaceURI" tag
        */
        IMPORT_C TInt AttributeCountL(const TDesC8& aName, const TDesC8& aNameSpaceURI);
        
        /*
        *        This method allows any tag removal. It removes first tag within the entry with parameters specified. 
        *         Paramteres are:
        *             @aAtomtag - name of tag to remove
        *             @aNameSpaceUri - tag to remove namespace
        *             @aAttributeName - name attribute specifing a tag
        *             @aValue - value of attribute specifing a tag
        *     for example:
        *         removes a tag 
        *        <atomtag xmlns=nameSpaceURI attributeName=value>  ... </atomtag>     
        */
        IMPORT_C void RemoveByAttributeL(const TDesC8& aAtomtag, const TDesC8& aNameSpaceUri, const TDesC8& aAttributeName, const TDesC8& aValue);
        
        /*
        *        This method allows any tag removal. It removes first tag within the entry with parameters specified. 
        *         Paramteres are:
        *             @aAtomtag - name of tag to remove
        *             @aNameSpaceUri - tag to remove namespace
        *             @aSubTagName - name attribute specifing a tag
        *             @aValue - value of attribute specifing a tag
        *     for example:
        *         removes a tag 
        *        
        *         <atomtag xmlns=nameSpaceURI>
        *                 ..
        *                 <subtagName>value</subtagName>
        *          </atomtag>     
        */
        IMPORT_C void RemoveBySubtagL(const TDesC8& aAtomtag, const TDesC8& aNameSpaceUri, const TDesC8& aSubTagName, const TDesC8& aValue);
        

    protected:

        /**
         *  Constructor.
         */
        CSenAtomMessage();
        
    protected: // Data
        TInt64 iId;
        
        // part of content information is contained as hbufs
        HBufC8* iTitle;
        HBufC8* iIdTag;
        HBufC8* iUpdated;
        HBufC8* iPublished;
    
    };

#endif /*CSENATOMMESSAGE_H_*/
