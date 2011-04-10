#####################
# FreeImage library
#

# To keep the paths short we go relative to where most of the source files are
LOCAL_PATH := $(call my-dir)/libfreeimage

include $(CLEAR_VARS)

LOCAL_MODULE     := freeimage

LOCAL_C_INCLUDES := libfreeimage \
                    libfreeimage/Metadata \
                    libfreeimage/FreeImageToolkit \
                    libfreeimage/LibJPEG \
                    libfreeimage/LibMNG \
                    libfreeimage/LibPNG \
                    libfreeimage/LibTIFF \
                    libfreeimage/ZLib \
                    libfreeimage/LibOpenJPEG \
                    libfreeimage/OpenEXR \
                    libfreeimage/OpenEXR/Half \
                    libfreeimage/OpenEXR/Iex \
                    libfreeimage/OpenEXR/IlmImf \
                    libfreeimage/OpenEXR/IlmThread \
                    libfreeimage/OpenEXR/Imath \
                    libfreeimage/OpenEXR/Imath

# Get rid of "warning: deprecated conversion from string constant to 'char*'"
LOCAL_CPPFLAGS   += -Wno-write-strings

# This list of files was taken from the file Makefile.srcs from FreeImage package root folder
# Only FreeImage/LibRawLite/*.* and FreeImage/PluginRAW.cpp were removed, due to difficulties
# in building with them. Also, lfind.c was added as the Android NDK r5b has no lfind().
LOCAL_SRC_FILES  := FreeImage/BitmapAccess.cpp \
                    FreeImage/ColorLookup.cpp \
                    FreeImage/FreeImage.cpp \
                    FreeImage/FreeImageC.c \
                    FreeImage/FreeImageIO.cpp \
                    FreeImage/GetType.cpp \
                    FreeImage/MemoryIO.cpp \
                    FreeImage/PixelAccess.cpp \
                    FreeImage/J2KHelper.cpp \
                    FreeImage/Plugin.cpp \
                    FreeImage/PluginBMP.cpp \
                    FreeImage/PluginCUT.cpp \
                    FreeImage/PluginDDS.cpp \
                    FreeImage/PluginEXR.cpp \
                    FreeImage/PluginG3.cpp \
                    FreeImage/PluginGIF.cpp \
                    FreeImage/PluginHDR.cpp \
                    FreeImage/PluginICO.cpp \
                    FreeImage/PluginIFF.cpp \
                    FreeImage/PluginJ2K.cpp \
                    FreeImage/PluginJP2.cpp \
                    FreeImage/PluginJPEG.cpp \
                    FreeImage/PluginKOALA.cpp \
                    FreeImage/PluginMNG.cpp \
                    FreeImage/PluginPCD.cpp \
                    FreeImage/PluginPCX.cpp \
                    FreeImage/PluginPFM.cpp \
                    FreeImage/PluginPICT.cpp \
                    FreeImage/PluginPNG.cpp \
                    FreeImage/PluginPNM.cpp \
                    FreeImage/PluginPSD.cpp \
                    FreeImage/PluginRAS.cpp \
                    FreeImage/PluginSGI.cpp \
                    FreeImage/PluginTARGA.cpp \
                    FreeImage/PluginTIFF.cpp \
                    FreeImage/PluginWBMP.cpp \
                    FreeImage/PluginXBM.cpp \
                    FreeImage/PluginXPM.cpp \
                    FreeImage/PSDParser.cpp \
                    FreeImage/TIFFLogLuv.cpp \
                    FreeImage/Conversion.cpp \
                    FreeImage/Conversion16_555.cpp \
                    FreeImage/Conversion16_565.cpp \
                    FreeImage/Conversion24.cpp \
                    FreeImage/Conversion32.cpp \
                    FreeImage/Conversion4.cpp \
                    FreeImage/Conversion8.cpp \
                    FreeImage/ConversionFloat.cpp \
                    FreeImage/ConversionRGBF.cpp \
                    FreeImage/ConversionType.cpp \
                    FreeImage/ConversionUINT16.cpp \
                    FreeImage/Halftoning.cpp \
                    FreeImage/tmoColorConvert.cpp \
                    FreeImage/tmoDrago03.cpp \
                    FreeImage/tmoFattal02.cpp \
                    FreeImage/tmoReinhard05.cpp \
                    FreeImage/ToneMapping.cpp \
                    FreeImage/NNQuantizer.cpp \
                    FreeImage/WuQuantizer.cpp \
                    DeprecationManager/Deprecated.cpp \
                    DeprecationManager/DeprecationMgr.cpp \
                    FreeImage/CacheFile.cpp \
                    FreeImage/MultiPage.cpp \
                    FreeImage/ZLibInterface.cpp \
                    Metadata/Exif.cpp \
                    Metadata/FIRational.cpp \
                    Metadata/FreeImageTag.cpp \
                    Metadata/IPTC.cpp \
                    Metadata/TagConversion.cpp \
                    Metadata/TagLib.cpp \
                    Metadata/XTIFF.cpp \
                    FreeImageToolkit/Background.cpp \
                    FreeImageToolkit/BSplineRotate.cpp \
                    FreeImageToolkit/Channels.cpp \
                    FreeImageToolkit/ClassicRotate.cpp \
                    FreeImageToolkit/Colors.cpp \
                    FreeImageToolkit/CopyPaste.cpp \
                    FreeImageToolkit/Display.cpp \
                    FreeImageToolkit/Flip.cpp \
                    FreeImageToolkit/JPEGTransform.cpp \
                    FreeImageToolkit/MultigridPoissonSolver.cpp \
                    FreeImageToolkit/Rescale.cpp \
                    FreeImageToolkit/Resize.cpp \
                    LibJPEG/jaricom.c \
                    LibJPEG/jcapimin.c \
                    LibJPEG/jcapistd.c \
                    LibJPEG/jcarith.c \
                    LibJPEG/jccoefct.c \
                    LibJPEG/jccolor.c \
                    LibJPEG/jcdctmgr.c \
                    LibJPEG/jchuff.c \
                    LibJPEG/jcinit.c \
                    LibJPEG/jcmainct.c \
                    LibJPEG/jcmarker.c \
                    LibJPEG/jcmaster.c \
                    LibJPEG/jcomapi.c \
                    LibJPEG/jcparam.c \
                    LibJPEG/jcprepct.c \
                    LibJPEG/jcsample.c \
                    LibJPEG/jctrans.c \
                    LibJPEG/jdapimin.c \
                    LibJPEG/jdapistd.c \
                    LibJPEG/jdarith.c \
                    LibJPEG/jdatadst.c \
                    LibJPEG/jdatasrc.c \
                    LibJPEG/jdcoefct.c \
                    LibJPEG/jdcolor.c \
                    LibJPEG/jddctmgr.c \
                    LibJPEG/jdhuff.c \
                    LibJPEG/jdinput.c \
                    LibJPEG/jdmainct.c \
                    LibJPEG/jdmarker.c \
                    LibJPEG/jdmaster.c \
                    LibJPEG/jdmerge.c \
                    LibJPEG/jdpostct.c \
                    LibJPEG/jdsample.c \
                    LibJPEG/jdtrans.c \
                    LibJPEG/jerror.c \
                    LibJPEG/jfdctflt.c \
                    LibJPEG/jfdctfst.c \
                    LibJPEG/jfdctint.c \
                    LibJPEG/jidctflt.c \
                    LibJPEG/jidctfst.c \
                    LibJPEG/jidctint.c \
                    LibJPEG/jmemmgr.c \
                    LibJPEG/jmemnobs.c \
                    LibJPEG/jquant1.c \
                    LibJPEG/jquant2.c \
                    LibJPEG/jutils.c \
                    LibJPEG/transupp.c \
                    LibMNG/libmng_callback_xs.c \
                    LibMNG/libmng_chunk_descr.c \
                    LibMNG/libmng_chunk_io.c \
                    LibMNG/libmng_chunk_prc.c \
                    LibMNG/libmng_chunk_xs.c \
                    LibMNG/libmng_cms.c \
                    LibMNG/libmng_display.c \
                    LibMNG/libmng_dither.c \
                    LibMNG/libmng_error.c \
                    LibMNG/libmng_filter.c \
                    LibMNG/libmng_hlapi.c \
                    LibMNG/libmng_jpeg.c \
                    LibMNG/libmng_object_prc.c \
                    LibMNG/libmng_pixels.c \
                    LibMNG/libmng_prop_xs.c \
                    LibMNG/libmng_read.c \
                    LibMNG/libmng_trace.c \
                    LibMNG/libmng_write.c \
                    LibMNG/libmng_zlib.c \
                    LibPNG/png.c \
                    LibPNG/pngerror.c \
                    LibPNG/pngget.c \
                    LibPNG/pngmem.c \
                    LibPNG/pngpread.c \
                    LibPNG/pngread.c \
                    LibPNG/pngrio.c \
                    LibPNG/pngrtran.c \
                    LibPNG/pngrutil.c \
                    LibPNG/pngset.c \
                    LibPNG/pngtrans.c \
                    LibPNG/pngwio.c \
                    LibPNG/pngwrite.c \
                    LibPNG/pngwtran.c \
                    LibPNG/pngwutil.c \
                    LibTIFF/tif_aux.c \
                    LibTIFF/tif_close.c \
                    LibTIFF/tif_codec.c \
                    LibTIFF/tif_color.c \
                    LibTIFF/tif_compress.c \
                    LibTIFF/tif_dir.c \
                    LibTIFF/tif_dirinfo.c \
                    LibTIFF/tif_dirread.c \
                    LibTIFF/tif_dirwrite.c \
                    LibTIFF/tif_dumpmode.c \
                    LibTIFF/tif_error.c \
                    LibTIFF/tif_extension.c \
                    LibTIFF/tif_fax3.c \
                    LibTIFF/tif_fax3sm.c \
                    LibTIFF/tif_flush.c \
                    LibTIFF/tif_getimage.c \
                    LibTIFF/tif_jpeg.c \
                    LibTIFF/tif_luv.c \
                    LibTIFF/tif_lzw.c \
                    LibTIFF/tif_next.c \
                    LibTIFF/tif_ojpeg.c \
                    LibTIFF/tif_open.c \
                    LibTIFF/tif_packbits.c \
                    LibTIFF/tif_pixarlog.c \
                    LibTIFF/tif_predict.c \
                    LibTIFF/tif_print.c \
                    LibTIFF/tif_read.c \
                    LibTIFF/tif_strip.c \
                    LibTIFF/tif_swab.c \
                    LibTIFF/tif_thunder.c \
                    LibTIFF/tif_tile.c \
                    LibTIFF/tif_version.c \
                    LibTIFF/tif_warning.c \
                    LibTIFF/tif_write.c \
                    LibTIFF/tif_zip.c \
                    LibTIFF/lfind.c \
                    ZLib/adler32.c \
                    ZLib/compress.c \
                    ZLib/crc32.c \
                    ZLib/deflate.c \
                    ZLib/infback.c \
                    ZLib/inffast.c \
                    ZLib/inflate.c \
                    ZLib/inftrees.c \
                    ZLib/trees.c \
                    ZLib/uncompr.c \
                    ZLib/zutil.c \
                    LibOpenJPEG/bio.c \
                    LibOpenJPEG/cio.c \
                    LibOpenJPEG/dwt.c \
                    LibOpenJPEG/event.c \
                    LibOpenJPEG/image.c \
                    LibOpenJPEG/j2k.c \
                    LibOpenJPEG/j2k_lib.c \
                    LibOpenJPEG/jp2.c \
                    LibOpenJPEG/jpt.c \
                    LibOpenJPEG/mct.c \
                    LibOpenJPEG/mqc.c \
                    LibOpenJPEG/openjpeg.c \
                    LibOpenJPEG/pi.c \
                    LibOpenJPEG/raw.c \
                    LibOpenJPEG/t1.c \
                    LibOpenJPEG/t2.c \
                    LibOpenJPEG/tcd.c \
                    LibOpenJPEG/tgt.c \
                    OpenEXR/IlmImf/ImfAttribute.cpp \
                    OpenEXR/IlmImf/ImfB44Compressor.cpp \
                    OpenEXR/IlmImf/ImfBoxAttribute.cpp \
                    OpenEXR/IlmImf/ImfChannelList.cpp \
                    OpenEXR/IlmImf/ImfChannelListAttribute.cpp \
                    OpenEXR/IlmImf/ImfChromaticities.cpp \
                    OpenEXR/IlmImf/ImfChromaticitiesAttribute.cpp \
                    OpenEXR/IlmImf/ImfCompressionAttribute.cpp \
                    OpenEXR/IlmImf/ImfCompressor.cpp \
                    OpenEXR/IlmImf/ImfConvert.cpp \
                    OpenEXR/IlmImf/ImfCRgbaFile.cpp \
                    OpenEXR/IlmImf/ImfDoubleAttribute.cpp \
                    OpenEXR/IlmImf/ImfEnvmap.cpp \
                    OpenEXR/IlmImf/ImfEnvmapAttribute.cpp \
                    OpenEXR/IlmImf/ImfFloatAttribute.cpp \
                    OpenEXR/IlmImf/ImfFrameBuffer.cpp \
                    OpenEXR/IlmImf/ImfFramesPerSecond.cpp \
                    OpenEXR/IlmImf/ImfHeader.cpp \
                    OpenEXR/IlmImf/ImfHuf.cpp \
                    OpenEXR/IlmImf/ImfInputFile.cpp \
                    OpenEXR/IlmImf/ImfIntAttribute.cpp \
                    OpenEXR/IlmImf/ImfIO.cpp \
                    OpenEXR/IlmImf/ImfKeyCode.cpp \
                    OpenEXR/IlmImf/ImfKeyCodeAttribute.cpp \
                    OpenEXR/IlmImf/ImfLineOrderAttribute.cpp \
                    OpenEXR/IlmImf/ImfLut.cpp \
                    OpenEXR/IlmImf/ImfMatrixAttribute.cpp \
                    OpenEXR/IlmImf/ImfMisc.cpp \
                    OpenEXR/IlmImf/ImfOpaqueAttribute.cpp \
                    OpenEXR/IlmImf/ImfOutputFile.cpp \
                    OpenEXR/IlmImf/ImfPizCompressor.cpp \
                    OpenEXR/IlmImf/ImfPreviewImage.cpp \
                    OpenEXR/IlmImf/ImfPreviewImageAttribute.cpp \
                    OpenEXR/IlmImf/ImfPxr24Compressor.cpp \
                    OpenEXR/IlmImf/ImfRational.cpp \
                    OpenEXR/IlmImf/ImfRationalAttribute.cpp \
                    OpenEXR/IlmImf/ImfRgbaFile.cpp \
                    OpenEXR/IlmImf/ImfRgbaYca.cpp \
                    OpenEXR/IlmImf/ImfRleCompressor.cpp \
                    OpenEXR/IlmImf/ImfScanLineInputFile.cpp \
                    OpenEXR/IlmImf/ImfStandardAttributes.cpp \
                    OpenEXR/IlmImf/ImfStdIO.cpp \
                    OpenEXR/IlmImf/ImfStringAttribute.cpp \
                    OpenEXR/IlmImf/ImfStringVectorAttribute.cpp \
                    OpenEXR/IlmImf/ImfTestFile.cpp \
                    OpenEXR/IlmImf/ImfThreading.cpp \
                    OpenEXR/IlmImf/ImfTileDescriptionAttribute.cpp \
                    OpenEXR/IlmImf/ImfTiledInputFile.cpp \
                    OpenEXR/IlmImf/ImfTiledMisc.cpp \
                    OpenEXR/IlmImf/ImfTiledOutputFile.cpp \
                    OpenEXR/IlmImf/ImfTiledRgbaFile.cpp \
                    OpenEXR/IlmImf/ImfTileOffsets.cpp \
                    OpenEXR/IlmImf/ImfTimeCode.cpp \
                    OpenEXR/IlmImf/ImfTimeCodeAttribute.cpp \
                    OpenEXR/IlmImf/ImfVecAttribute.cpp \
                    OpenEXR/IlmImf/ImfVersion.cpp \
                    OpenEXR/IlmImf/ImfWav.cpp \
                    OpenEXR/IlmImf/ImfZipCompressor.cpp \
                    OpenEXR/Imath/ImathBox.cpp \
                    OpenEXR/Imath/ImathColorAlgo.cpp \
                    OpenEXR/Imath/ImathFun.cpp \
                    OpenEXR/Imath/ImathMatrixAlgo.cpp \
                    OpenEXR/Imath/ImathRandom.cpp \
                    OpenEXR/Imath/ImathShear.cpp \
                    OpenEXR/Imath/ImathVec.cpp \
                    OpenEXR/Iex/IexBaseExc.cpp \
                    OpenEXR/Iex/IexThrowErrnoExc.cpp \
                    OpenEXR/Half/half.cpp \
                    OpenEXR/IlmThread/IlmThread.cpp \
                    OpenEXR/IlmThread/IlmThreadMutex.cpp \
                    OpenEXR/IlmThread/IlmThreadPool.cpp \
                    OpenEXR/IlmThread/IlmThreadSemaphore.cpp
