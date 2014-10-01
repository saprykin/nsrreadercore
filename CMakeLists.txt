cmake_minimum_required (VERSION 2.8.11)
project (nsrreadercore C CXX)

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "Debug")
endif()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

if (NOT NSR_PLATFORM)
	string (TOLOWER ${CMAKE_SYSTEM_NAME} NSR_PLATFORM)
endif()

set (CMAKE_AUTOMOC true)

find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})

set (SOURCES
	nsrabstractdocument.cpp
	nsrcharsetdetector.cpp
	nsrcroppads.cpp
	nsrcroppadscache.cpp
	nsrdjvudocument.cpp
	nsrpagecropper.cpp
	nsrpagescache.cpp
	nsrpopplerdocument.cpp
	nsrreadercore.cpp
	nsrrenderedpage.cpp
	nsrrenderrequest.cpp
	nsrrenderthread.cpp
	nsrsession.cpp
	nsrtextdocument.cpp
	nsrtiffdocument.cpp
)

set (DJVU_SOURCES
	3dparty/djvu/Arrays.cpp
	3dparty/djvu/atomic.cpp
	3dparty/djvu/BSByteStream.cpp
	3dparty/djvu/BSEncodeByteStream.cpp
	3dparty/djvu/ByteStream.cpp
	3dparty/djvu/DataPool.cpp
	3dparty/djvu/ddjvuapi.cpp
	3dparty/djvu/debug.cpp
	3dparty/djvu/DjVmDir.cpp
	3dparty/djvu/DjVmDir0.cpp
	3dparty/djvu/DjVmDoc.cpp
	3dparty/djvu/DjVmNav.cpp
	3dparty/djvu/DjVuAnno.cpp
	3dparty/djvu/DjVuDocEditor.cpp
	3dparty/djvu/DjVuDocument.cpp
	3dparty/djvu/DjVuDumpHelper.cpp
	3dparty/djvu/DjVuErrorList.cpp
	3dparty/djvu/DjVuFile.cpp
	3dparty/djvu/DjVuFileCache.cpp
	3dparty/djvu/DjVuGlobal.cpp
	3dparty/djvu/DjVuGlobalMemory.cpp
	3dparty/djvu/DjVuImage.cpp
	3dparty/djvu/DjVuInfo.cpp
	3dparty/djvu/DjVuMessage.cpp
	3dparty/djvu/DjVuMessageLite.cpp
	3dparty/djvu/DjVuNavDir.cpp
	3dparty/djvu/DjVuPalette.cpp
	3dparty/djvu/DjVuPort.cpp
	3dparty/djvu/DjVuText.cpp
	3dparty/djvu/DjVuToPS.cpp
	3dparty/djvu/GBitmap.cpp
	3dparty/djvu/GContainer.cpp
	3dparty/djvu/GException.cpp
	3dparty/djvu/GIFFManager.cpp
	3dparty/djvu/GMapAreas.cpp
	3dparty/djvu/GOS.cpp
	3dparty/djvu/GPixmap.cpp
	3dparty/djvu/GRect.cpp
	3dparty/djvu/GScaler.cpp
	3dparty/djvu/GSmartPointer.cpp
	3dparty/djvu/GString.cpp
	3dparty/djvu/GThreads.cpp
	3dparty/djvu/GUnicode.cpp
	3dparty/djvu/GURL.cpp
	3dparty/djvu/IFFByteStream.cpp
	3dparty/djvu/IW44EncodeCodec.cpp
	3dparty/djvu/IW44Image.cpp
	3dparty/djvu/JB2EncodeCodec.cpp
	3dparty/djvu/JB2Image.cpp
	3dparty/djvu/JPEGDecoder.cpp
	3dparty/djvu/miniexp.cpp
	3dparty/djvu/MMRDecoder.cpp
	3dparty/djvu/MMX.cpp
	3dparty/djvu/UnicodeByteStream.cpp
	3dparty/djvu/XMLParser.cpp
	3dparty/djvu/XMLTags.cpp
	3dparty/djvu/ZPCodec.cpp
)

set (POPPLER_SOURCES
	3dparty/poppler/fofi/FoFiBase.cc
	3dparty/poppler/fofi/FoFiEncodings.cc
	3dparty/poppler/fofi/FoFiIdentifier.cc
	3dparty/poppler/fofi/FoFiTrueType.cc
	3dparty/poppler/fofi/FoFiType1.cc
	3dparty/poppler/fofi/FoFiType1C.cc
	3dparty/poppler/goo/FixedPoint.cc
	3dparty/poppler/goo/gfile.cc
	3dparty/poppler/goo/gmem.cc
	3dparty/poppler/goo/gmempp.cc
	3dparty/poppler/goo/GooHash.cc
	3dparty/poppler/goo/GooList.cc
	3dparty/poppler/goo/GooString.cc
	3dparty/poppler/goo/GooTimer.cc
	3dparty/poppler/goo/grandom.cc
	3dparty/poppler/goo/gstrtod.cc
	3dparty/poppler/goo/ImgWriter.cc
	3dparty/poppler/goo/JpegWriter.cc
	3dparty/poppler/goo/PNGWriter.cc
	3dparty/poppler/goo/TiffWriter.cc
	3dparty/poppler/poppler/Annot.cc
	3dparty/poppler/poppler/Array.cc
	3dparty/poppler/poppler/BuiltinFont.cc
	3dparty/poppler/poppler/BuiltinFontTables.cc
	3dparty/poppler/poppler/CachedFile.cc
	3dparty/poppler/poppler/Catalog.cc
	3dparty/poppler/poppler/CharCodeToUnicode.cc
	3dparty/poppler/poppler/CMap.cc
	3dparty/poppler/poppler/CurlCachedFile.cc
	3dparty/poppler/poppler/CurlPDFDocBuilder.cc
	3dparty/poppler/poppler/DateInfo.cc
	3dparty/poppler/poppler/DCTStream.cc
	3dparty/poppler/poppler/Decrypt.cc
	3dparty/poppler/poppler/Dict.cc
	3dparty/poppler/poppler/Error.cc
	3dparty/poppler/poppler/FileSpec.cc
	3dparty/poppler/poppler/FlateStream.cc
	3dparty/poppler/poppler/FontEncodingTables.cc
	3dparty/poppler/poppler/FontInfo.cc
	3dparty/poppler/poppler/Form.cc
	3dparty/poppler/poppler/Function.cc
	3dparty/poppler/poppler/Gfx.cc
	3dparty/poppler/poppler/GfxFont.cc
	3dparty/poppler/poppler/GfxState.cc
	3dparty/poppler/poppler/GlobalParams.cc
	3dparty/poppler/poppler/Hints.cc
	3dparty/poppler/poppler/JArithmeticDecoder.cc
	3dparty/poppler/poppler/JBIG2Stream.cc
	3dparty/poppler/poppler/JPXStream.cc
	3dparty/poppler/poppler/Lexer.cc
	3dparty/poppler/poppler/Linearization.cc
	3dparty/poppler/poppler/Link.cc
	3dparty/poppler/poppler/LocalPDFDocBuilder.cc
	3dparty/poppler/poppler/Movie.cc
	3dparty/poppler/poppler/NameToCharCode.cc
	3dparty/poppler/poppler/Object.cc
	3dparty/poppler/poppler/OptionalContent.cc
	3dparty/poppler/poppler/Outline.cc
	3dparty/poppler/poppler/OutputDev.cc
	3dparty/poppler/poppler/Page.cc
	3dparty/poppler/poppler/PageLabelInfo.cc
	3dparty/poppler/poppler/PageTransition.cc
	3dparty/poppler/poppler/Parser.cc
	3dparty/poppler/poppler/PDFDoc.cc
	3dparty/poppler/poppler/PDFDocEncoding.cc
	3dparty/poppler/poppler/PDFDocFactory.cc
	3dparty/poppler/poppler/PopplerCache.cc
	3dparty/poppler/poppler/PreScanOutputDev.cc
	3dparty/poppler/poppler/ProfileData.cc
	3dparty/poppler/poppler/PSOutputDev.cc
	3dparty/poppler/poppler/PSTokenizer.cc
	3dparty/poppler/poppler/Rendition.cc
	3dparty/poppler/poppler/SecurityHandler.cc
	3dparty/poppler/poppler/Sound.cc
	3dparty/poppler/poppler/SplashOutputDev.cc
	3dparty/poppler/poppler/StdinCachedFile.cc
	3dparty/poppler/poppler/StdinPDFDocBuilder.cc
	3dparty/poppler/poppler/Stream.cc
	3dparty/poppler/poppler/TextOutputDev.cc
	3dparty/poppler/poppler/UnicodeMap.cc
	3dparty/poppler/poppler/UnicodeTypeTable.cc
	3dparty/poppler/poppler/UTF.cc
	3dparty/poppler/poppler/ViewerPreferences.cc
	3dparty/poppler/poppler/XpdfPluginAPI.cc
	3dparty/poppler/poppler/XRef.cc
	3dparty/poppler/splash/Splash.cc
	3dparty/poppler/splash/SplashBitmap.cc
	3dparty/poppler/splash/SplashClip.cc
	3dparty/poppler/splash/SplashFont.cc
	3dparty/poppler/splash/SplashFontEngine.cc
	3dparty/poppler/splash/SplashFontFile.cc
	3dparty/poppler/splash/SplashFontFileID.cc
	3dparty/poppler/splash/SplashFTFont.cc
	3dparty/poppler/splash/SplashFTFontEngine.cc
	3dparty/poppler/splash/SplashFTFontFile.cc
	3dparty/poppler/splash/SplashPath.cc
	3dparty/poppler/splash/SplashPattern.cc
	3dparty/poppler/splash/SplashScreen.cc
	3dparty/poppler/splash/SplashState.cc
	3dparty/poppler/splash/SplashT1Font.cc
	3dparty/poppler/splash/SplashT1FontEngine.cc
	3dparty/poppler/splash/SplashT1FontFile.cc
	3dparty/poppler/splash/SplashXPath.cc
	3dparty/poppler/splash/SplashXPathScanner.cc
)

set (TIFF_SOURCES
	3dparty/tiff/tif_aux.c
	3dparty/tiff/tif_close.c
	3dparty/tiff/tif_codec.c
	3dparty/tiff/tif_color.c
	3dparty/tiff/tif_compress.c
	3dparty/tiff/tif_dir.c
	3dparty/tiff/tif_dirinfo.c
	3dparty/tiff/tif_dirread.c
	3dparty/tiff/tif_dirwrite.c
	3dparty/tiff/tif_dumpmode.c
	3dparty/tiff/tif_error.c
	3dparty/tiff/tif_extension.c
	3dparty/tiff/tif_fax3.c
	3dparty/tiff/tif_fax3sm.c
	3dparty/tiff/tif_flush.c
	3dparty/tiff/tif_getimage.c
	3dparty/tiff/tif_jbig.c
	3dparty/tiff/tif_jpeg_12.c
	3dparty/tiff/tif_jpeg.c
	3dparty/tiff/tif_luv.c
	3dparty/tiff/tif_lzma.c
	3dparty/tiff/tif_lzw.c
	3dparty/tiff/tif_next.c
	3dparty/tiff/tif_ojpeg.c
	3dparty/tiff/tif_open.c
	3dparty/tiff/tif_packbits.c
	3dparty/tiff/tif_pixarlog.c
	3dparty/tiff/tif_predict.c
	3dparty/tiff/tif_print.c
	3dparty/tiff/tif_read.c
	3dparty/tiff/tif_stream.cxx
	3dparty/tiff/tif_strip.c
	3dparty/tiff/tif_swab.c
	3dparty/tiff/tif_thunder.c
	3dparty/tiff/tif_tile.c
	3dparty/tiff/tif_unix.c
	3dparty/tiff/tif_version.c
	3dparty/tiff/tif_warning.c
	3dparty/tiff/tif_write.c
	3dparty/tiff/tif_zip.c
)

set (UNIVERSALCHARDET_SOURCES
	3dparty/universalchardet/CharDistribution.cpp
	3dparty/universalchardet/JpCntx.cpp
	3dparty/universalchardet/LangBulgarianModel.cpp
	3dparty/universalchardet/LangCyrillicModel.cpp
	3dparty/universalchardet/LangGreekModel.cpp
	3dparty/universalchardet/LangHebrewModel.cpp
	3dparty/universalchardet/LangHungarianModel.cpp
	3dparty/universalchardet/LangThaiModel.cpp
	3dparty/universalchardet/nsBig5Prober.cpp
	3dparty/universalchardet/nsCharSetProber.cpp
	3dparty/universalchardet/nsEscCharsetProber.cpp
	3dparty/universalchardet/nsEscSM.cpp
	3dparty/universalchardet/nsEUCJPProber.cpp
	3dparty/universalchardet/nsEUCKRProber.cpp
	3dparty/universalchardet/nsEUCTWProber.cpp
	3dparty/universalchardet/nsGB2312Prober.cpp
	3dparty/universalchardet/nsHebrewProber.cpp
	3dparty/universalchardet/nsLatin1Prober.cpp
	3dparty/universalchardet/nsMBCSGroupProber.cpp
	3dparty/universalchardet/nsMBCSSM.cpp
	3dparty/universalchardet/nsSBCharSetProber.cpp
	3dparty/universalchardet/nsSBCSGroupProber.cpp
	3dparty/universalchardet/nsSJISProber.cpp
	3dparty/universalchardet/nsUniversalDetector.cpp
	3dparty/universalchardet/nsUTF8Prober.cpp
)

set (HEADERS
	insrsettings.h
	insrthumbnailer.h
	nsrabstractdocument.h
	nsrcroppads.h
	nsrcroppadscache.h
	nsrdjvudocument.h
	nsrpagecropper.h
	nsrpagescache.h
	nsrpopplerdocument.h
	nsrreadercore.h
	nsrreadercore_global.h
	nsrrenderedpage.h
	nsrrenderrequest.h
	nsrrenderthread.h
	nsrsession.h
	nsrtextdocument.h
	nsrtiffdocument.h
)

set (DJVU_HEADERS
	3dparty/djvu/config/${NSR_PLATFORM}/config-djvu.h
	3dparty/djvu/Arrays.h
	3dparty/djvu/atomic.h
	3dparty/djvu/BSByteStream.h
	3dparty/djvu/ByteStream.h
	3dparty/djvu/DataPool.h
	3dparty/djvu/ddjvuapi.h
	3dparty/djvu/debug.h
	3dparty/djvu/DjVmDir.h
	3dparty/djvu/DjVmDir0.h
	3dparty/djvu/DjVmDoc.h
	3dparty/djvu/DjVmNav.h
	3dparty/djvu/DjVuAnno.h
	3dparty/djvu/DjVuDocEditor.h
	3dparty/djvu/DjVuDocument.h
	3dparty/djvu/DjVuDumpHelper.h
	3dparty/djvu/DjVuErrorList.h
	3dparty/djvu/DjVuFile.h
	3dparty/djvu/DjVuFileCache.h
	3dparty/djvu/DjVuGlobal.h
	3dparty/djvu/DjVuImage.h
	3dparty/djvu/DjVuInfo.h
	3dparty/djvu/DjVuMessage.h
	3dparty/djvu/DjVuMessageLite.h
	3dparty/djvu/DjVuNavDir.h
	3dparty/djvu/DjVuPalette.h
	3dparty/djvu/DjVuPort.h
	3dparty/djvu/DjVuText.h
	3dparty/djvu/DjVuToPS.h
	3dparty/djvu/GBitmap.h
	3dparty/djvu/GContainer.h
	3dparty/djvu/GException.h
	3dparty/djvu/GIFFManager.h
	3dparty/djvu/GMapAreas.h
	3dparty/djvu/GOS.h
	3dparty/djvu/GPixmap.h
	3dparty/djvu/GRect.h
	3dparty/djvu/GScaler.h
	3dparty/djvu/GSmartPointer.h
	3dparty/djvu/GString.h
	3dparty/djvu/GThreads.h
	3dparty/djvu/GURL.h
	3dparty/djvu/IFFByteStream.h
	3dparty/djvu/IW44Image.h
	3dparty/djvu/JB2Image.h
	3dparty/djvu/JPEGDecoder.h
	3dparty/djvu/miniexp.h
	3dparty/djvu/MMRDecoder.h
	3dparty/djvu/MMX.h
	3dparty/djvu/Template.h
	3dparty/djvu/UnicodeByteStream.h
	3dparty/djvu/XMLParser.h
	3dparty/djvu/XMLTags.h
	3dparty/djvu/ZPCodec.h
)

set (POPPLER_HEADERS
	3dparty/poppler/config/${NSR_PLATFORM}/config-pdf.h
	3dparty/poppler/fofi/FoFiBase.h
	3dparty/poppler/fofi/FoFiEncodings.h
	3dparty/poppler/fofi/FoFiIdentifier.h
	3dparty/poppler/fofi/FoFiTrueType.h
	3dparty/poppler/fofi/FoFiType1.h
	3dparty/poppler/fofi/FoFiType1C.h
	3dparty/poppler/goo/FixedPoint.h
	3dparty/poppler/goo/gfile.h
	3dparty/poppler/goo/gmem.h
	3dparty/poppler/goo/GooHash.h
	3dparty/poppler/goo/GooLikely.h
	3dparty/poppler/goo/GooList.h
	3dparty/poppler/goo/GooMutex.h
	3dparty/poppler/goo/GooString.h
	3dparty/poppler/goo/GooTimer.h
	3dparty/poppler/goo/grandom.h
	3dparty/poppler/goo/gstrtod.h
	3dparty/poppler/goo/gtypes_p.h
	3dparty/poppler/goo/gtypes.h
	3dparty/poppler/goo/ImgWriter.h
	3dparty/poppler/goo/JpegWriter.h
	3dparty/poppler/goo/PNGWriter.h
	3dparty/poppler/goo/TiffWriter.h
	3dparty/poppler/poppler/Annot.h
	3dparty/poppler/poppler/Array.h
	3dparty/poppler/poppler/BuiltinFont.h
	3dparty/poppler/poppler/BuiltinFontTables.h
	3dparty/poppler/poppler/CachedFile.h
	3dparty/poppler/poppler/Catalog.h
	3dparty/poppler/poppler/CharCodeToUnicode.h
	3dparty/poppler/poppler/CharTypes.h
	3dparty/poppler/poppler/CMap.h
	3dparty/poppler/poppler/CompactFontTables.h
	3dparty/poppler/poppler/CurlCachedFile.h
	3dparty/poppler/poppler/CurlPDFDocBuilder.h
	3dparty/poppler/poppler/DateInfo.h
	3dparty/poppler/poppler/DCTStream.h
	3dparty/poppler/poppler/Decrypt.h
	3dparty/poppler/poppler/Dict.h
	3dparty/poppler/poppler/Error.h
	3dparty/poppler/poppler/ErrorCodes.h
	3dparty/poppler/poppler/FileSpec.h
	3dparty/poppler/poppler/FlateStream.h
	3dparty/poppler/poppler/FontEncodingTables.h
	3dparty/poppler/poppler/FontInfo.h
	3dparty/poppler/poppler/Form.h
	3dparty/poppler/poppler/Function.h
	3dparty/poppler/poppler/Gfx.h
	3dparty/poppler/poppler/GfxFont.h
	3dparty/poppler/poppler/GfxState_helpers.h
	3dparty/poppler/poppler/GfxState.h
	3dparty/poppler/poppler/GlobalParams.h
	3dparty/poppler/poppler/Hints.h
	3dparty/poppler/poppler/JArithmeticDecoder.h
	3dparty/poppler/poppler/JBIG2Stream.h
	3dparty/poppler/poppler/JPXStream.h
	3dparty/poppler/poppler/Lexer.h
	3dparty/poppler/poppler/Linearization.h
	3dparty/poppler/poppler/Link.h
	3dparty/poppler/poppler/LocalPDFDocBuilder.h
	3dparty/poppler/poppler/Movie.h
	3dparty/poppler/poppler/NameToCharCode.h
	3dparty/poppler/poppler/NameToUnicodeTable.h
	3dparty/poppler/poppler/Object.h
	3dparty/poppler/poppler/OptionalContent.h
	3dparty/poppler/poppler/Outline.h
	3dparty/poppler/poppler/OutputDev.h
	3dparty/poppler/poppler/Page.h
	3dparty/poppler/poppler/PageLabelInfo.h
	3dparty/poppler/poppler/PageTransition.h
	3dparty/poppler/poppler/Parser.h
	3dparty/poppler/poppler/PDFDoc.h
	3dparty/poppler/poppler/PDFDocBuilder.h
	3dparty/poppler/poppler/PDFDocEncoding.h
	3dparty/poppler/poppler/PDFDocFactory.h
	3dparty/poppler/poppler/poppler-config.h
	3dparty/poppler/poppler/PopplerCache.h
	3dparty/poppler/poppler/PreScanOutputDev.h
	3dparty/poppler/poppler/ProfileData.h
	3dparty/poppler/poppler/PSOutputDev.h
	3dparty/poppler/poppler/PSTokenizer.h
	3dparty/poppler/poppler/Rendition.h
	3dparty/poppler/poppler/SecurityHandler.h
	3dparty/poppler/poppler/Sound.h
	3dparty/poppler/poppler/SplashOutputDev.h
	3dparty/poppler/poppler/StdinCachedFile.h
	3dparty/poppler/poppler/StdinPDFDocBuilder.h
	3dparty/poppler/poppler/Stream-CCITT.h
	3dparty/poppler/poppler/Stream.h
	3dparty/poppler/poppler/TextOutputDev.h
	3dparty/poppler/poppler/UnicodeCClassTables.h
	3dparty/poppler/poppler/UnicodeCompTables.h
	3dparty/poppler/poppler/UnicodeDecompTables.h
	3dparty/poppler/poppler/UnicodeMap.h
	3dparty/poppler/poppler/UnicodeMapTables.h
	3dparty/poppler/poppler/UnicodeTypeTable.h
	3dparty/poppler/poppler/UTF.h
	3dparty/poppler/poppler/UTF8.h
	3dparty/poppler/poppler/ViewerPreferences.h
	3dparty/poppler/poppler/XpdfPluginAPI.h
	3dparty/poppler/poppler/XRef.h
	3dparty/poppler/splash/Splash.h
	3dparty/poppler/splash/SplashBitmap.h
	3dparty/poppler/splash/SplashClip.h
	3dparty/poppler/splash/SplashErrorCodes.h
	3dparty/poppler/splash/SplashFont.h
	3dparty/poppler/splash/SplashFontEngine.h
	3dparty/poppler/splash/SplashFontFile.h
	3dparty/poppler/splash/SplashFontFileID.h
	3dparty/poppler/splash/SplashFTFont.h
	3dparty/poppler/splash/SplashFTFontEngine.h
	3dparty/poppler/splash/SplashFTFontFile.h
	3dparty/poppler/splash/SplashGlyphBitmap.h
	3dparty/poppler/splash/SplashMath.h
	3dparty/poppler/splash/SplashPath.h
	3dparty/poppler/splash/SplashPattern.h
	3dparty/poppler/splash/SplashScreen.h
	3dparty/poppler/splash/SplashState.h
	3dparty/poppler/splash/SplashT1Font.h
	3dparty/poppler/splash/SplashT1FontEngine.h
	3dparty/poppler/splash/SplashT1FontFile.h
	3dparty/poppler/splash/SplashTypes.h
	3dparty/poppler/splash/SplashXPath.h
	3dparty/poppler/splash/SplashXPathScanner.h
)

set (TIFF_HEADERS
	3dparty/tiff/config/${NSR_PLATFORM}/tif_config.h
	3dparty/tiff/config/${NSR_PLATFORM}/tiffconf.h
	3dparty/tiff/t4.h
	3dparty/tiff/tif_dir.h
	3dparty/tiff/tif_fax3.h
	3dparty/tiff/tif_predict.h
	3dparty/tiff/tiff.h
	3dparty/tiff/tiffio.h
	3dparty/tiff/tiffiop.h
	3dparty/tiff/tiffvers.h
	3dparty/tiff/uvcode.h
)

set (UNIVERSALCHARDET_HEADERS
	3dparty/universalchardet/CharDistribution.h
	3dparty/universalchardet/JpCntx.h
	3dparty/universalchardet/nsBig5Prober.h
	3dparty/universalchardet/nsCharSetProber.h
	3dparty/universalchardet/nsCodingStateMachine.h
	3dparty/universalchardet/nsEscCharsetProber.h
	3dparty/universalchardet/nsEUCJPProber.h
	3dparty/universalchardet/nsEUCKRProber.h
	3dparty/universalchardet/nsEUCTWProber.h
	3dparty/universalchardet/nsGB2312Prober.h
	3dparty/universalchardet/nsHebrewProber.h
	3dparty/universalchardet/nsLatin1Prober.h
	3dparty/universalchardet/nsMBCSGroupProber.h
	3dparty/universalchardet/nsPkgInt.h
	3dparty/universalchardet/nsSBCharSetProber.h
	3dparty/universalchardet/nsSBCSGroupProber.h
	3dparty/universalchardet/nsSJISProber.h
	3dparty/universalchardet/nsUniversalDetector.h
	3dparty/universalchardet/nsUTF8Prober.h
)

include_directories (
	3dparty
	3dparty/djvu/config/${NSR_PLATFORM}
	3dparty/poppler/config/${NSR_PLATFORM}
	3dparty/tiff/config/${NSR_PLATFORM}
	3dparty/poppler
	3dparty/poppler/goo
	3dparty/poppler/poppler
	3dparty/universalchardet
	${PROJECT_SOURCE_DIR}
	${NSR_INCLUDE_DIRECTORIES}
)

add_library (${PROJECT_NAME} SHARED
	${SOURCES}
	${DJVU_SOURCES}
	${POPPLER_SOURCES}
	${TIFF_SOURCES}
	${UNIVERSALCHARDET_SOURCES}
	${HEADERS}
	${DJVU_HEADERS}
	${POPPLER_HEADERS}
	${TIFF_HEADERS}
	${UNIVERSALCHARDET_HEADERS}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions (${NSR_PLATFORM_DEFINES_DEBUG})
else()
	add_definitions (${NSR_PLATFORM_DEFINES_RELEASE})
endif()

set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${NSR_C_FLAGS_DEBUG}")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${NSR_C_FLAGS_RELEASE}")
set (CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${NSR_C_FLAGS_MINSIZEREL}")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${NSR_C_FLAGS_RELWITHDEBINFO}")

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${NSR_CXX_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${NSR_CXX_FLAGS_RELEASE}")
set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${NSR_CXX_FLAGS_MINSIZEREL}")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${NSR_CXX_FLAGS_RELWITHDEBINFO}")

target_link_libraries (${PROJECT_NAME}
	${QT_LIBRARIES}
	png
	jpeg
	curl
	iconv
	fontconfig
	freetype
	z
	bb
)

add_definitions (-DHAVE_CONFIG_H -DNSRREADERCORE_LIBRARY)
