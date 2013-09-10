#pragma once
#include "../base/system.hh"
#include "../base/boxconfig.hh"
#include "../misc/interpol.hh"

namespace DMT
{
	using System::Array;
	using System::mVector;

	template <typename GT>
	class Hessian_3
	{
		typedef typename GT::Point_3 Point_3;
		typedef typename GT::Vector_3 Vector_3;

		System::ptr<System::BoxConfig<3>> box;

		static inline unsigned idx(unsigned i, unsigned j)
		{
			return (i >= j  ? (i * (i + 1)) / 2 + j
					: (j * (j + 1)) / 2 + i);
		}

		public:
			std::vector<Array<double>> 		d_ij_data;
			std::vector<Array<double>>		d_lambda_data;
			std::vector<Array<Vector_3>>		d_ev1, d_ev2, d_ev3;

			std::vector<Misc::Interpol::Linear<Array<double>,3>> d_ij;
			std::vector<Misc::Interpol::Linear<Array<double>,3>> d_lambda;

			Hessian_3(System::ptr<System::BoxConfig<3>> box_, 
				System::Array<double> potential);

			double eigenvalue(unsigned k, mVector<double, 3> const &x) const;
			double eigenvalue(unsigned k, Point_3 const &x) const;
			void compute_eigenvalues();
	};
}

