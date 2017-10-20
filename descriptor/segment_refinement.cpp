//
// Created by NikoohematS on 11/23/2016.
//
#include <iostream>
#include <LaserPoints.h>
#include "Mls_Preparation.h"
#include <ctime>

LaserPoints segment_refinement(LaserPoints segmented_lp, int min_segment_size,
                               double maxdistanceInComponent, int method=1) {
    method =1;
    switch(method){
        case 1: {

            //vector <int> seg_numbers;
            //seg_numbers = segmented_lp.AttributeValues(SegmentNumberTag);
            vector<LaserPoints> vec_segments;
            vec_segments = LpToSegmentLpUsingBoost(segmented_lp, 0); // when output is zero nothing will be written to the disk

            /// we sort vec_segments by size of segments , descending
            //sort(vec_segments.begin(), vec_segments.end(), compare_lp_size); // unnecessary operation

            SegmentationParameters *seg_parameter;
            seg_parameter = new SegmentationParameters;
            int new_segment_nr=0;
            LaserPoints resegment_lpoints;

            cout << "# laserpoints " << " # segment points " << endl;
            for (auto seg_it : vec_segments){
                if(seg_it.size() > min_segment_size && seg_it.begin()->HasAttribute(SegmentNumberTag) &&
                   seg_it.begin()->Attribute(SegmentNumberTag) >=0){

                    printf("%10d %10d\r", segmented_lp.size(), seg_it.size());
                    TINEdges *edges;
                    edges = seg_it.DeriveEdges(*seg_parameter);
                    seg_parameter->MaxDistanceInComponent() = maxdistanceInComponent;
                    seg_it.RemoveLongEdges(edges->TINEdgesRef(),
                                           seg_parameter->MaxDistanceInComponent(),
                                           seg_parameter->DistanceMetricDimension() == 2);
                    seg_it.LabelComponents(edges->TINEdgesRef(), SegmentNumberTag);

                    /// partition each segment to smaller segments
                    vector<LaserPoints> segments_of_segment;
                    segments_of_segment = LpToSegmentLpUsingBoost(seg_it, 0);
                    //sort(segments_of_segment.begin(), segments_of_segment.end(), compare_lp_size); //unnecessary operation

                    for(auto it : segments_of_segment){
                        if(it.size() > min_segment_size){
                            new_segment_nr++;
                            it.SetAttribute(SegmentNumberTag, new_segment_nr);
                            resegment_lpoints.AddPoints(it);
                        }
                    }
                }
            }
            return resegment_lpoints;
        }
        case 2: {
            /// resegmenting laserpoints by removing longedges in the TIN and removing deformed segments
            LaserPoints relabeled_lpoints;
            int new_segment_number = 0;

            printf("Splitting strange shaped segments... & removing small (%d points) segments:\n", min_segment_size);
            if (segmented_lp.size() > min_segment_size) {
                if (segmented_lp.HasAttribute(SegmentNumberTag)) { /// check if points have segment attribute
                    int dominanat_segment_nr1, count1;
                    LaserPoints segment1_lpoints;
                    SegmentationParameters *seg_parameter;
                    seg_parameter = new SegmentationParameters;
                    cout << "# laserpoints " << " # segment points " << endl;
                    do {
                        /// get the segment_number and count of points for the dominanat segment
                        dominanat_segment_nr1 = segmented_lp.MostFrequentAttributeValue(SegmentNumberTag, count1);
                        segment1_lpoints.ErasePoints();
                        /// save points of the dominant segment
                        segment1_lpoints.AddTaggedPoints(segmented_lp, dominanat_segment_nr1, SegmentNumberTag);

                        //segment1_lpoints.Write("E:/Laser_data/Stanford_annotated/crop/dom_seg1.laser", false); //debug

                        printf("%10d %10d\r", segmented_lp.size(), segment1_lpoints.size());

                        TINEdges *edges;
                        edges = segment1_lpoints.DeriveEdges(*seg_parameter);
                        seg_parameter->MaxDistanceInComponent() = maxdistanceInComponent;
                        segment1_lpoints.RemoveLongEdges(edges->TINEdgesRef(),
                                                         seg_parameter->MaxDistanceInComponent(),
                                                         seg_parameter->DistanceMetricDimension() == 2);
                        segment1_lpoints.LabelComponents(edges->TINEdgesRef(), SegmentNumberTag);

                        /// do the same for 2nd dominanat segment
                        int dominanat_segment_nr2, count2;
                        LaserPoints segment2_lpoints;
/*                        segment1_lpoints.Write("E:/Laser_data/Stanford_annotated/crop/dom_seg1_segmented.laser",
                                               false); //debug*/

                        do {
                            /// get the segment_number for the dominanat segment
                            dominanat_segment_nr2 = segment1_lpoints.MostFrequentAttributeValue(SegmentNumberTag,
                                                                                                count2);
                            if (count2 > min_segment_size) {
                                new_segment_number++;
                                segment2_lpoints.ErasePoints();
                                segment2_lpoints.AddTaggedPoints(segment1_lpoints, dominanat_segment_nr2,
                                                                 SegmentNumberTag);
                                segment2_lpoints.SetAttribute(SegmentNumberTag, new_segment_number);

                                //segment2_lpoints.Write("E:/Laser_data/Stanford_annotated/crop/dom_seg2.laser", false); // debug

                                relabeled_lpoints.AddPoints(segment2_lpoints);
                                segment1_lpoints.RemoveTaggedPoints(dominanat_segment_nr2, SegmentNumberTag);
                            }

                        } while (count2 > min_segment_size);
                        segmented_lp.RemoveTaggedPoints(dominanat_segment_nr1, SegmentNumberTag);
                        dominanat_segment_nr1 = segmented_lp.MostFrequentAttributeValue(SegmentNumberTag, count1);
                    } while (count1 > min_segment_size);
                }
                printf("\n");
                //relabeled_lpoints.Write("D://test//indoorgraph//data//segment_refine_relabeled.laser", false);
            }
            return relabeled_lpoints;
        }

        default:
            return 0;
    }
}




