//
// Created by NikoohematS on 29-6-2017.
//

#include <string>
#include "LaserPoints.h"
#include "LaserPoint.h"
#include "boost/filesystem.hpp"

#ifndef ANNOTATOR_ANNOTATOR_H
#define ANNOTATOR_ANNOTATOR_H

#endif //ANNOTATOR_ANNOTATOR_H

using std::string;

enum ClassLabel { CLUnclassifiedInvalid=-1, CLUnclassified=1,

    // permanent structure
    CLWall, CLFloor, CLCeiling, CLDoor, CLWindow, CLBeam, CLColumn,
    // if we had staircase it should be classified in divisions,
    // but stairs and ramps are permanent structure object
    CLStairs, CLRamp,
    // part of the object
    CLObjectPart,
    // furniture
    CLClutter=20, CLBookcase, CLChair, CLBoard,
    CLTable, CLPLant, CLDrawer, CLMonitor, CLCabinet,
    CLTrashbin, CLCupboard, CLLamp, CLSofa, CLBed, CLTV,

    // space divisions
    CLCorridor=50, CLRoom, CLStairCase, CLOffice, CLConferenceRoom,
    CLAuditorium, CLStorage, CLWC, CLCopyRoom,
    CLPantry, CLLounge, CLLobby, CLOpenspace, CLBalcony,
    CLBedRoom, CLLibrary, CLCommunityRoom, CLKitchen,
    CLBuildingFloor, CLParking, CLBuilding,

    // this is for occlusion reasoning
    CLOccluded, CLOpening, CLOccupied,

    CLDirectoryPath  // should be string
};

struct ObjNameAndNumber{
    string obj_name;
    int obj_number;
};

class Annotator {

public:

    // return a vector of subdirectories and filename e.g. "E://dir1//di2//file.txt"
    // returns: dir1, dir2, file.txt
    vector<string> ParsingFilePathtoAnnotation(const string& dirname);

    // return a vector of subdirectories and filename, faster than method without boost
    vector<string> ParsingFilePathtoAnnotWithBoost(const string &dirname);

    // read files recursively
    vector<string> RecursiveDir (const string &dirname);

    // read files recursively and return file path and file name in a tuple format
    vector<std::pair<string, string>> RecursiveDirFileNameAndPath (const string &dirname);

    // read ascii2laser recursively
    LaserPoints ReadAscii2LaserBatch(char *rootname, char *directory, char* ascii_file_filter=0,
                                     int remove_double_points=0,
                                     int column_x = 1, int column_y = 2, int column_z = 3,
                                     int column_r = 0, int column_g = 0, int column_b = 0,
                                     int column_p = 0, int column_n = 0, int column_l = 0, int column_pl = 0,
                                     int column_pn = 0, int column_sn = 0, int column_poln = 0,
                                     int column_scan = 0, int column_t = 0, int column_a = 0,
                                     int column_int = 0, int column_fs = 0, int header_lines = 0,
                                     double x_offset = 0, double y_offset = 0, double z_offset = 0,
                                     int p_offset = 0,
                                     double x_scale = 1.0, double y_scale = 1.0, double z_scale = 1.0,
                                     int set_r = 0, int fixed_r = 0, int set_p = 0, int fixed_p = 0,
                                     int set_l = 0, int fixed_l = 0, int set_scan = 0, int fixed_scan = 0,
                                     int set_fs = 0, int fixed_fs = 0,
                                     int meta_type = 0, int max_subunit_size = 10000000,
                                     int rgb_scale = 1, int int_scale = 1,
                                     int column_nx = 0, int column_ny = 0, int column_nz = 0);

    void Ascii2LaserBatchWithParsingPath(char *rootname, char *directory, char *ascii_file_filter,
                                                string ascii_extension , int pos_divisionname_in_filepath,
                                                int pos_buildingname_in_filepath, int build_floor,
                                                int remove_double_points=0, bool store_points_per_division=0,
                                                int meta_type=0, int max_subunit_size = 10000000,
                                                int column_x=1, int column_y=2, int column_z=3,
                                                int column_r=0, int column_g=0, int column_b=0,
                                                int column_nx=0, int column_ny=0, int column_nz=0,
                                                int column_t=0,
                                                int column_p=0, int column_n=0, int column_l=0, int column_pl=0,
                                                int column_pn=0, int column_sn=0, int column_poln=0,
                                                int column_scan=0, int column_a=0,
                                                int column_int=0, int column_fs=0, int header_lines=0,
                                                double x_offset=0, double y_offset=0, double z_offset=0,
                                                int p_offset=0,
                                                double x_scale=1.0, double y_scale=1.0, double z_scale=1.0,
                                                int set_r=0, int fixed_r=0, int set_p=0, int fixed_p=0,
                                                int set_l=0, int fixed_l=0, int set_scan=0, int fixed_scan=0,
                                                int set_fs=0, int fixed_fs=0, int rgb_scale=1, int int_scale=1);


    /// batch to export comma delmited laser2ascii
    void Laser2AsciiBatchCommaDelimited(char *rootname, char *directory, char *ascii_file_filter,
                                        string ascii_extension , int pos_divisionname_in_filepath,
                                        int pos_buildingname_in_filepath, int build_floor,
                                        int remove_double_points=0, bool store_points_per_division=0,
                                        int meta_type=0, int max_subunit_size = 10000000,
                                        int column_x=1, int column_y=2, int column_z=3,
                                        int column_r=0, int column_g=0, int column_b=0,
                                        int column_nx=0, int column_ny=0, int column_nz=0,
                                        int column_t=0,
                                        int column_p=0, int column_n=0, int column_l=0, int column_pl=0,
                                        int column_pn=0, int column_sn=0, int column_poln=0,
                                        int column_scan=0, int column_a=0,
                                        int column_int=0, int column_fs=0, int header_lines=0,
                                        double x_offset=0, double y_offset=0, double z_offset=0,
                                        int p_offset=0,
                                        double x_scale=1.0, double y_scale=1.0, double z_scale=1.0,
                                        int set_r=0, int fixed_r=0, int set_p=0, int fixed_p=0,
                                        int set_l=0, int fixed_l=0, int set_scan=0, int fixed_scan=0,
                                        int set_fs=0, int fixed_fs=0, int rgb_scale=1, int int_scale=1);




    /// generate labels from file-path based on building-division name and object name
    std::tuple<int, int> LabelGenerator (string file_path,int position_divisionname_in_filepath,
                                         int position_buildingname_in_filepath, int building_floor);


    const char * ClassLabelName(int classlabel);

    ObjNameAndNumber TranslateObjectLabel(int objectLabel);

    /// convert labeltags to division and object class
    std::tuple<int, int> LabelToClass (int label);



    // writing a *.laser file of points to one or many *.txt file of annotations
    void ExportAnnotationToDirectory(const string& dirname) const;


    // merge annotations based on similar obj name, floor, room, label, ...
    LaserPoints MergeLabels(LaserPoints &laserpoints, const LaserPointTag tag,
                            vector<string> vector_of_labels, int target_label_value);

};



