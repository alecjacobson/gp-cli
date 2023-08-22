#include <Eigen/Geometry>
#include <igl/read_triangle_mesh.h>
#include <igl/write_triangle_mesh.h>
#include <igl/triangulated_grid.h>
#include <igl/snap_to_canonical_view_quat.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiPlugin.h>
#include <igl/opengl/glfw/imgui/ImGuizmoWidget.h>

int main(int argc, char *argv[])
{
  if(argc<3)
  {
    std::cerr<<R"(
USAGE:
  upright input.[obj|off|ply|stl|wrl] output.[obj|off|ply|stl|wrl]

)";
    return EXIT_FAILURE;
  }
  // Load a mesh from file
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  igl::read_triangle_mesh(argv[1],V,F);
  //
  Eigen::MatrixXd FV;
  Eigen::MatrixXi FF;
  igl::triangulated_grid(20,20,FV,FF);
  const double bbd = (V.colwise().maxCoeff() - V.colwise().minCoeff()).norm();
  FV = (2.0*FV.array()-1.0).eval()*bbd;
  FV.conservativeResize(FV.rows(),3);
  FV = (FV * (Eigen::Matrix3d()<<-1,0,0,0,0,1,0,1,0).finished()).eval();
  FV.col(1).setConstant(V.col(1).minCoeff());
  Eigen::MatrixXd FC(FF.rows(),4);
  for(int f = 0;f<FF.rows();f++)
  {
    switch(f%4)
    {
      case 0: case 1: FC.row(f)<<0.40,0.40,0.60,0.99; break;
      case 2: case 3: FC.row(f)<<0.22,0.22,0.42,0.99; break;
    }
  }


  // Set up viewer
  igl::opengl::glfw::Viewer vr;
  const size_t mid = vr.selected_data_index;
  vr.data_list[mid].set_mesh(V,F);
  vr.append_mesh();
  const size_t fid = vr.selected_data_index;
  vr.data_list[fid].set_mesh(FV,FF);
  vr.data_list[fid].set_colors(FC);
  vr.data_list[fid].F_material_ambient = vr.data_list[fid].F_material_diffuse;
  vr.data_list[fid].F_material_specular.setConstant(0);
  vr.data_list[fid].F_material_diffuse.setConstant(0);
  vr.data_list[fid].show_lines = false;
  vr.selected_data_index = mid;

  // Custom menu
 
  igl::opengl::glfw::imgui::ImGuiPlugin imgui_plugin;
  vr.plugins.push_back(&imgui_plugin);

  // Add a 3D gizmo plugin
  igl::opengl::glfw::imgui::ImGuizmoWidget guizmo;
  imgui_plugin.widgets.push_back(&guizmo);

  guizmo.operation = ImGuizmo::ROTATE;
  // Initialize ImGuizmo at mesh centroid
  guizmo.T.block(0,3,3,1) = 
    0.5*(V.colwise().maxCoeff() + V.colwise().minCoeff()).transpose().cast<float>();

  // Update can be applied relative to this remembered initial transform
  const Eigen::Matrix4f T0 = guizmo.T;
  const auto update = [&]()
  {
    const Eigen::Matrix4d TT = (guizmo.T*T0.inverse()).cast<double>().transpose();
    const Eigen::MatrixXd U = 
      (V.rowwise().homogeneous()*TT).rowwise().hnormalized();
    vr.data_list[mid].set_vertices(U);
    vr.data_list[mid].compute_normals();
    FV.col(1).setConstant(U.col(1).minCoeff());
    vr.data_list[fid].set_vertices(FV);
  };
  // Attach callback to apply imguizmo's transform to mesh
  guizmo.callback = [&](const Eigen::Matrix4f & T)
  {
    update();
  };
  const auto save = [&]()
  {
    if(igl::write_triangle_mesh(argv[2],vr.data().V,vr.data().F))
    {
      printf("Saved to %s\n.",argv[2]);
    }else
    {
      printf("Failed to save to %s\n.",argv[2]);
    }
  };
  const auto snap = [&]()
  {
    Eigen::Quaternionf qin;
    qin = Eigen::Matrix3f(guizmo.T.block(0,0,3,3));
    Eigen::Quaternionf qout;
    igl::snap_to_canonical_view_quat(qin,1.0,qout);
    guizmo.T.block(0,0,3,3) = qout.toRotationMatrix();
    update();
  };
  // Maya-style keyboard shortcuts for operation
  vr.callback_key_pressed = [&](decltype(vr) &,unsigned int key, int mod)
  {
    switch(key)
    {
      case 'S':case 's': save(); return true;
      case 'Z': if(mod|IGL_MOD_SHIFT){break;} snap(); return true;
    }
    return false;
  };
  std::cout<<R"(
Q,q  save and quit
S,s  save
Z    snap
)";
  vr.launch_init();
  vr.core().align_camera_center(vr.data_list[mid].V, vr.data_list[mid].F);
  vr.launch_rendering();
}
