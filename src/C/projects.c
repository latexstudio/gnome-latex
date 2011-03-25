/* projects.c generated by valac 0.10.3, the Vala compiler
 * generated from projects.vala, do not modify */

/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2010-2011 Sébastien Wilmet
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

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <gee.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gobject/gvaluecollector.h>


#define TYPE_PROJECT (project_get_type ())
typedef struct _Project Project;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))

#define TYPE_PROJECTS (projects_get_type ())
#define PROJECTS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_PROJECTS, Projects))
#define PROJECTS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_PROJECTS, ProjectsClass))
#define IS_PROJECTS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_PROJECTS))
#define IS_PROJECTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_PROJECTS))
#define PROJECTS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_PROJECTS, ProjectsClass))

typedef struct _Projects Projects;
typedef struct _ProjectsClass ProjectsClass;
typedef struct _ProjectsPrivate ProjectsPrivate;
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_markup_parse_context_free0(var) ((var == NULL) ? NULL : (var = (g_markup_parse_context_free (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _projects_unref0(var) ((var == NULL) ? NULL : (var = (projects_unref (var), NULL)))

#define TYPE_APPLICATION (application_get_type ())
#define APPLICATION(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_APPLICATION, Application))
#define APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_APPLICATION, ApplicationClass))
#define IS_APPLICATION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_APPLICATION))
#define IS_APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_APPLICATION))
#define APPLICATION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_APPLICATION, ApplicationClass))

typedef struct _Application Application;
typedef struct _ApplicationClass ApplicationClass;

#define TYPE_MAIN_WINDOW (main_window_get_type ())
#define MAIN_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_MAIN_WINDOW, MainWindow))
#define MAIN_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_MAIN_WINDOW, MainWindowClass))
#define IS_MAIN_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_MAIN_WINDOW))
#define IS_MAIN_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_MAIN_WINDOW))
#define MAIN_WINDOW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_MAIN_WINDOW, MainWindowClass))

typedef struct _MainWindow MainWindow;
typedef struct _MainWindowClass MainWindowClass;
#define _project_free0(var) ((var == NULL) ? NULL : (var = (project_free (var), NULL)))

#define TYPE_DOCUMENT (document_get_type ())
#define DOCUMENT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_DOCUMENT, Document))
#define DOCUMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_DOCUMENT, DocumentClass))
#define IS_DOCUMENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_DOCUMENT))
#define IS_DOCUMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_DOCUMENT))
#define DOCUMENT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_DOCUMENT, DocumentClass))

typedef struct _Document Document;
typedef struct _DocumentClass DocumentClass;
#define __g_list_free_g_object_unref0(var) ((var == NULL) ? NULL : (var = (_g_list_free_g_object_unref (var), NULL)))
typedef struct _ParamSpecProjects ParamSpecProjects;

struct _Project {
	GFile* directory;
	GFile* main_file;
};

struct _Projects {
	GTypeInstance parent_instance;
	volatile int ref_count;
	ProjectsPrivate * priv;
};

struct _ProjectsClass {
	GTypeClass parent_class;
	void (*finalize) (Projects *self);
};

struct _ProjectsPrivate {
	GeeLinkedList* projects;
	gboolean modified;
};

struct _ParamSpecProjects {
	GParamSpec parent_instance;
};


static Projects* projects_instance;
static Projects* projects_instance = NULL;
static gpointer projects_parent_class = NULL;

GType project_get_type (void) G_GNUC_CONST;
Project* project_dup (const Project* self);
void project_free (Project* self);
void project_copy (const Project* self, Project* dest);
void project_destroy (Project* self);
gpointer projects_ref (gpointer instance);
void projects_unref (gpointer instance);
GParamSpec* param_spec_projects (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void value_set_projects (GValue* value, gpointer v_object);
void value_take_projects (GValue* value, gpointer v_object);
gpointer value_get_projects (const GValue* value);
GType projects_get_type (void) G_GNUC_CONST;
#define PROJECTS_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_PROJECTS, ProjectsPrivate))
enum  {
	PROJECTS_DUMMY_PROPERTY
};
static Projects* projects_new (void);
static Projects* projects_construct (GType object_type);
static GFile* projects_get_xml_file (Projects* self);
static void projects_parser_start (Projects* self, GMarkupParseContext* context, const char* name, char** attr_names, int attr_names_length1, char** attr_values, int attr_values_length1, GError** error);
static void _projects_parser_start_gmarkup_parser_start_element_func (GMarkupParseContext* context, const char* element_name, char** attribute_names, char** attribute_values, gpointer self, GError** error);
static void projects_update_all_documents (Projects* self);
static void projects_update_all_menus (Projects* self);
Projects* projects_get_default (void);
Project* projects_get (Projects* self, gint id);
GeeLinkedList* projects_get_projects (Projects* self);
GType application_get_type (void) G_GNUC_CONST;
Application* application_get_default (void);
GType main_window_get_type (void) G_GNUC_CONST;
GList* application_get_windows (Application* self);
void main_window_update_config_project_sensitivity (MainWindow* self);
gboolean projects_add (Projects* self, Project* new_project, GFile** conflict_file);
static gboolean projects_conflict (Projects* self, GFile* dir1, GFile* dir2);
GType document_get_type (void) G_GNUC_CONST;
GList* application_get_documents (Application* self);
gint document_get_project_id (Document* self);
GFile* document_get_location (Document* self);
void document_set_project_id (Document* self, gint value);
static void _g_list_free_g_object_unref (GList* self);
gboolean projects_change_main_file (Projects* self, gint num, GFile* new_main_file);
void projects_delete (Projects* self, gint num);
void projects_clear_all (Projects* self);
void projects_save (Projects* self);
void utils_delete_file (GFile* file);
static void projects_finalize (Projects* obj);
static gint _vala_array_length (gpointer array);



static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


void project_copy (const Project* self, Project* dest) {
	dest->directory = _g_object_ref0 (self->directory);
	dest->main_file = _g_object_ref0 (self->main_file);
}


void project_destroy (Project* self) {
	_g_object_unref0 (self->directory);
	_g_object_unref0 (self->main_file);
}


Project* project_dup (const Project* self) {
	Project* dup;
	dup = g_new0 (Project, 1);
	project_copy (self, dup);
	return dup;
}


void project_free (Project* self) {
	project_destroy (self);
	g_free (self);
}


GType project_get_type (void) {
	static volatile gsize project_type_id__volatile = 0;
	if (g_once_init_enter (&project_type_id__volatile)) {
		GType project_type_id;
		project_type_id = g_boxed_type_register_static ("Project", (GBoxedCopyFunc) project_dup, (GBoxedFreeFunc) project_free);
		g_once_init_leave (&project_type_id__volatile, project_type_id);
	}
	return project_type_id__volatile;
}


static void _projects_parser_start_gmarkup_parser_start_element_func (GMarkupParseContext* context, const char* element_name, char** attribute_names, char** attribute_values, gpointer self, GError** error) {
	projects_parser_start (self, context, element_name, attribute_names, _vala_array_length (attribute_names), attribute_values, _vala_array_length (attribute_values), error);
}


static Projects* projects_construct (GType object_type) {
	Projects* self = (Projects*) g_type_create_instance (object_type);
	GeeLinkedList* _tmp0_;
	GFile* file;
	GError * _inner_error_ = NULL;
	self->priv->projects = (_tmp0_ = gee_linked_list_new (TYPE_PROJECT, (GBoxedCopyFunc) project_dup, project_free, NULL), _g_object_unref0 (self->priv->projects), _tmp0_);
	file = projects_get_xml_file (self);
	if (!g_file_query_exists (file, NULL)) {
		_g_object_unref0 (file);
		return self;
	}
	{
		char* contents;
		char* _tmp1_ = NULL;
		char* _tmp2_;
		GMarkupParser _tmp3_ = {0};
		GMarkupParser parser;
		GMarkupParseContext* context;
		contents = NULL;
		g_file_load_contents (file, NULL, &_tmp1_, NULL, NULL, &_inner_error_);
		contents = (_tmp2_ = _tmp1_, _g_free0 (contents), _tmp2_);
		if (_inner_error_ != NULL) {
			_g_free0 (contents);
			goto __catch23_g_error;
		}
		parser = (_tmp3_.start_element = _projects_parser_start_gmarkup_parser_start_element_func, _tmp3_.end_element = NULL, _tmp3_.text = NULL, _tmp3_.passthrough = NULL, _tmp3_.error = NULL, _tmp3_);
		context = g_markup_parse_context_new (&parser, 0, self, NULL);
		g_markup_parse_context_parse (context, contents, (gssize) (-1), &_inner_error_);
		if (_inner_error_ != NULL) {
			_g_markup_parse_context_free0 (context);
			_g_free0 (contents);
			goto __catch23_g_error;
		}
		projects_update_all_documents (self);
		projects_update_all_menus (self);
		_g_markup_parse_context_free0 (context);
		_g_free0 (contents);
	}
	goto __finally23;
	__catch23_g_error:
	{
		GError * e;
		e = _inner_error_;
		_inner_error_ = NULL;
		{
			fprintf (stderr, "Warning: impossible to load projects: %s\n", e->message);
			_g_error_free0 (e);
		}
	}
	__finally23:
	if (_inner_error_ != NULL) {
		_g_object_unref0 (file);
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return NULL;
	}
	_g_object_unref0 (file);
	return self;
}


static Projects* projects_new (void) {
	return projects_construct (TYPE_PROJECTS);
}


static gpointer _projects_ref0 (gpointer self) {
	return self ? projects_ref (self) : NULL;
}


Projects* projects_get_default (void) {
	Projects* result = NULL;
	if (projects_instance == NULL) {
		Projects* _tmp0_;
		projects_instance = (_tmp0_ = projects_new (), _projects_unref0 (projects_instance), _tmp0_);
	}
	result = _projects_ref0 (projects_instance);
	return result;
}


Project* projects_get (Projects* self, gint id) {
	Project* result = NULL;
	gboolean _tmp0_ = FALSE;
	g_return_val_if_fail (self != NULL, NULL);
	if (id >= 0) {
		_tmp0_ = id < gee_collection_get_size ((GeeCollection*) self->priv->projects);
	} else {
		_tmp0_ = FALSE;
	}
	g_return_val_if_fail (_tmp0_, NULL);
	result = (Project*) gee_abstract_list_get ((GeeAbstractList*) self->priv->projects, id);
	return result;
}


GeeLinkedList* projects_get_projects (Projects* self) {
	GeeLinkedList* result = NULL;
	g_return_val_if_fail (self != NULL, NULL);
	result = self->priv->projects;
	return result;
}


static void projects_update_all_menus (Projects* self) {
	g_return_if_fail (self != NULL);
	{
		Application* _tmp0_;
		GList* _tmp1_;
		GList* window_collection;
		GList* window_it;
		window_collection = (_tmp1_ = application_get_windows (_tmp0_ = application_get_default ()), _g_object_unref0 (_tmp0_), _tmp1_);
		for (window_it = window_collection; window_it != NULL; window_it = window_it->next) {
			MainWindow* window;
			window = _g_object_ref0 ((MainWindow*) window_it->data);
			{
				main_window_update_config_project_sensitivity (window);
				_g_object_unref0 (window);
			}
		}
	}
}


static void _g_list_free_g_object_unref (GList* self) {
	g_list_foreach (self, (GFunc) g_object_unref, NULL);
	g_list_free (self);
}


gboolean projects_add (Projects* self, Project* new_project, GFile** conflict_file) {
	gboolean result = FALSE;
	Application* _tmp4_;
	GList* _tmp5_;
	GList* docs;
	g_return_val_if_fail (self != NULL, FALSE);
	if (conflict_file != NULL) {
		*conflict_file = NULL;
	}
	{
		GeeIterator* _project_it;
		_project_it = gee_abstract_collection_iterator ((GeeAbstractCollection*) self->priv->projects);
		while (TRUE) {
			Project* _tmp0_;
			Project _tmp1_ = {0};
			Project _tmp2_;
			Project project;
			if (!gee_iterator_next (_project_it)) {
				break;
			}
			project = (_tmp2_ = (project_copy (_tmp0_ = (Project*) gee_iterator_get (_project_it), &_tmp1_), _tmp1_), _project_free0 (_tmp0_), _tmp2_);
			if (projects_conflict (self, project.directory, (*new_project).directory)) {
				GFile* _tmp3_;
				*conflict_file = (_tmp3_ = _g_object_ref0 (project.directory), _g_object_unref0 (*conflict_file), _tmp3_);
				result = FALSE;
				project_destroy (&project);
				_g_object_unref0 (_project_it);
				return result;
			}
			project_destroy (&project);
		}
		_g_object_unref0 (_project_it);
	}
	gee_abstract_collection_add ((GeeAbstractCollection*) self->priv->projects, new_project);
	self->priv->modified = TRUE;
	docs = (_tmp5_ = application_get_documents (_tmp4_ = application_get_default ()), _g_object_unref0 (_tmp4_), _tmp5_);
	{
		GList* doc_collection;
		GList* doc_it;
		doc_collection = docs;
		for (doc_it = doc_collection; doc_it != NULL; doc_it = doc_it->next) {
			Document* doc;
			doc = _g_object_ref0 ((Document*) doc_it->data);
			{
				if (document_get_project_id (doc) != (-1)) {
					_g_object_unref0 (doc);
					continue;
				}
				if (g_file_has_prefix (document_get_location (doc), (*new_project).directory)) {
					document_set_project_id (doc, gee_collection_get_size ((GeeCollection*) self->priv->projects) - 1);
				}
				_g_object_unref0 (doc);
			}
		}
	}
	projects_update_all_menus (self);
	result = TRUE;
	__g_list_free_g_object_unref0 (docs);
	return result;
}


gboolean projects_change_main_file (Projects* self, gint num, GFile* new_main_file) {
	gboolean result = FALSE;
	gboolean _tmp0_ = FALSE;
	Project* _tmp1_;
	Project _tmp2_ = {0};
	Project _tmp3_;
	Project project;
	GFile* _tmp4_;
	Application* _tmp5_;
	GList* _tmp6_;
	GList* docs;
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (new_main_file != NULL, FALSE);
	if (num >= 0) {
		_tmp0_ = num < gee_collection_get_size ((GeeCollection*) self->priv->projects);
	} else {
		_tmp0_ = FALSE;
	}
	g_return_val_if_fail (_tmp0_, FALSE);
	project = (_tmp3_ = (project_copy (_tmp1_ = (Project*) gee_abstract_list_get ((GeeAbstractList*) self->priv->projects, num), &_tmp2_), _tmp2_), _project_free0 (_tmp1_), _tmp3_);
	if (g_file_equal (new_main_file, project.main_file)) {
		result = FALSE;
		project_destroy (&project);
		return result;
	}
	g_return_if_fail (g_file_has_prefix (new_main_file, project.directory));
	project.main_file = (_tmp4_ = _g_object_ref0 (new_main_file), _g_object_unref0 (project.main_file), _tmp4_);
	gee_abstract_list_set ((GeeAbstractList*) self->priv->projects, num, &project);
	self->priv->modified = TRUE;
	docs = (_tmp6_ = application_get_documents (_tmp5_ = application_get_default ()), _g_object_unref0 (_tmp5_), _tmp6_);
	{
		GList* doc_collection;
		GList* doc_it;
		doc_collection = docs;
		for (doc_it = doc_collection; doc_it != NULL; doc_it = doc_it->next) {
			Document* doc;
			doc = _g_object_ref0 ((Document*) doc_it->data);
			{
				if (document_get_project_id (doc) == num) {
					document_set_project_id (doc, num);
				}
				_g_object_unref0 (doc);
			}
		}
	}
	result = TRUE;
	__g_list_free_g_object_unref0 (docs);
	project_destroy (&project);
	return result;
}


void projects_delete (Projects* self, gint num) {
	gboolean _tmp0_ = FALSE;
	Project* _tmp1_;
	Application* _tmp2_;
	GList* _tmp3_;
	GList* docs;
	g_return_if_fail (self != NULL);
	if (num >= 0) {
		_tmp0_ = num < gee_collection_get_size ((GeeCollection*) self->priv->projects);
	} else {
		_tmp0_ = FALSE;
	}
	g_return_if_fail (_tmp0_);
	_tmp1_ = (Project*) gee_abstract_list_remove_at ((GeeAbstractList*) self->priv->projects, num);
	_project_free0 (_tmp1_);
	self->priv->modified = TRUE;
	docs = (_tmp3_ = application_get_documents (_tmp2_ = application_get_default ()), _g_object_unref0 (_tmp2_), _tmp3_);
	{
		GList* doc_collection;
		GList* doc_it;
		doc_collection = docs;
		for (doc_it = doc_collection; doc_it != NULL; doc_it = doc_it->next) {
			Document* doc;
			doc = _g_object_ref0 ((Document*) doc_it->data);
			{
				if (document_get_project_id (doc) == num) {
					document_set_project_id (doc, -1);
				} else {
					if (document_get_project_id (doc) > num) {
						gint _tmp4_;
						_tmp4_ = document_get_project_id (doc);
						document_set_project_id (doc, _tmp4_ - 1);
						_tmp4_;
					}
				}
				_g_object_unref0 (doc);
			}
		}
	}
	projects_update_all_menus (self);
	__g_list_free_g_object_unref0 (docs);
}


void projects_clear_all (Projects* self) {
	g_return_if_fail (self != NULL);
	gee_abstract_collection_clear ((GeeAbstractCollection*) self->priv->projects);
	self->priv->modified = TRUE;
	projects_update_all_documents (self);
	projects_update_all_menus (self);
}


static void projects_update_all_documents (Projects* self) {
	Application* _tmp0_;
	GList* _tmp1_;
	GList* docs;
	g_return_if_fail (self != NULL);
	docs = (_tmp1_ = application_get_documents (_tmp0_ = application_get_default ()), _g_object_unref0 (_tmp0_), _tmp1_);
	{
		GList* doc_collection;
		GList* doc_it;
		doc_collection = docs;
		for (doc_it = doc_collection; doc_it != NULL; doc_it = doc_it->next) {
			Document* doc;
			doc = _g_object_ref0 ((Document*) doc_it->data);
			{
				document_set_project_id (doc, -1);
				{
					gint i;
					i = 0;
					{
						gboolean _tmp2_;
						_tmp2_ = TRUE;
						while (TRUE) {
							Project* _tmp3_;
							gboolean _tmp4_;
							if (!_tmp2_) {
								i++;
							}
							_tmp2_ = FALSE;
							if (!(i < gee_collection_get_size ((GeeCollection*) self->priv->projects))) {
								break;
							}
							if ((_tmp4_ = g_file_has_prefix (document_get_location (doc), (*(_tmp3_ = (Project*) gee_abstract_list_get ((GeeAbstractList*) self->priv->projects, i))).directory), _project_free0 (_tmp3_), _tmp4_)) {
								document_set_project_id (doc, i);
								break;
							}
						}
					}
				}
				_g_object_unref0 (doc);
			}
		}
	}
	__g_list_free_g_object_unref0 (docs);
}


static void projects_parser_start (Projects* self, GMarkupParseContext* context, const char* name, char** attr_names, int attr_names_length1, char** attr_values, int attr_values_length1, GError** error) {
	const char* _tmp11_;
	GQuark _tmp12_;
	static GQuark _tmp12__label0 = 0;
	static GQuark _tmp12__label1 = 0;
	GError * _inner_error_ = NULL;
	g_return_if_fail (self != NULL);
	g_return_if_fail (context != NULL);
	g_return_if_fail (name != NULL);
	_tmp11_ = name;
	_tmp12_ = (NULL == _tmp11_) ? 0 : g_quark_from_string (_tmp11_);
	if (_tmp12_ == ((0 != _tmp12__label0) ? _tmp12__label0 : (_tmp12__label0 = g_quark_from_static_string ("projects"))))
	switch (0) {
		default:
		{
			return;
		}
	} else if (_tmp12_ == ((0 != _tmp12__label1) ? _tmp12__label1 : (_tmp12__label1 = g_quark_from_static_string ("project"))))
	switch (0) {
		default:
		{
			Project project = {0};
			memset (&project, 0, sizeof (Project));
			{
				gint i;
				i = 0;
				{
					gboolean _tmp0_;
					_tmp0_ = TRUE;
					while (TRUE) {
						const char* _tmp6_;
						GQuark _tmp7_;
						static GQuark _tmp7__label0 = 0;
						static GQuark _tmp7__label1 = 0;
						if (!_tmp0_) {
							i++;
						}
						_tmp0_ = FALSE;
						if (!(i < attr_names_length1)) {
							break;
						}
						_tmp6_ = attr_names[i];
						_tmp7_ = (NULL == _tmp6_) ? 0 : g_quark_from_string (_tmp6_);
						if (_tmp7_ == ((0 != _tmp7__label0) ? _tmp7__label0 : (_tmp7__label0 = g_quark_from_static_string ("directory"))))
						switch (0) {
							default:
							{
								GFile* _tmp1_;
								project.directory = (_tmp1_ = g_file_new_for_uri (attr_values[i]), _g_object_unref0 (project.directory), _tmp1_);
								break;
							}
						} else if (_tmp7_ == ((0 != _tmp7__label1) ? _tmp7__label1 : (_tmp7__label1 = g_quark_from_static_string ("main_file"))))
						switch (0) {
							default:
							{
								GFile* _tmp2_;
								project.main_file = (_tmp2_ = g_file_new_for_uri (attr_values[i]), _g_object_unref0 (project.main_file), _tmp2_);
								break;
							}
						} else
						switch (0) {
							default:
							{
								char* _tmp3_;
								char* _tmp4_;
								GError* _tmp5_;
								_inner_error_ = (_tmp5_ = g_error_new_literal (G_MARKUP_ERROR, G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE, _tmp4_ = g_strconcat (_tmp3_ = g_strconcat ("unknown attribute \"", attr_names[i], NULL), "\"", NULL)), _g_free0 (_tmp4_), _g_free0 (_tmp3_), _tmp5_);
								{
									if (_inner_error_->domain == G_MARKUP_ERROR) {
										g_propagate_error (error, _inner_error_);
										project_destroy (&project);
										return;
									} else {
										project_destroy (&project);
										g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
										g_clear_error (&_inner_error_);
										return;
									}
								}
							}
						}
					}
				}
			}
			gee_abstract_collection_add ((GeeAbstractCollection*) self->priv->projects, &project);
			project_destroy (&project);
			break;
		}
	} else
	switch (0) {
		default:
		{
			char* _tmp8_;
			char* _tmp9_;
			GError* _tmp10_;
			_inner_error_ = (_tmp10_ = g_error_new_literal (G_MARKUP_ERROR, G_MARKUP_ERROR_UNKNOWN_ELEMENT, _tmp9_ = g_strconcat (_tmp8_ = g_strconcat ("unknown element \"", name, NULL), "\"", NULL)), _g_free0 (_tmp9_), _g_free0 (_tmp8_), _tmp10_);
			{
				if (_inner_error_->domain == G_MARKUP_ERROR) {
					g_propagate_error (error, _inner_error_);
					return;
				} else {
					g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
					g_clear_error (&_inner_error_);
					return;
				}
			}
		}
	}
}


static GFile* projects_get_xml_file (Projects* self) {
	GFile* result = NULL;
	char* path;
	g_return_val_if_fail (self != NULL, NULL);
	path = g_build_filename (g_get_user_data_dir (), "latexila", "projects.xml", NULL, NULL);
	result = g_file_new_for_path (path);
	_g_free0 (path);
	return result;
}


void projects_save (Projects* self) {
	GFile* file;
	char* content;
	char* _tmp7_;
	GError * _inner_error_ = NULL;
	g_return_if_fail (self != NULL);
	if (!self->priv->modified) {
		return;
	}
	file = projects_get_xml_file (self);
	if (gee_collection_get_size ((GeeCollection*) self->priv->projects) == 0) {
		utils_delete_file (file);
		_g_object_unref0 (file);
		return;
	}
	content = g_strdup ("<projects>\n");
	{
		GeeIterator* _project_it;
		_project_it = gee_abstract_collection_iterator ((GeeAbstractCollection*) self->priv->projects);
		while (TRUE) {
			Project* _tmp0_;
			Project _tmp1_ = {0};
			Project _tmp2_;
			Project project;
			char* _tmp3_;
			char* _tmp4_;
			char* _tmp5_;
			char* _tmp6_;
			if (!gee_iterator_next (_project_it)) {
				break;
			}
			project = (_tmp2_ = (project_copy (_tmp0_ = (Project*) gee_iterator_get (_project_it), &_tmp1_), _tmp1_), _project_free0 (_tmp0_), _tmp2_);
			content = (_tmp6_ = g_strconcat (content, _tmp5_ = g_strdup_printf ("  <project directory=\"%s\" main_file=\"%s\" />\n", _tmp3_ = g_file_get_uri (project.directory), _tmp4_ = g_file_get_uri (project.main_file)), NULL), _g_free0 (content), _tmp6_);
			_g_free0 (_tmp5_);
			_g_free0 (_tmp4_);
			_g_free0 (_tmp3_);
			project_destroy (&project);
		}
		_g_object_unref0 (_project_it);
	}
	content = (_tmp7_ = g_strconcat (content, "</projects>\n", NULL), _g_free0 (content), _tmp7_);
	{
		GFile* parent;
		gboolean _tmp8_ = FALSE;
		parent = g_file_get_parent (file);
		if (parent != NULL) {
			_tmp8_ = !g_file_query_exists (parent, NULL);
		} else {
			_tmp8_ = FALSE;
		}
		if (_tmp8_) {
			g_file_make_directory_with_parents (parent, NULL, &_inner_error_);
			if (_inner_error_ != NULL) {
				_g_object_unref0 (parent);
				goto __catch24_g_error;
			}
		}
		g_file_replace_contents (file, content, strlen (content), NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, &_inner_error_);
		if (_inner_error_ != NULL) {
			_g_object_unref0 (parent);
			goto __catch24_g_error;
		}
		_g_object_unref0 (parent);
	}
	goto __finally24;
	__catch24_g_error:
	{
		GError * e;
		e = _inner_error_;
		_inner_error_ = NULL;
		{
			fprintf (stderr, "Warning: impossible to save projects: %s\n", e->message);
			_g_error_free0 (e);
		}
	}
	__finally24:
	if (_inner_error_ != NULL) {
		_g_free0 (content);
		_g_object_unref0 (file);
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return;
	}
	_g_free0 (content);
	_g_object_unref0 (file);
}


static gboolean projects_conflict (Projects* self, GFile* dir1, GFile* dir2) {
	gboolean result = FALSE;
	gboolean _tmp0_ = FALSE;
	gboolean _tmp1_ = FALSE;
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (dir1 != NULL, FALSE);
	g_return_val_if_fail (dir2 != NULL, FALSE);
	if (g_file_has_prefix (dir1, dir2)) {
		_tmp1_ = TRUE;
	} else {
		_tmp1_ = g_file_has_prefix (dir2, dir1);
	}
	if (_tmp1_) {
		_tmp0_ = TRUE;
	} else {
		_tmp0_ = g_file_equal (dir1, dir2);
	}
	result = _tmp0_;
	return result;
}


static void value_projects_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void value_projects_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		projects_unref (value->data[0].v_pointer);
	}
}


static void value_projects_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = projects_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer value_projects_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* value_projects_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		Projects* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = projects_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* value_projects_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	Projects** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags & G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = projects_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* param_spec_projects (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	ParamSpecProjects* spec;
	g_return_val_if_fail (g_type_is_a (object_type, TYPE_PROJECTS), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer value_get_projects (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TYPE_PROJECTS), NULL);
	return value->data[0].v_pointer;
}


void value_set_projects (GValue* value, gpointer v_object) {
	Projects* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TYPE_PROJECTS));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, TYPE_PROJECTS));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		projects_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		projects_unref (old);
	}
}


void value_take_projects (GValue* value, gpointer v_object) {
	Projects* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TYPE_PROJECTS));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, TYPE_PROJECTS));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		projects_unref (old);
	}
}


static void projects_class_init (ProjectsClass * klass) {
	projects_parent_class = g_type_class_peek_parent (klass);
	PROJECTS_CLASS (klass)->finalize = projects_finalize;
	g_type_class_add_private (klass, sizeof (ProjectsPrivate));
}


static void projects_instance_init (Projects * self) {
	self->priv = PROJECTS_GET_PRIVATE (self);
	self->priv->modified = FALSE;
	self->ref_count = 1;
}


static void projects_finalize (Projects* obj) {
	Projects * self;
	self = PROJECTS (obj);
	_g_object_unref0 (self->priv->projects);
}


GType projects_get_type (void) {
	static volatile gsize projects_type_id__volatile = 0;
	if (g_once_init_enter (&projects_type_id__volatile)) {
		static const GTypeValueTable g_define_type_value_table = { value_projects_init, value_projects_free_value, value_projects_copy_value, value_projects_peek_pointer, "p", value_projects_collect_value, "p", value_projects_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (ProjectsClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) projects_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (Projects), 0, (GInstanceInitFunc) projects_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		GType projects_type_id;
		projects_type_id = g_type_register_fundamental (g_type_fundamental_next (), "Projects", &g_define_type_info, &g_define_type_fundamental_info, 0);
		g_once_init_leave (&projects_type_id__volatile, projects_type_id);
	}
	return projects_type_id__volatile;
}


gpointer projects_ref (gpointer instance) {
	Projects* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void projects_unref (gpointer instance) {
	Projects* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		PROJECTS_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
	}
}


static gint _vala_array_length (gpointer array) {
	int length;
	length = 0;
	if (array) {
		while (((gpointer*) array)[length]) {
			length++;
		}
	}
	return length;
}




