ADD_SUBDIRECTORY(templates)
ADD_SUBDIRECTORY(images)

INSTALL(FILES "latexhelp.html" DESTINATION ${DATA_DIR})
INSTALL(FILES "latexila.desktop" DESTINATION share/applications)
