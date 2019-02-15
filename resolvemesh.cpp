#include <igl/read_triangle_mesh.h>
//#include <igl/write_triangle_mesh.h>
// Use igl::xml version to support .dae collada files, too
#include <igl/xml/write_triangle_mesh.h>
#include <igl/copyleft/cgal/remesh_self_intersections.h>

#include <algorithm>
#include <string>
#include <iostream>

int main(int argc, char * argv[])
{
  using namespace std;
  using namespace Eigen;
  using namespace igl;
  MatrixXd Vin,Vout;
  MatrixXi Fin,Fout;
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
  resolvemesh input.[obj|off|ply|stl|wrl] output.[dae|obj|off|ply|stl|wrl]

  Note: .ply and .stl outputs are binary.
)";
    return EXIT_FAILURE;
  }
  if(!read_triangle_mesh(in,Vin,Fin))
  {
    return EXIT_FAILURE;
  }
  {
    Eigen::MatrixXi _1;
    Eigen::VectorXi _2,_3;
    copyleft::cgal::remesh_self_intersections(
      Vin,Fin,
      {},
      Vout,Fout,
      _1,_2,_3);
  }
  if(!write_triangle_mesh(out,Vout,Fout))
  {
    return EXIT_FAILURE;
  }
}

