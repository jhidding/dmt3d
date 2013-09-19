#pragma once
#include <CGAL/Cartesian.h>
#include <CGAL/Robust_circumcenter_traits_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/Surface_mesh_default_criteria_3.h>
#include <CGAL/Complex_2_in_triangulation_3.h>
#include <CGAL/IO/Complex_2_in_triangulation_3_file_writer.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>
#include "../base/format.hh"
#include "../ply/ply.hh"

namespace DMT
{
	using Misc::PLY;

	template <typename K>
	class Mesh
	{
		// traits class
		//typedef CGAL::Robust_circumcenter_traits_3<K2>  K;

		// vertex and cell types
		typedef CGAL::Surface_mesh_vertex_base_3<K> Vb;
		typedef CGAL::Surface_mesh_cell_base_3<K> Cb;
		typedef CGAL::Triangulation_cell_base_with_circumcenter_3<K, Cb> 
			Cb_with_circumcenter;
		// triangulation
		typedef CGAL::Triangulation_data_structure_3<Vb, Cb_with_circumcenter> Tds;

		public:
			typedef CGAL::Delaunay_triangulation_3<K, Tds> Tr;
	};

	template <typename FT_, typename Point_>
	class CGFunc
	{
		std::function<FT_ (Point_ const &)> foo;

		public:
			typedef FT_ FT;
			typedef Point_ Point;

			template <typename F>
			CGFunc(F foo_): foo(foo_) {}

			FT operator()(Point const &x) const { return foo(x); }
	};

	class LevelSet
	{
		private:
		typedef CGAL::Cartesian<double> K;
		typedef Mesh<K>::Tr		Tr;
		public:
		typedef K::Point_3		Point;
		typedef CGFunc<double, Point>	Func;
		typedef CGAL::Complex_2_in_triangulation_3<Tr>  C2t3;

		private:
		typedef Tr::Geom_traits		GT;
		typedef CGAL::Implicit_surface_3<GT, Func>	Surface_3;

		public:
			static std::pair<System::ptr<Tr>,System::ptr<C2t3>> make(
				System::ptr<System::BoxConfig<3>> box,
				Func data, double res)
			{
				auto tr = System::make_ptr<Tr>();
				auto c2t3 = System::make_ptr<C2t3>(*tr);

				unsigned N = box->N();

				GT::Sphere_3 bounding_sphere(CGAL::ORIGIN, 0.25 * N*N);

				Surface_3 surface(data, bounding_sphere);

				CGAL::Surface_mesh_default_criteria_3<Tr> criteria(15., 1./res, 1./res);
				CGAL::make_surface_mesh(*c2t3, surface, criteria, CGAL::Non_manifold_tag(), 1000);
				std::cerr << " points: " << tr->number_of_vertices() << " ... ";
				std::cerr << " facets: " << tr->number_of_facets() << " ... ";

				return std::pair<System::ptr<Tr>,System::ptr<C2t3>>(tr, c2t3);
			}

			static void write_ply(
				std::pair<System::ptr<Tr>, System::ptr<C2t3>> Q, std::string fn)
			{
				PLY ply;
				std::map<C2t3::Vertex_handle, int> V;
				unsigned cnt = 0;

				ply.add_comment("DMT3d generated isocontour");
				ply.add_element("vertex",
					PLY::scalar_type<float>("x"),
					PLY::scalar_type<float>("y"), 
					PLY::scalar_type<float>("z")
					);
				std::cerr << "vertices ... ";
				for (auto i = Q.second->vertices_begin(); i != Q.second->vertices_end(); ++i)
				{
					V[i] = cnt; ++cnt;
					auto pt = i->point();
					ply.put_data(
						PLY::scalar<float>(pt[0]), 
						PLY::scalar<float>(pt[1]),
						PLY::scalar<float>(pt[2]));
				}

				std::cerr << "faces ... ";
				ply.add_element("face",
					PLY::list_type<unsigned>("vertex_index"));
				for (auto i = Q.second->facets_begin(); i != Q.second->facets_end(); ++i)
				{
					std::vector<unsigned> v;
					for (unsigned j = 0; j < 4; ++j)
						if (j != i->second)
							v.push_back(V[i->first->vertex(j)]);
					ply.put_data(PLY::list<unsigned>(v));
				}

				ply.write(fn, PLY::BINARY);
			}
	};
}

