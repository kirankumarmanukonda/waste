#ifndef CAMERA_FUNCTIONS_H
#define CAMERA_FUNCTIONS_H
#include <arv.h>
#include "main.h"
#include <tiffio.h>
#include "common.h"
#include <sys/time.h>


//#include <assert.h>


GdkPixbuf *pixbuf_big;
extern rgb *buffer_rgb;
extern float *Z_val;

extern GtkWidget *lsciimage;

float perfu_sum_h = 0, perfu_sum_w = 0, perfu_sum_2 = 0, perfu_sum_3 = 0;
#define comp_num 2
float p_h[comp_num], p_w[comp_num], p_2[comp_num], p_3[comp_num];
int aver = 0;
int at_start = 1;
void cal_perfusion();
int check_noise_img(float x[comp_num], float per_val);
void filter_data();
float *mov_average(float value[4]);
float standardize(float val);
void cal_perfusion();



int camerainit();
void free_data();
int assign_ip_camera();
void create_stream();
void saveConfig(const char *filename, int exposureTime, int fps, int window, float rois[4][4], float beta, float SG);
void loadConfig(const char *filename, int *exposureTime, int *fps, int *window, float rois[4][4], float *beta, float *SG);

double map(double x, double in_min, double in_max, double out_min, double out_max);
int camera(GtkWidget *widget);
int reanalyse(GtkWidget *widget, gpointer image);
void prev_lsci();
int take_img();
void draw_points(GtkWidget *event_box, gpointer data);
void put_lsciimg();
int put_img();
static gboolean timer_callbackp(GtkWidget *chartp);
static gboolean timer_callbackbp(GtkWidget *chartbp);
void savefullwindow(GtkWidget *widget);

#endif
