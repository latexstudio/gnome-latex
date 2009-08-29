#ifndef CONFIG_H
#define CONFIG_H

#define PROGRAM_NAME "LaTeXila"
#define PROGRAM_VERSION "@latexila_VERSION@"

#cmakedefine LATEXILA_NLS_ENABLED
#cmakedefine LATEXILA_NLS_PACKAGE "@LATEXILA_NLS_PACKAGE@"
#cmakedefine LATEXILA_NLS_LOCALEDIR "@LATEXILA_NLS_LOCALEDIR@"

#define DATA_DIR "${DATA_DIR}"

#define FONT "Monospace"

#endif /* CONFIG_H */
