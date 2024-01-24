#include <igl/opengl/glfw/Viewer.h>
#include <string>
void draw_frame_to_png(
  const std::string & prefix,
  const int index,
  igl::opengl::glfw::Viewer & v);

// Implementation
#include <igl/stb/write_image.h>
void draw_frame_to_png(
  const std::string & prefix,
  const int index,
  igl::opengl::glfw::Viewer & v)
{
  using namespace igl;
  using namespace Eigen;
  char png_filename[512];
  sprintf(png_filename,"%s%06d.png",prefix.c_str(),index);
  std::cout<<" --> "<<png_filename<<"..."<<std::flush;
  // Allocate temporary buffers
  const int w = v.core().viewport(2);
  const int h = v.core().viewport(3);
  Matrix<unsigned char,Dynamic,Dynamic> R(w,h), G(w,h), B(w,h), A(w,h);
  // Draw the scene in the buffers
  v.core().draw_buffer(v.data(),false,R,G,B,A);
  // Save it to a PNG
  igl::stb::write_image(png_filename,R,G,B,A);
}
