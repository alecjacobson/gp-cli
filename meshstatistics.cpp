#include <igl/read_triangle_mesh.h>
#include <igl/readMSH.h>
#include <igl/readMESH.h>
#include <igl/pathinfo.h>
#include <igl/edges.h>
#include <igl/doublearea.h>
#include <igl/internal_angles.h>
#include <igl/all_edges.h>
#include <igl/sort.h>
#include <igl/sparse.h>
#include <igl/find.h>
#include <igl/centroid.h>
#include <igl/copyleft/cgal/remesh_self_intersections.h>
#include <igl/copyleft/cgal/RemeshSelfIntersectionsParam.h>
#include <igl/remove_duplicate_vertices.h>
#include <igl/slice_mask.h>
#include <igl/matlab_format.h>
#include <igl/adjacency_matrix.h>
#include <igl/vertex_components.h>
#include <igl/facet_components.h>
#include <algorithm>
#include <string>
#include <cstdio>

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
    case 2:
      in = argv[1];
      break;
    default:
      cerr<<R"(
USAGE:
  meshstatistics input.[mesh|msh|obj|off|ply|stl|wrl]
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
        return igl::readMSH(path,V,T);
      }else
      {
        std::cerr<<"Error: "<<
          e<<" is not a supported tet mesh file format."<<std::endl;
        return false;
      }
    };
    if(!read_tet_mesh(in,V,T,F))
    {
      return EXIT_FAILURE;
    }
  }else
  {
    if(!read_triangle_mesh(in,V,F))
    {
      return EXIT_FAILURE;
    }
  }
  printf("%-9s %56s\n","filename",                                         in.c_str());
  // V,T,F have been set
  
  const int num_vertices = V.rows();
  const int dim = V.cols();
  const int num_faces = F.rows();
  Eigen::MatrixXi uE;
  edges(F,uE);
  const int num_edges = uE.rows();
  Eigen::MatrixXi E,sortE;
  all_edges(F,E);
  {
    Eigen::MatrixXi _1;
    sort(E,2,true,sortE,_1);
  }

  // Basic Counts
  printf("%-53s % 12d\n","number of edges",                                  num_edges);
  printf("%-53s % 12d\n","number of faces",                                  num_faces);
  printf("%-53s % 12d\n","number of vertices",                               num_vertices);
  printf("%-53s % 12d\n","number of dimensions",                             dim);

  const double bbd = (V.colwise().maxCoeff()- V.colwise().minCoeff()).norm();
  const double small_area = 1e-7*2.*bbd*bbd;
  Eigen::VectorXd A;
  doublearea(V,F,A);
  const int num_small_triangles = (A.array()<=small_area).count();
  const double max_area = A.maxCoeff()/2.0;
  const double min_area = A.minCoeff()/2.0;
  const double small_angle = 0.01;
  Eigen::MatrixXd Phi;
  internal_angles(V,F,Phi);
  const int num_small_angles = (Phi.array()<=small_angle).count();
  const double max_angle = Phi.maxCoeff();
  const double min_angle = Phi.minCoeff();
  Eigen::RowVector3d centroid;
  double volume;
  igl::centroid(V,F,centroid,volume);

  // Sizes                                                  
  printf("%-53s % 12g\n","bounding box diagonal",                            bbd);
  printf("%-53s % 12g\n","minimum angle",                                    min_angle);
  printf("%-53s % 12g\n","maximum angle",                                    max_angle);
  printf("%-53s % 12g\n","minimum area",                                     min_area);
  printf("%-53s % 12g\n","maximum area",                                     max_area);
  printf("%-53s % 12g\n","volume",                                           volume);
  //printf("%-39s % 3g,% 3g,% 3g\n","centroid",                                centroid(0),centroid(1),centroid(2));
  printf("%-53s % 12g\n","centroid_x",                                       centroid(0));
  printf("%-53s % 12g\n","centroid_y",                                       centroid(1));
  printf("%-53s % 12g\n","centroid_z",                                       centroid(2));

  const double close_dist = 1e-7;
  const auto num_duplicate_up_to = [&](const double tol)->int
  {
    Eigen::MatrixXd VV;
    Eigen::VectorXi _1,_2;
    remove_duplicate_vertices(V,tol,VV,_1,_2);
    return num_vertices - VV.rows();
  };
  const int num_close_vertices = num_duplicate_up_to(close_dist*bbd);
  const int num_duplicate_vertices = num_duplicate_up_to(0);

  // Small things                                                  
  printf("%-53s % 12d\n","number of small triangles",                        num_small_triangles);
  printf("%-53s % 12d\n","number of small angles   ",                        num_small_angles);
  printf("%-53s % 12d\n","number of close vertices",                         num_close_vertices);

  Eigen::SparseMatrix<int> DA;
  sparse(
    sortE.col(0),
    sortE.col(1),
    Eigen::VectorXi::Constant(sortE.rows(),1,1),
    V.rows(),
    V.rows(),
    DA);
  Eigen::VectorXi DAI,DAJ,DAV;
  find(DA,DAI,DAJ,DAV);
  const int num_nonmanifold_edges = (DAV.array()>2).count();
  const int num_boundary_edges = (DAV.array()==1).count();

  Eigen::VectorXi BAI,BAJ,BAV;
  slice_mask(DAI,DAV.array() == 1,1,BAI);
  slice_mask(DAJ,DAV.array() == 1,1,BAJ);
  slice_mask(DAV,DAV.array() == 1,1,BAV);
  Eigen::SparseMatrix<int> BA;
  sparse(BAI,BAJ,BAV,V.rows(),V.rows(),BA);

  Eigen::SparseMatrix<int> OA;
  // Same adjacency matrix but count -1 if E(:,1)<E(:,2) for and +1 otherwise
  sparse(
    sortE.col(0),
    sortE.col(1),
    1-2*((E.col(0).array() < E.col(1).array()).cast<int>()),
    V.rows(),
    V.rows(),
    OA);
  OA = (OA - OA.cwiseProduct(BA)).eval();
  OA.prune(1);
  const int num_conflictingly_oriented_edges = OA.nonZeros();

  // https://stackoverflow.com/questions/46025565/how-to-symmetrize-a-sparse-matrix-in-eigen-c
  BA = (BA+Eigen::SparseMatrix<int>(BA.transpose())).eval();
  Eigen::VectorXi BC,Bcounts;
  vertex_components(BA,BC,Bcounts);
  const int num_boundary_loops = (Bcounts.array()>1).count();

  //// Don't count boundary edges (where "redirected" edge only occured once).
  //Eigen::VectorXi OAI,OAJ,OAV;
  //find(OA,OAI,OAJ,OAV);

  Eigen::SparseMatrix<int> VA;
  adjacency_matrix(F,VA);
  Eigen::VectorXi C,counts;
  vertex_components(VA,C,counts);
  {
    // Number of vertices assigned to connected components
    int i = C.rows();
    // Number of connected components so far
    int c = C.rows()==0 ? 0 : C.maxCoeff()+1;
    counts.conservativeResize( (V.rows() - i) + c);
    C.conservativeResize(V.rows(),1);
    for(;i<V.rows();i++)
    {
      C(i) = c;
      counts(c) = 1;
      c++;
    }
  }
  const int num_unreferenced_vertices = (counts.array() == 1).count();
  const int num_connected_components = C.maxCoeff()+1;
  const int euler_characteristic = num_vertices - num_edges + num_faces;
  // http://sketchesoftopology.wordpress.com/2008/02/04/genus-euler-characteristic-boundary-components/
  const int num_handles = 
    (2*(num_connected_components - num_unreferenced_vertices) - 
     num_boundary_loops - (euler_characteristic-num_unreferenced_vertices))/2;

  // Topological                                                  
  printf("%-53s % 12d\n","number of connected components",                   num_connected_components);
  printf("%-53s % 12d\n","number of unreferenced vertices",                  num_unreferenced_vertices);
  printf("%-53s % 12d\n","number of handles",                                num_handles);
  printf("%-53s % 12d\n","Euler characteristic",                             euler_characteristic);
  printf("%-53s % 12d\n","number of boundary loops",                         num_boundary_loops);
  printf("%-53s % 12d\n","number of boundary edges",                         num_boundary_edges);
  printf("%-53s % 12d\n","number of nonmanifold edges",                      num_nonmanifold_edges);
  printf("%-53s % 12d\n","number of conflictedly oriented edges",            num_conflictingly_oriented_edges);

  Eigen::MatrixXi uF;
  unique_simplices(F,uF);
  const int num_combinatorially_duplicate_faces = num_faces - uF.rows();
  const int num_geometrically_degenerate_faces = (A.array() == 0).count();
  const int num_combinatorially_degenerate_faces = 
    ((F.array().col(0)==F.array().col(1)) || 
     (F.array().col(1)==F.array().col(2)) ||
     (F.array().col(2)==F.array().col(0))).count();

  // Degeneracy                                                  
  printf("%-53s % 12d\n","number of duplicate vertices",                     num_duplicate_vertices);
  printf("%-53s % 12d\n","number of combinatorially duplicate faces",        num_combinatorially_duplicate_faces);
  printf("%-53s % 12d\n","number of geometrically degenerate faces",         num_geometrically_degenerate_faces);
  printf("%-53s % 12d\n","number of combinatorially degenerate faces",       num_combinatorially_degenerate_faces);

  const bool fast = false;
  int num_selfintersecting_pairs = 0;
  int num_intracomponent_selfintersecting_pairs = 0;
  if(!fast)
  {
    Eigen::MatrixXd V3 = V;
    V3.conservativeResize(V.rows(),3);
    if(V.cols()<3)
    {
      V3.col(2).setConstant(0);
    }
    Eigen::MatrixXi Fnd;
    slice_mask(F,A.array()>0,1,Fnd);
    Eigen::MatrixXd _V;
    Eigen::MatrixXi _F;
    Eigen::MatrixXi IF;
    Eigen::VectorXi _2,_3;
    copyleft::cgal::remesh_self_intersections(
      V3,Fnd,
      copyleft::cgal::RemeshSelfIntersectionsParam(true,false,false),
      _V,_F,
      IF,_2,_3);
    num_selfintersecting_pairs = IF.rows();
    Eigen::VectorXi CF;
    slice(C,F.col(0),1,CF);
    Eigen::VectorXi CIF0,CIF1;
    slice(CF,IF.col(0),1,CIF0);
    slice(CF,IF.col(1),1,CIF1);
    num_intracomponent_selfintersecting_pairs = 
      (CIF0.array() == CIF1.array()).count();

  }

  if(!fast)                                                   
  {                                                  
  printf("%-53s % 12d\n","number of intra-component self-intersecting pairs",num_intracomponent_selfintersecting_pairs);
  printf("%-53s % 12d\n","number of self-intersecting pairs",                num_selfintersecting_pairs);
  }
}
