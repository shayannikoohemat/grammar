//
// Created by NikoohematS on 11-8-2017.
//

//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee,
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/directed_graph.hpp>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
//#include <boost/test/unit_test.hpp>
//#include <boost/test/minimal.hpp>


using namespace boost;

/// labeled graph example
int labeledgraph()
{
    using AdjList = adjacency_list<setS, vecS, directedS>;
    using Graph = labeled_graph<AdjList, std::string, hash_mapS>;

    Graph g;

    for (std::string name : { "root", "vert_2", "vert_3" }) {
        add_vertex(name, g);
    }

    struct { std::string from, to; } edges [] = {
            {"root", "vert_2"},
            {"vert_2", "vert_3"},
            {"vert_2", "vert_3"},
            {"vert_2", "new_1"},
            {"new_1", "new_2"},
    };

    for (auto const& addition : edges) {
        if (g.vertex(addition.from) == g.null_vertex())
            std::cout << "source vertex (" << addition.from << ") not present\n";
        else if (g.vertex(addition.to) == g.null_vertex())
            std::cout << "target vertex (" << addition.to << ") not present\n";
        else {
            auto insertion = add_edge_by_label(addition.from, addition.to, g);
            std::cout << "Edge: (" << addition.from << " -> " << addition.to << ") "
                      << (insertion.second? "inserted":"already exists")
                      << "\n";
        }
    }
}



/// simples graph add_edge
typedef boost::adjacency_list<listS, vecS, undirectedS> undigraph;
int test()
{
    undigraph g;
    add_edge (0, 1, g);
    add_edge (0, 3, g);
    add_edge (1, 2, g);
    add_edge (2, 3, g);

    return 0;
}


/// traverse and print neighbor vertices example
typedef boost::adjacency_list<listS, vecS, directedS> mygraph;
int traverse()
{
    mygraph g;
    add_edge (0, 1, g);
    add_edge (0, 3, g);
    add_edge (1, 2, g);
    add_edge (2, 3, g);
    mygraph::vertex_iterator vertexIt, vertexEnd;
    mygraph::adjacency_iterator neighbourIt, neighbourEnd;
    tie(vertexIt, vertexEnd) = vertices(g);
    for (; vertexIt != vertexEnd; ++vertexIt)
    {
        std::cout << *vertexIt << " --> ";
        tie(neighbourIt, neighbourEnd) = adjacent_vertices(*vertexIt, g);
        for (; neighbourIt != neighbourEnd; ++neighbourIt)
            std::cout << " , " << *neighbourIt  << " ";
        std::cout << "\n";
    }
    return 0;
}

/// in_edge and out_edge example *not_working*
/*typedef boost::adjacency_list<listS, vecS, bidirectionalS> mybigraph;
int track_edges()
{
    mybigraph g;
    add_edge (0, 1, g);
    add_edge (0, 3, g);
    add_edge (1, 2, g);
    add_edge (2, 3, g);
    mybigraph::vertex_iterator vertexIt, vertexEnd;
    mybigraph::in_edge_iterator inedgeIt, inedgeEnd;
    mybigraph::in_edge_iterator outedgeIt, outedgeEnd; tie(vertexIt, vertexEnd) = vertices(g);
    for (; vertexIt != vertexEnd; ++vertexIt)
    {
        std::cout << "incoming edges for " << *vertexIt << ": ";
        tie(inedgeIt, inedgeEnd) = in_edges(*vertexIt, g);
        for(; inedgeIt != inedgeEnd; ++inedgeIt)
        {
            std::cout << *inedgeIt << " ";
        }
        std::cout << "\n";
    }
    for (; vertexIt != vertexEnd; ++vertexIt)
    {
        std::cout << "out-edges for " << *vertexIt << ": " ;
        tie(outedgeIt, outedgeEnd) = out_edges(*vertexIt, g); //  Similar to incoming edges
    }
    return 0;
}*/


/// access to the properties example
/*int graph_access()
{
    mybigraph g;
    boost::add_edge (0, 1, 8, g);
    add_edge (0, 3, 18, g);
    add_edge (1, 2, 20, g);
    add_edge (2, 3, 2, g);
    add_edge (3, 1, 1, g);
    add_edge (1, 3, 7, g);
    std::cout << "Number of edges: " << num_edges(g) << "\n";
    std::cout << "Number of vertices: " << num_vertices(g) << "\n";
    mybigraph::vertex_iterator vertexIt, vertexEnd; tie(vertexIt, vertexEnd) = vertices(g);
    for (; vertexIt != vertexEnd; ++vertexIt)
    {
        std::cout << "in-degree for " << *vertexIt << ": "
                  << in_degree(*vertexIt, g) << "\n";
        std::cout << "out-degree for " << *vertexIt << ": "
                  << out_degree(*vertexIt, g) << "\n";
    }
    mybigraph::edge_iterator edgeIt,
            edgeEnd; tie(edgeIt, edgeEnd) = edges(g);
    for (; edgeIt!= edgeEnd; ++edgeIt)
    { std::cout << "edge " << source(*edgeIt, g) << "-->"
                << target(*edgeIt, g) << "\n";
    }
}*/


/// example of a family tree
/*enum family
{ Jeanie, Debbie, Rick, John, Amanda, Margaret, Benjamin, N };

int family_tree(){
    using namespace boost;
    const char *name[] = { "Jeanie", "Debbie", "Rick", "John", "Amanda",
                           "Margaret", "Benjamin"
    };

    adjacency_list <> g(N);
    add_edge(Jeanie, Debbie, g);
    add_edge(Jeanie, Rick, g);
    add_edge(Jeanie, John, g);
    add_edge(Debbie, Amanda, g);
    add_edge(Rick, Margaret, g);
    add_edge(John, Benjamin, g);

    graph_traits < adjacency_list <> >::vertex_iterator i, end;
    graph_traits < adjacency_list <> >::adjacency_iterator ai, a_end;
    property_map < adjacency_list <>, vertex_index_t >::type
            index_map = get(vertex_index, g);

    for (boost::tie(i, end) = vertices(g); i != end; ++i) {
        std::cout << name[get(index_map, *i)];
        boost::tie(ai, a_end) = adjacent_vertices(*i, g);
        if (ai == a_end)
            std::cout << " has no children";
        else
            std::cout << " is the parent of ";
        for (; ai != a_end; ++ai) {
            std::cout << name[get(index_map, *ai)];
            if (boost::next(ai) != a_end)
                std::cout << ", ";
        }
        std::cout << std::endl;
    }

    boost::write_graphviz(std::cout, g);
    return EXIT_SUCCESS;
}*/


/// graphviz test
int graphvis_out(){

    struct VertexProperties{
        int v_no;
    };

    struct EdgeProperty{
        int e_no;
        std::string e_name;
    };

    // Graph type
    typedef adjacency_list<vecS, vecS, directedS, VertexProperties, EdgeProperty> Graph;
    Graph g;
    std::vector<std::string> NameVec; // for dot file names


// write the dot file
    std::string strDotFile = "E:/Laser_data/Stanford_annotated/out/dotfile.gv";
    std::ofstream dotfile (strDotFile.c_str ());
    write_graphviz (dotfile, g, make_label_writer(&NameVec[0]));
}


struct vertex_info {
    int color;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, vertex_info> Graph;
typedef std::pair<int, int> Edge;

int graphviz2(void) {
    Graph g;
    add_edge(0, 1, g);
    add_edge(1, 2, g);

    // replace this with some traversing and assigning of colors to the 3 vertices  ...
    // should I use bundled properties for this?
    // it's unclear how I would get write_graphviz to recognize a bundled property as the color attribute
    g[0].color = 1;
    std::ofstream outf("E:/Laser_data/Stanford_annotated/out/min.gv");

    boost::dynamic_properties dp;
    dp.property("color", get(&vertex_info::color, g));
    dp.property("node_id", get(boost::vertex_index, g));
    write_graphviz_dp(outf, g, dp);
}

// Author: Douglas Gregor



/*void test_graph_read_write(const std::string& filename)
{

    typedef boost::adjacency_list<vecS, vecS, directedS,
            property<vertex_name_t, std::string>,
            property<edge_weight_t, double> > Digraph;

    typedef boost::adjacency_list<vecS, vecS, undirectedS,
            property<vertex_name_t, std::string>,
            property<edge_weight_t, double> > Graph;

    std::ifstream in(filename.c_str());
    BOOST_REQUIRE(in);

    Graph g;
    dynamic_properties dp;
    dp.property("id", get(vertex_name, g));
    dp.property("weight", get(edge_weight, g));
    BOOST_CHECK(read_graphviz(in, g, dp, "id"));

    BOOST_CHECK(num_vertices(g) == 4);
    BOOST_CHECK(num_edges(g) == 4);

    typedef graph_traits<Graph>::vertex_descriptor Vertex;

    std::map<std::string, Vertex> name_to_vertex;
    BGL_FORALL_VERTICES(v, g, Graph)
            name_to_vertex[get(vertex_name, g, v)] = v;

    // Check vertices
    BOOST_CHECK(name_to_vertex.find("0") != name_to_vertex.end());
    BOOST_CHECK(name_to_vertex.find("1") != name_to_vertex.end());
    BOOST_CHECK(name_to_vertex.find("foo") != name_to_vertex.end());
    BOOST_CHECK(name_to_vertex.find("bar") != name_to_vertex.end());

    // Check edges
    BOOST_CHECK(edge(name_to_vertex["0"], name_to_vertex["1"], g).second);
    BOOST_CHECK(edge(name_to_vertex["1"], name_to_vertex["foo"], g).second);
    BOOST_CHECK(edge(name_to_vertex["foo"], name_to_vertex["bar"], g).second);
    BOOST_CHECK(edge(name_to_vertex["1"], name_to_vertex["bar"], g).second);

    BOOST_CHECK(get(edge_weight, g,
                    edge(name_to_vertex["0"], name_to_vertex["1"], g).first)
                == 3.14159);
    BOOST_CHECK(get(edge_weight, g,
                    edge(name_to_vertex["1"], name_to_vertex["foo"], g).first)
                == 2.71828);
    BOOST_CHECK(get(edge_weight, g,
                    edge(name_to_vertex["foo"], name_to_vertex["bar"], g).first)
                == 10.0);
    BOOST_CHECK(get(edge_weight, g,
                    edge(name_to_vertex["1"], name_to_vertex["bar"], g).first)
                == 10.0);

    // Write out the graph
    write_graphviz_dp(std::cout, g, dp, std::string("id"));
}*/

/*int test_main(int, char*[])
{
    test_graph_read_write("graphviz_example.dot");

    return 0;
}*/


enum files_e { dax_h, yow_h, boz_h, zow_h, foo_cpp,
    foo_o, bar_cpp, bar_o, libfoobar_a,
    zig_cpp, zig_o, zag_cpp, zag_o,
    libzigzag_a, killerapp, N };
const char* name[] = { "dax.h", "yow.h", "boz.h", "zow.h", "foo.cpp",
                       "foo.o", "bar.cpp", "bar.o", "libfoobar.a",
                       "zig.cpp", "zig.o", "zag.cpp", "zag.o",
                       "libzigzag.a", "killerapp" };

int graphvis_writelabels()
{

    typedef std::pair<int,int> Edge;
    Edge used_by[] = {
            Edge(dax_h, foo_cpp), Edge(dax_h, bar_cpp), Edge(dax_h, yow_h),
            Edge(yow_h, bar_cpp), Edge(yow_h, zag_cpp),
            Edge(boz_h, bar_cpp), Edge(boz_h, zig_cpp), Edge(boz_h, zag_cpp),
            Edge(zow_h, foo_cpp),
            Edge(foo_cpp, foo_o),
            Edge(foo_o, libfoobar_a),
            Edge(bar_cpp, bar_o),
            Edge(bar_o, libfoobar_a),
            Edge(libfoobar_a, libzigzag_a),
            Edge(zig_cpp, zig_o),
            Edge(zig_o, libzigzag_a),
            Edge(zag_cpp, zag_o),
            Edge(zag_o, libzigzag_a),
            Edge(libzigzag_a, killerapp)
    };
    const int nedges = sizeof(used_by)/sizeof(Edge);
    int weights[nedges];
    std::fill(weights, weights + nedges, 1);

    using namespace boost;

    typedef adjacency_list< vecS, vecS, directedS,
            property< vertex_color_t, default_color_type >,
            property< edge_weight_t, int >
    > Graph;
    Graph g(used_by, used_by + nedges, weights, N);

    //write_graphviz(std::cout, g, make_label_writer(name));

    std::ofstream outf("E:/Laser_data/Stanford_annotated/out/labeled_graph.dot"); // or labeled_graph.gv
    write_graphviz(outf, g, make_label_writer(name));

    //***NOTE***: to generate a png using graphviz dot language use this command in cmd
    // >> dot -Tpng -o x.png x.dot

}

