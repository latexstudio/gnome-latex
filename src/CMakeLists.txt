SET(latexila_src main.c 	main.h
	         callbacks.c	callbacks.h
		 print.c	print.h
		 symbols.c	symbols.h
		 actions.c	actions.h
		 prefs.c	prefs.h
		 ui.c		ui.h)
ADD_EXECUTABLE(latexila ${latexila_src})
TARGET_LINK_LIBRARIES(latexila ${GTK2_LIBRARIES})

INSTALL(TARGETS latexila DESTINATION "bin")
INSTALL(FILES ui.xml DESTINATION "share/latexila")
