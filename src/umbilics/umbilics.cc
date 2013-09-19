#include "../base/system.hh"
#include "../base/format.hh"
#include "../base/boxconfig.hh"
#include "../base/fourier.hh"
#include "../base/fft.hh"
#include "../dmt/hessian.hh"
#include "../dmt/mesh.hh"

using namespace System;

#include "constraint_functions.hh"

void command_umbilics(int argc, char **argv)
{
	Argv args = read_arguments(argc, argv,
		Option(0, "h", "help", "false", "shows this help."),

		Option(Option::VALUED | Option::CHECK, "i", "id", date_string(), 
			"Identifier of the run. This tag is prefixed to all input "
			"and output filenames. By default the current date is used. "));
		
	if (args.get<bool>("help"))
	{
		std::cerr << "DMT3D - catastrophes at your doorstep.\n"
			"Copyright Johan Hidding, June 2013 - licence: GPL3.\n\n"
			"Compute the Lagrangian Umbilic curves, the lines of D4\n"
			"singularities.\n";
		args.print(std::cerr);
		exit(0);
	}

	std::ifstream fi(Misc::format(args["id"], ".init.conan"));
	Header H(fi); History I(fi); I << args;
	Array<double> potential = load_from_file<double>(fi, "potential");

	auto box = make_ptr<BoxConfig<3>>(H.get<unsigned>("mbits"), H.get<double>("size"));
	DMT::Hessian_3<> hessian(box, potential);

	/* create a mesh with double size, all places with one odd index are edges. 
	 * places with two odd indices are faces, places with three odd indices are
	 * cells. If all indices arr even, we have a vertex. 
	 */
	//Array<double> double_mesh(box->size() * 8);

	/* for each vertex compute the seven constraint functions */
	//Array<double> constraint(box->size());

	//std::ofstream fo(C["id"] + ".umbilics.conan"); H.to_file(fo); I.to_file(fo);
	for (unsigned i = 0; i < 7; ++i)
	{
		std::cerr << "constraint no. " << i << " ... ";
		DMT::LevelSet::Func constraint([&hessian, i] (DMT::LevelSet::Point const &p)
		{
			mVector<double, 3> x; x[0] = p[0]; x[1] = p[1]; x[2] = p[2];
			return (*constraint_function[i])(
				hessian.d_ij[0](x), hessian.d_ij[2](x),
				hessian.d_ij[5](x), hessian.d_ij[1](x),
				hessian.d_ij[4](x), hessian.d_ij[3](x));
		});

		auto M = DMT::LevelSet::make(box, constraint, 1.0);
		DMT::LevelSet::write_ply(M, Misc::format("constraint_", i, ".ply"));
		std::cerr << "done\n";
	}
}

Global<Command> _COMMAND_UMBILICS("umbilics", command_umbilics);

