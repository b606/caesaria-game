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
//
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_CITYSERVICE_INFO_H_INCLUDED__
#define __CAESARIA_CITYSERVICE_INFO_H_INCLUDED__

#include "cityservice.hpp"
#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"

class CityServiceInfo : public CityService
{
public:
  class Parameters
  {
  public:
    DateTime date;
    int population;
    int funds;
    int tax;
    int taxpayes;
    int monthWithFood;
    int foodKoeff;
    int godsMood;
    int needWorkers;
    int workless;
    int colloseumCoverage;
    int theaterCoverage;
    int entertainment;
    int lifeValue;

    Parameters()
    {
      population = 0;
      funds = 0;
      tax = 0;
      taxpayes = 0;
      monthWithFood = 0;
      foodKoeff = 0;
      godsMood = 0;
      needWorkers = 0;
      workless = 0;
      tax = 0;
      colloseumCoverage = 0;
      theaterCoverage = 0;
      entertainment = 0;
      lifeValue = 0;
    }
  };

  static CityServicePtr create( PlayerCityPtr city );

  void update( const unsigned int time );
  Parameters getLast() const;

  static std::string getDefaultName();

private:
  CityServiceInfo( PlayerCityPtr city );

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_CITYSERVICE_INFO_H_INCLUDED__