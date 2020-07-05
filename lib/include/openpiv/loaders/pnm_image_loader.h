
#pragma once

#include "loaders/image_loader.h"

namespace openpiv::core {

    /// loader of PNM images with support for bit depths
    /// over 8-bits per channel; will sniff the input data for
    /// P[1-6] header.
    ///
    /// Treats contained data as linear i.e. not a "true" PNM
    /// image as no gamma correction is applied
    class pnm_image_loader : public image_loader
    {
    public:
        pnm_image_loader();
        ~pnm_image_loader();

        std::string name() const override;
        int priority() const override;
        image_loader_ptr_t clone() const override;
        bool can_load( std::istream& ) const override;
        bool can_save() const override;
        size_t num_images() const override;

        bool open( std::istream& is ) override;
        bool extract( size_t index, g16_image& ) override;
        bool extract( size_t index, gf_image& ) override;
        bool extract( size_t index, rgba16_image& ) override;

        void save( std::ostream&, const g16_image& ) const override;
        void save( std::ostream&, const gf_image& ) const override;
        void save( std::ostream&, const rgba16_image& ) const override;
        void save( std::ostream&, const g16_image_view& ) const override;
        void save( std::ostream&, const gf_image_view& ) const override;
        void save( std::ostream&, const rgba16_image_view& ) const override;

    private:
        struct impl;
        std::unique_ptr<impl> impl_;
    };

}
