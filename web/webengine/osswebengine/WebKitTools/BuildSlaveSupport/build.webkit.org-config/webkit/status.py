#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  
#
#

from buildbot.status import html, mail, words

web = html.WebStatus(http_port=8010)

allBuildsEmail = mail.MailNotifier(fromaddr="buildbot@webkit.org",
                                    extraRecipients=["mark+webkit-builds@bdash.net.nz"],
                                    sendToInterestedUsers=False)
breakageEmail = mail.MailNotifier(fromaddr="buildbot@webkit.org",
                                  lookup=mail.Domain("webkit.org"),
                                  mode="failing")

IRC = words.IRC(host="irc.freenode.net",
                nick="webkit-build",
                channels=["#webkit-build"],
#                announceAllBuilds=True
                )

def getStatusListeners():
    return [web, allBuildsEmail, breakageEmail, IRC]
