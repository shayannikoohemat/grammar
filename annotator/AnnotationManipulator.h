//
// Created by NikoohematS on 7-8-2017.
//

#ifndef ANNOTATOR_ANNOTATIONMANIPULATION_H
#define ANNOTATOR_ANNOTATIONMANIPULATION_H

#endif //ANNOTATOR_ANNOTATIONMANIPULATION_H


#include <iostream>
#include "Annotator.h"
#include <sstream>
#include <limits.h>
#include "LaserUnit.h"

void ExportClasLabelNames(char* directory);

void ImportAnnotatedAsciiToLaser(string dirname, string ascii_extension);

LaserPoints MergeLabels (LaserPoints lp);

void LaserPointsManipulationBatch(string directory, string ouput_directory);

void Laser2AsciiBatch(string directory,
                      int store_x, int store_y, int store_z,
                      string out_put_directory="",
                      int store_r=0, int store_g=0, int store_b=0,
                      int store_l=0, int store_l2=0, int store_fs=0,
                      int store_p=0, int store_lpf=0,  int store_pl=0,
                      int store_pn=0, int store_sn=0, int store_lsn=0,
                      int store_poln=0, int store_scan=0, int store_t=0,
                      int store_a=0, int store_cnt=0, int store_tt=0,
                      int store_xt=0, int store_yt=0, int store_zt=0);

LaserSubUnit readAsciiPCD(char *ascii_file, char* ascii_file_filter,
                          int remove_double_points,
                          int column_x, int column_y, int column_z,
                          int column_r, int column_g, int column_b,
                          int column_p, int column_n, int column_l, int column_pl,
                          int column_pn, int column_sn, int column_poln,
                          int column_scan, int column_t, int column_a,
                          int column_int, int column_fs, int header_lines,
                          double x_offset, double y_offset, double z_offset,
                          int p_offset,
                          double x_scale, double y_scale, double z_scale,
                          int set_r, int fixed_r, int set_p, int fixed_p,
                          int set_l, int fixed_l, int set_scan, int fixed_scan,
                          int set_fs, int fixed_fs,
                          char *rootname, char *output_directory,
                          int meta_type, int max_subunit_size,
                          int rgb_scale, int int_scale,
                          int column_nx, int column_ny, int column_nz);


