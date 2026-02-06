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

#ifdef YAVL_C_NAMESPACE
#define __NAMESPACE(NS,SYM) NS##_##SYM
#define _NAMESPACE(NS,SYM) __NAMESPACE(NS,SYM)
#define NS(SYM) _NAMESPACE(YAVL_C_NAMESPACE,SYM)
#else
#define NS(SYM) SYM
#endif

#ifdef YAVL_C_NAMESPACE_UPPER
#define __NAMESPACE_UP(NS,SYM) NS##_##SYM
#define _NAMESPACE_UP(NS,SYM) __NAMESPACE_UP(NS,SYM)
#define NS_UPPER(SYM) _NAMESPACE_UP(YAVL_C_NAMESPACE_UPPER,SYM)
#else
#define NS_UPPER(SYM) SYM
#endif
