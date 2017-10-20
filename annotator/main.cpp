#include <iostream>
#include <sstream>
#include <ctime>
#include "AnnotationManipulator.h"
#include "LaserPoints.h"
#include "boost/filesystem.hpp"


void laser2ascii_cpp(char *long_filter, char *infile,
                     int store_x, int store_y, int store_z,
                     int store_r, int store_g, int store_b,
                     int store_p, int store_lpf, int store_l, int store_pl,
                     int store_pn, int store_sn, int store_lsn, int store_poln,
                     int store_fs, int store_scan, int store_t,
                     int store_a, int store_cnt, int store_tt,
                     int store_xt, int store_yt, int store_zt, char *asciifile);

int main() {

    std::clock_t start;
    double duration;
    start = std::clock();
    char *directory, *output;
    //directory =  (char*) "E:/Laser_data/Stanford_annotated/Annotated_Laserpoints";
    //output =  (char*) "E:/Laser_data/Stanford_annotated/modified";
    //directory = (char*) "E:/Laser_data/Stanford_annotated/crop/test_l2a";
    //output = (char*) "D:/test/stanford_data/AREA1_testset2/modified";

    /// function 1
    //ExportClasLabelNames(directory);

    /// function2
    //ImportAnnotatedAsciiToLaser(directory, ".txt");

    /// function3
    //LaserPointsManipulationBatch(directory, output);

    //function4
    //laser2ascii_cpp(directory, 0, 1, 2, 3, 4, 5, 6, 0, 0, 7, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    //string dir_str = "E:/Laser_data/Stanford_annotated/Annotated_Laserpoints";
    //       dir_str = "E:/Laser_data/Stanford_annotated/paraview/office2526_divisions";
    //Laser2AsciiBatch(dir_str, 1, 2, 3, "", 4, 5, 6, 7, 8);

    // function5
    char* asciipcdfile;
    asciipcdfile =  (char*) "E:/Laser_data/Stanford_annotated/supervoxel/supervoxels_labeledcloud.pcd";
    output    =  (char*) "E:/Laser_data/Stanford_annotated/supervoxel/";
    readAsciiPCD(asciipcdfile);


    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    printf("Total processing time: %.3f m \n", duration/60);

    std::cout << "Press ENTER to continue...";
    std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
}