/*******************************************************************************
 *                            _            _    _                              *
 *                           | |          | |  (_)                             *
 *                           | |_ __ _ ___| | ___                              *
 *                           | __/ _` / __| |/ / |                             *
 *                           | || (_| \__ \   <| |                             *
 *                            \__\__,_|___/_|\_\_|                             *
 *                                                                             *
 * Copyright (C) 2015                                                          *
 * Marian Triebe  <marian.triebe  (at) haw-hamburg.de>                         *
 *                                                                             *
 * Distributed under the terms and conditions of the BSD 3-Clause License      *
 * See accompanying file LICENSE.                                              *
 *                                                                             *
 * If you did not receive a copy of the license file, see                      *
 * http://opensource.org/licenses/BSD-3-Clause                                 *
 *******************************************************************************/

#ifndef SCHEDULER_DATA_HPP
#define SCHEDULER_DATA_HPP

#include <mutex>
#include <condition_variable>

class work_stealing;
class work_sharing;

class scheduler_data {
  friend work_stealing;
  friend work_sharing;

  std::thread m_thread;
  std::mutex  m_lock;
  std::condition_variable m_empty;
  std::condition_variable m_new;
};

#endif // SCHEDULER_DATA_HPP
