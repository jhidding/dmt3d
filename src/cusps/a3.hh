#pragma once

#include "../base/boxconfig.hh"
#include "../base/array.hh"
//#include "../cgal/interpol.hh"

#include <CGAL/Cartesian.h>
#include <CGAL/Surface_mesh_default_criteria_3.h>
#include <CGAL/Complex_2_in_triangulation_3.h>
#include <CGAL/IO/Complex_2_in_triangulation_3_file_writer.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>
#include "../dmt/mesh.hh"
#include "../dmt/hessian.hh"

namespace DMT
{
	template <typename GT, typename Func>
	class ImplicitA3: public CGAL::Implicit_surface_3<GT, Func>
	{
		public:
			using CGAL::Implicit_surface_3<GT,Func>::Implicit_surface_3;
	};

	class Cusps
	{
		typedef CGAL::Cartesian<double> 		K;
		typedef Mesh<K>::Tr 				Tr;
		typedef CGAL::Complex_2_in_triangulation_3<Tr> 	C2t3;

		typedef Tr::Geom_traits 			GT;

		public:
		typedef GT::Point_3				Point;

		private:
		typedef CGFunc<double, Point>			Func;
		typedef ImplicitA3<GT, Func> 			Surface_3;


		public:
			static void run(System::ptr<System::BoxConfig<3>> box, 
					System::ptr<Hessian_3> hessian,
					unsigned k, double res, std::string const &filename);
	};
}

