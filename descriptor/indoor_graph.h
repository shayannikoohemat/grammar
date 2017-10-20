//
// Created by NikoohematS on 18-8-2017.
//

#include <string>
#include <iostream>
#include <vector>
#include <boost/range/size.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/intrusive/set_hook.hpp>
#include <boost/intrusive/set.hpp>

#ifndef DESCRIPTOR_INDOOR_GRAPH_H
#define DESCRIPTOR_INDOOR_GRAPH_H

#endif //DESCRIPTOR_INDOOR_GRAPH_H

namespace bi = boost::intrusive;

/// vertex properties
struct VertexData : boost::intrusive::set_base_hook<bi::link_mode<bi::auto_unlink>, bi::constant_time_size<false> >
{
    std::string obj_label; // concatenation of labels:    20011_153027
    std::string obj_class_num; //object class and number:     wall_1
    std::string obj_long_name; // object name, label, number: 20011_153027 (wall_1)
    int num;
    double confidence=0.0;
    std::vector <int> segments_list;
    bool is_leaf=false;
    bool is_object=false;

    //VertexData(std::string label, int num) : obj_label(label), num(num) {}

    struct by_label{
        std::string const& operator()(VertexData const& vd) const { return vd.obj_label; }
    };
};

/// edges properties
struct EdgeData
{
    std::string edge_name;
    double edge_confidence=0.0;
};

/// define the boost-graph from BoostGraphLibrary (BGL)
typedef boost::adjacency_list<boost::vecS, boost::vecS,
        boost::bidirectionalS,
        VertexData,
        boost::property<boost::edge_weight_t, double, EdgeData> > SceneGraph;

/// make zip template for range-based loops
template<typename T>
using VecOfVec = std::vector< std::vector<T> >;

template<class... Conts>
auto zip_range(Conts&... conts) -> decltype(boost::make_iterator_range(
        boost::make_zip_iterator(boost::make_tuple(conts.begin()...)),
        boost::make_zip_iterator(boost::make_tuple(conts.end()...))))
{
    return {boost::make_zip_iterator(boost::make_tuple(conts.begin()...)),
            boost::make_zip_iterator(boost::make_tuple(conts.end()...))};
}


/// generate a filtered graph by its vertex-label
/*template <typename TLabel>
struct filter_by_label {
    filter_by_label() { }
    filter_by_label()
};*/


/// graphviz customize edgewrtier
template <class Name>
class myEdgeWriter {
public:
    myEdgeWriter(Name _name) : name(_name) {}
    template <class Vertex>
    void operator()(std::ostream& out, const Vertex& v) const {
        out << "[label=\"" << name[v].edge_name << "\"]";
        out << "graph [bgcolor=lightgrey]" << std::endl;
        out << "node [shape=circle color=white]" << std::endl;
        //out <<       "[style=filled, fillcolor=red]" << std::endl;
        out << "edge [style=dashed]" << std::endl;
    }
private:
    Name name;  /// name here is an instance of the graph
};

SceneGraph Build_SceneGraph(LaserPoints & lp, char* output, bool out_segments_and_objects,
                bool sort_output, bool verbose);

void SegmentedObjectsToGraph(LaserPoints & relabeled_lp, char* output, bool out_segments_and_objects,
                             bool sort_output, bool verbose);

/// read a text file of annotated object-groups and convert them to a vector of groups
VecOfVec< string> read_object_groups(char *filename, int column_object, int column_division,
                         int column_group, bool verobse = false);

/// convert longlabels of groups to names
VecOfVec< string> translate_groups_to_objectnames(vector < vector< string> > vec_v,
                                                          bool verbose=false);

/// read a directory of csv-files of annotated object-groups and convert them to a vector of groups
/// NOTE: column numbers start from 1 not zero
VecOfVec< string> read_object_groups_batch(char* directory, int column_object,int column_division,
                                           int column_group, bool verbose, string csv_file_extension=".csv");

void build_hierarchy_graph(SceneGraph sg, VecOfVec<string> vecofgroupobjs, char *output,
                            bool translate_labels=true, bool minimal_graph=false ,bool verbose=false);


/*
typedef adjacency_list<vecS, vecS, undirectedS, no_property, edge_property_c> graph_t;
typedef graph_t::edge_descriptor edge_id_t;
enum edge_type_e {
    A, B, C, D
};

class edge_predicate_c {
public:
    edge_predicate_c() : graph_m(0) {}
    edge_predicate_c(graph_t& graph) : graph_m(&graph) {}
    bool operator()(const edge_id_t& edge_id) const {
        edge_type_e type = (*graph_m)[edge_id].type_m;
        return (type == A || type == B);
    }
private:
    graph_t* graph_m;
};

graph_t g(n);
boost::filtered_graph<graph_t, edge_predicate_c> fg(g, edge_predicate_c(g));*/


/*template <class Graph>
class my_vertex_writer {
public:
    my_vertex_writer(Graph& g) : g_(g) {}

    template <class Vertex>
    void operator()(std::ostream& out, const Vertex& v) const {
        // pseudo-code!
        if (0 == boost::size(in_edges(v, g_)))
            out << "[style=\"invis\"]";
    }
private:
    Graph& g_;
};*/

/*template <class WeightMap,class CapacityMap>
 *
 * https://stackoverflow.com/questions/11369115/how-to-print-a-graph-in-graphviz-with-multiple-properties-displayed
class edge_writer {
public:
    edge_writer(WeightMap w, CapacityMap c) : wm(w),cm(c) {}
    template <class Edge>
    void operator()(ostream &out, const Edge& e) const {
        out << "[label=\"" << wm[e] << "\", taillabel=\"" << cm[e] << "\"]";
    }
private:
    WeightMap wm;
    CapacityMap cm;
};

template <class WeightMap, class CapacityMap>
inline edge_writer<WeightMap,CapacityMap>
make_edge_writer(WeightMap w,CapacityMap c) {
    return edge_writer<WeightMap,CapacityMap>(w,c);
}*/
