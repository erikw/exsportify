#ifndef DEBUG_H
#define DEBUG_H

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

using namespace boost::log::trivial;

#define logt(lvl) BOOST_LOG_TRIVIAL(lvl)

#endif
