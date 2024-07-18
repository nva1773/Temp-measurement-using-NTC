//--------------------------------------------------------------------------------------------------------------------------
// Vdd ---o Vdd is supply voltage, typically 5V
// |
// Rt (thermistor) MF52A103J3470 NTC a nominal resistance of 10 kOm at 21 C
// |
// Vo ---o Vo is output from the voltage divider: Vo = Vdd*R/(R+Rt)
// |
// R1 (fixed resistor, R1 approx. equal to Rt
// |
// GND
//
Rt = R1*( 1023.0 / (float)Vo - 1.0 );

//--------------------------------------------------------------------------------------------------------------------------
// Vdd ---o Vdd is supply voltage, typically 5V
// |
// R1 (fixed resistor, R1 approx. equal to Rt
// |
// Vo ---o Vo is output from the voltage divider: Vo = Vdd*R/(R+Rt)
// |
// Rt (thermistor) NTC
// |
// GND
//
Rt = R1/( 1023.0 / (float)Vo - 1.0 );

//--------------------------------------------------------------------------------------------------------------------------
// Vdd ---o Vdd is supply voltage, typically 5V
// |
// R1 (fixed resistor, R1 approx. equal to Rt
// |
// Vo ---o Vo is output from the voltage divider: Vo = Vdd*Rt/(R+Rt)
// |
// Rt (thermistor) NTC
// |
// GND
// Rt = R1*(Vo/(Vdd-Vo))= R1/(Vdd/Vo - 1)
//
// Beta = see datatsheet on NTC (float)
// Tk = 1/(Ln(Rt/R25)/Beta + 1/T25)
//
// Rt and R25 are the nominal values of NTC resistance at T and T25 respectively (T expressed in K).
// R25 = 10000 at T25 = 298 (273+25); Rt = R/( 1023.0 / (float)Vo - 1.0 ); 
//
// Example:
/******************************************************************************
 * Function:        static void TemperatureCalc(void)
 *
 * Vdd ---o Vdd is supply voltage, typically 5V
 * |
 * R1 (fixed resistor, R1 approx. equal to Rt)
 * |
 * Vo ---o Vo is output from the voltage divider: Vo = Vdd*R/(R+Rt)
 * |
 * Rt (thermistor) NTC
 * |
 * GND
 *
 * R1 2700.0
 * Vdd = 1023.0
 * Rt = R1/(Vdd/Vo - 1)
 * R25 = 10000.0
 * Beta 3000.0
 * T = 1/(Ln(Rt/R25)/Beta + 1/T25)
 * Time calculate = 3244 mks (OSCILLATOR = 16MHz)
*****************************************************************************/
// Const and Variable of temperature NTC
#define Beta            3000.00  // the Beta-value for the NTC transitor
#define R25				10000.0  // R at T = 25 C
#define R1				2700.0	 // Fixed resistance in the voltage divider
#define Tamb            0.003356 // 1/T25 = 1/(273 + 25) = 1/298
#define Tzero           273.0   // 2730.0 for format XX.X; 273.0 for XX
//
int Vadc; // Integer value of voltage reading
float LogRt,Rt,T;
unsigned char Temperature;
//
Vadc = 805; // Rt = 10k (25C) ; For format XX.X need 12bit
Temperature = TemperatureCalc(Vadc);
// unsigned int for format XX.X
unsigned char TemperatureCalc(int Vo)
{
	unsigned char retval; // unsigned int for format XX.X
	// Calculation
	Rt = R1/( 1023.0 / (float)Vo - 1.0 ); // 4095 for format XX.X
	LogRt = log(Rt/R25);
	T = 1/(logRt/Beta + Tamb); // 10 for format XX.X
    T = T - Tzero;
	// Rounding to integer
	retval = (unsigned char)((T*10+5)/10);
	return retval;
}

// Для повышения точности измерения из 10 бит до 12 бит: (10 bit -> 12 bit)
// 1. Cкладываем 16 раз измеренное 10 битное значение и сдвигаем в право >> на 2 (12 бит).
// 2. Полученое 12 битные значения складываем 16 раз и делим на 16, 
// тем самым усредняя его (т.е. фильтруем помеху). В результате точные 12 бит.
