
// openpiv
#include "core/exception_builder.h"
#include "core/image.h"
#include "core/util.h"
#include "loaders/image_loader.h"

using namespace openpiv;
using namespace openpiv::core;

// local
#include "pyutils.h"

// std
#include <fstream>
#include <vector>

// pybind
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;


void add_loader(py::module& m)
{
    m.def( "load_g_u16",
           [](const std::string& filename) -> std::vector<image_g_16>
           {
               // open file
               std::ifstream is(filename, std::ios::binary);
               if ( !is.is_open() )
                   core::exception_builder<std::runtime_error>() << "failed to open " << filename;

               // find loader
               auto loader{ core::image_loader_registry::find(is) };
               if ( !loader )
                   core::exception_builder<std::runtime_error>() << "failed to find loader for " << filename;

               // open image file
               if ( !loader->open(is) )
                   core::exception_builder<std::runtime_error>() << "failed to open with loader " << filename;

               // load all images
               std::vector<image_g_16> images;
               for ( size_t index = 0; index < loader->num_images(); ++index )
               {
                   image_g_16 im;
                   loader->extract( index, im );
                   images.push_back( std::move( im ) );
               }
               return images;
           });
}

