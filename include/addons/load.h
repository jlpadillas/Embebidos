/*
 * load.h
 * 
 * Copyright 2016 Hector Perez <perezh@unican.es>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

/* Title: load
 * 
 * Facilities to load the CPU */
#ifndef __EV3C_LOAD
#define __EV3C_LOAD

/*
   Function: do_some_job
   Function that creates a load on the CPU.

   Parameters:
      load - percentage of load on CPU (0-100).
      seconds - time interval with load on CPU before returning (0-60).

   Returns:
      0 on success, -1 on failure.

   See Also:

*/
int do_some_job (int load, int seconds);

#endif
