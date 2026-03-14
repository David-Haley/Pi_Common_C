// No-op GPIO stub for simulation builds.
// Implements the same symbols as gpio_driver.c but without libgpiod or
// any hardware-specific headers. All operations succeed (return 0 or a
// valid non-negative line number) so that Ada elaboration code that calls
// Open_GPIO_Chip and Open_Output at startup does not raise exceptions.

#include <string.h>

int Open_GPIO_Chip (const char *Chip_Name)
{
    (void)Chip_Name;
    return 0; // success
}

void Close_GPIO_Line (int Line_Number)
{
    (void)Line_Number;
}

void Close_GPIO_Chip (void)
{
}

int Open_Output (const char *Line_Name)
{
    (void)Line_Name;
    return 0; // fake line number 0 (not -1, so Ada Bind_Pin does not raise)
}

int Set_High (int Line_Number)
{
    (void)Line_Number;
    return 0;
}

int Set_Low (int Line_Number)
{
    (void)Line_Number;
    return 0;
}

int Pulse_High (int Line_Number)
{
    (void)Line_Number;
    return 0;
}

int Pulse_Low (int Line_Number)
{
    (void)Line_Number;
    return 0;
}

int Open_Input (const char *Line_Name)
{
    (void)Line_Name;
    return 0; // fake line number
}

int Read_Input (int Line_Number)
{
    (void)Line_Number;
    return 0; // pin low
}
