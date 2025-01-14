// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef __MC_LEVEL_HPP__
#define __MC_LEVEL_HPP__

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "mc/dynamic_buffer.hpp"
#include "mc/utils.hpp"
#include "mc/marker.hpp"

#include "nbt/types.hpp"

namespace mc {
  namespace fs = boost::filesystem;

  class level_info;
  class level;
  class region;

  typedef boost::shared_ptr<level_info> level_info_ptr;
  typedef boost::shared_ptr<level> level_ptr;
  typedef boost::shared_ptr<region> region_ptr;

  class invalid_file : std::exception {
    private:
      const char* message;
    public:
      invalid_file(const char* message) : message(message) {}

      const char* what() const throw() {
        return message;
      }
  };

  struct Palette_Compound {
    boost::shared_ptr<nbt::String> Name;
  };

  struct Section_Compound {
    boost::shared_ptr<nbt::ByteArray> BlockStates;
    boost::ptr_vector<Palette_Compound> Palette;
    boost::shared_ptr<nbt::ByteArray> SkyLight;
    boost::shared_ptr<nbt::ByteArray> BlockLight;
    nbt::Byte Y;
  };

  struct Level_Compound {
    nbt::Int xPos;
    nbt::Int zPos;
    boost::shared_ptr<nbt::IntArray> Heightmaps;
    boost::ptr_vector<Section_Compound> Sections;
  };

  class level
  {
    public:
      level(level_info_ptr _level_info);
      ~level();

      std::string get_path();
      time_t modification_time();

      /*
       * might throw invalid_file if the file is not gramatically correct
       */
      void read(dynamic_buffer& buffer);

      boost::shared_ptr<Level_Compound> get_level();

      bool operator<(const level& other) const;
    private:
      level_info_ptr _level_info;

      // these must be public for the parser to be able to reach them.
      std::vector<marker> signs;

      boost::shared_ptr<Level_Compound> Level;
  };
}

#endif /* __MC_LEVEL_HPP__ */
