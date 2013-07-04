#include "a3.hh"
#include "../misc/gradient.hh"

using namespace DMT;
using namespace System;

void Cusps::run(System::ptr<BoxConfig<3>> box, System::ptr<Hessian_3> hessian, 
		unsigned k, double res, std::string const &filename)
{
	Tr tr;            // 3D-Delaunay triangulation
	C2t3 c2t3(tr);    // 2D-complex in 3D-Delaunay triangulation

	// bounding box
	//double L = box->L();
	//double scale = box->scale();
	unsigned N = box->N();
	GT::Sphere_3 bounding_sphere(CGAL::ORIGIN, 0.25 * N*N);

	Misc::Interpol::Linear<Misc::Gradient<3>,3> grad_lambda(
		box, Misc::Gradient<3>(box, hessian->d_lambda_data[k]));

	Misc::Interpol::Linear<Array<mVector<double,3>>,3> eigen_vector(
		box, hessian->d_ev[k]);

	Func foo = [grad_lambda, eigen_vector] (Point const &x) -> double
	{
		mVector<double, 3> X; X[0] = x[0]; X[1] = x[1]; X[2] = x[2];
		mVector<double, 3> a = grad_lambda(X), b = eigen_vector(X);
		return a.dot(b);
	};

	// definition of the surface
	Surface_3 surface(foo, bounding_sphere);

	// defining meshing criteria
	CGAL::Surface_mesh_default_criteria_3<Tr> criteria(15., 1./res, 1./res);

	// meshing surface
	CGAL::make_surface_mesh(c2t3, surface, criteria, CGAL::Non_manifold_tag(), 1000);

	std::ofstream out(filename);
	CGAL::output_surface_facets_to_off (out, c2t3);
//	std::cout << "Final number of points: " << tr.number_of_vertices() << "\n";
}

