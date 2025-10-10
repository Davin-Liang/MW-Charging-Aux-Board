#ifndef __TEST_TASK_H
#define	__TEST_TASK_H

#include "ff.h"

FRESULT write_arrays_to_CSV(const TCHAR * path, float * historyVoltages, float * historyPowers);
FRESULT write_v_p_to_csv(const TCHAR * path, float voltage, float power, int index,int channel);
FRESULT write_x_y_v_p_to_csv(const TCHAR * path, float x, float y, float * optimalVs, float optimalP);

#endif


