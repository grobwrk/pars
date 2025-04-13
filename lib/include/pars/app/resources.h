/*
Copyright (c) 2025 Giuseppe Roberti.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include "pars/log.h"

#include <fmt/format.h>

#include <mutex>
#include <stdexcept>
#include <unordered_map>

namespace pars::app
{

template<typename resource_t, typename mtx_t>
struct lockable
{
  using resource_type = resource_t;
  using mutex_type = mtx_t;

  resource_type& resource;
  mutex_type& mutex;
};

template<typename resource_t, typename mutex_t, typename key_t,
         typename lock_t = std::unique_lock<mutex_t>>
struct locked
{
public:
  using resource_type = resource_t;
  using mutex_type = mutex_t;
  using key_type = key_t;
  using lock_type = lock_t;

  locked(lockable<resource_type, mutex_type> l, key_type k)
    : lockable_m{l}
    , key_m{k}
    , guard_m{lock_type{l.mutex}}
  {
  }

  auto& resource() { return lockable_m.resource; }

  auto& guard() { return guard_m; }

  auto& key() { return key_m; }

private:
  lockable<resource_type, mutex_type> lockable_m;
  key_type key_m;
  lock_type guard_m;
};

template<typename key_t, typename resource_t, typename mutex_t = std::mutex>
class resources
{
public:
  using key_type = key_t;
  using resource_type = resource_t;
  using mutex_type = mutex_t;

  resources() {}

  std::size_t count()
  {
    auto guard = std::lock_guard{mtx_m};

    return resources_m.size();
  }

  bool contains(const key_type key)
  {
    auto guard = std::lock_guard{mtx_m};

    return resources_m.contains(key);
  }

  template<class... args_t>
  auto find_or_emplace(const key_type key, args_t&&... args)
  {
    if (resources_m.contains(key))
      return find_locked(key, resources_m);
    return emplace(key, std::forward<args>(args)...);
  }

  template<class... args_t>
  auto emplace(const key_type key, args_t&&... args)
  {
    auto guard = std::lock_guard{mtx_m};

    auto mtx = mtxs_m.emplace(std::piecewise_construct,
                              std::forward_as_tuple(key), std::tuple());
    if (!mtx.second)
      throw std::runtime_error(fmt::format(
        "Unable to emplace a new Resource Mutex [key: 0x{:X}]", key));

    auto res = resources_m.emplace(key, std::forward<args_t>(args)...);

    if (!res.second)
    {
      mtxs_m.erase(mtx.first);

      throw std::runtime_error(
        fmt::format("Unable to emplace a new Resource [key: 0x{:X}]", key));
    }

    pars::debug(SL, lf::app, "Emplaced Resource [key: 0x{:X}]", key);

    return locked{lockable{res.first->second, mtx.first->second}, key};
  }

  auto locked_resource(const key_type k)
    -> locked<resource_type, mutex_type, key_type>
  {
    return find_locked(k, resources_m);
  }

  void delete_resource(key_type k)
  {
    auto guard = std::lock_guard{mtx_m};

    {
      auto guard = std::lock_guard{mtxs_m.at(k)};

      resources_m.erase(k);
    }

    mtxs_m.erase(k);
  }

private:
  auto find_locked(const key_type key,
                   std::unordered_map<key_type, resource_type>& xs)
    -> locked<resource_type, mutex_type, key_type>
  {
    auto guard = std::lock_guard{mtx_m};

    if (!xs.contains(key))
      throw std::out_of_range(
        fmt::format("Object not found for Key 0x{:X}", key));

    auto& x = xs.at(key);

    return locked{lockable{x, mtxs_m.at(key)}, key};
  }

  std::mutex mtx_m; ///< protects resources_m, mark_cleaned_m
  std::unordered_map<key_type, mutex_type>
    mtxs_m; ///< protects elements of resources_m
  std::unordered_map<key_type, resource_type> resources_m;
};

} // namespace pars::app
