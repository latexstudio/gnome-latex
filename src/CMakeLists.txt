SET(latexila_src
	main.c			main.h
	callbacks.c		callbacks.h
	cb_latex.c		cb_latex.h
	print.c			print.h
	symbols.c		symbols.h
	external_commands.c	external_commands.h
	latex_output_filter.c	latex_output_filter.h
	prefs.c			prefs.h
	ui.c			ui.h
	tool_menu_action.c	tool_menu_action.h
	file_browser.c		file_browser.h
	templates.c		templates.h
	utils.c			utils.h
	log.c			log.h
)

ADD_EXECUTABLE(latexila ${latexila_src})
TARGET_LINK_LIBRARIES(latexila ${GTK2_LIBRARIES})

INSTALL(TARGETS latexila DESTINATION "bin")
