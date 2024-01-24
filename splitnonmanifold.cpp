#include <igl/read_triangle_mesh.h>
// Use igl::xml version to support .dae collada files, too
#include <igl/xml/write_triangle_mesh.h>
#include <igl/pathinfo.h>
#include <igl/split_nonmanifold.h>
#include <igl/matlab_format.h>

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
  splitnonmanifold input.[mesh|msh|obj|off|ply|stl|wrl] output.[dae|mesh|obj|off|ply|stl|wrl]

  Note: .ply and .stl outputs are binary.
)";
    return EXIT_FAILURE;
  }
  if(!read_triangle_mesh(in,V,F)) return EXIT_FAILURE;
  {
    Eigen::VectorXi _1;
    igl::split_nonmanifold( 
      Eigen::MatrixXd(V),Eigen::MatrixXi(F),
      V,F,_1);
    std::cout<<igl::matlab_format(V,"cV")<<std::endl;
    std::cout<<igl::matlab_format_index(F,"cF")<<std::endl;
  }
  return xml::write_triangle_mesh(out,V,F,igl::FileEncoding::Binary) ? 
      EXIT_SUCCESS : EXIT_FAILURE;
}
