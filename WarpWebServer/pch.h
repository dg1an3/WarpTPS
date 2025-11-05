#ifndef PCH_H
#define PCH_H

#include <assert.h>
#define ASSERT assert

#include <signal.h>

#include <string>
#include <vector>
#include <algorithm>

#include <iostream>
#include <fstream>
#include <sstream>

#include "framework.h"

#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

#include <boost/asio.hpp>

// include png io
#include <boost/gil/extension/io/pnm/tags.hpp>
#include <boost/gil/extension/io/png/tags.hpp>
#include <boost/gil/io/write_view.hpp>
namespace gil = boost::gil;

// inclue the boost geometry header
#include <boost/geometry.hpp>
namespace bg = boost::geometry;

// inclue the boost geometry header
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;

#include "TPSTransform.h"

#endif //PCH_H
