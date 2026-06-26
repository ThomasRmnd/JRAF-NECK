#ifndef JRAFNECK_SELECTION_MUON_HPP_
#define JRAFNECK_SELECTION_MUON_HPP_

#include "event/track.hpp"
#include "selection/selection.hpp"
#include "selection/spatial.hpp"
#include "selection/time.hpp"

class muon_veto_selection : public selection_base {

public:

    muon_veto_selection(const track& trk) :
        c_trk{trk}
    {}

    virtual ~muon_veto_selection() = default;

    virtual bool is_in(const vertex& vtx) const = 0;

    const track c_trk;

};

class time_range_muon_veto_selection : public muon_veto_selection {

public:

    time_range_muon_veto_selection(const track& trk, const timestamp& min, const timestamp& max) :
        muon_veto_selection{trk},
        c_time{trk.ts, min, max}
    {}

    ~time_range_muon_veto_selection() override = default;

    bool is_in(const vertex& vtx) const override {
        return c_time.is_in(vtx);
    }

private:

    const time_range_selection c_time;

};

class cylindrical_muon_veto_selection : public muon_veto_selection {

public:

    cylindrical_muon_veto_selection(const track& trk, double radius, const timestamp& min, const timestamp& max) :
        muon_veto_selection{trk},
        c_cylindrical{trk.ipos, trk.fpos, radius},
        c_time{trk.ts, min, max}
    {}

    ~cylindrical_muon_veto_selection() override = default;

    bool is_in(const vertex& vtx) const override {
        return c_cylindrical.is_in(vtx) && c_time.is_in(vtx);
    }

private:

    const cylindrical_selection c_cylindrical;
    const time_range_selection c_time;

};

#endif // JRAFNECK_SELECTION_MUON_HPP_