#include "a3.hh"
#include "../misc/gradient.hh"
#include "../ply/ply.hh"

using namespace DMT;
using namespace System;

using Misc::PLY;
typedef Cusps::Vector_3 Vector_3;
typedef Cusps::Point_3 Point_3;

template <typename Function>
std::function<Vector_3 (Point_3 const &)> to_cgal(Function f)
{
	return [f] (Point_3 const &p)
	{
		mVector<double,3> x; x[0] = p[0]; x[1] = p[1]; x[2] = p[2];
	       	mVector<double,3> y = f(x);
		Vector_3 v(y[0], y[1], y[2]);
		return v;
	};
}

void Cusps::run(System::ptr<BoxConfig<3>> box, System::ptr<Hessian_3<>> hessian, 
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
		box, hessian->d_ev1[k]);

	auto g = to_cgal(grad_lambda),
	     e = to_cgal(eigen_vector);

	// definition of the surface
	Surface_3 surface(g, e, bounding_sphere);

	// defining meshing criteria
	CGAL::Surface_mesh_default_criteria_3<Tr> criteria(15., 1./res, 1./res);

	// meshing surface
	CGAL::make_surface_mesh(c2t3, surface, criteria, CGAL::Non_manifold_tag(), 1000);

	std::ofstream out(filename);

	PLY ply;
	std::map<C2t3::Vertex_handle, int> V;
	unsigned cnt = 0;

	ply.add_comment("DMT3d generated isocontour");
	ply.add_element("vertex", 
		PLY::scalar_type<float>("x"), 
		PLY::scalar_type<float>("y"), 
		PLY::scalar_type<float>("z"),
		PLY::scalar_type<uint8_t>("red"), 
		PLY::scalar_type<uint8_t>("green"), 
		PLY::scalar_type<uint8_t>("blue")
		);
	for (auto i = c2t3.vertices_begin(); i != c2t3.vertices_end(); ++i)
	{
		V[i] = cnt; ++cnt;
		auto pt = i->point();
		double v = g(pt) * e(pt);
		uint8_t r = std::max(255 - int(fabs(v) * 10000), 0);
		uint8_t g = std::max(255 - int(fabs(v) * 100000), 0);
		uint8_t b = std::max(255 - int(fabs(v) * 1000000), 0);
		ply.put_data(
			PLY::scalar<float>(pt[0]), 
			PLY::scalar<float>(pt[1]),
			PLY::scalar<float>(pt[2]),
			PLY::scalar<uint8_t>(r),
			PLY::scalar<uint8_t>(g),
			PLY::scalar<uint8_t>(b));
	}
	ply.add_element("face",
		PLY::list_type<unsigned>("vertex_index"));
	for (auto i = c2t3.facets_begin(); i != c2t3.facets_end(); ++i)
	{
		std::vector<unsigned> v;
		for (unsigned j = 0; j < 4; ++j)
			if (j != i->second)
				v.push_back(V[i->first->vertex(j)]);
		ply.put_data(PLY::list<unsigned>(v));
	}

	ply.write(filename, PLY::BINARY);
//	std::cout << "Final number of points: " << tr.number_of_vertices() << "\n";
}

