#-------------------------------------------------
#
# Project created by QtCreator 2016-07-22T10:49:04
#
#-------------------------------------------------

QT       += core gui
CONFIG += console

QMAKE_CXXFLAGS += /openmp

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

qtHaveModule(printsupport): QT += printsupport
qtHaveModule(opengl): QT += opengl



TARGET = StructuredLight3D
TEMPLATE = app


SOURCES += main.cpp\
    view.cpp \
    widget.cpp \
    imageitem.cpp \
    calibrate.cpp \
    calibratemachine.cpp \
    mutil_wavelength.cpp \
    src/common/debug.cpp \
    src/common/disparity_map.cpp \
    src/common/module.cpp \
    src/common/other.cpp \
    src/common/parameters.cpp \
    src/common/returncode.cpp \
    src/common/pattern/pattern.cpp \
    src/common/pattern/pattern_sequence.cpp \
    src/common/image/image.cpp \
    src/common/image/image_window.cpp \
    src/common/capture/capture.cpp \
    src/common/capture/capture_sequence.cpp \
    src/dlp_platforms/lightcrafter_4500/common.cpp \
    src/dlp_platforms/lightcrafter_4500/dlpc350_api.cpp \
    src/dlp_platforms/lightcrafter_4500/dlpc350_firmware.cpp \
    src/dlp_platforms/lightcrafter_4500/dlpc350_usb.cpp \
    src/dlp_platforms/lightcrafter_4500/lcr4500.cpp \
    src/structured_light/structured_light.cpp \
    src/camera/camera.cpp \
    src/camera/pg_flycap2/pg_flycap2_c.cpp \
    src/dlp_platforms/dlp_platform.cpp \
    capturethread.cpp \
    binary_code.cpp \
    reconstruct3d.cpp \
    phaseshift.cpp \
    yuan_pattern.cpp \
    paintdialog.cpp \
    MultipleLinearRegression.cpp \
    hash_table.cpp

HEADERS  += \
    view.h \
    widget.h \
    imageitem.h \
    calibrate.h \
    calibratemachine.h \
    mutil_wavelength.h \
    capturethread.h \
    binary_code.h \
    reconstruct3d.h \
    phaseshift.h \
    yuan_pattern.h \
    paintdialog.h \
    MultipleLinearRegression.h \
    hash_table.h

RESOURCES += \
    images.qrc

DISTFILES += \
    fileprint.png \
    qt4logo.png \
    rotateleft.png \
    rotateright.png \
    zoomin.png \
    zoomout.png




INCLUDEPATH += ../DLP_Path/DLP-ALC-LIGHTCRAFTER-SDK/include


OPENCV_DIR = "../OpenCV2.4.13_build_for_VS2015_x86"
INCLUDEPATH += $$OPENCV_DIR/include



CONFIG(release, debug|release) {
LIBS += -L$$OPENCV_DIR/x86/vc14/lib\
        -lopencv_calib3d2413\
        -lopencv_contrib2413\
        -lopencv_core2413\
        -lopencv_features2d2413\
        -lopencv_flann2413\
        -lopencv_gpu2413\
        -lopencv_highgui2413\
        -lopencv_imgproc2413\
        -lopencv_legacy2413\
        -lopencv_ml2413\
        -lopencv_nonfree2413\
        -lopencv_objdetect2413\
        -lopencv_ocl2413\
        -lopencv_photo2413\
        -lopencv_stitching2413\
        -lopencv_superres2413\
        -lopencv_video2413\
        -lopencv_videostab2413\
}
CONFIG(debug, debug|release) {

LIBS += -L$$OPENCV_DIR/x86/vc14/lib\
        -lopencv_calib3d2413d\
        -lopencv_contrib2413d\
        -lopencv_core2413d\
        -lopencv_features2d2413d\
        -lopencv_flann2413d\
        -lopencv_gpu2413d\
        -lopencv_highgui2413d\
        -lopencv_imgproc2413d\
        -lopencv_legacy2413d\
        -lopencv_ml2413d\
        -lopencv_nonfree2413d\
        -lopencv_objdetect2413d\
        -lopencv_ocl2413d\
        -lopencv_photo2413d\
        -lopencv_stitching2413d\
        -lopencv_superres2413d\
        -lopencv_video2413d\
        -lopencv_videostab2413d\
}

FLYCAPTURE_DIR = "../FlyCapture2"
INCLUDEPATH += $$FLYCAPTURE_DIR/include
LIBS += -L$$FLYCAPTURE_DIR/lib/C -lFlyCapture2_C


INCLUDEPATH += ../DLP_Path/glfw_3.2.1/include
LIBS+=../DLP_Path/glfw_3.2.1/lib-vc2015/glfw3dll.lib


LIBS+= ../DLP_Path/hidapi-master/windows/Debug/hidapi.lib


INCLUDEPATH +=  ../DLP_Path/asio-1.10.8/include

FORMS += \
    paintdialog.ui





#INCLUDEPATH += ../PCL1.8.0/include/pcl-1.8\


#INCLUDEPATH += ../PCL1.8.0/include/pcl-1.8/pcl\


#INCLUDEPATH += ../PCL1.8.0/3rdParty/Boost/include/boost-1_61\


#INCLUDEPATH += ../PCL1.8.0/3rdParty/Eigen/eigen3\


#INCLUDEPATH += ../PCL1.8.0/3rdParty/FLANN/include\


#INCLUDEPATH += ../PCL1.8.0/3rdParty/FLANN/include/flann\


#INCLUDEPATH += ../PCL1.8.0/3rdParty/OpenNI/Include\


#INCLUDEPATH += ../PCL1.8.0/3rdParty/Qhull/include\


#INCLUDEPATH += ../PCL1.8.0/3rdParty/VTK/include/vtk-7.0\

#PCL_DIR = "../PCL1.8.0"

#CONFIG(debug,debug|release){
#LIBS += -L$$PCL_DIR/lib\
#        -lpcl_common_debug\
#        -lpcl_features_debug\
#        -lpcl_filters_debug\
#        -lpcl_io_debug\
#        -lpcl_io_ply_debug\
#        -lpcl_kdtree_debug\
#        -lpcl_keypoints_debug\
#        -lpcl_ml_debug\
#        -lpcl_octree_debug\
#        -lpcl_outofcore_debug\
#        -lpcl_people_debug\
#        -lpcl_recognition_debug\
#        -lpcl_registration_debug\
#        -lpcl_sample_consensus_debug\
#        -lpcl_search_debug\
#        -lpcl_segmentation_debug\
#        -lpcl_stereo_debug\
#        -lpcl_surface_debug\
#        -lpcl_tracking_debug\
#        -lpcl_visualization_debug\

#LIBS += -L$$PCL_DIR/3rdParty/Boost/lib\
#        -llibboost_atomic-vc140-mt-gd-1_61\
#        -llibboost_chrono-vc140-mt-gd-1_61\
#        -llibboost_container-vc140-mt-gd-1_61\
#        -llibboost_context-vc140-mt-gd-1_61\
#        -llibboost_coroutine-vc140-mt-gd-1_61\
#        -llibboost_date_time-vc140-mt-gd-1_61\
#        -llibboost_exception-vc140-mt-gd-1_61\
#        -llibboost_filesystem-vc140-mt-gd-1_61\
#        -llibboost_graph-vc140-mt-gd-1_61\
#        -llibboost_iostreams-vc140-mt-gd-1_61\
#        -llibboost_locale-vc140-mt-gd-1_61\
#        -llibboost_log-vc140-mt-gd-1_61\
#        -llibboost_log_setup-vc140-mt-gd-1_61\
#        -llibboost_math_c99-vc140-mt-gd-1_61\
#        -llibboost_math_c99f-vc140-mt-gd-1_61\
#        -llibboost_math_c99l-vc140-mt-gd-1_61\
#        -llibboost_math_tr1-vc140-mt-gd-1_61\
#        -llibboost_math_tr1f-vc140-mt-gd-1_61\
#        -llibboost_math_tr1l-vc140-mt-gd-1_61\
#        -llibboost_mpi-vc140-mt-gd-1_61\
#        -llibboost_prg_exec_monitor-vc140-mt-gd-1_61\
#        -llibboost_program_options-vc140-mt-gd-1_61\
#        -llibboost_random-vc140-mt-gd-1_61\
#        -llibboost_regex-vc140-mt-gd-1_61\
#        -llibboost_serialization-vc140-mt-gd-1_61\
#        -llibboost_system-vc140-mt-gd-1_61\
#        -llibboost_test_exec_monitor-vc140-mt-gd-1_61\
#        -llibboost_thread-vc140-mt-gd-1_61\
#        -llibboost_timer-vc140-mt-gd-1_61\
#        -llibboost_type_erasure-vc140-mt-gd-1_61\
#        -llibboost_unit_test_framework-vc140-mt-gd-1_61\
#        -llibboost_wave-vc140-mt-gd-1_61\
#        -llibboost_wserialization-vc140-mt-gd-1_61

#LIBS += -L$$PCL_DIR/3rdParty/FLANN/lib\
#        -lflann-gd\
#        -lflann_cpp_s-gd\
#        -lflann_s-gd

#LIBS += -L$$PCL_DIR/3rdParty/OpenNI/Lib\
#        -lOpenNI2

#LIBS += -L$$PCL_DIR/3rdParty/Qhull/lib\
#        -lqhull-d\
#        -lqhullcpp-d\
#        -lqhullstatic-d\
#        -lqhullstatic_r-d\
#        -lqhull_p-d\
#        -lqhull_r-d

#LIBS += -L$$PCL_DIR/3rdParty/VTK/lib\
#        -lvtkalglib-7.0-gd\
#        -lvtkChartsCore-7.0-gd\
#        -lvtkCommonColor-7.0-gd\
#        -lvtkCommonComputationalGeometry-7.0-gd\
#        -lvtkCommonCore-7.0-gd\
#        -lvtkCommonDataModel-7.0-gd\
#        -lvtkCommonExecutionModel-7.0-gd\
#        -lvtkCommonMath-7.0-gd\
#        -lvtkCommonMisc-7.0-gd\
#        -lvtkCommonSystem-7.0-gd\
#        -lvtkCommonTransforms-7.0-gd\
#        -lvtkDICOMParser-7.0-gd\
#        -lvtkDomainsChemistry-7.0-gd\
#        -lvtkexoIIc-7.0-gd\
#        -lvtkexpat-7.0-gd\
#        -lvtkFiltersAMR-7.0-gd\
#        -lvtkFiltersCore-7.0-gd\
#        -lvtkFiltersExtraction-7.0-gd\
#        -lvtkFiltersFlowPaths-7.0-gd\
#        -lvtkFiltersGeneral-7.0-gd\
#        -lvtkFiltersGeneric-7.0-gd\
#        -lvtkFiltersGeometry-7.0-gd\
#        -lvtkFiltersHybrid-7.0-gd\
#        -lvtkFiltersHyperTree-7.0-gd\
#        -lvtkFiltersImaging-7.0-gd\
#        -lvtkFiltersModeling-7.0-gd\
#        -lvtkFiltersParallel-7.0-gd\
#        -lvtkFiltersParallelImaging-7.0-gd\
#        -lvtkFiltersProgrammable-7.0-gd\
#        -lvtkFiltersSelection-7.0-gd\
#        -lvtkFiltersSMP-7.0-gd\
#        -lvtkFiltersSources-7.0-gd\
#        -lvtkFiltersStatistics-7.0-gd\
#        -lvtkFiltersTexture-7.0-gd\
#        -lvtkFiltersVerdict-7.0-gd\
#        -lvtkfreetype-7.0-gd\
#        -lvtkGeovisCore-7.0-gd\
#        -lvtkgl2ps-7.0-gd\
#        -lvtkhdf5-7.0-gd\
#        -lvtkhdf5_hl-7.0-gd\
#        -lvtkImagingColor-7.0-gd\
#        -lvtkImagingCore-7.0-gd\
#        -lvtkImagingFourier-7.0-gd\
#        -lvtkImagingGeneral-7.0-gd\
#        -lvtkImagingHybrid-7.0-gd\
#        -lvtkImagingMath-7.0-gd\
#        -lvtkImagingMorphological-7.0-gd\
#        -lvtkImagingSources-7.0-gd\
#        -lvtkImagingStatistics-7.0-gd\
#        -lvtkImagingStencil-7.0-gd\
#        -lvtkInfovisCore-7.0-gd\
#        -lvtkInfovisLayout-7.0-gd\
#        -lvtkInteractionImage-7.0-gd\
#        -lvtkInteractionStyle-7.0-gd\
#        -lvtkInteractionWidgets-7.0-gd\
#        -lvtkIOAMR-7.0-gd\
#        -lvtkIOCore-7.0-gd\
#        -lvtkIOEnSight-7.0-gd\
#        -lvtkIOExodus-7.0-gd\
#        -lvtkIOExport-7.0-gd\
#        -lvtkIOGeometry-7.0-gd\
#        -lvtkIOImage-7.0-gd\
#        -lvtkIOImport-7.0-gd\
#        -lvtkIOInfovis-7.0-gd\
#        -lvtkIOLegacy-7.0-gd\
#        -lvtkIOLSDyna-7.0-gd\
#        -lvtkIOMINC-7.0-gd\
#        -lvtkIOMovie-7.0-gd\
#        -lvtkIONetCDF-7.0-gd\
#        -lvtkIOParallel-7.0-gd\
#        -lvtkIOParallelXML-7.0-gd\
#        -lvtkIOPLY-7.0-gd\
#        -lvtkIOSQL-7.0-gd\
#        -lvtkIOVideo-7.0-gd\
#        -lvtkIOXML-7.0-gd\
#        -lvtkIOXMLParser-7.0-gd\
#        -lvtkjpeg-7.0-gd\
#        -lvtkjsoncpp-7.0-gd\
#        -lvtklibxml2-7.0-gd\
#        -lvtkmetaio-7.0-gd\
#        -lvtkNetCDF-7.0-gd\
#        -lvtkNetCDF_cxx-7.0-gd\
#        -lvtkoggtheora-7.0-gd\
#        -lvtkParallelCore-7.0-gd\
#        -lvtkpng-7.0-gd\
#        -lvtkproj4-7.0-gd\
#        -lvtkRenderingAnnotation-7.0-gd\
#        -lvtkRenderingContext2D-7.0-gd\
#        -lvtkRenderingContextOpenGL-7.0-gd\
#        -lvtkRenderingCore-7.0-gd\
#        -lvtkRenderingFreeType-7.0-gd\
#        -lvtkRenderingGL2PS-7.0-gd\
#        -lvtkRenderingImage-7.0-gd\
#        -lvtkRenderingLabel-7.0-gd\
#        -lvtkRenderingLIC-7.0-gd\
#        -lvtkRenderingLOD-7.0-gd\
#        -lvtkRenderingOpenGL-7.0-gd\
#        -lvtkRenderingVolume-7.0-gd\
#        -lvtkRenderingVolumeOpenGL-7.0-gd\
#        -lvtksqlite-7.0-gd\
#        -lvtksys-7.0-gd\
#        -lvtktiff-7.0-gd\
#        -lvtkverdict-7.0-gd\
#        -lvtkViewsContext2D-7.0-gd\
#        -lvtkViewsCore-7.0-gd\
#        -lvtkViewsInfovis-7.0-gd\
#        -lvtkzlib-7.0-gd


#} else {
#LIBS += -L$$PCL_DIR/lib\
#        -lpcl_common_release\
#        -lpcl_features_release\
#        -lpcl_filters_release\
#        -lpcl_io_release\
#        -lpcl_io_ply_release\
#        -lpcl_kdtree_release\
#        -lpcl_keypoints_release\
#        -lpcl_ml_release\
#        -lpcl_octree_release\
#        -lpcl_outofcore_release\
#        -lpcl_people_release\
#        -lpcl_recognition_release\
#        -lpcl_registration_release\
#        -lpcl_sample_consensus_release\
#        -lpcl_search_release\
#        -lpcl_segmentation_release\
#        -lpcl_stereo_release\
#        -lpcl_surface_release\
#        -lpcl_tracking_release\
#        -lpcl_visualization_release\

#LIBS += -L$$PCL_DIR/3rdParty/Boost/lib\
#        -llibboost_atomic-vc140-mt-1_61\
#        -llibboost_chrono-vc140-mt-1_61\
#        -llibboost_container-vc140-mt-1_61\
#        -llibboost_context-vc140-mt-1_61\
#        -llibboost_coroutine-vc140-mt-1_61\
#        -llibboost_date_time-vc140-mt-1_61\
#        -llibboost_exception-vc140-mt-1_61\
#        -llibboost_filesystem-vc140-mt-1_61\
#        -llibboost_graph-vc140-mt-1_61\
#        -llibboost_iostreams-vc140-mt-1_61\
#        -llibboost_locale-vc140-mt-1_61\
#        -llibboost_log-vc140-mt-1_61\
#        -llibboost_log_setup-vc140-mt-1_61\
#        -llibboost_math_c99-vc140-mt-1_61\
#        -llibboost_math_c99f-vc140-mt-1_61\
#        -llibboost_math_c99l-vc140-mt-1_61\
#        -llibboost_math_tr1-vc140-mt-1_61\
#        -llibboost_math_tr1f-vc140-mt-1_61\
#        -llibboost_math_tr1l-vc140-mt-1_61\
#        -llibboost_mpi-vc140-mt-1_61\
#        -llibboost_prg_exec_monitor-vc140-mt-1_61\
#        -llibboost_program_options-vc140-mt-1_61\
#        -llibboost_random-vc140-mt-1_61\
#        -llibboost_regex-vc140-mt-1_61\
#        -llibboost_serialization-vc140-mt-1_61\
#        -llibboost_system-vc140-mt-1_61\
#        -llibboost_test_exec_monitor-vc140-mt-1_61\
#        -llibboost_thread-vc140-mt-1_61\
#        -llibboost_timer-vc140-mt-1_61\
#        -llibboost_type_erasure-vc140-mt-1_61\
#        -llibboost_unit_test_framework-vc140-mt-1_61\
#        -llibboost_wave-vc140-mt-1_61\
#        -llibboost_wserialization-vc140-mt-1_61


#LIBS += -L$$PCL_DIR/3rdParty/FLANN/lib\
#        -lflann\
#        -lflann_cpp_s\
#        -lflann_s

#LIBS += -L$$PCL_DIR/3rdParty/OpenNI/Lib\
#        -lOpenNI2

#LIBS += -L$$PCL_DIR/3rdParty/Qhull/lib\
#        -lqhull\
#        -lqhullcpp\
#        -lqhullstatic\
#        -lqhullstatic_r\
#        -lqhull_p\
#        -lqhull_r

##LIBS += -L$$PCL_DIR/3rdParty/VTK/lib\
##        -lvtkalglib-7.0\
##        -lvtkChartsCore-7.0\
##        -lvtkCommonColor-7.0\
##        -lvtkCommonComputationalGeometry-7.0\
##        -lvtkCommonCore-7.0\
##        -lvtkCommonDataModel-7.0\
##        -lvtkCommonExecutionModel-7.0\
##        -lvtkCommonMath-7.0\
##        -lvtkCommonMisc-7.0\
##        -lvtkCommonSystem-7.0\
##        -lvtkCommonTransforms-7.0\
##        -lvtkDICOMParser-7.0\
##        -lvtkDomainsChemistry-7.0\
##        -lvtkexoIIc-7.0\
##        -lvtkexpat-7.0\
##        -lvtkFiltersAMR-7.0\
##        -lvtkFiltersCore-7.0\
##        -lvtkFiltersExtraction-7.0\
##        -lvtkFiltersFlowPaths-7.0\
##        -lvtkFiltersGeneral-7.0\
##        -lvtkFiltersGeneric-7.0\
##        -lvtkFiltersGeometry-7.0\
##        -lvtkFiltersHybrid-7.0\
##        -lvtkFiltersHyperTree-7.0\
##        -lvtkFiltersImaging-7.0\
##        -lvtkFiltersModeling-7.0\
##        -lvtkFiltersParallel-7.0\
##        -lvtkFiltersParallelImaging-7.0\
##        -lvtkFiltersProgrammable-7.0\
##        -lvtkFiltersSelection-7.0\
##        -lvtkFiltersSMP-7.0\
##        -lvtkFiltersSources-7.0\
##        -lvtkFiltersStatistics-7.0\
##        -lvtkFiltersTexture-7.0\
##        -lvtkFiltersVerdict-7.0\
##        -lvtkfreetype-7.0\
##        -lvtkGeovisCore-7.0\
##        -lvtkgl2ps-7.0\
##        -lvtkhdf5-7.0\
##        -lvtkhdf5_hl-7.0\
##        -lvtkImagingColor-7.0\
##        -lvtkImagingCore-7.0\
##        -lvtkImagingFourier-7.0\
##        -lvtkImagingGeneral-7.0\
##        -lvtkImagingHybrid-7.0\
##        -lvtkImagingMath-7.0\
##        -lvtkImagingMorphological-7.0\
##        -lvtkImagingSources-7.0\
##        -lvtkImagingStatistics-7.0\
##        -lvtkImagingStencil-7.0\
##        -lvtkInfovisCore-7.0\
##        -lvtkInfovisLayout-7.0\
##        -lvtkInteractionImage-7.0\
##        -lvtkInteractionStyle-7.0\
##        -lvtkInteractionWidgets-7.0\
##        -lvtkIOAMR-7.0\
##        -lvtkIOCore-7.0\
##        -lvtkIOEnSight-7.0\
##        -lvtkIOExodus-7.0\
##        -lvtkIOExport-7.0\
##        -lvtkIOGeometry-7.0\
##        -lvtkIOImage-7.0\
##        -lvtkIOImport-7.0\
##        -lvtkIOInfovis-7.0\
##        -lvtkIOLegacy-7.0\
##        -lvtkIOLSDyna-7.0\
##        -lvtkIOMINC-7.0\
##        -lvtkIOMovie-7.0\
##        -lvtkIONetCDF-7.0\
##        -lvtkIOParallel-7.0\
##        -lvtkIOParallelXML-7.0\
##        -lvtkIOPLY-7.0\
##        -lvtkIOSQL-7.0\
##        -lvtkIOVideo-7.0\
##        -lvtkIOXML-7.0\
##        -lvtkIOXMLParser-7.0\
##        -lvtkjpeg-7.0\
##        -lvtkjsoncpp-7.0\
##        -lvtklibxml2-7.0\
##        -lvtkmetaio-7.0\
##        -lvtkNetCDF-7.0\
##        -lvtkNetCDF_cxx-7.0\
##        -lvtkoggtheora-7.0\
##        -lvtkParallelCore-7.0\
##        -lvtkpng-7.0\
##        -lvtkproj4-7.0\
##        -lvtkRenderingAnnotation-7.0\
##        -lvtkRenderingContext2D-7.0\
##        -lvtkRenderingContextOpenGL-7.0\
##        -lvtkRenderingCore-7.0\
##        -lvtkRenderingFreeType-7.0\
##        -lvtkRenderingGL2PS-7.0\
##        -lvtkRenderingImage-7.0\
##        -lvtkRenderingLabel-7.0\
##        -lvtkRenderingLIC-7.0\
##        -lvtkRenderingLOD-7.0\
##        -lvtkRenderingOpenGL-7.0\
##        -lvtkRenderingVolume-7.0\
##        -lvtkRenderingVolumeOpenGL-7.0\
##        -lvtksqlite-7.0\
##        -lvtksys-7.0\
##        -lvtktiff-7.0\
##        -lvtkverdict-7.0\
##        -lvtkViewsContext2D-7.0\
##        -lvtkViewsCore-7.0\
##        -lvtkViewsInfovis-7.0\
##        -lvtkzlib-7.0

#}

#INCLUDEPATH += ../VTKQT/VTK_install/include/vtk-7.1\

#LIBS += -L../VTKQT/VTK_install/lib\
#                -lvtkalglib-7.1\
#                -lvtkChartsCore-7.1\
#                -lvtkCommonColor-7.1\
#                -lvtkCommonComputationalGeometry-7.1\
#                -lvtkCommonCore-7.1\
#                -lvtkCommonDataModel-7.1\
#                -lvtkCommonExecutionModel-7.1\
#                -lvtkCommonMath-7.1\
#                -lvtkCommonMisc-7.1\
#                -lvtkCommonSystem-7.1\
#                -lvtkCommonTransforms-7.1\
#                -lvtkDICOMParser-7.1\
#                -lvtkDomainsChemistry-7.1\
#                -lvtkDomainsChemistryOpenGL2-7.1\
#                -lvtkexoIIc-7.1\
#                -lvtkexpat-7.1\
#                -lvtkFiltersAMR-7.1\
#                -lvtkFiltersCore-7.1\
#                -lvtkFiltersExtraction-7.1\
#                -lvtkFiltersFlowPaths-7.1\
#                -lvtkFiltersGeneral-7.1\
#                -lvtkFiltersGeneric-7.1\
#                -lvtkFiltersGeometry-7.1\
#                -lvtkFiltersHybrid-7.1\
#                -lvtkFiltersHyperTree-7.1\
#                -lvtkFiltersImaging-7.1\
#                -lvtkFiltersModeling-7.1\
#                -lvtkFiltersParallel-7.1\
#                -lvtkFiltersParallelImaging-7.1\
#                -lvtkFiltersPoints-7.1\
#                -lvtkFiltersProgrammable-7.1\
#                -lvtkFiltersSelection-7.1\
#                -lvtkFiltersSMP-7.1\
#                -lvtkFiltersSources-7.1\
#                -lvtkFiltersStatistics-7.1\
#                -lvtkFiltersTexture-7.1\
#                -lvtkFiltersVerdict-7.1\
#                -lvtkfreetype-7.1\
#                -lvtkGeovisCore-7.1\
#                -lvtkgl2ps-7.1\
#                -lvtkglew-7.1\
#                -lvtkGUISupportQt-7.1\
#                -lvtkGUISupportQtOpenGL-7.1\
#                -lvtkGUISupportQtSQL-7.1\
#                -lvtkhdf5-7.1\
#                -lvtkhdf5_hl-7.1\
#                -lvtkImagingColor-7.1\
#                -lvtkImagingCore-7.1\
#                -lvtkImagingFourier-7.1\
#                -lvtkImagingGeneral-7.1\
#                -lvtkImagingHybrid-7.1\
#                -lvtkImagingMath-7.1\
#                -lvtkImagingMorphological-7.1\
#                -lvtkImagingSources-7.1\
#                -lvtkImagingStatistics-7.1\
#                -lvtkImagingStencil-7.1\
#                -lvtkInfovisCore-7.1\
#                -lvtkInfovisLayout-7.1\
#                -lvtkInteractionImage-7.1\
#                -lvtkInteractionStyle-7.1\
#                -lvtkInteractionWidgets-7.1\
#                -lvtkIOAMR-7.1\
#                -lvtkIOCore-7.1\
#                -lvtkIOEnSight-7.1\
#                -lvtkIOExodus-7.1\
#                -lvtkIOExport-7.1\
#                -lvtkIOGeometry-7.1\
#                -lvtkIOImage-7.1\
#                -lvtkIOImport-7.1\
#                -lvtkIOInfovis-7.1\
#                -lvtkIOLegacy-7.1\
#                -lvtkIOLSDyna-7.1\
#                -lvtkIOMINC-7.1\
#                -lvtkIOMovie-7.1\
#                -lvtkIONetCDF-7.1\
#                -lvtkIOParallel-7.1\
#                -lvtkIOParallelXML-7.1\
#                -lvtkIOPLY-7.1\
#                -lvtkIOSQL-7.1\
#                -lvtkIOTecplotTable-7.1\
#                -lvtkIOVideo-7.1\
#                -lvtkIOXML-7.1\
#                -lvtkIOXMLParser-7.1\
#                -lvtkjpeg-7.1\
#                -lvtkjsoncpp-7.1\
#                -lvtklibxml2-7.1\
#                -lvtkmetaio-7.1\
#                -lvtkNetCDF-7.1\
#                -lvtkNetCDF_cxx-7.1\
#                -lvtkoggtheora-7.1\
#                -lvtkParallelCore-7.1\
#                -lvtkpng-7.1\
#                -lvtkproj4-7.1\
#                -lvtkRenderingAnnotation-7.1\
#                -lvtkRenderingContext2D-7.1\
#                -lvtkRenderingContextOpenGL2-7.1\
#                -lvtkRenderingCore-7.1\
#                -lvtkRenderingFreeType-7.1\
#                -lvtkRenderingGL2PSOpenGL2-7.1\
#                -lvtkRenderingImage-7.1\
#                -lvtkRenderingLabel-7.1\
#                -lvtkRenderingLOD-7.1\
#                -lvtkRenderingOpenGL2-7.1\
#                -lvtkRenderingQt-7.1\
#                -lvtkRenderingVolume-7.1\
#                -lvtkRenderingVolumeOpenGL2-7.1\
#                -lvtksqlite-7.1\
#                -lvtksys-7.1\
#                -lvtktiff-7.1\
#                -lvtkverdict-7.1\
#                -lvtkViewsContext2D-7.1\
#                -lvtkViewsCore-7.1\
#                -lvtkViewsInfovis-7.1\
#                -lvtkViewsQt-7.1\
#                -lvtkzlib-7.1
