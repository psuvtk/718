# invoke SourceDir generated makefile for dss_srr.pe674
dss_srr.pe674: .libraries,dss_srr.pe674
.libraries,dss_srr.pe674: package/cfg/dss_srr_pe674.xdl
	$(MAKE) -f F:\Workspace\CCS\srr_16xx_dss/src/makefile.libs

clean::
	$(MAKE) -f F:\Workspace\CCS\srr_16xx_dss/src/makefile.libs clean

