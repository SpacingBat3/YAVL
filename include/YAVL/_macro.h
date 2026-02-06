/*
 * SPDX-License: GPL-3.0-or-later
 *
 * Copyright (C) 2026 Dawid Papiewski
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this
 * program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#if !defined(YAVL_VEC_H_STRUCT) || !defined(YAVL_VEC_H_STRUCT_IN)
#error This is private header that should not be included
#else /* !defined(YAVL_VEC_H_STRUCT) || !defined(YAVL_VEC_H_STRUCT_IN) */

/// "reserved" macro – my method of private struct fields in C
/// ---
/// Usage: reserved(bytes, bits)
#if __STDC_VERSION__ >= 202311L
#define reserved(B,...) _BitInt((B*8) __VA_OPT__(+) __VA_ARGS__) : (B*8) __VA_OPT__(+) __VA_ARGS__
#else /* __STDC_VERSION__ >= 202311L */
// This assumes unsigned long long size is enough to store any type as bit field.
#define reserved(B,...) unsigned long long : B*8 __VA_OPT__(+) __VA_ARGS__
#endif /* __STDC_VERSION__ >= 202311L */

#endif /* !defined(YAVL_VEC_H_STRUCT) || !defined(YAVL_VEC_H_STRUCT_IN) */
