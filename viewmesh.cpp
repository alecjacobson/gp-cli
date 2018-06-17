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
  if(argc!=2)
  {
    cerr<<R"(
USAGE:
  viewmesh input.[obj|off|ply|stl|wrl]
)";
    return EXIT_FAILURE;
  }
  if(!igl::read_triangle_mesh(argv[1],V,F))
  {
    return EXIT_FAILURE;
  }
  igl::opengl::glfw::Viewer v;
  v.data().set_mesh(V,F);
  v.launch();
  return EXIT_SUCCESS;
}
