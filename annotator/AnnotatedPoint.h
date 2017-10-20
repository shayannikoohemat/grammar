//
// Created by NikoohematS on 29-6-2017.
//

#ifndef ANNOTATOR_ANNOTATED_POINT_H
#define ANNOTATOR_ANNOTATED_POINT_H

#endif //ANNOTATOR_ANNOTATED_POINT_H

#include "LaserPoint.h"
#include <vector>

/*struct PointAnnotations{
     string_label;
     object_name;
     room_name;
     floor;
     building_name;
     path;

};*/

class AnnotatedPoint: public LaserPoint
{
protected:

    // annotation is a fixed vector: [string_labels, object_name, room, floor, building, path]
    vector<string> point_annotations;

public:

    /// Default constructor
    AnnotatedPoint() : LaserPoint()
    {
        tags = NULL; attributes = NULL; Initialise();
        point_annotations.assign(6, "");
    }

    // copy constructor
    AnnotatedPoint(const AnnotatedPoint &annpoint)
    {
        point_annotations.assign(6, "");
        tags = NULL; attributes = NULL;
        Initialise();
        *this = annpoint;
    };

    // destructor
/*    ~AnnotatedPoint()  TODO: define *.cpp class for annotatedpoint
    {
        if (num_attributes > 0)
        {
            free(tags);
            free(attributes);
            point_annotations.clear();}
    };*/


    /// Copy assignment
    AnnotatedPoint & operator = (const LaserPoint &pt)
    {
        LaserPoint::operator = (pt);
        point_annotations.assign(6, "");

        return(*this);
    };


    // set string label e.g. wall, door, window
    void SetStringLabel(string string_label){
        point_annotations[0] = string_label;
    }

    // set object name e.g. wall_1, wall_2, window_2, door_3
    void SetObjectName(string object_name){
        point_annotations[1] = object_name;
    }

    // set partition name e.g. room, hallway, office, storage, ...
    void SetRoomName(string partition_name){
        point_annotations[2] = partition_name;
    }

    // set floor e.g. floor_1, floor_2, floor_3, ...
    void SetFloor(string floor){
        point_annotations[3] = floor;
    }

    // set building name e.g. campus, house, ITC, ...
    void SetBuildingName(string building_name){
        point_annotations[4] = building_name;
    }

    // set path of the annotated file
    void SetPath(string path){
        point_annotations[5] = path;
    }

    // return point annotation vector
    vector<string> PointAnnotations()
    { return  point_annotations; }

};