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

#include "minimap_window.hpp"
#include "gfx/tilemap.hpp"
#include "game/minimap_colours.hpp"
#include "gfx/tile.hpp"
#include "gfx/tileoverlay.hpp"
#include "core/time.hpp"
#include "gfx/engine.hpp"
#include "core/event.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"

using namespace gfx;
using namespace constants;

namespace gui
{

class Minimap::Impl
{
public:
  PictureRef minimap;
  PictureRef fullmap;

  Tilemap const* tilemap;
  int climate;

  MinimapColors* colors;

  int lastTimeUpdate;
  Point center;

  void getTerrainColours(const Tile& tile, int &c1, int &c2);
  void getBuildingColours(const Tile& tile, int &c1, int &c2);
  void updateImage();

  Point getOffset() { return Point( 146/2 - center.x(), 112/2 + center.y() - tilemap->size()*2); }

public oc3_signals:
  Signal1<TilePos> onCenterChangeSignal;
};

Minimap::Minimap(Widget* parent, Rect rect, const Tilemap& tilemap, int climate)
  : Widget( parent, -1, rect ), _d( new Impl )
{
  _d->tilemap = &tilemap;
  _d->climate = climate;
  _d->lastTimeUpdate = 0;
  _d->fullmap.reset( Picture::create( Size( _d->tilemap->size() * 2 ) ) );
  _d->minimap.reset( Picture::create( Size( 144, 110 ) ) );
  _d->colors = new MinimapColors( (ClimateType)climate );
  setTooltipText( _("##minimap_tooltip##") );
}

Point getBitmapCoordinates(int x, int y, int mapsize ) {  return Point( x + y, x + mapsize - y - 1 ); }
void getBuildingColours( const Tile& tile, int &c1, int &c2 );

void Minimap::Impl::getTerrainColours(const Tile& tile, int &c1, int &c2)
{
  int rndData = tile.originalImgId();
  int num3 = rndData & 0x3;
  int num7 = rndData & 0x7;

  TileOverlay::Type ovType = construction::unknown;
  if( tile.overlay().isValid() )
    ovType = tile.overlay()->type();

  if (tile.getFlag( Tile::tlTree ))
  {
    c1 = colors->colour(MinimapColors::MAP_TREE1, num3);
    c2 = colors->colour(MinimapColors::MAP_TREE2, num7);
  }
  else if (tile.getFlag( Tile::tlRock ))
  {
    c1 = colors->colour(MinimapColors::MAP_ROCK1, num3);
    c2 = colors->colour(MinimapColors::MAP_ROCK2, num3);
  }
  else if(tile.getFlag( Tile::tlDeepWater) )
  {
    c1 = colors->colour(MinimapColors::MAP_WATER1, num3);
    c2 = colors->colour(MinimapColors::MAP_WATER2, num3);
  }
  else if(tile.getFlag( Tile::tlWater ))
  {
    c1 = colors->colour(MinimapColors::MAP_WATER1, num3);
    c2 = colors->colour(MinimapColors::MAP_WATER2, num7);
  }
  else if (tile.getFlag( Tile::tlRoad ))
  {
    c1 = colors->colour(MinimapColors::MAP_ROAD, 0);
    c2 = colors->colour(MinimapColors::MAP_ROAD, 1);
  }
  else if (tile.getFlag( Tile::tlMeadow ))
  {
    c1 = colors->colour(MinimapColors::MAP_FERTILE1, num3);
    c2 = colors->colour(MinimapColors::MAP_FERTILE2, num7);
  }
  else if (tile.getFlag( Tile::tlWall ))
  {
    c1 = colors->colour(MinimapColors::MAP_WALL, 0);
    c2 = colors->colour(MinimapColors::MAP_WALL, 1);
  }
  else if( ovType == building::aqueduct  )
  {
    c1 = colors->colour(MinimapColors::MAP_AQUA, 0);
    c2 = colors->colour(MinimapColors::MAP_AQUA, 1);
  }
  else if (tile.getFlag( Tile::tlBuilding ))
  {
    getBuildingColours(tile, c1, c2);
  }
  else // plain terrain
  {
    c1 = colors->colour(MinimapColors::MAP_EMPTY1, num3);
    c2 = colors->colour(MinimapColors::MAP_EMPTY2, num7);
  }

  c1 |= 0xff000000;
  c2 |= 0xff000000;
}

void Minimap::Impl::getBuildingColours(const Tile& tile, int &c1, int &c2)
{
  TileOverlayPtr overlay = tile.overlay();

  if (overlay == NULL)
    return;

  TileOverlay::Type type = overlay->type();

  switch(type)
  {
    case building::house:
    {
      switch (overlay->size().width())
      {
        case 1:
          {
            c1 = colors->colour(MinimapColors::MAP_HOUSE, 0);
            c2 = colors->colour(MinimapColors::MAP_HOUSE, 1);
            break;
          }
        default:
          {
            c1 = colors->colour(MinimapColors::MAP_HOUSE, 2);
            c2 = colors->colour(MinimapColors::MAP_HOUSE, 0);
          }
        }
        break;
        }
      case building::reservoir:
        {
          c1 = colors->colour(MinimapColors::MAP_AQUA, 1);
          c2 = colors->colour(MinimapColors::MAP_AQUA, 0);
          break;
        }
      default:
        {
          switch (overlay->size().width())
          {
          case 1:
          {
            c1 = colors->colour(MinimapColors::MAP_BUILDING, 0);
            c2 = colors->colour(MinimapColors::MAP_BUILDING, 1);
            break;
          }
          default:
          {
            c1 = colors->colour(MinimapColors::MAP_BUILDING, 0);
            c2 = colors->colour(MinimapColors::MAP_BUILDING, 2);
          }
        }
    }
  }

  c1 |= 0xff000000;
  c2 |= 0xff000000;
}

void Minimap::Impl::updateImage()
{
  int mapsize = tilemap->size();

  fullmap->lock();
  // here we can draw anything

  int border = (162 - mapsize) / 2;
  int max = border + mapsize;

  for (int y = border; y < max; y++)
  {
    for (int x = border; x < max; x++)
    {
      const Tile& tile = tilemap->at(x - border, y - border);

      Point pnt = getBitmapCoordinates(x - border, y - border, mapsize);
      int c1, c2;
      getTerrainColours( tile, c1, c2);

      if( pnt.x() >= fullmap->width()-1 || pnt.y() >= fullmap->height() )
        continue;

      fullmap->setPixel( pnt, c1);
      fullmap->setPixel( pnt + Point( 1, 0 ), c2);
    }
  }

  // show center of screen on minimap
  // Exit out of image size on small carts... please fix it

  /*sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX(),     mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ(), kWhite);
  sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX() + 1, mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ(), kWhite);
  sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX(),     mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() + 1, kWhite);
  sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX() + 1, mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() + 1, kWhite);

  for ( int i = TilemapRenderer::instance().getMapArea().getCenterX() - 18; i <= TilemapRenderer::instance().getMapArea().getCenterX() + 18; i++ )
  {
    sdlFacade.setPixel(surface, i, mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() + 34, kYellow);
    sdlFacade.setPixel(surface, i, mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() - 34, kYellow);
  }

  for ( int j = mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() - 34; j <= mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() + 34; j++ )
  {
    sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX() - 18, j, kYellow);
    sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX() + 18, j, kYellow);
  }
  */

  fullmap->unlock();

  // this is window where minimap is displayed
  minimap->fill( 0xff000000, Rect() );
  minimap->draw( *fullmap, getOffset() );
}

/* end of helper functions */

namespace {
  static const int kWhite  = 0xFFFFFF;
  static const int kYellow = 0xFFFF00;
}

void Minimap::draw(Engine& painter)
{
  if( !isVisible() )
    return;

  if( DateTime::elapsedTime() - _d->lastTimeUpdate > 250 )
  {
    _d->updateImage();
    _d->lastTimeUpdate = DateTime::elapsedTime();
  }

  painter.draw( *_d->minimap, screenLeft(), screenTop() ); // 152, 145

  Widget::draw( painter );
}

void Minimap::setCenter( Point pos) {  _d->center = pos; }

bool Minimap::onEvent(const NEvent& event)
{
  if( sEventMouse == event.EventType
      && mouseLbtnRelease == event.mouse.type )
  {
    Point clickPosition = event.mouse.pos() - absoluteRect().UpperLeftCorner;

    int mapsize = _d->tilemap->size();

    clickPosition -= _d->getOffset();
    TilePos tpos;
    tpos.setI( (clickPosition.x() + clickPosition.y() - mapsize + 1) / 2 );
    tpos.setJ( -clickPosition.y() + tpos.i() + mapsize - 1 );

    _d->onCenterChangeSignal.emit( tpos );
  }

  return Widget::onEvent( event );
}

Signal1<TilePos>& Minimap::onCenterChange(){  return _d->onCenterChangeSignal; }

}//end namespace gui
