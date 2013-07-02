#include "../base/system.hh"
#include "../base/boxconfig.hh"
#include "interpol.hh"

namespace DMT
{
	using System::Array;
	using System::mVector;

	class Hessian_3
	{
		System::ptr<System::BoxConfig<3>> box;

		static inline unsigned idx(unsigned i, unsigned j)
		{
			return (i >= j  ? (i * (i + 1)) / 2 + j
					: (j * (j + 1)) / 2 + i);
		}

		public:
			std::vector<Array<double>> 		d_ij_data;
			std::vector<Array<double>>		d_lambda;
			std::vector<Array<mVector<double, 3>>>	d_ev;

			std::vector<Misc::Interpol::Linear<Array<double>,3>> d_ij;

			Hessian_3(System::ptr<System::BoxConfig<3>> box_, 
				System::Array<double> potential);

			double eigenvalue(unsigned k, mVector<double, 3> const &x) const;
			void compute_eigenvalues();
	};
}

