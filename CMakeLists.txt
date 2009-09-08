CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(latexila)

# version
SET(latexila_MAJOR 0)
SET(latexila_MINOR 0)
SET(latexila_PATCH "1.99")
SET(latexila_VERSION ${latexila_MAJOR}.${latexila_MINOR}.${latexila_PATCH})

MESSAGE(STATUS "*** Building LaTeXila ${latexila_VERSION} ***")

IF(NOT DATA_DIR)
  SET(DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/latexila" CACHE PATH "The directory where the data is")
ENDIF(NOT DATA_DIR)

OPTION(ENABLE_NLS "Native Language Support" ON)

IF(ENABLE_NLS)
  SET(LATEXILA_NLS_ENABLED TRUE)
  SET(LATEXILA_NLS_PACKAGE "latexila")
  SET(LATEXILA_NLS_LOCALEDIR "${CMAKE_INSTALL_PREFIX}/share/locale")
  MESSAGE(STATUS "Native language support enabled.")
  FIND_PACKAGE(Gettext REQUIRED)
  FILE(GLOB latexila_PO_FILES ${latexila_SOURCE_DIR}/po/*.po)
  GETTEXT_CREATE_TRANSLATIONS(po/latexila.pot ${latexila_PO_FILES})
  # with ALL it doesn't work, wait the new version of FindGettext module...
  # so we must run "make translations"
  #GETTEXT_CREATE_TRANSLATIONS(po/latexila.pot ALL ${latexila_PO_FILES})
ELSE(ENABLE_NLS)
  MESSAGE(STATUS "Native language support disabled.")
ENDIF(ENABLE_NLS)

SET(CMAKE_C_FLAGS "-Wall -std=c99 -DGTK_DISABLE_DEPRECATED=1")

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK2 REQUIRED gtk+-2.0 gtksourceview-2.0>=2.4)
SET(GTKSOURCEVIEW_VERSION ${GTK2_gtksourceview-2.0_VERSION})

INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIRS} ${latexila_BINARY_DIR}/src)

CONFIGURE_FILE(${latexila_SOURCE_DIR}/src/config.h.cmake ${latexila_BINARY_DIR}/src/config.h)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(images)

# packages
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_PACKAGE_VERSION_MAJOR ${latexila_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${latexila_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${latexila_PATCH})
SET(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME latexila-${latexila_VERSION})
SET(CPACK_SOURCE_IGNORE_FILES "~$" ".gz$")

INCLUDE(CPack)
