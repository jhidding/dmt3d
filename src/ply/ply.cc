#include "ply.hh"

using namespace Misc;

#ifdef UNITTEST
#include "../base/unittest.hh"

Test::Unit PLY_test(
	"1000:PLY",
	"Testing the writing of a correct .ply file. "
	"If this test succeeds, it should write a correct "
	"mesh to a test file called ply_test.ply. ",
	[] ()
{
	return true;
});

#endif

template <> std::string PLY::type<int8_t>::name = "char";
template <> std::string PLY::type<uint8_t>::name = "uchar";
template <> std::string PLY::type<int16_t>::name = "short";
template <> std::string PLY::type<uint16_t>::name = "ushort";	
template <> std::string PLY::type<int32_t>::name = "int";
template <> std::string PLY::type<uint32_t>::name = "uint";
template <> std::string PLY::type<float>::name = "float";
template <> std::string PLY::type<double>::name = "double";

std::ostream &operator<<(std::ostream &out, PLY::Property const &property)
{
	return out << "property " << property.type_expression() << " " << property.name() << std::endl;
}

std::ostream &operator<<(std::ostream &out, PLY::Element const &element)
{
	out << "element " << element.name() << " " << element.count() << std::endl;

	for (auto property : element.properties())
		out << *property;

	return out;
}

std::ostream &operator<<(std::ostream &out, PLY::Header const &header)
{
	out << "ply" << std::endl
		<< "format " << (header.format() == PLY::Header::ASCII ? "ascii" : "binary") 
		<< " 1.0" << std::endl;

	for (auto comment : header.comments())
		out << "comment " << comment << std::endl;

	for (auto element : header.elements())
		out << *element;

	out << "end_header" << std::endl;

	return out;
}

// vim:ts=4:sw=4
