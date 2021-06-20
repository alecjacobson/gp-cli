#include <Eigen/Dense>
#include <igl/read_triangle_mesh.h>
// Use igl::xml version to support .dae collada files, too
#include <igl/xml/write_triangle_mesh.h>
#include <igl/copyleft/cgal/mesh_boolean.h>
#include <igl/copyleft/cgal/piecewise_constant_winding_number.h>
#include <iostream>
#include <string>

int main(int argc, char * argv[])
{
  using namespace Eigen;
  using namespace std;
  using namespace igl;
  using namespace igl::copyleft::cgal;
  Eigen::MatrixXd VA,VB,VC;
  Eigen::MatrixXi FA,FB,FC;
  string A_filename,B_filename,C_filename;
  string type_str = "union";
  MeshBooleanType type = MESH_BOOLEAN_TYPE_UNION;
  if(argc>=5)
  {
    A_filename = argv[1];
    B_filename = argv[2];
    type_str = argv[3];
    C_filename = argv[4];
  }else
  {
    cerr<<R"(Usage:

    ./meshboolean [path to mesh A] [path to mesh B] [type] [path to output mesh C]

Where [type] is one of the following: "union", "intersect", "minus", or "xor"

Paths may be to .obj, .off, .ply, .stl, or .wrl files.

For example,

    ./meshboolean A.obj B.off union C.stl
)"<<endl;
    return EXIT_FAILURE;
  }
  if(type_str == "union")
  {
    type = MESH_BOOLEAN_TYPE_UNION;
  }else if(type_str == "intersect")
  {
    type = MESH_BOOLEAN_TYPE_INTERSECT;
  }else if(type_str == "minus")
  {
    type = MESH_BOOLEAN_TYPE_MINUS;
  }else if(type_str == "xor")
  {
    type = MESH_BOOLEAN_TYPE_XOR;
  }else if(type_str == "resolve")
  {
    type = MESH_BOOLEAN_TYPE_RESOLVE;
  }else
  {
    cerr<<"Error: unknown type ("<<type_str<<")"<<endl;
    return EXIT_FAILURE;
  }
  if(!read_triangle_mesh(A_filename,VA,FA))
  {
    cerr<<"Error: couldn't read "<<A_filename<<endl;
    return EXIT_FAILURE;
  }
  if(B_filename == "-")
  {
    VB.resize(0,3);
    FB.resize(0,3);
  }else if(!read_triangle_mesh(B_filename,VB,FB))
  {
    cerr<<"Error: couldn't read "<<B_filename<<endl;
    return EXIT_FAILURE;
  }
  if(!igl::copyleft::cgal::piecewise_constant_winding_number(VA,FA))
  {
    cerr<<"Input mesh ("<<A_filename<<") is not PWN."<<endl;
    return EXIT_FAILURE;
  }
  if(!igl::copyleft::cgal::piecewise_constant_winding_number(VB,FB))
  {
    cerr<<"Input mesh ("<<B_filename<<") is not PWN."<<endl;
    return EXIT_FAILURE;
  }
  VectorXi J;
  mesh_boolean(VA,FA,VB,FB,type,VC,FC,J);
  if(!write_triangle_mesh(C_filename,VC,FC))
  {
    cerr<<"Error: couldn't write to "<<C_filename<<endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
