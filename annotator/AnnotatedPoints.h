//
// Created by NikoohematS on 29-6-2017.
//

#ifndef ANNOTATOR_ANNOTATEDPOINTS_H
#define ANNOTATOR_ANNOTATEDPOINTS_H

#endif //ANNOTATOR_ANNOTATEDPOINTS_H


#include "LaserPoints.h"
#include <vector>

class AnnotatedPoints: public LaserPoints
{
    public:
    // default constructor
    AnnotatedPoints()
    {
        LaserPoints::Initialise();
    }

    /*    bool HasAnnotation(vector annotations) const
        {
            LaserPoints::const_iterator point;

            for (point=begin(); point!=end(); point++)
                if (point->HasAttribute(tag)) return true;
            return false;
        }*/
};