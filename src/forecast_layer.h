#ifndef FORECAST_LAYER_H
#define FORECAST_LAYER_H

#include "weather_layer.h"
#include "receive_keys.h"

typedef struct {
	Layer layer;
	BmpContainer icon_layer;
	TextLayer day_layer;
	TextLayer temp_layer;
	TextLayer temp_time_layer;
	bool has_weather_icon;
  char day_name[8];
  char precip_prob[5];
  char temp[5];
  char output_string[12];
  bool time_bool;
  char time_min[8];
  char time_max[8];
  char min_max_time_string[12];
} ForecastLayer;


void forecast_layer_init(ForecastLayer* forecast_layer, GPoint pos, bool time_bool);
void forecast_layer_deinit(ForecastLayer* forecast_layer);
void forecast_layer_set_icon(ForecastLayer* forecast_layer, WeatherIcon icon);
void forecast_layer_update(ForecastLayer* forecast_layer,char data_packs[NUM_TERMS][TERM_LEN],
  int time_key, int icon_key, int min_temp_key, int max_temp_key, int min_time_key, int max_time_key);

#endif
