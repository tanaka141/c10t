#ifndef _ENGINE_ISOMETRIC_BASE_HPP
#define _ENGINE_ISOMETRIC_BASE_HPP

#include "engine/engine_base.hpp"
#include "engine/block_rotation.hpp"
#include "engine/functions.hpp"

#include <boost/foreach.hpp>

template<typename C>
class isometric_base : public engine_base<C> {
public:
  typedef uint64_t pos_t;
  typedef boost::shared_ptr<mc::level> level_ptr;
  typedef boost::shared_ptr<image_operations> image_operations_ptr;

  isometric_base(engine_settings& s, mc::world& world)
    : engine_base<C>(s, world)
  {
  }

  void render(level_ptr level, image_operations_ptr oper)
  {
    const engine_settings& s = engine_base<C>::get_settings();

    boost::shared_ptr<mc::Level_Compound> L = level->get_level();

    pos_t image_width = 0;
    pos_t image_height = 0;

    engine_base<C>::project_limits(image_width, image_height);

    oper->set_limits(image_width + 1, image_height);

    BOOST_FOREACH(mc::Section_Compound Section, L->Sections) {
      block_rotation br_blockstates(s.rotation, Section.BlockStates);
      block_rotation br_palette(s.rotation, Section.Palette);
      block_rotation br_block_light(s.rotation, Section.BlockLight);
      block_rotation br_sky_light(s.rotation, Section.SkyLight);

      for (int y = 0; y < 16; y++) {
        int abs_y = (16 * Section.Y) + y;

        for (int z = 0; z < mc::MapZ; z++) {
          for (int x = mc::MapX - 1; x >= 0; x--) {
            br_blockstates.set_xz(x, z);
            br_palette.set_xz(x, z);
            br_block_light.set_xz(x, z);
            br_sky_light.set_xz(x, z);

            int block_type = br_blockstates.get8(y);

            if (block_type >=0 && s.excludes[block_type]) {
              continue;
            }

            point p(x, abs_y, z);

            pos_t px = 0;
            pos_t py = 0;

            engine_base<C>::project_position(p, px, py);

            int block_data = br_palette.get4(y);

            color top = mc::get_color(block_type, block_data);
            color side = mc::get_side_color(block_type, block_data);

            //int block_light = br_block_light.get4(y + 1);
            //int sky_light = br_sky_light.get4(y + 1);

            //apply_shading(s, block_light, sky_light, 0, y, top);
            //apply_shading(s, 0, 0, 0, y, side);

            switch(mc::MaterialModes[block_type]) {
            case mc::Block:
              render_block(oper, block_type, px, py, top, side);
            case mc::HalfBlock:
              render_halfblock(oper, block_type, px, py, top, side);
              break;
            case mc::TorchBlock:
              render_torchblock(oper, block_type, px, py, top, side);
              break;
            case mc::LargeFlowerBlock:
              // Check if the requested block is the top block
              if(block_data & 0x08) {
                // Small sanity check
                if(y > 0 && br_blockstates.get8(y-1) == block_type) {
                  // Minecraft currently doesn't set the lower bits to the
                  // corresponding type so we have to do this here.
                  block_data = br_palette.get4(y-1) | 0x08;
                  top =  mc::get_color(block_type, block_data);
                  side = mc::get_side_color(block_type, block_data);
                }
                else {
                  // Top block not placed on a correct bottom block.
                  // The expected LargeFlower multi block structure is invalid, skip it.
                  continue;
                }
              }
              render_block(oper, block_type, px, py, top, side);
              break;
            }
          }
        }
      }
    }
  }

  virtual void render_block(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
  virtual void render_halfblock(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
  virtual void render_torchblock(image_operations_ptr, int, pos_t, pos_t, color, color) = 0;
};

#endif /* _ENGINE_ISOMETRIC_BASE_HPP */
