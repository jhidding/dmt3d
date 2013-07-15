#pragma once
#include <string>
#include <iostream>
#include <utility>

namespace Misc
{
	// PLY {{{1
	class PLY
	{
		using System::ptr;

		class Header;
		using Header::Property;

		class Data;
		using Data::Item;

		Header 			m_header;
		Data			m_data;

		public:
			template <typename ...Args>
			void add_element(std::string const &name, Args &&...properties)
			{
				m_header.add_element(name);
				add_properties(std::forward<Args>(args)...);
			}

			void put_data(Datum const &datum)
			{
				m_data.write(item);
				m_data.next_item();
				m_header.add_item();
			}

			template <typename ...Args>
			void put_data(Datum const &datum, Args &&...args)
			{
				m_data.write(item);
				m_data.next_datum();
				put_data(std::forward<Args>(args)...);
			}

		private:
			void add_properties(Property const &prop)
			{
				m_header.add_property(prop);
			}

			template <typename ...Args>
			void add_properties(Property const &prop, Args &&...args)
			{
				m_header.add_property(prop);
				add_properties(std::forward<Args>(args)...);
			}
	};
	// }}}1 PLY

	// PLY::Header {{{1
	// PLY::Header {{{2
	class PLY::Header
	{
		template <typename T>
		struct type { static std::string name; }

		public:
			class Property
			{
				std::string m_name;

				public:
					Property(std::string const &name_): m_name(name_) {}
					std::string const &name() const { return m_name; }
					virtual std::string type_expression() const = 0;
			};

			template <typename T>
			class Scalar: public Property
			{
				public:
					using Property::Property;

					std::string type_expression() const
					{ return type<T>::name; }
			};

			template <typename T, typename length_type = uint8_t>
			class List: public Property
			{
				public:
					using Property::Property;

					std::string type_expression() const
					{ return "list " + type<length_type>::name + " " 
						+ type<T>::name; }
			};

			class Element;

			enum Format { ASCII, BINARY };
			typedef std::string Comment;

		private:
			Format 								m_format;
			std::vector<Comment> 				m_comment;
			std::vector<ptr<Element>>			m_elements;

		public:
			Format format() const 
				{ return m_format; }
			std::vector<Comment> const &comments() const 
				{ return m_comments; }
			std::vector<ptr<Element>> const &elements() const 
				{ return m_elements; }

			void add_element(std::string const &name)
			{
				m_elements.push_back(new Element(name));
			}

			void add_property(Property const &property)
			{
				m_elements.back()->add_property(property);
			}

			void add_item()
			{
				m_elements.back()->add_item();
			}
	};
	// }}}2 PLY::Header
	
	// PLY::Header::Element {{{2
	class PLY::Header::Element
	{
		std::string 						m_name;
		size_t								m_count;
		std::vector<ptr<Property>> 			m_properties;

		public:
			Element(std::string const &name_): m_name(name_), m_count(0) {}
			std::string const &name() const { return m_name; }
			size_t count() const { return m_count; }

			template <typename T>
			void add_scalar(std::string const &name)
			{
				m_properties.push_back(new Scalar<T>(name));
			}

			template <typename T, typename length_type = uint8_t>
			void add_list(std::string const &name)
			{
				m_properties.push_back(new List<T, length_type>(name));
			}

			void add_item()
			{
				++m_count;
			}
	};
	// }}}2 PLY::Header::Element
	// }}}1 PLY::Header

	// PLY::Data {{{1
	class PLY::Data
	{
		public:
			class Datum;

			template <typename T>
			class Scalar: Datum
			{
				T	m_value;

				public:
					write(
			};

			template <typename T>
			class List: Datum
			{
				std::vector<T> m_value;

				public:
			};

			template 
			void write_value(Datum const &datum) = 0;
	};
	// }}}1
}

// vim:ts=4:sw=4:fdm=marker
