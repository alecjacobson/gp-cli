#include <igl/read_triangle_mesh.h>
//#include <igl/write_triangle_mesh.h>
// Use igl::xml version to support .dae collada files, too
#include <igl/xml/write_triangle_mesh.h>
#include <igl/pathinfo.h>
#include <igl/embree/reorient_facets_raycast.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <cstdlib>

int main(int argc, char * argv[])
{
  using namespace std;
  using namespace Eigen;
  using namespace igl;
  MatrixXd V;
  MatrixXi F;
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
  reorient input.[mesh|msh|obj|off|ply|stl|wrl] output.[dae|mesh|obj|off|ply|stl|wrl]

)";
    return EXIT_FAILURE;
  }
  read_triangle_mesh(in,V,F);
  MatrixXi FF;
  {
    Eigen::VectorXi I;
    embree::reorient_facets_raycast(V,F,FF,I);
  }
  return xml::write_triangle_mesh(out,V,FF,igl::FileEncoding::Binary) ? 
      EXIT_SUCCESS : EXIT_FAILURE;
}
