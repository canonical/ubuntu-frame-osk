#include <stdio.h>
#include <assert.h>
#define GNOME_DESKTOP_USE_UNSTABLE_API
#include "libgnome-desktop/gnome-xkb-info.h"

#define LAYOUT_LIST_PATH_F "%s/etc/layouts.txt", getenv("SNAP")

char* layout_list_path() {
    ssize_t path_len = snprintf(NULL, 0, LAYOUT_LIST_PATH_F);
    char* path_buf = malloc(path_len + 1);
    assert(path_buf);
    snprintf(path_buf, path_len + 1, LAYOUT_LIST_PATH_F);
    return path_buf;
}

char* read_file(const char* path) {
    FILE* fp = fopen(path, "r");
    if (!fp) {
        return NULL;
    }
    assert(fseek(fp, 0, SEEK_END) == 0);
    long int file_len = ftell(fp);
    assert(file_len >= 0);
    assert(fseek(fp, 0, SEEK_SET) == 0);
    char* file_buf = malloc(file_len + 1);
    assert(file_buf);
    int read_len = fread(file_buf, sizeof(char), file_len, fp);
    assert(ferror(fp) == 0);
    fclose(fp);
    assert(read_len <= file_len);
    file_buf[read_len] = '\0';
    return file_buf;
}

void print_layout(GnomeXkbInfo* info, const char* layout) {
    const char* display_name = NULL;
    gnome_xkb_info_get_layout_info(info, layout, &display_name, NULL, NULL, NULL);
    printf("%-14s%s\n", layout, display_name);
}

void print_layouts(GnomeXkbInfo* info, const char* layout_buf) {
    int line_start = 0;
    for (int i = 0; layout_buf[i]; i++) {
        if (layout_buf[i] == '\n') {
            int line_len = i - line_start;
            if (line_len > 0) {
                char* line_buf = malloc(line_len + 1);
                assert(line_buf);
                memcpy(line_buf, layout_buf + line_start, line_len);
                line_buf[line_len] = '\0';
                print_layout(info, line_buf);
                free(line_buf);
            }
            line_start = i + 1;
        }
    }
}

void g_log_ignore (
    const gchar* log_domain,
    GLogLevelFlags log_level,
    const gchar* message,
    gpointer user_data
) {}


int main() {
    char* layout_list_path_buf = layout_list_path();
    char* layout_list_buf = read_file(layout_list_path_buf);
    if (!layout_list_buf) {
        fprintf(stderr, "Failed to read %s\n", layout_list_path_buf);
        return 1;
    }
    free(layout_list_path_buf);

    // Supress "Failed to load '/usr/share/xml/iso-codes/iso_639.xml'" warnings
    g_log_set_default_handler(g_log_ignore, NULL);

    GnomeXkbInfo* info = gnome_xkb_info_new();
    if (!info) {
        fprintf(stderr, "Error making XKB info\n");
        return 1;
    }

    print_layouts(info, layout_list_buf);

    free(layout_list_buf);
    g_object_unref(G_OBJECT(info));

    return 0;
}
