#ifndef PARS_FMT_H
#define PARS_FMT_H

#ifndef PARS_LOGGING_ENABLED
#include <fmt/format.h>
namespace pars
{
using fmt::format;
using fmt::formatter;
using fmt::format_context;
using fmt::format_to;
};
#else
#include <format>
namespace pars
{
using std::format;
using std::format_context;
using std::format_to;
using std::formatter;
}; // namespace pars
#endif

#endif // PARS_FMT_H
