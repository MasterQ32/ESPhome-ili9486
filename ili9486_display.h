#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"
#include "ili9486_defines.h"
#include "ili9486_init.h"

namespace esphome {
namespace ili9486 {

enum ILI9486Model { TFT_35 };

enum ILI9486ColorMode {
  BITS_16,
};

class ILI9486Display : public PollingComponent,
                       public display::DisplayBuffer,
                       public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                             spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_20MHZ> {
 public:
  void set_dc_pin(GPIOPin *dc_pin) { dc_pin_ = dc_pin; }
  float get_setup_priority() const override;
  void set_reset_pin(GPIOPin *reset) { this->reset_pin_ = reset; }
  void set_led_pin(GPIOPin *led) { this->led_pin_ = led; }
  void set_model(ILI9486Model model) { this->model_ = model; }

  void set_buffer_color_mode(ILI9486ColorMode color_mode) { this->buffer_color_mode_ = color_mode; }

  void command(uint8_t value);
  void data(uint8_t value);
  void send_command(uint8_t command_byte, const uint8_t *data_bytes, uint8_t num_data_bytes);
  uint8_t read_command(uint8_t command_byte, uint8_t index);
  virtual void initialize() = 0;

  void update() override;

  void fill(Color color) override;

  void dump_config() override;
  void setup() override {
    this->setup_pins_();
    this->initialize();
    this->setup_memory_();
  }

  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  void setup_pins_();

  void init_lcd_(const uint8_t *init_cmd);
  void set_addr_window_(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  void invert_display_(bool invert);
  void reset_();
  void fill_internal_(Color color);
  void display_();
  uint16_t color565_from_color332(uint8_t color_8bit);
  uint8_t color332_from_color565(uint16_t color_16bit);
  uint8_t color332_from_color(Color color);
  uint16_t color565_from_color(Color color);

  ILI9486Model model_;
  int16_t width_{320};   ///< Display width as modified by current rotation
  int16_t height_{480};  ///< Display height as modified by current rotation
  uint16_t x_low_{0};
  uint16_t y_low_{0};
  uint16_t x_high_{0};
  uint16_t y_high_{0};

  ILI9486ColorMode buffer_color_mode_{BITS_16};

  uint32_t get_buffer_length_();
  int get_width_internal() override;
  int get_height_internal() override;

  void start_command_();
  void end_command_();
  void start_data_();
  void end_data_();

  GPIOPin *reset_pin_{nullptr};
  GPIOPin *led_pin_{nullptr};
  GPIOPin *dc_pin_;
  GPIOPin *busy_pin_{nullptr};
private:
  inline uint16_t * buffer16() const {
    return reinterpret_cast<uint16_t *>(this->buffer_);
  }
  inline uint8_t * buffer8() const {
    return reinterpret_cast<uint8_t *>(this->buffer_);
  }

  void setup_memory_();
};

//-----------   ILI9486_24_TFT display --------------
class ILI9486TFT35 : public ILI9486Display {
 public:
  void initialize() override;
};
}  // namespace ili9486
}  // namespace esphome
