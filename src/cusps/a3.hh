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
	template <typename GT, typename Surface>
	class Mesher_traits: public CGAL::Implicit_surface_oracle_3<GT, Surface>
	{
		typedef CGAL::Implicit_surface_oracle_3<GT, Surface> Base;

		typedef typename GT::Point_3	Point_3;
		typedef typename GT::Segment_3	Segment_3;
		typedef typename GT::Ray_3		Ray_3;
		typedef typename GT::Line_3		Line_3;

		typedef Surface Surface_3;
		typedef CGAL::Object Object;

		public:
			class Intersect_3: public Base::Intersect_3
			{
				Object operator()(const Surface_3& surface, const Ray_3& r)
				{
					Object o = Base::operator()(surface, r);
				}

				Object operator()(const Surface_3& surface, const Line_3& l)
				{
					Object o = Base::operator()(surface, l);
				}

				Object operator()(const Surface_3& surface, Segment_3 s)
				{
					Object o = Base::operator()(surface, s);
				}
			};

			class Construct_initial_points
			{
				public:
					template <typename OutputIteratorPoints>
					OutputIteratorPoints operator() (
						const Surface_3& surface, 
						OutputIteratorPoints out, 
						int n = 20) // WARNING: why 20?
					{
						// use morse-theory to find extrema
					}
			};

			Intersect_3 intersect_3_object() const
			{
				return Intersect_3(Base::intersect_3_object());
			}

			Construct_initial_points construct_initial_points_object() const
			{
				return Construct_initial_points();	
			}
	};

	template <typename GT, typename Func>
	class ImplicitA3: public CGAL::Implicit_surface_3<GT, Func>
	{
		typedef CGAL::Implicit_surface_3<GT, Func> Base;

		public:
			typedef Mesher_traits<GT, ImplicitA3> Surface_mesher_traits_3;

			//using CGAL::Implicit_surface_3<GT,Func>::Implicit_surface_3;	
			
			template <typename A, typename B>
			ImplicitA3(A a, B b):
				Base(a, b)
			{}

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

// vim:sw=4:ts=4
