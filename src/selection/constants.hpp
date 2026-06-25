#ifndef JRAFNECK_SELECTION_CONSTANTS_HPP_
#define JRAFNECK_SELECTION_CONSTANTS_HPP_

#include "selection/energy.hpp"
#include "selection/flasher.hpp"
#include "selection/muon.hpp"
#include "selection/spatial.hpp"
#include "selection/time.hpp"
#include "selection/vertex.hpp"

// ------------------------------------------------------------------------------------------------
// Energy
// ------------------------------------------------------------------------------------------------

inline const energy_range_selection g_prompt_energy_cut{0.7, 12.0};
inline const energy_range_selection g_delayed_hydrogen_energy_cut{2.0, 2.5};
inline const energy_range_selection g_delayed_carbon_energy_cut{4.5, 5.5};

inline const energy_range_selection g_multiplicity_energy_cut{2.0, 12.0};
inline const energy_range_selection g_neutron_energy_cut{1.5, 20.0};

// ------------------------------------------------------------------------------------------------
// Flasher
// ------------------------------------------------------------------------------------------------

const flasher_selection g_flasher_cut{0.55, 0.45, 170.0, 80.0};

// ------------------------------------------------------------------------------------------------
// Muon
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Spatial
// ------------------------------------------------------------------------------------------------

inline const fiducial_volume_selection g_fiducial_volume_cut{16500.0}; // 17200.0
inline const fiducial_volume_selection g_acrylic_sphere_cut{17700.0};
inline const chimney_selection g_chimney_cut{15500.0, 2000.0};

// ------------------------------------------------------------------------------------------------
// Time
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Vertex
// ------------------------------------------------------------------------------------------------

#endif // JRAFNECK_SELECTION_CONSTANTS_HPP_