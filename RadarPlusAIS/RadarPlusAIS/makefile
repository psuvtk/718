OBJ=RadarPlusAIS.cc TransParaFactoryUtil.cpp DES.cpp md5.cpp

UnitTest_AIS_OBJ=TransParaFactoryUtil.cpp DES.cpp md5.cpp UnitTest_AIS.cc
UnitTest_Radar_OBJ=TransParaFactoryUtil.cpp DES.cpp md5.cpp UnitTest_Radar.cc

CC=g++
LDFLAG=-lpthread


RadarPlusAis.run: $(OBJ)
	$(CC) $(OBJ) $(LDFLAG) -o $@

UnitTest: $(UnitTest_AIS_OBJ) $(UnitTest_Radar_OBJ)
	rm -f *.o
	rm -f RadarPlusAis.run UnitTest_AIS.run UnitTest_Radar.run main
	$(CC) UnitTest_AIS.cc $(LDFLAG) -o UnitTest_AIS.run
	$(CC) $(UnitTest_Radar_OBJ) $(LDFLAG) -o UnitTest_Radar.run


.PHONY: clean
clean:
	rm -f *.o
	rm -f RadarPlusAis.run UnitTest_AIS.run UnitTest_Radar.run main
