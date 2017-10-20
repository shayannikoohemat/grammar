//
// Created by NikoohematS on 18-8-2017.
//

#include <iostream>
#include <string>
#include "LaserPoints.h"
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#include "indoor_graph.h"
#include "Annotator.h"
#include "Mls_Preparation.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/intrusive/set_hook.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/graph_utility.hpp>

/*using by_label_inx_t = boost::intrusive::set< VertexData, boost::intrusive::constant_time_size<false>,
        boost::intrusive::key_of_value<VertexData::by_label> >;*/
/// populate a tree from segments and their object-parents
//TODO: refactor input args, check if there is a need to write the relabeled points
SceneGraph Build_SceneGraph(LaserPoints & lp, char* output, bool out_segments_and_objects,
                bool sort_output, bool verbose)
{
    /* NOTE: no need for relabeled points, just segmented points is OK
     * for this algorithm to generate the graph */

    /// record system time
    std::clock_t start;
    double duration;
    start = std::clock();

    vector<LaserPoints> vec_segments;
    char* segments_out;
    //vec_segments = CollectSegmentLaserpoints(lp, output, verbose); // too expensive
    segments_out = out_segments_and_objects ? output : 0;
    vec_segments = LpToSegmentLpUsingBoost(lp, segments_out); // when output is zero nothing will be written to the disk

    /// we sort vec_segments to have a more readable output, otherwise it's not necessary
    if(sort_output) sort(vec_segments.begin(), vec_segments.end(), compare_lp_segmenttag);

    /// instantiate
    SceneGraph scene_graph;
    //typedef boost::labeled_graph<SceneGraph, std::string, boost::hash_mapS> Labeled_SceneGraph;
    //Labeled_SceneGraph scene_graph;

    typedef boost::graph_traits<SceneGraph>::vertex_descriptor vertex_t;
    typedef boost::graph_traits<SceneGraph>::edge_descriptor edge_t;
    boost::property_map<SceneGraph, boost::vertex_index_t>::type    index_map;
    vector<string>   obj_labels_v;
    std::map<std::string, size_t> vertexMap;

    /// for indexing of vertices similar thing to vertexMap
    //bi::set<VertexData, bi::constant_time_size<false>, bi::key_of_value<VertexData::by_label> > by_label_inx_t;

    bool obj_exist=false;
    LaserPoints relabeledpoints;
    vector <pair<int, double> > vec_seg_confidence;
    std::stringstream sstm_objname, sstm_label, sstm_segment, sstm_obj_longname;

    for (auto segment = vec_segments.begin(); segment != vec_segments.end(); segment++) {

        int seg_nr = (*segment)[0].Attribute(SegmentNumberTag);
        if (seg_nr < 0) seg_nr = -1;

        //// *** We relabel points in a segment based on the majority class of a label and
        //// majority class of a division
        int majority_division, majority_object;
        int majority_obj_count, majority_div_count;
        majority_object     = segment->MostFrequentAttributeValue(LabelTag, majority_obj_count);
        majority_division   = segment->MostFrequentAttributeValue(Label2Tag, majority_div_count);
        segment->SetAttribute(LabelTag, majority_object);
        segment->SetAttribute(Label2Tag, majority_division);
        relabeledpoints.AddPoints(*segment);
        /// confidence represent percentage of majority of object class
        double segment_confidence = majority_obj_count / (double) segment->size();
        auto segment_confidence_pair = std::make_pair(seg_nr, segment_confidence);
        vec_seg_confidence.push_back(segment_confidence_pair);

        /// generate object name
        int labeltag, label2tag;
        labeltag    = majority_object;
        label2tag   = majority_division;
        //labeltag  = (*segment)[0].Attribute(LabelTag); /// extra work
        //label2tag = (*segment)[0].Attribute(Label2Tag); /// extra work
        ObjNameAndNumber object;
        Annotator annotator;
        object  = annotator.TranslateObjectLabel(labeltag);
        string obj_name_str; // Clutter_1

        sstm_objname << object.obj_name << "_" << object.obj_number; // Clutter_1
        obj_name_str = sstm_objname.str();
        sstm_objname.str("");

        /// generate object name by label1 and label2
        string label1_label2;
        sstm_label << labeltag << "_" << label2tag; // 2001_150002
        label1_label2 = sstm_label.str();
        sstm_label.str("");

        /// generate obj label+(class-num)
        string obj_longname;
        sstm_obj_longname << label1_label2 << "(" << obj_name_str << ")";
        obj_longname = sstm_obj_longname.str();
        sstm_obj_longname.str("");


        /// check if this object label is already created
        if(std::find(obj_labels_v.begin(), obj_labels_v.end(), label1_label2) != obj_labels_v.end()){
            obj_exist = true;
        }else{
            obj_labels_v.push_back(label1_label2);
        }

        /// add segment vertex
        vertex_t v = boost::add_vertex(scene_graph); // this is the segment vertex
        /// convert seg_nr to string
        sstm_segment << seg_nr;
        string seg_nr_str;
        seg_nr_str = sstm_segment.str();
        sstm_segment.str("");
        /// assign properties
        scene_graph[v].obj_label     = seg_nr_str;
        scene_graph[v].obj_class_num = seg_nr_str;
        scene_graph[v].obj_long_name = seg_nr_str;
        scene_graph[v].num           = seg_nr;         // segment number
        scene_graph[v].confidence    = segment_confidence;
        scene_graph[v].is_leaf       = true;

        if (!obj_exist){  /// if object-vertex doesn't exist
            /// add segment's parent as an obj-vertex
            vertex_t o = boost::add_vertex(scene_graph);
            /// assign properties
            scene_graph[o].obj_label        = label1_label2;  // 2001_150002
            scene_graph[o].obj_class_num    = obj_name_str; // Wall_1
            scene_graph[o].obj_long_name    = obj_longname; // 2001_150002(Wall_1)
            scene_graph[o].is_object        = true;
            scene_graph[o].segments_list.push_back(seg_nr);
            //obj_vertices.push_back(o);

            /// Create the edge between vertex and its parent object
            edge_t e; bool b;
            boost::tie(e,b) = boost::add_edge(o,v,scene_graph); /// direction does matter
            /// add edge properties
            scene_graph[e].edge_confidence = segment_confidence;
            //boost::tie(e,b) = boost::add_edge(vec_segments[0],v,scene_graph);
            index_map = boost::get(boost::vertex_index, scene_graph);

            //auto source_obj = boost::source(e, scene_graph);
            //auto source_inx =  boost::get(index_map, source_obj);

            auto parent_inx = boost::get(index_map, o);
            auto segment_inx = boost::get(index_map, v);

            /// make a map of labels as keys and vertex-index
            vertexMap.insert(std::pair<std::string, size_t> (label1_label2, parent_inx));
            vertexMap.insert(std::pair<std::string, size_t> (seg_nr_str, segment_inx));
        } else { /// if object-vertex already exists
            /// find object-vertex index by its label in the vertexMap
            auto obj_vertex_inx = vertexMap.find(label1_label2)->second;

            /// Create the edge between vertex and its already existed parent object
            edge_t e; bool b;
            boost::tie(e,b) = boost::add_edge(obj_vertex_inx, v, scene_graph);  /// direction does matter
            /// add edge properties
            scene_graph[e].edge_confidence = segment_confidence;
            /// update index_map
            index_map = boost::get(boost::vertex_index, scene_graph);
            /// get the segment index
            auto segment_inx = boost::get(index_map, v);
            /// add segment as key and its vertex-index to the map
            vertexMap.insert(std::pair<std::string, size_t> (seg_nr_str, segment_inx));
            /// set obj_exist to false again
            obj_exist = false;
        }
    }

    /// define vertexMap
    //boost::intrusive::set< VertexData, boost::intrusive::constant_time_size<false>,
            //boost::intrusive::key_of_value<VertexData::by_label> > label_inx;
    //by_label_inx_t label_inx;
/*    auto reindex = [&] {
        label_inx.clear();
        for (auto vd : boost::make_iterator_range(boost::vertices(scene_graph))){
            //label_inx.insert(scene_graph[vd]);
        }
    };*/

/*    reindex();
    std::cout << "Index: " << by_label_idx_t.size() << " elements\n";

    scene_graph.clear();
    std::cout << "Index: " << by_label_idx_t.size() << " elements\n";

    for (auto& vertex : by_label_idx_t) {
        std::cout << vertex.label << " " << vertex.num << "\n";
    }*/


    /// Store objects graph based on their segments
    FILE *graph_segment_objet_output;
    char str_root[500];
    strcpy (str_root,output); // initialize the str_root with root string
    graph_segment_objet_output = fopen(strcat(str_root, "/graph_segment_objet_output.txt"),"w");
    fprintf(graph_segment_objet_output, "Object ID (object class_no) --> segment_no (confidence) \n");

    /// print neighbour vertices
    SceneGraph::vertex_iterator vertexIt, vertexEnd;
    tie(vertexIt, vertexEnd) = vertices(scene_graph);
    SceneGraph::adjacency_iterator neighbourIt, neighbourEnd;
    for (; vertexIt != vertexEnd; ++vertexIt)
    {
        //std::cout << get(&VertexData::obj_label, scene_graph)[*vertexIt] << " is connected with: ";
        /// if not leaf then is an object
        if(!scene_graph[*vertexIt].is_leaf) {
            std::cout << scene_graph[*vertexIt].obj_label << " (" <<
                      scene_graph[*vertexIt].obj_class_num << ")" << " --> ";

            /// print to the file
            fprintf(graph_segment_objet_output, "%13s (%11s ) --> ",
                    scene_graph[*vertexIt].obj_label.c_str(), scene_graph[*vertexIt].obj_class_num.c_str());

            /// loop in the leaves of one object
            tie(neighbourIt, neighbourEnd) = adjacent_vertices(*vertexIt, scene_graph);
            int count = 0;
            for (; neighbourIt != neighbourEnd; ++neighbourIt) {
                count++;
                //std::cout << " , " << get(&VertexData::obj_label, scene_graph)[*neighbourIt] << " ";
                std::cout << " ," << scene_graph[*neighbourIt].obj_label <<
                          " (" << std::fixed << std::setprecision(2) <<
                          scene_graph[*neighbourIt].confidence << ")" << " ";

                /// print to the file
                if (count == 1) {
                    fprintf(graph_segment_objet_output, "%s (%.2f)",
                            scene_graph[*neighbourIt].obj_label.c_str(), scene_graph[*neighbourIt].confidence);
                } else {
                    fprintf(graph_segment_objet_output, " | %s (%.2f)",
                            scene_graph[*neighbourIt].obj_label.c_str(), scene_graph[*neighbourIt].confidence);
                }
            }
            std::cout << "\n";
            fprintf(graph_segment_objet_output, "\n");
        }

    }
    fclose(graph_segment_objet_output);
    sstm_label.clear(); sstm_objname.clear(); sstm_segment.clear(); sstm_obj_longname.clear();

    /// print the output in graphviz with dot language
    boost::dynamic_properties dp;
    //dp.property("label", get(&VertexData::obj_label, scene_graph));
    //dp.property("label", get(&VertexData::obj_class_num, scene_graph));
    dp.property("label", get(&VertexData::obj_long_name, scene_graph));
    dp.property("weight", get(&VertexData::confidence, scene_graph));
    dp.property("label", get(&EdgeData::edge_confidence, scene_graph));
    //dp.property("color", get(&vertex_info::color, g));
    dp.property("node_id", get(boost::vertex_index, scene_graph));  // node_id for graphviz
    //BOOST_CHECK(read_graphviz(in, g, dp, "id"));

    /// Write out the graph
    //write_graphviz_dp(std::cout, scene_graph, dp, std::string("label"));
    /// print on the screen
    std::cout << "graph.dot out: " << endl;
    write_graphviz_dp(std::cout, scene_graph, dp, std::string("label"));

    /// Store objects graph based on their segments
    char *graph_dot;
    strcpy (str_root,output); // initialize the str_root with root string
    graph_dot = strcat(str_root, "/graph_.dot");
    std::ofstream graph_out(graph_dot);
    //write_graphviz(graph_out, scene_graph, boost::make_label_writer(get(&VertexData::obj_class_num, scene_graph)));
    write_graphviz_dp(graph_out, scene_graph, dp);

    /// customized properties writer
    //myEdgeWriter<SceneGraph> w(scene_graph);
    //boost::write_graphviz(graph_out,scene_graph,boost::default_writer(),w);
    //std::ofstream outf("E:/Laser_data/Stanford_annotated/out/graph_.gv");
    //write_graphviz_dp(outf, scene_graph, dp);


    /// another way of printing
    // std::cout << "Print Graph: \n";
    //boost::print_graph(scene_graph, boost::get(boost::vertex_bundle, scene_graph));

    return scene_graph;

    /// time record
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    printf("Total segments partitioning time: %.3f m \n", duration/60);  //debug
}


/// populate a tree from relabeled points, so each object is connected to its children (segments)
/// **NOTE**: the use of relabeled points for this function. Relabeled points means each segment
/// is already relabeled so it has just one parent-object
//TODO: refactor the input parameters, write the graph in an outputfile
void SegmentedObjectsToGraph(LaserPoints &relabeled_lp, char *output,
                             bool out_segments_and_objects, bool sort_output, bool verbose)
{


    vector<LaserPoints> vec_objects;
    char* out_objects;
    out_objects = out_segments_and_objects ? output : 0;
    vec_objects = PartitionLpByTag1Tag2(relabeled_lp, LabelTag, Label2Tag, out_objects);

    /// if necessary sort the output by labeltag
    //if (sort_output) sort(vec_objects.begin(), vec_objects.end(), compare_lp_labeltag);
    if (sort_output) sort(vec_objects.begin(), vec_objects.end(), compare_lp_label2tag); //sort by division

    typedef boost::adjacency_list<boost::vecS, boost::vecS,
            boost::bidirectionalS,
            VertexData,
            boost::property<boost::edge_weight_t, double, EdgeData> > SceneGraph;

    SceneGraph scene_graph;
    typedef boost::graph_traits<SceneGraph>::vertex_descriptor vertex_t;
    typedef boost::graph_traits<SceneGraph>::edge_descriptor edge_t;

    for (auto obj : vec_objects) {

        /// generate object name
        int obj_no, obj_div, labeltag, label2tag;
        labeltag = obj[0].Attribute(LabelTag);
        label2tag = obj[0].Attribute(Label2Tag);
        ObjNameAndNumber object;
        Annotator annotator;
        object = annotator.TranslateObjectLabel(labeltag);
        string obj_name_str;
        std::stringstream sstream;
        sstream << object.obj_name << "_" << object.obj_number; // Clutter_1
        obj_name_str = sstream.str();

        /// generate object name by label1 and label2
        string label1_label2;
        std::stringstream ss;
        ss << labeltag << "_" << label2tag; // 2001_150002
        label1_label2 = ss.str();

        //fprintf(segments_report_per_object, "%13s (%11s ) --> ", label1_label2.c_str(), obj_label_str.c_str());
        // generate a list of segments in an object
        vector<int> segment_numbers;
        segment_numbers = obj.AttributeValues(SegmentNumberTag);

        //auto v1 = boost::add_vertex(objects_graph);

        vertex_t v = boost::add_vertex(scene_graph); // this is the object vertex
        scene_graph[v].obj_label = label1_label2;         // 2001_150002
        scene_graph[v].obj_class_num = obj_name_str;    // Wall_1
        scene_graph[v].segments_list = segment_numbers; // 125, 125, ...

        for (int i =0 ; i < segment_numbers.size(); i++){

            vertex_t u = boost::add_vertex(scene_graph); // this is the segment vertex
            std::stringstream seg_s; seg_s << segment_numbers[i];
            scene_graph[u].obj_label = seg_s.str();
            scene_graph[u].num = segment_numbers[i];
            scene_graph[u].is_leaf = true;

            // Create an edge connecting segment and its parent (object)
            edge_t e; bool b;
            boost::tie(e,b) = boost::add_edge(v, u, scene_graph);
            scene_graph[e].edge_confidence = 0.0;

        }

    }

    SceneGraph::vertex_iterator vertexIt, vertexEnd;
    tie(vertexIt, vertexEnd) = vertices(scene_graph);

    /// print neighbour vertices
    SceneGraph::adjacency_iterator neighbourIt, neighbourEnd;
    for (; vertexIt != vertexEnd; ++vertexIt)
    {
        //std::cout << get(&VertexData::obj_label, scene_graph)[*vertexIt] << " is connected with: ";
        if(!scene_graph[*vertexIt].is_leaf)
            std::cout << scene_graph[*vertexIt].obj_label << " --> ";
        tie(neighbourIt, neighbourEnd) = adjacent_vertices(*vertexIt, scene_graph);
        for (; neighbourIt != neighbourEnd; ++neighbourIt)
            //std::cout << " , " << get(&VertexData::obj_label, scene_graph)[*neighbourIt] << " ";
            std::cout << " , " << scene_graph[*neighbourIt].obj_label << " ";
        std::cout << "\n";
    }
    /// print out- and in-edges
    SceneGraph::in_edge_iterator inedgeIt, inedgeEnd;
    SceneGraph::in_edge_iterator outedgeIt, outedgeEnd;
    //SceneGraph::out_edge_iterator outedgeIt, outedgeEnd;

}

/// This function builds a graph from already build objects graph and vector of object groups
void build_hierarchy_graph(SceneGraph sg, VecOfVec<string> vecofgroupobjs, char *output,
                             bool translate_labels , bool minimal_graph , bool verbose){

    SceneGraph harch_graph;
    harch_graph = sg;

    /// translate vecof_groupobjects from numbers to names: 2001_123053 -> Wall_1
    VecOfVec<string> vecof_translatedgroups;
    vecof_translatedgroups = translate_groups_to_objectnames(vecofgroupobjs);

    typedef boost::graph_traits<SceneGraph>::vertex_descriptor vertex_t;
    typedef boost::graph_traits<SceneGraph>::edge_descriptor edge_t;

    /// make index_map to access index of vertices
    boost::property_map<SceneGraph, boost::vertex_index_t>::type IndexMap;
    IndexMap = boost::get(boost::vertex_index, sg);

    /// make a map of label as string and vertex index
    std::map<std::string, vertex_t> vertexMap;
    /// make a map of indices and vertex labels
    SceneGraph::vertex_iterator vertexIt, vertexEnd;
    tie(vertexIt, vertexEnd) = vertices(sg);
    for (; vertexIt != vertexEnd; ++vertexIt){
        string label;
        label = sg[*vertexIt].obj_label;
        vertex_t v;
        v = IndexMap[*vertexIt];
        vertexMap.insert(std::pair<std::string, vertex_t> (label, v));
    }

    /// extend objects-graph (sg) by adding obj-groups as their parents
    /// loop through groupobjects and its translated version
    for(auto&& t : zip_range(vecofgroupobjs, vecof_translatedgroups)){
        //vector<string> objgroup;
        auto objgroup = boost::get<0>(t);
        auto objgroup_translated = boost::get<1>(t);

        //std::cout << t.get<0>() << " : " << t.get<1>() << "\n";
        /// build the harch_graph
        /// this is an objectgroup vertex
        vertex_t group_vertex = boost::add_vertex(harch_graph);

        /// make a group label
        std::stringstream sstm_longlabel, sstm_longlabel_translated;
        /// longlabel is concatination of children labels
        copy(objgroup.begin(),objgroup.end(), ostream_iterator<string>(sstm_longlabel,"/"));
        /// transltedlonglabel is a conversion from numbers to name 2001_123053 -> Wall_1
        copy(objgroup_translated.begin(),objgroup_translated.end(), ostream_iterator<string>(sstm_longlabel_translated,"/"));
        string grouplabel = sstm_longlabel.str();
        string grouplabel_translated = sstm_longlabel_translated.str();
        harch_graph[group_vertex].obj_label = grouplabel;
        harch_graph[group_vertex].obj_long_name = grouplabel_translated;

        for(auto&& obj : objgroup){ /// this is an object label: 2001_153025
            /// find object-vertex by its label in the vertexMap
            vertex_t obj_vertex = vertexMap.find(obj)->second;
            /// Create the edge between object-vertex and its parent group-object
            edge_t e; bool b;
            boost::tie(e,b) = boost::add_edge(group_vertex, obj_vertex, harch_graph); /// direction does matter
        }
    }

    /// print the output in graphviz with dot language
    boost::dynamic_properties dp;
    if(!translate_labels) dp.property("label", get(&VertexData::obj_label, harch_graph));
    //dp.property("label", get(&VertexData::obj_class_num, harch_graph));
    if(translate_labels) dp.property("label", get(&VertexData::obj_long_name, harch_graph));
    dp.property("weight", get(&VertexData::confidence, harch_graph));
    dp.property("label", get(&EdgeData::edge_confidence, harch_graph));
    dp.property("node_id", get(boost::vertex_index, harch_graph));  // node_id for graphviz

    /// print on the screen
    if(verbose){
        std::cout << "graph_group.dot out: " << endl;
        write_graphviz_dp(std::cout, harch_graph, dp, std::string("label"));
    }

    /// save the graph in graphviz dot format
    char str_root[500];
    char *graph_dot;
    strcpy (str_root,output); // initialize the str_root with root string
    graph_dot = strcat(str_root, "/graph_group_.dot");
    std::ofstream graph_out(graph_dot);
    //write_graphviz(graph_out, scene_graph, boost::make_label_writer(get(&VertexData::obj_class_num, harch_graph)));
    write_graphviz_dp(graph_out, harch_graph, dp);

    /// make a minimal graph for visulisation, collapse labels, remove clutter labels ,...
/*    if(minimal_graph){
        SceneGraph::vertex_iterator vIt, vEnd;
        SceneGraph::adjacency_iterator neighbourIt, neighbourEnd;
        tie(vIt, vIt) = vertices(harch_graph);
        for (; vIt != vEnd; ++vIt){

        }
    }*/

};


/// read a text file of annotated object-groups and convert them to a vector of groups
VecOfVec< string> read_object_groups(char *filename, int column_object,int column_division,
                         int column_group, bool verbose){

    std::ifstream infile(filename);
    if (!infile) {
        cerr << "Unable to open the file";
        exit(1);
    }

    /// just skip the first line
    infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n' );

    typedef boost::unordered::unordered_multimap<int, std::string> UnorderObjectGroups;
    UnorderObjectGroups umap;

    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream isstm(line);
        if (!isstm) { break; } /// if error
        vector <string> line_vec;
        string column;
        while (std::getline(isstm, column, ',')) {
            line_vec.push_back(column);
        }

        /// process the line_vec elements
        string div_str, obj_str, prev_group_no, group_no_str;
        int group_no;
        obj_str = line_vec[column_object-1]; // 2001  /// -1 because vector starts from 0
        div_str = line_vec[column_division-1]; // 153025
        group_no_str = line_vec[column_group-1]; // 1
        group_no = atoi(group_no_str.c_str());  /// stoi() doesn't work with this compiler

        std::stringstream sstm;
        sstm << obj_str << "_" << div_str;
        string obj_div_str;
        obj_div_str = sstm.str();

        umap.emplace(group_no, obj_div_str);

    }  /// end of file

    /// process the umap
    vector < vector< string> > result; /// vector of obj_div labels

    auto umap_begin = umap.begin();
    auto umap_end = umap.end();

    while (umap_begin != umap_end) {
        /// range is the iterator to pair of multimap objects
        auto range = umap.equal_range(umap_begin->first);

        std::vector<string> tmp;
        tmp.reserve(std::distance(range.first, range.second));
        /// loop through the iterators of range and collect strings with similar key
        string previous_value;
        for (auto i = range.first; i != range.second; ++i) {
            boost::move(i->second);
            /// if the value is not in the tmp vector then add it
            if(!(std::find(tmp.begin(), tmp.end(), i->second) != tmp.end())){
                tmp.push_back(i->second);
            }
        }
        result.push_back(std::move(tmp));
        umap_begin = range.second;
    }
    infile.close();
    std::cout << "\n number of object-groups: "<< result.size() << endl;

    /// print the result just for debug
    if (verbose){
        for (auto it=result.begin(); it != result.end(); ++it){

            std::string long_grouplabel;
            for (auto it_str=(*it).begin(); it_str != (*it).end(); it_str++){
                std::stringstream sstm_longlabel;
                copy((*it).begin(),(*it).end(), ostream_iterator<string>(sstm_longlabel,"/"));
                long_grouplabel = sstm_longlabel.str();
            }
            std::cout << long_grouplabel << endl;
        }
    }

    VecOfVec< string> vecofvec;
    vecofvec = result;
    return vecofvec;
}

/// convert longlabels of groups to names
VecOfVec<string> translate_groups_to_objectnames(VecOfVec<string> vec_v, bool verbose){

    vector< vector <string> > vec_v_new;
    for (auto vec : vec_v){ // vec: {2001_153025, 2002_153026, 4016_153025}

        vector <string> vec_new;
        for (auto s : vec){ // s: 2001_153025
            string obj_str;
            std::stringstream ss;
            ss.str(s);
            std::getline(ss, obj_str, '_'); // obj_str: 2001

            /// convert obj_str to obj class and number: 2001 -> Wall_1
            ObjNameAndNumber object;
            Annotator annotator;
            object  = annotator.TranslateObjectLabel(atoi(obj_str.c_str()));
            string obj_name_str;
            std::stringstream sstream;
            sstream << object.obj_name << "_" << object.obj_number; // Clutter_1
            obj_name_str = sstream.str();
            vec_new.push_back(obj_name_str);
            //cout << obj_name_str << endl; // debug
        }
        vec_v_new.push_back(vec_new);
    }

    /// print the result just for debug
    if (verbose){
        for (auto it=vec_v_new.begin(); it != vec_v_new.end(); ++it){

            std::string long_grouplabel;
            for (auto it_str=(*it).begin(); it_str != (*it).end(); it_str++){
                std::stringstream sstm_longlabel;
                copy((*it).begin(),(*it).end(), ostream_iterator<string>(sstm_longlabel,"/"));
                long_grouplabel = sstm_longlabel.str();
            }
            std::cout << long_grouplabel << endl;
        }
    }
    VecOfVec<string> vecOfvec;
    vecOfvec = vec_v_new;

    return vecOfvec;
}

/// read a batch of text files of annotated object-groups and convert them to a vector of groups
VecOfVec< string> read_object_groups_batch(char* directory, int column_object,int column_division,
                                     int column_group, bool verbose, string csv_file_extension){

    /// extract a vector of filespath
    vector<string> filespath;
    Annotator annotator;
    filespath = annotator.RecursiveDir(directory);

    // collect just files with requested extension in a vector
    vector <string> csv_filespath;
    vector <string>::iterator csv_file_it;
    for (csv_file_it = filespath.begin(); csv_file_it != filespath.end(); csv_file_it++){
        string extension; /// extension should have the "." in the head -> .csv
        extension = boost::filesystem::extension(*csv_file_it);
        if (extension == csv_file_extension){
            csv_filespath.push_back(*csv_file_it);
        }
    }
    VecOfVec<string> vecofvec_all; /// vector of all objectgroups, for all the files
    vector<string>::iterator filepath_it;
    for (filepath_it = csv_filespath.begin(); filepath_it != csv_filespath.end(); filepath_it++) {
        /// convert filepath string to char
        char *cfilepath = new char[filepath_it -> length() + 1];
        strcpy(cfilepath, filepath_it -> c_str());
        /// build a vector of objectgroups per file
        VecOfVec<string> vv;
        vv = read_object_groups(cfilepath, column_object, column_division, column_group, verbose);

        /// add vector of current file to the vector of other files
        vecofvec_all.insert(vecofvec_all.end(), vv.begin(), vv.end());
    }
    return vecofvec_all;
}

