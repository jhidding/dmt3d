#include "dmt.hh"
#include "../base/boxconfig.hh"
#include "../base/array.hh"
#include <iostream>
#include <fstream>
#include <functional>

using namespace DMT;
using namespace System;

void Folds::run(System::ptr<BoxConfig<3>> box, Func foo, double res,
		std::function<Point (const Point &)> za, std::string const &filename)
{
	Tr tr;            // 3D-Delaunay triangulation
	C2t3 c2t3(tr);    // 2D-complex in 3D-Delaunay triangulation

	// bounding box
	//double L = box->L();
	//double scale = box->scale();
	unsigned N = box->N();
	GT::Sphere_3 bounding_sphere(CGAL::ORIGIN, 0.25 * N*N);

	// definition of the surface
	Surface_3 surface(foo, bounding_sphere);

	// defining meshing criteria
	CGAL::Surface_mesh_default_criteria_3<Tr> criteria(15., 1./res, 1./res);

	// meshing surface
	CGAL::make_surface_mesh(c2t3, surface, criteria, CGAL::Manifold_with_boundary_tag(), 1000);

	// map surface
	for (auto p = c2t3.vertices_begin(); p != c2t3.vertices_end(); ++p)
	{
		p->set_point(za(p->point()));
	}

	std::ofstream out(filename);
	CGAL::output_surface_facets_to_off (out, c2t3);
	std::cout << "Final number of points: " << tr.number_of_vertices() << "\n";
}

