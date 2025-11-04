//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "pch.h"

#include "request_handler.hpp"
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"

#include <boost/gil/extension/io/pnm.hpp>
#include <boost/gil/extension/io/pnm/write.hpp>

namespace http {
	namespace server {

		request_handler::request_handler(const std::string& doc_root)
			: doc_root_(doc_root) {
		}

		void generate_image(const std::string& request_path, reply& rep) {
			auto queryAt = request_path.find("?");
			assert(queryAt != std::string::npos);

			std::string queryString = request_path.substr(queryAt + 1);
			auto equalAt = queryString.find("=");
			std::string queryParam = queryString.substr(0, equalAt);
			std::string queryValue = queryString.substr(equalAt + 1);
			cout << "query " << queryParam << "=" << queryValue << endl;

			if (queryParam.find("pos") != 0) {
				rep = reply::stock_reply(reply::bad_request);
				return;
			}

			int pos = std::stoi(queryValue);



			// write data into image
			std::stringstream out_buffer(ios_base::out | ios_base::binary);
			gil::gray8_image_t img(256, 256);
			using namespace boost::gil;
			write_view(out_buffer, view(img), pnm_tag());

			// Fill out the reply to be sent to the client.
			rep.status = reply::ok;
			// char buf[512];
			rep.content = out_buffer.str();
			rep.headers.resize(2);
			rep.headers[0].name = "Content-Length";
			rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = mime_types::extension_to_type("pnm");
		}

		void request_handler::handle_request(const request& req, reply& rep) {
			// Decode url to path.
			std::string request_path;
			if (!url_decode(req.uri, request_path)) {
				rep = reply::stock_reply(reply::bad_request);
				return;
			}

			// Request path must be absolute and not contain "..".
			if (request_path.empty() || request_path[0] != '/'
				|| request_path.find("..") != std::string::npos) {
				rep = reply::stock_reply(reply::bad_request);
				return;
			}

			// if is a query, then handle image generation
			auto queryAt = request_path.find("?");
			if (queryAt != std::string::npos) {
				generate_image(request_path, rep);
				return;
			}

			// If path ends in slash (i.e. is a directory) then add "index.html".
			if (request_path[request_path.size() - 1] == '/') {
				request_path += "index.html";
			}

			// Determine the file extension.
			std::size_t last_slash_pos = request_path.find_last_of("/");
			std::size_t last_dot_pos = request_path.find_last_of(".");
			std::string extension;
			if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
				extension = request_path.substr(last_dot_pos + 1);
			}

			// Open the file to send back.
			std::string full_path = doc_root_ + request_path;
			std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
			if (!is) {
				rep = reply::stock_reply(reply::not_found);
				return;
			}

			// Fill out the reply to be sent to the client.
			rep.status = reply::ok;
			char buf[512];
			while (is.read(buf, sizeof(buf)).gcount() > 0)
				rep.content.append(buf, (int)is.gcount());
			rep.headers.resize(2);
			rep.headers[0].name = "Content-Length";
			rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = mime_types::extension_to_type(extension);
		}

		bool request_handler::url_decode(const std::string& in, std::string& out) {
			out.clear();
			out.reserve(in.size());
			for (std::size_t i = 0; i < in.size(); ++i) {
				if (in[i] == '%') {
					if (i + 3 <= in.size()) {
						int value = 0;
						std::istringstream is(in.substr(i + 1, 2));
						if (is >> std::hex >> value) {
							out += static_cast<char>(value);
							i += 2;
						} else {
							return false;
						}
					}
					else {
						return false;
					}
				} else if (in[i] == '+') {
					out += ' ';
				} else {
					out += in[i];
				}
			}
			return true;
		}

	} // namespace server
} // namespace http
