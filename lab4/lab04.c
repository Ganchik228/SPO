#include "lab04_sqlite.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

#define UI_FILE "lab04.glade"

enum
{
    EMPLOYEE_ID = 0,
    FIO,
    DR,
    GENDER,
    POST_ID,
    POST_NAME,
    ORGA_ID,
    ORGA_NAME,
    ORGA_INN // Добавленный столбец
};

struct MainWindowObjects
{
    GtkWindow *main_window;
    GtkTreeView *treeview;
    GtkListStore *liststore;
    GtkAdjustment *adjustment;
    GtkTreeViewColumn *column_employee_id;
    GtkTreeViewColumn *column_post_name;
    GtkTreeViewColumn *column_orga_name;
    GtkTreeViewColumn *column_orga_inn; // Новый столбец
    GtkTreeViewColumn *column_post_id;
    GtkTreeViewColumn *column_orga_id;
} mainWindowObjects;

int callback(void *not_used, int argc, char **argv, char **col_names)
{
    GtkTreeIter iter;
    if (argc == 9)
    {
        gtk_list_store_append(GTK_LIST_STORE(mainWindowObjects.liststore), &iter);
        gtk_list_store_set(GTK_LIST_STORE(mainWindowObjects.liststore), &iter, EMPLOYEE_ID,
                           atoi(argv[EMPLOYEE_ID]), FIO, argv[FIO], DR, argv[DR],
                           GENDER, argv[GENDER], POST_ID, atoi(argv[POST_ID]),
                           POST_NAME, argv[POST_NAME], ORGA_ID, atoi(argv[ORGA_ID]),
                           ORGA_NAME, argv[ORGA_NAME], ORGA_INN, argv[ORGA_INN], -1); // Добавление ORGA_INN
    }
    return 0;
}

int main(int argc, char **argv)
{
    GtkBuilder *builder;
    GError *error = NULL;
    gtk_init(&argc, &argv);

    builder = gtk_builder_new();

    if (!gtk_builder_add_from_file(builder, UI_FILE, &error))
    {
        g_warning("%s\n", error->message);
        g_free(error);
        return (1);
    }

    mainWindowObjects.main_window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    mainWindowObjects.treeview =
        GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview_employees"));
    mainWindowObjects.liststore =
        GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore_employees"));
    mainWindowObjects.adjustment =
        GTK_ADJUSTMENT(gtk_builder_get_object(builder, "adjustment"));

    mainWindowObjects.column_employee_id =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "column_employee_id"));
    mainWindowObjects.column_post_name =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "column_post_name"));
    mainWindowObjects.column_orga_name =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "column_orga_name"));
    mainWindowObjects.column_orga_inn = // Создание нового столбца
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "column_orga_inn")); 
    mainWindowObjects.column_post_id =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "column_post_id"));
    mainWindowObjects.column_orga_id =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "column_orga_id"));

    gtk_builder_connect_signals(builder, &mainWindowObjects);

    g_object_unref(G_OBJECT(builder));
    gtk_widget_show_all(GTK_WIDGET(mainWindowObjects.main_window));

    sqlite_get_data();

    gtk_main();
}

G_MODULE_EXPORT void on_btnsave_clicked(GtkWidget *button, gpointer data)
{
    GtkTreeIter iter;
    gboolean reader =
        gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mainWindowObjects.liststore), &iter);
    while (reader)
    {
        gint employee_id, post_id, orga_id;
        gchar *fio, *dr, *gender;
        gtk_tree_model_get(GTK_TREE_MODEL(mainWindowObjects.liststore), &iter, EMPLOYEE_ID, &employee_id,
                           FIO, &fio, DR, &dr, GENDER, &gender, POST_ID, &post_id,
                           ORGA_ID, &orga_id, -1);
        sqlite_update(employee_id, fio, dr, gender, post_id, orga_id);
        reader = gtk_tree_model_iter_next(GTK_TREE_MODEL(mainWindowObjects.liststore), &iter);
    }
    gtk_list_store_clear(mainWindowObjects.liststore);
    sqlite_get_data();
}

G_MODULE_EXPORT void on_show_hidden_toggled(GtkToggleButton *button, gpointer data)
{
    gboolean visible = gtk_toggle_button_get_active(button);
    gtk_tree_view_column_set_visible(mainWindowObjects.column_employee_id, visible);
    gtk_tree_view_column_set_visible(mainWindowObjects.column_orga_id, visible);
    gtk_tree_view_column_set_visible(mainWindowObjects.column_post_id, visible);
}

G_MODULE_EXPORT void on_cellrenderertext_fio_edited(GtkCellRendererText *renderer, gchar *path,
                                                         gchar *new_text, gpointer data)
{
    if (g_ascii_strcasecmp(new_text, "") != 0)
    {
        GtkTreeIter iter;
        GtkTreeModel *model;
        model = gtk_tree_view_get_model(mainWindowObjects.treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path))
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, FIO, new_text, -1);
    }
}

G_MODULE_EXPORT void on_cellrenderertext_dr_edited(GtkCellRendererText *renderer, gchar *path,
                                                      gchar *new_text, gpointer data)
{
    if (g_ascii_strcasecmp(new_text, "") != 0)
    {
        GtkTreeIter iter;
        GtkTreeModel *model;
        model = gtk_tree_view_get_model(mainWindowObjects.treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path))
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, DR, new_text, -1);
    }
}

G_MODULE_EXPORT void on_cellrenderertext_gender_edited(GtkCellRendererText *renderer, gchar *path,
                                                      gchar *new_text, gpointer data)
{
    if (g_ascii_strcasecmp(new_text, "") != 0)
    {
        GtkTreeIter iter;
        GtkTreeModel *model;
        model = gtk_tree_view_get_model(mainWindowObjects.treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path))
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, GENDER, new_text, -1);
    }
}

G_MODULE_EXPORT void on_btnabout_clicked(GtkButton *button, gpointer data)
{
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "О программе", mainWindowObjects.main_window,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_OK", GTK_RESPONSE_NONE, NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 15);
    GtkWidget *label = gtk_label_new("\nСамая лучшая база данных\n");
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show(label);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

G_MODULE_EXPORT void on_window_destroy(GtkWidget *window, gpointer data)
{
    gtk_main_quit();
}

G_MODULE_EXPORT void on_btnexit_clicked(GtkWidget *window, gpointer data)
{
    gtk_main_quit();
}
