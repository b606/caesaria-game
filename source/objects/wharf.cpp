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

#include "wharf.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "core/foreach.hpp"
#include "walker/fishing_boat.hpp"
#include "core/foreach.hpp"
#include "good/goodstore.hpp"
#include "constants.hpp"

using namespace constants;

class Wharf::Impl
{
public:
  enum { southPic=54, northPic=52, westPic=55, eastPic=53 };
  FishingBoatPtr boat;
};

Wharf::Wharf() : CoastalFactory(Good::none, Good::fish, building::wharf, Size(2)), _d( new Impl )
{
  // transport 52 53 54 55
  setPicture( ResourceGroup::wharf, Impl::northPic );
}

void Wharf::destroy()
{
  city::Helper helper( _getCity() );

  if( _d->boat.isValid() )
  {
    _d->boat->die();
  }

  CoastalFactory::destroy();
}

void Wharf::timeStep(const unsigned long time)
{
  CoastalFactory::timeStep(time);

  //try get good from storage building for us
  if( time % 22 == 1 && numberWorkers() > 0 && getWalkers().size() == 0 )
  {
    receiveGood();
    deliverGood();
  }

  //start/stop animation when workers found
  bool mayAnimate = mayWork();

  if( mayAnimate && _animationRef().isStopped() )
  {
    _animationRef().start();
  }

  if( !mayAnimate && _animationRef().isRunning() )
  {
    _animationRef().stop();
  }

  //no workers or no good in stock... stop animate
  if( !mayAnimate )
  {
    return;
  }

  if( getProgress() >= 100.0 )
  {
    if( getGoodStore().qty( getOutGoodType() ) < getGoodStore().capacity( getOutGoodType() )  )
    {
      updateProgress( -100.f );
      //gcc fix for temporaly ref object
      GoodStock tmpStock( getOutGoodType(), 100, 100 );
      getGoodStore().store( tmpStock, 100 );
    }
  }
  else
  {
    if( _d->boat.isValid() && !_d->boat->isBusy() && outStockRef().empty() )
    {
      _d->boat->startCatch();
    }
  }
}

ShipPtr Wharf::getBoat() const
{
  return ptr_cast<Ship>( _d->boat );
}

void Wharf::assignBoat( ShipPtr boat )
{
  _d->boat = ptr_cast<FishingBoat>( boat );
  if( _d->boat.isValid() )
  {
    _d->boat->setBase( this );
  }
}

bool Wharf::mayWork() const
{
  bool mayWork = CoastalFactory::mayWork();
  return (mayWork && _d->boat.isValid());
}

void Wharf::_updatePicture(Direction direction)
{
  switch( direction )
  {
  case south: setPicture( ResourceGroup::wharf, Impl::southPic ); break;
  case north: setPicture( ResourceGroup::wharf, Impl::northPic ); break;
  case west: setPicture( ResourceGroup::wharf, Impl::westPic ); break;
  case east: setPicture( ResourceGroup::wharf, Impl::eastPic ); break;

  default: break;
  }
}



