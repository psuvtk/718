/*----------------------------------------------------------------------------*/
/* Linker Settings                                                            */
--retain="*(.intvecs)"

--stack_size=0x700

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */
-ldsplib.ae64P
-llibmmwavealg_xwr16xx.ae674
-lmathlib.ae674
-llibsoc_xwr16xx.ae674

SECTIONS
{
    systemHeap : {} >> L2SRAM_UMAP0 | L2SRAM_UMAP1
    .l2data : {} >> L2SRAM_UMAP0 | L2SRAM_UMAP1

    /* HSSRAM has output data from processing chain running on DSP */
    .demoSharedMem > HSRAM

    /* L3SRAM has code that is overlaid with data, so data must be
       marked uninitialized. Application can initialize this section
       using _L3data_* symbols defined below. Code should be written carefully as
       these are linker symbols (see for example http://e2e.ti.com/support/development_tools/compiler/f/343/t/92002 ):
        
        extern far uint8_t _L3data_start; // the type here does not matter
        extern far uint8_t _L3data_size;  // the type here does not matter

        memset((void *)_symval(&_L3data_start), 0, (uint32_t) _symval(&_L3data_size));
    */ 
    .l3data: type=NOINIT, start(_L3data_start), size(_L3data_size), load=L3SRAM PAGE 1

    /* Bootloader cannot load L1DSRAM, make sure to mark as NOINIT */
    .l1data : type=NOINIT, load=L1DSRAM

    /* Currently bootloader does not allow loading in L1PSRAM because of supporting
       low power. Below fast code is loaded in L3SRAM but run from L1PSRAM. The copy-in
       is called during initialization phase and copy-out is not used but can be added when
       supporting low-power mode (where L1 contents are not retained).
     */    
    .fastCode: 
    {
        dsplib.ae64P(.text)
        libmmwavealg_xwr16xx.ae674(.text)
        mathlib.ae674(.text)
        dss_data_path.oe674(.text:secondDimFFTandLog2Computation)
        dss_data_path.oe674(.text:MmwDemo_processChirp)
        dss_data_path.oe674(.text:MmwDemo_interChirpProcessing)
        dss_data_path.oe674(.text:MmwDemo_XYestimation)
        dss_data_path.oe674(.text:cfarCa_SO_dBwrap_withSNR)
        dss_data_path.oe674(.text:cfarCadB_SO_withSNR)
        /*dss_data_path.oe674(.text:cfarPeakGroupingAlongDoppler)
        dss_data_path.oe674(.text:pruneToPeaks)
        dss_data_path.oe674(.text:findKLargestPeaks)
        dss_data_path.oe674(.text:disambiguateVel)
        dss_data_path.oe674(.text:MmwDemo_setDopplerLine)
        dss_data_path.oe674(.text:MmwDemo_getDopplerLine)
        */
        dss_data_path.oe674(.text:rangeBasedPruning)
       /* dss_data_path.oe674(.text:MmwDemo_magnitudeSquared)*/
        dss_data_path.oe674(.text:azimuthProcessing)
        dss_data_path.oe674(.text:MmwDemo_waitEndOfChirps)
        clusteringDBscan.oe674(.text:clusteringDBscan_findNeighbors2Fixed)
        Extended_Kalman_Filter_xyz.oe674(.text:isTargetWithinDataAssociationThresh)
    } load=L3SRAM PAGE 0, run=L1PSRAM PAGE 0, table(_MmwDemo_fastCode_L1PSRAM_copy_table, compression=off)
    
    /* This is auto generated by linker related to copy table above */
    .ovly > L2SRAM_UMAP0 | L2SRAM_UMAP1
    
    /* Overlay one-time/init-time (and non-critical in cycles) with L3 data,
       will be erased during data path processing. Note do not put any
       code that is required related to start/stop/reconfig processing */
    .overlay:
    {
        libsoc_xwr16xx.ae674 (.text:SOC_init)
        dss_main.oe674 (.text:main)
        dss_main.oe674 (.text:MmwDemo_populateSRR)
        dss_main.oe674 (.text:MmwDemo_populateUSRR)
        dss_data_path.oe674 (.text:MmwDemo_dataPathInitEdma)
    } > L3SRAM PAGE 0
   
}
/*----------------------------------------------------------------------------*/
