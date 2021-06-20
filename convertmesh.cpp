#include <igl/read_triangle_mesh.h>
//#include <igl/write_triangle_mesh.h>
// Use igl::xml version to support .dae collada files, too
#include <igl/xml/write_triangle_mesh.h>
#include <igl/readMSH.h>
#include <igl/readMESH.h>
#include <igl/writeMESH.h>
#include <igl/boundary_facets.h>
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
  convertmesh input.[mesh|msh|obj|off|ply|stl|wrl] output.[dae|mesh|obj|off|ply|stl|wrl]

  Note: .ply and .stl outputs are binary.
)";
    return EXIT_FAILURE;
  }
  const auto is_tet = [](const std::string & path)->bool
  {
    std::string d,b,e,f;
    igl::pathinfo(path,d,b,e,f);
    std::transform(e.begin(), e.end(), e.begin(), ::tolower);
    return e == "mesh" || e == "msh";
  };
  if( is_tet(in) && is_tet(out) )
  {
    const auto read_tet_mesh = [](
      const std::string & path,
      Eigen::MatrixXd & V,
      Eigen::MatrixXi & T,
      Eigen::MatrixXi & F)->bool
    {
      std::string d,b,e,f;
      igl::pathinfo(path,d,b,e,f);
      std::transform(e.begin(), e.end(), e.begin(), ::tolower);
      if(e == "mesh")
      {
        return igl::readMESH(path,V,T,F);
      }else if(e == "msh")
      {
        Eigen::VectorXi _1,_2;
        bool res = igl::readMSH(path,V,F,T,_1,_2);
        if(F.rows() == 0 && T.rows()>0)
        {
          igl::boundary_facets(T,F);
          F = F.rowwise().reverse().eval();
        }
        return res;
      }else
      {
        std::cerr<<"Error: "<<
          e<<" is not a supported tet mesh file format."<<std::endl;
        return false;
      }
    };
    const auto write_tet_mesh = [](
      const std::string & path,
      const Eigen::MatrixXd & V,
      const Eigen::MatrixXi & T,
      const Eigen::MatrixXi & F)->bool
    {
      std::string d,b,e,f;
      igl::pathinfo(path,d,b,e,f);
      std::transform(e.begin(), e.end(), e.begin(), ::tolower);
      if(e == "mesh")
      {
        return igl::writeMESH(path,V,T,F);
      }else
      {
        std::cerr<<"Error: "<<
          e<<" is not a supported tet mesh file format."<<std::endl;
        return false;
      }
    };
    return 
      read_tet_mesh(in,V,T,F) && write_tet_mesh(out,V,T,F) ? 
      EXIT_SUCCESS : EXIT_FAILURE;
  }else
  {
    return 
      read_triangle_mesh(in,V,F) && 
      xml::write_triangle_mesh(out,V,F,igl::FileEncoding::Binary) ? 
        EXIT_SUCCESS : EXIT_FAILURE;
  }
}
