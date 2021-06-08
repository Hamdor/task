/*******************************************************************************
 *                            _            _    _                              *
 *                           | |          | |  (_)                             *
 *                           | |_ __ _ ___| | ___                              *
 *                           | __/ _` / __| |/ / |                             *
 *                           | || (_| \__ \   <| |                             *
 *                            \__\__,_|___/_|\_\_|                             *
 *                                                                             *
 * Copyright (C) 2015 - 2021                                                   *
 * Marian Triebe                                                               *
 *                                                                             *
 * Distributed under the terms and conditions of the BSD 3-Clause License      *
 * See accompanying file LICENSE.                                              *
 *                                                                             *
 * If you did not receive a copy of the license file, see                      *
 * http://opensource.org/licenses/BSD-3-Clause                                 *
 *******************************************************************************/

struct copy_move {};

struct no_move_no_copy {
  no_move_no_copy() = default;
  no_move_no_copy(const no_move_no_copy&) = delete;
  no_move_no_copy(no_move_no_copy&&) = delete;
  no_move_no_copy& operator=(const no_move_no_copy&) = delete;
  no_move_no_copy& operator=(no_move_no_copy&&) = delete;
};

struct move_only {
  move_only() = default;
  move_only(const move_only&) = delete;
  move_only(move_only&&) = default;
  move_only& operator=(const move_only&) = delete;
  move_only& operator=(move_only&&) = default;
};

struct copy_only {
  copy_only() = default;
  copy_only(const copy_only&) = default;
  copy_only(copy_only&&) = delete;
  copy_only& operator=(const copy_only&) = default;
  copy_only& operator=(copy_only&&) = delete;
};
