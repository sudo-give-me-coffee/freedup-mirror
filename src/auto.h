#ifndef AUTO_H
#define AUTO_H 1
/*
    This file is part of FreeDup.

    FreeDup -- search for duplicate files in one or more directory hierarchies
    Copyright (C) 2007,2008,2009 Andreas Neuper

    FreeDup is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FreeDup is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FreeDup.  If not, see <http://www.gnu.org/licenses/>.

*/
/* freedup was written by AN <AN@freedup.org>  */
const int autotest(const unsigned char*a);
const unsigned long autostart(const char*a);
const unsigned long autosize(const char*a);
const unsigned long autoend(const char*a);

typedef
struct extratabelle {
	const char	     *type;
        const unsigned long (*size) (const char*);
        const unsigned long (*start)(const char*);
        const unsigned long (*end)  (const char*);
        const int	    (*test) (const unsigned char*);
} extratab;

extern extratab extra[];
extern char tag1char;
extern char tag2char;
#endif /*auto.h*/
