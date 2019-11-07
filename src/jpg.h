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
#ifndef JPG_H
#define JPG_H 1

/* freedup was written by AN <AN@freedup.org>  */
const int jpgtest(const unsigned char*a);
const unsigned long jpgstart(const char*a);
const unsigned long jpgsize(const char*a);
const unsigned long jpgend(const char*a);

#endif /*JPG_H*/
