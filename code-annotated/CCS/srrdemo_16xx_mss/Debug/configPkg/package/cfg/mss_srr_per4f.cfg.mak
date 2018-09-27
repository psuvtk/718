# invoke SourceDir generated makefile for mss_srr.per4f
mss_srr.per4f: .libraries,mss_srr.per4f
.libraries,mss_srr.per4f: package/cfg/mss_srr_per4f.xdl
	$(MAKE) -f H:\Workspace\CCS\srrdemo_16xx_mss/src/makefile.libs

clean::
	$(MAKE) -f H:\Workspace\CCS\srrdemo_16xx_mss/src/makefile.libs clean

