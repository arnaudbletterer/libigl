#include <igl/readOBJ.h>
#include <igl/readPLY.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/exact_geodesic.h>
#include <igl/colormap.h>
#include <igl/unproject_onto_mesh.h>
#include <iostream>
#include <string>
#include "tutorial_shared_path.h"


int main(int argc, char *argv[])
{
  using namespace Eigen;
  using namespace std;
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  igl::opengl::glfw::Viewer viewer;
  // Load a mesh in OFF format

  std::string filename(TUTORIAL_SHARED_PATH "/armadillo.obj");

  if(argc == 2)
  {
	filename = std::string(argv[1]);
  }

  std::string extension(filename.substr(filename.rfind(".")+1));

  if(extension == "obj")
  {
	igl::readOBJ(filename, V, F);
  }
  else if(extension == "ply")
  {
	igl::readPLY(filename, V, F);
  }

  const auto update_distance = [&](const int vid)
  {
	Eigen::VectorXi VS,FS,VT,FT;
	// The selected vertex is the source
	VS.resize(1);
	VS << vid;
	// All vertices are the targets
	VT.setLinSpaced(V.rows(),0,V.rows()-1);
	Eigen::VectorXd d;
	std::cout<<"Computing geodesic distance to vertex "<<vid<<"..."<<std::endl;
	igl::exact_geodesic(V,F,VS,FS,VT,FT,d);
	const double strip_size = 0.05;
	// The function should be 1 on each integer coordinate
	// d = (d/strip_size*M_PI).array().sin().abs().eval();
	// Compute per-vertex colors
	Eigen::MatrixXd C;
	igl::colormap(igl::COLOR_MAP_TYPE_JET,d,true,C);
	// Plot the mesh
	viewer.data().set_mesh(V, F);
	viewer.data().set_colors(C);
  };

  // Plot a distance when a vertex is picked
  viewer.callback_mouse_down =
  [&](igl::opengl::glfw::Viewer& viewer, int, int)->bool
  {
	int fid;
	Eigen::Vector3f bc;
	// Cast a ray in the view direction starting from the mouse position
	double x = viewer.current_mouse_x;
	double y = viewer.core.viewport(3) - viewer.current_mouse_y;
	if(igl::unproject_onto_mesh(
	  Eigen::Vector2f(x,y),
	  viewer.core.view,
	  viewer.core.proj,
	  viewer.core.viewport,
	  V,
	  F,
	  fid,
	  bc))
	{
	  int max;
	  bc.maxCoeff(&max);
	  int vid = F(fid,max);
	  update_distance(vid);
	  return true;
	}
	return false;
  };
  viewer.data().set_mesh(V,F);

  cout << "Click on mesh to define new source.\n" << std::endl;
  update_distance(2677);
  return viewer.launch();
}
