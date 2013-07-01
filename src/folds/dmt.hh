#pragma once

#include "../base/boxconfig.hh"
#include "../base/array.hh"
//#include "../cgal/interpol.hh"

#include <CGAL/Cartesian.h>
#include <CGAL/Robust_circumcenter_traits_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/Surface_mesh_default_criteria_3.h>
#include <CGAL/Complex_2_in_triangulation_3.h>
#include <CGAL/IO/Complex_2_in_triangulation_3_file_writer.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>
#include <CGAL/Bbox_3.h>

namespace DMT
{
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

	class Folds
	{
		typedef CGAL::Cartesian<double> 		K;
		typedef Mesh<K>::Tr 				Tr;

		// c2t3
		typedef CGAL::Complex_2_in_triangulation_3<Tr> 	C2t3;

		typedef Tr::Geom_traits 			GT;
		public:
			typedef GT::Point_3				Point;
		private:
		typedef CGFunc<double, Point>			Func;
		typedef CGAL::Implicit_surface_3<GT, Func> 	Surface_3;

		public:
			static void run(System::ptr<System::BoxConfig<3>> box, 
					Func data, double res,
					std::function<Point (Point const &)> za,
					std::string const &filename);
	};
}

