#include <igl/read_triangle_mesh.h>
#include <igl/remove_duplicate_vertices.h>
//#include <igl/write_triangle_mesh.h>
// Use igl::xml version to support .dae collada files, too
#include <igl/xml/write_triangle_mesh.h>
#include <igl/pathinfo.h>
#include <algorithm>
#include <string>
#include <iostream>

int main(int argc, char * argv[])
{
  using namespace std;
  using namespace Eigen;
  using namespace igl;
  MatrixXd V;
  MatrixXi T,F;
  string in,out;
  switch(argc)
  {
    case 3:
      in = argv[1];
      out = argv[2];
      break;
    default:
      cerr<<R"(
USAGE:
  mergestl input.[mesh|msh|obj|off|ply|stl|wrl] output.[dae|mesh|obj|off|ply|stl|wrl]

  Note: .ply and .stl outputs are binary.
)";
    return EXIT_FAILURE;
  }
  if(!read_triangle_mesh(in,V,F)) return EXIT_FAILURE;
  {
    Eigen::VectorXi _1,_2;
    igl::remove_duplicate_vertices( 
      Eigen::MatrixXd(V),Eigen::MatrixXi(F),0,V,_1,_2,F);
  }
  return xml::write_triangle_mesh(out,V,F,false) ? 
      EXIT_SUCCESS : EXIT_FAILURE;
}
