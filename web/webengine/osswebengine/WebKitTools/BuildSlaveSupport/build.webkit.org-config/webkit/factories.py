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

from webkit.steps import *
from buildbot.process import factory

s = factory.s

class BuildFactory(factory.BuildFactory):
    useProgress = False
    def __init__(self):
        factory.BuildFactory.__init__(self, [s(CheckOutSource)])

class StandardBuildFactory(BuildFactory):
    def __init__(self):
        BuildFactory.__init__(self)
        self.steps.append(s(SetConfiguration, configuration="release"))
        self.steps.append(s(self.getCompileStep(), configuration="release"))
        self.steps.append(s(self.getJavaScriptCoreTestStep()))
        self.steps.append(s(LayoutTest))
        self.steps.append(s(UploadLayoutResults))
#        self.steps.append(s(UploadDiskImage))

    def getCompileStep(self):
        return CompileWebKit

    def getJavaScriptCoreTestStep(self):
        return JavaScriptCoreTest


class NoSVGBuildFactory(StandardBuildFactory):
    def getCompileStep(self):
        return CompileWebKitNoSVG


class PixelTestBuildFactory(BuildFactory):
    def __init__(self):
        BuildFactory.__init__(self)
        self.steps.append(s(SetConfiguration, configuration="release"))
        self.steps.append(s(CompileWebKit, configuration="release"))
        self.steps.append(s(PixelLayoutTest))
        self.steps.append(s(UploadLayoutResults))

class LeakBuildFactory(BuildFactory):
    def __init__(self):
        BuildFactory.__init__(self)
        self.steps.append(s(SetConfiguration, configuration="debug"))
        self.steps.append(s(CompileWebKit, configuration="debug"))
        self.steps.append(s(JavaScriptCoreTest))
        self.steps.append(s(LeakTest))
        self.steps.append(s(UploadLayoutResults))
#        self.steps.append(s(UploadDiskImage))

class PageLoadTestBuildFactory(BuildFactory):
    def __init__(self):
        BuildFactory.__init__(self)
        self.steps.append(s(CompileWebKit, configuration="release"))
        self.steps.append(s(PageLoadTest))

Win32BuildFactory = StandardBuildFactory

class GtkBuildFactory(StandardBuildFactory):
    def getCompileStep(self):
        return CompileWebKitGtk

    def getJavaScriptCoreTestStep(self):
        return JavaScriptCoreTestGtk

class CoverageDataBuildFactory(BuildFactory):
    def __init__(self):
        BuildFactory.__init__(self)
        self.steps.append(s(GenerateCoverageData))
        self.steps.append(s(UploadCoverageData))
