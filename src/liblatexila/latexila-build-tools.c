/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2014 - Sébastien Wilmet <swilmet@gnome.org>
 *
 * GNOME LaTeX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNOME LaTeX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNOME LaTeX.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:build-tools
 * @title: LatexilaBuildTools
 * @short_description: Build tools base class
 *
 * Base class for the build tools. The build tools are stored in an XML file.
 * The XML file contents is loaded into data structures in memory.
 * There are two subclasses: #LatexilaBuildToolsDefault and
 * #LatexilaBuildToolsPersonal. The default build tools and personal build tools
 * have a different behavior. A personal build tool can be modified for example,
 * while a default build tool can only be enabled or disabled. That's why
 * subclasses exist.
 */

#include "config.h"
#include "latexila-build-tools.h"
#include <glib/gi18n.h>
#include "latexila-build-tool.h"
#include "latexila-build-job.h"
#include "latexila-post-processor.h"

struct _LatexilaBuildToolsPrivate
{
	/* Used during the XML file parsing to load the build tools. */
	LatexilaBuildTool *cur_tool;
	LatexilaBuildJob *cur_job;
};

enum
{
	SIGNAL_LOADED,
	SIGNAL_MODIFIED,
	LAST_SIGNAL
};

G_DEFINE_TYPE_WITH_PRIVATE (LatexilaBuildTools, latexila_build_tools, G_TYPE_OBJECT)

static guint signals[LAST_SIGNAL];

static void
latexila_build_tools_handle_not_found_error_default (LatexilaBuildTools *build_tools,
						     GFile              *xml_file,
						     GError             *error)
{
	/* Do nothing. If it is an error that the XML file doesn't exist, override
	 * this vfunc to display an error message.
	 * Another way would have been to add a property xml_file_must_exist or
	 * something like that.
	 * A class should not be aware of its subclasses, so explicit case analysis on
	 * the type of the object is not a solution.
	 */
}

static void
latexila_build_tools_dispose (GObject *object)
{
	LatexilaBuildTools *build_tools = LATEXILA_BUILD_TOOLS (object);

	g_list_free_full (build_tools->build_tools, g_object_unref);
	build_tools->build_tools = NULL;

	g_clear_object (&build_tools->priv->cur_tool);
	g_clear_object (&build_tools->priv->cur_job);

	G_OBJECT_CLASS (latexila_build_tools_parent_class)->dispose (object);
}

static void
latexila_build_tools_class_init (LatexilaBuildToolsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = latexila_build_tools_dispose;

	klass->handle_not_found_error = latexila_build_tools_handle_not_found_error_default;

	/**
	 * LatexilaBuildTools::loaded:
	 * @build_tools: a #LatexilaBuildTools object.
	 *
	 * The ::loaded signal is emitted when the build tools are fully loaded.
	 */
	signals[SIGNAL_LOADED] = g_signal_new ("loaded",
					       LATEXILA_TYPE_BUILD_TOOLS,
					       G_SIGNAL_RUN_LAST,
					       0, NULL, NULL, NULL,
					       G_TYPE_NONE, 0);
	/**
	 * LatexilaBuildTools::modified:
	 * @build_tools: a #LatexilaBuildTools object.
	 *
	 * The ::modified signal is emitted when a build tool is modified.
	 */
	signals[SIGNAL_MODIFIED] = g_signal_new ("modified",
						 LATEXILA_TYPE_BUILD_TOOLS,
						 G_SIGNAL_RUN_LAST,
						 0, NULL, NULL, NULL,
						 G_TYPE_NONE, 0);
}

static void
latexila_build_tools_init (LatexilaBuildTools *build_tools)
{
	build_tools->priv = latexila_build_tools_get_instance_private (build_tools);
}

static void
parser_start_element (GMarkupParseContext  *context,
		      const gchar          *element_name,
		      const gchar         **attribute_names,
		      const gchar         **attribute_values,
		      gpointer              user_data,
		      GError              **error)
{
	LatexilaBuildTools *build_tools = user_data;

	if (g_str_equal (element_name, "tools") ||
	    g_str_equal (element_name, "label") ||
	    g_str_equal (element_name, "description") ||
	    g_str_equal (element_name, "open"))
	{
		/* do nothing */
	}
	else if (g_str_equal (element_name, "tool"))
	{
		LatexilaBuildTool *cur_tool;
		gint i;

		g_clear_object (&build_tools->priv->cur_tool);
		cur_tool = latexila_build_tool_new ();
		build_tools->priv->cur_tool = cur_tool;

		for (i = 0; attribute_names[i] != NULL; i++)
		{
			if (g_str_equal (attribute_names[i], "id"))
			{
				gint id = g_strtod (attribute_values[i], NULL);
				g_object_set (cur_tool, "id", id, NULL);
			}
			/* "show" was the previous name of "enabled" */
			else if (g_str_equal (attribute_names[i], "show") ||
				 g_str_equal (attribute_names[i], "enabled"))
			{
				gboolean enabled = g_str_equal (attribute_values[i], "true");
				g_object_set (cur_tool, "enabled", enabled, NULL);
			}
			else if (g_str_equal (attribute_names[i], "extensions"))
			{
				g_object_set (cur_tool, "extensions", attribute_values[i], NULL);
			}
			else if (g_str_equal (attribute_names[i], "icon"))
			{
				/* Migrate from GtkStock to icon-names. */
				if (g_str_equal (attribute_values[i], "gtk-execute"))
				{
					g_object_set (cur_tool, "icon", "system-run", NULL);
				}
				else if (g_str_equal (attribute_values[i], "gtk-file"))
				{
					g_object_set (cur_tool, "icon", "text-x-generic", NULL);
				}
				/* Migrate from old icon names specific to latexila. */
				else if (g_str_equal (attribute_values[i], "view_dvi"))
				{
					g_object_set (cur_tool, "icon", "latexila-dvi", NULL);
				}
				else if (g_str_equal (attribute_values[i], "view_pdf"))
				{
					g_object_set (cur_tool, "icon", "latexila-pdf", NULL);
				}
				else if (g_str_equal (attribute_values[i], "view_ps"))
				{
					g_object_set (cur_tool, "icon", "latexila-ps", NULL);
				}
				else
				{
					g_object_set (cur_tool, "icon", attribute_values[i], NULL);
				}
			}
			else if (error != NULL)
			{
				*error = g_error_new (G_MARKUP_ERROR,
						      G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE,
						      "unknown attribute \"%s\"",
						      attribute_names[i]);
			}
		}
	}
	else if (g_str_equal (element_name, "job"))
	{
		LatexilaBuildJob *cur_job;
		gint i;

		g_clear_object (&build_tools->priv->cur_job);
		cur_job = latexila_build_job_new ();
		build_tools->priv->cur_job = cur_job;

		for (i = 0; attribute_names[i] != NULL; i++)
		{
			if (g_str_equal (attribute_names[i], "postProcessor"))
			{
				LatexilaPostProcessorType type;

				if (latexila_post_processor_get_type_from_name (attribute_values[i], &type))
				{
					g_object_set (cur_job, "post-processor-type", type, NULL);
				}
				else if (error != NULL)
				{
					*error = g_error_new (G_MARKUP_ERROR,
							      G_MARKUP_ERROR_INVALID_CONTENT,
							      "unknown post processor \"%s\"",
							      attribute_values[i]);
				}
			}
			/* For compatibility (no longer used) */
			else if (g_str_equal (attribute_names[i], "mustSucceed"))
			{
			}
			else if (error != NULL)
			{
				*error = g_error_new (G_MARKUP_ERROR,
						      G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE,
						      "unknown attribute \"%s\"",
						      attribute_names[i]);
			}
		}
	}
	else if (error != NULL)
	{
		*error = g_error_new (G_MARKUP_ERROR,
				      G_MARKUP_ERROR_UNKNOWN_ELEMENT,
				      "unknown element \"%s\"",
				      element_name);
	}
}

static void
parser_end_element (GMarkupParseContext  *context,
		    const gchar          *element_name,
		    gpointer              user_data,
		    GError              **error)
{
	LatexilaBuildTools *build_tools = user_data;

	if (g_str_equal (element_name, "tools") ||
	    g_str_equal (element_name, "label") ||
	    g_str_equal (element_name, "description") ||
	    g_str_equal (element_name, "open"))
	{
		/* do nothing */
	}
	else if (g_str_equal (element_name, "tool"))
	{
		build_tools->build_tools = g_list_prepend (build_tools->build_tools,
							   build_tools->priv->cur_tool);
		build_tools->priv->cur_tool = NULL;
	}
	else if (g_str_equal (element_name, "job"))
	{
		latexila_build_tool_add_job (build_tools->priv->cur_tool,
					     build_tools->priv->cur_job);
		build_tools->priv->cur_job = NULL;
	}
	else if (error != NULL)
	{
		*error = g_error_new (G_MARKUP_ERROR,
				      G_MARKUP_ERROR_UNKNOWN_ELEMENT,
				      "unknown element \"%s\"",
				      element_name);
	}
}

static void
parser_text (GMarkupParseContext  *context,
	     const gchar          *text,
	     gsize                 text_len,
	     gpointer              user_data,
	     GError              **error)
{
	LatexilaBuildTools *build_tools = user_data;
	const gchar *element_name = g_markup_parse_context_get_element (context);
	gchar *stripped_text = g_strndup (text, text_len);

	stripped_text = g_strstrip (stripped_text);

	if (g_str_equal (element_name, "job"))
	{
		g_object_set (build_tools->priv->cur_job, "command", stripped_text, NULL);
	}
	else if (g_str_equal (element_name, "label"))
	{
		g_object_set (build_tools->priv->cur_tool, "label", _ (stripped_text), NULL);
	}
	else if (g_str_equal (element_name, "description"))
	{
		g_object_set (build_tools->priv->cur_tool, "description", _ (stripped_text), NULL);
	}
	else if (g_str_equal (element_name, "open"))
	{
		g_object_set (build_tools->priv->cur_tool, "files-to-open", stripped_text, NULL);
	}

	g_free (stripped_text);
}

static void
parse_contents (LatexilaBuildTools *build_tools,
		gchar              *contents)
{
	GMarkupParser parser;
	GMarkupParseContext *context;
	GError *error = NULL;

	parser.start_element = parser_start_element;
	parser.end_element = parser_end_element;
	parser.text = parser_text;
	parser.passthrough = NULL;
	parser.error = NULL;

	context = g_markup_parse_context_new (&parser, 0, build_tools, NULL);

	g_markup_parse_context_parse (context, contents, -1, &error);

	if (error != NULL)
	{
		g_warning ("Error while parsing build tools: %s", error->message);
		g_error_free (error);
		error = NULL;
		goto out;
	}

	g_markup_parse_context_end_parse (context, &error);

	if (error != NULL)
	{
		g_warning ("Error while ending build tools parser: %s", error->message);
		g_error_free (error);
		error = NULL;
		goto out;
	}

out:
	build_tools->build_tools = g_list_reverse (build_tools->build_tools);

	g_markup_parse_context_free (context);
	g_free (contents);

	g_signal_emit (build_tools, signals[SIGNAL_LOADED], 0);
}

static void
load_contents_cb (GFile              *xml_file,
		  GAsyncResult       *result,
		  LatexilaBuildTools *build_tools)
{
	gchar *contents = NULL;
	GError *error = NULL;

	g_file_load_contents_finish (xml_file, result, &contents, NULL, NULL, &error);

	if (error != NULL)
	{
		if (error->domain == G_IO_ERROR &&
		    error->code == G_IO_ERROR_NOT_FOUND)
		{
			LATEXILA_BUILD_TOOLS_GET_CLASS (build_tools)->handle_not_found_error (build_tools,
											      xml_file,
											      error);
		}
		else
		{
			g_warning ("Error while loading the contents of the build tools XML file: %s",
				   error->message);
		}

		g_error_free (error);
	}

	if (contents != NULL)
	{
		parse_contents (build_tools, contents);
	}

	g_object_unref (build_tools);
}

/**
 * latexila_build_tools_load:
 * @build_tools: a #LatexilaBuildTools object.
 * @xml_file: the XML file.
 *
 * Loads asynchronously the XML file contents and parses it.
 * This function is used by subclasses of #LatexilaBuildTools.
 * When the file is fully loaded, the #LatexilaBuildTools::loaded signal is
 * emitted.
 */
void
latexila_build_tools_load (LatexilaBuildTools *build_tools,
			   GFile              *xml_file)
{
	g_return_if_fail (LATEXILA_IS_BUILD_TOOLS (build_tools));
	g_return_if_fail (G_IS_FILE (xml_file));

	/* Avoid finalization of build_tools during the async operation. */
	g_object_ref (build_tools);

	g_file_load_contents_async (xml_file,
				    NULL,
				    (GAsyncReadyCallback) load_contents_cb,
				    build_tools);
}

/**
 * latexila_build_tools_nth:
 * @build_tools: a #LatexilaBuildTools object.
 * @tool_num: the build tool position in the list.
 *
 * Returns: (transfer none): the #LatexilaBuildTool located at @tool_num.
 */
LatexilaBuildTool *
latexila_build_tools_nth (LatexilaBuildTools *build_tools,
			  guint               tool_num)
{
	LatexilaBuildTool *build_tool;

	g_return_val_if_fail (LATEXILA_IS_BUILD_TOOLS (build_tools), NULL);

	build_tool = g_list_nth_data (build_tools->build_tools, tool_num);
	g_return_val_if_fail (build_tool != NULL, NULL);

	return build_tool;
}

/**
 * latexila_build_tools_set_enabled:
 * @build_tools: a #LatexilaBuildTools object.
 * @tool_num: the build tool position in the list.
 * @enabled: whether to enable the build tool.
 */
void
latexila_build_tools_set_enabled (LatexilaBuildTools *build_tools,
				  guint               tool_num,
				  gboolean            enabled)
{
	LatexilaBuildTool *build_tool = g_list_nth_data (build_tools->build_tools, tool_num);

	g_return_if_fail (build_tool != NULL);

	g_object_set (build_tool, "enabled", enabled, NULL);
	g_signal_emit (build_tools, signals[SIGNAL_MODIFIED], 0);
}
