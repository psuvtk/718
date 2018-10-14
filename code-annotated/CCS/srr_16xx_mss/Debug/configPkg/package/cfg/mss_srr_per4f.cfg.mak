# invoke SourceDir generated makefile for mss_srr.per4f
mss_srr.per4f: .libraries,mss_srr.per4f
.libraries,mss_srr.per4f: package/cfg/mss_srr_per4f.xdl
	$(MAKE) -f F:\Workspace\CCS\srr_16xx_mss/src/makefile.libs

clean::
	$(MAKE) -f F:\Workspace\CCS\srr_16xx_mss/src/makefile.libs clean

