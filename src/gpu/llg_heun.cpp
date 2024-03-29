//-----------------------------------------------------------------------------
//
// This source file is part of the VAMPIRE open source package under the
// GNU GPL (version 2) licence (see licence file for details).
//
// (c) R F L Evans 2014. All rights reserved.
//
//-----------------------------------------------------------------------------

// C++ standard library headers

// Vampire headers
#include "gpu.hpp"
#include "cuda.hpp"
#include "errors.hpp"
//#include "opencl.hpp"

namespace gpu{

   //--------------------------------------------------------------------------
   // Function to call correct llg_heun function depending on cuda,opencl etc
   //--------------------------------------------------------------------------
   void llg_heun(){

      #ifdef CUDA
         cuda::llg_heun();
      #elseif OPENCL
         opencl::llg_heun();
      #endif

      return;
   }

} // end of namespace gpu
