#pragma once
#include <CGAL/Cartesian.h>
#include <CGAL/Robust_circumcenter_traits_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>

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
}

