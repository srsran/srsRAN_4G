#
# Copyright 2012-2013 The Iris Project Developers. See the
# COPYRIGHT file at the top-level directory of this distribution
# and at http://www.softwareradiosystems.com/iris/copyright.html.
#
# This file is part of the Iris Project.
#
# Iris is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# Iris is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# A copy of the GNU Lesser General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

# - Try to find the Qwt includes and library
# - Defines the following:
#
# QWT_FOUND - system has Qwt
# QWT_INCLUDE_DIR - where to find qwt.h
# QWT_INCLUDE_DIRS - the qwt include directories
# QWT_LIBRARY - where to find the Qwt library (not for general use)
# QWT_LIBRARIES - the libraries to link against to use Qwt
# QWT_MAJOR_VERSION - major version
# QWT_MINOR_VERSION - minor version
# QWT_PATCH_VERSION - patch version
# QWT_VERSION_STRING - version (ex. 5.2.1)

SET(QWT_FOUND "NO")

FIND_PATH(QWT_INCLUDE_DIR qwt.h
  /usr/local/qwt/include
  /usr/local/include
  /usr/include/qwt
  /usr/include/qwt-qt4
  /usr/include/qwt5
  /usr/include
  /opt/local/include/qwt #macports path
  $ENV{QWT_DIR}/include
  $ENV{QWT_DIR}/src
  $ENV{QWTDIR}/include
  $ENV{QWTDIR}/src
  $ENV{QWT_ROOT}/include
  $ENV{QWT_ROOT}/src
  $ENV{QWTROOT}/include
  $ENV{QWTROOT}/src
)

SET(QWT_INCLUDE_DIRS ${QWT_INCLUDE_DIR})

# version
SET(_VERSION_FILE ${QWT_INCLUDE_DIR}/qwt_global.h)
IF(EXISTS ${_VERSION_FILE} )
  FILE( STRINGS ${_VERSION_FILE} _VERSION_LINE REGEX "define[ ]+QWT_VERSION_STR")
  IF( _VERSION_LINE )
    STRING( REGEX REPLACE ".*define[ ]+QWT_VERSION_STR[ ]+\"(.*)\".*" "\\1" QWT_VERSION_STRING "${_VERSION_LINE}" )
    STRING( REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\1" QWT_MAJOR_VERSION "${QWT_VERSION_STRING}" )
    STRING( REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\2" QWT_MINOR_VERSION "${QWT_VERSION_STRING}" )
    STRING( REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\3" QWT_PATCH_VERSION "${QWT_VERSION_STRING}" )
  ENDIF()
ENDIF()


# check version
SET( _QWT_VERSION_MATCH TRUE )
IF( Qwt_FIND_VERSION AND QWT_VERSION_STRING )
  IF( Qwt_FIND_VERSION_EXACT )
    IF( NOT Qwt_FIND_VERSION VERSION_EQUAL QWT_VERSION_STRING )
      SET( _QWT_VERSION_MATCH FALSE )
    ENDIF()
  ELSE()
    IF( QWT_VERSION_STRING VERSION_LESS Qwt_FIND_VERSION )
      SET( _QWT_VERSION_MATCH FALSE )
    ENDIF()
  ENDIF()
ENDIF()

SET(POTENTIAL_LIBRARY_PATHS /usr/local/qwt/lib /usr/local/lib /usr/lib /opt/local/lib
              $ENV{QWT_DIR}/lib $ENV{QWTDIR}/lib $ENV{QWT_ROOT}/lib $ENV{QWTROOT}/lib)
              
SET(QWT_NAMES ${QWT_NAMES} qwt qwt-qt4 qwt5 )
FIND_LIBRARY(QWT_LIBRARY
  NAMES ${QWT_NAMES}
  PATHS ${POTENTIAL_LIBRARY_PATHS}
)
MARK_AS_ADVANCED(QWT_LIBRARY)

IF (QWT_LIBRARY)

  IF(WIN32 AND NOT CYGWIN)

    SET(QWT_NAMES_DEBUG qwtd qwtd-qt4 qwtd5 )
    FIND_LIBRARY(QWT_LIBRARY_DEBUG
      NAMES ${QWT_NAMES_DEBUG}
      PATHS ${POTENTIAL_LIBRARY_PATHS}
    )
    MARK_AS_ADVANCED(QWT_LIBRARY_DEBUG)
    
    IF(QWT_LIBRARY_DEBUG)
      SET(QWT_LIBRARIES optimized ${QWT_LIBRARY} debug ${QWT_LIBRARY_DEBUG} CACHE DOC "QWT library files")
    ELSE(QWT_LIBRARY_DEBUG)
      SET(QWT_LIBRARIES ${QWT_LIBRARY} CACHE DOC "QWT library files")
    ENDIF(QWT_LIBRARY_DEBUG)
    
    ADD_DEFINITIONS(-DQWT_DLL)
    
  ELSE(WIN32 AND NOT CYGWIN)
  
    SET(QWT_LIBRARIES ${QWT_LIBRARY} CACHE DOC "QWT library files")
    
  ENDIF(WIN32 AND NOT CYGWIN)
  
  SET(QWT_FOUND "YES")

  IF (CYGWIN)
    IF(BUILD_SHARED_LIBS)
    # No need to define QWT_USE_DLL here, because it's default for Cygwin.
    ELSE(BUILD_SHARED_LIBS)
    SET (QWT_DEFINITIONS -DQWT_STATIC)
    ENDIF(BUILD_SHARED_LIBS)
  ENDIF (CYGWIN)

ENDIF (QWT_LIBRARY)

# handle the QUIETLY and REQUIRED arguments
INCLUDE( FindPackageHandleStandardArgs )
IF( CMAKE_VERSION LESS 2.8.3 )
  FIND_PACKAGE_HANDLE_STANDARD_ARGS( Qwt DEFAULT_MSG QWT_LIBRARY QWT_INCLUDE_DIR _QWT_VERSION_MATCH )
ELSE()
  FIND_PACKAGE_HANDLE_STANDARD_ARGS( Qwt REQUIRED_VARS QWT_LIBRARY QWT_INCLUDE_DIR _QWT_VERSION_MATCH VERSION_VAR QWT_VERSION_STRING )
ENDIF()

MARK_AS_ADVANCED(QWT_INCLUDE_DIR QWT_LIBRARY)

