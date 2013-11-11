#
# Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
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

# It has the following dependencies on its invoking makefile
#
# EPOCROOT
#
PROGRAM=SVGTBinEncode.exe
XERCES_DLL=xerces-c_2_8.dll
#SYMDEFCPP_DLL=symdefcppdll.dll
PARENT_DIR=\sf\mw\uitools\svgtencoder\svgtenc\standalone\group

SOURCES_CPP=..\Internal\xmlReader\src\MyRXMLReader.cpp \
	 ..\Internal\Sax2Print\SAX2PrintHandlers.cpp \
	 ..\Internal\SVGEngine\src\SVGEncoderMain.cpp \
	 ..\Internal\SVGEngine\src\SVGEngineImpl.cpp \
	 ..\Internal\SVGEngine\src\SVGTEncodeInterfaceImpl.cpp \
	 ..\Internal\SVGEngine\src\SVGEvent.cpp \
	 ..\Internal\SVGEngine\src\SVGContentHandler.cpp \
	 ..\Internal\SVGEngine\src\SVGAttributeVerifier.cpp \
	 ..\Internal\SVGEngine\src\SVGErrorImpl.cpp \
	 ..\Internal\SVGEngine\src\SVGEncoder.cpp \
	 ..\Internal\svgImpl\src\SVGElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGDocumentImpl.cpp \
	 ..\Internal\svgImpl\src\SVGDOMImplementationImpl.cpp \
	 ..\Internal\svgImpl\src\SVGSolidColorElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGDiscardElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGMediaAnimationElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGMediaElementBase.cpp \
	 ..\Internal\svgImpl\src\SVGAudioElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGSvgElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGGElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGUseElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGStyleElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGAElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGUriReferenceImpl.cpp \
	 ..\Internal\svgImpl\src\SVGLineElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGRectElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGCircleElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGEllipseElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGPolylineElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGPathElementImpl.cpp \
	 ..\Internal\svgimpl\src\SVGMpathElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGTextElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGImageElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGFontElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGFontFaceElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGGlyphElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGMissingGlyphElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGHkernElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGAnimationBase.cpp \
	 ..\Internal\svgImpl\src\SVGSetElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGAnimateElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGAnimateTransformElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGAnimateMotionElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGLinearGradientElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGGradientElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGRadialGradientElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGStopElementImpl.cpp \
	 ..\Internal\svgImpl\src\SVGClrCssValueImpl.cpp \
	 ..\Internal\svgImpl\src\SVGPaintCssValueImpl.cpp \
	 ..\Internal\svgImpl\src\SVGIntCssValueImpl.cpp \
	 ..\Internal\svgImpl\src\SVGFloatCssValueImpl.cpp \
	 ..\Internal\svgImpl\src\SVGStrCssValueImpl.cpp \
	 ..\Internal\svgImpl\src\SVGVectorCssValueImpl.cpp \
	 ..\Internal\svgImpl\src\SVGTransformListImpl.cpp \
	 ..\Internal\svgImpl\src\SVGTransformableImpl.cpp \
	 ..\Internal\svgImpl\src\SVGTestsImpl.cpp \
	 ..\Internal\svgImpl\src\SVGLangSpaceImpl.cpp \
	 ..\Internal\svgImpl\src\SVGFitToViewBoxImpl.cpp \
	 ..\Internal\svgImpl\src\SVGPreserveAspectRatioImpl.cpp \
	 ..\Internal\svgImpl\src\SVGPointLexer.cpp \
	 ..\Internal\svgImpl\src\SVGSchemaData.cpp \
	 ..\Internal\svgImpl\src\SVGPathDataParser.cpp \
	 ..\Internal\svgImpl\src\SVGAnimTimeController.cpp \
	 ..\Internal\svgImpl\src\SVGStringTokenizer.cpp \
	 ..\Internal\svgImpl\src\SVGRelToAbsPath.cpp \
	 ..\Internal\svgImpl\src\SVGAnimTimingParser.cpp \
	 ..\Internal\svgImpl\src\SVGFontMap.cpp \
	 ..\Internal\svgImpl\src\SVGColor.cpp \
	 ..\Internal\XMLDOMIMPL\src\SVGXmlElementImpl.cpp \
	 ..\Internal\gfx2d\src\GfxFloatFixPt.cpp \
	 ..\Internal\gfx2d\src\Gfxtrignometric.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxPoint2D.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxAffineTransform.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxGeneralPath.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxGeneralPathIteratorP.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxFlatteningPathIterator.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxRectangle2D.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxRectangularShape.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxRectangleIteratorP.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxLine2D.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxLineIteratorP.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxEllipse2D.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxEllipseIteratorP.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxRoundRectangle2D.cpp \
	 ..\Internal\gfx2d\src\GfxGeom\GfxRoundRectangleIteratorP.cpp \
	 ..\Internal\gfx2d\src\GfxGc\GfxColor.cpp \
	 ..\Internal\gfx2d\src\GfxGc\GfxGradientPaint.cpp \
	 ..\Internal\testingsymdefcppdll\Buf.cpp \
	 ..\Internal\testingsymdefcppdll\Des16.cpp \
	 ..\Internal\testingsymdefcppdll\Des8.cpp \
	 ..\Internal\testingsymdefcppdll\Desc16.cpp \
	 ..\Internal\testingsymdefcppdll\Desc8.cpp \
	 ..\Internal\testingsymdefcppdll\Hbufc16.cpp \
	 ..\Internal\testingsymdefcppdll\Hbufc8.cpp \
	 ..\Internal\testingsymdefcppdll\Rfile.cpp \
	 ..\Internal\testingsymdefcppdll\Rpointerarraybase.cpp \
	 ..\Internal\testingsymdefcppdll\Tbufbase16.cpp \
	 ..\Internal\testingsymdefcppdll\Tbufbase8.cpp \
	 ..\Internal\testingsymdefcppdll\TBufC.cpp \
	 ..\Internal\testingsymdefcppdll\Tbufcbase16.cpp \
	 ..\Internal\testingsymdefcppdll\Tbufcbase8.cpp \
	 ..\Internal\testingsymdefcppdll\Tchar.cpp \
	 ..\Internal\testingsymdefcppdll\TInt64.cpp \
	 ..\Internal\testingsymdefcppdll\Tkeydesc8array.cpp \
	 ..\Internal\testingsymdefcppdll\Tlex.cpp \
	 ..\Internal\testingsymdefcppdll\Tptr16.cpp \
	 ..\Internal\testingsymdefcppdll\Tptr8.cpp \
	 ..\Internal\testingsymdefcppdll\Tptrc16.cpp \
	 ..\Internal\testingsymdefcppdll\Tptrc8.cpp \
	 ..\Internal\testingsymdefcppdll\Trgb.cpp \
	 ..\Internal\testingsymdefcppdll\utf.cpp \
	 ..\Internal\Nvg\src\NVGCircleElement.cpp \
	 ..\Internal\Nvg\src\NvgCommandSection.cpp \
	 ..\Internal\Nvg\src\NvgDefElement.cpp \
	 ..\Internal\Nvg\src\NVGElement.cpp \
	 ..\Internal\Nvg\src\NVGElementFactory.cpp \
	 ..\Internal\Nvg\src\NVGEllipseElement.cpp \
	 ..\Internal\Nvg\src\NVGEncoder.cpp \
	 ..\Internal\Nvg\src\NvgHeaderSection.cpp \
	 ..\Internal\Nvg\src\NvgOffsetVector.cpp \
	 ..\Internal\Nvg\src\NvgPaintSection.cpp \
	 ..\Internal\Nvg\src\NvgPathSection.cpp \
	 ..\Internal\Nvg\src\NVGPolygonElement.cpp \
	 ..\Internal\Nvg\src\NVGRectElement.cpp \
	 ..\Internal\Nvg\src\NVGSVGElement.cpp \
	 ..\Internal\Nvg\src\NVGPathElement.cpp \
	 ..\Internal\Nvg\src\NVGLinearGradientElement.cpp \
	 ..\Internal\Nvg\src\NVGRadialGradientElement.cpp \
	 ..\Internal\Nvg\src\NVGStopElement.cpp \
	 ..\Internal\Nvg\src\NVGLineElement.cpp \
	 ..\Internal\Nvg\src\NVGGElement.cpp \
	 

SOURCES_C=..\Internal\gfx2d\VGRasterizer\src\svgapi.c \
	 ..\Internal\gfx2d\VGRasterizer\src\svgmath.c \
	 ..\Internal\gfx2d\VGRasterizer\src\svgpath.c \
	 ..\Internal\gfx2d\VGRasterizer\src\svgsubdivide.c 

OBJECTS_CPP_DEB=$(SOURCES_CPP:.cpp=.dbg.o)
OBJECTS_C_DEB=$(SOURCES_C:.c=.dbg.o)
OBJECTS_CPP_REL=$(SOURCES_CPP:.cpp=.o)
OBJECTS_C_REL=$(SOURCES_C:.c=.o)


#STLPORT_INC=C:\PROGRA~1\MICROS~3\VC98\Include\stlport

INCLUDES=/I"$(PARENT_DIR)" \
	  /I"$(PARENT_DIR)\inc" \
	  /I"$(EPOCROOT)epoc32\include" \
	  /I"$(STLPORT_INC)"

VPATH=$(PARENT_DIR)
CC=cl.exe
LINK=link.exe
LINK32_FLAGS= ..\lib_win\xerces-c_2.lib
DEFINES = /nologo /MT /GX /I "..\Internal\SVGEngine\inc" /I "..\Internal\Gfx2D\inc" /I "..\Internal\Gfx2D\VGRasterizer\inc" /I "..\Internal\Xmldomimpl\inc" /I "..\Internal\SVGImpl\inc\SVGDOM" /I "..\Internal\SVGImpl\inc" /I "..\Internal\Path\inc" /I "..\Internal\xmlReader\inc" /I "..\Internal\Sax2Print" /I "..\Internal\Nvg\inc" /D ARM /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
CLFLAGS = /Od /nologo /GF /W4 /Fd -GX

$(PROGRAM) : $(OBJECTS_CPP) $(OBJECTS_C)
#	echo $(SOURCES_CPP)
#	echo $(SOURCES_C)
	@chdir $(PARENT_DIR) && $(LINK) /OUT:"$(PROGRAM)" $(OBJECTS_CPP) $(OBJECTS_C) $(LINK32_FLAGS)

DEB : $(OBJECTS_CPP_DEB) $(OBJECTS_C_DEB)
	echo "DEB build Called"
#	echo $(SOURCES_CPP)
#	echo $(SOURCES_C)
	@chdir $(PARENT_DIR) && $(LINK) /OUT:"$(PROGRAM).dbg" $(OBJECTS_CPP_DEB) $(OBJECTS_C_DEB) $(LINK32_FLAGS)


REL : $(OBJECTS_CPP_REL) $(OBJECTS_C_REL)
	echo "REL build Called"
#	echo $(SOURCES_CPP)
#	echo $(SOURCES_C)
	@chdir $(PARENT_DIR) && $(LINK) /OUT:"$(PROGRAM).rel" $(OBJECTS_CPP_REL) $(OBJECTS_C_REL) $(LINK32_FLAGS)

%.o : %.cpp 
	@chdir $(PARENT_DIR) && $(CC) $(DEFINES) $(CLFLAGS) $(INCLUDES) /Fo"$@" /c $<

%.dbg.o : %.cpp 
	@chdir $(PARENT_DIR) && $(CC) $(DEFINES) $(CLFLAGS) /Zi $(INCLUDES) /Fo"$@" /c $<
 
%.o : %.c 
	@chdir $(PARENT_DIR) && $(CC) $(DEFINES) $(CLFLAGS) $(INCLUDES) /Fo"$@" /c $<

%.dbg.o : %.c 
	@chdir $(PARENT_DIR) && $(CC) $(DEFINES) $(CLFLAGS) /Zi $(INCLUDES) /Fo"$@" /c $<

DEB_install: $(PROGRAM).dbg
#	echo $(PARENT_DIR)
#	echo $(BUILD_DIR)
#	echo $(CURDIR)
	copy /y "$(PARENT_DIR)\$(PROGRAM).dbg" "\epoc32\RELEASE\tools\deb\$(PROGRAM)"
	copy /y "\sf\mw\uitools\svgtencoder\svgtenc\standalone\lib_win\$(XERCES_DLL)" "\epoc32\RELEASE\tools\deb\$(XERCES_DLL)"
REL_install: $(PROGRAM).rel
#	echo $(PARENT_DIR)
#	echo $(BUILD_DIR)
#	echo $(CURDIR)
	copy /y "$(PARENT_DIR)\$(PROGRAM).rel" "\epoc32\tools\$(PROGRAM)"
	copy /y "\sf\mw\uitools\svgtencoder\svgtenc\standalone\lib_win\$(XERCES_DLL)" "\epoc32\tools\$(XERCES_DLL)"
DEB_CLEAN:
#	echo $(CURDIR)
	@chdir $(PARENT_DIR) && erase /f $(OBJECTS_CPP_DEB) 2>>nul
	@chdir $(PARENT_DIR) && erase /f $(OBJECTS_C_DEB) 2>>nul
	@chdir $(PARENT_DIR) && erase /f $(PROGRAM).dbg 2>>nul
	@erase /f "\epoc32\RELEASE\tools\deb\$(PROGRAM)" 2>>nul
	@erase /f "\epoc32\RELEASE\tools\deb\$(XERCES_DLL)" 2>>nul

REL_clean:
#	echo $(CURDIR)
	@chdir $(PARENT_DIR) && erase /f $(OBJECTS_CPP_REL) 2>>nul
	@chdir $(PARENT_DIR) && erase /f $(OBJECTS_C_REL) 2>>nul
	@chdir $(PARENT_DIR) && erase /f $(PROGRAM).rel 2>>nul
	@erase /f "\epoc32\tools\$(PROGRAM)" 2>>nul
	@erase /f "\epoc32\tools\$(XERCES_DLL)" 2>>nul

_clean: 
