//
// Created by NikoohematS on 25-7-2017.
//

#include "LaserPoints.h"

void MLS_preprocessing(LaserPoints &points, double min_distance)
{
    LaserPoints::iterator       point1, point2, point;
    bool                        done=false;
    int                         count;
    double                      height_diff, sum;
    std::vector<LaserPoints>    clusters;
    std::vector<LaserPoints>::iterator cluster;
    LaserPoints                 new_cluster;

    // Check there is something to thin out
    if (points.size() < 2) return ;

    // Sort points on XYZ coordinates
    points.SortOnCoordinates();

    // Set all points as unfiltered
    points.SetUnFiltered();

    point1 = points.begin();
    while (!done) {
        // Find sequence with the same XY coordinates
        point2 = point1+1;
        count = 0;
        while (!done) {
            if (point2 == points.end()) done = true;
            else if (fabs(point2->X() - point1->X()) < 0.001 &&
                     fabs(point2->Y() - point1->Y()) < 0.001) {
                count++, point2++;
            }
            else done = true;
        }
        // Process sequences of same XY coordinates
        if (count) {
            // Put points into clusters
            for (point=point1; point!=point2; point++) {
                for (cluster=clusters.begin(), done=false;
                     cluster!=clusters.end() && !done; cluster++) {
                    height_diff = cluster->begin()->Z() - point->Z();
                    if (fabs(height_diff) < min_distance) {
                        cluster->push_back(*point);
                        done = true;
                    }
                }
                // Create new cluster if needed
                if (!done) {
                    new_cluster.push_back(*point);
                    clusters.push_back(new_cluster);
                    new_cluster.ErasePoints();
                }
            }
            // Calculate average height for each cluster
            for (cluster=clusters.begin(); cluster!=clusters.end(); cluster++) {
                // Calculate height sum
                sum = 0.0;
                for (point=cluster->begin(); point!=cluster->end(); point++)
                    sum += point->Z();
                // Store average heights in the first couple of points
                point1->Z() = sum / cluster->size();
                point1++;
                // Clear cluster
                cluster->ErasePoints();
            }
            // Set the remaining points to filtered
            for (; point1!=point2; point1++) point1->SetFiltered();
            // Clear clusters
            clusters.erase(clusters.begin(), clusters.end());
        }
        // Prepare for next sequence
        if (point2 != points.end()) {
            done = false;
            point1 = point2;
        }
    }

    // Delete all marked points
    points.RemoveTaggedPoints(1, IsFilteredTag);

    // Remove tag from all remaining points
    points.RemoveAttribute(IsFilteredTag);

}
