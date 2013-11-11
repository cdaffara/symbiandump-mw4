#include <QDebug>
#include "smfSns.h"
#include <QString>
#include "smfUserProfile.h"
#include "smfSocialProfile.h"
#include "dsm.h"


int main(){

    QString sns_name = "facebook";
    QString sns_url = "http://www.facebook.com";
    SMFSocialNetworkingSite mySns (sns_name, sns_url);

    qDebug() << "Name of my SNS: " << mySns.snsName();
    qDebug() << "URL of my SNS: " << mySns.snsURL();
    qDebug() << "ID of my SNS(should be negative): " << mySns.snsID();

    QString user_name = "Test User";
    QString user_contact_id = "1234567890";
    SMFUserProfile myUser(user_name, user_contact_id);

    qDebug() << "Name of my user: " << myUser.name();
    qDebug() << "Contact ID of my user: " << myUser.contactID();
    qDebug() << "User ID of my user: " << myUser.userID();

    DataStoreManager* dsm_p = DataStoreManager::getDataStoreManager();
    if(dsm_p != NULL){
        dsm_p->addUserProfile(myUser);
        dsm_p->addSNSEntry(mySns);
       // dsm_p->deleteUserProfile(myUser);
        qDebug() << "----------------\nState: " << dsm_p->getState();
        qDebug() << "Error Message: " << dsm_p->getError() << "\n----------------";
    }
    else
        qDebug() << "Error getting database handle";

    qDebug() << "Name of my user: " << myUser.name();
    qDebug() << "Contact ID of my user: " << myUser.contactID();
    qDebug() << "User ID of my user: " << myUser.userID();

    qDebug() << "Name of my SNS: " << mySns.snsName();
    qDebug() << "URL of my SNS: " << mySns.snsURL();
    qDebug() << "ID of my SNS(should be negative): " << mySns.snsID();

    QString profile_url = sns_url + "/test_user";
    QString screen_alias = "scrnALIAS";
    SMFSocialProfile mySocPrfl(mySns,profile_url, screen_alias);

    qDebug() << "My Social Profile's associated SNS ID: " << mySocPrfl.associatedSnsID();
    qDebug() << "My social Profile's associated User ID: " << mySocPrfl.associatedUserID();
    qDebug() << "ProfileURL: " << mySocPrfl.profileURL();
    qDebug() << "Screen Alias: " << mySocPrfl.screenAlias();
    qDebug() << "My Social Profile's ID: " << mySocPrfl.profileID();

    DataStoreManager* dsm2 = DataStoreManager::getDataStoreManager(); // jsut to test that only one object is created.
    if (dsm_p == dsm2)
        qDebug() << "true";

    if(dsm2 != NULL){
        dsm2->createRelation(myUser, mySocPrfl);
        dsm2->addSocialProfile(mySocPrfl);

        //dsm2->deleteSocialProfile(mySocPrfl);
    }

    qDebug() << "My Social Profile's associated SNS ID: " << mySocPrfl.associatedSnsID();
    qDebug() << "My social Profile's associated User ID: " << mySocPrfl.associatedUserID();
    qDebug() << "ProfileURL: " << mySocPrfl.profileURL();
    qDebug() << "Screen Alias: " << mySocPrfl.screenAlias();
    qDebug() << "My Social Profile's ID: " << mySocPrfl.profileID();

    dsm2->getAllRelated(myUser);
    dsm2->getRelatedByService(myUser,mySns);

    return 0;
}

