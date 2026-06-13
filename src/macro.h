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

// __builtin_expect variant
#if defined(__has_builtin) && (!defined(LIKELY) || !defined(UNLIKELY))
#if __has_builtin(__builtin_expect)
// Hint compiler about branch predictions
#define LIKELY(x)    __builtin_expect((x), true)
#define UNLIKELY(x)  __builtin_expect((x), false)
#endif
#endif
// Generic macro defs
#if !defined(LIKELY) || !defined(UNLIKELY)
// Standard branch prediction logic
#define LIKELY(x)    (x)
#define UNLIKELY(x)  (x)
#endif
