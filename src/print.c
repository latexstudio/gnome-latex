/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2009, 2010 Sébastien Wilmet
 *
 * LaTeXila is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LaTeXila is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LaTeXila.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "print.h"

void
print_info (const char *format, ...)
{
	va_list va;
	va_start (va, format);
	vprintf (format, va);
	printf ("\n");
}

void
print_warning (const char *format, ...)
{
	va_list va;
	va_start (va, format);
	fprintf (stderr, "Warning: ");
	vfprintf (stderr, format, va);
	fprintf (stderr, "\n");
}

void
print_error (const char *format, ...)
{
	va_list va;
	va_start (va, format);
	fprintf (stderr, "Error: ");
	vfprintf (stderr, format, va);
	fprintf (stderr, "\n");
}
