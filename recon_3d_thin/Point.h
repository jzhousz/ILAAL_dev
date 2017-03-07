#ifndef POINT_H
#define POINT_H


/**
* AnalyzeSkeleton_ plugin for ImageJ(C).
* Copyright (C) 2008,2009 Ignacio Arganda-Carreras
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation (http://www.gnu.org/licenses/gpl.txt )
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/

#include <string>

using namespace std;


class Point
{
public:

	int x = 0;	/** x- coordinate */
	int y = 0;	/** y- coordinate */
	int z = 0;	/** z- coordinate */

	Point();
	Point(int x, int y, int z);


};

#endif