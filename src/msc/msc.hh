#pragma once
#include "../base/array.hh"
#include "../base/mvector.hh"
#include "../base/mdspace.hh"
#include "../base/cvector.hh"

#include <cstdint>
#include <vector>

namespace DMT
{
	template <unsigned R>
	class MSC
	{
		using System::cVector;
		using System::Array;
		using System::MdRange;
		using System::mVector;

		cVector<R> 	single_box, double_box;
					// single_box is the natural geometry of the
					// source array, whereas double_box has twice
					// the resolution to store edges and cells
					// as well as vertices.

		std::vector<size_t> neighbours;
					// contains all 27 cells in a little cube

		Array<double>	data;
					// source data

		Array<uint8_t> 	discrete_vector;
					// index into box.dx, in [0, 6)
					// vertices point to one of 6 edges
					// edges point to one of 4 faces
					// faces point to one of 2 cells

		class Cell
		{
			size_t 		i;
			double		value;

			public:
				// the amount of odd values in the index vector
				// gives the type of cell:
				//  0 - node
				//  1 - edge
				//  2 - face
				//  3 - cell
				inline uint8_t rank()
				{
					return double_box.count_odd(i);
				}

				Cell(size_t i_, double value_):
					i(i_), value(value_) {}

				// operator<, the priority_queue gives priority to
				// high values, we need to prioritize low values.
				// so I consistently use operator>
				bool operator<(Cell const &o) const
				{
					if (value == o.value) 
						return rank() > o.rank();

					return (value > o.value);
				}
		};

		std::priority_queue<Cell> Q;

		public:
			MSC(unsigned bits, Array<double> data_):
				single_box(bits), double_box(bits+1),
				data(data_)
			{
				// generate the 27-neighbourhood
				mVector<int, R> unit(1);
				for (mVector<int, R> const &dx : MdRange<R>(mVector<int, R>(3)))
					neighbours.push_back(single_box.loc(dx) - single_box.loc(unit));
			}

			void find_minima();
	};

	template <unsigned R>
	void MSC<R>::find_minima()
	{
		// iterate over data box
		for (size_t i = 0; i < single_box.size(); ++i)
		{
			// find minimum
			size_t n_star = 0;
			double min = data[i];

			for (size_t n : neighbours)
			{
				size_t idx = single_box.add(i, n);

				if (data[idx] < min)
				{
					n_star = n;
					min = data[idx];
				}
			}

			// if it is in the center, store the double-coordinate
			if (n_star == 0)
				Q.push(single_box.double_grid(i));
		}
	}


}

// vim:sw=4:ts=4
