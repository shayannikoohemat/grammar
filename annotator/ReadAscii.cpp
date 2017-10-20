//
// Created by NikoohematS on 23-1-2017.
//

#include <stdlib.h>
#include <string.h>
#include "LaserPoints.h"



LaserPoints Read_Ascii(char *ascii_file){

    FILE *ascii;
    printf ("Reading in ascii laser points...\n");
    ascii = Open_Compressed_File(ascii_file, "r");
    if (!ascii) {
        fprintf(stderr, "Skip the file %s because of opening error \n", ascii_file);
        return (0);
    }

//read in ascii laser points, transfer to laser format
    char    line[2048], *comma;
    double  value[21];
    int     index1=0;
    LaserPoint point;
    LaserPoints temp_laser_points, laser_points;

    // Skip the header records
    fgets(line, 2048, ascii);  // skip the header file
    if (feof(ascii)) {
        fprintf(stderr, "Error: end of file reached after reading header lines.\n");
        return (0);
    }

    // hard coded columns:
    int column_x, column_y, column_z, column_r, column_g, column_b,
    column_nx, column_ny, column_nz, column_p;
    column_x = 1 , column_y = 2, column_z = 3;
    column_r = 4 , column_g = 5, column_b = 6;
    column_nx = column_ny = column_nz = 0;
    column_p = 0; // pulse count

    int rgb_scale =1;

/*    // Set the point type
    if (column_b) {
        if (column_p) temp_laser_points.Scanner().SetPointType(MultiColourPoint);
        else temp_laser_points.Scanner().SetPointType(ColourPoint);
    }
    else if (column_r) {
        if (column_p) temp_laser_points.Scanner().SetPointType(MultiReflectancePoint);
        else temp_laser_points.Scanner().SetPointType(ReflectancePoint);
    }
    else {
        if (column_p) temp_laser_points.Scanner().SetPointType(MultiPoint);
        else temp_laser_points.Scanner().SetPointType(NormalPoint);
    }*/

    do {
        if (fgets(line, 1024, ascii)) {
            index1++;
            // Remove the comma's
            while ((comma = strchr(line, ',')) != NULL) * comma = ' ';

            // Read the next line
            sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                   value + 1, value + 2, value + 3, value + 4, value + 5,
                   value + 6, value + 7, value + 8, value + 9, value + 10,
                   value + 11, value + 12, value + 13, value + 14, value + 15,
                   value + 16, value + 17, value + 18, value + 19, value + 20);

            // Copy the data to the laser point
            point.X() = value[column_x];
            point.Y() = value[column_y];
            point.Z() = value[column_z];

            // set colors by variable column
            if (column_g) {  // Colour point
                point.SetColour((int) value[column_r] / rgb_scale,
                                (int) value[column_g] / rgb_scale,
                                (int) value[column_b] / rgb_scale);
            }

            // set normal attributes if there is
            if (column_nx)    point.SetAttribute(NormalXTag, (float) value[column_nx]);
            if (column_ny)    point.SetAttribute(NormalYTag, (float) value[column_ny]);
            if (column_nz)    point.SetAttribute(NormalZTag, (float) value[column_nz]);
            // set time
            //point.SetDoubleAttribute(TimeTag, value[1]); // value [1] hard coded

            temp_laser_points.push_back(point);
        }
        //if (index1 == (index1 / 1000) * 1000) {
        //    temp_laser_points.RemoveAlmostDoublePoints(false, 0.01);
            printf(" %d / %d\r", laser_points.size(), index1);
            fflush(stdout);
            laser_points.AddPoints(temp_laser_points);
            temp_laser_points.ErasePoints();
        //}
        //    }
    } while (!feof(ascii));
    Close_Compressed_File(ascii);
    printf("\nRead %d points\n", laser_points.size());

    return laser_points;
}


