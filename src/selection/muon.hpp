#ifndef JRAFNECK_SELECTION_MUON_HPP_
#define JRAFNECK_SELECTION_MUON_HPP_

#include "event/track.hpp"
#include "selection/selection.hpp"
#include "selection/spatial.hpp"
#include "selection/time.hpp"

namespace jraf {

class muon_veto_selection : public jraf::selection {

public:

    muon_veto_selection(const jraf::track& trk) :
        c_trk{trk}
    {}

    virtual ~muon_veto_selection() = default;

    virtual bool is_in(const jraf::vertex& vtx) const = 0;

    const jraf::track c_trk;

};

class time_range_muon_veto_selection : public muon_veto_selection {

public:

    time_range_muon_veto_selection(const jraf::track& trk, const jraf::timestamp& min, const jraf::timestamp& max) :
        muon_veto_selection{trk},
        c_time{trk.ts, min, max}
    {}

    ~time_range_muon_veto_selection() override = default;

    bool is_in(const jraf::vertex& vtx) const override {
        return c_time.is_in(vtx);
    }

private:

    const jraf::time_range_selection c_time;

};

class cylindrical_muon_veto_selection : public muon_veto_selection {

public:

    cylindrical_muon_veto_selection(const jraf::track& trk, double radius, const jraf::timestamp& min, const jraf::timestamp& max) :
        muon_veto_selection{trk},
        c_cylindrical{trk.ipos, trk.fpos, radius},
        c_time{trk.ts, min, max}
    {}

    ~cylindrical_muon_veto_selection() override = default;

    bool is_in(const jraf::vertex& vtx) const {
        return c_cylindrical.is_in(vtx) && c_time.is_in(vtx);
    }

private:

    const jraf::cylindrical_selection c_cylindrical;
    const jraf::time_range_selection c_time;

};

} // namespace jraf

#endif // JRAFNECK_SELECTION_MUON_HPP_