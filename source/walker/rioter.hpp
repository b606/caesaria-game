// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __OPENCAESAR3_RIOTER_H_INCLUDE_
#define __OPENCAESAR3_RIOTER_H_INCLUDE_

#include "walker.hpp"

class Rioter;
typedef SmartPtr<Rioter> RioterPtr;

class Rioter : public Walker
{
public:
  static RioterPtr create( CityPtr city );
  virtual ~Rioter();

  virtual void onNewTile();
  virtual void onDestination();
  virtual void timeStep(const unsigned long time);
  void send2City( HousePtr house );

  virtual void die();

private:
  Rioter( CityPtr city );

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif//__OPENCAESAR3_RIOTER_H_INCLUDE_