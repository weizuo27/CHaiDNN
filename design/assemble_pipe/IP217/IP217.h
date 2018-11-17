/*----------------------------------------------------
Copyright 2017 Xilinx, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
----------------------------------------------------*/

#include <ap_int.h>
#include <string.h>

//# Debug flag
#define DEBUG			0

#include "IP217_config.h"
#include "IP217.hpp"

#define DPACK 16

namespace IP217
{
void PoolingLayerLineBuffer
(
    GMEM_MAXPOOLTYPE* inMem1,
    GMEM_MAXPOOLTYPE* inMem2,
    hls::stream< GMEM_MAXPOOLTYPE > &inStream1,
    hls::stream< GMEM_MAXPOOLTYPE > &inStream2,
    GMEM_MAXPOOLTYPE* outMem1,
    GMEM_MAXPOOLTYPE* outMem2,
    hls::stream< GMEM_MAXPOOLTYPE > &outStream1,
    hls::stream< GMEM_MAXPOOLTYPE > &outStream2,
    int* scalarPoolArgs
);

}



