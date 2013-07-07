#ifdef UNITTEST
#include <iomanip>
#include "cvector.hh"
#include "unittest.hh"

using namespace System;

void p(cVector<2> const &b, size_t i)
{
	std::cout << std::setw(2) << std::setfill('0') << b.i(i,0) << "," 
		<< std::setw(2) << std::setfill('0') << b.i(i,1) << " ";
}

Test::Unit cVector_test(
	"0000:cVector",
	"Testing the functioning of the cVector module. "
	"The cVector is an object that manipulates size_t "
	"values being indices into multidimensional arrays "
	"of shape (N, N, ...) and N being a power of two. "
	"It performs these operations by doing bit shifting "
	"and masking. These operators asume the array represents "
	"a periodic space.",
	[] ()
{
	cVector<2> a(2), b(3), c(4);
	std::cout << "dx: ";
	for (auto x : b.dx)
		std::cout << b.c2m(x) << "; ";
	std::cout << std::endl;

	std::cout << "dx2: ";
	for (auto x : b.dx2)
		std::cout << b.c2m(x) << "; ";
	std::cout << std::endl;

	std::cout << "sq: ";
	for (auto x : b.sq)
		std::cout << b.c2m(x) << "; ";
	std::cout << std::endl;


	std::cout << "0 - sq: ";
	for (auto x : b.sq)
		std::cout << b.c2m(b.sub(0, x)) << "; ";
	std::cout << std::endl;

	std::cout << "0 - dx2: ";
	for (auto x : b.dx2)
		std::cout << b.c2m(b.sub(0, x)) << "; ";
	std::cout << std::endl;

	std::cout << "count-odd:";
	for (size_t x = 0; x < b.size(); ++x)
	{
		if (b.i(x, 0) == 0) std::cout << std::endl;
		std::cout << unsigned(b.count_odd(x)) << " ";
	}
	std::cout << std::endl;

	std::cout << "double-grid:";
	for (size_t x = 0; x < b.size(); ++x)
	{
		if (b.i(x, 0) == 0) std::cout << std::endl;
		p(c, b.double_grid(x));
	}
	std::cout << std::endl;

	std::cout << "half-grid:";
	for (size_t x = 0; x < b.size(); ++x)
	{
		if (b.i(x, 0) == 0) std::cout << std::endl;
		p(a, b.half_grid(x));
	}
	std::cout << std::endl;

	std::cout << "add-half 2 2:";
	for (size_t x = 0; x < b.size(); ++x)
	{
		if (b.i(x, 0) == 0) std::cout << std::endl;
		p(a, b.add_half(x, 36));
	}
	std::cout << std::endl;
	return true;
});

#endif

