#include "draw_frame_to_png.h"
#include <igl/read_triangle_mesh.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/parallel_for.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <vector>



int main(int argc, char * argv[])
{
  using namespace std;
  using namespace Eigen;
  bool bounce = true;
  bool realign_camera_on_update = false;
  const double fps = 30;
  double last_update_t = 0;
  int anim_direction = 1;
  // 150 should actually be the width of the terminal screen. Should also
  // truncate printed lines in update() to fit in terminal line.
  const string clear_line = string("\r")+string(150,' ')+string("\r");

  if(argc<2)
  {
    cerr<<R"(
USAGE:
  scrubmesh mesh-1.obj mesh-2.obj ...
)";
    return EXIT_FAILURE;
  }
 
  vector<MatrixXd> Vlist(argc-1,MatrixXd());
  vector<MatrixXi> Flist(argc-1,MatrixXi());
  vector<std::string> filename_list(argc-1);
  // Read files in parallel
  const int n = argc-1;
  {
    const double tic = igl::get_seconds();
    int error = EXIT_SUCCESS;
    std::cout<<std::endl;
    igl::parallel_for(n,[&](const int i)
    {
      if(error == EXIT_FAILURE) return;
      const auto & filename = argv[i+1];
      std::cout<<clear_line<<"\rLoading "<<filename<<"..."<<std::flush;
      filename_list[i] = filename;
      if(!igl::read_triangle_mesh(filename,Vlist[i],Flist[i]))
      {
        error = EXIT_FAILURE;
      }
    });
    if(error == EXIT_FAILURE)
    {
      return error;
    }
    std::cout<<clear_line<<"\rLoaded "<<n<<" meshes in "<<igl::get_seconds()-tic<<" secs"<<std::endl;
  }
  assert(Vlist.size() == n);
  assert(Flist.size() == n);
  int index = 0;

  igl::opengl::glfw::Viewer v;
  std::cout<<R"(scrubmesh
  B,b  toggle whether animation bounces or loops
  R,r  realign camera to center of model on update
  P,p  write screenshots of entire sequence to ./scrubmesh-%06d.png
  .    step forward one frame
  ,    step backward one frame
  >    fast forward to last frame
  <    rewind to first frame
)";

  const auto update = [&]()
  {
    //// .clear() has a bunch of other side-effects (e.g., "resetting"
    //// face_based, etc.)
    //v.data().clear();
    v.data().V.resize(0,3);
    v.data().F.resize(0,3);
    v.data().set_mesh(Vlist[index],Flist[index]);
    v.data().compute_normals();
    if(realign_camera_on_update)
    {
      v.core().align_camera_center(Vlist[index],Flist[index]);
    }
    std::cout<<clear_line<<filename_list[index]<< std::flush;
    last_update_t = igl::get_seconds();
  };
  const auto increment = [&]()
  {
    index++;
    update();
  };
  const auto rewind = [&]()
  {
    index = 0;
    update();
  };
  const auto fast_forward = [&]()
  {
    index = n-1;
    update();
  };
  const auto decrement = [&]()
  {
    index--;
    update();
  };

  v.callback_key_pressed = 
    [&](igl::opengl::glfw::Viewer& /*viewer*/, unsigned int key, int mod)->bool
  {
    switch(key)
    {
    default:
      return false;
    case 'A':
    case 'a':
      v.core().is_animating ^= 1;
      if(v.core().is_animating)
      {
        last_update_t = igl::get_seconds();
      }
      break;
    case 'B':
    case 'b':
      bounce ^= 1;
      if(!bounce) anim_direction = 1;
      break;
    case 'P':
    case 'p':
      {
        const int index_copy = index;
        rewind();
        for(int i = 0;i<n;i++)
        {
          draw_frame_to_png("./scrubmesh-",i,v);
          if(i+1<n) increment();
        }
        std::cout<<clear_line;
        index = index_copy;
        update();
        break;
      }
    case 'R':
    case 'r':
      realign_camera_on_update ^= 1;
      update();
      break;
    case '>':
      fast_forward();
      break;
    case '<':
      rewind();
      break;
    case '.':
    case ',':
      if(key == '.')
      {
        if(index+1<n) increment();
      }else
      {
        if(index-1 >=0) decrement();
      }
      break;
    break;
    }
    return true;
  };

  v.callback_pre_draw = [&](igl::opengl::glfw::Viewer &)->bool
  {
    if(v.core().is_animating)
    {
      const double now = igl::get_seconds();
      if( (now-last_update_t) > 1./fps )
      {
        // reflect?
        const bool reflect = index+anim_direction >= n || index+anim_direction < 0;
        if(bounce && reflect)
        {
          anim_direction *= -1;
        }
        if(!bounce && reflect)
        {
          rewind();
        }else
        {
          if(anim_direction>0)
          {
            increment();
          }else
          {
            decrement();
          }
        }
      }
    }
    return false;
  };

  // Prepare a line so that we can overwrite it with each update
  std::cout<<std::endl;
  //v.core().is_animating = true;
  update();
  v.launch();
  // New line so that shell is reset
  std::cout<<std::endl;
  return EXIT_SUCCESS;
}
