
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
        virtual ~pnm_image_loader();

        virtual bool can_load( std::istream& ) const override;
        virtual bool can_save() const override;
        virtual void load( std::istream&, g16_image& ) const override;
        virtual void load( std::istream&, gf_image& ) const override;
        virtual void load( std::istream&, rgba16_image& ) const override;
        virtual void save( std::ostream&, const g16_image& ) const override;
        virtual void save( std::ostream&, const gf_image& ) const override;
        virtual void save( std::ostream&, const rgba16_image& ) const override;
        virtual void save( std::ostream&, const g16_image_view& ) const override;
        virtual void save( std::ostream&, const gf_image_view& ) const override;
        virtual void save( std::ostream&, const rgba16_image_view& ) const override;

        virtual std::string name() const override;
        virtual int priority() const override;
    };

}
