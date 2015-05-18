//-----------------------------------------------------------------------------
//
//  Vampire - A code for atomistic simulation of magnetic materials
//
//  Copyright (C) 2009-2012 R.F.L.Evans
//
//  Email:richard.evans@york.ac.uk
//
//  This program is free software; you can redistribute it and/or modify 
//  it under the terms of the GNU General Public License as published by 
//  the Free Software Foundation; either version 2 of the License, or 
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License 
//  along with this program; if not, write to the Free Software Foundation, 
//  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// ----------------------------------------------------------------------------
//
///
/// @file
/// @brief Contains the Hysteresis program
///
/// @details Performs a field loop to calculate field dependent magnetisation
///
/// @section License
/// Use of this code, either in source or compiled form, is subject to license from the authors.
/// Copyright \htmlonly &copy \endhtmlonly Richard Evans, 2009-2010. All Rights Reserved.
///
/// @section info File Information
/// @author  Richard Evans, richard.evans@york.ac.uk
/// @version 1.1
/// @date    21/03/2011
/// @internal
///	Created:		05/02/2011
///	Revision:	21/03/2011
///=====================================================================================
///

// Standard Libraries
#include <cstdlib>

// Vampire Header files
#include "vmath.hpp"
#include "errors.hpp"
#include "sim.hpp"
#include "stats.hpp"
#include "vio.hpp"


namespace program{

/// @brief Function to calculate the hysteresis loop
///
/// @callgraph
/// @callergraph
///
/// @details Consists of a sequence of sub-calculations of fixed temperature. The system is initialised 
/// ordered. After initialisation a whole hysteresis loop of the system and coercivity are calculated.
///
/// @section License
/// Use of this code, either in source or compiled form, is subject to license from the authors.
/// Copyright \htmlonly &copy \endhtmlonly Richard Evans, 2009-2010. All Rights Reserved.
///
/// @section Information
/// @author  Weijia Fan, wf507@york.ac.uk
/// @version 1.0
/// @date    27/01/2010
///
/// @return EXIT_SUCCESS
/// 
/// @internal
///	Created:		27/01/2010
///	Revision:	  ---
///=====================================================================================
///
int hysteresis(){
	
	// check calling of routine if error checking is activated
	if(err::check==true){std::cout << "program::hysteresis has been called" << std::endl;}
	
	// Setup min and max fields and increment (uT)
	int iHmax=vmath::iround(double(sim::Hmax)*1.0E6);
	int iHmin=vmath::iround(double(sim::Hmin)*1.0E6);
	int miHmax=-iHmax;       
	int parity_old;
	int iH_old;
	int start_time;

	// Equilibrate system in saturation field
	sim::H_applied=sim::Heq;
	// Initialise sim::integrate only if it not a checkpoint
	if(sim::load_checkpoint_flag && sim::load_checkpoint_continue_flag){ 
	}
	else sim::integrate(sim::equilibration_time);
	
        // Hinc must be positive
	int iHinc=vmath::iround(double(fabs(sim::Hinc))*1.0E6);

        int Hfield;
        int iparity=sim::parity;
	parity_old=iparity;

        // Save value of iH from previous simulation
	if(sim::load_checkpoint_continue_flag){
		iH_old=int(sim::iH);
//		std::cout << std::endl << parity_old << "\t" << sim::iH << "\t" << iH_old << "\t" << sim::time << std::endl;
	}

//        std::cout << iparity << "\t" << Hfield << "\t" << sim::parity << "\t" << sim::iH << "\t" << iHmin << "\t" << iHmax << "\t" << sim::Hmin << "\t" << sim::Hmax << std::endl << std::endl;

	// Perform Field Loop -parity
	while(iparity<2){

		if(sim::load_checkpoint_flag && sim::load_checkpoint_continue_flag){
			if(parity_old<0){
				if(iparity<0){
//					if(iH_old<=0) miHmax=iH_old+iHinc;
//					else if(iH_old>0) miHmax=iH_old+iHinc;
					miHmax=iH_old;
//					miHmax=iH_old+iHinc;
				}
				else if(iparity>0 && iH_old<=0) miHmax=(iHmax-iHinc);
//				else if(iparity>0 && iH_old>0) miHmax=-iHmax; //iH_old;
				else if(iparity>0 && iH_old>0) miHmax=-(iHmax);
			}
			else if(parity_old>0){
//				if(iH_old<=0) miHmax=iH_old;
//				else if(iH_old>0) miHmax=iHinc;
				miHmax=iH_old;
//				miHmax=iH_old+iHinc;
			}
			Hfield=miHmax;
		}
		else	Hfield=miHmax;

		// Perform Field Loop -field
		while(Hfield<=iHmax){
			
			// Set applied field (Tesla)
			sim::H_applied=double(Hfield)*double(iparity)*1.0e-6;
//			sim::H_applied=double(sim::iH)*double(sim::parity)*1.0e-6;
			
			// Reset start time
			start_time=sim::time;
			
			// Reset mean magnetisation counters
			stats::mag_m_reset();

			// Integrate system
			while(sim::time<sim::loop_time+start_time){

				// Integrate system
				sim::integrate(sim::partial_time);
			
				// Calculate mag_m, mag
				stats::mag_m();

			}

			// Increment of iH
			Hfield+=iHinc;
			sim::iH=int64_t(Hfield); //sim::iH+=iHinc;

			// Output to screen and file after each field
			vout::data();

//			std::cout << iparity << "\t" << Hfield << "\t" << sim::parity << "\t" << sim::iH << "\t" << iHmax << "\t" << iHinc << "\t" << sim::time << "\t" << start_time << "\t" << sim::partial_time << "\t" << sim::loop_time << std::endl;

		} // End of field loop
                
		// Increment of parity
                iparity+=2;
                sim::parity=int64_t(iparity);
//                sim::parity+=2;

//        std::cout << iparity << "\t" << Hfield << "\t" << sim::parity << "\t" << sim::iH << "\t" << iHmin << "\t" << iHmax << "\t" << sim::Hmin << "\t" << sim::Hmax << "\t" <<std::endl<<std::endl; 
	} // End of parity loop

//        std::cout << iparity << "\t" << Hfield << "\t" << sim::parity << "\t" << sim::iH << "\t" << iHmin << "\t" << iHmax << "\t" << sim::Hmin << "\t" << sim::Hmax << "\t" <<std::endl<<std::endl; 


	return EXIT_SUCCESS;
  }

}//end of namespace program


