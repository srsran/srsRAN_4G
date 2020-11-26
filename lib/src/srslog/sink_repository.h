/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLOG_SINK_REPOSITORY_H
#define SRSLOG_SINK_REPOSITORY_H

#include "object_repository.h"
#include "sinks/stream_sink.h"

namespace srslog {

/// The sink repository stores sink instances associated to an id. Both stdout
/// and stderr stream sinks are created on construction so they accessible
/// without the need of creating them previously.
/// NOTE: Thread safe class.
class sink_repository
{
  /// Identifiers for stdout and stderr sinks.
  static constexpr char stdout_id[] = "stdout#";
  static constexpr char stderr_id[] = "stderr#";

  object_repository<std::string, std::unique_ptr<sink> > repo;

public:
  sink_repository()
  {
    //:TODO: GCC5 or lower versions emits an error if we use the new() expression directly, use redundant
    //piecewise_construct instead.
    repo.emplace(std::piecewise_construct,
                 std::forward_as_tuple(stdout_id),
                 std::forward_as_tuple(new stream_sink(sink_stream_type::stdout)));
    repo.emplace(std::piecewise_construct,
                 std::forward_as_tuple(stderr_id),
                 std::forward_as_tuple(new stream_sink(sink_stream_type::stderr)));
  }

  /// Returns the instance of the sink that writes to stdout.
  sink& get_stdout_sink()
  {
    auto s = repo.find(stdout_id);
    assert(s && "stdout sink should always exist");
    return *(s->get());
  }
  const sink& get_stdout_sink() const
  {
    const auto s = repo.find(stdout_id);
    assert(s && "stdout sink should always exist");
    return *(s->get());
  }

  /// Returns the instance of the sink that writes to stderr.
  sink& get_stderr_sink()
  {
    auto s = repo.find(stderr_id);
    assert(s && "stderr sink should always exist");
    return *(s->get());
  }
  const sink& get_stderr_sink() const
  {
    const auto s = repo.find(stderr_id);
    assert(s && "stderr sink should always exist");
    return *(s->get());
  }

  /// Finds a sink with the specified id in the repository. Returns a pointer to
  /// the sink, otherwise nullptr if not found.
  sink* find(const std::string& id)
  {
    auto p = repo.find(id);
    return (p) ? p->get() : nullptr;
  }
  const sink* find(const std::string& id) const
  {
    const auto p = repo.find(id);
    return (p) ? p->get() : nullptr;
  }

  /// Returns an instance of a sink specified by the input arguments.
  template <typename... Args>
  sink& fetch_sink(Args&&... args)
  {
    return *repo.emplace(std::forward<Args>(args)...);
  }

  /// Returns a copy of the list of registered sinks.
  std::vector<sink*> contents() const
  {
    auto repo_contents = repo.contents();

    std::vector<sink*> data;
    data.reserve(repo_contents.size());
    for (const auto& s : repo_contents) {
      data.push_back(s->get());
    }

    return data;
  }
};

constexpr char sink_repository::stdout_id[];
constexpr char sink_repository::stderr_id[];

} // namespace srslog

#endif // SRSLOG_SINK_REPOSITORY_H
