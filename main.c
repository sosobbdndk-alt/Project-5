#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define SENSOR_TEMP    0U
#define SENSOR_HUMID   1U
#define SENSOR_LIGHT   2U
#define SENSOR_SOIL    3U
#define SENSOR_COUNT   4U

#define ACT_HEATER     0U
#define ACT_COOLER     1U
#define ACT_HUMIDIFIER 2U
#define ACT_LIGHTS     3U
#define ACT_IRRIGATION 4U
#define ACTUATOR_COUNT 5U

#define HISTORY_LEN    10U

typedef struct {
    int16_t  temp;     /* -10..60 deg C */
    uint16_t humid;    /* 0..100 % */
    uint16_t light;    /* 0..2000 Lux */
    uint16_t soil;     /* 0..100 % */
} Reading_t;

static Reading_t history[HISTORY_LEN];
static uint8_t   historyCount;
static Reading_t currentRead;
static uint8_t   actuators[ACTUATOR_COUNT];

/* Function Prototypes */
static void initController(void);
static void showDashboard(void);
static void setSensors(void);
static void autoControl(void);
static void pushHistory(Reading_t r);
static void showAverages(void);
static void climateReport(void);

static int readInt(int *out)
{
    char buf[64];
    if (fgets(buf, (int)sizeof(buf), stdin) == NULL) {
        return 0;
    }
    return sscanf(buf, "%d", out) == 1;
}

static void initController(void)
{
    currentRead.temp = 22;
    currentRead.humid = 65U;
    currentRead.light = 600U;
    currentRead.soil = 55U;

    for (uint8_t i = 0U; i < ACTUATOR_COUNT; i++) {
        actuators[i] = 0U;
    }
    historyCount = 0U;
}

static void pushHistory(Reading_t r)
{
    if (historyCount < HISTORY_LEN) {
        history[historyCount++] = r;
    } else {
        for (uint8_t i = 0U; i < HISTORY_LEN - 1U; i++) {
            history[i] = history[i + 1U];
        }
        history[HISTORY_LEN - 1U] = r;
    }
}

static void autoControl(void)
{
    /* Temperature Control (Ideal: 18 - 28 C) */
    actuators[ACT_HEATER] = (currentRead.temp < 18) ? 1U : 0U;
    actuators[ACT_COOLER] = (currentRead.temp > 28) ? 1U : 0U;

    /* Humidity Control (Ideal: >= 50%) */
    actuators[ACT_HUMIDIFIER] = (currentRead.humid < 50U) ? 1U : 0U;

    /* Lighting Control (Ideal: >= 400 Lux) */
    actuators[ACT_LIGHTS] = (currentRead.light < 400U) ? 1U : 0U;

    /* Irrigation Control (Ideal Soil: >= 40%) */
    actuators[ACT_IRRIGATION] = (currentRead.soil < 40U) ? 1U : 0U;

    pushHistory(currentRead);
}

static void showDashboard(void)
{
    const char *actNames[ACTUATOR_COUNT] = {
        "Heater", "Cooler", "Humidifier", "Grow Lights", "Irrigation Pump"
    };

    printf("\n--- Greenhouse Live Sensors ---\n");
    printf("Temperature : %d C\n", currentRead.temp);
    printf("Humidity    : %u %%\n", currentRead.humid);
    printf("Light Level : %u Lux\n", currentRead.light);
    printf("Soil Moisture: %u %%\n", currentRead.soil);

    printf("\n--- Actuator States ---\n");
    for (uint8_t i = 0U; i < ACTUATOR_COUNT; i++) {
        printf("[%-16s] : %s\n", actNames[i], actuators[i] ? "ON (ACTIVE)" : "OFF (IDLE)");
    }
}

static void setSensors(void)
{
    int t = 0, h = 0, l = 0, s = 0;
    printf("Enter Temp (-10..60 C): ");
    if (!readInt(&t) || t < -10 || t > 60) {
        printf("Invalid temperature!\n");
        return;
    }
    printf("Enter Humidity (0..100 %%): ");
    if (!readInt(&h) || h < 0 || h > 100) {
        printf("Invalid humidity!\n");
        return;
    }
    printf("Enter Light (0..2000 Lux): ");
    if (!readInt(&l) || l < 0 || l > 2000) {
        printf("Invalid light!\n");
        return;
    }
    printf("Enter Soil Moisture (0..100 %%): ");
    if (!readInt(&s) || s < 0 || s > 100) {
        printf("Invalid soil moisture!\n");
        return;
    }

    currentRead.temp = (int16_t)t;
    currentRead.humid = (uint16_t)h;
    currentRead.light = (uint16_t)l;
    currentRead.soil = (uint16_t)s;

    autoControl();
    printf("Sensors updated. Auto-control applied.\n");
}

static void showAverages(void)
{
    if (historyCount == 0U) {
        printf("No sensor history recorded yet.\n");
        return;
    }

    int32_t tSum = 0;
    uint32_t hSum = 0U, lSum = 0U, sSum = 0U;

    for (uint8_t i = 0U; i < historyCount; i++) {
        tSum += history[i].temp;
        hSum += history[i].humid;
        lSum += history[i].light;
        sSum += history[i].soil;
    }

    printf("\n--- Historical Averages (%u samples) ---\n", historyCount);
    printf("Avg Temperature  : %d C\n", (int)(tSum / (int32_t)historyCount));
    printf("Avg Humidity     : %u %%\n", (uint16_t)(hSum / historyCount));
    printf("Avg Light        : %u Lux\n", (uint16_t)(lSum / historyCount));
    printf("Avg Soil Moisture: %u %%\n", (uint16_t)(sSum / historyCount));
}

static void climateReport(void)
{
    uint8_t activeCount = 0U;
    for (uint8_t i = 0U; i < ACTUATOR_COUNT; i++) {
        if (actuators[i]) {
            activeCount++;
        }
    }

    printf("\n========= GREENHOUSE CLIMATE REPORT =========\n");
    printf("Total Historical Samples : %u / %u\n", historyCount, HISTORY_LEN);
    printf("Active Actuators         : %u of %u\n", activeCount, ACTUATOR_COUNT);
    printf("Climate Status           : %s\n", (activeCount == 0U) ? "BALANCED (Optimal)" : "REGULATING");
    printf("Heating / Cooling State  : %s\n",
           actuators[ACT_HEATER] ? "HEATING" : (actuators[ACT_COOLER] ? "COOLING" : "IDLE"));
    printf("Irrigation Status        : %s\n", actuators[ACT_IRRIGATION] ? "PUMPING WATER" : "OFF");
    printf("=============================================\n");
}

int main(void)
{
    initController();
    autoControl();
    int opt = -1;

    do {
        printf("\n1.Dashboard 2.UpdateSensors 3.EvaluateNow 4.Averages 5.Report 0.Exit > ");
        if (!readInt(&opt)) {
            printf("Invalid input!\n");
            continue;
        }

        switch (opt) {
            case 1: showDashboard(); break;
            case 2: setSensors(); break;
            case 3: autoControl(); printf("Evaluated.\n"); break;
            case 4: showAverages(); break;
            case 5: climateReport(); break;
            case 0: printf("Shutting down controller...\n"); break;
            default: printf("Unknown option!\n"); break;
        }
    } while (opt != 0);

    return 0;
}