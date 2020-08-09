#include <igl/read_triangle_mesh.h>
//#include <igl/write_triangle_mesh.h>
// Use igl::xml version to support .dae collada files, too
#include <igl/xml/write_triangle_mesh.h>
#include <igl/pathinfo.h>
#include <igl/qslim.h>
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
  double f_in = 0.1;
  switch(argc)
  {
    case 4:
      f_in = atof(argv[1]);
      in = argv[2];
      out = argv[3];
      break;
    default:
      cerr<<R"(
USAGE:
  decimate [fraction] input.[mesh|msh|obj|off|ply|stl|wrl] output.[dae|mesh|obj|off|ply|stl|wrl]

or 

  decimate [whole number] input.[mesh|msh|obj|off|ply|stl|wrl] output.[dae|mesh|obj|off|ply|stl|wrl]

)";
    return EXIT_FAILURE;
  }
  read_triangle_mesh(in,V,F);
  const int max_m = (f_in < 1?f_in*F.rows(): f_in);
  MatrixXd dV;
  MatrixXi dF;
  {
    Eigen::VectorXi J,I;
    qslim(V,F,max_m,dV,dF,J,I);
  }
  return xml::write_triangle_mesh(out,dV,dF,false) ? 
      EXIT_SUCCESS : EXIT_FAILURE;
}
