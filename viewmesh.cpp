#include <igl/read_triangle_mesh.h>
#include <igl/opengl/glfw/Viewer.h>
#include <algorithm>
#include <string>
#include <iostream>

int main(int argc, char * argv[])
{
  using namespace std;
  using namespace Eigen;
  MatrixXd V;
  MatrixXi F;
  if(argc<2)
  {
    cerr<<R"(
USAGE:
  viewmesh input.[obj|off|ply|stl|wrl]

or
  viewmesh input1.obj input2.ply ...
)";
    return EXIT_FAILURE;
  }
  igl::opengl::glfw::Viewer v;
  {
    const Eigen::Matrix<double,Eigen::Dynamic,3> CM = 
      (Eigen::Matrix<double,Eigen::Dynamic,3>(9,3) <<
      1      ,0.89412,0.22745,
      0.65098,0.33725,0.15686,
      0.96863,0.50588,0.74902,
          0.6,    0.6,    0.6,
      0.89412,0.10196, 0.1098,
      0.21569,0.49412,0.72157,
      0.30196,0.68627, 0.2902,
      0.59608,0.30588,0.63922,
            1,0.49804,      0).finished();
    int i = 1;
    while(true)
    {
      if(!igl::read_triangle_mesh(argv[i],V,F))
      {
        return EXIT_FAILURE;
      }
      v.data().set_mesh(V,F);
      v.data().set_colors(CM.row((i-1)%CM.rows()));
      if(i==CM.rows())
      {
        std::cerr<<"Warning: #meshes ("<<argc<<") > #colors ("<<CM.rows()<<
          "). Colors will repeat."<<std::endl;
      }
      i++;
      if(i>=argc)
      {
        break;
      }
      // prepare to read another mesh
      v.append_mesh();
    }
  }
  v.launch();
  return EXIT_SUCCESS;
}
