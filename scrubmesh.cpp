#include <igl/read_triangle_mesh.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/parallel_for.h>
#include <igl/png/writePNG.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <list>
#include <iterator>



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
 
  // Use a list so that meshes are _not_ contiguous in memory
  list<MatrixXd> Vlist(argc-1,MatrixXd());
  list<MatrixXi> Flist(argc-1,MatrixXi());
  vector<std::string> filename_list(argc-1);
  // Read files in parallel
  const int n = argc-1;
  {
    vector<list<MatrixXd>::iterator> Viters;
    vector<list<MatrixXi>::iterator> Fiters;
    const double tic = igl::get_seconds();
    auto Viter = Vlist.begin();
    auto Fiter = Flist.begin();
    for(int i = 0;i<n;i++)
    {
      Viters.push_back(Viter);
      Viter++;
      Fiters.push_back(Fiter);
      Fiter++;
    }
    int error = EXIT_SUCCESS;
    std::cout<<std::endl;
    igl::parallel_for(n,[&](const int i)
    {
      if(error == EXIT_FAILURE) return;
      const auto & filename = argv[i+1];
      std::cout<<clear_line<<"\rLoading "<<filename<<"..."<<std::flush;
      filename_list[i] = filename;
      if(!igl::read_triangle_mesh(filename,*Viters[i],*Fiters[i]))
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
  auto Viter = Vlist.begin();
  auto Fiter = Flist.begin();

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
    v.data().clear();
    v.data().set_mesh(*Viter,*Fiter);
    v.data().compute_normals();
    if(realign_camera_on_update)
    {
      v.core.align_camera_center(*Viter,*Fiter);
    }
    std::cout<<clear_line<<filename_list[index]<< std::flush;
    last_update_t = igl::get_seconds();
  };
  const auto increment = [&]()
  {
    assert(Vlist.size() == Flist.size());
    index++;
    assert(std::next(Viter) != Vlist.end());
    Viter++;
    assert(std::next(Fiter) != Flist.end());
    Fiter++;
    update();
  };
  const auto rewind = [&]()
  {
    index = 0;
    Viter = Vlist.begin();
    Fiter = Flist.begin();
    update();
  };
  const auto fast_forward = [&]()
  {
    index = n-1;
    Viter = std::prev(Vlist.end());
    Fiter = std::prev(Flist.end());
    update();
  };
  const auto decrement = [&]()
  {
    assert(Vlist.size() == Flist.size());
    index--;
    assert(Viter != Vlist.begin());
    Viter--;
    assert(Fiter!= Flist.begin());
    Fiter--;
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
      v.core.is_animating ^= 1;
      if(v.core.is_animating)
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
        const auto Viter_copy = Viter;
        const auto Fiter_copy = Fiter;
        rewind();
        for(int i = 0;i<n;i++)
        {
          char png_filename[512];
          sprintf(png_filename,"./scrubmesh-%06d.png",i);
          std::cout<<" --> "<<png_filename<<"..."<<std::flush;
          // Allocate temporary buffers
          const int w = v.core.viewport(2);
          const int h = v.core.viewport(3);
          Matrix<unsigned char,Dynamic,Dynamic> R(w,h), G(w,h), B(w,h), A(w,h);
          // Draw the scene in the buffers
          v.core.draw_buffer(v.data(),false,R,G,B,A);
          // Save it to a PNG
          igl::png::writePNG(R,G,B,A,png_filename);
          if(i+1<n) increment();
        }
        std::cout<<clear_line;
        index = index_copy;
        Viter = Viter_copy;
        Fiter = Fiter_copy;
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
    if(v.core.is_animating)
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
  //v.core.is_animating = true;
  update();
  v.launch();
  // New line so that shell is reset
  std::cout<<std::endl;
  return EXIT_SUCCESS;
}
