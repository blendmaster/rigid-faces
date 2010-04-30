CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( AAM )

SET( QT_AAMTRAINER_SRCS
     ${CMAKE_SOURCE_DIR}/src/main.cpp
     ${CMAKE_SOURCE_DIR}/src/opencvWebcam.cpp
     ${CMAKE_SOURCE_DIR}/src/delaunay.cpp
     ${CMAKE_SOURCE_DIR}/src/canvas.cpp
          ${CMAKE_SOURCE_DIR}/src/landmarkpoint.cpp
          ${CMAKE_SOURCE_DIR}/src/lkFitting.cpp
          ${CMAKE_SOURCE_DIR}/src/pca_aam.cpp
     ${CMAKE_SOURCE_DIR}/src/qtUtils.cpp
   )
SET( RUNNER_SRCS
     ${CMAKE_SOURCE_DIR}/src/runner.cpp
     ${CMAKE_SOURCE_DIR}/src/opencvWebcam.cpp
     ${CMAKE_SOURCE_DIR}/src/delaunay.cpp
    ${CMAKE_SOURCE_DIR}/src/lkFitting.cpp
   )

SET( QT_AAMTRAINER_MOC_HDRS
     ${CMAKE_SOURCE_DIR}/include/aamTrainer.h
      ${CMAKE_SOURCE_DIR}/include/canvas.h
      ${CMAKE_SOURCE_DIR}/include/landmarkpoint.h

  )


SET( QT_AAMTRAINER_UIS
     ${CMAKE_SOURCE_DIR}/aam/data/aamTrainer.ui
  )


SET(BIN_INSTALL_DIR
    "/bin"
    CACHE PATH "The subdirectory relative to the install prefix for executables (default is bin)"
   )

SET(DATA_INSTALL_DIR
    "/share"
    CACHE PATH "The subdirectory relative to the install prefix where shared data will be installed (default is share)."
   )


FIND_PACKAGE( Qt4 REQUIRED )
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")
FIND_PACKAGE(OpenCV REQUIRED)
SET( VERSION "0.3" )
SET( PKGDATADIR "${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DIR}" )
SET( BINDIR "${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_DIR}" )
ADD_DEFINITIONS ( -D_GLIBCXX_PROFILE -Wall -ftree-vectorizer-verbose=3 -pipe -O3 -g -ffast-math -fomit-frame-pointer  -msse2  -DVERSION='"${VERSION}"' -DPKGDATADIR=\"${PKGDATADIR}\" -DBINDIR=\"${BINDIR}\")

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_UI( QT_AAMTRAINER_UI_HDRS ${QT_AAMTRAINER_UIS} )
QT4_WRAP_CPP( QT_AAMTRAINER_MOC_SRCS ${QT_AAMTRAINER_MOC_HDRS} )

INCLUDE_DIRECTORIES( "${CMAKE_SOURCE_DIR}/include" )
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})

ADD_EXECUTABLE( qt-aamtrainer ${QT_AAMTRAINER_SRCS} ${QT_AAMTRAINER_MOC_SRCS} ${QT_AAMTRAINER_RC_SRCS} ${QT_AAMTRAINER_UI_HDRS} )
ADD_EXECUTABLE( runner ${RUNNER_SRCS})
TARGET_LINK_LIBRARIES( runner ${OPENCV_LIBRARIES})

TARGET_LINK_LIBRARIES( qt-aamtrainer ${QT_LIBRARIES} ${OPENCV_LIBRARIES})


INSTALL(
    TARGETS
    qt-aamtrainer
    DESTINATION
    ${BINDIR}
)
