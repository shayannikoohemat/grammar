//
// Created by NikoohematS on 29-6-2017.
//

#include "Annotator.h"
#include "AnnotatedPoint.h"
#include "AnnotatedPoints.h"
#include "ReadAscii.cpp"
#include <iostream>
#include <string>
#include "LaserPoints.h"
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#include <string>
#include "LaserPoint.h"
#include <fstream>
#include "setConsoleColor.cpp"
#include "ascii2laser_cpp.cc"

int StringtoClassLabel (string string_label){
    if (string_label == "wall" || string_label == "Wall")               return CLWall;          // 2
    if (string_label == "ceiling" || string_label == "Ceiling")         return CLCeiling;       // 3
    if (string_label == "floor" || string_label == "Floor")             return CLFloor;         // 4
    if (string_label == "window" || string_label == "Window")           return CLWindow;        // 5
    if (string_label == "door" || string_label == "Door")               return CLDoor;          // 6
    if (string_label == "beam" || string_label == "Beam")               return CLBeam;          // 7
    if (string_label == "column" || string_label == "Column")           return CLColumn;        // 8
    if (string_label == "objectpart" || string_label == "Ojectpart")    return CLObjectPart;    //
    if (string_label == "stairs" || string_label == "Stairs")           return CLStairs;        //
    // furniture group
    if (string_label == "clutter" || string_label == "Clutter")         return CLClutter;
    if (string_label == "bookcase" || string_label == "Bookcase")       return CLBookcase;
    if (string_label == "chair" || string_label == "Chair")             return CLChair;
    if (string_label == "board" || string_label == "Board")             return CLBoard;
    if (string_label == "table" || string_label == "Table")             return CLTable;
    if (string_label == "sofa" || string_label == "Sofa")               return CLSofa;

    // unclassified
    if (string_label == "unclassified") return CLUnclassified;
    else{
        return CLUnclassified;
    }

}

int StringtoBuildingDivision (string building_division){
    if (building_division == "room" || building_division == "Room")                     return CLRoom;          //
    if (building_division == "office" || building_division == "Office")                 return CLOffice;
    if (building_division == "auditorium" || building_division == "Auditorium")         return CLAuditorium;
    if (building_division == "conferenceRoom" || building_division == "ConferenceRoom") return CLConferenceRoom; //
    if (building_division == "WC" || building_division == "wc")                         return CLWC;
    if (building_division == "copyRoom" || building_division == "CopyRoom")             return CLCopyRoom;
    if (building_division == "pantry" || building_division == "Pantry")                 return CLPantry;
    if (building_division == "lounge" || building_division == "Lounge")                 return CLLounge;
    if (building_division == "storage" || building_division == "Storage")               return CLStorage;
    if (building_division == "hallway" || building_division == "Hallway" ||
        building_division == "corridor" || building_division == "Corridor")             return CLCorridor;      //
    if (building_division == "openspace" || building_division == "Openspace")           return CLOpenspace;
    if (building_division == "lobby" || building_division == "Lobby")                   return CLLobby;
    if (building_division == "balcony" || building_division == "Balcony" ||
        building_division == "terrace" || building_division == "Terrace")               return CLBalcony;
    if (building_division == "kitchen" || building_division == "Kitchen")               return CLKitchen;
    if (building_division == "staircase" || building_division == "Staircase")           return CLStairCase;     //

    // unclassified
    if (building_division == "unclassified") return CLUnclassified;
    else{
        return CLUnclassified;
    }

}

ObjNameAndNumber StringtoStruct (string objectname){
    /// extracting object number and object class from object name
    // TODO: extracting object number is error prone
    string obj_num_str = boost::regex_replace(objectname, boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
    int obj_number;
    obj_number = std::atoi(obj_num_str.c_str());  // convert ot int

    /// extracting object name
    auto pos = objectname.find_last_of("_");
    string obj_name, obj_num_str2;
    if (pos!= std::string::npos){
        obj_name = objectname.substr(0, pos);
        if (obj_number ==0) {  // this is backup solution for extracting object_number
            obj_num_str2 = objectname.substr(pos+1, objectname.length());
            obj_number = std::atoi(obj_num_str2.c_str());  // convert to int
        }
    } else {
        obj_name = objectname;
    }

    ObjNameAndNumber objNameAndNumber;
    objNameAndNumber.obj_number = obj_number;
    objNameAndNumber.obj_name   = obj_name;

    return objNameAndNumber;
}

/// convert ascii files in a root directory recursively to one or more laser files
LaserPoints Annotator::ReadAscii2LaserBatch(char *rootname, char *directory, char* ascii_file_filter,
                        int remove_double_points, int column_x, int column_y, int column_z,
                        int column_r, int column_g, int column_b, int column_p, int column_n,
                        int column_l, int column_pl,int column_pn, int column_sn, int column_poln, int column_scan,
                        int column_t, int column_a,int column_int, int column_fs,
                        int header_lines, double x_offset, double y_offset, double z_offset,
                        int p_offset, double x_scale, double y_scale, double z_scale,int set_r, int fixed_r, int set_p,
                        int fixed_p, int set_l, int fixed_l, int set_scan, int fixed_scan,
                        int set_fs, int fixed_fs, int meta_type, int max_subunit_size, int rgb_scale, int int_scale,
                        int column_nx, int column_ny, int column_nz)
{

    /// extract a vector of filespath
    vector<string> filespath;
    filespath = this -> RecursiveDir(directory);

    LaserPoints laserpoints;
    vector<string>::iterator filepath_it;
    int subunits_cnt=0;
    for (filepath_it = filespath.begin(); filepath_it != filespath.end(); filepath_it++){

        /// convert filepath string to char
        char *cfilepath = new char[filepath_it -> length() + 1];
        strcpy(cfilepath, filepath_it -> c_str());
        LaserSubUnit pointset_lasr;
        pointset_lasr =  ascii2laser_cpp(cfilepath, ascii_file_filter,
                         remove_double_points,
                         column_x,  column_y,  column_z,
                         column_r,  column_g,  column_b,
                         column_p,  column_n,  column_l,  column_pl,
                         column_pn,  column_sn,  column_poln, column_scan,  column_t,  column_a,
                         column_int,  column_fs,  header_lines, x_offset,  y_offset,  z_offset,
                         p_offset, x_scale,  y_scale,  z_scale, set_r,  fixed_r,  set_p,  fixed_p,
                         set_l,  fixed_l,  set_scan,  fixed_scan, set_fs,  fixed_fs, rootname,  directory,
                         meta_type,  max_subunit_size, rgb_scale,  int_scale,
                         column_nx,  column_ny,  column_nz);

        laserpoints.AddPoints(pointset_lasr);

        /// if size of laserpoints exceeds the threshold we write them and empty laserpoints
        if (laserpoints.size() >= max_subunit_size){
            //string root_name = "subunit_laserpoints_" ;
            string laserpoints_filename;
            subunits_cnt++;
            std::stringstream sstm;
            sstm << directory << "/" << rootname << subunits_cnt << ".laser";
            laserpoints_filename = sstm.str();

            SetColor(GREEN);
            laserpoints.Write(laserpoints_filename.c_str(), false);
            SetColor(GRAY);
            laserpoints.ErasePoints();
        }

    }
    return laserpoints; // returns remaining of the points
}


/// convert ascii files in a root directory recursively to one or more laser files
/// parsing file path (division name, object classes) to label tag and add to the points
void Annotator::Ascii2LaserBatchWithParsingPath(char *rootname, char *directory, char* ascii_file_filter,
                                    string ascii_extension , int pos_divisionname_in_filepath,
                                    int pos_buildingname_in_filepath, int build_floor,
                                    int remove_double_points,bool store_points_per_division,
                                    int meta_type, int max_subunit_size,
                                    int column_x, int column_y, int column_z,
                                    int column_r, int column_g, int column_b,
                                    int column_nx, int column_ny, int column_nz,
                                    int column_t, int column_p, int column_n, int column_l,
                                    int column_pl, int column_pn, int column_sn, int column_poln,
                                    int column_scan, int column_a, int column_int, int column_fs,
                                    int header_lines, double x_offset, double y_offset, double z_offset,
                                    int p_offset, double x_scale, double y_scale, double z_scale,
                                    int set_r, int fixed_r, int set_p, int fixed_p,
                                    int set_l, int fixed_l, int set_scan, int fixed_scan,
                                    int set_fs, int fixed_fs, int rgb_scale, int int_scale)
{

    /// extract a vector of filespath
    vector<string> filespath;
    filespath = this -> RecursiveDir(directory);

    // collect just ascii_files in a vector
    vector <string> ascii_filespath;
    vector <string>::iterator ascii_it;
    for (ascii_it = filespath.begin(); ascii_it != filespath.end(); ascii_it++){
        string extension;
        extension = boost::filesystem::extension(*ascii_it);
        if (ascii_extension == extension){
            ascii_filespath.push_back(*ascii_it);
        }
    }

    /// Store filepath for unclassified files in a text file
    FILE *unclassified_files;
    char str_root[500];
    strcpy (str_root,directory); // initialize the str_root with root string
    unclassified_files = fopen(strcat(str_root, "/unclassified_files.check"),"w");

    LaserPoints laserpoints;
    LaserPoints division_laserpoints;
    vector<string>::iterator filepath_it;
    int laserblock_cnt=0;  // collection of subunits based on max_subunit_size
    int ascii_files_cnt =0;
    int total_laserpoints=0;
    string previous_div_name;
    bool last_ascii_file=0;
    for (filepath_it = ascii_filespath.begin(); filepath_it != ascii_filespath.end(); filepath_it++){

        ascii_files_cnt++;
        //std::cout << "file path: " << *filepath_it << endl;
        /// convert filepath string to char
        char *cfilepath = new char[filepath_it -> length() + 1];
        strcpy(cfilepath, filepath_it -> c_str());
        LaserSubUnit pointset_lasr;
        pointset_lasr =  ascii2laser_cpp(cfilepath, ascii_file_filter,
                         remove_double_points, column_x,  column_y,  column_z,
                         column_r,  column_g,  column_b, column_p,  column_n,  column_l,  column_pl,
                         column_pn,  column_sn,  column_poln, column_scan,  column_t,  column_a,
                         column_int,  column_fs,  header_lines, x_offset,  y_offset,  z_offset,
                         p_offset, x_scale,  y_scale,  z_scale, set_r,  fixed_r,  set_p,  fixed_p,
                         set_l,  fixed_l,  set_scan,  fixed_scan, set_fs,  fixed_fs, rootname,
                         directory, meta_type,  max_subunit_size, rgb_scale,  int_scale,
                         column_nx,  column_ny,  column_nz);

        if (pointset_lasr.size() > 10){
            int labeltag=0, label2tag=0;
            std::tuple<int, int> labeltags;

            /// generate labels based on the filepath, and then set labels: division and object name
            labeltags = this -> LabelGenerator(*filepath_it, pos_divisionname_in_filepath,
                                       pos_buildingname_in_filepath, build_floor);
            labeltag = std::get<0>(labeltags);
            label2tag = std::get<1>(labeltags);
            if (labeltag != 0)
            {
                pointset_lasr.SetAttribute(LabelTag, labeltag);
                pointset_lasr.SetAttribute(Label2Tag, label2tag);
            }else{
                /// if labels were unclassified then store the path in a file
                string unclassified_filepath;
                unclassified_filepath = *filepath_it;
                fprintf(unclassified_files, "%s \n" , unclassified_filepath.c_str());
                pointset_lasr.SetAttribute(LabelTag, labeltag);  // labeltag =0
                pointset_lasr.SetAttribute(Label2Tag, label2tag); // label2tag =0
            }
            laserpoints.AddPoints(pointset_lasr);
            //laserpoints.Write("D:/test/AREA1_testset2/temp_lp.laser", false); // debug
        }
        delete cfilepath;

        /// store points per division folder e.g. office_1
        // TODO: this scope doesnt check the max_subunit_size variable
        if (store_points_per_division){
            vector<string> annotations;
            annotations = this -> ParsingFilePathtoAnnotWithBoost(*filepath_it);
            /// extract room_name or division name
            string division_name;
            division_name = annotations[pos_divisionname_in_filepath]; //i=5 -> Office_1
            if (ascii_files_cnt == 1) previous_div_name = division_name;
            if (division_name == previous_div_name){
                if (pointset_lasr.size() > 10)  division_laserpoints.AddPoints(pointset_lasr);
                previous_div_name = division_name;
                /// if last file of last folder
                if (ascii_files_cnt == ascii_filespath.size()) {
                    string division_file;
                    std::stringstream sstm;
                    sstm << directory << "/" << previous_div_name << ".laser";
                    division_file = sstm.str();;
                    division_laserpoints.Write(division_file.c_str(), false);
                    total_laserpoints += division_laserpoints.size();
                    division_laserpoints.ErasePoints();
                    sstm.clear();
                }
            }else {
                string division_file;
                std::stringstream sstm;
                sstm << directory << "/" << previous_div_name << ".laser";
                division_file = sstm.str();
                division_laserpoints.Write(division_file.c_str(), false);
                total_laserpoints += division_laserpoints.size();
                division_laserpoints.ErasePoints();
                if (pointset_lasr.size() > 10)  division_laserpoints.AddPoints(pointset_lasr);
                previous_div_name = division_name;
                sstm.clear();
            }
        }

        /// if size of laserpoints exceeds the threshold we write them and clear laserpoints
        if (!store_points_per_division){
            if (laserpoints.size() >= max_subunit_size){
                //string root_name = "subunit_laserpoints_" ;
                string laserpoints_filename;
                laserblock_cnt++;
                std::stringstream sstm;
                sstm << directory << "/" << rootname << laserblock_cnt << ".laser";
                laserpoints_filename = sstm.str();

                SetColor(GREEN);
                laserpoints.Write(laserpoints_filename.c_str(), false);
                total_laserpoints += laserpoints.size();
                SetColor(GRAY);
                laserpoints.ErasePoints();
                sstm.clear();
            }
        }
    } // end of for ascii_filespath vector
    fclose(unclassified_files);
    printf ("Total No of processed files: %d\n", ascii_files_cnt);
    printf ("Total No of processed points: %d\n", total_laserpoints);

    /// remaining points
    strcpy (str_root,directory);
    if (store_points_per_division) laserpoints.Write(strcat(str_root,"/all_laserpoints.laser"), false);
    if (!store_points_per_division) laserpoints.Write(strcat(str_root,"/remaining_laserpoints_.laser"), false);
}


/// batch to export comma delmited laser2ascii
/*void Annotator::Laser2AsciiBatchCommaDelimited(char *rootname, char *directory, char* ascii_file_filter,
                                               string ascii_extension , int pos_divisionname_in_filepath,
                                               int pos_buildingname_in_filepath, int build_floor,
                                               int remove_double_points,bool store_points_per_file,
                                               int meta_type, int max_subunit_size,
                                               int column_x, int column_y, int column_z,
                                               int column_r, int column_g, int column_b,
                                               int column_nx, int column_ny, int column_nz,
                                               int column_t, int column_p, int column_n, int column_l,
                                               int column_pl, int column_pn, int column_sn, int column_poln,
                                               int column_scan, int column_a, int column_int, int column_fs,
                                               int header_lines, double x_offset, double y_offset, double z_offset,
                                               int p_offset, double x_scale, double y_scale, double z_scale,
                                               int set_r, int fixed_r, int set_p, int fixed_p,
                                               int set_l, int fixed_l, int set_scan, int fixed_scan,
                                               int set_fs, int fixed_fs, int rgb_scale, int int_scale)
{
    /// extract a vector of filespath
    vector<string> filespath;
    filespath = this -> RecursiveDir(directory);

    // collect just laser_files in a vector
    vector <string> laser_filespath;
    vector <string>::iterator it;
    for (it = filespath.begin(); it != filespath.end(); it++){
        string extension;
        extension = boost::filesystem::extension(*it);
        if (extension == ".laser"){
            laser_filespath.push_back(*it);
        }
    }

    int laser_files_cnt=0;
    for (auto laser_file =laser_filespath.begin(); laser_file != laser_filespath.end(); laser_file++){

        laser_files_cnt++;
        //std::cout << "file path: " << *filepath_it << endl;
        /// convert filepath string to char
        char *claserfilepath = new char[laser_file -> length() + 1];
        strcpy(claserfilepath, laser_file -> c_str());
        ...
    }

}*/


std::tuple<int, int> Annotator::LabelGenerator (string file_path,int position_divisionname_in_filepath,
                                                int position_buildingname_in_filepath, int building_floor){

    vector<string> annotations;
    annotations = this -> ParsingFilePathtoAnnotWithBoost(file_path);
    std::cout << "file path: " << file_path << endl;

    string object_name, division_name, building_name;
    /// extract object_name
    string filename; // e.g. Table_1.txt
    filename = boost::filesystem::path(file_path).filename().string();
    auto pos = filename.find_last_of(".");
    if (pos != std::string::npos) {
        object_name = filename.substr(0, pos); // e.g. Table_1
    } else {
        object_name = filename;
    }

    /// extract room_name or division name
    division_name = annotations[position_divisionname_in_filepath]; //i=5 -> Office_1

    /// extract building name
    building_name = annotations[position_buildingname_in_filepath]; //i=4 -> Area_1

    /// extrating object number and object class from object name
    ObjNameAndNumber parsingobjname;
    parsingobjname = StringtoStruct(object_name);
    int object_number;
    string object_class;
    object_number = parsingobjname.obj_number; // e.g. 1
    object_class  = parsingobjname.obj_name; // e.g. Table

    /// if object_number is zero it means it contains strings so we map it to 999
    if (object_number ==0) object_number =999;

    /// extract classlabel from class enum
    int classlabel;
    classlabel = CLUnclassified;
    classlabel = StringtoClassLabel(object_class);

    /// if previous method didn't return correct class we try with std::find() method
    if (classlabel == CLUnclassified){
        vector < string > class_labels;
        //TODO:  hard coded list
        class_labels = {"wall", "ceiling", "floor", "clutter", "window", "door", "beam", "column"
                , "bookcase", "chair", "board", "table", "stairs", "objectpart",
                        "Wall", "Ceiling", "Floor", "Clutter", "Window", "Door", "Beam", "Column"
                , "Bookcase", "Chair", "Board", "Table", "Stairs", "Objectpart"};

        for (int i = 0; i< class_labels.size(); i++){
            if (object_name.find(class_labels[i]) != std::string::npos) {
                std::cout << class_labels[i] << '\n';
                classlabel = StringtoClassLabel(class_labels[i]);
                break;
            }
        }
    }

    /// extracting building division information
    ObjNameAndNumber parsingbuildingdivision;
    parsingbuildingdivision = StringtoStruct(division_name);  // e.g. division_name = Office_1

    string building_division;
    int building_div_num;

    building_division = parsingbuildingdivision.obj_name;  // e.g. office
    building_div_num  = parsingbuildingdivision.obj_number; // e.g. 1

    /// if building_div_num is zero it means it contained strings so we map it to 999
    if (building_div_num == 0) building_div_num = 999;

    int class_building_div;
    class_building_div = CLUnclassified;
    class_building_div = StringtoBuildingDivision(building_division);

    /// if previous method didn't return correct class of building_div we try with std::find() method
    if (class_building_div == CLUnclassified){
        //TODO:  hard coded list
        vector <string> building_divisions;
        building_divisions = {"room", "Room", "office", "Office", "auditorium", "Auditorium",
                              "hallway", "Hallway", "conferenceRoom" ,"ConferenceRoom",
                              "storage", "Storage", "WC", "wc", "copyRoom", "CopyRoom",
                              "pantry", "Pantry", "lounge", "Lounge", "lobby", "Lobby",
                              "corridor", "Corridor", "openspace", "Openspace"};

        for (int i = 0; i< building_divisions.size(); i++){
            if (building_division.find(building_divisions[i]) != std::string::npos) {
                std::cout << building_divisions[i] << '\n';
                class_building_div = StringtoBuildingDivision(building_divisions[i]);
                break;
            }
        }
    }

    if (classlabel != CLUnclassified){
        SetColor(GREEN);
        printf ("Class Label: %s , Division Label: %s \n \n", object_class.c_str(), building_division.c_str());
        SetColor(GRAY);
    } else {
        SetColor(GREEN);
        printf ("Class Label: %s <Unclassified>, Division Label: %s \n \n", object_class.c_str(), building_division.c_str());
        SetColor(GRAY);
    }

    /// point class and number:
    int label_tag, label2_tag;
    /// 00/000 first two digits for object class, last three digits for object number
    label_tag = classlabel * 1000 + object_number; // e.g. "05012" means class 5 obj_number 12
    if (classlabel == CLUnclassified) label_tag = 0;
    ///  00/00/000 building_division_class has 7 digits, 2 for floors, 2 for division class, 3 for division number
    label2_tag =  building_floor * 100000 + class_building_div * 1000 + building_div_num;


    auto labels = std::make_tuple (label_tag, label2_tag);

    return labels;

}

// return a vector of subdirectories and filename
vector<string> Annotator::ParsingFilePathtoAnnotation(const string& dirname){

    vector<string> annotations;
    string temp_str;
    temp_str = dirname;
    while(temp_str.rfind("/")) {
        auto pos = temp_str.rfind("/\\"); // returns the position of last occurrence
        if (pos!= std::string::npos){
            string substring;
            substring = temp_str.substr(pos+1, temp_str.length() - pos);
            temp_str = temp_str.erase(pos, dirname.length() - pos);
            if (!substring.empty()) annotations.push_back(substring);
        }
    }
    return annotations;
}

// return a vector of subdirectories and filenames, faster than method without boost
vector<string> Annotator::ParsingFilePathtoAnnotWithBoost(const string &dirname) {

    vector<string> annotations;
    for (const auto &part : boost::filesystem::path(dirname)) {
        annotations.emplace_back(part.filename().string());
    }
    return annotations;
}

/// retrun a vector of filespath with a recursive directory search
vector<string> Annotator::RecursiveDir (const string &dirname) {

    vector<string> files_path;
    for ( boost::filesystem::recursive_directory_iterator end, dir(dirname);
          dir != end; ++dir ) {
        //std::cout << *dir << "\n";  // full path
        //std::cout << dir->path().filename() << "\n"; // just last bit
        if (!boost::filesystem::is_directory(*dir)){ // skip directory paths
            files_path.push_back(dir ->path().string());
        }
    }
    return files_path;
}


vector<std::pair<string, string>> Annotator::RecursiveDirFileNameAndPath (const string &dirname){

    std::pair<string, string> path_file;
    vector<std::pair<string, string>> path_file_list;
    for ( boost::filesystem::recursive_directory_iterator end, dir(dirname);
          dir != end; ++dir ) {
        //std::cout << *dir << "\n";  // full path
        //std::cout << dir->path().filename() << "\n"; // just last bit
        if (!boost::filesystem::is_directory(*dir)){ // skip directory paths
            path_file = std::make_pair(dir ->path().string() , dir->path().filename().string());
            path_file_list.push_back(path_file);
        }
    }
    return path_file_list;
}



//// example from boost library for find
/*bool find_file( const boost::filesystem::path & dir_path,         // in this directory,
                const std::string & file_name, // search for this name,
                boost::filesystem::path & path_found )            // placing path here if found
{
    if ( !exists( dir_path ) ) return false;
    directory_iterator end_itr; // default construction yields past-the-end
    for ( directory_iterator itr( dir_path );
          itr != end_itr;
          ++itr )
    {
        if ( is_directory(itr->status()) )
        {
            if ( find_file( itr->path(), file_name, path_found ) ) return true;
        }
        else if ( itr->leaf() == file_name ) // see below
        {
            path_found = itr->path();
            return true;
        }
    }
    return false;
}*/

const char * Annotator::ClassLabelName(int classlabel)
{
    switch ((int) classlabel) {
        case CLUnclassifiedInvalid: return "Unclassified Invalid";
        case CLUnclassified:        return "Unclassified";
            // permanant structure
        case CLWall:                return "Wall";
        case CLFloor:               return "Floor";
        case CLCeiling:             return "Ceiling";
        case CLDoor:                return "Door";
        case CLWindow:              return "Window";
        case CLBeam:                return "Beam";
        case CLColumn:              return "Column";

        case CLStairs:              return "Stairs";
        case CLRamp:                return "Ramp";
            // space divisions
        case CLCorridor:            return "Corridor, Hallway";
        case CLRoom:                return "Room";
        case CLStairCase:           return "Stair Case";
        case CLOffice:              return "Office";
        case CLConferenceRoom:      return "Conference Room";
        case CLAuditorium:          return "Auditorium";
        case CLStorage:             return "Storage";
        case CLWC:                  return "WC";
        case CLCopyRoom:            return "Copy Room";
        case CLPantry:              return "Pantry";
        case CLLounge:              return "Lounge";
        case CLLobby:               return "Lobby";
        case CLOpenspace:           return "Open Space";
        case CLBalcony:             return "Balcony, Terrace";
        case CLBedRoom:             return "Bedroom";
        case CLLibrary:             return "Library";
        case CLCommunityRoom:       return "Community Room";
        case CLKitchen:             return "Kitchen";
        case CLBuildingFloor:       return "Building Floor";
        case CLParking:             return "Parking";
        case CLBuilding:            return "Building";

        // furniture
        case CLClutter:             return "Clutter";
        case CLObjectPart:          return "Object Part";
        case CLBookcase:            return "Bookcase";
        case CLChair:               return "Chair";
        case CLBoard:               return "Board";
        case CLTable:               return "Table";
        case CLPLant:               return "Plant";
        case CLDrawer:              return "Drawer";
        case CLMonitor:             return "Monitor";
        case CLCabinet:             return "Cabinet";
        case CLTrashbin:            return "Trashbin";
        case CLCupboard:            return "Cupboard";
        case CLLamp:                return "Lamp";
        case CLSofa:                return "Sofa";
        case CLBed:                 return "Bed";
        case CLTV:                  return "TV";
        // for occlusion reasoning
        case CLOccluded:            return "Occluded";
        case CLOpening:             return "Opening";
        case CLOccupied:            return "Occupied";

        case CLDirectoryPath:       return "Path to the directory";

        default:                    return "Unknown";
    }
}


ObjNameAndNumber Annotator::TranslateObjectLabel(int objectLabel){
    int obj_number, object_class;
    object_class    = objectLabel / 1000;       // 00,000      -->  object-class, obj-number
    obj_number      = objectLabel % 1000;

    ObjNameAndNumber objNameNum;
    objNameNum.obj_name = this -> ClassLabelName(object_class);
    objNameNum.obj_number = obj_number;

    return objNameNum;
}



