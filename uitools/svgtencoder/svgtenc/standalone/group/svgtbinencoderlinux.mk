PROGRAM=svgtbinencode
OBJS=.
PARENT_DIR=.
SOURCES_CPP=../Internal/Gfx2D/src/GfxFloatFixPt.cpp \
../Internal/Gfx2D/src/Gfxtrignometric.cpp \
../Internal/Gfx2D/src/GfxGc/GfxColor.cpp \
../Internal/Gfx2D/src/GfxGc/GfxGradientPaint.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxAffineTransform.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxEllipse2D.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxEllipseIteratorP.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxFlatteningPathIterator.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxGeneralPath.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxGeneralPathIteratorP.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxLine2D.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxLineIteratorP.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxPoint2D.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxRectangle2D.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxRectangleIteratorP.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxRectangularShape.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxRoundRectangle2D.cpp \
../Internal/Gfx2D/src/GfxGeom/GfxRoundRectangleIteratorP.cpp \
../Internal/Nvg/src/NVGCircleElement.cpp \
../Internal/Nvg/src/NvgCommandSection.cpp \
../Internal/Nvg/src/NVGDefElement.cpp \
../Internal/Nvg/src/NVGElement.cpp \
../Internal/Nvg/src/NVGElementFactory.cpp \
../Internal/Nvg/src/NVGEllipseElement.cpp \
../Internal/Nvg/src/NVGEncoder.cpp \
../Internal/Nvg/src/NVGGElement.cpp \
../Internal/Nvg/src/NvgHeaderSection.cpp \
../Internal/Nvg/src/NVGLinearGradientElement.cpp \
../Internal/Nvg/src/NVGLineElement.cpp \
../Internal/Nvg/src/NvgOffsetVector.cpp \
../Internal/Nvg/src/NvgPaintSection.cpp \
../Internal/Nvg/src/NVGPathElement.cpp \
../Internal/Nvg/src/NvgPathSection.cpp \
../Internal/Nvg/src/NVGPolygonElement.cpp \
../Internal/Nvg/src/NVGRadialGradientElement.cpp \
../Internal/Nvg/src/NVGRectElement.cpp \
../Internal/Nvg/src/NVGStopElement.cpp \
../Internal/Nvg/src/NVGSVGElement.cpp \
../Internal/Sax2Print/SAX2PrintHandlers.cpp \
../Internal/SVGEngine/src/SVGAttributeVerifier.cpp \
../Internal/SVGEngine/src/SVGContentHandler.cpp \
../Internal/SVGEngine/src/Svgencoder.cpp \
../Internal/SVGEngine/src/SVGEncoderMain.cpp \
../Internal/SVGEngine/src/SVGEngineImpl.cpp \
../Internal/SVGEngine/src/SVGErrorImpl.cpp \
../Internal/SVGEngine/src/SVGEvent.cpp \
../Internal/SVGEngine/src/SVGTEncodeInterfaceImpl.cpp \
../Internal/SVGImpl/src/SVGAElementImpl.cpp \
../Internal/SVGImpl/src/SVGAnimateElementImpl.cpp \
../Internal/SVGImpl/src/SVGAnimateMotionElementImpl.cpp \
../Internal/SVGImpl/src/SVGAnimateTransformElementImpl.cpp \
../Internal/SVGImpl/src/SVGAnimationBase.cpp \
../Internal/SVGImpl/src/SVGAnimTimeController.cpp \
../Internal/SVGImpl/src/SVGAnimTimingParser.cpp \
../Internal/SVGImpl/src/SVGAudioElementImpl.cpp \
../Internal/SVGImpl/src/SVGCircleElementImpl.cpp \
../Internal/SVGImpl/src/SVGClrCssValueImpl.cpp \
../Internal/SVGImpl/src/SVGColor.cpp \
../Internal/SVGImpl/src/SVGDiscardElementImpl.cpp \
../Internal/SVGImpl/src/SVGDocumentImpl.cpp \
../Internal/SVGImpl/src/SVGDOMImplementationImpl.cpp \
../Internal/SVGImpl/src/SVGElementImpl.cpp \
../Internal/SVGImpl/src/SVGEllipseElementImpl.cpp \
../Internal/SVGImpl/src/SVGFitToViewBoxImpl.cpp \
../Internal/SVGImpl/src/SVGFloatCssValueImpl.cpp \
../Internal/SVGImpl/src/SVGFontElementImpl.cpp \
../Internal/SVGImpl/src/SVGFontFaceElementImpl.cpp \
../Internal/SVGImpl/src/SVGFontMap.cpp \
../Internal/SVGImpl/src/SVGGElementImpl.cpp \
../Internal/SVGImpl/src/SVGGlyphElementImpl.cpp \
../Internal/SVGImpl/src/SVGGradientElementImpl.cpp \
../Internal/SVGImpl/src/SVGImageElementImpl.cpp \
../Internal/SVGImpl/src/SVGIntCssValueImpl.cpp \
../Internal/SVGImpl/src/SVGLangSpaceImpl.cpp \
../Internal/SVGImpl/src/SVGLinearGradientElementImpl.cpp \
../Internal/SVGImpl/src/SVGLineElementImpl.cpp \
../Internal/SVGImpl/src/SVGMediaAnimationElementImpl.cpp \
../Internal/SVGImpl/src/SVGMediaElementBase.cpp \
../Internal/SVGImpl/src/SVGMissingGlyphElementImpl.cpp \
../Internal/SVGImpl/src/SVGMpathElementImpl.cpp \
../Internal/SVGImpl/src/SVGPaintCssValueImpl.cpp \
../Internal/SVGImpl/src/SVGPathDataParser.cpp \
../Internal/SVGImpl/src/SVGPathElementImpl.cpp \
../Internal/SVGImpl/src/SVGPointLexer.cpp \
../Internal/SVGImpl/src/SVGPolylineElementImpl.cpp \
../Internal/SVGImpl/src/SVGPreserveAspectRatioImpl.cpp \
../Internal/SVGImpl/src/SVGRadialGradientElementImpl.cpp \
../Internal/SVGImpl/src/SVGRectElementImpl.cpp \
../Internal/SVGImpl/src/SVGRelToAbsPath.cpp \
../Internal/SVGImpl/src/SVGSchemaData.cpp \
../Internal/SVGImpl/src/SVGSetElementImpl.cpp \
../Internal/SVGImpl/src/SVGSolidColorElementImpl.cpp \
../Internal/SVGImpl/src/SVGStopElementImpl.cpp \
../Internal/SVGImpl/src/SVGStrCssValueImpl.cpp \
../Internal/SVGImpl/src/SVGStringTokenizer.cpp \
../Internal/SVGImpl/src/SVGStyleElementImpl.cpp \
../Internal/SVGImpl/src/SVGTestsImpl.cpp \
../Internal/SVGImpl/src/SVGTextElementImpl.cpp \
../Internal/SVGImpl/src/SVGTransformListImpl.cpp \
../Internal/SVGImpl/src/SVGTransformableImpl.cpp \
../Internal/SVGImpl/src/SVGUriReferenceImpl.cpp \
../Internal/SVGImpl/src/SVGUseElementImpl.cpp \
../Internal/SVGImpl/src/SVGVectorCssValueImpl.cpp \
../Internal/SVGImpl/src/SvgHkernelementimpl.cpp \
../Internal/SVGImpl/src/Svgsvgelementimpl.cpp \
../Internal/testingsymdefcppdll/Buf.cpp \
../Internal/testingsymdefcppdll/Des16.cpp \
../Internal/testingsymdefcppdll/Des8.cpp \
../Internal/testingsymdefcppdll/Desc16.cpp \
../Internal/testingsymdefcppdll/Desc8.cpp \
../Internal/testingsymdefcppdll/Hbufc16.cpp \
../Internal/testingsymdefcppdll/Hbufc8.cpp \
../Internal/testingsymdefcppdll/Rfile.cpp \
../Internal/testingsymdefcppdll/Rpointerarraybase.cpp \
../Internal/testingsymdefcppdll/Tbufbase16.cpp \
../Internal/testingsymdefcppdll/Tbufbase8.cpp \
../Internal/testingsymdefcppdll/TBufC.cpp \
../Internal/testingsymdefcppdll/Tbufcbase16.cpp \
../Internal/testingsymdefcppdll/Tbufcbase8.cpp \
../Internal/testingsymdefcppdll/Tchar.cpp \
../Internal/testingsymdefcppdll/TInt64.cpp \
../Internal/testingsymdefcppdll/Tkeydesc8array.cpp \
../Internal/testingsymdefcppdll/Tlex.cpp \
../Internal/testingsymdefcppdll/Tptr16.cpp \
../Internal/testingsymdefcppdll/Tptr8.cpp \
../Internal/testingsymdefcppdll/Tptrc16.cpp \
../Internal/testingsymdefcppdll/Tptrc8.cpp \
../Internal/testingsymdefcppdll/Trgb.cpp \
../Internal/testingsymdefcppdll/utf.cpp \
../Internal/Xmldomimpl/src/SVGXmlElementImpl.cpp \
../Internal/xmlReader/src/MyRXMLReader.cpp

SOURCES_C=../Internal/Gfx2D/VGRasterizer/src/svgapi.c \
../Internal/Gfx2D/VGRasterizer/src/svgpath.c \
../Internal/Gfx2D/VGRasterizer/src/svgmath.c \
../Internal/Gfx2D/VGRasterizer/src/svgsubdivide.c


OBJECTS_CPP_DEB=$(SOURCES_CPP:.cpp=.dbg.o)
OBJECTS_C_DEB=$(SOURCES_C:.c=.dbg.o)
OBJECTS_CPP_REL=$(SOURCES_CPP:.cpp=.o)
OBJECTS_C_REL=$(SOURCES_C:.c=.o)

STLPORT_INC=/usr/local/lib
INCLUDES=-I "$(PARENT_DIR)" -I "$(PARENT_DIR)/inc" -I usr/include 
	#  /I"$(EPOCROOT)epoc32/include" /
	 # /I"$(STLPORT_INC)"

VPATH=$(PARENT_DIR)

# Uncomment this, to generate a debug version.
# debug=-g3

CC=g++ -O2 -m32 $(debug)

LINK=ld
LINK32_FLAGS=-L../lib_linux -lxercesenc-c
DEFINES =-I "../Internal/SVGEngine/inc" \
-I "../Internal/Gfx2D/inc" \
-I "../Internal/Gfx2D/VGRasterizer/inc" \
-I "../Internal/Xmldomimpl/inc" \
-I "../Internal/SVGImpl/inc/SVGDOM" \
-I "../Internal/SVGImpl/inc" \
-I "../Internal/xmlReader/inc" \
-I "../Internal/Sax2Print" \
-I "../Internal/Nvg/inc" \
-I "../Internal/Path/inc" -D ARM -D NDEBUG -D _CONSOLE -D _MBCS
#/YX /FD /c
#CLFLAGS = /Od /nologo /GF -W4 /Fd -GX
CLFLAGS =-ansi -umacro -fms-extensions -w -fshort-wchar $(debug)
#/nologo /MT /GX 
$(PROGRAM) : $(OBJECTS_CPP_REL) $(OBJECTS_C_REL) 
#	echo $(SOURCES_CPP)
#	echo $(SOURCES_C)
	$(CC) -lstdc++ -lpthread -o $(PROGRAM) $(OBJECTS_CPP_REL) $(OBJECTS_C_REL) $(LINK32_FLAGS)

#DEB : $(OBJECTS_CPP_DEB) 
	echo "DEB build Called"
#	echo $(SOURCES_CPP)
#	echo $(SOURCES_C)
#	$(LINK) /OUT:"$(PROGRAM).dbg" $(OBJECTS_CPP_DEB) $(LINK32_FLAGS)


REL : $(OBJECTS_CPP_REL) $(OBJECTS_C_REL)
	echo "REL build Called"
#	echo $(SOURCES_CPP)
#	echo $(SOURCES_C)
	$(LINK) /OUT:"$(PROGRAM).rel" $(OBJECTS_CPP_REL) $(OBJECTS_C_REL) $(LINK32_FLAGS)

%.o : %.cpp 
	$(CC) $(DEFINES) $(CLFLAGS) $(INCLUDES) -o "$@" -c $<

%.dbg.o : %.cpp 
	$(CC) $(DEFINES) $(CLFLAGS) $(INCLUDES) -o "$@" -c $<

%.o : %.c 
	$(CC) $(DEFINES) $(CLFLAGS) $(INCLUDES) -o "$@" -c $<

%.dbg.o : %.c 
	$(CC) $(DEFINES) $(CLFLAGS) $(INCLUDES) -o "$@" -c $<

#EB_install: $(PROGRAM).dbg
#	echo $(PARENT_DIR)
#	echo $(BUILD_DIR)
#	echo $(CURDIR)
#	copy /y "$(PARENT_DIR)/$(PROGRAM).dbg" "/epoc32/RELEASE/tools/deb/$(PROGRAM)"
#	copy /y "/S60/mw/graphics/SVGTopt/SVGTEncoder/standalone/lib/$(OBJS)" "/epoc32/RELEASE/tools/deb/$(OBJS)"
#REL_install: $(PROGRAM).rel
#	echo $(PARENT_DIR)
#	echo $(BUILD_DIR)
#	echo $(CURDIR)
#	copy /y "$(PARENT_DIR)/$(PROGRAM).rel" "/epoc32/tools/$(PROGRAM)"
#	copy /y "/S60/mw/graphics/SVGTopt/SVGTEncoder/standalone/lib/$(OBJS)" "/epoc32/tools/$(OBJS)"
#DEB_CLEAN:
#	echo $(CURDIR)
#	@chdir $(PARENT_DIR) && erase /f $(OBJECTS_CPP_DEB) 2>>nul
#	@chdir $(PARENT_DIR) && erase /f $(OBJECTS_C_DEB) 2>>nul
#	@chdir $(PARENT_DIR) && erase /f $(PROGRAM).dbg 2>>nul
#	@erase /f "/epoc32/RELEASE/tools/deb/$(PROGRAM)" 2>>nul
#	@erase /f "/epoc32/RELEASE/tools/deb/$(OBJS)" 2>>nul

#REL_clean:
#	echo $(CURDIR)
#	@chdir $(PARENT_DIR) && erase /f $(OBJECTS_CPP_REL) 2>>nul
#	@chdir $(PARENT_DIR) && erase /f $(OBJECTS_C_REL) 2>>nul
#	@chdir $(PARENT_DIR) && erase /f $(PROGRAM).rel 2>>nul
#	@erase /f "/epoc32/tools/$(PROGRAM)" 2>>nul
#	@erase /f "/epoc32/tools/$(OBJS)" 2>>nul


clean : FORCE
	cd ../; \
	find -name *.o -type f -exec rm {} \;

FORCE:
