#ifndef PDF_GEN
#define PDF_GEN_H

#include <hpdf.h>
#include <time.h> //for get time
#include <setjmp.h>// error handling pdf
#include <sys/stat.h> //struct stat buffer in the final_report

#include "common.h"


HPDF_Doc pdf;
HPDF_Page page[10];
HPDF_Font def_font;
HPDF_REAL tw, st;
HPDF_REAL height;
HPDF_REAL width;

int pg_num = 0;

jmp_buf env;


char rep_name[300];
int init_pdf = 1;

#ifdef HPDF_DLL
void __stdcall
#else
void
#endif
    error_handler(HPDF_STATUS error_no,
                  HPDF_STATUS detail_no,
                  void *user_data)
{
  printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
         (HPDF_UINT)detail_no);
  longjmp(env, 1);
}









void Page_headings(const char *study_type, char *name, char *age, char *gender, char *pat_id, char *pat_clin);
void plot_graph(HPDF_Page page, float *x_values, float *y_values, int num_points, float max_x, float max_y, float mov_x, float mov_y, float width, float height);
void print_table1();
void print_table2(float x, float y);
void rect_roi_draw_pdf(float x_pos, float y_pos, float resize_w, float resize_h, float image_height);
void free_roi_draw_pdf(float x_pos, float y_pos, float resize_w, float resize_h, float image_height);
void report();
void remove_cliniciation(HPDF_Page page, double x, double y, double width, double height);
void final_report(int last);

#endif