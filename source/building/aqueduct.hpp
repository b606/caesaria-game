// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __CAESARIA_AQUEDUCT_H_INCLUDED__
#define __CAESARIA_AQUEDUCT_H_INCLUDED__

#include "watersupply.hpp"

class Aqueduct : public WaterSource
{
public:
  Aqueduct();

  virtual void build(PlayerCityPtr city, const TilePos& pos );
  Picture& computePicture(PlayerCityPtr city,
                          const TilesArray* tmp = NULL,
                          const TilePos pos = TilePos(0, 0));
  virtual void initTerrain(Tile& terrain);
  virtual bool canBuild(PlayerCityPtr city, const TilePos& pos ) const;
  virtual bool isNeedRoadAccess() const;
  virtual void destroy();
  virtual bool isWalkable() const; 
  virtual bool isRoad() const;

  void updatePicture(PlayerCityPtr city);
  void addWater( const WaterSource& source );

protected:
  virtual void _waterStateChanged();
};

#endif // __CAESARIA_AQUEDUCT_H_INCLUDED__
