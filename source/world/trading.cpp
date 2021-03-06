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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "trading.hpp"
#include "empire.hpp"
#include "city.hpp"
#include "good/storage.hpp"
#include "core/utils.hpp"
#include "core/foreach.hpp"
#include "merchant.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "good/helper.hpp"
#include "game/gamedate.hpp"

namespace world
{

enum { idxBuyPrice=0, idxSellPrice=1 };

class Prices : public std::map< good::Product, PriceInfo >
{
public:
  VariantMap save() const
  {
    VariantMap ret;
    foreach( it, *this )
    {
      VariantList tmp;
      tmp << it->second.buy << it->second.sell;
      ret[ good::Helper::getTypeName( it->first ) ] = tmp;
    }

    return ret;
  }

  void load( const VariantMap& stream )
  {
    foreach( it, stream )
    {
      good::Product gtype = good::Helper::getType( it->first );
      if( gtype != good::none )
      {
        VariantList vl = it->second.toList();
        setPrice( gtype, vl.get( idxBuyPrice, 0 ).toInt(), vl.get( idxSellPrice, 0 ).toInt() );
      }
    }
  }

  void setPrice( good::Product type, int buy, int sell )
  {
    (*this)[ type ].buy = buy;
    (*this)[ type ].sell = sell;
  }
};

typedef std::map< unsigned int, TraderoutePtr > TradeRoutes;

class Trading::Impl
{
public:
  EmpirePtr empire;
  TradeRoutes routes;
  Prices empirePrices;

  void initStandartPrices();
};

Trading::Trading() : _d( new Impl )
{
  _d->initStandartPrices();
}

void Trading::timeStep( unsigned int time )
{
  if( game::Date::isDayChanged() )
  {
    foreach( it,_d->routes )
    {
      it->second->timeStep( time );
    }
  }
}

void Trading::init( EmpirePtr empire )
{
  _d->empire = empire;
}

VariantMap Trading::save() const
{
  VariantMap ret;
  VariantMap routesVm;
  foreach( it, _d->routes )
  {
    routesVm[ it->second->getName() ] = it->second->save();
  }

  ret[ "routes" ] = routesVm;
  VARIANT_SAVE_CLASS_D( ret, _d, empirePrices )

  return ret;
}

void Trading::load(const VariantMap& stream)
{
  VariantMap routes = stream.get( "routes" ).toMap();
  foreach( it, routes )
  {
    StringArray cityNames = utils::split( it->first,"<->" );
    if( !cityNames.empty() )
    {
      std::string beginCity = cityNames.valueOrEmpty( 0 );
      std::string endCity = cityNames.valueOrEmpty( 1 );
      TraderoutePtr route = createRoute( beginCity, endCity );
      if( route.isValid() )
      {
        route->load( it->second.toMap() );
      }
      else
      {
        Logger::warning( "WARNING!!! Trading::load cant create route from %s to %s",
                         beginCity.c_str(), endCity.c_str() );
      }
    }
    else
    {
      Logger::warning( "WARNING!!! Trading::load cant create route from " + it->first );
    }
  } 

  VARIANT_LOAD_CLASS_D( _d, empirePrices, stream )
}

Trading::~Trading()
{

}

void Trading::sendMerchant(const std::string& begin, const std::string& end,
                            good::Store &sell, good::Store &buy )
{
  TraderoutePtr route = findRoute( begin, end );
  if( !route.isValid() )
  {
    Logger::warning( "Trade route no exist [%s to %s]", begin.c_str(), end.c_str() );
    return;
  }

  route->addMerchant( begin, sell, buy );
}

TraderoutePtr Trading::findRoute( const std::string& begin, const std::string& end )
{
  unsigned int routeId = Traderoute::getId( begin, end );

  TradeRoutes::iterator it = _d->routes.find( routeId );
  if( it == _d->routes.end() )
  {
    Logger::warning( "!!! WARNING: Trade route no exist [%s to %s]", begin.c_str(), end.c_str() );
    return TraderoutePtr();
  }

  return it->second;
}

TraderoutePtr Trading::findRoute( unsigned int index )
{
  bool invalidIndex = index >= _d->routes.size();
  if( invalidIndex )
    return TraderoutePtr();

  TradeRoutes::iterator it = _d->routes.begin();
  std::advance( it, index );
  return it->second;
}

TraderoutePtr Trading::createRoute( const std::string& begin, const std::string& end )
{
  TraderoutePtr route = findRoute( begin, end );
  if( route.isValid() )
  {
    Logger::warning( "!!!WARNING: Want create route, but it exist [%s to %s]", begin.c_str(), end.c_str() );
    return route;
  }

  route = TraderoutePtr( new Traderoute( _d->empire, begin, end ) );
  unsigned int routeId = Traderoute::getId( begin, end );
  _d->routes[ routeId ] = route;
  route->drop();

  return route;
}

void Trading::setPrice(good::Product gtype, int bCost, int sCost)
{
  _d->empirePrices.setPrice( gtype, bCost, sCost );
}

PriceInfo Trading::getPrice(good::Product gtype)
{
  PriceInfo ret;
  Prices::const_iterator it = _d->empirePrices.find( gtype );

  if( it != _d->empirePrices.end() )
  {
    ret = it->second;
  }

  return ret;
}

TraderouteList Trading::routes( const std::string& begin )
{
  TraderouteList ret;

  CityPtr city = _d->empire->findCity( begin );

  foreach( it, _d->routes )
  {
    if( it->second->beginCity() == city || it->second->endCity() == city )
    {
      ret.push_back( it->second );
    }
  }

  return ret;
}

TraderouteList Trading::routes()
{
  TraderouteList ret;
  foreach( item, _d->routes ) { ret.push_back( item->second ); }

  return ret;
}

void Trading::Impl::initStandartPrices()
{
  Prices& b = empirePrices;
  b.setPrice( good::wheat,      28,  22 );
  b.setPrice( good::vegetable,  38,  30 );
  b.setPrice( good::fruit,      38,  30 );
  b.setPrice( good::olive,      42,  34 );
  b.setPrice( good::grape,      44,  36 );
  b.setPrice( good::meat,       44,  36 );
  b.setPrice( good::wine,       215, 160);
  b.setPrice( good::oil,        180, 140);
  b.setPrice( good::iron,       60,  40 );
  b.setPrice( good::timber,     50,  35 );
  b.setPrice( good::clay,       40,  30 );
  b.setPrice( good::marble,     200, 140);
  b.setPrice( good::weapon,     250, 180);
  b.setPrice( good::furniture,  200, 150);
  b.setPrice( good::pottery,    180, 140);
}

}//end namespace world
