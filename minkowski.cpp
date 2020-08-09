#include <igl/read_triangle_mesh.h>
#include <igl/writeOBJ.h>
#include <igl/copyleft/cgal/minkowski_sum.h>

int main(int argc, char * argv[])
{
  Eigen::MatrixXd VA,VB;
  Eigen::MatrixXi FA,FB;
  igl::read_triangle_mesh(argv[1],VA,FA);
  igl::read_triangle_mesh(argv[2],VB,FB);
  Eigen::MatrixXd W;
  Eigen::MatrixXi G;
  Eigen::VectorXi J;
  igl::copyleft::cgal::minkowski_sum(VA,FA,VB,FB,true,W,G,J);
  igl::writeOBJ(argv[3],W,G);
}
