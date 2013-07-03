#pragma once
#include "../base/mvector.hh"
#include "../base/boxconfig.hh"

namespace Misc
{
	template <unsigned R>
	class Gradient
	{
		typedef System::mVector<double, R> Vector;

		System::ptr<System::BoxConfig<R>> box;
		System::Array<double> data;

		public:
			typedef Vector value_type;

			Gradient(System::ptr<System::BoxConfig<R>> box_, System::Array<double> data_):
				box(box_), data(data_) {}

			inline double fd(typename System::cVector<R>::C const &i, unsigned k) const
			{
				return data[i - box->dx2()[k]] / 12. - 2. * data[i - box->dx()[k]] / 3.
					+ 2. * data[i + box->dx()[k]] / 3. - data[i + box->dx2()[k]] / 12.;
			}

			Vector operator[](size_t i) const
			{
				Vector v; 
				for (unsigned k = 0; k < R; ++k)
					v[k] = fd(box->box()[i], k);
				return v;
			}
	};
}

