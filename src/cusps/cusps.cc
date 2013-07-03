#include "../base/system.hh"
#include "../base/format.hh"
#include "../base/boxconfig.hh"
#include "../base/fourier.hh"
#include "../base/fft.hh"
#include "../misc/interpol.hh"


#include "a3.hh"
#include "../dmt/hessian.hh"

using namespace System;

#include <iomanip>
inline std::string time_string(double b)
{
	std::ostringstream ss;
	ss << std::setfill('0') << std::setw(5) << (int)round(b * 10000);
	return ss.str();
}

void command_a3(int argc, char **argv)
{
	Argv args = read_arguments(argc, argv,
		Option(0, "h", "help", "false", "shows this help."),

		Option(Option::VALUED | Option::CHECK, "i", "id", date_string(), 
			"Identifier of the run. This tag is prefixed to all input "
			"and output filenames. By default the current date is used. "),

		Option(Option::VALUED | Option::CHECK, "r", "res", "2",
			"resolution at which to mesh."));
		
	if (args.get<bool>("help"))
	{
		std::cerr << "DMT3D - catastrophes at your doorstep.\n"
			"Copyright Johan Hidding, June 2013 - licence: GPL3.\n\n"
			"This command computes the A_3 planes.\n ";
		args.print(std::cerr);
		exit(0);
	}

	std::ifstream fi(Misc::format(args["id"], ".init.conan"));
	Header H(fi); History I(fi);
	Array<double> potential = load_from_file<double>(fi, "potential");

	auto box = make_ptr<BoxConfig<3>>(
		H.get<unsigned>("mbits"), H.get<double>("size"));

	std::cerr << "computing eigenvalues ... ";
	auto hessian = System::make_ptr<DMT::Hessian_3>(box, potential);
	hessian->compute_eigenvalues();
	std::cerr << " [ done ]\n";

	std::cerr << "meshing surface ... ";
	for (unsigned k = 0; k < 3; ++k)
	{
		std::cerr << " " << k << " ";
		DMT::Cusps::run(box, hessian, k, args.get<double>("res"),
			Misc::format(args["id"], ".l", k, ".cusps.off"));
	}
	std::cerr << " [ done ]\n";
}

Global<Command> _COMMAND_a3("cusps", command_a3);


