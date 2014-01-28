/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>.
 * This file is part of OSLD-lib (http://https://github.com/ismagom/osld-lib)
 *
 * OSLD-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSLD-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OSLD-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

typedef _Complex float cf_t;

void phch_cp_prb_ref(cf_t **input, cf_t **output, int offset, int nof_refs,
		int nof_prb, bool advance_input);
void phch_cp_prb(cf_t **input, cf_t **output, int nof_prb);
void phch_put_prb_ref_(cf_t **input, cf_t **output, int offset, int nof_refs,
		int nof_prb);
void phch_get_prb_ref(cf_t **input, cf_t **output, int offset, int nof_refs,
		int nof_prb);
