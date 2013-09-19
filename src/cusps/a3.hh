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
	class Mesher_traits: public CGAL::Surface_mesher::Implicit_surface_oracle_3<GT, Surface>
	{
		typedef CGAL::Surface_mesher::Implicit_surface_oracle_3<GT, Surface> Base;

		public:
			typedef typename GT::Point_3	Point_3;
			typedef typename GT::Segment_3	Segment_3;
			typedef typename GT::Ray_3		Ray_3;
			typedef typename GT::Line_3		Line_3;

			typedef Surface Surface_3;
			typedef CGAL::Object Object;

			class Intersect_3: public Base::Intersect_3
			{
				public:
					Intersect_3(typename Base::Intersect_3 const &I):
						Base::Intersect_3(I) {}

					Object operator()(const Surface_3& surface, const Ray_3& r)
					{
						Object o = Base::Intersect_3::operator()(surface, r);
						Point_3 p;

						if (CGAL::assign(p, o))
						{

						}
						else
						{
							return o;
						}
					}

					Object operator()(const Surface_3& surface, const Line_3& l)
					{
						Object o = Base::Intersect_3::operator()(surface, l);
						return o;
					}

					Object operator()(const Surface_3& surface, const Segment_3& s)
					{
						Object o = Base::Intersect_3::operator()(surface, s);
						return o;
					}
			};

			/*
			class Construct_initial_points
			{
				public:
					template <typename OutputIteratorPoints>
					OutputIteratorPoints operator() (
						const Surface_3& surface, 
						OutputIteratorPoints out, 
						int n = 20) // WARNING: why 20?
					{
						// later: use morse-theory to find extrema
					}
			};
			*/

			Intersect_3 intersect_3_object() const
			{
				return Intersect_3(Base::intersect_3_object());
			}

			/*
			Construct_initial_points construct_initial_points_object() const
			{
				return Construct_initial_points();	
			}
			*/
	};

	template <typename GT, typename Func>
	class ImplicitA3: public CGAL::Implicit_surface_3<GT, Func>
	{
		typedef CGAL::Implicit_surface_3<GT, Func> Base;
		typedef typename GT::Point_3 Point_3;
		typedef typename GT::Vector_3 Vector_3;

		public:
			typedef Mesher_traits<GT, ImplicitA3> Surface_mesher_traits_3;
			typedef std::function<double (Point_3 const &)> ScalarField;
			typedef std::function<Vector_3 (Point_3 const &)> VectorField;

			template <typename GradL, typename EigenV>
			static ScalarField dot_product(
					GradL grad_lambda, EigenV eigen_vector)
			{
				return [grad_lambda, eigen_vector] (Point_3 const &X) -> double
				{
					Vector_3 a = grad_lambda(X), b = eigen_vector(X);
					return a * b;
				};
			};

			VectorField l, e;

			template <typename GradL, typename EigenV, typename BB>
			ImplicitA3(GradL grad_lambda, EigenV eigen_vector, BB bb):
				Base(dot_product(grad_lambda, eigen_vector), bb),
				l(grad_lambda), e(eigen_vector)
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
		typedef GT::Point_3				Point_3;
		typedef GT::Vector_3			Vector_3;

		private:
		typedef CGFunc<double, Point>			Func;
		typedef ImplicitA3<GT, Func> 			Surface_3;


		public:
			static void run(System::ptr<System::BoxConfig<3>> box, 
					System::ptr<Hessian_3<>> hessian,
					unsigned k, double res, std::string const &filename);
	};
}

// vim:sw=4:ts=4
