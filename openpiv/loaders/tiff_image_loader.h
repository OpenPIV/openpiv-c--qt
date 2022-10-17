
#pragma once

#include "loaders/image_loader.h"

namespace openpiv::core {

    /// loader of TIFF images with support for bit depths
    /// over 8-bits per channel; will sniff the input data for
    /// TIFF of 0x49 49 2a 00 or 0x4d 4d 00 2a
    class tiff_image_loader : public image_loader
    {
    public:
        tiff_image_loader();
        ~tiff_image_loader();

        std::string name() const override;
        int priority() const override;
        image_loader_ptr_t clone() const override;
        bool can_load( std::istream& ) const override;
        bool can_save() const override;
        size_t num_images() const override;

        bool open( std::istream& is ) override;
        bool extract( size_t index, image_g_16& ) override;
        bool extract( size_t index, image_g_f& ) override;
        bool extract( size_t index, image_rgba_16& ) override;

        void save( std::ostream&, const image_g_16& ) const override;
        void save( std::ostream&, const image_g_f& ) const override;
        void save( std::ostream&, const image_rgba_16& ) const override;
        void save( std::ostream&, const image_g_16_view& ) const override;
        void save( std::ostream&, const image_g_f_view& ) const override;
        void save( std::ostream&, const image_rgba_16_view& ) const override;

    private:
        struct impl;
        std::unique_ptr<impl> impl_;
    };

}
