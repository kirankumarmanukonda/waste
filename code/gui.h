#include <stdio.h>
#include <stdint.h> //get uint16_t
#include <stdlib.h> //rand function

#include <slope/slope.h>// plot graphs
#include <math.h> //math operations

#ifndef GUI_H	/******	it prevents the header file include multiple times  */
#define GUI_H
#include <arv.h>
#include "main.h"
#include "common.h"
#include "calculations.h"


/***********GTK Initialisations************/

GtkWidget *window, *window1, *window2, *history_window;
GtkWidget *fixed, *fixed4;
GtkWidget *lsciimage;
GtkWidget *normalimg;
GtkWidget *heading_angio;
GtkWidget *lscibox;
GtkWidget *previewlsci;
GtkWidget *savelsci;
GtkWidget *event_box;
GtkWidget *event_box_normal, *normalbox;
GtkWidget *spinner_image;
GtkWidget *warning_msg, *cmap_scale_box;

GtkWidget *pmbox, *pmbox1;
GtkWidget *previewpm;
GtkWidget *pmgraph, *pmgraph1;
GtkWidget *savepm;

GtkWidget *bpbox, *bp_connect;
GtkWidget *previewbp;
GtkWidget *bpgraph;
GtkWidget *savebp;
GtkWidget *bp_prompt, *bp_prompt_val;

GtkWidget *preview, *calibrate;
GtkWidget *mainevent;

GtkWidget *save_analysis;
GtkWidget *report_gen;
GtkWidget *save_analysis_msg;

GtkWidget *free_select_button;
GtkWidget *rect_select_button;
GtkWidget *exp_tim_id, *exp_decrease, *exp_increase;

GtkWidget *back_home_button;
GtkWidget *back_porh;
GtkWidget *back_perfusion;
GtkWidget *back_elevation;

GtkWidget *start_new_test;
GtkWidget *history_button;
GtkWidget *calendar, *cal_box;
GtkWidget *id_find, *name_find;
GtkWidget *test_date_from, *test_date_to;

GtkWidget *alltests_buttons;
GtkWidget *home;
GtkWidget *save;



GtkWidget *zoom_in_button;
GtkWidget *zoom_out_button, *zoom_out_button1;

GtkWidget *roi_table, *porh_label_box, *roi_table_elevation, *elevation_buttons, *roi_table_porh, *elevation_label;
GtkWidget *roi_1, *roi_2, *roi_3, *roi_4, *roi_5, *roi_6, *roi_7, *roi_8;

GtkStyleContext *context, *clinical_impress, *clinical_box, *context_scrolled_window, *clinical_box_buttons, *context_scrolled_head, *context_roi_type;
GtkWidget *r1_button, *r2_button, *r3_button, *r4_button;

GtkWidget *r1_mean;
GtkWidget *r2_mean;
GtkWidget *r3_mean;
GtkWidget *r4_mean;

GtkWidget *r1_perc;
GtkWidget *r2_perc;
GtkWidget *r3_perc;
GtkWidget *r4_perc;

GtkWidget *r1_sd;
GtkWidget *r2_sd;
GtkWidget *r3_sd;
GtkWidget *r4_sd;

GtkWidget *r1_area;
GtkWidget *r2_area;
GtkWidget *r3_area;
GtkWidget *r4_area;

GtkWidget *r1_mean_elev, *r2_mean_elev, *r3_mean_elev, *r4_mean_elev;
GtkWidget *r1_perc_elev, *r2_perc_elev, *r3_perc_elev, *r4_perc_elev;
GtkWidget *r1_sd_elev, *r2_sd_elev, *r3_sd_elev, *r4_sd_elev;
GtkWidget *r1_ratio_elev, *r2_ratio_elev, *r3_ratio_elev, *r4_ratio_elev;

/**********************     table 2     ***************/
// GtkWidget *r1_button1, *r2_button1, *r3_button1, *r4_button1;
GtkWidget *r1_rf, *r2_rf, *r3_rf, *r4_rf;
GtkWidget *r1_bz, *r2_bz, *r3_bz, *r4_bz;
GtkWidget *r1_pf, *r2_pf, *r3_pf, *r4_pf;
GtkWidget *r1_pobl, *r2_pobl, *r3_pobl, *r4_pobl;
GtkWidget *r1_tm, *r2_tm, *r3_tm, *r4_tm;
GtkWidget *r1_th, *r2_th, *r3_th, *r4_th;
GtkWidget *r1_tb, *r2_tb, *r3_tb, *r4_tb;

GtkWidget *r1_rf2bz, *r2_rf2bz, *r3_rf2bz, *r4_rf2bz;
GtkWidget *r1_bz2pf, *r2_bz2pf, *r3_bz2pf, *r4_bz2pf;
GtkWidget *r1_rf2pf, *r2_rf2pf, *r3_rf2pf, *r4_rf2pf;

GtkWidget *intro_animation;
GdkPixbufAnimation *animation;

GtkWidget *tool_bar;
GtkWidget *scale_widgetu, *scale_widgetl;


////////////////graph initialisation/////////

SlopeScale *scalep, *scalebp;
SlopeItem *series, *series2, *seriesp, *series3, *seriesbp;
double *x, *y, *xp, *yp, *x2, *y2, *x3, *y3, *xbp, *ybp;
long n = 200;
const double dx = 4.0 * G_PI / 200;
GtkWidget *chartp, *chartbp;
SlopeFigure *figurep, *figurebp;



void show_perf_table();
void show_porh_table();

int take_img();
void draw_points(GtkWidget *event_box, gpointer data);
void put_lsciimg();
int put_img();
char *getConnectedWifiSSID();
gboolean onclick_bp_connect(GtkWidget *widget, gpointer data);
void reset_roi();
gboolean stop_spinner();
void show_spinner(char msg[100], int time, GtkWidget *window);
void camera_init_dialog();
gboolean onclick_calibrate(GtkWidget *widget, gpointer data);
gboolean ontogglepreviewlsci(GtkWidget *widget, gpointer data);
gboolean onclickmainevent(GtkWidget *widget, gpointer data);
void retrive_bp_data();
void retrive_time_data();
gboolean onclick_history(GtkWidget *widget, gpointer data);
gboolean onclick_home(GtkWidget *widget, gpointer data);
void remove_trailing_spaces(char *str);
gboolean onclick_save(GtkWidget *widget, gpointer data);
static void date_selected(GtkCalendar *calendar, gpointer user_data);
void cal_drop_function();
gboolean calender_dropped_from(GtkWidget *widget, gpointer data);
gboolean calender_dropped_to(GtkWidget *widget, gpointer data);

gboolean onclick_new_test(GtkWidget *widget, gpointer data);
void perfusion_widgets();
void porh_widgets();
int switch_analysis();
void saving_old_state();
void get_back_old_state();
void get_back_old_state();
void make_folders(char folder_name[20], char elev_folder[10]);
gboolean simulate_delayed_button_click(gpointer button);
int reanalyse_folder_check(char folder_name[20]);
gboolean onclick_perfusion(GtkWidget *widget, gpointer data);
gboolean onclick_elevation(GtkWidget *widget, gpointer data);
gboolean onclick_porh(GtkWidget *widget, gpointer data);
gboolean switch_elevation(GtkWidget *widget, gpointer data);
int open_report();
void on_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data);
gboolean onclick_report_gen(GtkWidget *widget, gpointer user_data);
void color_active_roi(GtkWidget *widget);
gboolean onclick_free_select(GtkWidget *widget, gpointer data);
gboolean onclick_rect_select(GtkWidget *widget, gpointer data);
gboolean onclick_circle_select(GtkWidget *widget, gpointer data);
gboolean show_roi_clicked(GtkWidget *widget, gpointer data);
void clear_graph();
void clear_text_view(GtkTextView *text_view);
int come_to_initial_state();
void on_home_response(GtkDialog *dialog, gint response_id, gpointer user_data);
gboolean on_back_home_clicked(GtkWidget *widget, gpointer data);


void clear_list_store();
static int callback(void *data, int argc, char **argv, char **azColName);
void convert_date_format(char *date);
int search_database();
gboolean onclick_find(GtkWidget *widget, gpointer data);
gboolean onclickfind_enter(GtkWidget *widget, gpointer data);
int update_data_base(char id[50], char column[15], char value[5]);
int add_new_data_base(char id[50], char name[50], int age, char date[12], char time[6]);
GtkWidget *create_tree_view();
void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
void show_dialog(char *msg);

static void on_scale_value_changed_upper(GtkWidget *widget, gpointer data);
static void on_scale_value_changed_lower(GtkWidget *widget, gpointer data);
void clear_graph(void);
int update_data_base(char *id, char *col, char *val);
int fill_list(void);
void on_window_destroy(GtkWidget *widget, gpointer user_data);
void activate(GtkApplication *app, gpointer user_data);
void sigsegv_handler(int signal);

#endif
