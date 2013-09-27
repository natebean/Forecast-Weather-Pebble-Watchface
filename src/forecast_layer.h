#ifndef FORECAST_LAYER_H
#define FORECAST_LAYER_H

#include "weather_layer.h"

typedef struct {
	Layer layer;
	BmpContainer icon_layer;
	TextLayer day_layer;
	TextLayer temp_layer;
	//TextLayer temp_layer_background;
	bool has_weather_icon;
	char temp_str[8];
} ForecastLayer;


void forecast_layer_init(ForecastLayer* forecast_layer, GPoint pos);
void forecast_layer_deinit(ForecastLayer* forecast_layer);
void forecast_layer_set_icon(ForecastLayer* forecast_layer, WeatherIcon icon);
//void forecast_layer_set_temperature(ForecastLayer* forecast_layer, int16_t temperature);

#endif
