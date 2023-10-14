#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gtk/gtk.h> //gui application

#define WORKING_DIR "/home/ayati/Videos/gladetest_v2.5.1/gladetest/"
#define WIDGET_WINDOW "window"
#define data_pnts_cnt 15000

float perfusionh = 0, perfusionw = 0, perfusion2 = 0, perfusion3 = 0, calib_SG = 0, calib_beta = 0;
float perfusionh_norm = 0, perfusionw_norm = 0, perfusion2_norm = 0, perfusion3_norm = 0;



#define wi 1920
#define h 1200

#define size wi *h

#define perf_scale_x 2.133333333
#define perf_scale_y 2.4

#define BUFFER_SIZE wi *h

GtkBuilder *builder; GtkWidget *test; int exposureTime;
char *pat_name, *pat_age, *pat_gender, *pat_id, *pat_clin, *pat_doc;

int preview_flag = 0, bp = 0;
int switch_elevation_flag = 0; // 1 -> its in elevation mode
int elev = 0;                  // 1 -> switch to elevation analysis

int max_y_val_graph;
int colormap_calib = 0;
float cmap_min_val = 30;
float cmap_max_val = 120;

static long count = 0, countbp = 0;

int free_select = 0;

typedef struct
{
  double x;
  double y;
} Point;

#define num_roi 4
#define num_vertex 10

Point polygon[num_roi][num_vertex + 1]; // polygon1[7], polygon2[7];
int vertex = 0, roi_vertexes[4];
int free_roi_num = 0;
int control_roi[4], control_roi_num = 0;
int control_rect_roi[4] = {1, 1, 1, 1};
int control_rect_roi_num = 0;
int free_select_exe_once = 1;
char *free_roi_color[4] = {"Black", "Black", "Black", "Black"};

float rois[4][4];
float circle_rois[4][3]; // four rois and center radius x_p, x_rel, y_p, y_rel
#define data_pnts_cnt 15000

float points[data_pnts_cnt];
float points2[data_pnts_cnt];
float points3[data_pnts_cnt];
float points4[data_pnts_cnt];
float points_calib_SG[data_pnts_cnt];
float points_calib_beta[data_pnts_cnt];

float time_axis[data_pnts_cnt];
int image_no = 1;
double elapsed_time;

int bp_points[data_pnts_cnt];
float points_raw[data_pnts_cnt];
float points2_raw[data_pnts_cnt];
float points3_raw[data_pnts_cnt];
float points4_raw[data_pnts_cnt];

float points_norm[data_pnts_cnt];
float points2_norm[data_pnts_cnt];
float points3_norm[data_pnts_cnt];
float points4_norm[data_pnts_cnt];

int control_porh = 0;
int control_reports[20];

///////////reportinit////////////////
char reportname[500];
char filename[500] = "";
char dirbase[500] = "";

int report_num = 0;

char image_location[250], image_location_preview[250], current_working_dir[250];

int calib_flag = 0, calib_device = 0;
int from_calib_button = 0;

void show_dialog(char *msg);
int findMax(float a, float b, float c, float d, int e);
char *intToString(int num);
char *floatToString(float num);
int cal_percentage(float ref, float pnt);
int check_proper(float x[data_pnts_cnt], int i);






#endif