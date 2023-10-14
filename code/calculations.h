#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include "common.h"


int sum_d_norm_h = 0, sum_d_norm_w = 0, sum_d_norm_2 = 0, sum_d_norm_3 = 0;
float r1_elev_ratio, r2_elev_ratio, r3_elev_ratio, r4_elev_ratio = 0;
float roi_area[4];

float mean_d_h, mean_d_w = 0, mean_d_2 = 0, mean_d_3 = 0;
float mean_calib_SG = 0, mean_calib_beta = 0;
int mean_d_norm_h, mean_d_norm_w = 0, mean_d_norm_2 = 0, mean_d_norm_3 = 0;

float sum_d_h, sum_d_w = 0, sum_d_2 = 0, sum_d_3 = 0, sum_calib_SG = 0, sum_calib_beta = 0;
int perc_d_h, perc_d_w = 0, perc_d_2 = 0, perc_d_3 = 0;
int std_dev_d_h, std_dev_d_w = 0, std_dev_d_2 = 0, std_dev_d_3 = 0;
int pre_bas_lin_h, pre_bas_lin_w, pre_bas_lin_2, pre_bas_lin_3;
int post_bas_lin_h = 100, post_bas_lin_w = 1000, post_bas_lin_2 = 1000, post_bas_lin_3 = 1000;

float tm_h = 0, tm_w = 0, tm_2 = 0, tm_3 = 0;
float th_h = 0, th_w = 0, th_2 = 0, th_3 = 0;
float tb_h = 0, tb_w = 0, tb_2 = 0, tb_3 = 0;
int med_d_h = 0, med_d_w = 0, med_d_2 = 0, med_d_3 = 0;
int min_d_h = 1000, min_d_w = 1000, min_d_2 = 1000, min_d_3 = 1000;
int max_d_h, max_d_w = 0, max_d_2 = 0, max_d_3 = 0;

void reset_table_values();
void finding_roi_area();
void cal_data_porh();
void cal_data_perf();



#endif